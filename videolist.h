#ifndef VIDEOLIST_H
#define VIDEOLIST_H

#include <QDockWidget>
#include <QFrame>
#include <QWindow>
#include <QDebug>
#include <QListWidgetItem>
#include <QFile>
#include <QFileDialog>

namespace Ui {
class VideoList;
}

class VideoList : public QDockWidget
{
    Q_OBJECT

public:
    explicit VideoList(QWidget *parent = nullptr);
    ~VideoList();

    void playStart();
    void playNext();
    void playStop();

signals:
    void playVideo(QString path);

private slots:
    void on_listVideo_itemDoubleClicked(QListWidgetItem *item);

    void on_btnAddFile_clicked();

    void on_btnAddDir_clicked();

    void on_btnDelete_clicked();

    void on_btnDeleteAll_clicked();

private:
    Ui::VideoList *ui;
    QListWidgetItem * playingItem;
};

#endif // VIDEOLIST_H
