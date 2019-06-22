//
// Created by chang on 2017-07-31.
//

#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QtWidgets/QHBoxLayout>
#include <QDebug>
#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>
#include <QTextEdit>

#include "ConvertDataDialog.h"
#include "ConvertDataDialog.h"
#include "global.h"

const QString GBK = "GBK";
const QString UTF8 = "UTF8";

ConvertDataDialog::ConvertDataDialog(QWidget *parent, Qt::WindowFlags f) {

    createUi();
    createConnect();
}

void ConvertDataDialog::createUi() {

    inputTextEdit = new QTextEdit(this);
    inputTextEdit->setMinimumSize(600, 200);

    hexCheckBox = new QCheckBox(tr("HEX"), this);
    hexCheckBox->setChecked(true);

    fromCodecComboBox = new QComboBox(this);
    fromCodecComboBox->addItems(QStringList() << UTF8 << GBK);
    exchangeCodecButton = new QPushButton(tr("<<=>>"), this);
    toCodecComboBox = new QComboBox(this);
    toCodecComboBox->addItems(QStringList() << GBK << UTF8);

    parseButton = new QPushButton(tr("解析"), this);
    convertButton = new QPushButton(tr("转换"), this);

    auto convertLayout = new QHBoxLayout;
    convertLayout->addWidget(hexCheckBox);
    convertLayout->addStretch();
    convertLayout->addWidget(fromCodecComboBox);
    convertLayout->addWidget(exchangeCodecButton);
    convertLayout->addWidget(toCodecComboBox);
    convertLayout->addStretch();
    convertLayout->addWidget(parseButton);
    convertLayout->addWidget(convertButton);

    resultTextEdit = new QTextEdit(this);
    resultTextEdit->setMinimumSize(600, 200);

    auto layout = new QVBoxLayout;
    layout->addWidget(inputTextEdit);
    layout->addLayout(convertLayout);
    layout->addWidget(resultTextEdit);

    layout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(layout);
    setWindowTitle(tr("数据转换"));
}

void ConvertDataDialog::createConnect() {

    connect(exchangeCodecButton, &QPushButton::clicked, [this] {
        auto fromCodec = fromCodecComboBox->currentText();
        auto toCodec = toCodecComboBox->currentText();

        fromCodecComboBox->setCurrentText(toCodec);
        toCodecComboBox->setCurrentText(fromCodec);
    });

    connect(parseButton, &QPushButton::clicked, [this] {
        auto inputText = inputTextEdit->toPlainText();
        auto hex = hexCheckBox->isChecked();
        auto fromCodec = fromCodecComboBox->currentText();
        auto toCodec = toCodecComboBox->currentText();
        if (inputText.isEmpty()) {
            showWarning("", QString(tr("请输入内容！")));
            return;
        }
        if (hex) {
            auto byteArray = dataFromHex(inputText);

            QString resultText;

            if (fromCodec == GBK) {
                resultText = fromGbk(byteArray);
            } else if (fromCodec == UTF8) {
                resultText = fromUtf8(byteArray);
            }
            if (resultText.isEmpty()) {
                showWarning("", QString(tr("请检查输入数据是否有误！")));
                return;
            }
            resultTextEdit->setText(resultText);
        } else {
            const auto &text = inputText;

            QString resultText;
            if (fromCodec == GBK) {
                resultText = toGbkByteArray(text).toHex(' ').toUpper();
            } else {
                resultText = toUtf8ByteArray(text).toHex(' ').toUpper();
            }
            if (resultText.isEmpty()) {
                showWarning("", QString(tr("请检查输入数据是否有误！")));
                return;
            }
            resultTextEdit->setText(resultText);
        }
    });

    connect(convertButton, &QPushButton::clicked, [this] {
        auto inputText = inputTextEdit->toPlainText();
        auto hex = hexCheckBox->isChecked();
        auto fromCodec = fromCodecComboBox->currentText();
        auto toCodec = toCodecComboBox->currentText();

        if (hex) {
            auto byteArray = dataFromHex(inputText);

            QString text;

            if (fromCodec == GBK) {
                text = fromGbk(byteArray);
            } else if (fromCodec == UTF8) {
                text = fromUtf8(byteArray);
            }

            if (text.isEmpty()) {
                showWarning("", QString(tr("请检查输入数据是否有误！")));
                return;
            }

            QString convertedText;
            if (toCodec == GBK) {
                convertedText = toGbkByteArray(text).toHex(' ').toUpper();
            } else {
                convertedText = toUtf8ByteArray(text).toHex(' ').toUpper();
            }

            if (convertedText.isEmpty()) {
                showWarning("", QString(tr("请检查输入数据是否有误！")));
                return;
            }

            resultTextEdit->setText(convertedText);
        } else {
            const auto &text = inputText;

            QString resultText;
            if (toCodec == GBK) {
                resultText = toGbkByteArray(text).toHex(' ').toUpper();
            } else {
                resultText = toUtf8ByteArray(text).toHex(' ').toUpper();
            }
            if (resultText.isEmpty()) {
                showWarning("", QString(tr("请检查输入数据是否有误！")));
                return;
            }
            resultTextEdit->setText(resultText);
        }

    });
}
