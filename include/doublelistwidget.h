#ifndef DOUBLELISTWIDGET_H
#define DOUBLELISTWIDGET_H
#include <QObject>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>

class DoubleListWidget: public QWidget
{
    Q_OBJECT
public:
    explicit DoubleListWidget(QWidget *widget = nullptr);
    ~DoubleListWidget();
    QHBoxLayout *layout() const;
    void addLeftItem(const QString &text, enum Qt::MatchFlag flag);
    void delLeftItem(const QString &text, enum Qt::MatchFlag flag);
    void addLeftItem(QListWidgetItem *item);
    void delLeftItem(QListWidgetItem *item);
    void addRightItem(const QString &text, enum Qt::MatchFlag flag);
    void delRightItem(const QString &text, enum Qt::MatchFlag flag);
    void addRightItem(QListWidgetItem *item);
    void delRightItem(QListWidgetItem *item);
    const QList<QListWidgetItem *>findLeftItem(const QString &text, enum Qt::MatchFlag flag);
    const QList<QListWidgetItem *>findRightItem(const QString &text, enum Qt::MatchFlag flag);
    void moveLeftItemToRight(const QString &text, enum Qt::MatchFlag flag);
    void moveRightItemToLeft(const QString &text, enum Qt::MatchFlag flag);
    void moveCurrentLeftItemToRight();
    void moveCurrentLeftItemToRightAll();
    void moveCurrentRightItemToLeft();
    void moveCurrentRightItemToLeftAll();
    void moveRightItemUp();
    void moveRightItemDown();
    void clearLeftList();
    void clearRightList();
    void clearList();
    const QListWidgetItem *currentLeftItem() const;
    const QListWidgetItem *currentRightItem() const;
    QString leftTitle() const;
    void setLeftTitle(const QString &title);
    QString rightTitle() const;
    void setRightTitle(const QString &title);
    size_t leftListCount() const;
    size_t rightListCount() const;
    bool disableEdit() const;
    void setDisableEdit();
    void setEnableEdit();
    int listMinimumWidth() const;
    void setListMinimumWidth(int width);
    int listMaximumWidth() const;
    void setListMaximumWidth(int width);
    int listMinimumHeight() const;
    void setListMinimumHeight(int height);
    int listMaximumHeight() const;
    void setListMaximumHeight(int height);
    const QStringList &leftListContent() const;
    const QStringList &rightListContent() const;
    void setLeftCurrentRow(int row);
    void setRightCurrentRow(int row);

Q_SIGNALS:
    void listCountChanged(int leftCount, int rightCount);

private:
    void updateMoveEnable();

    QListWidget *leftListWidget;
    QListWidget *rightListWidget;
    QPushButton *upButton;
    QPushButton *downButton;
    QPushButton *toLeftButton;
    QPushButton *toRightButton;
    QPushButton *toLeftButtonAll;
    QPushButton *toRightButtonAll;
    QLabel *leftTitleLabel;
    QLabel *rightTitleLabel;
    QHBoxLayout *hlayout;
    QStringList leftListString;
    QStringList rightListString;
    bool isEdit;
};

#endif // DOUBLELISTWIDGET_H
