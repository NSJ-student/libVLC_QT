#include "videolist.h"
#include "ui_videolist.h"

VideoList::VideoList(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::VideoList), playingItem(Q_NULLPTR)
{
    ui->setupUi(this);
}

VideoList::~VideoList()
{
    delete ui;
}

void VideoList::playStart()
{
    if(ui->listVideo->count() > 0)
    {
        playingItem = ui->listVideo->item(0);

        emit playVideo(playingItem->text());
    }
}

void VideoList::playNext()
{
    if(playingItem)
    {
        if(ui->listVideo->row(playingItem)+1 < ui->listVideo->count())
        {
            playingItem = ui->listVideo->item(ui->listVideo->row(playingItem)+1);
        }
        else if(ui->listVideo->count() > 0)
        {
            playingItem = ui->listVideo->item(0);
        }
        else
        {
            playingItem = Q_NULLPTR;
        }

        emit playVideo(playingItem->text());
    }
}

void VideoList::playStop()
{
    playingItem = Q_NULLPTR;
}

void VideoList::on_listVideo_itemDoubleClicked(QListWidgetItem *item)
{
    emit playVideo(item->text());
    playingItem = item;
}


void VideoList::on_btnAddFile_clicked()
{
    QStringList paths =
             QFileDialog::getOpenFileNames(
                this, ("Select Video"),
                QString(), QString("Video (*.mp4 *.avi *.wmv);; All Files (*)"));

    if(paths.count() > 0)
    {
        foreach(QString name, paths)
        {
            QListWidgetItem * item = new QListWidgetItem(name);
            ui->listVideo->addItem(item);
        }
    }
}


void VideoList::on_btnAddDir_clicked()
{
    QString path =
             QFileDialog::getExistingDirectory(this, ("Select Video File"));

    if(!path.isNull() && !path.isEmpty())
    {
        QStringList filter;
        filter << "*.mp4" << "*.wmv" << "*.avi";

        QStringList file_list = QDir(path).entryList(filter, QDir::Files, QDir::Name);
        foreach(QString name, file_list)
        {
            QListWidgetItem * item = new QListWidgetItem(path + "/" + name);
            ui->listVideo->addItem(item);
        }
    }
}


void VideoList::on_btnDelete_clicked()
{
    foreach(QListWidgetItem * item, ui->listVideo->selectedItems())
    {
        if(playingItem == item)
        {
            if(ui->listVideo->row(item)+1 < ui->listVideo->count())
            {
                playingItem = ui->listVideo->item(ui->listVideo->row(item)+1);
            }
            else if(ui->listVideo->count() > 0)
            {
                playingItem = ui->listVideo->item(0);
            }
            else
            {
                playingItem = Q_NULLPTR;
            }
        }
        delete ui->listVideo->takeItem(ui->listVideo->row(item));
    }
}


void VideoList::on_btnDeleteAll_clicked()
{
    ui->listVideo->clear();
    playingItem = Q_NULLPTR;
}

