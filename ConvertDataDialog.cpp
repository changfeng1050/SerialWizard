//
// Created by chang on 2017-07-31.
//

#include <QtCore/QTextCodec>
#include <QScrollArea>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QtWidgets/QHBoxLayout>
#include <QDebug>
#include <QCheckBox>
#include <QComboBox>
#include <QTextEdit>

#include "ConvertDataDialog.h"
#include "global.h"

QMap<QString, QTextCodec *> codecMap;
QMap<QString, QTextEdit *> codecTextEditMap;

QStringList otherCodecList;

ConvertDataDialog::ConvertDataDialog(QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f) {

    setWindowFlags(
            Qt::Dialog | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);

    codecMap.clear();
    codecMap["01-GB18030/GB2312/GBK"] = QTextCodec::codecForName("GB18030");
    codecMap["02-UTF-8"] = QTextCodec::codecForName("UTF-8");
    codecMap["03-BIG5"] = QTextCodec::codecForName("Big5");
    codecMap["04-BIG5-HKSCS"] = QTextCodec::codecForName("Big5-HKSCS");
//    codecMap["05-UTF-16"] = QTextCodec::codecForName("UTF-16");
//    codecMap["06-UTF-16BE"] = QTextCodec::codecForName("UTF-16BE");
//    codecMap["07-UTF-16LE"] = QTextCodec::codecForName("UTF-16LE");
//    codecMap["08-UTF-32"] = QTextCodec::codecForName("UTF-32");
//    codecMap["09-UTF-32BE"] = QTextCodec::codecForName("UTF-32BE");
//    codecMap["10-UTF-32LE"] = QTextCodec::codecForName("UTF-32LE");

    otherCodecList.clear();
    otherCodecList << "UTF-16"
                   << "UTF-16BE"
                   << "UTF-16LE"
                   << "UTF-32"
                   << "UTF-32BE"
                   << "UTF-32LE"
                   << "CP949"
                   << "EUC-KR"
                   << "EUC-KR"
                   << "HP-ROMAN8"
                   << "IBM 850"
                   << "IBM 866"
                   << "IBM 874"
                   << "IOS 2022-JP"
                   << "ISO 8859-1"
                   << "ISO 8859-2"
                   << "ISO 8859-3"
                   << "ISO 8859-4"
                   << "ISO 8859-5"
                   << "ISO 8859-6"
                   << "ISO 8859-7"
                   << "ISO 8859-8"
                   << "ISO 8859-9"
                   << "ISO 8859-10"
                   << "ISO 8859-13"
                   << "ISO 8859-14"
                   << "ISO 8859-15"
                   << "ISO 8859-16"
                   << "Iscii-Bng"
                   << "Iscii-Dev"
                   << "Iscii-Gjr"
                   << "Iscii-Knd"
                   << "Iscii-Mlm"
                   << "Iscii-Ori"
                   << "Iscii-Pnj"
                   << "Iscii-Tlg"
                   << "Iscii-Tml"
                   << "KOI8-R"
                   << "KOI8-U"
                   << "Macintosh"
                   << "Shift-JIS"
                   << "TIS-620"
                   << "TSCII"
                   << "Windows-1250"
                   << "Windows-1251"
                   << "Windows-1252"
                   << "Windows-1253"
                   << "Windows-1254"
                   << "Windows-1255"
                   << "Windows-1256"
                   << "Windows-1257"
                   << "Windows-1258";

    createUi();
    createConnect();
}

void ConvertDataDialog::createUi() {

    inputTextEdit = new QTextEdit(this);
    inputTextEdit->setMinimumSize(1280, 64);
    inputTextEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    inputTextEdit->setFontPointSize(12);
    inputTextEdit->setFont(QFont("Microsoft YaHei"));

    hexCheckBox = new QCheckBox(tr("HEX"), this);
    hexCheckBox->setChecked(false);

    auto separatorLabel = new QLabel(tr("分隔符"), this);
    separatorLineEdit = new QLineEdit(" ,", this);
    separatorLineEdit->setFixedWidth(50);
    separatorLabel->setBuddy(separatorLineEdit);

    auto prefixLabel = new QLabel(tr("前缀"), this);
    prefixLineEdit = new QLineEdit("0x", this);
    prefixLineEdit->setFixedWidth(50);
    prefixLabel->setBuddy(prefixLineEdit);

    auto convertLayout = new QHBoxLayout;
    convertLayout->addWidget(hexCheckBox);
    convertLayout->addStretch();
    convertLayout->addWidget(separatorLabel);
    convertLayout->addWidget(separatorLineEdit);
    convertLayout->addWidget(prefixLabel);
    convertLayout->addWidget(prefixLineEdit);

    outputLayout = new QVBoxLayout;
    auto codecKeys = codecMap.keys();
    for (const QString &key: codecKeys) {
        auto label = new QLabel(key, this);
        label->setStyleSheet("color:blue;");
        auto textEdit = new QTextEdit(this);
        textEdit->setMinimumHeight(12);
        textEdit->setFontPointSize(12);
        textEdit->setReadOnly(true);
        textEdit->setFont(QFont("Microsoft YaHei"));
        textEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        label->setBuddy(textEdit);
        codecTextEditMap[key] = textEdit;
        outputLayout->addWidget(label);
        outputLayout->addWidget(textEdit);
    }

    auto otherLabel = new QLabel(tr("其他编码"), this);
    otherCodexComboBox = new QComboBox(this);
    otherCodexComboBox->addItems(otherCodecList);
    otherLabel->setBuddy(otherCodexComboBox);

    auto otherLayout = new QHBoxLayout;
    otherLayout->addWidget(otherLabel);
    otherLayout->addWidget(otherCodexComboBox);
    otherLayout->addStretch();

    otherCodecTextEdit = new QTextEdit(this);
    otherCodecTextEdit->setMinimumHeight(12);
    otherCodecTextEdit->setFontPointSize(12);
    otherCodecTextEdit->setReadOnly(true);
    otherCodecTextEdit->setFont(QFont("Microsoft YaHei"));
    otherCodecTextEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    outputLayout->addLayout(otherLayout);
    outputLayout->addWidget(otherCodecTextEdit);

    auto layout = new QVBoxLayout;
    layout->addWidget(inputTextEdit);
    layout->addLayout(convertLayout);
    layout->addLayout(outputLayout);

    setLayout(layout);
    setWindowTitle(tr("字符编码"));
}

void ConvertDataDialog::createConnect() {
    connect(inputTextEdit, &QTextEdit::textChanged, this, [this] {
        processData();
    });

    connect(hexCheckBox, &QCheckBox::stateChanged, this, [this] {
        processData();
    });

    connect(prefixLineEdit, &QLineEdit::textChanged, this, [this] {
        processData();
    });

    connect(separatorLineEdit, &QLineEdit::textChanged, this, [this] {
        processData();
    });

    connect(otherCodexComboBox, &QComboBox::currentTextChanged, this, [this] {
        processData();
    });

    inputTextEdit->setPlainText("123 ABC 中国 中國");
}

void ConvertDataDialog::processData() {
    if (inputTextEdit->toPlainText().isEmpty()) {
        for (auto &textEdit: codecTextEditMap) {
            textEdit->clear();
        }
        return;
    }
    auto separator = separatorLineEdit->text();
    auto prefix = prefixLineEdit->text();
    if (hexCheckBox->isChecked()) {
        auto keys = codecTextEditMap.keys();
        for (const QString &key: keys) {
            auto codec = codecMap[key];
            auto byteArray = dataFromHex(inputTextEdit->toPlainText());
            codecTextEditMap[key]->setPlainText(fromCodec(byteArray, codec));
        }
        auto otherCodec = QTextCodec::codecForName(otherCodexComboBox->currentText().toUtf8());
        otherCodecTextEdit->setPlainText(fromCodec(dataFromHex(inputTextEdit->toPlainText()), otherCodec));
    } else {
        auto keys = codecTextEditMap.keys();
        for (const QString &key: keys) {
            auto codec = codecMap[key];
            codecTextEditMap[key]->setPlainText(
                    dataToHex(toCodecByteArray(inputTextEdit->toPlainText(), codec), separator, prefix));
        }
        auto otherCodec = QTextCodec::codecForName(otherCodexComboBox->currentText().toUtf8());
        otherCodecTextEdit->setPlainText(
                dataToHex(toCodecByteArray(inputTextEdit->toPlainText(), otherCodec), separator, prefix));
    }
}