#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtNetwork>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QClipboard>
#include <fstream>
#include <iostream>


class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
public slots:
    void getClipboardData();
    void prepareDownload();
    void downloadFile(QNetworkReply*);
    void saveDownload(QNetworkReply*);
    void downloadProgress(qint64, qint64);
    void finishAndMerge();
private:
    QNetworkAccessManager manager;
    QNetworkRequest *request=nullptr;
    QNetworkReply *reply=nullptr;
    QUrl url;
    QFile file;
    QClipboard *clipboard;
    QVBoxLayout vboxlayout;
    QHBoxLayout hboxlayout;
    QLabel label;
    QPushButton downloadButton;
    QPushButton mergeButton;
    QProgressBar progressbar;

};
#endif // WIDGET_H
