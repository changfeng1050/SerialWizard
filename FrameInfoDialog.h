//
// Created by chang on 2017-08-02.
//

#ifndef SERIALWIZARD_FRAMEINFODIALOG_H
#define SERIALWIZARD_FRAMEINFODIALOG_H


#include <QtWidgets/QDialog>

class QLineEdit;

class QComboBox;

class QPushButton;

typedef struct {
    QString frame;
    unsigned char head;
    unsigned char end;
    int lenIndex;
    int lenCount;
    int headLen;
    int endLen;
} FrameInfo;


class FrameInfoDialog : public QDialog {
Q_OBJECT
public:
    explicit FrameInfoDialog(const FrameInfo &frameInfo, QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

signals:
    void frameInfoChanged(FrameInfo info);

private:
    void createUi();

    void initUiValue(FrameInfo info);

    void createConnect();

    FrameInfo info;

    QLineEdit *frameLineEdit;
    QLineEdit *frameHeadLineEdit;
    QLineEdit *frameEndLineEdit;
    QLineEdit *frameLenIndexLineEdit;
    QComboBox *frameLenCountComboBox;

    QLineEdit *frameHeadLenLineEdit;
    QLineEdit *frameEndLenLineEdit;

    QPushButton *okButton;
    QPushButton *cancelButton;

};


#endif //SERIALWIZARD_FRAMEINFODIALOG_H
