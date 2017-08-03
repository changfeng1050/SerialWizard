//
// Created by chang on 2017-08-02.
//

#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include "FrameInfoDialog.h"

FrameInfoDialog::FrameInfoDialog(const FrameInfo &info,QWidget *parent, Qt::WindowFlags f)
        : QDialog(parent, f),info(info){
    createUi();
    initUiValue(info);
    createConnect();
}

void FrameInfoDialog::createUi() {

    setMinimumWidth(800);

    auto frameLabel = new QLabel(tr("数据帧样例"), this);
    frameLineEdit = new QLineEdit(this);
    frameLabel->setBuddy(frameLineEdit);

    auto hLayout1 = new QHBoxLayout;
    hLayout1->addWidget(frameLabel);
    hLayout1->addWidget(frameLineEdit);

    auto frameHeadLabel = new QLabel(tr("帧头"), this);
    frameHeadLineEdit = new QLineEdit(this);
    frameHeadLineEdit->setMaximumWidth(50);
    frameHeadLabel->setBuddy(frameHeadLineEdit);

    auto frameEndLabel = new QLabel(tr("帧尾"), this);
    frameEndLineEdit = new QLineEdit(this);
    frameEndLineEdit->setMaximumWidth(50);
    frameEndLabel->setBuddy(frameEndLineEdit);


    auto hLayout2 = new QHBoxLayout;
    hLayout2->addWidget(frameHeadLabel);
    hLayout2->addWidget(frameHeadLineEdit);
    hLayout2->addWidget(frameEndLabel);
    hLayout2->addWidget(frameEndLineEdit);
    hLayout2->addStretch();

    auto frameLenIndexLabel = new QLabel(tr("帧长度标识索引"), this);
    frameLenIndexLineEdit = new QLineEdit(this);
    frameLenIndexLineEdit->setMaximumWidth(50);
    frameLenIndexLabel->setBuddy(frameLenIndexLineEdit);

    auto frameLenCountLabel = new QLabel(tr("帧长度标识的长度"), this);
    frameLenCountComboBox = new QComboBox(this);
    frameLenCountComboBox->addItems(QStringList() << "1" << "2");
    frameEndLabel->setBuddy(frameLenCountComboBox);

    auto frameHeadLenLabel = new QLabel(tr("帧头部长度"), this);
    frameHeadLenLineEdit = new QLineEdit(this);
    frameHeadLenLineEdit->setMaximumWidth(50);
    frameHeadLenLabel->setBuddy(frameHeadLenLineEdit);

    auto frameEndLenLabel = new QLabel(tr("帧尾部长度"), this);
    frameEndLenLineEdit = new QLineEdit(this);
    frameEndLenLineEdit->setMaximumWidth(50);
    frameEndLabel->setBuddy(frameEndLenLineEdit);

    auto hLayout3 = new QHBoxLayout;
    hLayout3->addWidget(frameLenIndexLabel);
    hLayout3->addWidget(frameLenIndexLineEdit);
    hLayout3->addWidget(frameLenCountLabel);
    hLayout3->addWidget(frameLenCountComboBox);
    hLayout3->addWidget(frameHeadLenLabel);
    hLayout3->addWidget(frameHeadLenLineEdit);
    hLayout3->addWidget(frameEndLenLabel);
    hLayout3->addWidget(frameEndLenLineEdit);
    hLayout3->addStretch();

    okButton = new QPushButton(tr("确认"), this);
    cancelButton = new QPushButton(tr("取消"), this);

    auto hLayout4 = new QHBoxLayout;
    hLayout4->addStretch();
    hLayout4->addWidget(okButton);
    hLayout4->addWidget(cancelButton);

    auto mainLayout = new QVBoxLayout;
    mainLayout->addLayout(hLayout1);
    mainLayout->addLayout(hLayout2);
    mainLayout->addLayout(hLayout3);
    mainLayout->addStretch();
    mainLayout->addLayout(hLayout4);

    setLayout(mainLayout);
}


void FrameInfoDialog::initUiValue(FrameInfo info) {

    frameLineEdit->setText(info.frame);

    frameHeadLineEdit->setText(QString::number(info.head, 16).toUpper());
    frameEndLineEdit->setText(QString::number(info.end, 16).toUpper());

    frameLenIndexLineEdit->setText(QString::number(info.lenIndex));
    frameLenCountComboBox->setCurrentText(QString::number(info.lenCount));

    frameHeadLenLineEdit->setText(QString::number(info.headLen));
    frameEndLenLineEdit->setText(QString::number(info.endLen));
}

void FrameInfoDialog::createConnect() {
    connect(okButton, &QPushButton::clicked, [this] {
        FrameInfo info = FrameInfo();
        info.frame = frameLineEdit->text();
        info.head = (unsigned char) frameHeadLineEdit->text().toInt(nullptr, 16);
        info.end = (unsigned char) frameEndLineEdit->text().toInt(nullptr, 16);
        info.lenIndex = frameLenIndexLineEdit->text().toInt();
        info.lenCount = frameLenCountComboBox->currentText().toInt();
        info.headLen = frameHeadLenLineEdit->text().toInt();
        info.endLen = frameEndLenLineEdit->text().toInt();
        emit frameInfoChanged(info);
        accept();
    });
    connect(cancelButton, &QPushButton::clicked, this, &FrameInfoDialog::reject);
}

