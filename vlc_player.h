#ifndef VLC_PLAYER_H
#define VLC_PLAYER_H

#include <QFrame>
#include <QWindow>
#include <QVideoWidget>

#ifdef __cplusplus
extern "C" {
#endif

typedef int ssize_t;
#include <vlc/vlc.h>

#ifdef __cplusplus
};
#endif


namespace Ui {
class vlc_player;
}

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

signals:
    void position_changed(long current, long total);


private:
    Ui::vlc_player *ui;

    /**************/
    //   VLC
    /**************/
    libvlc_instance_t * vlc_inst;
    libvlc_media_player_t * vlc_mp;
    libvlc_media_t * vlc_m;
    QString current_path;

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
