#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStyle>
#include <QFile>
#include <QFileDialog>

#include "vlc_player.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = Q_NULLPTR);
    ~MainWindow();

private slots:
    void on_btnPlay_clicked();

    void on_btnPause_clicked();

    void on_btnStop_clicked();

    void on_btnVideoOpen_clicked();

protected:
    void showEvent(QShowEvent * event);

private:
    Ui::MainWindow *ui;

    vlc_player * videoWidget;
};
#endif // MAINWINDOW_H
