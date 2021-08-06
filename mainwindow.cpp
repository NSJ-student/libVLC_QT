#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->btnPlay->setIcon(this->style()->standardIcon(QStyle::SP_MediaPlay));
    ui->btnPause->setIcon(this->style()->standardIcon(QStyle::SP_MediaPause));
    ui->btnStop->setIcon(this->style()->standardIcon(QStyle::SP_MediaStop));

    ui->btnVideoOpen->setIcon(this->style()->standardIcon(QStyle::SP_DialogOpenButton));

    videoWidget = new vlc_player(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_btnPlay_clicked()
{
    if(!ui->txtVideoPath->text().isEmpty())
    {
        videoWidget->play(ui->txtVideoPath->text());
    }
}

void MainWindow::on_btnPause_clicked()
{
    videoWidget->pause();
}

void MainWindow::on_btnStop_clicked()
{
    videoWidget->stop();
}

void MainWindow::on_btnVideoOpen_clicked()
{
    QString path =
             QFileDialog::getOpenFileName(
                this, ("Select Video"),
                QString(), QString("Video (*.mp4 *.avi *.wmv);; All Files (*)"));

    if(!path.isNull() && !path.isEmpty())
    {
        ui->txtVideoPath->setText(path);
    }
}

void MainWindow::showEvent(QShowEvent *event)
{
    videoWidget->init_vlc();
    ui->verticalLayout->insertWidget(0, videoWidget);
}

