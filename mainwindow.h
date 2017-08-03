//
// Created by chang on 2017-07-28.
//

#ifndef SERIALWIZARD_MAINWINDOW_H
#define SERIALWIZARD_MAINWINDOW_H

class QPushButton;

class QComboBox;

class SerialPort;

class QTextBrowser;

class QCheckBox;

class QLineEdit;

class QTextEdit;

class QTimer;

class SerialSettings;

#include <QtWidgets/QMainWindow>
#include "FrameInfoDialog.h"
#include "serialport.h"


class MainWindow : public QMainWindow {
Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

    void init();

    void initUi();

    void initConnect();

    void createStatusBar();
    ~MainWindow() override;


public:

signals:

    void serialStateChanged(bool);

public slots:

    void openSerialPort();

    void closeSerialPort();

    void sendData();

private slots:

    void setOpenSerialButtonText(bool isOpen);

    void displayReceiveData(const QByteArray &data);

    void displaySentData(const QByteArray &data);

    void open();

    void save();

    void tool();

    void openDataValidator();

    void openFrameInfoSettingDialog();

    void clearReceivedData();

    void saveReceivedData();

    void clearSentData();

    void saveSentData();

private:

    void readSettings();

    void writeSettings();

    FrameInfo readFrameInfo() const;

    void writeFrameInfo(const FrameInfo &info) const;

    void createActions();

    void createMenu();

    void upDateSendData(bool isHex, const QString &text);

    QByteArray getNextFrameData();

    QByteArray getNextFrameData(QByteArray *data, int startIndex, FrameInfo *frameInfo);


    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *toolManu;
    QMenu *helpMenu;

    QAction *openAct;
    QAction *saveAct;
    QAction *exitAct;
    QAction *validateDataAct;

    SerialPort *serialPort;
    SerialSettings *serialSettings;

    // 串口设置
    QComboBox *serialPortNameComboBox;
    QComboBox *serialPortBaudRateComboBox;
    QComboBox *serialPortParityComboBox;
    QComboBox *serialPortDataBitsComboBox;
    QComboBox *serialPortStopBitsComboBox;
    QPushButton *openSerialButton;

    // 接收设置
    QCheckBox *addLineReturnCheckBox;
    QCheckBox *displayReceiveDataAsHexCheckBox;
    QCheckBox *pauseReceiveCheckBox;
    QPushButton *saveReceiveDataButton;
    QPushButton *clearReceiveDataButton;
    QCheckBox *addReceiveTimestampCheckBox;

    // 发送设置
    QCheckBox *sendHexCheckBox;
    QCheckBox *displaySendDataCheckBox;
    QCheckBox *displaySendDataAsHexCheckBox;
    QCheckBox *autoSendCheckBox;
    QLineEdit *sendIntervalLineEdit;
    QPushButton *frameInfoSettingButton;
    QPushButton *saveSentDataButton;
    QPushButton *clearSentDataButton;


    FrameInfo *frameInfo;


    QTextBrowser *receiveDataBrowser;
    QTextBrowser *sendDataBrowser;

    QTextEdit *sendTextEdit;

    QPushButton *sendButton;

    QTimer *autoSendTimer;
    QByteArray *mySendData;
    QString sendText;

};


#endif //SERIALWIZARD_MAINWINDOW_H
