//
// Created by chang on 2017-07-31.
//

#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QtWidgets/QHBoxLayout>
#include <QDebug>

#include "CalculateCheckSumDialog.h"

CalculateCheckSumDialog::CalculateCheckSumDialog(QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f) {
    setWindowFlags(
            Qt::Dialog | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
    createUi();
    createConnect();
}

void CalculateCheckSumDialog::createUi() {

    inputLineEdit = new QLineEdit(this);
    inputLineEdit->setMinimumWidth(600);

    calculateButton = new QPushButton(tr("计算"), this);

    xorResultLabel = new QLabel(this);

    sumResultLabel = new QLabel(this);

    auto inputLayout = new QHBoxLayout;
    inputLayout->addWidget(inputLineEdit);
    inputLayout->addWidget(calculateButton);

    auto resultLayout = new QVBoxLayout;
    resultLayout->addWidget(xorResultLabel);
    resultLayout->addWidget(sumResultLabel);

    xorResultLabel->setVisible(false);
    sumResultLabel->setVisible(false);

    auto layout = new QVBoxLayout;
    layout->addLayout(inputLayout);
    layout->addLayout(resultLayout);

    setLayout(layout);
    setWindowTitle(tr("计算检验"));
}

void CalculateCheckSumDialog::createConnect() {
    connect(calculateButton, &QPushButton::clicked, [this] {

        xorResultLabel->hide();
        sumResultLabel->hide();

        auto input = inputLineEdit->text();
        input.remove(" ");

        if (input.isEmpty()) {
            return;
        }

        QByteArray data = QByteArray();

        for (int i = 0; i < input.count(); i += 2) {
            auto t = input.mid(i, 2);
            data.append(QByteArray::fromHex(t.toLocal8Bit()));
        }

        qDebug() << "calculate data:" << data.toHex();

        auto sumResult = getSumResult(data);
        auto xorResult = getXorResult(data);

        sumResultLabel->setText(QString("%1%2").arg("检验和 ").arg(sumResult));
        xorResultLabel->setText(QString("%1%2").arg("异或值 ").arg(xorResult));

        xorResultLabel->show();
        sumResultLabel->show();
    });
}

QString CalculateCheckSumDialog::getSumResult(const QByteArray &data) {
    int sum = 0x00;
    for (auto s: data) {
        sum += s;
    }
    return QString::number(sum & 0x00FFFF, 16).left(4).toUpper();
}

QString CalculateCheckSumDialog::getXorResult(const QByteArray &data) {
    if (data.isEmpty()) {
        return "";
    }
    char sum = data[0];

    for (int i = 1; i < data.count(); i++) {
        sum = sum ^ data[i];
    }

    return QString::number(sum & 0x00FF, 16).left(2).toUpper();
}


