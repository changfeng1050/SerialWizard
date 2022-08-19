//
// Created by chang on 2021/6/14.
//

#ifndef SERIALWIZARD_DATAPROCESSDIALOG_H
#define SERIALWIZARD_DATAPROCESSDIALOG_H

#include <QDialog>

class QLabel;

class QTextEdit;

class QLineEdit;

class QRadioButton;

class QCheckBox;

class QPushButton;

class DataProcessDialog : public QDialog {
Q_OBJECT
public:
    explicit DataProcessDialog(const QString &text = "", QWidget *parent = nullptr,
                               Qt::WindowFlags f = Qt::WindowFlags());

    ~DataProcessDialog() override;

    enum class MatchPositionType {
        First = 0,
        Last,
        Middle,
    };

    enum class LineReturn {
        RN = 0,
        R,
        N,
        Space,
        None,
        Custom
    };

    QString text();

    QString matchText();

    int fromIndex();

    bool includeMatchText();

    int matchPosition();

    MatchPositionType matchPositionType();

    LineReturn lineReturn();

    int matchLineCount();

    QList<int> getMatchIndexList();

    QStringList getMatchIndexTextList();

protected:
    void keyPressEvent(QKeyEvent *) override;

    void closeEvent(QCloseEvent *event) override;

    void dropEvent(QDropEvent *event) override;

    void dragEnterEvent(QDragEnterEvent *event) override;

public Q_SLOT:

    void replaceLineReturn();

    void splitLine();

    void deleteFrame();

private:
    void createUi();

    void readSettings();

    void writeSettings();

    void createConnect();

    void processText(QString (*p)(DataProcessDialog *, const QString &));

    void processDeleteLines(bool (*p)(DataProcessDialog *, const QString &));

    void processKeepLines(bool  (*p)(DataProcessDialog *, const QString &));

    QTextEdit *textEdit{nullptr};
    QLabel *lineCountLabel{nullptr};
    QLabel *charCountLabel{nullptr};
    QPushButton *simplifiedButton;
    QLineEdit *fromIndexLineEdit{nullptr};
    QRadioButton *matchFirstRadioButton{nullptr};
    QRadioButton *matchLastRadioButton{nullptr};
    QRadioButton *matchMiddleRadioButton{nullptr};
    QLineEdit *matchPositionLineEdit{nullptr};
    QCheckBox *includeMatchTextCheckBox{nullptr};

    QLineEdit *matchTextLineEdit{nullptr};
    QLineEdit *matchTextCountLineEdit{nullptr};

    QPushButton *deleteTextButton{nullptr};
    QPushButton *deletePrefixButton{nullptr};
    QPushButton *deleteSuffixButton{nullptr};
    QPushButton *deleteBeforeButton{nullptr};
    QPushButton *deleteAfterButton{nullptr};
    QPushButton *deleteMatchTextLineButton{nullptr};
    QPushButton *keepMatchTextLineButton{nullptr};

    QRadioButton *returnTypeRnRadioButton{nullptr};
    QRadioButton *returnTypeRRadioButton{nullptr};
    QRadioButton *returnTypeNRadioButton{nullptr};
    QRadioButton *returnTypeSpaceRadioButton{nullptr};
    QRadioButton *returnTypeNoneRadioButton{nullptr};
    QRadioButton *returnTypeCustomRadioButton{nullptr};
    QLineEdit *customReturnLineEdit{nullptr};

    QPushButton *returnTypeReplaceButton{nullptr};

    QLineEdit *framePrefixLineEdit{nullptr};
    QLineEdit *frameSuffixLineEdit{nullptr};
    QPushButton *frameSplitLineButton{nullptr};
    QPushButton *frameDeleteButton{nullptr};

    QPushButton *deleteShorterLineButton{nullptr};
    QPushButton *deleteShorterOrEqualLineButton{nullptr};
    QPushButton *deleteEqualLineButton{nullptr};
    QPushButton *deleteLongerOrEqualLineButton{nullptr};
    QPushButton *deleteLongerLineButton{nullptr};

    QPushButton *keepShorterLineButton{nullptr};
    QPushButton *keepShorterOrEqualLineButton{nullptr};
    QPushButton *keepEqualLineButton{nullptr};
    QPushButton *keepLongerOrEqualLineButton{nullptr};
    QPushButton *keepLongerLineButton{nullptr};

    QList<QLineEdit *> indexLineEditList;
    QList<QLineEdit *> matchLineEditList;
    QPushButton *deleteMatchLineButton{nullptr};
    QPushButton *keepMatchLineButton{nullptr};

    QPushButton *saveButton{nullptr};
    QPushButton *cancelButton{nullptr};
};

#endif //SERIALWIZARD_DATAPROCESSDIALOG_H
