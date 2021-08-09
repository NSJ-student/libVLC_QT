#ifndef VLC_PLAYER_H
#define VLC_PLAYER_H

#include <QFrame>
#include <QWindow>
#include <QVideoWidget>
#include <QDebug>
#include <QThread>
#include <QMetaType>

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(Q_OS_LINUX)
typedef int ssize_t;
#endif
#include <vlc/vlc.h>

#ifdef __cplusplus
};
#endif

Q_DECLARE_METATYPE(libvlc_state_t);

class VideoWorker : public QObject
{
    Q_OBJECT

public:
    VideoWorker(QObject *parent = Q_NULLPTR) :
        QObject(parent), vlc_mp(Q_NULLPTR), vlc_m(Q_NULLPTR)
    {

    }
    void setParams(libvlc_media_player_t * mp, libvlc_media_t * m)
    {
        vlc_mp = mp;
        vlc_m  = m;
    }
    bool cancel;

signals:
    void finished();
    void setTotalTime(long total_ms);
    void setCurrentTime(long current_ms);
    void setVideoState(libvlc_state_t state);

public slots:
    void body()
    {
        qDebug() << __func__ << QThread::currentThread();
        if(!vlc_m)
        {
            emit finished();
            return;
        }
        if(!vlc_mp)
        {
            emit finished();
            return;
        }

        cancel = false;
        libvlc_time_t total_ms_pre = libvlc_media_player_get_length(vlc_mp);
        libvlc_time_t current_ms_pre = libvlc_media_player_get_time(vlc_mp);
        libvlc_state_t state_pre  = libvlc_media_get_state(vlc_m);
        emit setTotalTime(total_ms_pre);
        emit setCurrentTime(current_ms_pre);
        emit setVideoState(state_pre);

        while(1)
        {
            if(cancel)
            {
                return;
            }

            libvlc_state_t state = libvlc_media_get_state(vlc_m);
            if ((state == libvlc_Stopped) ||
                (state == libvlc_Error) ||
                (state == libvlc_Ended) ||
                (state == libvlc_NothingSpecial))
            {
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

            QThread::msleep(100);
        }

        emit finished();
    }

private:
    libvlc_media_player_t * vlc_mp;
    libvlc_media_t * vlc_m;
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

    libvlc_state_t state();
    void set_time(long current_ms);

signals:
    void updateTotalTime(long total_ms);
    void updateCurrentTime(long current_ms);
    void updateVideoState(libvlc_state_t state);
    void videoEnd();

public slots:
    void videoThreadDone();
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
