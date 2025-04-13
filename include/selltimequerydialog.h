#ifndef SELLTIMEQUERYDIALOG_H
#define SELLTIMEQUERYDIALOG_H
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include "completeedit.h"

class SellTimeQueryDialog : public QDialog
{
    Q_OBJECT
public:
    SellTimeQueryDialog(QWidget *parent = nullptr);
    ~SellTimeQueryDialog();
    void setup();
    QString getInputStaCode();
    void queryScName();
    void queryScNameReply(QVariantMap &varMap);
    void queryAllCacheSaleTime();
    void queryAllCacheSaleTimeReply(QVariantMap &varMap);
    void queryLcTime();
    void queryLcTimeReply(QVariantMap &varMap);
    void setQueryText(const QString &text);
    void setCompleter(InputCompleter *ic);

private:
    QLabel *msgLabel;
    QLabel *dispLabel;
    CompleteEdit *queryStaLe;
    QMap<QString, QString> cityStaName;
};

#endif // SELLTIMEQUERYDIALOG_H
