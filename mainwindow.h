#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStyle>
#include <QFile>
#include <QFileDialog>
#include <QTime>

#include "vlc_player.h"
#include "videolist.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = Q_NULLPTR);
    ~MainWindow();

public slots:
    void playVideo(QString path);
    void setTotalTime(long total_ms);
    void setCurrentTime(long current_ms);
    void setVideoState(libvlc_state_t state);
    void videoEnd();
    void videoEndError();
    void listVisibilityChanged(bool visible);

    void onActionactionMoveForward_triggered();
    void onActionactionMoveBackward_triggered();

private slots:
    void on_btnPlay_clicked();

    void on_btnPause_clicked();

    void on_btnStop_clicked();

    void on_progressVideo_sliderMoved(int position);

    void on_btnVideoList_clicked();

    void on_actionAlways_on_Top_triggered(bool checked);

    void on_actionFullscreen_triggered(bool checked);

    void on_actionClose_triggered();

protected:
    void showEvent(QShowEvent * event);

private:
    Ui::MainWindow *ui;
    VideoList * m_videolist;
};
#endif // MAINWINDOW_H
