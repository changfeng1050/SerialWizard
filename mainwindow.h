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

class SerialController;

#include <QtWidgets/QMainWindow>

struct RunConfig {
    QString lastDir;
    QString lastFilePath;
};

class MainWindow : public QMainWindow {
Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

    enum class SerialType {
        Normal = 1,
        TcpServer,
        TcpClient,
        Bridge,
        SerialBridge,
    };

    enum class LineReturn {
        RN = 1,
        R,
        N,
    };

    void init();

    void initUi();

    void setAcceptWindowDrops();

    void createConnect();

    void createStatusBar();

    ~MainWindow() override;

    enum class SendType {
        Line
    };
protected:
    void dropEvent(QDropEvent *event) override;

    void dragEnterEvent(QDragEnterEvent *event) override;

signals:

    void serialStateChanged(bool);

    void writeBytesChanged(qint64 bytes);

    void readBytesChanged(qint64 bytes);

    void writeLinesChanged(qint64 lines);

    void currentWriteCountChanged(qint64 count);

public slots:

    void openReadWriter();

    void closeReadWriter();

    void sendNextData();

    void readData();

    qint64 writeData(const QByteArray &data);

    void setOpenButtonText(bool isOpen);

    void displayReceiveData(const QByteArray &data);

    void displaySentData(const QByteArray &data);

    void open();

    void save();

    void openDataValidator();

    void openConvertDataDialog();

    void openDataProcessDialog(const QString &text);

    void clearReceivedData();

    void saveReceivedData();

    void clearSentData();

    void saveSentData();

    void handlerSerialNotOpen();

    void updateStatusMessage(const QString &message);

    void updateReadBytes(qint64 bytes);

    void updateWriteBytes(qint64 bytes);

    void updateWriteLines(qint64 lines);

    void updateCurrentWriteCount(qint64 count);

    void updateTcpClient(const QString &address, qint16 port);

    void clearTcpClient();

    void updateSendType();

private:

    enum class AutoSendState {
        NotStart, Sending, Finish
    };

    bool isReadWriterOpen();

    bool isReadWriterConnected();

    void readSettings();

    void writeSettings();

    void createActions();

    void createMenu();

    void updateSendData(bool isHex, const QString &text);

    void updateSerialPortNames();

    void updateReceiveCount(qint64 count);

    void startAutoSendTimerIfNeed();

    void stopAutoSend();

    void resetSendButtonText();

    void updateTotalSendCount(qint64 count);

    void showReadData(const QByteArray &data);

    void showSendData(const QByteArray &data);

    static QStringList getSerialNameList();

    RunConfig *runConfig{nullptr};

    //状态栏
    QLabel *statusBarSendLinesLabel{nullptr};
    QLabel *statusBarReadBytesLabel{nullptr};
    QLabel *statusBarWriteBytesLabel{nullptr};
    QPushButton *statusBarResetCountButton{nullptr};

    QMenu *fileMenu{nullptr};
    QMenu *editMenu{nullptr};
    QMenu *toolMenu{nullptr};
    QMenu *helpMenu{nullptr};

    QAction *openAct{nullptr};
    QAction *saveAct{nullptr};
    QAction *exitAct{nullptr};
    QAction *validateDataAct{nullptr};
    QAction *convertDataAct{nullptr};
    QAction *dataProcessAct{nullptr};

    AbstractReadWriter *_readWriter{nullptr};

    qint64 sendCount{0};
    qint64 receiveCount{0};
    qint64 totalSendLineCount{0};

    QRadioButton *serialRadioButton{nullptr};
    QRadioButton *tcpServerRadioButton{nullptr};
    QRadioButton *tcpClientRadioButton{nullptr};
    QRadioButton *bridgeRadioButton{nullptr};
    QRadioButton *serialBridgeRadioButton{nullptr};

    QButtonGroup *readWriterButtonGroup{nullptr};

    // 串口设置
    QComboBox *serialPortNameComboBox{nullptr};
    QComboBox *serialPortBaudRateComboBox{nullptr};
    QComboBox *serialPortParityComboBox{nullptr};
    QComboBox *serialPortDataBitsComboBox{nullptr};
    QComboBox *serialPortStopBitsComboBox{nullptr};
    QPushButton *openSerialButton{nullptr};
    QPushButton *refreshSerialButton{nullptr};

    // 第二串口设置
    QComboBox *secondSerialPortNameComboBox{nullptr};
    QComboBox *secondSerialPortBaudRateComboBox{nullptr};
    QComboBox *secondSerialPortParityComboBox{nullptr};
    QComboBox *secondSerialPortDataBitsComboBox{nullptr};
    QComboBox *secondSerialPortStopBitsComboBox{nullptr};

    // TCP设置
    QComboBox *tcpAddressComboBox{nullptr};
    QLineEdit *tcpPortLineEdit{nullptr};
    QLabel *tcpClientLabel{nullptr};

    // 接收设置
    QCheckBox *addLineReturnCheckBox{nullptr};
    QCheckBox *displayReceiveDataAsHexCheckBox{nullptr};
    QCheckBox *pauseReceiveCheckBox{nullptr};
    QPushButton *saveReceiveDataButton{nullptr};
    QPushButton *clearReceiveDataButton{nullptr};
    QCheckBox *addReceiveTimestampCheckBox{nullptr};

    // 发送设置
    QCheckBox *hexCheckBox{nullptr};
    QCheckBox *displaySendDataCheckBox{nullptr};
    QCheckBox *displaySendDataAsHexCheckBox{nullptr};
    QCheckBox *autoSendCheckBox{nullptr};
    QLineEdit *sendIntervalLineEdit{nullptr};
    QLineEdit *emptyLineDelayLindEdit{nullptr};
    QPushButton *saveSentDataButton{nullptr};
    QPushButton *clearSentDataButton{nullptr};

    QCheckBox *loopSendCheckBox{nullptr};
    QPushButton *resetLoopSendButton{nullptr};
    QLineEdit *currentSendCountLineEdit{nullptr};
    QLabel *totalSendCountLabel{nullptr};
    QPushButton *previousButton{nullptr};
    QPushButton *nextButton{nullptr};

    QTextBrowser *receiveDataBrowser{nullptr};
    QTextBrowser *sendDataBrowser{nullptr};

    QTextEdit *sendTextEdit{nullptr};

    QPushButton *sendLineButton{nullptr};
    QPushButton *processTextButton{nullptr};
    QPushButton *clearTextButton{nullptr};
    QCheckBox *sendLineReturnCheckBox{nullptr};
    QRadioButton *sendRNLineReturnButton{nullptr};
    QRadioButton *sendRReturnLineButton{nullptr};
    QRadioButton *sendNReturnLineButton{nullptr};

    QButtonGroup *lineReturnButtonGroup{nullptr};

    AutoSendState autoSendState{AutoSendState::NotStart};

    QTimer *autoSendTimer{nullptr};

    SerialController *serialController{nullptr};

    bool _loopSend{false};

    SendType _sendType{SendType::Line};

    SerialType _serialType{SerialType::Normal};

    int skipSendCount{0};

    QByteArray lineReturn;

    bool _dirty{true};
};

#endif //SERIALWIZARD_MAINWINDOW_H
