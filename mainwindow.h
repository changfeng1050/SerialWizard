//
// Created by chang on 2017-07-28.
//

#ifndef SERIALWIZARD_MAINWINDOW_H
#define SERIALWIZARD_MAINWINDOW_H

class QPushButton;

class QComboBox;

class QTextBrowser;

class QCheckBox;

class QLineEdit;

class QLabel;

class QTextEdit;

class QTimer;

class SerialSettings;

class AbstractReadWriter;

class QRadioButton;

class QButtonGroup;

#include <QtWidgets/QMainWindow>
#include "FrameInfoDialog.h"

struct RunConfig {
    QString lastDir;
    QString lastFilePath;
};


class MainWindow : public QMainWindow {
Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

    void init();

    void initUi();

    void createConnect();

    void createStatusBar();

    ~MainWindow() override;


public:

signals:

    void serialStateChanged(bool);

    void writeBytesChanged(qint64 bytes);

    void readBytesChanged(qint64 bytes);

    void currentWriteCountChanged(qint64 count);

public slots:


    void openReadWriter();

    void closeReadWriter();

    void sendOneFrameData();

    void sendOneLineData();

    void sendFixedCountData();

    void sendAllData();

    void readData();

    qint64 writeData(const QByteArray &data);

    void setOpenButtonText(bool isOpen);

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

    void handlerSerialNotOpen();

    void updateStatusMessage(const QString &message);

    void updateReadBytes(qint64 bytes);

    void updateWriteBytes(qint64 bytes);

    void updateCurrentWriteCount(qint64 count);

    void updateTcpClient(const QString &address, qint16 port);

    void clearTcpClient();


private:

    enum class SendType {
        Normal, Frame, Line, FixBytes
    };

    enum class AutoSendState {
        NotStart, Sending, Finish
    };

    bool isReadWriterOpen();

    bool isReadWriterConnected();

    void readSettings();

    void writeSettings();

    FrameInfo readFrameInfo() const;

    void writeFrameInfo(const FrameInfo &info) const;

    void createActions();

    void createMenu();

    void upDateSendData(bool isHex, const QString &text);

    QByteArray getNextFrameData();

    QByteArray getNextLineData();

    QByteArray getNextFrameData(QByteArray *data, int startIndex, FrameInfo *frameInfo);

    QByteArray getNextFixedCountData(QByteArray *data, int startIndex, int count);

    void sendStatusMessage(const QString &msg);

    void updateSendCount(qint64 count);

    void updateReceiveCount(qint64 count);

    void startAutoSendTimerIfNeed();

    void stopAutoSend();

    void resetSendButtonText();

    void updateTotalSendCount(qint64 count);

    void showReadData(const QByteArray &data);

    void showSendData(const QByteArray &data);

    QStringList getSerialNameList();

    RunConfig *runConfig{nullptr};

    //状态栏
    QLabel *statusBarReadBytesLabel;
    QLabel *statusBarWriteBytesLabel;
    QPushButton *statusBarResetCountButton;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *toolMenu;
    QMenu *helpMenu;

    QAction *openAct;
    QAction *saveAct;
    QAction *exitAct;
    QAction *validateDataAct;

    AbstractReadWriter *_readWriter{nullptr};

    qint64 sendCount{0};
    qint64 receiveCount{0};

    QRadioButton *serialRadioButton;
    QRadioButton *tcpRadioButton;
    QRadioButton *brigdeRadioButton;


    QButtonGroup *readWriterButtonGroup;

    // 串口设置
    QComboBox *serialPortNameComboBox;
    QComboBox *serialPortBaudRateComboBox;
    QComboBox *serialPortParityComboBox;
    QComboBox *serialPortDataBitsComboBox;
    QComboBox *serialPortStopBitsComboBox;
    QPushButton *openSerialButton;

    // TCP设置
    QLineEdit *tcpAddressLineEdit;
    QLineEdit *tcpPortLineEdit;
    QLabel *tcpClientLabel;

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

    QCheckBox *loopSendCheckBox;
    QPushButton *resetLoopSendButton;
    QLineEdit *currentSendCountLineEdit;
    QLabel *totalSendCountLabel;

    FrameInfo *frameInfo;


    QTextBrowser *receiveDataBrowser;
    QTextBrowser *sendDataBrowser;

    QTextEdit *sendTextEdit;

    QPushButton *sendFrameButton;
    QPushButton *sendLineButton;
    QPushButton *sendFixBytesButton;
    QLineEdit *byteCountLineEdit;
    QPushButton *sendAllButton;

    SendType sendType{SendType::Normal};

    AutoSendState autoSendState{AutoSendState::NotStart};

    QTimer *autoSendTimer{nullptr};
    QByteArray *mySendData{nullptr};
    QStringList *mySendList{nullptr};

    int currentSendCount{0};
    int totalSendCount{0};

    bool loopSend{false};
};


#endif //SERIALWIZARD_MAINWINDOW_H
