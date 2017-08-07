//
// Created by chang on 2017-07-28.
//
#include <QAction>
#include <QCheckBox>
#include <QLineEdit>
#include <QMenu>
#include <QMenuBar>
#include <QtSerialPort/QSerialPort>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QGroupBox>
#include <QDebug>
#include <QTextBrowser>
#include <QtWidgets/QFileDialog>
#include <QTimer>
#include <QtCore/QSettings>
#include <QtCore/QProcess>
#include <QStatusBar>

#include "MainWindow.h"
#include "CalculateCheckSumDialog.h"
#include "global.h"

int lastIndex = 0;
int lastLineIndex = 0;

static QByteArray dataToHex(const QByteArray &data) {
    QByteArray result = data.toHex().toUpper();

    for (int i = 2; i < result.size(); i += 3)
        result.insert(i, ' ');

    return result;
}

static QByteArray dataFromHex(const QString &hex) {
    QByteArray line = hex.toLatin1();
    line.replace(' ', QByteArray());

    auto result = QByteArray::fromHex(line);

    return result;
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), receiveCount(0), sendCount(0) {

    init();
    initUi();
    initConnect();

    readSettings();
    createActions();
    createMenu();
    createStatusBar();
}

MainWindow::~MainWindow() {

    writeSettings();

    if (serialPort != nullptr) {
        serialPort->close();
        serialPort = nullptr;
    }
}


void MainWindow::init() {
    serialPort = new SerialPort(this);
    autoSendTimer = new QTimer();
    mySendData = nullptr;
    mySendList = nullptr;
    frameInfo = nullptr;

}

void MainWindow::initUi() {

    setMinimumSize(1024, 900);
    auto serialPortNameLabel = new QLabel(tr("串口"), this);
    QStringList serialPortNameList;
    for (int i = 0; i < 20; ++i) {
        serialPortNameList << QString("COM%1").arg(i + 1);
    }
    serialPortNameComboBox = new QComboBox(this);
    serialPortNameComboBox->addItems(serialPortNameList);
    serialPortNameLabel->setBuddy(serialPortNameComboBox);

    auto *serialPortBaudRateLabel = new QLabel(tr("波特率"), this);
    serialPortBaudRateComboBox = new QComboBox(this);
    serialPortBaudRateComboBox->addItems(QStringList()
                                                 << "1200"
                                                 << "2400"
                                                 << "4800"
                                                 << "9600"
                                                 << "19200"
                                                 << "38400"
                                                 << "57600"
                                                 << "115200"

    );
    serialPortBaudRateLabel->setBuddy(serialPortBaudRateComboBox);


    auto serialPortDataBitsLabel = new QLabel(tr("数据位"), this);
    serialPortDataBitsComboBox = new QComboBox(this);
    serialPortDataBitsComboBox->addItems(QStringList() << "5" << "6" << "7" << "8");
    serialPortDataBitsLabel->setBuddy(serialPortDataBitsComboBox);

    auto serialPortStopBitsLabel = new QLabel(tr("停止位"), this);
    serialPortStopBitsComboBox = new QComboBox(this);
    serialPortStopBitsLabel->setBuddy(serialPortStopBitsComboBox);
    serialPortStopBitsComboBox->addItem(tr("1"), QSerialPort::OneStop);
    serialPortStopBitsComboBox->addItem(tr("1.5"), QSerialPort::OneAndHalfStop);
    serialPortStopBitsComboBox->addItem(tr("2"), QSerialPort::TwoStop);

    auto serialPortParityLabel = new QLabel(tr("校验位"), this);
    serialPortParityComboBox = new QComboBox(this);
    serialPortParityComboBox->addItem(tr("无校验"), QSerialPort::NoParity);
    serialPortParityComboBox->addItem(tr("奇校验"), QSerialPort::OddParity);
    serialPortParityComboBox->addItem(tr("偶校验"), QSerialPort::EvenParity);
    serialPortParityComboBox->addItem(tr("空校验"), QSerialPort::SpaceParity);
    serialPortParityComboBox->addItem(tr("标志校验"), QSerialPort::MarkParity);
    serialPortParityLabel->setBuddy(serialPortParityComboBox);

    auto serialPortSettingsGridLayout = new QGridLayout;
    serialPortSettingsGridLayout->addWidget(serialPortNameLabel, 0, 0);
    serialPortSettingsGridLayout->addWidget(serialPortNameComboBox, 0, 1);
    serialPortSettingsGridLayout->addWidget(serialPortBaudRateLabel, 1, 0);
    serialPortSettingsGridLayout->addWidget(serialPortBaudRateComboBox, 1, 1);
    serialPortSettingsGridLayout->addWidget(serialPortDataBitsLabel, 2, 0);
    serialPortSettingsGridLayout->addWidget(serialPortDataBitsComboBox, 2, 1);
    serialPortSettingsGridLayout->addWidget(serialPortStopBitsLabel, 3, 0);
    serialPortSettingsGridLayout->addWidget(serialPortStopBitsComboBox, 3, 1);
    serialPortSettingsGridLayout->addWidget(serialPortParityLabel, 4, 0);
    serialPortSettingsGridLayout->addWidget(serialPortParityComboBox, 4, 1);

    openSerialButton = new QPushButton(tr("打开"), this);

    auto serialPortSettingsLayout = new QVBoxLayout;
    serialPortSettingsLayout->addLayout(serialPortSettingsGridLayout);
    serialPortSettingsLayout->addWidget(openSerialButton);

    auto serialPortGroupBox = new QGroupBox(tr("串口设置"));
    serialPortGroupBox->setLayout(serialPortSettingsLayout);


    addLineReturnCheckBox = new QCheckBox(tr("自动换行"), this);
    displayReceiveDataAsHexCheckBox = new QCheckBox(tr("按十六进制显示"), this);
    addReceiveTimestampCheckBox = new QCheckBox(tr("添加接收时间戳"), this);
    pauseReceiveCheckBox = new QCheckBox(tr("暂停接收数据"), this);
    saveReceiveDataButton = new QPushButton(tr("保存数据"), this);
    clearReceiveDataButton = new QPushButton(tr("清除显示"), this);

    auto receiveSettingLayout = new QVBoxLayout;
    receiveSettingLayout->addWidget(addLineReturnCheckBox);
    receiveSettingLayout->addWidget(displayReceiveDataAsHexCheckBox);
    receiveSettingLayout->addWidget(addReceiveTimestampCheckBox);
    receiveSettingLayout->addWidget(pauseReceiveCheckBox);

    auto receiveSettingHBoxLayout1 = new QHBoxLayout;
    receiveSettingHBoxLayout1->addWidget(saveReceiveDataButton);
    receiveSettingHBoxLayout1->addWidget(clearReceiveDataButton);

    receiveSettingLayout->addLayout(receiveSettingHBoxLayout1);

    auto receiveSettingGroupBox = new QGroupBox(tr("接收设置"));
    receiveSettingGroupBox->setLayout(receiveSettingLayout);

    receiveDataBrowser = new QTextBrowser(this);
    auto receiveDataLayout = new QVBoxLayout;
    receiveDataLayout->addWidget(receiveDataBrowser);
    auto receiveDataGroupBox = new QGroupBox(tr("数据接收显示"));
    receiveDataGroupBox->setLayout(receiveDataLayout);

    sendHexCheckBox = new QCheckBox(tr("按十六进制发送"), this);
    displaySendDataCheckBox = new QCheckBox(tr("显示发送数据"), this);
    displaySendDataAsHexCheckBox = new QCheckBox(tr("十六进制显示发送"), this);


    autoSendCheckBox = new QCheckBox(tr("自动循环发送"), this);
    auto sendIntervalLabel = new QLabel(tr("自动发送间隔(毫秒)"), this);
    sendIntervalLineEdit = new QLineEdit(this);
    sendIntervalLineEdit->setMaximumWidth(50);
    sendIntervalLineEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Maximum);
    sendIntervalLabel->setBuddy(sendIntervalLineEdit);

    auto sendIntervalLayout = new QHBoxLayout;
    sendIntervalLayout->addWidget(sendIntervalLabel);
    sendIntervalLayout->addWidget(sendIntervalLineEdit);

    auto autoSendLayout = new QVBoxLayout;
    autoSendLayout->addWidget(autoSendCheckBox);
    autoSendLayout->addLayout(sendIntervalLayout);
    auto autoSendGroupBox = new QGroupBox("自动发送设置");
    autoSendGroupBox->setLayout(autoSendLayout);

    saveSentDataButton = new QPushButton(tr("保存数据"), this);
    clearSentDataButton = new QPushButton(tr("清除显示"), this);
    auto sendSettingsHLayout1 = new QHBoxLayout;
    sendSettingsHLayout1->addWidget(saveSentDataButton);
    sendSettingsHLayout1->addWidget(clearSentDataButton);;

    auto sendSettingLayout = new QVBoxLayout;
    sendSettingLayout->addWidget(sendHexCheckBox);
    sendSettingLayout->addWidget(displaySendDataCheckBox);
    sendSettingLayout->addWidget(displaySendDataAsHexCheckBox);
    sendSettingLayout->addLayout(sendSettingsHLayout1);

    auto sendSettingGroupBox = new QGroupBox(tr("发送设置"));
    sendSettingGroupBox->setLayout(sendSettingLayout);

    frameInfoSettingCheckBox = new QCheckBox(tr("按照数据帧格式"), this);
    sendFrameButton = new QPushButton(tr("发送下一帧"), this);

    auto frameLayout = new QVBoxLayout;
    frameLayout->addWidget(frameInfoSettingCheckBox);
    frameLayout->addWidget(sendFrameButton);


    auto frameGroupBox = new QGroupBox(tr("按数据帧发送"));
    frameGroupBox->setLayout(frameLayout);

    auto lineGroupBox = new QGroupBox(tr("按行发送"));
    sendLineButton = new QPushButton(tr("发送下一行"));
    auto lineLayout = new QVBoxLayout;
    lineLayout->addWidget(sendLineButton);
    lineGroupBox->setLayout(lineLayout);

    auto fixBytesGroupBox = new QGroupBox(tr("按固定字节发送"));

    auto byteCountLabel = new QLabel(tr("每次发送字节数"), this);
    byteCountLineEdit = new QLineEdit(this);
    byteCountLineEdit->setMaximumWidth(50);
    byteCountLineEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Maximum);
    byteCountLabel->setBuddy(byteCountLineEdit);

    auto byteCountLayout = new QHBoxLayout;
    byteCountLayout->addWidget(byteCountLabel);
    byteCountLayout->addWidget(byteCountLineEdit);

    sendFixBytesButton = new QPushButton(tr("发送下一帧"), this);

    auto sendBytByteCountLayout = new QVBoxLayout;
    sendBytByteCountLayout->addLayout(byteCountLayout);
    sendBytByteCountLayout->addWidget(sendFixBytesButton);
    fixBytesGroupBox->setLayout(sendBytByteCountLayout);

    auto optionSendLayout = new QVBoxLayout;
    optionSendLayout->addWidget(frameGroupBox);
    optionSendLayout->addWidget(lineGroupBox);
    optionSendLayout->addWidget(fixBytesGroupBox);

    sendDataBrowser = new QTextBrowser(this);

    auto sendDataLayout = new QVBoxLayout;
    sendDataLayout->addWidget(sendDataBrowser);
    auto sendDataGroupBox = new QGroupBox(tr("数据发送显示"));
    sendDataGroupBox->setLayout(sendDataLayout);

    sendTextEdit = new QTextEdit(this);
    transferHexButton = new QPushButton(tr("转换为十六进制"), this);
    transferAsciiButton = new QPushButton(tr("转换为文本"), this);

    sendButton = new QPushButton(tr("发送"));

    auto sendButtonLayout = new QVBoxLayout;
    sendButtonLayout->addWidget(transferHexButton);
    sendButtonLayout->addWidget(transferAsciiButton);
    sendButtonLayout->addWidget(sendButton);

    auto sendLayout = new QHBoxLayout;
    sendLayout->addLayout(optionSendLayout);
    sendLayout->addWidget(sendTextEdit);
    sendLayout->addLayout(sendButtonLayout);

    auto sendGroupBox = new QGroupBox;
    sendGroupBox->setLayout(sendLayout);

    auto mainVBoxLayout1 = new QVBoxLayout;
    mainVBoxLayout1->addWidget(serialPortGroupBox);
    mainVBoxLayout1->addWidget(receiveSettingGroupBox);
    mainVBoxLayout1->addWidget(sendSettingGroupBox);
    mainVBoxLayout1->addWidget(autoSendGroupBox);
//    mainVBoxLayout1->addWidget(frameGroupBox);
//    mainVBoxLayout1->addWidget(lineGroupBox);
//    mainVBoxLayout1->addWidget(fixBytesGroupBox);
    mainVBoxLayout1->addStretch();

    auto mainVBoxLayout2 = new QVBoxLayout;
    mainVBoxLayout2->addWidget(receiveDataGroupBox);
    mainVBoxLayout2->addWidget(sendDataGroupBox);
    mainVBoxLayout2->addWidget(sendGroupBox);

    auto widget = new QWidget(this);

    auto mainLayout = new QHBoxLayout;

    mainLayout->addLayout(mainVBoxLayout1);
    mainLayout->addLayout(mainVBoxLayout2);

    widget->setLayout(mainLayout);
    setCentralWidget(widget);
}

void MainWindow::createStatusBar() {

    auto receiveByteCountLabel = new QLabel(tr("接收:"), this);
    statusBarReceiveByteCountLabel = new QLabel(this);
    statusBarReceiveByteCountLabel->setMinimumWidth(100);

    auto sendByteCountLabel = new QLabel(tr("发送:"), this);

    statusBarSendByteCountLabel = new QLabel(this);
    statusBarSendByteCountLabel->setMinimumWidth(100);

    statusBarResetCountButton = new QPushButton(tr("重置"), this);

    statusBar()->addPermanentWidget(receiveByteCountLabel);
    statusBar()->addPermanentWidget(statusBarReceiveByteCountLabel);
    statusBar()->addPermanentWidget(sendByteCountLabel);
    statusBar()->addPermanentWidget(statusBarSendByteCountLabel);
    statusBar()->addPermanentWidget(statusBarResetCountButton);


    connect(statusBarResetCountButton, &QPushButton::clicked, [this] {
        receiveCount = 0;
        sendCount = 0;
        updateReceiveCount(receiveCount);
        updateSendCount(sendCount);
    });
}


void MainWindow::openSerialPort() {

    auto settings = new SerialSettings();
    settings->name = serialPortNameComboBox->currentText();
    settings->baudRate = serialPortBaudRateComboBox->currentText().toInt();
    settings->dataBits = (QSerialPort::DataBits) serialPortDataBitsComboBox->currentText().toInt();
    settings->stopBits = (QSerialPort::StopBits) serialPortStopBitsComboBox->currentData().toInt();
    settings->parity = (QSerialPort::Parity) serialPortParityComboBox->currentData().toInt();

    serialPort->setSerialSettings(*settings);
    auto result = serialPort->open();

    qDebug() << settings->name << settings->baudRate << settings->dataBits << settings->stopBits << settings->parity;

    if (result == SerialOpenResult::SerialOpenResultSuccess) {
        qDebug() << "serial opened success";
    } else {
        qDebug() << "serial opened failed";
    }

    emit serialStateChanged(result == SerialOpenResult::SerialOpenResultSuccess);
}

void MainWindow::closeSerialPort() {
    if (serialPort != nullptr) {
        serialPort->close();
    }

    emit serialStateChanged(false);
}

void MainWindow::initConnect() {

    connect(this, &MainWindow::serialStateChanged, [this](bool isOpen) {
        setOpenSerialButtonText(isOpen);
    });


    connect(openSerialButton, &QPushButton::clicked, [=](bool value) {
        if (serialPort->isOpen()) {
            closeSerialPort();
        } else {
            openSerialPort();
        }
    });


    connect(saveReceiveDataButton, &QPushButton::clicked, this, &MainWindow::saveReceivedData);
    connect(clearReceiveDataButton, &QPushButton::clicked, this, &MainWindow::clearReceivedData);

    connect(saveSentDataButton, &QPushButton::clicked, this, &MainWindow::saveSentData);
    connect(clearSentDataButton, &QPushButton::clicked, this, &MainWindow::clearSentData);


    connect(autoSendCheckBox, &QCheckBox::clicked, [this] {
        autoSendTimer->stop();
    });

    connect(serialPort, &SerialPort::dataReceived, this, &MainWindow::receivedData);
    connect(serialPort, &SerialPort::dataSent, this, &MainWindow::sentData);

    connect(frameInfoSettingCheckBox, &QCheckBox::clicked, [this] {
        if (frameInfoSettingCheckBox->isChecked()) {
            openFrameInfoSettingDialog();
        } else {
            frameInfo = nullptr;
        }
    });

    connect(sendFrameButton, &QPushButton::clicked, [this](bool value) {
        sendType = SendType::Frame;
        upDateSendData(sendHexCheckBox->isChecked(), sendTextEdit->toPlainText());
        sendOneFrameData();

        startAutoSendTimerIfNeed();
    });

    connect(sendLineButton, &QPushButton::clicked, [this] {
        sendType = SendType::Line;
        upDateSendData(sendHexCheckBox->isChecked(), sendTextEdit->toPlainText());
        sendOneLineData();

        startAutoSendTimerIfNeed();
    });

    connect(sendFixBytesButton, &QPushButton::clicked, [this] {
        sendType = SendType::FixBytes;
        upDateSendData(sendHexCheckBox->isChecked(), sendTextEdit->toPlainText());
        sendFixedCountData();
        startAutoSendTimerIfNeed();
    });

    connect(sendButton, &QPushButton::clicked, [this](bool value) {
        sendType = SendType::Normal;
        upDateSendData(sendHexCheckBox->isChecked(), sendTextEdit->toPlainText());
        sendAllData();

        startAutoSendTimerIfNeed();
    });

    connect(transferHexButton, &QPushButton::clicked, [this] {
        auto text = sendTextEdit->toPlainText();
        auto result = QString(dataToHex(text.toLocal8Bit()));
        sendTextEdit->setText(result);
    });
    connect(transferAsciiButton, &QPushButton::clicked, [this] {
        auto text = sendTextEdit->toPlainText();
        QString result = QString::fromLocal8Bit(dataFromHex(text));
        sendTextEdit->setText(result);
    });

    connect(autoSendTimer, &QTimer::timeout, [this] {
        switch (sendType) {
            case SendType::Normal:
                sendAllData();
                break;
            case SendType::Frame:
                sendOneFrameData();
                break;
            case SendType::Line:
                sendOneLineData();
                break;
            case SendType::FixBytes:
                sendFixedCountData();
                break;
        }
    });
}

void MainWindow::setOpenSerialButtonText(bool isOpen) {
    if (isOpen) {
        openSerialButton->setText(tr("关闭"));
    } else {
        openSerialButton->setText("打开");
    }
}

void MainWindow::createActions() {
    openAct = new QAction(tr("&打开(&O)"), this);
    openAct->setShortcut(QKeySequence::Open);
    openAct->setStatusTip(tr("打开一个文件"));
    connect(openAct, &QAction::triggered, this, &MainWindow::open);

    saveAct = new QAction(tr("&保存(&S)"), this);
    saveAct->setShortcut(QKeySequence::Save);
    saveAct->setStatusTip(tr("保存一个文件"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::save);

    validateDataAct = new QAction(tr("计算校验(&C)"), this);
    validateDataAct->setShortcut(tr("Ctr+C"));
    validateDataAct->setStatusTip(tr("计算数据校验值"));
    connect(validateDataAct, &QAction::triggered, this, &MainWindow::openDataValidator);

}

void MainWindow::createMenu() {
    fileMenu = menuBar()->addMenu(tr("文件(&F)"));
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);

    toolMenu = menuBar()->addMenu(tr("工具(&T)"));
    toolMenu->addAction(validateDataAct);


}

void MainWindow::open() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("打开数据文件"), "/", "");
    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly)) {
        auto data = file.readAll();

        sendTextEdit->setText(QString::fromLocal8Bit(data));
    }
}

void MainWindow::save() {
    saveReceivedData();
}

void MainWindow::tool() {

}

void MainWindow::openDataValidator() {
    CalculateCheckSumDialog dialog(this);
    dialog.exec();
}

void MainWindow::displayReceiveData(const QByteArray &data) {

    if (pauseReceiveCheckBox->isChecked()) {
        return;
    }

    static QString s;

    s.clear();

    if (addReceiveTimestampCheckBox->isChecked()) {
        s.append("[").append(getTimestamp()).append("] ");
    }

    if (displayReceiveDataAsHexCheckBox->isChecked()) {
        s.append(dataToHex(data));
    } else {
        s.append(QString::fromLocal8Bit(data));
    }

    if (addLineReturnCheckBox->isChecked() || addReceiveTimestampCheckBox->isChecked()) {
        receiveDataBrowser->append(s);
    } else {
        auto text = receiveDataBrowser->toPlainText();
        text.append(s);
        receiveDataBrowser->setText(text);
        auto cursor = receiveDataBrowser->textCursor();
        receiveDataBrowser->moveCursor(QTextCursor::End);
    }

}

void MainWindow::displaySentData(const QByteArray &data) {
    if (displaySendDataAsHexCheckBox->isChecked()) {
        sendDataBrowser->append(dataToHex(data));
    } else {
        sendDataBrowser->append(QString::fromLocal8Bit(data));
    }
}

void MainWindow::openFrameInfoSettingDialog() {

    auto info = readFrameInfo();

    FrameInfoDialog dialog(info, this);

    connect(&dialog, &FrameInfoDialog::frameInfoChanged, [this](FrameInfo info) {
        frameInfo = new FrameInfo(info);
        writeFrameInfo(info);
        qDebug() << "head" << frameInfo->head << "end" << frameInfo->end
                 << "len index" << info.lenIndex << "len count" << info.lenCount
                 << "head len" << info.headLen << "end len" << info.endLen;
    });


    dialog.exec();
}

QByteArray MainWindow::getNextFrameData() {

    if (mySendData == nullptr) {
        return QByteArray();
    }

    if (!frameInfoSettingCheckBox->isChecked() || frameInfo == nullptr) {
        int len = 128;
        auto data = mySendData->mid(lastIndex, len);
        if (data.count() == 0) {
            lastIndex = 0;
        }
        lastIndex += data.count();
        return data;
    } else {
        auto data = getNextFrameData(mySendData, lastIndex, frameInfo);
        if (data.count() == 0) {
            lastIndex = 0;
        }

        return data;
    }
}

QByteArray MainWindow::getNextLineData() {
    if (mySendList == nullptr || mySendList->isEmpty()) {
        return QByteArray();
    }

    if (lastLineIndex + 1 > mySendList->count()) {
        lastLineIndex = 0;
    }
    auto line = (*mySendList)[lastLineIndex];
    lastLineIndex++;

//    qDebug() << "getNextLineData()"  << line;
    if (sendHexCheckBox->isChecked()) {
        return dataFromHex(line);
    } else {
        return line.toLocal8Bit();
    }
}


void MainWindow::sendAllData() {
    auto &data = *mySendData;
    if (data.isEmpty() || !serialPort->isOpen()) {
        return;
    }

    serialPort->write(data);
}

void MainWindow::sendOneFrameData() {
    auto data = getNextFrameData();
    if (data.isEmpty()) {
        return;
    }

    if (serialPort->isOpen()) {
        serialPort->write(data);
    }
}

void MainWindow::sendOneLineData() {
    auto data = getNextLineData();
    if (data.isEmpty()) {
        return;
    }
    if (serialPort->isOpen()) {
        qDebug() << "sendOneLineData()" << data.toHex();
        serialPort->write(data);
    }
}


void MainWindow::sendFixedCountData() {
    auto data = getNextFixedCountData(mySendData, lastIndex, byteCountLineEdit->text().toInt());
    if (data.isEmpty()) {
        return;
    }
    if (serialPort->isOpen()) {
        serialPort->write(data);
    }
}

void MainWindow::upDateSendData(bool isHex, const QString &text) {
    if (mySendData != nullptr) {
        return;
    }
    if (mySendData == nullptr) {
        mySendData = new QByteArray;
    }
    mySendData->clear();
    if (isHex) {
        mySendData->append(dataFromHex(text));
    } else {
        mySendData->append(text.toLocal8Bit());
    }

    if (mySendList == nullptr) {
        mySendList = new QStringList;
    }
    mySendList->clear();
    QString text_temp(text);
    QTextStream in(&text_temp);

    while (!in.atEnd()) {
        auto line = in.readLine();
        *mySendList << line;
    }
}

QByteArray MainWindow::getNextFrameData(QByteArray *data, int startIndex, FrameInfo *frameInfo) {
    int headIndex = data->indexOf(frameInfo->head, startIndex);
    int endIndex = data->indexOf(frameInfo->end, startIndex + 1);

    lastIndex = endIndex;

    return data->mid(headIndex, endIndex - headIndex + 1);
}

QByteArray MainWindow::getNextFixedCountData(QByteArray *data, int startIndex, int count) {
    if (data == nullptr || data->isEmpty()) {
        return QByteArray();
    }
    if (startIndex + count + 1 < data->count()) {
        lastIndex = startIndex + count;
    } else {
        lastIndex = 0;
    }
    return data->mid(lastIndex, count);
}

void MainWindow::readSettings() {

    qDebug() << "readSettings";

    QSettings settings("Zhou Jinlong", "Serial Wizard");

    settings.beginGroup("SerialSettings");
    auto nameIndex = settings.value("name", 0).toInt();
    auto baudRateIndex = settings.value("baud_rate", 0).toInt();
    auto dataBitsIndex = (QSerialPort::DataBits) settings.value("data_bits", 0).toInt();
    auto stopBitsIndex = (QSerialPort::StopBits) settings.value("stop_bits", 0).toInt();
    auto parityIndex = (QSerialPort::Parity) settings.value("parity", 0).toInt();
    auto sendText = settings.value("send_text", "").toString();

    serialPortNameComboBox->setCurrentIndex(nameIndex);
    serialPortBaudRateComboBox->setCurrentIndex(baudRateIndex);
    serialPortDataBitsComboBox->setCurrentIndex(dataBitsIndex);
    serialPortStopBitsComboBox->setCurrentIndex(stopBitsIndex);
    serialPortParityComboBox->setCurrentIndex(parityIndex);

    sendTextEdit->setText(sendText);


    settings.beginGroup("SerialReceiveSettings");
    auto addLineReturn = settings.value("add_line_return", true).toBool();
    auto displayReceiveDataAsHex = settings.value("display_receive_data_as_hex", false).toBool();
    auto addTimestamp = settings.value("add_timestamp", false).toBool();

    addLineReturnCheckBox->setChecked(addLineReturn);
    displayReceiveDataAsHexCheckBox->setChecked(displayReceiveDataAsHex);
    addReceiveTimestampCheckBox->setChecked(addTimestamp);

    settings.beginGroup("SerialSendSettings");
    auto sendAsHex = settings.value("send_as_hex", false).toBool();
    auto displaySendData = settings.value("display_send_data", false).toBool();
    auto displaySendDataAsHex = settings.value("display_send_data_as_hex", false).toBool();
    auto autoSend = settings.value("auto_send", false).toBool();
    auto autoSendInterval = settings.value("auto_send_interval", 100).toInt();

    auto enableFrameInfo = settings.value("enable_frame_info", false).toBool();

    auto fixByteCount = settings.value("fix_byte_count", false).toInt();

    sendHexCheckBox->setChecked(sendAsHex);
    displaySendDataCheckBox->setChecked(displaySendData);
    displaySendDataAsHexCheckBox->setChecked(displaySendDataAsHex);
    autoSendCheckBox->setChecked(autoSend);
    sendIntervalLineEdit->setText(QString::number(autoSendInterval));

    frameInfoSettingCheckBox->setChecked(enableFrameInfo);
    byteCountLineEdit->setText(QString::number(fixByteCount));

    frameInfo = new FrameInfo(readFrameInfo());
}

void MainWindow::writeSettings() {

    qDebug() << "writeSettings()";

    QSettings settings("Zhou Jinlong", "Serial Wizard");

    settings.beginGroup("SerialSettings");
    settings.setValue("name", serialPortNameComboBox->currentIndex());
    settings.setValue("baud_rate", serialPortBaudRateComboBox->currentIndex());
    settings.setValue("data_bits", serialPortDataBitsComboBox->currentIndex());
    settings.setValue("stop_bits", serialPortStopBitsComboBox->currentIndex());
    settings.setValue("parity", serialPortParityComboBox->currentIndex());

    settings.setValue("send_text", sendTextEdit->toPlainText());

    settings.beginGroup("SerialReceiveSettings");
    settings.setValue("add_line_return", addLineReturnCheckBox->isChecked());
    settings.setValue("display_receive_data_as_hex", displaySendDataAsHexCheckBox->isChecked());
    settings.setValue("add_timestamp", addReceiveTimestampCheckBox->isChecked());

    settings.beginGroup("SerialSendSettings");
    settings.setValue("send_as_hex", sendHexCheckBox->isChecked());
    settings.setValue("display_send_data", displaySendDataCheckBox->isChecked());
    settings.setValue("display_send_data_as_hex", displaySendDataAsHexCheckBox->isChecked());
    settings.setValue("auto_send", autoSendCheckBox->isChecked());
    settings.setValue("auto_send_interval", sendIntervalLineEdit->text().toInt());

    settings.setValue("enable_frame_info", frameInfoSettingCheckBox->isChecked());

    settings.setValue("fix_byte_count", byteCountLineEdit->text().toInt());


    settings.sync();

}

FrameInfo MainWindow::readFrameInfo() const {
    QSettings settings("Zhou Jinlong", "Serial Wizard");

    settings.beginGroup("FrameInfo");

    FrameInfo info;

    info.frame = settings.value("frame",
                                "AA A0 5F 88 24 00 00 BC AE 04 90 05 09 15 DF 00 0A 14 D0 FB B4 AB D3 C3 D3 EF 31 31 C7 EB D5 BE CE C8 B7 F6 BA C3 02 09 05 1C D0 FB B4 AB D3 C3 D3 EF 31 32 B3 B5 C1 BE D7 AA CD E4 C7 EB D5 BE CE C8 B7 F6 BA C3 02 09 05 20 D0 FB B4 AB D3 C3 D3 EF 31 33 C7 EB B1 A3 B9 DC BA C3 CB E6 C9 ED D0 AF B4 F8 B5 C4 B2 C6 CE EF 02 09 05 1E D0 FB B4 AB D3 C3 D3 EF 31 34 BB B6 D3 AD B3 CB D7 F8 B1 BE B9 AB CB BE B9 B7 53 55").toString();
    info.head = (unsigned char) settings.value("head", 0xAA).toInt();
    info.end = (unsigned char) settings.value("end", 0x55).toInt();
    info.lenIndex = settings.value("len_index", 3).toInt();
    info.lenCount = settings.value("len_count", 1).toInt();
    info.headLen = settings.value("head_len", 5).toInt();
    info.endLen = settings.value("end_len", 3).toInt();

    return info;
}

void MainWindow::writeFrameInfo(const FrameInfo &info) const {
    QSettings settings("Zhou Jinlong", "Serial Wizard");

    settings.beginGroup("FrameInfo");

    settings.setValue("frame", info.frame);

    settings.setValue("head", info.head);
    settings.setValue("end", info.end);

    settings.setValue("len_index", info.lenIndex);
    settings.setValue("len_count", info.lenCount);

    settings.setValue("head_len", 5);
    settings.setValue("end_len", 3);

    settings.sync();
}

void MainWindow::clearReceivedData() {
    receiveDataBrowser->clear();
}

void MainWindow::saveReceivedData() {
    QString fileName = QFileDialog::getSaveFileName(this, tr("保存接收数据"),
                                                    "/", tr("Text (*.txt)"));
    if (fileName.isEmpty()) {
        return;
    }
    qDebug() << fileName;

    QFile::remove(fileName);

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly)) {
        QDataStream in(&file);

        in << receiveDataBrowser->toPlainText().toLocal8Bit();

        file.close();


        if (okToContinue(tr("消息"), tr("接收数据保存成功,是否打开所在文件夹？"))) {
            QProcess::startDetached("explorer.exe /select," + QDir::toNativeSeparators(fileName));
        }

    }
}

void MainWindow::clearSentData() {
    sendDataBrowser->clear();
}


void MainWindow::saveSentData() {
    QString fileName = QFileDialog::getSaveFileName(this, tr("保存发送数据"),
                                                    "/",
                                                    tr("Text (*.txt)"));
    if (fileName.isEmpty()) {
        return;
    }
    qDebug() << fileName;

    QFile::remove(fileName);

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly)) {
        QDataStream in(&file);

        in << sendDataBrowser->toPlainText().toLocal8Bit();

        file.close();

        if (okToContinue(tr("消息"), tr("发送数据保存成功,是否打开所在文件夹？"))) {
            QProcess::startDetached("explorer.exe /select," + QDir::toNativeSeparators(fileName));
        }

    }

}

void MainWindow::sendStatusMessage(const QString &msg) {
    statusBar()->showMessage(msg);
}

void MainWindow::updateSendCount(qint64 count) {
    statusBarSendByteCountLabel->setText(QString::number(count));
}

void MainWindow::updateReceiveCount(qint64 count) {
    statusBarReceiveByteCountLabel->setText(QString::number(count));
}

void MainWindow::receivedData(const QByteArray &data) {
    displayReceiveData(data);
    receiveCount += data.count();
    updateReceiveCount(receiveCount);
}

void MainWindow::sentData(const QByteArray &data) {
    displaySentData(data);
    sendCount += data.count();
    updateSendCount(sendCount);
}

void MainWindow::startAutoSendTimerIfNeed() {
    if (autoSendCheckBox->isChecked()) {
        autoSendTimer->start(sendIntervalLineEdit->text().toInt());
    }
}











