#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <qhextextedit.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    QSerialPort serialPort;

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QHexTextEdit *transmitt_text_edit = nullptr;
    QHexTextEdit *receive_text_edit = nullptr;
    QStringList getPortNames();
    QAction *mb_action = nullptr;

private slots:
    void onBaudrateChanged();
    void onConnectBtnClicked();
    void onDisconnectBtnClicked();
    void onRescanBtnClicked();
    void onSendBtnClicked();
    void onRxClearBtnClicked();
    void onTxClearBtnClicked();

    void serialPortHandleReadyRead();
//    void serialPortHandleTimeout();
    void serialPortHandleError(QSerialPort::SerialPortError error);
    void on_actionExit_triggered();
    void actionAbout_triggered();
};

#endif // MAINWINDOW_H
