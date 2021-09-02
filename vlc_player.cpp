#include "vlc_player.h"

#define LIB_DLLEXTERN __declspec(dllimport)

vlc_player::vlc_player(QWidget *parent) :
    QVideoWidget(parent),
    vlc_init(false), vlc_inst(Q_NULLPTR), vlc_mp(Q_NULLPTR), vlc_m(Q_NULLPTR)
{
    m_video_worker.moveToThread(&m_video_thread);
    connect(&m_video_thread, SIGNAL(started()), &m_video_worker, SLOT(body()));
    connect(&m_video_worker, SIGNAL(setTotalTime(long)), this, SLOT(setTotalTime(long)));
    connect(&m_video_worker, SIGNAL(setCurrentTime(long)), this, SLOT(setCurrentTime(long)));
    qRegisterMetaType<libvlc_state_t>("libvlc_state_t");
    connect(&m_video_worker, SIGNAL(setVideoState(libvlc_state_t)), this, SLOT(setVideoState(libvlc_state_t)));
    connect(&m_video_worker, SIGNAL(finished()), this, SLOT(videoThreadDone()));
    connect(&m_video_worker, SIGNAL(finished_error()), this, SLOT(videoThreadError()));
}

vlc_player::~vlc_player()
{
    video_thread_cancel();
    stop();

    /*
    if(vlc_m)
    {
        libvlc_media_release(vlc_m);
    }
    */
    if(vlc_mp)
    {
        libvlc_media_player_release(vlc_mp);
    }
    if(vlc_inst)
    {
        libvlc_release(vlc_inst);
    }
}

bool vlc_player::play(const QString &path)
{
    if(!vlc_obj_check())
    {
        return false;
    }

    libvlc_state_t state = libvlc_media_player_get_state(vlc_mp);
    if(state == libvlc_Paused)
    {
        libvlc_media_player_play(vlc_mp);
        return true;
    }

    m_video_worker.cancel = true;
    stop();

    QString new_path = QString("file:///") + path;
    vlc_m = libvlc_media_new_location(vlc_inst, new_path.toUtf8().data());
    if(vlc_m == Q_NULLPTR)
    {
        qDebug() << "vlc_m == NULL";
        return false;
    }

    libvlc_media_add_option(vlc_m, "--no-osd");
    libvlc_media_player_set_media(vlc_mp, vlc_m);
    libvlc_media_release(vlc_m);

    libvlc_media_player_play(vlc_mp);

    current_path = path;
    m_video_worker.setParams(vlc_mp);
    m_video_thread.start();
    return true;
}

bool vlc_player::pause()
{
    if(!vlc_obj_check())
    {
        return false;
    }

    libvlc_state_t state = libvlc_media_player_get_state(vlc_mp);
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

//    libvlc_media_release(vlc_m);
#if (LIBVLC_VERSION_MAJOR >= 4)
    libvlc_media_player_stop_async(vlc_mp);
#else
    libvlc_media_player_stop(vlc_mp);
#endif
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
    if(!vlc_mp)
    {
        return libvlc_Error;
    }

    return libvlc_media_player_get_state(vlc_mp);
}

void vlc_player::set_time(long current_ms)
{
    if(!vlc_obj_check())
    {
        return;
    }
    if(!libvlc_media_player_is_playing(vlc_mp))
    {
        return;
    }

#if (LIBVLC_VERSION_MAJOR >= 4)
    libvlc_media_player_set_time(vlc_mp, current_ms, false);
#else
    libvlc_media_player_set_time(vlc_mp, current_ms);
#endif
}

void vlc_player::set_aspect_ratio(char *ratio, float factor)
{
    if(!vlc_obj_check())
    {
        return;
    }
    if(!libvlc_media_player_is_playing(vlc_mp))
    {
        return;
    }

    libvlc_video_set_aspect_ratio(vlc_mp, ratio);
    if(factor < 0)
    {
        return;
    }
    libvlc_video_set_scale(vlc_mp, factor);
}

void vlc_player::move_next()
{
    if(!vlc_obj_check())
    {
        return;
    }
    if(!libvlc_media_player_is_playing(vlc_mp))
    {
        return;
    }

    libvlc_time_t current_ms = libvlc_media_player_get_time(vlc_mp);
    current_ms += 5000;

#if (LIBVLC_VERSION_MAJOR >= 4)
    libvlc_media_player_set_time(vlc_mp, current_ms, false);
#else
    libvlc_media_player_set_time(vlc_mp, current_ms);
#endif
}

void vlc_player::move_prev()
{
    if(!vlc_obj_check())
    {
        return;
    }
    if(!libvlc_media_player_is_playing(vlc_mp))
    {
        return;
    }

    libvlc_time_t current_ms = libvlc_media_player_get_time(vlc_mp);
    current_ms -= 5000;
    if(current_ms < 0)
    {
        current_ms = 0;
    }

#if (LIBVLC_VERSION_MAJOR >= 4)
    libvlc_media_player_set_time(vlc_mp, current_ms, false);
#else
    libvlc_media_player_set_time(vlc_mp, current_ms);
#endif
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
    else
    {
        emit videoEndError();
    }
}

void vlc_player::videoThreadError()
{
    emit videoEndError();
}

bool vlc_player::init_vlc()
{
    if(vlc_init)
    {
        return false;
    }

    const char * arguments[] {
        "--no-osd"
    };
    vlc_inst = libvlc_new(sizeof(arguments)/sizeof(arguments[0]), arguments);
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
