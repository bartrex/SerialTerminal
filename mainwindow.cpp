#include "mainwindow.h"
#include "aboutdialog.h"
#include "ui_mainwindow.h"
#include <QSerialPortInfo>
#include <QDesktopWidget>
#include <QApplication>
#include <QIntValidator>
#include <QRegExp>
#include <QMessageBox>
#include <QByteArray>

constexpr uint32_t baudrates[] {4800, 9600, 19200, 38400, 57600, 115200};
constexpr uint8_t stopbits[] {1, 2};
const QString pariies[] {"none", "even", "odd"};
constexpr uint8_t databits[] {5, 6, 7, 8};
const QString custormbrCaption {"Custom BR"};

constexpr uint8_t index_0 = 0;
constexpr uint8_t index_1 = 1;
constexpr uint8_t index_3 = 3;
constexpr uint8_t index_4 = 4;
constexpr uint8_t index_6 = 6;

const QString aboutCaption {"About"};
const QString errorCaption {"Error"};
const QString cantOpenCaption {"I can't open port %1!"};
const QString portDoesntExistCaption {"Port %1 doesn't exist!"};

const QString comboBoxName {"comboBox_"};
const QString radioButtonName {"radioButton_"};
const QString transmittEditName {"transmitt_edit"};
const QString receiveEditName {"receive_edit"};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QRect desktopRect = QApplication::desktop()->availableGeometry(this);
    QPoint center = desktopRect.center();
    move(center.x()-width()/2, center.y()-height()/2);

    transmitt_text_edit = new QHexTextEdit(Qt::GlobalColor::darkGreen, Qt::GlobalColor::blue);
    transmitt_text_edit->setReadOnly(true);
    transmitt_text_edit->setObjectName(transmittEditName);

    receive_text_edit = new QHexTextEdit(Qt::GlobalColor::darkGreen, Qt::GlobalColor::blue);
    receive_text_edit->setReadOnly(true);
    receive_text_edit->setObjectName(receiveEditName);

    ui->verticalLayout_3->addWidget(transmitt_text_edit);
    ui->verticalLayout_5->addWidget(receive_text_edit);

    QStringList portNamesList = getPortNames();
    for(QString serialName : portNamesList)
    {
        ui->comboBox_1->addItem(serialName);
    }

    for(uint32_t baudrate : baudrates)
    {
        QString baudrateStr = QString::number(baudrate);
        ui->comboBox_2->addItem(baudrateStr);
    }
    ui->comboBox_2->addItem(custormbrCaption);
    ui->comboBox_2->setCurrentIndex(index_1);

    for(uint8_t stopbit : stopbits)
    {
        QString stopbitStr = QString::number(stopbit);
        ui->comboBox_3->addItem(stopbitStr);
    }
    ui->comboBox_3->setCurrentIndex(index_0);

    for(QString parity : pariies)
    {
        ui->comboBox_4->addItem(parity);
    }
    ui->comboBox_4->setCurrentIndex(index_0);

    for(uint8_t dataWidth : databits)
    {
        QString dataWidthStr = QString::number(dataWidth);
        ui->comboBox_5->addItem(dataWidthStr);
    }
    ui->comboBox_5->setCurrentIndex(index_3);

    connect(ui->comboBox_2, SIGNAL(currentIndexChanged(int)), this, SLOT(onBaudrateChanged()));
    connect(ui->pushButton_1, SIGNAL(clicked()), this, SLOT(onConnectBtnClicked()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(onDisconnectBtnClicked()));
    connect(ui->pushButton_3, SIGNAL(clicked()), this, SLOT(onRescanBtnClicked()));
    connect(ui->pushButton_4, SIGNAL(clicked()), this, SLOT(onSendBtnClicked()));
    connect(ui->rxClearButton, SIGNAL(clicked()), this, SLOT(onRxClearBtnClicked()));
    connect(ui->txClearButton, SIGNAL(clicked()), this, SLOT(onTxClearBtnClicked()));

    connect(&serialPort, SIGNAL(readyRead()), this, SLOT(serialPortHandleReadyRead()));
    connect(&serialPort, &QSerialPort::errorOccurred, this, &MainWindow::serialPortHandleError);

    QRegExp regExp {"^[1-9][0-9]{0,5}"};
    ui->lineEdit_1->setValidator(new QRegExpValidator(regExp));

    if(ui->comboBox_1->count() > 0)
    {
        ui->pushButton_1->setEnabled(true);
    }

    mb_action = new QAction(ui->menuBar);
    mb_action->setText(aboutCaption);
    ui->menuBar->addAction(mb_action);
    connect(mb_action, SIGNAL(triggered()), this, SLOT(actionAbout_triggered()));
}

MainWindow::~MainWindow()
{
    if(receive_text_edit != nullptr)
    {
        delete receive_text_edit;
    }

    if(transmitt_text_edit != nullptr)
    {
        delete transmitt_text_edit;
    }

    if(serialPort.isOpen())
    {
        serialPort.close();
    }

    if(mb_action)
    {
        delete mb_action;
    }
    delete ui;
}

QStringList MainWindow::getPortNames()
{
    QStringList portNamesList;
    const auto serialPortInfos = QSerialPortInfo::availablePorts();
    for(const QSerialPortInfo& serialPortInfo : serialPortInfos)
    {
        if(!serialPortInfo.isBusy())
        {
            QString serialName = serialPortInfo.portName();
            portNamesList.append(serialName);
        }
    }
    return portNamesList;
}

void MainWindow::onBaudrateChanged()
{
    QString comboBoxTest = ui->comboBox_2->currentText();
    if(comboBoxTest == custormbrCaption)
    {
        ui->label_6->setEnabled(true);
        ui->lineEdit_1->setEnabled(true);
    }
    else
    {
        ui->label_6->setEnabled(false);
        ui->lineEdit_1->setEnabled(false);
    }
}

void MainWindow::onConnectBtnClicked()
{
    QString serialPortName = ui->comboBox_1->currentText();
    QStringList portNamesList = getPortNames();
    if(portNamesList.contains(serialPortName))
    {
        QFileInfo file_info(serialPortName);
        if (file_info.isSymLink())
        {
            serialPort.setPortName(file_info.symLinkTarget());
        }
        else
        {
            serialPort.setPortName(file_info.path());
        }
        qint32 baudrate = 0;
        QString baudrateStr{};
        if(ui->comboBox_2->currentText() != custormbrCaption)
        {
            baudrateStr = ui->comboBox_2->currentText();
        }
        else
        {
            baudrateStr = ui->lineEdit_1->text();
        }
        baudrate = baudrateStr.toInt();
        serialPort.setBaudRate(baudrate);

        QSerialPort::DataBits db = QSerialPort::Data8;
        switch(ui->comboBox_5->currentIndex())
        {
        case 0:
            db = QSerialPort::Data5;
            break;
        case 1:
            db = QSerialPort::Data6;
            break;
        case 2:
            db = QSerialPort::Data7;
            break;
        default:
            db = QSerialPort::Data8;
            break;
        }
        serialPort.setDataBits(db);

        QSerialPort::StopBits sb = QSerialPort::TwoStop;
        switch(ui->comboBox_3->currentIndex())
        {
        case 0:
            sb = QSerialPort::OneStop;
            break;
        default:
            sb = QSerialPort::TwoStop;
            break;
        }
        serialPort.setStopBits(sb);

        QSerialPort::Parity parity = QSerialPort::NoParity;
        switch(ui->comboBox_4->currentIndex())
        {
        case 0:
            parity = QSerialPort::NoParity;
            break;
        case 1:
            parity = QSerialPort::EvenParity;
            break;
        case 2:
            parity = QSerialPort::OddParity;
            break;
        default:
            parity = QSerialPort::NoParity;
            break;
        }
        serialPort.setParity(parity);

        if(ui->radioButton_1->isChecked())
        {
            serialPort.setFlowControl(QSerialPort::NoFlowControl);
        }
        else if(ui->radioButton_2->isChecked())
        {
            serialPort.setFlowControl(QSerialPort::HardwareControl);
        }
        else if(ui->radioButton_3->isChecked())
        {
            serialPort.setFlowControl(QSerialPort::SoftwareControl);
        }
        else
        {
            serialPort.setFlowControl(QSerialPort::NoFlowControl);
        }


        if(!serialPort.open(QIODevice::ReadWrite))
        {
            QString formattedCaption = QString(cantOpenCaption).arg(serialPortName);
            QMessageBox::information(this, errorCaption, formattedCaption);
        }
        else
        {
            ui->pushButton_1->setEnabled(false);
            ui->pushButton_2->setEnabled(true);
            ui->pushButton_3->setEnabled(false);
            ui->pushButton_4->setEnabled(true);
            for(int i = 1; i < index_6; i++)
            {
                QString comboName = comboBoxName + QString::number(i);
                QComboBox *cb = MainWindow::findChild<QComboBox *>(comboName);
                if(cb)
                {
                    cb->setEnabled(false);
                }
            }

            for(int i = 1; i < index_4; i++)
            {
                QString radioBtnName = radioButtonName + QString::number(i);
                QRadioButton *rb = MainWindow::findChild<QRadioButton *>(radioBtnName);
                if(rb)
                {
                    rb->setEnabled(false);
                }
            }
        }
    }
    else
    {
        QString formattedCaption = QString(portDoesntExistCaption).arg(serialPortName);
        QMessageBox::information(this, errorCaption, formattedCaption);
    }
}

void MainWindow::onDisconnectBtnClicked()
{
    ui->pushButton_1->setEnabled(true);
    ui->pushButton_2->setEnabled(false);
    ui->pushButton_3->setEnabled(true);
    ui->pushButton_4->setEnabled(false);

    for(int i = 1; i < index_6; i++)
    {
        QString comboName = comboBoxName + QString::number(i);
        QComboBox *cb = MainWindow::findChild<QComboBox *>(comboName);
        cb->setEnabled(true);
    }

    for(int i = 1; i < index_4; i++)
    {
        QString radioBtnName = radioButtonName + QString::number(i);
        QRadioButton *rb = MainWindow::findChild<QRadioButton *>(radioBtnName);
        rb->setEnabled(true);
    }

    QString comboBoxTest = ui->comboBox_2->currentText();
    if(comboBoxTest == custormbrCaption)
    {
        ui->label_6->setEnabled(true);
        ui->lineEdit_1->setEnabled(true);
    }
    else
    {
        ui->label_6->setEnabled(false);
        ui->lineEdit_1->setEnabled(false);
    }
    if(serialPort.isOpen())
    {
        serialPort.close();
    }
}

void MainWindow::onRescanBtnClicked()
{
    QStringList portNamesList = getPortNames();
    ui->comboBox_1->clear();
    for(QString serialName : portNamesList)
    {
        ui->comboBox_1->addItem(serialName);
    }
}

void MainWindow::onSendBtnClicked()
{
    QString sentLine = ui->lineEdit_2->text();
    if(ui->txHexCheckBox->checkState())
    {
        transmitt_text_edit->addHex(sentLine.toUtf8());
    }
    else
    {
        transmitt_text_edit->addText(sentLine);
    }

    serialPort.write(sentLine.toUtf8());
}

void MainWindow::onRxClearBtnClicked()
{
    receive_text_edit->clear();
}

void MainWindow::onTxClearBtnClicked()
{
    transmitt_text_edit->clear();
}

void MainWindow::serialPortHandleReadyRead()
{
    QByteArray readData = serialPort.readAll();
    if(ui->rxHexCheckBox->checkState())
    {
        receive_text_edit->addHex(readData);
    }
    else
    {
        QString readLine {readData};
        receive_text_edit->addText(readLine);
    }
}

void MainWindow::serialPortHandleError(QSerialPort::SerialPortError error)
{
    (void)error;
}

void MainWindow::on_actionExit_triggered()
{
    QCoreApplication::exit();
}

void MainWindow::actionAbout_triggered()
{
    AboutDialog about;
    about.setModal(true);
    about.exec();
}
