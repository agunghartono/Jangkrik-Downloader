#include "widget.h"
#include <QByteArray>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{


    setLayout(&vboxlayout);
    downloadButton.setText("Download");
    mergeButton.setText("Merge");
    progressbar.setStyleSheet("QProgressBar{border: 2px solid grey; border-radius:5px;text-align:center}"
                              "QProgressBar::chunk {background-color: #32cd32; width: 7 px; margin:0.5px;}");

    vboxlayout.addWidget(&label,0,Qt::AlignCenter);
    hboxlayout.addWidget(&downloadButton);
    hboxlayout.addWidget(&mergeButton);
    vboxlayout.addLayout(&hboxlayout);
    vboxlayout.addWidget(&progressbar);

    label.setText("<img src=://pics/jangkrik.png width=90 height=50></img>"
                  "<b>Welcome to Jangkrik Downloader!</b><br>Please hit the \"Merge button\" once download is finished to avoid app failure.");

    clipboard=qApp->clipboard();
    connect(clipboard,&QClipboard::dataChanged,this,&Widget::getClipboardData);
    connect(&downloadButton,SIGNAL(clicked()),SLOT(prepareDownload()));
    connect(&mergeButton,SIGNAL(clicked()),SLOT(finishAndMerge()));

}

void Widget::getClipboardData()
{
    label.setText(clipboard->text());
    adjustSize();
}

void Widget::prepareDownload()
{
    downloadButton.setEnabled(false);

    request=new QNetworkRequest;
    url.setUrl(clipboard->text());
    request->setUrl (url);
    manager.head(*request);

    file.setFileName(url.fileName());

    connect(&manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloadFile(QNetworkReply*)));
}

void Widget::downloadFile(QNetworkReply *replying)
{
    disconnect(&manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloadFile(QNetworkReply*)));

    if (replying->error())
    {
        label.setText("Error");
        replying->deleteLater();
        return;
    }

    int length=replying->header(QNetworkRequest::ContentLengthHeader).toInt();
    replying->deleteLater();

    int sep=6;
    QByteArray barray[6];
    int lengthpart=length/sep;
    int leng=lengthpart;
    int len=0;

    for (int i=0;i<sep;i++)
    {
        barray[i].append("bytes=");
        barray[i].append(QString::number(len));
        barray[i].append("-");
        leng=lengthpart*(i+1);
        if (leng!=lengthpart*sep)
        {
            barray[i].append(QString::number(leng));
        }
        len=leng+1;
    }

    connect(&manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(saveDownload(QNetworkReply*)));
    for(int i=1;i<=6;i++)
    {
        request->setAttribute(QNetworkRequest::User,i);
        request->setRawHeader("Range",barray[i-1]);
        reply=manager.get(*request);
        connect(reply, &QNetworkReply::downloadProgress, this, &Widget::downloadProgress);
     }
}

void Widget::saveDownload(QNetworkReply *replying)
{
    if (replying->error())
    {
        label.setText("Error");
        return;
    }

    QString stringName;
    stringName=url.fileName()+".part"+replying->request().attribute(QNetworkRequest::User).toString();
    file.setFileName(stringName);
    file.open(QIODevice::WriteOnly);
    file.write(replying->readAll());

    file.flush();
    file.close();

    replying->deleteLater();
}

void Widget::downloadProgress(qint64 accepted, qint64 total)
{
    label.setText(file.fileName()+" : "+QString::number(accepted)+" / "+QString::number(total));
    double progress=(accepted/static_cast<double>(total))*100;
    progressbar.setValue(static_cast<int>(progress));
}

void Widget::finishAndMerge()
{

    disconnect(&manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(saveDownload(QNetworkReply*)));

    QString filenamePart=url.fileName();

    std::string aaa=filenamePart.toStdString()+".part";

    std::string bbb=aaa+std::to_string(1);
    std::ifstream source;
    source.open(bbb,std::ios::binary);
    if (!source.is_open()){source.close(); return;}
    source.close();

    std::ofstream destination(filenamePart.toStdString(),std::ios::binary);

    for (int i=1;i<7;i++)
    {
        std::string bbb=aaa+std::to_string(i);
        std::cout<<bbb<<std::endl;
        source.open(bbb,std::ios::binary);
        destination<<source.rdbuf();
        source.close();
        std::remove (bbb.c_str());
    }
    destination.close();

    delete request;
    request=nullptr;
    downloadButton.setEnabled(true);
}

Widget::~Widget()
{
    if (request!=nullptr) delete request;
}
