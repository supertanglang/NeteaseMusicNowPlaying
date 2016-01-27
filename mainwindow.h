#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QProcess>
#include <QMessageBox>
#include <QDebug>
#include <QByteArray>
#include <QFileSystemWatcher>
#include <QJsonDocument>
#include <QTextStream>
#include <QTextCodec>
#include <QRegExp>
#include <QTest>

struct songDetail{
    QString songName;
    QString songID;
    QString albumName;
    QString artistName;
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private slots:
    void on_pushButton_clicked();

public slots:
    void fileChanged(QString);

private:
    Ui::MainWindow *ui;
    QFileSystemWatcher fsWatcher;
    bool checkRunning();
    void addWatcher();
    songDetail solveJson(QString);
    void checkLog();
    void writeOutput(songDetail);
};



#endif // MAINWINDOW_H
