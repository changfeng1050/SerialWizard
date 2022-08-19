//
// Created by chang on 2017-07-28.
//
#include <QAction>
#include <QCheckBox>
#include <QDragEnterEvent>
#include <QDebug>
#include <QLineEdit>
#include <QMenu>
#include <QMenuBar>
#include <QtSerialPort/QSerialPort>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QGroupBox>
#include <QTextBrowser>
#include <QtWidgets/QFileDialog>
#include <QTimer>
#include <QtCore/QSettings>
#include <QtCore/QProcess>
#include <QStatusBar>
#include <QSplitter>
#include <data/SerialReadWriter.h>
#include <data/TcpServerReadWriter.h>
#include <data/TcpClientReadWriter.h>
#include <QRadioButton>
#include <QButtonGroup>
#include <data/BridgeReadWriter.h>
#include <QMimeData>
#include <QtSerialPort/QSerialPortInfo>
#include <data/SerialBridgeReadWriter.h>
#include <utils/FileUtil.h>
#include <QTextCodec>

#include "mainwindow.h"
#include "CalculateCheckSumDialog.h"
#include "global.h"

#include "serial/SerialController.h"
#include "serial/LineSerialController.h"
#include "ConvertDataDialog.h"
#include "DataProcessDialog.h"

QList<QNetworkInterface> getProperIpList() {
    auto ipList = getNetworkInterfaces();
    QList<QNetworkInterface> list;
    for (auto &ip: ipList) {
        if (ip.type() == QNetworkInterface::Wifi || ip.type() == QNetworkInterface::Ethernet) {
            auto address = getIpAddress(ip);
            if (!address.isEmpty() && !address.startsWith("169.254")) {
                list << ip;
            }
        }
    }
    return list;
}

QString getProperIp(const QString &currentAddress) {
    auto ipList = getProperIpList();
    qDebug() << "last tcp address:" << currentAddress;
    QString selectAddress = "";
    if (!currentAddress.isEmpty() && !ipList.isEmpty()) {
        for (const auto &ip: ipList) {
            if (getIpAddress(ip) == currentAddress) {
                selectAddress = currentAddress;
                break;
            }
        }
    }
    if (selectAddress.isEmpty() && !ipList.isEmpty()) {
        do {
            for (const auto &ip: ipList) {
                if (ip.type() == QNetworkInterface::Wifi && !getIpAddress(ip).isEmpty()) {
                    selectAddress = getIpAddress(ip);
                    break;
                }
            }
            if (!selectAddress.isEmpty()) {
                break;
            }
            for (const auto &ip: ipList) {
                if (ip.type() == QNetworkInterface::Ethernet && !getIpAddress(ip).isEmpty()) {
                    selectAddress = getIpAddress(ip);
                }
            }
            if (!selectAddress.isEmpty()) {
                break;
            }

            selectAddress = getIpAddress(ipList.first());
        } while (false);
    }

    return selectAddress;
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {

    init();
    initUi();
    setAcceptWindowDrops();
    createConnect();

    readSettings();
    createActions();
    createMenu();
    createStatusBar();

    updateStatusMessage(tr("就绪！"));
}

MainWindow::~MainWindow() {
    writeSettings();
    closeReadWriter();
}

void MainWindow::init() {
    autoSendTimer = new QTimer();
}

void MainWindow::initUi() {

    setWindowTitle(tr("串口调试工具"));

    setMinimumSize(1280, 900);

    serialRadioButton = new QRadioButton(tr("串口"), this);
    tcpServerRadioButton = new QRadioButton("TCP(服务器)", this);
    tcpClientRadioButton = new QRadioButton("TCP(客户端)", this);
    bridgeRadioButton = new QRadioButton(tr("桥接"), this);
    serialBridgeRadioButton = new QRadioButton(tr("转发"), this);

    serialRadioButton->setChecked(true);

    serialRadioButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    tcpServerRadioButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    tcpClientRadioButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    bridgeRadioButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    readWriterButtonGroup = new QButtonGroup;
    readWriterButtonGroup->addButton(serialRadioButton);
    readWriterButtonGroup->addButton(tcpServerRadioButton);
    readWriterButtonGroup->addButton(tcpClientRadioButton);
    readWriterButtonGroup->addButton(bridgeRadioButton);
    readWriterButtonGroup->addButton(serialBridgeRadioButton);

    auto readWriterButtonLayout = new QGridLayout;
    readWriterButtonLayout->addWidget(serialRadioButton, 0, 0);
    readWriterButtonLayout->addWidget(bridgeRadioButton, 0, 1);
    readWriterButtonLayout->addWidget(tcpServerRadioButton, 1, 0);
    readWriterButtonLayout->addWidget(tcpClientRadioButton, 1, 1);
    readWriterButtonLayout->addWidget(serialBridgeRadioButton, 2, 0);

    auto readWriterButtonGroupBox = new QGroupBox(tr("打开模式"));
    readWriterButtonGroupBox->setLayout(readWriterButtonLayout);

    auto serialPortNameLabel = new QLabel(tr("串口"), this);

    serialPortNameComboBox = new QComboBox(this);
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
                                                 << "25600"
                                                 << "57600"
                                                 << "115200"
                                                 << "256000"
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

    auto secondSerialPortNameLabel = new QLabel(tr("串口"), this);
    QStringList secondSerialPortNameList = getSerialNameList();

    secondSerialPortNameComboBox = new QComboBox(this);
    secondSerialPortNameLabel->setBuddy(secondSerialPortNameComboBox);

    auto *secondSerialPortBaudRateLabel = new QLabel(tr("波特率"), this);
    secondSerialPortBaudRateComboBox = new QComboBox(this);
    secondSerialPortBaudRateComboBox->addItems(QStringList()
                                                       << "1200"
                                                       << "2400"
                                                       << "4800"
                                                       << "9600"
                                                       << "19200"
                                                       << "38400"
                                                       << "25600"
                                                       << "57600"
                                                       << "115200"
                                                       << "256000"
    );
    secondSerialPortBaudRateLabel->setBuddy(secondSerialPortBaudRateComboBox);

    auto secondSerialPortDataBitsLabel = new QLabel(tr("数据位"), this);
    secondSerialPortDataBitsComboBox = new QComboBox(this);
    secondSerialPortDataBitsComboBox->addItems(QStringList() << "5" << "6" << "7" << "8");
    secondSerialPortDataBitsLabel->setBuddy(secondSerialPortDataBitsComboBox);

    auto secondSerialPortStopBitsLabel = new QLabel(tr("停止位"), this);
    secondSerialPortStopBitsComboBox = new QComboBox(this);
    secondSerialPortStopBitsLabel->setBuddy(secondSerialPortStopBitsComboBox);
    secondSerialPortStopBitsComboBox->addItem(tr("1"), QSerialPort::OneStop);
    secondSerialPortStopBitsComboBox->addItem(tr("1.5"), QSerialPort::OneAndHalfStop);
    secondSerialPortStopBitsComboBox->addItem(tr("2"), QSerialPort::TwoStop);

    auto secondSerialPortParityLabel = new QLabel(tr("校验位"), this);
    secondSerialPortParityComboBox = new QComboBox(this);
    secondSerialPortParityComboBox->addItem(tr("无校验"), QSerialPort::NoParity);
    secondSerialPortParityComboBox->addItem(tr("奇校验"), QSerialPort::OddParity);
    secondSerialPortParityComboBox->addItem(tr("偶校验"), QSerialPort::EvenParity);
    secondSerialPortParityComboBox->addItem(tr("空校验"), QSerialPort::SpaceParity);
    secondSerialPortParityComboBox->addItem(tr("标志校验"), QSerialPort::MarkParity);
    secondSerialPortParityLabel->setBuddy(secondSerialPortParityComboBox);

    auto secondSerialPortSettingsGridLayout = new QGridLayout;
    secondSerialPortSettingsGridLayout->addWidget(secondSerialPortNameLabel, 0, 0);
    secondSerialPortSettingsGridLayout->addWidget(secondSerialPortNameComboBox, 0, 1);
    secondSerialPortSettingsGridLayout->addWidget(secondSerialPortBaudRateLabel, 1, 0);
    secondSerialPortSettingsGridLayout->addWidget(secondSerialPortBaudRateComboBox, 1, 1);
    secondSerialPortSettingsGridLayout->addWidget(secondSerialPortDataBitsLabel, 2, 0);
    secondSerialPortSettingsGridLayout->addWidget(secondSerialPortDataBitsComboBox, 2, 1);
    secondSerialPortSettingsGridLayout->addWidget(secondSerialPortStopBitsLabel, 3, 0);
    secondSerialPortSettingsGridLayout->addWidget(secondSerialPortStopBitsComboBox, 3, 1);
    secondSerialPortSettingsGridLayout->addWidget(secondSerialPortParityLabel, 4, 0);
    secondSerialPortSettingsGridLayout->addWidget(secondSerialPortParityComboBox, 4, 1);

    auto firstSerialSettingsWidget = new QWidget();
    firstSerialSettingsWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    firstSerialSettingsWidget->setLayout(serialPortSettingsGridLayout);

    auto secondSerialSettingsWidget = new QWidget();
    secondSerialSettingsWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    secondSerialSettingsWidget->setLayout(secondSerialPortSettingsGridLayout);

    auto serialTabWidget = new QTabWidget(this);
    serialTabWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    serialTabWidget->addTab(firstSerialSettingsWidget, tr("第一串口设置"));
    serialTabWidget->addTab(secondSerialSettingsWidget, tr("第二串口设置"));

    openSerialButton = new QPushButton(tr("打开"), this);
    refreshSerialButton = new QPushButton(tr("刷新"), this);

    auto serialOpenRefreshLayout = new QHBoxLayout;
    serialOpenRefreshLayout->addWidget(openSerialButton);
    serialOpenRefreshLayout->addWidget(refreshSerialButton);

    tcpAddressComboBox = new QComboBox(this);
    tcpAddressComboBox->setEditable(true);
    tcpAddressComboBox->setMaximumWidth(120);
    tcpAddressComboBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Maximum);
    tcpPortLineEdit = new QLineEdit(this);
    tcpPortLineEdit->setAlignment(Qt::AlignCenter);
    tcpPortLineEdit->setMaximumWidth(50);
    tcpPortLineEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Maximum);

    tcpClientLabel = new QLabel(this);

    auto tcpEditLayout = new QHBoxLayout;
    tcpEditLayout->addWidget(tcpAddressComboBox);
    tcpEditLayout->addWidget(tcpPortLineEdit);
    auto tcpLayout = new QVBoxLayout;
    tcpLayout->addStretch();
    tcpLayout->addLayout(tcpEditLayout);
    tcpLayout->addWidget(tcpClientLabel);
    tcpLayout->addStretch();

    auto tcpGroupBox = new QGroupBox(tr("TCP设置"));
    tcpGroupBox->setLayout(tcpLayout);

    addLineReturnCheckBox = new QCheckBox(tr("自动换行"), this);
    displayReceiveDataAsHexCheckBox = new QCheckBox(tr("十六进制显示"), this);
    addReceiveTimestampCheckBox = new QCheckBox(tr("添加时间戳"), this);
    pauseReceiveCheckBox = new QCheckBox(tr("暂停接收"), this);
    saveReceiveDataButton = new QPushButton(tr("保存数据"), this);
    clearReceiveDataButton = new QPushButton(tr("清除显示"), this);

    auto receiveSettingLayout = new QGridLayout;
    receiveSettingLayout->addWidget(addLineReturnCheckBox, 0, 0);
    receiveSettingLayout->addWidget(displayReceiveDataAsHexCheckBox, 0, 1);
    receiveSettingLayout->addWidget(addReceiveTimestampCheckBox, 1, 0);
    receiveSettingLayout->addWidget(pauseReceiveCheckBox, 1, 1);

    receiveSettingLayout->addWidget(saveReceiveDataButton, 2, 0);
    receiveSettingLayout->addWidget(clearReceiveDataButton, 2, 1);

    auto receiveSettingGroupBox = new QGroupBox(tr("接收设置"));
    receiveSettingGroupBox->setLayout(receiveSettingLayout);

    receiveDataBrowser = new QTextBrowser(this);
    receiveDataBrowser->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    auto receiveDataLayout = new QVBoxLayout;
    receiveDataLayout->addWidget(receiveDataBrowser);
    auto receiveDataGroupBox = new QGroupBox(tr("数据接收显示"));
    receiveDataGroupBox->setLayout(receiveDataLayout);
    receiveDataGroupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    displaySendDataCheckBox = new QCheckBox(tr("显示发送数据"), this);
    displaySendDataAsHexCheckBox = new QCheckBox(tr("十六进制显示"), this);

    autoSendCheckBox = new QCheckBox(tr("自动发送"), this);
    auto sendIntervalLabel = new QLabel(tr("间隔(毫秒)"), this);
    sendIntervalLineEdit = new QLineEdit(this);
    sendIntervalLineEdit->setAlignment(Qt::AlignCenter);
    sendIntervalLineEdit->setMaximumWidth(50);
    sendIntervalLineEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Maximum);
    sendIntervalLabel->setBuddy(sendIntervalLineEdit);

    auto emptyLineDelayLabel = new QLabel(tr("空行间隔(毫秒)"), this);
    emptyLineDelayLindEdit = new QLineEdit(this);
    emptyLineDelayLindEdit->setAlignment(Qt::AlignCenter);
    emptyLineDelayLindEdit->setMaximumWidth(50);
    emptyLineDelayLindEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Maximum);
    emptyLineDelayLabel->setBuddy(emptyLineDelayLindEdit);

    auto emptyLineDelayLayout = new QHBoxLayout;
    emptyLineDelayLayout->addWidget(emptyLineDelayLabel);
    emptyLineDelayLayout->addWidget(emptyLineDelayLindEdit);

    auto autoSendLayout = new QHBoxLayout;
    autoSendLayout->addWidget(autoSendCheckBox);
    autoSendLayout->addWidget(sendIntervalLabel);
    autoSendLayout->addWidget(sendIntervalLineEdit);

    auto autoSendSettingsLayout = new QVBoxLayout;
    autoSendSettingsLayout->addLayout(autoSendLayout);
    autoSendSettingsLayout->addLayout(emptyLineDelayLayout);
    auto autoSendGroupBox = new QGroupBox("自动发送设置");
    autoSendGroupBox->setLayout(autoSendSettingsLayout);

    loopSendCheckBox = new QCheckBox(tr("循环发送"), this);
    resetLoopSendButton = new QPushButton(tr("重置计数"), this);
    currentSendCountLineEdit = new QLineEdit(this);
    currentSendCountLineEdit->setText("0");
    currentSendCountLineEdit->setAlignment(Qt::AlignCenter);
    currentSendCountLineEdit->setMaximumWidth(30);
    currentSendCountLineEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);

    auto currentSendCountLabel = new QLabel(tr("计数"), this);
    currentSendCountLabel->setBuddy(currentSendCountLineEdit);
    auto divideLabel = new QLabel(tr("/"), this);
    totalSendCountLabel = new QLabel(tr("0"), this);
    previousButton = new QPushButton(tr("⯇"), this);
    previousButton->setMaximumWidth(50);
    nextButton = new QPushButton(tr("⯈"), this);
    nextButton->setMaximumWidth(50);

    auto loopLayout1 = new QHBoxLayout;
    loopLayout1->addWidget(loopSendCheckBox);
    loopLayout1->addWidget(resetLoopSendButton);

    auto sendCountLayout = new QHBoxLayout;
    sendCountLayout->setAlignment(Qt::AlignLeft);
    sendCountLayout->addWidget(currentSendCountLabel);
    sendCountLayout->addWidget(currentSendCountLineEdit);
    sendCountLayout->addWidget(divideLabel);
    sendCountLayout->addWidget(totalSendCountLabel);
    sendCountLayout->addWidget(previousButton);
    sendCountLayout->addWidget(nextButton);

    auto loopSendLayout = new QVBoxLayout;
    loopSendLayout->addLayout(loopLayout1);
    loopSendLayout->addLayout(sendCountLayout);
    auto loopSendGroupBox = new QGroupBox(tr("循环发送设置"), this);
    loopSendGroupBox->setLayout(loopSendLayout);

    saveSentDataButton = new QPushButton(tr("保存数据"), this);
    clearSentDataButton = new QPushButton(tr("清除显示"), this);

    auto sendSettingLayout = new QGridLayout;
    sendSettingLayout->addWidget(displaySendDataCheckBox, 0, 0);
    sendSettingLayout->addWidget(displaySendDataAsHexCheckBox, 0, 1);
    sendSettingLayout->addWidget(saveSentDataButton, 1, 0);
    sendSettingLayout->addWidget(clearSentDataButton, 1, 1);

    auto sendSettingGroupBox = new QGroupBox(tr("发送设置"));
    sendSettingGroupBox->setLayout(sendSettingLayout);

    auto lineGroupBox = new QGroupBox(tr("按行发送"));
    sendLineButton = new QPushButton(tr("发送下一行"));
    auto lineLayout = new QVBoxLayout;
    lineLayout->addWidget(sendLineButton);
    lineGroupBox->setLayout(lineLayout);

    auto processTextLayout = new QHBoxLayout;
    processTextButton = new QPushButton(tr("数据处理"), this);
    clearTextButton = new QPushButton(tr("清空"), this);
    processTextLayout->addWidget(processTextButton);
    processTextLayout->addWidget(clearTextButton);

    hexCheckBox = new QCheckBox(tr("十六进制"), this);
    sendLineReturnCheckBox = new QCheckBox(tr("自动添加换行"), this);
    sendRNLineReturnButton = new QRadioButton(tr("\\r\\n"), this);
    sendRReturnLineButton = new QRadioButton(tr("\\r"), this);
    sendNReturnLineButton = new QRadioButton(tr("\\n"), this);

    lineReturnButtonGroup = new QButtonGroup(this);
    lineReturnButtonGroup->addButton(sendRNLineReturnButton);
    lineReturnButtonGroup->addButton(sendRReturnLineButton);
    lineReturnButtonGroup->addButton(sendNReturnLineButton);

    auto lineReturnLayout = new QHBoxLayout;
    lineReturnLayout->addWidget(sendRNLineReturnButton);
    lineReturnLayout->addWidget(sendRReturnLineButton);
    lineReturnLayout->addWidget(sendNReturnLineButton);

    auto optionSendLayout = new QVBoxLayout;
    optionSendLayout->addWidget(lineGroupBox);
    optionSendLayout->addLayout(processTextLayout);
    optionSendLayout->addWidget(sendLineReturnCheckBox);
    optionSendLayout->addLayout(lineReturnLayout);
    optionSendLayout->addWidget(hexCheckBox);

    optionSendLayout->setSizeConstraint(QLayout::SetFixedSize);

    sendDataBrowser = new QTextBrowser(this);
    sendDataBrowser->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto sendDataLayout = new QVBoxLayout;
    sendDataLayout->addWidget(sendDataBrowser);
    auto sendDataGroupBox = new QGroupBox(tr("数据发送显示"));
    sendDataGroupBox->setLayout(sendDataLayout);
    sendDataGroupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto dataBrowserSplitter = new QSplitter(this);
    dataBrowserSplitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    dataBrowserSplitter->addWidget(receiveDataGroupBox);
    dataBrowserSplitter->addWidget(sendDataGroupBox);

    sendTextEdit = new QTextEdit(this);
    sendTextEdit->setMinimumHeight(100);
    sendTextEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    auto sendLayout = new QHBoxLayout;
    sendLayout->addLayout(optionSendLayout);
    sendLayout->addWidget(sendTextEdit);

    sendLayout->setSizeConstraint(QLayout::SetFixedSize);

    auto mainVBoxLayout1 = new QVBoxLayout;
    mainVBoxLayout1->setSizeConstraint(QLayout::SetFixedSize);
    mainVBoxLayout1->addWidget(readWriterButtonGroupBox);
    mainVBoxLayout1->addWidget(serialTabWidget);
    mainVBoxLayout1->addWidget(tcpGroupBox);
    mainVBoxLayout1->addLayout(serialOpenRefreshLayout);
    mainVBoxLayout1->addWidget(receiveSettingGroupBox);
    mainVBoxLayout1->addWidget(sendSettingGroupBox);
    mainVBoxLayout1->addWidget(autoSendGroupBox);
    mainVBoxLayout1->addWidget(loopSendGroupBox);
    mainVBoxLayout1->addStretch();

    auto mainVBoxLayout2 = new QVBoxLayout;
    mainVBoxLayout2->addWidget(dataBrowserSplitter);
    mainVBoxLayout2->addLayout(sendLayout);

    auto widget = new QWidget(this);

    auto mainLayout = new QHBoxLayout;

    mainLayout->addLayout(mainVBoxLayout1);
    mainLayout->addLayout(mainVBoxLayout2);

    widget->setLayout(mainLayout);
    setCentralWidget(widget);
}

void MainWindow::setAcceptWindowDrops() {
    sendTextEdit->setAcceptDrops(false);
    sendDataBrowser->setAcceptDrops(false);
    receiveDataBrowser->setAcceptDrops(false);
    setAcceptDrops(true);
}

void MainWindow::createStatusBar() {

    auto receiveByteCountLabel = new QLabel(tr("接收:"), this);
    statusBarReadBytesLabel = new QLabel(this);
    statusBarReadBytesLabel->setAlignment(Qt::AlignCenter);
    statusBarReadBytesLabel->setMinimumWidth(50);
    statusBarReadBytesLabel->setText("0");

    auto sendByteCountLabel = new QLabel(tr("发送:"), this);
    statusBarWriteBytesLabel = new QLabel(this);
    statusBarWriteBytesLabel->setAlignment(Qt::AlignCenter);
    statusBarWriteBytesLabel->setMinimumWidth(50);
    statusBarWriteBytesLabel->setText("0");

    auto sendLineCountLabel = new QLabel(tr("发送行数:"), this);
    statusBarSendLinesLabel = new QLabel(this);
    statusBarSendLinesLabel->setAlignment(Qt::AlignCenter);
    statusBarSendLinesLabel->setMinimumWidth(50);
    statusBarSendLinesLabel->setText("0");

    statusBarResetCountButton = new QPushButton(tr("重置计数"), this);

    statusBar()->addPermanentWidget(receiveByteCountLabel);
    statusBar()->addPermanentWidget(statusBarReadBytesLabel);
    statusBar()->addPermanentWidget(sendByteCountLabel);
    statusBar()->addPermanentWidget(statusBarWriteBytesLabel);
    statusBar()->addPermanentWidget(sendLineCountLabel);
    statusBar()->addPermanentWidget(statusBarSendLinesLabel);
    statusBar()->addPermanentWidget(statusBarResetCountButton);

    connect(statusBarResetCountButton, &QPushButton::clicked, [this] {
        receiveCount = 0;
        sendCount = 0;
        totalSendLineCount = 0;
        emit writeBytesChanged(sendCount);
        emit readBytesChanged(receiveCount);
        emit writeLinesChanged(totalSendLineCount);
    });
}

void MainWindow::openReadWriter() {
    if (_readWriter != nullptr) {
        _readWriter->close();
        delete _readWriter;
        _readWriter = nullptr;
        emit serialStateChanged(false);
    }

    bool result;

    if (readWriterButtonGroup->checkedButton() == serialRadioButton) {
        _serialType = SerialType::Normal;
        auto settings = new SerialSettings();
        settings->name = serialPortNameComboBox->currentText();
        settings->baudRate = serialPortBaudRateComboBox->currentText().toInt();

        settings->dataBits = (QSerialPort::DataBits) serialPortDataBitsComboBox->currentText().toInt();
        settings->stopBits = (QSerialPort::StopBits) serialPortStopBitsComboBox->currentData().toInt();
        settings->parity = (QSerialPort::Parity) serialPortParityComboBox->currentData().toInt();
        auto readWriter = new SerialReadWriter(this);
        readWriter->setSerialSettings(*settings);
        qDebug() << settings->name << settings->baudRate << settings->dataBits << settings->stopBits
                 << settings->parity;
        result = readWriter->open();
        if (!result) {
            showWarning(tr("消息"), tr("串口被占用或者不存在"));
            return;
        }
        _readWriter = readWriter;
        _serialType = SerialType::Normal;
    } else if (readWriterButtonGroup->checkedButton() == tcpServerRadioButton) {
        _serialType = SerialType::TcpServer;
        auto address = tcpAddressComboBox->currentText();
        bool ok;
        auto port = tcpPortLineEdit->text().toInt(&ok);
        if (!ok) {
            showMessage("", tr("端口格式不正确"));
            return;
        }

        auto readWriter = new TcpServerReadWriter(this);
        readWriter->
                setAddress(address);
        readWriter->
                setPort(port);
        qDebug() << address << port;
        result = readWriter->open();
        if (!result) {
            showWarning("", tr("建立服务器失败"));
            return;
        }
        connect(readWriter, &TcpServerReadWriter::currentSocketChanged, this, &MainWindow::updateTcpClient);
        connect(readWriter, &TcpServerReadWriter::connectionClosed, this, &MainWindow::clearTcpClient);
        _readWriter = readWriter;
    } else if (readWriterButtonGroup->checkedButton() == tcpClientRadioButton) {
        _serialType = SerialType::TcpClient;
        auto address = tcpAddressComboBox->currentText();
        bool ok;
        auto port = tcpPortLineEdit->text().toInt(&ok);
        if (!ok) {
            showMessage("", tr("端口格式不正确"));
            return;
        }

        auto readWriter = new TcpClientReadWriter(this);
        readWriter->setAddress(address);
        readWriter->setPort(port);

        qDebug() << address << port;
        result = readWriter->open();
        if (!result) {
            showError("", tr("连接服务器失败"));
            return;
        }
        _readWriter = readWriter;
    } else if (readWriterButtonGroup->checkedButton() == serialBridgeRadioButton) {
        _serialType = SerialType::SerialBridge;

        auto settings1 = new SerialSettings();
        settings1->name = serialPortNameComboBox->currentText();
        settings1->baudRate = serialPortBaudRateComboBox->currentText().toInt();

        settings1->dataBits = (QSerialPort::DataBits) serialPortDataBitsComboBox->currentText().toInt();
        settings1->stopBits = (QSerialPort::StopBits) serialPortStopBitsComboBox->currentData().toInt();
        settings1->parity = (QSerialPort::Parity) serialPortParityComboBox->currentData().toInt();

        auto settings2 = new SerialSettings();
        settings2->name = secondSerialPortNameComboBox->currentText();
        settings2->baudRate = secondSerialPortBaudRateComboBox->currentText().toInt();

        settings2->dataBits = (QSerialPort::DataBits) secondSerialPortDataBitsComboBox->currentText().toInt();
        settings2->stopBits = (QSerialPort::StopBits) secondSerialPortStopBitsComboBox->currentText().toInt();
        settings2->parity = (QSerialPort::Parity) secondSerialPortParityComboBox->currentText().toInt();

        auto readWriter = new SerialBridgeReadWriter(this);

        readWriter->setSettings(*settings1, *settings2);
        result = readWriter->open();
        if (!result) {
            showWarning(tr("消息"), QString(tr("串口被占用或者不存在,%1")).arg(readWriter->settingsText()));
            return;
        }

        connect(readWriter, &SerialBridgeReadWriter::serial1DataRead, [this](const QByteArray &data) {
            showSendData(data);
        });

        connect(readWriter, &SerialBridgeReadWriter::serial2DataRead, [this](const QByteArray &data) {
            showReadData(data);
        });

        _readWriter = readWriter;
    } else {
        _serialType = SerialType::Bridge;
        auto settings = new SerialSettings();
        settings->name = serialPortNameComboBox->currentText();
        settings->baudRate = serialPortBaudRateComboBox->currentText().toInt();

        settings->dataBits = (QSerialPort::DataBits) serialPortDataBitsComboBox->currentText().toInt();
        settings->stopBits = (QSerialPort::StopBits) serialPortStopBitsComboBox->currentData().toInt();
        settings->parity = (QSerialPort::Parity) serialPortParityComboBox->currentData().toInt();

        auto address = tcpAddressComboBox->currentText();
        bool ok;
        auto port = tcpPortLineEdit->text().toInt(&ok);
        if (!ok) {
            showMessage("", tr("端口格式不正确"));
            return;
        }

        auto readWriter = new BridgeReadWriter(this);

        readWriter->setSettings(*settings, address, static_cast<qint16>(port));
        result = readWriter->open();
        if (!result) {
            showWarning(tr("消息"), tr("串口被占用或者不存在"));
            return;
        }

        connect(readWriter, &BridgeReadWriter::currentSocketChanged,
                this, &MainWindow::updateTcpClient);
        connect(readWriter, &BridgeReadWriter::connectionClosed,
                this, &MainWindow::clearTcpClient);
        connect(readWriter, &BridgeReadWriter::serialDataRead, [this](const QByteArray &data) {
            showSendData(data);
        });
        connect(readWriter, &BridgeReadWriter::tcpDataRead, [this](const QByteArray &data) {
            showReadData(data);
        });

        _readWriter = readWriter;
    }
    connect(_readWriter, &AbstractReadWriter::readyRead, this, &MainWindow::readData);

    emit serialStateChanged(result);
}

void MainWindow::closeReadWriter() {
    stopAutoSend();
    if (_readWriter != nullptr) {
        _readWriter->close();
        delete _readWriter;
        _readWriter = nullptr;
    }
    emit serialStateChanged(false);
}

void MainWindow::createConnect() {

    connect(readWriterButtonGroup, QOverload<QAbstractButton *, bool>::of(&QButtonGroup::buttonToggled),
            [=](QAbstractButton *button, bool checked) {
                if (checked && isReadWriterOpen()) {
                    SerialType serialType;
                    if (button == tcpServerRadioButton) {
                        serialType = SerialType::TcpServer;
                    } else if (button == tcpClientRadioButton) {
                        serialType = SerialType::TcpClient;
                    } else if (button == bridgeRadioButton) {
                        serialType = SerialType::Bridge;
                    } else {
                        serialType = SerialType::Normal;
                    }

                    if (serialType != _serialType) {
                        if (showWarning("", tr("串口配置已经改变，是否重新打开串口？"))) {
                            openReadWriter();
                        }
                    }
                }
            });

    connect(this, &MainWindow::serialStateChanged, [this](bool isOpen) {
        setOpenButtonText(isOpen);
        QString stateText;
        if (isOpen) {
            stateText = QString(tr("串口打开成功，%1")).arg(_readWriter->settingsText());
        } else {
            stateText = QString(tr("串口关闭"));
        }
        skipSendCount = 0;
        updateStatusMessage(stateText);
    });

    connect(this, &MainWindow::readBytesChanged, this, &MainWindow::updateReadBytes);
    connect(this, &MainWindow::writeBytesChanged, this, &MainWindow::updateWriteBytes);
    connect(this, &MainWindow::writeLinesChanged, this, &MainWindow::updateWriteLines);
    connect(this, &MainWindow::currentWriteCountChanged, this, &MainWindow::updateCurrentWriteCount);

    connect(openSerialButton, &QPushButton::clicked, [=](bool value) {
        if (!isReadWriterOpen()) {
            openReadWriter();
        } else {
            closeReadWriter();
        }
    });

    connect(refreshSerialButton, &QPushButton::clicked, [=] {
        _dirty = true;
        updateSerialPortNames();
    });

    connect(saveReceiveDataButton, &QPushButton::clicked, this, &MainWindow::saveReceivedData);
    connect(clearReceiveDataButton, &QPushButton::clicked, this, &MainWindow::clearReceivedData);

    connect(saveSentDataButton, &QPushButton::clicked, this, &MainWindow::saveSentData);
    connect(clearSentDataButton, &QPushButton::clicked, this, &MainWindow::clearSentData);

    connect(autoSendCheckBox, &QCheckBox::clicked, [this] {
        autoSendTimer->stop();
    });

    connect(loopSendCheckBox, &QCheckBox::stateChanged, [this] {
        _loopSend = loopSendCheckBox->isChecked();
    });

    connect(resetLoopSendButton, &QPushButton::clicked, [this] {
        skipSendCount = 0;
        serialController->setCurrentCount(0);
        emit currentWriteCountChanged(0);
    });

    connect(previousButton, &QPushButton::clicked, [this] {
        serialController->decreaseCurrentCount();
        updateCurrentWriteCount(serialController->getCurrentCount());
    });

    connect(nextButton, &QPushButton::clicked, [this] {
        serialController->increaseCurrentCount();
        updateCurrentWriteCount(serialController->getCurrentCount());
    });

    connect(currentSendCountLineEdit, &QLineEdit::editingFinished, [this] {
        bool ok;
        auto newCount = currentSendCountLineEdit->text().toInt(&ok);
        if (ok) {
            serialController->setCurrentCount(newCount);
        } else {
            currentSendCountLineEdit->setText(QString::number(serialController->getCurrentCount()));
        }
    });

    connect(sendLineButton, &QPushButton::clicked, [this] {
        if (!isReadWriterConnected()) {
            handlerSerialNotOpen();
            return;
        }

        if (autoSendState == AutoSendState::Sending) {
            stopAutoSend();
        } else {
            if (_dirty) {
                _dirty = false;
                _sendType = SendType::Line;
                updateSendData(hexCheckBox->isChecked(), sendTextEdit->toPlainText());
                updateSendType();
            }
            sendNextData();
            startAutoSendTimerIfNeed();
        }

        if (autoSendState == AutoSendState::Sending) {
            sendLineButton->setText(tr("停止"));
        } else {
            resetSendButtonText();
        }
    });

    connect(processTextButton, &QPushButton::clicked, [this] {
        openDataProcessDialog(sendTextEdit->toPlainText());
    });

    connect(clearTextButton, &QPushButton::clicked, [this] {
        sendTextEdit->clear();
    });

    connect(lineReturnButtonGroup, QOverload<QAbstractButton *, bool>::of(&QButtonGroup::buttonToggled),
            [=](QAbstractButton *button, bool checked) {
                if (checked) {
                    if (button == sendRReturnLineButton) {
                        lineReturn = QByteArray("\r");
                    } else if (button == sendNReturnLineButton) {
                        lineReturn = QByteArray("\n");
                    } else {
                        lineReturn = QByteArray("\r\n");
                    }
                }
            });

    connect(autoSendTimer, &QTimer::timeout,
            [this] {
                sendNextData();
            });
    connect(hexCheckBox, &QCheckBox::stateChanged, [this] {
        this->_dirty = true;
    });

    connect(sendTextEdit, &QTextEdit::textChanged, [this] {
        this->_dirty = true;
    });
}

void MainWindow::setOpenButtonText(bool isOpen) {
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

    validateDataAct = new QAction(tr("计算校验(&E)"), this);
    validateDataAct->setShortcut(tr("Ctrl+E"));
    validateDataAct->setStatusTip(tr("计算数据校验值"));
    connect(validateDataAct, &QAction::triggered, this, &MainWindow::openDataValidator);

    convertDataAct = new QAction(tr("字符编码(&T)"));
    convertDataAct->setShortcut(tr("Ctrl+T"));
    convertDataAct->setStatusTip(tr("字符编码"));
    connect(convertDataAct, &QAction::triggered, this, &MainWindow::openConvertDataDialog);

    dataProcessAct = new QAction(tr("数据处理(&P)"));
    dataProcessAct->setShortcut(tr("Ctrl+P"));
    dataProcessAct->setStatusTip(tr("数据处理"));
    connect(dataProcessAct, &QAction::triggered, [this] {
        openDataProcessDialog("");
    });
}

void MainWindow::createMenu() {
    fileMenu = menuBar()->addMenu(tr("文件(&F)"));
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);

    toolMenu = menuBar()->addMenu(tr("工具(&T)"));
    toolMenu->addAction(validateDataAct);
    toolMenu->addAction(convertDataAct);
    toolMenu->addAction(dataProcessAct);
}

void MainWindow::open() {
    auto lastDir = runConfig->lastDir;
    QString fileName = QFileDialog::getOpenFileName(this, tr("打开数据文件"), lastDir, "");
    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly)) {
        runConfig->lastDir = getFileDir(fileName);
        auto data = file.readAll();
        sendTextEdit->setText(QString::fromLocal8Bit(data));
    }
}

void MainWindow::save() {
    saveReceivedData();
}

void MainWindow::openDataValidator() {
    CalculateCheckSumDialog dialog(this);
    dialog.setModal(true);
    dialog.exec();
}

void MainWindow::openConvertDataDialog() {
    ConvertDataDialog dialog(this);
    dialog.setModal(true);
    dialog.exec();
}

void MainWindow::openDataProcessDialog(const QString &text) {
    DataProcessDialog dialog(text, this);
    dialog.setModal(true);

    int result = dialog.exec();
    if (result == QDialog::Accepted) {
        sendTextEdit->setText(dialog.text());
    }
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

    if (!s.isEmpty()) {
        s.append(" ");
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

void MainWindow::sendNextData() {
    if (isReadWriterConnected()) {
        if (skipSendCount > 0) {
            auto delay = skipSendCount * sendIntervalLineEdit->text().toInt();
            updateStatusMessage(QString("%1毫秒后发送下一行").arg(delay));
            skipSendCount--;
            return;
        }

        qDebug() << "sendNextData readEnd:" << serialController->readEnd() << "current:"
                 << serialController->getCurrentCount();
        if (!_loopSend && autoSendCheckBox->isChecked() && serialController->readEnd()) {
            serialController->setCurrentCount(0);
            stopAutoSend();
            return;
        }
        auto data = serialController->readNextFrame();
        if (data.isEmpty()) {
            updateStatusMessage(tr("空行，不发送"));
            if (autoSendCheckBox->isChecked()) {
                auto emptyDelay = emptyLineDelayLindEdit->text().toInt();
                auto sendInterval = sendIntervalLineEdit->text().toInt();
                if (emptyDelay > sendInterval) {
                    skipSendCount = emptyDelay / sendInterval;
                    if (emptyDelay % sendInterval != 0) {
                        skipSendCount += 1;
                    }
                    skipSendCount--;
                    updateStatusMessage(QString(tr("空行,%1毫秒后发送下一行")).arg(emptyDelay));
                }
            }
            emit currentWriteCountChanged(serialController->getCurrentCount());
            return;
        }
        writeData(data);
        if (sendLineReturnCheckBox->isChecked()) {
            writeData(lineReturn);
        }
        if (hexCheckBox->isChecked()) {
            updateStatusMessage(QString(tr("发送 %1")).arg(QString(dataToHex(data))));
        } else {
            updateStatusMessage(QString(tr("发送 %1")).arg(QString(data)));
        }
        emit currentWriteCountChanged(serialController->getCurrentCount());
    } else {
        handlerSerialNotOpen();
    }
}

void MainWindow::updateSendData(bool isHex, const QString &text) {
    if (serialController != nullptr) {
        QStringList lines = getLines(text);
        QList<QByteArray> dataList;
        if (isHex) {
            for (auto &line: lines) {
                dataList << dataFromHex(line);
            }
        } else {
            for (auto &line: lines) {
                dataList << line.toLocal8Bit();
            }
        }
        serialController->setData(dataList);
        auto totalSendCount = serialController->getTotalCount();
        updateTotalSendCount(totalSendCount);
    }
}

void MainWindow::readSettings() {

    qDebug() << "readSettings";

    updateSerialPortNames();

    QSettings settings("Zhou Jinlong", "Serial Wizard");

    settings.beginGroup("Basic");
    auto serialType = SerialType(settings.value("serial_type", static_cast<int >(SerialType::Normal)).toInt());
    if (serialType == SerialType::TcpServer) {
        tcpServerRadioButton->setChecked(true);
    } else if (serialType == SerialType::TcpClient) {
        tcpClientRadioButton->setChecked(true);
    } else if (serialType == SerialType::Bridge) {
        bridgeRadioButton->setChecked(true);
    } else if (serialType == SerialType::SerialBridge) {
        serialBridgeRadioButton->setChecked(true);
    } else {
        serialRadioButton->setChecked(true);
    }

    _serialType = serialType;

    settings.beginGroup("SerialSettings");
    auto nameIndex = settings.value("name", 0).toInt();
    auto baudRateIndex = settings.value("baud_rate", 5).toInt();
    auto dataBitsIndex = (QSerialPort::DataBits) settings.value("data_bits", 3).toInt();
    auto stopBitsIndex = (QSerialPort::StopBits) settings.value("stop_bits", 0).toInt();
    auto parityIndex = (QSerialPort::Parity) settings.value("parity", 0).toInt();
    auto sendText = settings.value("send_text", "").toString();

    auto maxCount = serialPortNameComboBox->maxCount();
    if (nameIndex > maxCount - 1) {
        nameIndex = 0;
    }
    serialPortNameComboBox->setCurrentIndex(nameIndex);
    serialPortBaudRateComboBox->setCurrentIndex(baudRateIndex);
    serialPortDataBitsComboBox->setCurrentIndex(dataBitsIndex);
    serialPortStopBitsComboBox->setCurrentIndex(stopBitsIndex);
    serialPortParityComboBox->setCurrentIndex(parityIndex);

    auto name2Index = settings.value("name2", 0).toInt();
    auto baudRate2Index = settings.value("baud_rate2", 5).toInt();
    auto dataBits2Index = (QSerialPort::DataBits) settings.value("data_bits2", 3).toInt();
    auto stopBits2Index = (QSerialPort::StopBits) settings.value("stop_bits2", 0).toInt();
    auto parity2Index = (QSerialPort::Parity) settings.value("parity2", 0).toInt();

    auto maxCount2 = serialPortNameComboBox->maxCount();
    if (name2Index > maxCount2 - 1) {
        name2Index = 0;
    }
    secondSerialPortNameComboBox->setCurrentIndex(name2Index);
    secondSerialPortBaudRateComboBox->setCurrentIndex(baudRate2Index);
    secondSerialPortDataBitsComboBox->setCurrentIndex(dataBits2Index);
    secondSerialPortStopBitsComboBox->setCurrentIndex(stopBits2Index);
    secondSerialPortParityComboBox->setCurrentIndex(parity2Index);

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
    auto emptyLineDelay = settings.value("empty_line_delay", 0).toInt();

    auto loopSend = settings.value("loop_send", false).toBool();

    hexCheckBox->setChecked(sendAsHex);
    displaySendDataCheckBox->setChecked(displaySendData);
    displaySendDataAsHexCheckBox->setChecked(displaySendDataAsHex);
    autoSendCheckBox->setChecked(autoSend);
    loopSendCheckBox->setChecked(loopSend);
    sendIntervalLineEdit->setText(QString::number(autoSendInterval));
    emptyLineDelayLindEdit->setText(QString::number(emptyLineDelay));

    auto sendLineReturn = settings.value("send_line_return", false).toBool();
    sendLineReturnCheckBox->setChecked(sendLineReturn);

    auto sendLineReturnType = LineReturn(
            settings.value("send_line_return_type", static_cast<int >(LineReturn::RN)).toInt());
    if (sendLineReturnType == LineReturn::R) {
        sendRReturnLineButton->setChecked(true);
    } else if (sendLineReturnType == LineReturn::N) {
        sendNReturnLineButton->setChecked(true);
    } else {
        sendRNLineReturnButton->setChecked(true);
    }

    auto ipList = getProperIpList();
    tcpAddressComboBox->clear();
    QStringList ipAddressList;
    for (auto &ip: ipList) {
        ipAddressList << getIpAddress(ip);
    }
    tcpAddressComboBox->addItems(ipAddressList);

    settings.beginGroup("TcpSettings");
    auto ipAddress = settings.value("tcp_address", "").toString();
    qDebug() << "last tcp address:" << ipAddress;
    auto selectAddress = getProperIp(ipAddress);
    tcpAddressComboBox->setCurrentText(selectAddress);

    auto tcpPort = settings.value("tcp_port").toInt();
    tcpPortLineEdit->setText(QString::number(tcpPort));

    sendTextEdit->setText(sendText);
    updateTotalSendCount(getLines(sendText).count());

    settings.beginGroup("RunConfig");
    auto lastDir = settings.value("last_dir", "").toString();
    auto lastFilePath = settings.value("last_file_path", "").toString();

    runConfig = new RunConfig;
    runConfig->lastDir = lastDir;
    runConfig->lastFilePath = lastFilePath;

    _loopSend = loopSend;

    serialController = new LineSerialController();

    updateSendType();
}

void MainWindow::writeSettings() {

    qDebug() << "writeSettings()";

    QSettings settings("Zhou Jinlong", "Serial Wizard");

    settings.beginGroup("Basic");
    SerialType serialType;

    if (tcpServerRadioButton->isChecked()) {
        serialType = SerialType::TcpServer;
    } else if (tcpClientRadioButton->isChecked()) {
        serialType = SerialType::TcpClient;
    } else if (bridgeRadioButton->isChecked()) {
        serialType = SerialType::Bridge;
    } else if (serialBridgeRadioButton->isChecked()) {
        serialType = SerialType::SerialBridge;
    } else {
        serialType = SerialType::Normal;
    }

    settings.setValue("serial_type", static_cast<int >(serialType));

    settings.beginGroup("SerialSettings");
    settings.setValue("name", serialPortNameComboBox->currentIndex());
    settings.setValue("baud_rate", serialPortBaudRateComboBox->currentIndex());
    settings.setValue("data_bits", serialPortDataBitsComboBox->currentIndex());
    settings.setValue("stop_bits", serialPortStopBitsComboBox->currentIndex());
    settings.setValue("parity", serialPortParityComboBox->currentIndex());

    settings.setValue("name2", secondSerialPortNameComboBox->currentIndex());
    settings.setValue("baud_rate2", secondSerialPortBaudRateComboBox->currentIndex());
    settings.setValue("data_bits2", secondSerialPortDataBitsComboBox->currentIndex());
    settings.setValue("stop_bits2", secondSerialPortStopBitsComboBox->currentIndex());
    settings.setValue("parity2", secondSerialPortParityComboBox->currentIndex());

    settings.setValue("send_text", sendTextEdit->toPlainText());

    settings.beginGroup("SerialReceiveSettings");
    settings.setValue("add_line_return", addLineReturnCheckBox->isChecked());
    settings.setValue("display_receive_data_as_hex", displayReceiveDataAsHexCheckBox->isChecked());
    settings.setValue("add_timestamp", addReceiveTimestampCheckBox->isChecked());

    settings.beginGroup("SerialSendSettings");
    settings.setValue("send_as_hex", hexCheckBox->isChecked());
    settings.setValue("display_send_data", displaySendDataCheckBox->isChecked());
    settings.setValue("display_send_data_as_hex", displaySendDataAsHexCheckBox->isChecked());
    settings.setValue("auto_send", autoSendCheckBox->isChecked());
    settings.setValue("auto_send_interval", sendIntervalLineEdit->text().toInt());
    settings.setValue("empty_line_delay", emptyLineDelayLindEdit->text().toInt());
    settings.setValue("loop_send", loopSendCheckBox->isChecked());
    settings.setValue("send_line_return", sendLineReturnCheckBox->isChecked());

    LineReturn sendLineReturn;
    if (sendRNLineReturnButton->isChecked()) {
        sendLineReturn = LineReturn::RN;
    } else if (sendRReturnLineButton->isChecked()) {
        sendLineReturn = LineReturn::R;
    } else if (sendNReturnLineButton->isChecked()) {
        sendLineReturn = LineReturn::N;
    } else {
        sendLineReturn = LineReturn::RN;
    }

    settings.setValue("send_line_return_type", static_cast<int >(sendLineReturn));

    settings.beginGroup("TcpSettings");
    settings.setValue("tcp_address", tcpAddressComboBox->currentText());
    settings.setValue("tcp_port", tcpPortLineEdit->text().toInt());

    settings.beginGroup("RunConfig");
    settings.setValue("last_dir", runConfig->lastDir);
    settings.setValue("last_file_path", runConfig->lastFilePath);

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
    auto lastDir = runConfig->lastDir;
    QString fileName = QFileDialog::getSaveFileName(this, tr("保存发送数据"),
                                                    lastDir,
                                                    tr("Text (*.txt)"));
    if (fileName.isEmpty()) {
        return;
    }
    qDebug() << fileName;

    QFile::remove(fileName);

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly)) {

        runConfig->lastDir = getFileDir(fileName);

        QDataStream in(&file);

        in << sendDataBrowser->toPlainText().toLocal8Bit();

        file.close();

        if (okToContinue(tr("消息"), tr("发送数据保存成功,是否打开所在文件夹？"))) {
            QProcess::startDetached("explorer.exe /select," + QDir::toNativeSeparators(fileName));
        }
    }
}

void MainWindow::updateSerialPortNames() {
    auto serialPortNameList = getSerialNameList();
    serialPortNameComboBox->clear();
    serialPortNameComboBox->addItems(serialPortNameList);

    secondSerialPortNameComboBox->clear();
    secondSerialPortNameComboBox->addItems(serialPortNameList);

    auto ipList = getProperIpList();
    tcpAddressComboBox->clear();
    QStringList ipAddressList;
    for (auto &ip: ipList) {
        ipAddressList << getIpAddress(ip);
    }
    tcpAddressComboBox->addItems(ipAddressList);
    auto ip = getProperIp("");
    tcpAddressComboBox->setCurrentText(ip);
}

void MainWindow::updateReceiveCount(qint64 count) {
    statusBarReadBytesLabel->setText(QString::number(count));
}

void MainWindow::readData() {
    auto data = _readWriter->readAll();
    if (!data.isEmpty()) {
        displayReceiveData(data);
        receiveCount += data.count();
        updateReceiveCount(receiveCount);
        emit readBytesChanged(receiveCount);
    }
}

qint64 MainWindow::writeData(const QByteArray &data) {
    if (!data.isEmpty() && isReadWriterConnected()) {
        auto count = _readWriter->write(data);
        displaySentData(data);
        sendCount += count;
        emit writeBytesChanged(sendCount);
        totalSendLineCount++;
        emit writeLinesChanged(totalSendLineCount);
        return count;
    }
    return 0;
}

void MainWindow::startAutoSendTimerIfNeed() {
    if (autoSendCheckBox->isChecked()) {
        autoSendTimer->start(sendIntervalLineEdit->text().toInt());
        autoSendState = AutoSendState::Sending;
    } else {
        autoSendState = AutoSendState::Finish;
    }
}

void MainWindow::handlerSerialNotOpen() {
    autoSendTimer->stop();
    if (_serialType == SerialType::TcpServer) {
        showMessage(tr("消息"), tr("TCP服务器未建立或者没有客户端连接"));
    } else if (_serialType == SerialType::TcpClient) {
        showMessage(tr("消息"), tr("没有连接到服务器"));
    } else if (_serialType == SerialType::Bridge) {
        showMessage(tr("消息"), tr("串口未打开，或者TCP服务器为建立，或者没有客户端连接"));
    } else {
        showMessage(tr("消息"), tr("串口未打开，请打开串口"));
    }
}

void MainWindow::updateStatusMessage(const QString &message) {
    statusBar()->showMessage(message);
}

void MainWindow::updateReadBytes(qint64 bytes) {
    statusBarReadBytesLabel->setText(QString::number(bytes));
}

void MainWindow::updateWriteBytes(qint64 bytes) {
    statusBarWriteBytesLabel->setText(QString::number(bytes));
}

void MainWindow::updateWriteLines(qint64 lines) {
    statusBarSendLinesLabel->setText(QString::number(lines));
}

void MainWindow::stopAutoSend() {
    autoSendTimer->stop();
    autoSendState = AutoSendState::Finish;

    resetSendButtonText();
}

void MainWindow::resetSendButtonText() {
    sendLineButton->setText(tr("发送下一行"));
}

void MainWindow::updateTotalSendCount(qint64 count) {
    totalSendCountLabel->setText(QString::number(count));
}

bool MainWindow::isReadWriterOpen() {
    return _readWriter != nullptr && _readWriter->isOpen();
}

void MainWindow::updateCurrentWriteCount(qint64 count) {
    currentSendCountLineEdit->setText(QString::number(count));
}

bool MainWindow::isReadWriterConnected() {
    return _readWriter != nullptr && _readWriter->isConnected();
}

void MainWindow::updateTcpClient(const QString &address, qint16 port) {
    auto clientPort = static_cast<uint>(port) & 0x00FFFF;
    tcpClientLabel->setText(QString("client %1:%2").arg(address).arg(clientPort));
    updateStatusMessage(QString(tr("与TCP客户端 %1:%2建立连接")).arg(address).arg(clientPort));
}

void MainWindow::clearTcpClient() {
    tcpClientLabel->clear();
    updateStatusMessage(QString(QString("TCP客户端已断开")));
}

void MainWindow::showReadData(const QByteArray &data) {
    if (!data.isEmpty()) {
        displayReceiveData(data);
        receiveCount += data.count();
        updateReceiveCount(receiveCount);
        emit readBytesChanged(receiveCount);
    }
}

void MainWindow::showSendData(const QByteArray &data) {
    if (!data.isEmpty() && isReadWriterConnected()) {
        displaySentData(data);
        sendCount += data.count();
        emit writeBytesChanged(sendCount);
        totalSendLineCount++;
        emit writeLinesChanged(totalSendLineCount);
    }
}

QStringList MainWindow::getSerialNameList() {

    auto serialPortInfoList = QSerialPortInfo::availablePorts();
    QStringList l;
    for (auto &s: serialPortInfoList) {
        l.append(s.portName());
    }
    return l;
}

void MainWindow::updateSendType() {

    if (serialController->sendType() == _sendType) {
        return;
    }

    SerialController *newController = nullptr;

    if (_sendType == SendType::Line) {
        newController = new LineSerialController(serialController);
    }
    serialController = newController;
}

void MainWindow::dropEvent(QDropEvent *event) {
    auto urls = event->mimeData()->urls();
    if (urls.isEmpty()) {
        event->ignore();
        return;
    }

    QStringList filePathList;

    QString filePath;
    for (auto &url: urls) {
        auto fileInfo = QFileInfo(url.toLocalFile());
        if (isTextFile(fileInfo.fileName())) {
            filePath = fileInfo.absoluteFilePath();
            break;
        }
    }

    qDebug() << "accept file:" << filePath;

    if (!filePath.isEmpty()) {
        auto text = readFromFile(filePath);
        sendTextEdit->setText(text);
    } else {
        event->ignore();
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasFormat("text/uri-list")) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}
