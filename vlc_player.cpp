#include "vlc_player.h"
#include "ui_vlc_player.h"

#define LIB_DLLEXTERN __declspec(dllimport)

vlc_player::vlc_player(QWidget *parent) :
    QVideoWidget(parent),
    ui(new Ui::vlc_player),
    vlc_inst(Q_NULLPTR), vlc_mp(Q_NULLPTR), vlc_m(Q_NULLPTR)
{
    this->setStyleSheet("background-color: rgb(0, 0, 0);");
}

vlc_player::~vlc_player()
{
    delete ui;
}

bool vlc_player::play(const QString &path)
{
    if(!vlc_obj_check())
    {
        return false;
    }

    libvlc_media_player_stop(vlc_mp);

    vlc_m = libvlc_media_new_path(vlc_inst, path.toUtf8().data());
    if(vlc_m == Q_NULLPTR)
    {
        return false;
    }

    libvlc_media_player_set_media(vlc_mp, vlc_m);
    libvlc_media_release(vlc_m);

    libvlc_media_player_play(vlc_mp);

    current_path = path;
    return true;
}

bool vlc_player::pause()
{
    if(!vlc_obj_check())
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
    return true;
}

libvlc_state_t vlc_player::state()
{
    if(!vlc_m)
    {
        return libvlc_Error;
    }

    return libvlc_media_get_state(vlc_m);;
}

bool vlc_player::init_vlc()
{
    const char * const vlc_args[] = {
          "--verbose=2", // Be much more verbose then normal for debugging purpose
          "--intf=dummy"
    };
    vlc_inst = libvlc_new(sizeof(vlc_args) / sizeof(vlc_args[0]), vlc_args);
//    vlc_inst = libvlc_new(0, NULL);
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

    return true;
}
