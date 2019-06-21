//
// Created by chang on 2017-07-31.
//

#ifndef SERIALWIZARD_CONVERTDATADIALOG_H
#define SERIALWIZARD_CONVERTDATADIALOG_H


#include <QtWidgets/QDialog>

class QLineEdit;
class QLabel;
class QPushButton;

class CalculateCheckSumDialog : public QDialog {
    Q_OBJECT
public:
    explicit CalculateCheckSumDialog(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());



private:
    void createUi();
    void createConnect();

    QString getSumResult(const QByteArray &data);
    QString getXorResult(const QByteArray &data);

    QLineEdit *inputLineEdit;

    QPushButton *calculateButton;
    QLabel *xorResultLabel;
    QLabel *sumResultLabel;


};


#endif //SERIALWIZARD_CONVERTDATADIALOG_H
