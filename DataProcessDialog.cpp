//
// Created by chang on 2021/6/14.
//
#include <QMimeData>
#include <QCloseEvent>
#include <QTextEdit>
#include <QLineEdit>
#include <QRadioButton>
#include <QPushButton>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QButtonGroup>
#include <QCheckBox>
#include <QLabel>
#include <QSettings>
#include <QFileInfo>

#include "DataProcessDialog.h"
#include "global.h"
#include "utils/FileUtil.h"

static const int MATCH_INDEX_COUNT = 5;

QString convertSimplified(DataProcessDialog *dialog, const QString &line) {
    return line.simplified();
}

QString convertRemoveText(DataProcessDialog *dialog, const QString &line) {
    auto text = dialog->matchText();
    if (line.contains(text)) {
        return QString(line).remove(text);
    }
    return line;
}

QString convertRemovePrefix(DataProcessDialog *dialog, const QString &line) {
    auto text = dialog->matchText();
    if (line.startsWith(text)) {
        return line.right(line.count() - text.count());
    }
    return line;
}

QString convertRemoveSuffix(DataProcessDialog *dialog, const QString &line) {
    auto text = dialog->matchText();
    if (line.endsWith(text)) {
        return line.chopped(line.count());
    }
    return line;
}

QString convertRemoveBefore(DataProcessDialog *dialog, const QString &line) {
    auto text = dialog->matchText();
    if (!line.contains(text)) {
        return line;
    }

    int index = -1;
    int fromIndex = dialog->fromIndex();
    if (dialog->matchPositionType() == DataProcessDialog::MatchPositionType::First) {
        index = line.indexOf(text, fromIndex);
    } else if (dialog->matchPositionType() == DataProcessDialog::MatchPositionType::Last) {
        index = line.lastIndexOf(text, fromIndex);
    } else if (dialog->matchPositionType() == DataProcessDialog::MatchPositionType::Middle) {
        int position = dialog->matchPosition();
        int pos = fromIndex;
        int currentIndex;
        do {
            currentIndex = line.indexOf(text, pos);
            if (currentIndex < 0) {
                break;
            }
            pos = currentIndex + text.count();
            position--;
        } while (position > 0);
        if (currentIndex > -1) {
            index = currentIndex;
        }
    } else {
        index = line.indexOf(text, fromIndex);
    }

    if (index > -1 && dialog->includeMatchText()) {
        index += text.count();
    }
    return line.mid(index);
}

QString convertRemoveAfter(DataProcessDialog *dialog, const QString &line) {
    auto text = dialog->matchText();
    if (!line.contains(text)) {
        return line;
    }

    int index = -1;
    int fromIndex = dialog->fromIndex();
    if (dialog->matchPositionType() == DataProcessDialog::MatchPositionType::First) {
        index = line.lastIndexOf(text, fromIndex);
    } else if (dialog->matchPositionType() == DataProcessDialog::MatchPositionType::Last) {
        index = line.indexOf(text, fromIndex);
    } else if (dialog->matchPositionType() == DataProcessDialog::MatchPositionType::Middle) {
        int position = dialog->matchPosition();
        int pos = fromIndex;
        int currentIndex;
        do {
            currentIndex = line.lastIndexOf(text, pos);
            if (currentIndex < 0) {
                break;
            }
            pos = currentIndex + text.count();
            position--;
        } while (position > 0);
        if (currentIndex > -1) {
            index = currentIndex;
        }
    } else {
        index = line.lastIndexOf(text, fromIndex);
    }

    if (index > -1 && !dialog->includeMatchText()) {
        index += text.count();
    }
    return line.mid(0, index);
}

bool shorterLine(DataProcessDialog *dialog, const QString &line) {
    return line.count() < dialog->matchLineCount();
}

bool shorterOrEqualLine(DataProcessDialog *dialog, const QString &line) {
    return line.count() <= dialog->matchLineCount();
}

bool equalLine(DataProcessDialog *dialog, const QString &line) {
    return line.count() == dialog->matchLineCount();
}

bool longerOrEqualLine(DataProcessDialog *dialog, const QString &line) {
    return line.count() >= dialog->matchLineCount();
}

bool longerLine(DataProcessDialog *dialog, const QString &line) {
    return line.count() > dialog->matchLineCount();
}

bool deleteMatchTextLine(DataProcessDialog *dialog, const QString &line) {
    return line.contains(dialog->matchText());
}

bool saveMatchTextLine(DataProcessDialog *dialog, const QString &line) {
    return !line.contains(dialog->matchText());
}

bool deleteMatchIndexLine(DataProcessDialog *dialog, const QString &line) {
    auto items = line.split(QRegExp("\\s+"));
    auto indexList = dialog->getMatchIndexList();
    auto textList = dialog->getMatchIndexTextList();
    QMap<int, QString> map;
    for (int i = 0; i < MATCH_INDEX_COUNT; i++) {
        auto index = indexList[i] - 1;
        if (index >= 0) {
            map[index] = textList[i];
        }
    }

    if (map.isEmpty()) {
        return false;
    }
    for (auto &index: map.keys()) {
        if (index > items.count() - 1) {
            return false;
        }
        if (map[index] != items[index]) {
            return false;
        }
    }

    return true;
}

bool keepMatchIndexLine(DataProcessDialog *dialog, const QString &line) {
    auto items = line.split(QRegExp("\\s+"));
    auto indexList = dialog->getMatchIndexList();
    auto textList = dialog->getMatchIndexTextList();
    QMap<int, QString> map;
    for (int i = 0; i < MATCH_INDEX_COUNT; i++) {
        auto index = indexList[i] - 1;
        if (index >= 0) {
            map[index] = textList[i];
        }
    }

    if (map.isEmpty()) {
        return false;
    }
    for (auto &index: map.keys()) {
        if (index > items.count() - 1) {
            return false;
        }
        if (map[index] != items[index]) {
            return true;
        }
    }

    return false;
}

DataProcessDialog::DataProcessDialog(const QString &text, QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f) {
    setWindowFlags(
            Qt::Dialog | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
    createUi();
    createConnect();
    readSettings();
    if (!text.isEmpty()) {
        textEdit->setText(text);
    }

    textEdit->setAcceptDrops(false);
    setAcceptDrops(true);
}

DataProcessDialog::~DataProcessDialog() {
    writeSettings();
}

void DataProcessDialog::createUi() {
    setMinimumSize(1280, 900);
    textEdit = new QTextEdit(this);
    textEdit->setSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Expanding);

    simplifiedButton = new QPushButton(tr("自动整理"));
    simplifiedButton->setToolTip(tr("自动删除两端空白字符,多个空白字符转换成一个空白字符"));

    lineCountLabel = new QLabel(this);
    charCountLabel = new QLabel(this);

    auto simplifiedLayout = new QHBoxLayout;
    simplifiedLayout->addWidget(simplifiedButton);
    simplifiedLayout->addStretch();
    simplifiedLayout->addWidget(lineCountLabel);
    simplifiedLayout->addWidget(charCountLabel);

    auto matchInfoLayout = new QVBoxLayout;

    auto matchTextLabel = new QLabel(tr("匹配字符串"), this);
    matchTextLineEdit = new QLineEdit(this);
    matchTextLabel->setBuddy(matchTextLineEdit);

    auto matchTextLayout = new QHBoxLayout;
    matchTextLayout->addWidget(matchTextLabel);
    matchTextLayout->addWidget(matchTextLineEdit);

    auto positionLayout = new QHBoxLayout;
    auto fromIndexLabel = new QLabel(tr("起始字符/字节(从0开始)"), this);
    fromIndexLineEdit = new QLineEdit(this);
    fromIndexLineEdit->setAlignment(Qt::AlignCenter);
    fromIndexLineEdit->setFixedWidth(30);
    fromIndexLabel->setBuddy(fromIndexLineEdit);
    matchFirstRadioButton = new QRadioButton(tr("匹配第一个"), this);
    matchLastRadioButton = new QRadioButton(tr("匹配最后一个"), this);
    matchMiddleRadioButton = new QRadioButton(tr("配置第N个(从0开始)"), this);
    auto matchPositionRadioGroup = new QButtonGroup(this);
    matchPositionRadioGroup->addButton(matchFirstRadioButton);
    matchPositionRadioGroup->addButton(matchLastRadioButton);
    matchPositionRadioGroup->addButton(matchMiddleRadioButton);
    matchPositionLineEdit = new QLineEdit(this);
    matchPositionLineEdit->setAlignment(Qt::AlignCenter);
    matchPositionLineEdit->setFixedWidth(30);

    positionLayout->addWidget(fromIndexLabel);
    positionLayout->addWidget(fromIndexLineEdit);
    positionLayout->addStretch();
    positionLayout->addWidget(matchFirstRadioButton);
    positionLayout->addWidget(matchLastRadioButton);
    positionLayout->addWidget(matchMiddleRadioButton);
    positionLayout->addWidget(matchPositionLineEdit);

    matchInfoLayout->addLayout(matchTextLayout);
    matchInfoLayout->addLayout(positionLayout);

    auto deleteLayout = new QHBoxLayout;
    includeMatchTextCheckBox = new QCheckBox(tr("包括匹配字符"), this);
    deleteTextButton = new QPushButton(tr("字符串"), this);
    deletePrefixButton = new QPushButton(tr("前缀字符"), this);
    deleteSuffixButton = new QPushButton(tr("后缀字符"), this);
    deleteBeforeButton = new QPushButton(tr("匹配字符串之前的字符"), this);
    deleteAfterButton = new QPushButton(tr("匹配字符串之后的字符"), this);
    deleteMatchTextLineButton = new QPushButton(tr("删除匹配字符行"), this);
    keepMatchTextLineButton = new QPushButton(tr("保留匹配字符行"), this);
    deleteLayout->addWidget(includeMatchTextCheckBox);
    auto deleteLabel = new QLabel(tr("删除"));
    deleteLayout->addWidget(deleteLabel);
    deleteLayout->addWidget(deleteTextButton);
    deleteLayout->addWidget(deletePrefixButton);
    deleteLayout->addWidget(deleteSuffixButton);
    deleteLayout->addWidget(deleteBeforeButton);
    deleteLayout->addWidget(deleteAfterButton);
    deleteLayout->addStretch();
    deleteLayout->addWidget(deleteMatchTextLineButton);
    deleteLayout->addWidget(keepMatchTextLineButton);

    auto deleteGroupBox = new QGroupBox(tr("按字符串处理"), this);
    deleteGroupBox->setLayout(deleteLayout);

    auto returnReplaceLabel = new QLabel(tr("将换行符替换为"), this);
    returnTypeRnRadioButton = new QRadioButton("\\r\\n", this);
    returnTypeRRadioButton = new QRadioButton("\\r", this);
    returnTypeNRadioButton = new QRadioButton("\\n", this);
    returnTypeSpaceRadioButton = new QRadioButton(tr("空格"), this);
    returnTypeNoneRadioButton = new QRadioButton(tr("无"), this);
    returnTypeCustomRadioButton = new QRadioButton(tr("自定义"), this);

    auto returnTypeButtonGroup = new QButtonGroup(this);
    returnTypeButtonGroup->addButton(returnTypeRnRadioButton);
    returnTypeButtonGroup->addButton(returnTypeRRadioButton);
    returnTypeButtonGroup->addButton(returnTypeNRadioButton);
    returnTypeButtonGroup->addButton(returnTypeSpaceRadioButton);
    returnTypeButtonGroup->addButton(returnTypeNoneRadioButton);
    returnTypeButtonGroup->addButton(returnTypeCustomRadioButton);

    customReturnLineEdit = new QLineEdit(tr(""), this);
    returnTypeReplaceButton = new QPushButton(tr("替换"), this);

    auto returnTypeLayout = new QHBoxLayout;
    returnTypeLayout->addWidget(returnReplaceLabel);
    returnTypeLayout->addWidget(returnTypeRnRadioButton);
    returnTypeLayout->addWidget(returnTypeRRadioButton);
    returnTypeLayout->addWidget(returnTypeNRadioButton);
    returnTypeLayout->addWidget(returnTypeSpaceRadioButton);
    returnTypeLayout->addWidget(returnTypeNoneRadioButton);
    returnTypeLayout->addWidget(returnTypeCustomRadioButton);
    returnTypeLayout->addWidget(customReturnLineEdit);
    returnTypeLayout->addStretch();
    returnTypeLayout->addWidget(returnTypeReplaceButton);

    auto returnTypeGroupBox = new QGroupBox(tr("换行替换"), this);
    returnTypeGroupBox->setLayout(returnTypeLayout);

    auto framePrefixLabel = new QLabel(tr("帧头字符串"));
    framePrefixLineEdit = new QLineEdit(this);
    framePrefixLabel->setBuddy(framePrefixLineEdit);
    auto frameSuffixLabel = new QLabel(tr("帧尾字符串"));
    frameSuffixLineEdit = new QLineEdit(this);
    frameSuffixLabel->setBuddy(frameSuffixLineEdit);
    frameSplitLineButton = new QPushButton(tr("重新换行"), this);
    frameDeleteButton = new QPushButton(tr("删除帧"), this);

    auto frameLayout = new QHBoxLayout;
    frameLayout->addWidget(framePrefixLabel);
    frameLayout->addWidget(framePrefixLineEdit);
    frameLayout->addWidget(frameSuffixLabel);
    frameLayout->addWidget(frameSuffixLineEdit);
    frameLayout->addStretch();
    frameLayout->addWidget(frameSplitLineButton);
    frameLayout->addWidget(frameDeleteButton);

    auto frameGroupBox = new QGroupBox(tr("帧处理"), this);
    frameGroupBox->setLayout(frameLayout);

    auto matchTextCountLabel = new QLabel(tr("匹配字符串的字符数"), this);
    matchTextCountLineEdit = new QLineEdit(this);
    matchTextCountLineEdit->setAlignment(Qt::AlignCenter);
    matchTextCountLineEdit->setFixedWidth(30);
    matchTextCountLabel->setBuddy(matchTextCountLineEdit);
    deleteShorterLineButton = new QPushButton(tr("删除短于的行"));
    deleteShorterOrEqualLineButton = new QPushButton(tr("删除短于或等于的行"));
    deleteEqualLineButton = new QPushButton(tr("删除等于的行"));
    deleteLongerOrEqualLineButton = new QPushButton(tr("删除长于或等于行"));
    deleteLongerLineButton = new QPushButton(tr("删除长于的行"));

    keepShorterLineButton = new QPushButton(tr("保留短于的行"));
    keepShorterOrEqualLineButton = new QPushButton(tr("保留短于或等于的行"));
    keepEqualLineButton = new QPushButton(tr("保留等于的行"));
    keepLongerOrEqualLineButton = new QPushButton(tr("保留长于或等于的行"));
    keepLongerLineButton = new QPushButton(tr("保留长于的行"));

    auto deleteLineLayout = new QHBoxLayout;
    deleteLineLayout->addWidget(matchTextCountLabel);
    deleteLineLayout->addWidget(matchTextCountLineEdit);
    deleteLineLayout->addWidget(deleteShorterLineButton);
    deleteLineLayout->addWidget(deleteShorterOrEqualLineButton);
    deleteLineLayout->addWidget(deleteEqualLineButton);
    deleteLineLayout->addWidget(deleteLongerLineButton);
    deleteLineLayout->addWidget(deleteLongerOrEqualLineButton);
    deleteLineLayout->addStretch();
    deleteLineLayout->addWidget(keepShorterLineButton);
    deleteLineLayout->addWidget(keepShorterOrEqualLineButton);
    deleteLineLayout->addWidget(keepEqualLineButton);
    deleteLineLayout->addWidget(keepLongerOrEqualLineButton);
    deleteLineLayout->addWidget(keepLongerLineButton);

    auto deleteLineGroupBox = new QGroupBox(tr("按行字符数处理"), this);
    deleteLineGroupBox->setLayout(deleteLineLayout);

    auto lineIndexLayout = new QHBoxLayout;
    auto lineIndexLabel1 = new QLabel(tr("第"), this);
    lineIndexLayout->addWidget(lineIndexLabel1);
    for (int i = 0; i < MATCH_INDEX_COUNT; i++) {
        auto indexLineEdit = new QLineEdit(this);
        indexLineEdit->setAlignment(Qt::AlignCenter);
        indexLineEdit->setFixedWidth(30);
        indexLineEditList.append(indexLineEdit);
        lineIndexLayout->addWidget(indexLineEdit);
    }
    auto lineIndexLabel2 = new QLabel(tr("位置(空格间隔)字符串值为"), this);
    lineIndexLayout->addWidget(lineIndexLabel2);
    for (int i = 0; i < MATCH_INDEX_COUNT; i++) {
        auto matchLineEdit = new QLineEdit(this);
        matchLineEdit->setAlignment(Qt::AlignCenter);
        matchLineEditList.append(matchLineEdit);
        lineIndexLayout->addWidget(matchLineEdit);
    }
    lineIndexLayout->addStretch();
    deleteMatchLineButton = new QPushButton(tr("删除行"), this);
    keepMatchLineButton = new QPushButton(tr("保留行"), this);
    lineIndexLayout->addWidget(deleteMatchLineButton);
    lineIndexLayout->addWidget(keepMatchLineButton);

    auto lineIndexGroup = new QGroupBox(tr("高级行处理"), this);
    lineIndexGroup->setLayout(lineIndexLayout);

    cancelButton = new QPushButton(tr("取消"), this);
    saveButton = new QPushButton(tr("保存"), this);
    auto operationLayout = new QHBoxLayout;
    operationLayout->addStretch();
    operationLayout->addWidget(cancelButton);
    operationLayout->addWidget(saveButton);

    auto layout = new QVBoxLayout();
    layout->addWidget(textEdit);
    layout->addLayout(simplifiedLayout);
    layout->addLayout(matchInfoLayout);
    layout->addWidget(deleteGroupBox);
    layout->addWidget(returnTypeGroupBox);
    layout->addWidget(frameGroupBox);
    layout->addWidget(deleteLineGroupBox);
    layout->addWidget(lineIndexGroup);
    layout->addLayout(operationLayout);
    setLayout(layout);
    setWindowTitle(tr("数据处理"));
}

void DataProcessDialog::createConnect() {

    connect(textEdit, &QTextEdit::textChanged, [this] {
        QString text = textEdit->toPlainText();
        auto charCount = text.count();
        auto lineCount = 0;
        int i = 0;
        while (i < charCount) {
            if (text[i] == '\n') {
                lineCount++;
                i++;
            } else if (text[i] == '\r') {
                if (i + 1 < charCount && text[i + 1] == '\n') {
                    lineCount++;
                    i += 2;
                } else {
                    lineCount++;
                    i++;
                }
            } else {
                i++;
            }
        }
        lineCount += 1; // 行数比换行符号多1
        if (text.isEmpty()) {
            lineCountLabel->setText("");
            charCountLabel->setText("");
        } else {
            lineCountLabel->setText(QString(tr("%1 行")).arg(lineCount));
            charCountLabel->setText(QString(tr("%1 字符")).arg(charCount));
        }
    });

    connect(matchTextLineEdit, &QLineEdit::textChanged, [this](const QString &text) {
        matchTextCountLineEdit->setText(QString::number(text.count()));
        auto items = matchTextLineEdit->text().split(QRegExp("\\s+"));
        for (const auto &item: indexLineEditList) {
            item->setText("");
        }
        for (const auto &item: matchLineEditList) {
            item->setText("");
        }

        for (int i = 0; i < items.count(); i++) {
            if (i < indexLineEditList.count()) {
                indexLineEditList[i]->setText(QString::number(i + 1));
                matchLineEditList[i]->setText(items[i]);
            }
        }
    });

    connect(simplifiedButton, &QPushButton::clicked, [this] {
        processText(convertSimplified);
    });

    connect(deleteTextButton, &QPushButton::clicked, [this] {
        processText(convertRemoveText);
    });

    connect(deletePrefixButton, &QPushButton::clicked, [this] {
        processText(convertRemovePrefix);
    });

    connect(deleteSuffixButton, &QPushButton::clicked, [this] {
        processText(convertRemoveSuffix);
    });
    connect(deleteBeforeButton, &QPushButton::clicked, [this] {
        processText(convertRemoveBefore);
    });
    connect(deleteAfterButton, &QPushButton::clicked, [this] {
        processText(convertRemoveAfter);
    });

    connect(returnTypeReplaceButton, &QPushButton::clicked, this, &DataProcessDialog::replaceLineReturn);

    connect(frameSplitLineButton, &QPushButton::clicked, this, &DataProcessDialog::splitLine);

    connect(frameDeleteButton, &QPushButton::clicked, this, &DataProcessDialog::deleteFrame);

    connect(deleteShorterLineButton, &QPushButton::clicked, [this] {
        processDeleteLines(shorterLine);
    });
    connect(deleteShorterOrEqualLineButton, &QPushButton::clicked, [this] {
        processDeleteLines(shorterOrEqualLine);
    });
    connect(deleteEqualLineButton, &QPushButton::clicked, [this] {
        processDeleteLines(equalLine);
    });

    connect(deleteLongerOrEqualLineButton, &QPushButton::clicked, [this] {
        processDeleteLines(longerOrEqualLine);
    });

    connect(deleteLongerLineButton, &QPushButton::clicked, [this] {
        processDeleteLines(longerLine);
    });

    connect(keepShorterLineButton, &QPushButton::clicked, [this] {
        processKeepLines(shorterLine);
    });

    connect(keepShorterOrEqualLineButton, &QPushButton::clicked, [this] {
        processKeepLines(shorterOrEqualLine);
    });

    connect(keepEqualLineButton, &QPushButton::clicked, [this] {
        processKeepLines(equalLine);
    });

    connect(keepLongerOrEqualLineButton, &QPushButton::clicked, [this] {
        processKeepLines(longerOrEqualLine);
    });

    connect(keepLongerLineButton, &QPushButton::clicked, [this] {
        processKeepLines(longerLine);
    });

    connect(deleteMatchTextLineButton, &QPushButton::clicked, [this] {
        processDeleteLines(deleteMatchTextLine);
    });

    connect(keepMatchTextLineButton, &QPushButton::clicked, [this] {
        processDeleteLines(saveMatchTextLine);
    });

    connect(deleteMatchLineButton, &QPushButton::clicked, [this] {
        processDeleteLines(deleteMatchIndexLine);
    });

    connect(keepMatchLineButton, &QPushButton::clicked, [this] {
        processDeleteLines(keepMatchIndexLine);
    });

    connect(cancelButton, &QPushButton::clicked, [this] {
        if (showQuestion("", tr("保存到发送框"))) {
            accept();
        } else {
            reject();
        }
    });

    connect(saveButton, &QPushButton::clicked, [this] {
        accept();
    });
}

void DataProcessDialog::processText(QString (*p)(DataProcessDialog *, const QString &)) {
    auto text = textEdit->toPlainText();
    if (text.isEmpty()) return;
    QString t = text;
    QTextStream in(&t);
    QStringList lines;
    while (!in.atEnd()) {
        auto line = in.readLine();
        line = p(this, line);
        lines.append(line);
    }
    textEdit->setPlainText(lines.join("\n"));
}

void DataProcessDialog::processDeleteLines(bool (*p)(DataProcessDialog *, const QString &)) {
    auto text = textEdit->toPlainText();
    if (text.isEmpty()) {
        return;
    }
    QString t = text;
    QTextStream in(&t);
    QStringList lines;
    while (!in.atEnd()) {
        auto line = in.readLine();
        auto needDelete = p(this, line);
        if (!needDelete) lines.append(line);
    }
    textEdit->setPlainText(lines.join("\n"));
}

void DataProcessDialog::processKeepLines(bool (*p)(DataProcessDialog *, const QString &)) {
    auto text = textEdit->toPlainText();
    if (text.isEmpty()) {
        return;
    }
    QString t = text;
    QTextStream in(&t);
    QStringList lines;
    while (!in.atEnd()) {
        auto line = in.readLine();
        auto need = p(this, line);
        if (need) lines.append(line);
    }
    textEdit->setPlainText(lines.join("\n"));
}

QString DataProcessDialog::matchText() {
    return matchTextLineEdit->text();
}

int DataProcessDialog::matchPosition() {
    bool ok;
    auto value = matchPositionLineEdit->text().toInt(&ok);
    if (ok) {
        return value;
    } else {
        return 0;
    }
}

int DataProcessDialog::fromIndex() {
    bool ok;
    auto value = fromIndexLineEdit->text().toInt(&ok);
    if (ok) {
        return value;
    } else {
        return 0;
    }
}

bool DataProcessDialog::includeMatchText() {
    return includeMatchTextCheckBox->isChecked();
}

DataProcessDialog::MatchPositionType DataProcessDialog::matchPositionType() {
    if (matchFirstRadioButton->isChecked()) {
        return MatchPositionType::First;
    } else if (matchLastRadioButton->isChecked()) {
        return MatchPositionType::Last;
    } else if (matchMiddleRadioButton->isChecked()) {
        return MatchPositionType::Middle;
    } else {
        return MatchPositionType::First;
    }
}

DataProcessDialog::LineReturn DataProcessDialog::lineReturn() {
    if (returnTypeRnRadioButton->isChecked()) {
        return LineReturn::RN;
    } else if (returnTypeRRadioButton->isChecked()) {
        return LineReturn::R;
    } else if (returnTypeNRadioButton->isChecked()) {
        return LineReturn::N;
    } else if (returnTypeSpaceRadioButton->isChecked()) {
        return LineReturn::Space;
    } else if (returnTypeNoneRadioButton->isChecked()) {
        return LineReturn::None;
    } else if (returnTypeCustomRadioButton->isChecked()) {
        return LineReturn::Custom;
    } else {
        return LineReturn::Space;
    }
}

int DataProcessDialog::matchLineCount() {
    return matchTextLineEdit->text().count();
}

QList<int> DataProcessDialog::getMatchIndexList() {
    QList<int> indexList;
    for (int i = 0; i < MATCH_INDEX_COUNT; i++) {
        bool ok;
        auto index = indexLineEditList[i]->text().toInt(&ok);
        if (ok) {
            indexList.append(index);
        } else {
            indexList.append(-1);
        }
    }
    return indexList;
}

QStringList DataProcessDialog::getMatchIndexTextList() {
    QList<QString> matchList;
    for (int i = 0; i < MATCH_INDEX_COUNT; i++) {
        matchList.append(matchLineEditList[i]->text());
    }
    return matchList;
}

void DataProcessDialog::replaceLineReturn() {
    auto text = textEdit->toPlainText();
    if (text.isEmpty()) {
        return;
    }
    QString t = text;
    QTextStream in(&t);
    QStringList lines;
    while (!in.atEnd()) {
        auto line = in.readLine();
        lines.append(line);
    }
    QString separator = " ";
    switch (lineReturn()) {
        case LineReturn::RN:
            separator = "\r\n";
            break;
        case LineReturn::R:
            separator = "\r";
            break;
        case LineReturn::N:
            separator = "\n";
            break;
        case LineReturn::Space:
            separator = " ";
            break;
        case LineReturn::None:
            separator = "";
            break;
        case LineReturn::Custom:
            separator = customReturnLineEdit->text();
            break;
        default:
            separator = " ";
            break;
    }
    textEdit->setPlainText(lines.join(separator));
}

void DataProcessDialog::splitLine() {
    auto prefix = framePrefixLineEdit->text().trimmed();
    auto suffix = frameSuffixLineEdit->text().trimmed();
    if (prefix.isEmpty() && suffix.isEmpty()) {
        return;
    }

    auto text = textEdit->toPlainText();
    if (text.isEmpty()) {
        return;
    }
    QString t = text;
    QTextStream in(&t);
    QStringList lines;
    auto separator = ' ';
    while (!in.atEnd()) {
        auto line = in.readLine();
        lines.append(line);
    }
    text = lines.join(separator).simplified();
    text.replace(suffix + separator + prefix, suffix + '\n' + prefix);
    text.replace(suffix + prefix, suffix + '\n' + prefix);
    QString tt = text;
    QTextStream textStream(&tt);
    lines.clear();
    while (!textStream.atEnd()) {
        auto line = textStream.readLine().trimmed();
        if (line != "\n" && !line.isEmpty()) {
            lines.append(line);
        }
    }
    text = lines.join("\n");
    textEdit->setText(text);
}

void DataProcessDialog::readSettings() {

    QSettings settings("Zhou Jinlong", "Serial Wizard");

    settings.beginGroup("data_process");

    auto text = settings.value("text", "").toString();
    textEdit->setText(text);
    auto matchText = settings.value("match_text", "").toString();
    matchTextLineEdit->setText(matchText);
    auto fromIndex = settings.value("from_index", "0").toString();
    fromIndexLineEdit->setText(fromIndex);
    auto positionType = MatchPositionType(
            settings.value("match_position_type", static_cast<int >(MatchPositionType::First)).toInt());
    if (positionType == MatchPositionType::First) {
        matchFirstRadioButton->setChecked(true);
    } else if (positionType == MatchPositionType::Middle) {
        matchMiddleRadioButton->setChecked(true);
    } else if (positionType == MatchPositionType::Last) {
        matchLastRadioButton->setChecked(true);
    } else {
        matchFirstRadioButton->setChecked(true);
    }
    auto matchPosition = settings.value("match_position", "0").toString();
    matchPositionLineEdit->setText(matchPosition);
    auto includeMatchText = settings.value("include_match_text", true).toBool();
    includeMatchTextCheckBox->setChecked(includeMatchText);
    auto lineReturn = LineReturn(settings.value("line_return_type", static_cast<int>(LineReturn::Space)).toInt());
    if (lineReturn == LineReturn::RN) {
        returnTypeRnRadioButton->setChecked(true);
    } else if (lineReturn == LineReturn::R) {
        returnTypeRRadioButton->setChecked(true);
    } else if (lineReturn == LineReturn::N) {
        returnTypeNoneRadioButton->setChecked(true);
    } else if (lineReturn == LineReturn::Space) {
        returnTypeSpaceRadioButton->setChecked(true);
    } else if (lineReturn == LineReturn::None) {
        returnTypeNoneRadioButton->setChecked(true);
    } else if (lineReturn == LineReturn::Custom) {
        returnTypeCustomRadioButton->setChecked(true);
    } else {
        returnTypeSpaceRadioButton->setChecked(true);
    }
    auto framePrefix = settings.value("frame_prefix", "").toString();
    framePrefixLineEdit->setText(framePrefix);
    auto suffix = settings.value("frame_suffix", "").toString();
    frameSuffixLineEdit->setText(suffix);
}

void DataProcessDialog::writeSettings() {
    QSettings settings("Zhou Jinlong", "Serial Wizard");

    settings.beginGroup("data_process");

    settings.setValue("text", textEdit->toPlainText());
    settings.setValue("match_text", matchTextLineEdit->text());
    settings.setValue("from_index", fromIndexLineEdit->text());
    settings.setValue("match_position_type", static_cast<int>(matchPositionType()));
    settings.setValue("match_position", matchPositionLineEdit->text());
    settings.setValue("include_match_text", includeMatchTextCheckBox->isChecked());
    settings.setValue("line_return_type", static_cast<int >(lineReturn()));
    settings.setValue("frame_prefix", framePrefixLineEdit->text());
    settings.setValue("frame_suffix", frameSuffixLineEdit->text());

    settings.sync();
}

QString DataProcessDialog::text() {
    return textEdit->toPlainText();
}

void DataProcessDialog::closeEvent(QCloseEvent *event) {
    if (showQuestion("", tr("保存到发送框"))) {
        accept();
    } else {
        reject();
    }
}

void DataProcessDialog::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
        if (showQuestion("", tr("保存到发送框"))) {
            accept();
        } else {
            reject();
        }
    } else {
        QDialog::keyPressEvent(event);
    }
}

void DataProcessDialog::dropEvent(QDropEvent *event) {
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
        textEdit->setText(text);
    } else {
        event->ignore();
    }
}

void DataProcessDialog::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasFormat("text/uri-list")) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void DataProcessDialog::deleteFrame() {
    auto prefix = framePrefixLineEdit->text().trimmed();
    auto suffix = frameSuffixLineEdit->text().trimmed();
    if (prefix.isEmpty() || suffix.isEmpty()) {
        return;
    }

    auto text = textEdit->toPlainText();
    if (text.isEmpty()) {
        return;
    }
    QString t = text;
    QTextStream in(&t);
    QStringList lines;
    auto separator = ' ';
    while (!in.atEnd()) {
        auto line = in.readLine();
        lines.append(line);
    }
    text = lines.join(separator).simplified();
    int prefixIndex = -1;
    int suffixIndex = -1;

    prefixIndex = text.indexOf(prefix);

    while (prefixIndex >= 0) {
        suffixIndex = text.indexOf(suffix, prefixIndex + prefix.length());
        if (suffixIndex >= 0) {
            text = text.remove(prefixIndex, suffixIndex - prefixIndex + suffix.length());
        }
    }
    textEdit->setText(text);
}
