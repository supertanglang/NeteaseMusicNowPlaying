#include "mainwindow.h"
#include "ui_mainwindow.h"
QString neteaseMusicLogFile = QDir::homePath()+"/Library/Containers/com.netease.163music/Data/Documents/storage/Logs/music.163.log";
QString outputTXTPath = QDir::homePath()+"/163MusicNowPlaying.txt";
QString changeCountStr = "0";
QString currentLog;
QString currentRegexParse;
QFile neteaseMusicLogFileInfo(neteaseMusicLogFile);
QFile outputTXT(outputTXTPath);
QTextCodec *codec = QTextCodec::codecForName("UTF-8");
QTextStream in(&neteaseMusicLogFileInfo);
QTextStream out(&outputTXT);
QRegExp logRegex("\\[info\\]player._\\$load, , .*");
int changeCount = 0;
songDetail current;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow){
    ui->setupUi(this);

    //add fileSystemWatcher
    addWatcher();

    QTextCodec::setCodecForLocale(codec);

    if(!neteaseMusicLogFileInfo.open(QIODevice::ReadOnly)){
        //Netease Music log file open failed
        QMessageBox::critical(this,"错误","找不到网易云音乐日志");
        exit(0);
    }

    if(!outputTXT.open(QIODevice::ReadWrite)){
        //Now Playing txt create failed
        QMessageBox::critical(this,"错误","创建输出目标失败"+outputTXTPath);
        exit(0);
    }

    setWindowFlags(windowFlags()&Qt::WindowCloseButtonHint& ~Qt::WindowMaximizeButtonHint& ~Qt::WindowMinimizeButtonHint);
    setFixedSize(this->width(), this->height());
}

void MainWindow::addWatcher(){
    fsWatcher.addPath(neteaseMusicLogFile);
    connect(&fsWatcher, SIGNAL(fileChanged(QString)), this, SLOT(fileChanged(QString)));
}

void MainWindow::checkLog(){
    QTest::qWait(250);  //wait for 250ms in case log still writing
    int pos = -1;
    while(!in.atEnd()){
        //read the log till EOF
        currentLog = in.readLine();
        pos = currentLog.indexOf(logRegex);
        if ( pos >= 0 ){
            currentRegexParse = logRegex.cap(0).remove(0,23);
            qDebug()<<currentRegexParse;
            current = solveJson(currentRegexParse);
            ui->label_2->setText(current.albumName);
            ui->label_3->setText(current.artistName);
            ui->label_4->setText(current.songID);
            ui->label_5->setText(current.songName);
            writeOutput(current);
        }
    }
    qDebug()<<"QTextStream in at the end of log";
}

void MainWindow::writeOutput(songDetail currSong){
    outputTXT.resize(0);//Clear file content
    out<<currSong.artistName<<" - "<<currSong.songName;
    out.flush();
}

void MainWindow::fileChanged(QString){
    changeCount++;
    qDebug()<<changeCount;
    changeCountStr = QString::number(changeCount);
    ui->label->setText(changeCountStr);
    checkLog();
}


MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::on_pushButton_clicked(){
    if(checkRunning()){
        ui->pushButton->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")+"  正在运行");
        checkLog();
    }else{
        ui->pushButton->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")+"  未发现进程");
    }
}

bool MainWindow::checkRunning(){
    //Call ps to check if Netease Music is running
    QProcess ps;
    QString str;
    QStringList args;
    args<<"-c";
    args<<"ps -A | grep NeteaseMusic | grep -v grep";
    ps.start("sh",args);
    ps.waitForFinished();
    QByteArray bArr;
    bArr=ps.readAllStandardOutput();
    str=bArr;
    qDebug()<<str;
    if(str.contains("NeteaseMusic")){
        return true;
    }else{
        return false;
    }
}

songDetail MainWindow::solveJson(QString jsonStr){
    QByteArray bArr;
    songDetail current;
    bArr=jsonStr.toUtf8();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(bArr);
    QVariantMap result = jsonDoc.toVariant().toMap();
    qDebug()<<result["albumName"].toString();
    qDebug()<<result["artistName"].toString();
    qDebug()<<result["songId"].toString();
    qDebug()<<result["songName"].toString();
    current.albumName  = result["albumName"].toString();
    current.artistName = result["artistName"].toString();
    current.songID     = result["songId"].toString();
    current.songName   = result["songName"].toString();
    return current;
}
