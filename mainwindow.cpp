#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), m_videolist(Q_NULLPTR)
{
    ui->setupUi(this);

    ui->btnPlay->setIcon(this->style()->standardIcon(QStyle::SP_MediaPlay));
    ui->btnPause->setIcon(this->style()->standardIcon(QStyle::SP_MediaPause));
    ui->btnStop->setIcon(this->style()->standardIcon(QStyle::SP_MediaStop));

    m_videolist = new VideoList(this);
    this->addDockWidget(Qt::RightDockWidgetArea, m_videolist);
    connect(m_videolist, SIGNAL(playVideo(QString)), this, SLOT(playVideo(QString)));
    connect(m_videolist, SIGNAL(visibilityChanged(bool)), this, SLOT(listVisibilityChanged(bool)));
    ui->btnVideoList->setChecked(true);

    connect(ui->videoWidget, SIGNAL(updateTotalTime(long)), this, SLOT(setTotalTime(long)));
    connect(ui->videoWidget, SIGNAL(updateCurrentTime(long)), this, SLOT(setCurrentTime(long)));
    qRegisterMetaType<libvlc_state_t>("libvlc_state_t");
    connect(ui->videoWidget, SIGNAL(updateVideoState(libvlc_state_t)), this, SLOT(setVideoState(libvlc_state_t)));
    connect(ui->videoWidget, SIGNAL(videoEnd()), this, SLOT(videoEnd()), Qt::QueuedConnection);
    connect(ui->videoWidget, SIGNAL(videoEndError()), this, SLOT(videoEndError()), Qt::QueuedConnection);

    QAction * actionMoveNext = new QAction(this);
    actionMoveNext->setShortcut(Qt::Key_Right);
    connect(actionMoveNext, SIGNAL(triggered()), this, SLOT(onActionactionMoveForward_triggered()));
    this->addAction(actionMoveNext);

    QAction * actionMovePrev = new QAction(this);
    actionMovePrev->setShortcut(Qt::Key_Left);
    connect(actionMovePrev, SIGNAL(triggered()), this, SLOT(onActionactionMoveBackward_triggered()));
    this->addAction(actionMovePrev);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::playVideo(QString path)
{
    ui->txtVideoPath->setText(path);
    ui->videoWidget->play(path);
}

void MainWindow::setTotalTime(long total_ms)
{
    ui->progressVideo->setMaximum(total_ms);

    int hour = (total_ms/1000/60/60)%24;
    int minute = (total_ms/1000/60)%60;
    int second = (total_ms/1000)%60;
    QTime time(hour, minute, second);
    ui->lblTotalTime->setText(time.toString("hh:mm:ss"));
}

void MainWindow::setCurrentTime(long current_ms)
{
    ui->progressVideo->setValue(current_ms);

    int hour = (current_ms/1000/60/60)%24;
    int minute = (current_ms/1000/60)%60;
    int second = (current_ms/1000)%60;
    QTime time(hour, minute, second);
    ui->lblCurrentTime->setText(time.toString("hh:mm:ss"));
}

void MainWindow::setVideoState(libvlc_state_t state)
{
    switch(state)
    {
    case libvlc_Playing: {
        ui->lblState->setText("Playing");
        ui->btnPause->setText("Pause");
        return;
    }
    case libvlc_Paused: {
        ui->lblState->setText("Paused");
        ui->btnPause->setText("Resume");
        return;
    }
    case libvlc_Stopped: ui->lblState->setText("Stopped"); break;
    case libvlc_Ended: ui->lblState->setText("Ended"); break;
    case libvlc_Error: ui->lblState->setText("Error"); break;
    case libvlc_Buffering: ui->lblState->setText("Buffering"); break;
    case libvlc_Opening: ui->lblState->setText("Opening"); break;
    case libvlc_NothingSpecial: ui->lblState->setText("None"); break;
    }

    ui->btnPause->setText("Pause");
}

void MainWindow::videoEnd()
{
    ui->progressVideo->setValue(0);
    ui->lblState->setText("Ended");
    m_videolist->playNext();
}

void MainWindow::videoEndError()
{
    ui->progressVideo->setValue(0);
    ui->lblState->setText("Ended");
}

void MainWindow::on_btnPlay_clicked()
{
    if(!ui->txtVideoPath->text().isEmpty())
    {
        ui->videoWidget->play(ui->txtVideoPath->text());
    }
    else
    {
        m_videolist->playStart();
    }
}

void MainWindow::on_btnPause_clicked()
{
    libvlc_state_t state = ui->videoWidget->state();
    if(state == libvlc_Playing )
    {
        ui->btnPause->setText("Resume");
    }
    else if(state == libvlc_Paused)
    {
        ui->btnPause->setText("Pause");
    }
    ui->videoWidget->pause();
}

void MainWindow::on_btnStop_clicked()
{
    ui->videoWidget->stop();
    m_videolist->playStop();
    ui->btnPause->setText("Pause");
}

void MainWindow::showEvent(QShowEvent *event)
{
    ui->videoWidget->init_vlc();
}

void MainWindow::on_progressVideo_sliderMoved(int position)
{
    if((ui->videoWidget->state() == libvlc_Playing) ||
            (ui->videoWidget->state() == libvlc_Paused))
    {
        ui->videoWidget->set_time(position);
    }
}

void MainWindow::listVisibilityChanged(bool visible)
{
    ui->btnVideoList->setChecked(visible);
}

void MainWindow::on_btnVideoList_clicked()
{
    if(m_videolist->isVisible())
    {
        ui->btnVideoList->setChecked(false);
        m_videolist->hide();
    }
    else
    {
        ui->btnVideoList->setChecked(true);
        m_videolist->show();
    }
}

void MainWindow::on_actionAlways_on_Top_triggered(bool checked)
{
    Qt::WindowFlags flags = this->windowFlags();
    if (checked)
    {
        this->setWindowFlags(flags | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
        this->show();
    }
    else
    {
        this->setWindowFlags(flags ^ (Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint));
        this->show();
    }
}

void MainWindow::on_actionFullscreen_triggered(bool checked)
{
    if (checked)
    {
#if defined(Q_OS_LINUX)
//        this->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::WindowType_Mask);
#endif
        this->showFullScreen();
    }
    else
    {
        this->showNormal();
    }
}


void MainWindow::on_actionClose_triggered()
{
    this->close();
}


void MainWindow::onActionactionMoveForward_triggered()
{
    ui->videoWidget->move_next();
}

void MainWindow::onActionactionMoveBackward_triggered()
{
    ui->videoWidget->move_prev();
}

void MainWindow::on_btnSetAspectRatio_clicked()
{
    QString ratio = ui->lineVlcAspectRatio->text();
    ui->videoWidget->set_aspect_ratio(ratio.toUtf8().data(), ui->dsbVlcScale->value());
}

