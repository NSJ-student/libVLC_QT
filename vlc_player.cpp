#include "vlc_player.h"

#define LIB_DLLEXTERN __declspec(dllimport)

vlc_player::vlc_player(QWidget *parent) :
    QVideoWidget(parent),
    vlc_init(false), vlc_inst(Q_NULLPTR), vlc_mp(Q_NULLPTR), vlc_m(Q_NULLPTR)
{
    this->setStyleSheet("background-color: rgb(0, 0, 0);");

    m_video_worker.moveToThread(&m_video_thread);
    connect(&m_video_thread, SIGNAL(started()), &m_video_worker, SLOT(body()));
    connect(&m_video_worker, SIGNAL(setTotalTime(long)), this, SLOT(setTotalTime(long)));
    connect(&m_video_worker, SIGNAL(setCurrentTime(long)), this, SLOT(setCurrentTime(long)));
    qRegisterMetaType<libvlc_state_t>("libvlc_state_t");
    connect(&m_video_worker, SIGNAL(setVideoState(libvlc_state_t)), this, SLOT(setVideoState(libvlc_state_t)));
    connect(&m_video_worker, SIGNAL(finished()), this, SLOT(videoThreadDone()));
}

vlc_player::~vlc_player()
{
    stop();
}

bool vlc_player::play(const QString &path)
{
    if(!vlc_obj_check())
    {
        return false;
    }

    if(vlc_m)
    {
        libvlc_state_t state = libvlc_media_get_state(vlc_m);
        if(state == libvlc_Paused)
        {
            libvlc_media_player_play(vlc_mp);
            return true;
        }
    }

    video_thread_cancel();
    libvlc_media_player_stop(vlc_mp);

    QString new_path = QString("file:///") + path;
    vlc_m = libvlc_media_new_location(vlc_inst, new_path.toUtf8().data());
    if(vlc_m == Q_NULLPTR)
    {
        qDebug() << "vlc_m == NULL";
        return false;
    }

    libvlc_media_player_set_media(vlc_mp, vlc_m);
    libvlc_media_release(vlc_m);

    libvlc_media_player_play(vlc_mp);

    current_path = path;
    m_video_worker.setParams(vlc_mp, vlc_m);
    m_video_thread.start();
    return true;
}

bool vlc_player::pause()
{
    if(!vlc_obj_check())
    {
        return false;
    }
    if(!vlc_m)
    {
        return false;
    }

    libvlc_state_t state = libvlc_media_get_state(vlc_m);
    if(state == libvlc_Playing )
    {
        libvlc_media_player_pause(vlc_mp);
        return true;
    }
    else if(state == libvlc_Paused)
    {
        libvlc_media_player_play(vlc_mp);
        return true;
    }
    else
    {
        return false;
    }
}

bool vlc_player::stop()
{
    if(!vlc_obj_check())
    {
        return false;
    }

    libvlc_media_player_stop(vlc_mp);
    m_video_thread.wait(1000);
    if(m_video_thread.isRunning())
    {
        m_video_thread.quit();
        m_video_thread.wait(1000);
        if(m_video_thread.isRunning())
        {
            qDebug() << "terminate";
            m_video_thread.terminate();
        }
    }

    return true;
}

libvlc_state_t vlc_player::state()
{
    if(!vlc_m)
    {
        return libvlc_Error;
    }

    return libvlc_media_get_state(vlc_m);
}

void vlc_player::set_time(long current_ms)
{
    if(!vlc_obj_check())
    {
        return;
    }

    libvlc_media_player_set_time(vlc_mp, current_ms);
}

void vlc_player::setTotalTime(long total_ms)
{
    emit updateTotalTime(total_ms);
}

void vlc_player::setCurrentTime(long current_ms)
{
    emit updateCurrentTime(current_ms);
}

void vlc_player::setVideoState(libvlc_state_t state)
{
    emit updateVideoState(state);
}

void vlc_player::video_thread_cancel()
{
    if(m_video_thread.isRunning())
    {
        m_video_worker.cancel = true;
        m_video_thread.wait(1000);
        if(m_video_thread.isRunning())
        {
            m_video_thread.quit();
            m_video_thread.wait(1000);
            if(m_video_thread.isRunning())
            {
                qDebug() << "terminate";
                m_video_thread.terminate();
            }
        }
    }
}

void vlc_player::videoThreadDone()
{
    if(!m_video_worker.cancel)
    {
        emit videoEnd();
    }
}

bool vlc_player::init_vlc()
{
    if(vlc_init)
    {
        return false;
    }

    vlc_inst = libvlc_new(0, NULL);
    if(vlc_inst == Q_NULLPTR)
    {
        return false;
    }

    vlc_mp = libvlc_media_player_new(vlc_inst);
    if(vlc_mp == Q_NULLPTR)
    {
        return false;
    }

#if defined(Q_OS_LINUX)
    libvlc_media_player_set_xwindow(vlc_mp, this->winId());
#else
    HWND windowID = reinterpret_cast<HWND>(this->winId());
    libvlc_media_player_set_hwnd(vlc_mp, windowID);
#endif

    vlc_init = true;
    return true;
}

#include "moc_vlc_player.cpp"
