//
// Created by chang on 2017-07-31.
//

#ifndef SERIALWIZARD_CALCULATECHECKSUMDIALOG_H
#define SERIALWIZARD_CALCULATECHECKSUMDIALOG_H

#include <QtWidgets/QDialog>

class QLineEdit;

class QCheckBox;

class QComboBox;

class QTextEdit;

class QVBoxLayout;

class ConvertDataDialog : public QDialog {
Q_OBJECT
public:
    explicit ConvertDataDialog(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

private:
    void createUi();

    void createConnect();

    void processData();

    QTextEdit *inputTextEdit;

    QCheckBox *hexCheckBox;

    QLineEdit *prefixLineEdit;

    QLineEdit *separatorLineEdit;

    QVBoxLayout *outputLayout;

    QComboBox *otherCodexComboBox;
    QTextEdit *otherCodecTextEdit;
};

#endif //SERIALWIZARD_CALCULATECHECKSUMDIALOG_H
