#ifndef VLC_PLAYER_H
#define VLC_PLAYER_H

#include <QFrame>
#include <QWindow>
#include <QVideoWidget>
#include <QDebug>
#include <QThread>
#include <QMetaType>

#if !defined(Q_OS_LINUX)
typedef int ssize_t;
#endif
#include <vlc/vlc.h>
#include <vlc/libvlc_version.h>

Q_DECLARE_METATYPE(libvlc_state_t);

class VideoWorker : public QObject
{
    Q_OBJECT

public:
    VideoWorker(QObject *parent = Q_NULLPTR) :
        QObject(parent), vlc_mp(Q_NULLPTR)
    {

    }
    void setParams(libvlc_media_player_t * mp)
    {
        vlc_mp = mp;
    }
    bool cancel;

signals:
    void finished();
    void finished_error();
    void setTotalTime(long total_ms);
    void setCurrentTime(long current_ms);
    void setVideoState(libvlc_state_t state);

public slots:
    void body()
    {
//        qDebug() << __func__ << QThread::currentThread();
        if(!vlc_mp)
        {
            emit finished_error();
            return;
        }

        cancel = false;
        libvlc_time_t total_ms_pre = libvlc_media_player_get_length(vlc_mp);
        libvlc_time_t current_ms_pre = libvlc_media_player_get_time(vlc_mp);
        libvlc_state_t state_pre = libvlc_media_player_get_state(vlc_mp);
        emit setTotalTime(total_ms_pre);
        emit setCurrentTime(current_ms_pre);
        emit setVideoState(state_pre);

        // wait for play (500ms)
        for(int error_count=0; error_count<10; error_count++)
        {
            if(cancel)
            {
                emit finished_error();
                return;
            }
            if(libvlc_media_player_is_playing(vlc_mp))
            {
                break;
            }
            QThread::msleep(50);
        }

        while(1)
        {
            if(cancel)
            {
                emit finished_error();
                return;
            }

            libvlc_state_t state = libvlc_media_player_get_state(vlc_mp);

            if ((state == libvlc_Stopped) ||
                (state == libvlc_Error)  ||
                (state == libvlc_Ended))
            {
                qDebug() << "state: " << state
                         << "state_pre: " << state_pre;
                break;
            }

            long total_ms = libvlc_media_player_get_length(vlc_mp);
            long current_ms = libvlc_media_player_get_time(vlc_mp);

            if (total_ms != total_ms_pre)
            {
                emit setTotalTime(total_ms);
                total_ms_pre  = total_ms;
            }
            if (current_ms != current_ms_pre)
            {
                emit setCurrentTime(current_ms);
                current_ms_pre  = current_ms;
            }
            if (state != state_pre)
            {
                emit setVideoState(state);
                state_pre  = state;
            }

            QThread::msleep(60);
        }

        if(!cancel)
        {
            emit finished();
        }
        else
        {
            emit finished_error();
        }
    }

private:
    libvlc_media_player_t * vlc_mp;
};

class vlc_player : public QVideoWidget
{
    Q_OBJECT

public:
    explicit vlc_player(QWidget *parent = Q_NULLPTR);
    ~vlc_player();

    /**************/
    //   VLC
    /**************/
    bool init_vlc();

    bool play(const QString &path);
    bool pause();
    bool stop();

    void move_next();
    void move_prev();

    libvlc_state_t state();
    void set_time(long current_ms);
    void set_aspect_ratio(char * ratio, float factor);

signals:
    void updateTotalTime(long total_ms);
    void updateCurrentTime(long current_ms);
    void updateVideoState(libvlc_state_t state);
    void videoEnd();
    void videoEndError();

public slots:
    void videoThreadDone();
    void videoThreadError();
    void setTotalTime(long total_ms);
    void setCurrentTime(long current_ms);
    void setVideoState(libvlc_state_t state);

private:
    /**************/
    //   VLC
    /**************/
    bool vlc_init;
    libvlc_instance_t * vlc_inst;
    libvlc_media_player_t * vlc_mp;
    libvlc_media_t * vlc_m;
    QString current_path;

    VideoWorker m_video_worker;
    QThread m_video_thread;


    void video_thread_cancel();
    bool vlc_obj_check()
    {
        if(vlc_inst == Q_NULLPTR)
        {
            return false;
        }
        if(vlc_mp == Q_NULLPTR)
        {
            return false;
        }

        return true;
    }
};

#endif // VLC_PLAYER_H
