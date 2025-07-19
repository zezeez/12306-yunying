#include "doublelistwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

DoubleListWidget::DoubleListWidget(QWidget *widget): QWidget(widget)
{
    QFont font;
    font.setPointSize(10);

    leftTitleLabel = new QLabel;
    leftListWidget = new QListWidget;
    rightTitleLabel = new QLabel;
    rightListWidget = new QListWidget;

    QVBoxLayout *vLayout = new QVBoxLayout;
    leftTitleLabel->setFont(font);
    vLayout->addWidget(leftTitleLabel);
    vLayout->addWidget(leftListWidget);

    QVBoxLayout *vLayout2 = new QVBoxLayout;
    vLayout2->addStretch();
    toRightButton = new QPushButton;
    toRightButton->setIcon(QIcon(QStringLiteral(":/icon/images/arrow_right.png")));
    connect(toRightButton, &QPushButton::clicked, this, &DoubleListWidget::moveCurrentLeftItemToRight);
    toRightButton->setEnabled(false);
    vLayout2->addWidget(toRightButton);

    toLeftButton = new QPushButton;
    toLeftButton->setIcon(QIcon(QStringLiteral(":/icon/images/arrow_left.png")));
    connect(toLeftButton, &QPushButton::clicked, this, &DoubleListWidget::moveCurrentRightItemToLeft);
    toLeftButton->setEnabled(false);
    vLayout2->addWidget(toLeftButton);
    vLayout2->addStretch();
    upButton = new QPushButton;
    upButton->setIcon(QIcon(QStringLiteral(":/icon/images/arrow_up.png")));
    connect(upButton, &QPushButton::clicked, this, &DoubleListWidget::moveRightItemUp);
    vLayout2->addWidget(upButton);
    downButton = new QPushButton;
    downButton->setIcon(QIcon(QStringLiteral(":/icon/images/arrow_down.png")));
    connect(downButton, &QPushButton::clicked, this, &DoubleListWidget::moveRightItemDown);
    vLayout2->addWidget(downButton);
    vLayout2->addStretch();

    toRightButtonAll = new QPushButton;
    toRightButtonAll->setIcon(QIcon(QStringLiteral(":/icon/images/double_arrow_right.png")));
    connect(toRightButtonAll, &QPushButton::clicked, this, &DoubleListWidget::moveCurrentLeftItemToRightAll);
    toRightButtonAll->setEnabled(false);
    vLayout2->addWidget(toRightButtonAll);

    toLeftButtonAll = new QPushButton;
    toLeftButtonAll->setIcon(QIcon(QStringLiteral(":/icon/images/double_arrow_left.png")));
    connect(toLeftButtonAll, &QPushButton::clicked, this, &DoubleListWidget::moveCurrentRightItemToLeftAll);
    toLeftButtonAll->setEnabled(false);
    vLayout2->addWidget(toLeftButtonAll);


    QVBoxLayout *vLayout3 = new QVBoxLayout;
    rightTitleLabel->setFont(font);
    vLayout3->addWidget(rightTitleLabel);
    vLayout3->addWidget(rightListWidget);

    hlayout = new QHBoxLayout;
    hlayout->addLayout(vLayout);
    hlayout->addLayout(vLayout2);
    hlayout->addLayout(vLayout3);
    setLayout(hlayout);

    isEdit = true;
}

DoubleListWidget::~DoubleListWidget()
{

}

QHBoxLayout *DoubleListWidget::layout() const
{
    return hlayout;
}

void DoubleListWidget::addLeftItem(const QString &text, enum Qt::MatchFlag flag)
{
    QListWidgetItem *item;
    QList<QListWidgetItem *> itemList = rightListWidget->findItems(text, flag);
    if (itemList.isEmpty()) {
        item = new QListWidgetItem;
    } else {
        item = rightListWidget->takeItem(rightListWidget->row(itemList[0]));
    }
    item->setText(text);
    addLeftItem(item);
}

void DoubleListWidget::delLeftItem(const QString &text, enum Qt::MatchFlag flag)
{
    QList<QListWidgetItem *> itemList = leftListWidget->findItems(text, flag);
    if (itemList.isEmpty()) {
        return;
    }
    delLeftItem(itemList[0]);
}

void DoubleListWidget::addLeftItem(QListWidgetItem *item)
{
    leftListWidget->addItem(item);
    leftListWidget->setCurrentItem(item);
    toRightButton->setEnabled(true);
    toRightButtonAll->setEnabled(true);
    leftListString.append(item->text());
    updateMoveEnable();
    emit listCountChanged(leftListWidget->count(), rightListWidget->count());
}

void DoubleListWidget::delLeftItem(QListWidgetItem *item)
{
    leftListWidget->takeItem(leftListWidget->row(item));
    leftListString.removeOne(item->text());
    delete item;
    updateMoveEnable();
    emit listCountChanged(leftListWidget->count(), rightListWidget->count());
}

void DoubleListWidget::addRightItem(const QString &text,  enum Qt::MatchFlag flag)
{
    QListWidgetItem *item;
    QList<QListWidgetItem *> itemList = leftListWidget->findItems(text, flag);
    if (itemList.isEmpty()) {
        item = new QListWidgetItem;
    } else {
        item = leftListWidget->takeItem(leftListWidget->row(itemList[0]));
    }
    item->setText(text);
    addRightItem(item);
}

void DoubleListWidget::delRightItem(const QString &text, enum Qt::MatchFlag flag)
{
    QList<QListWidgetItem *> itemList = leftListWidget->findItems(text, flag);
    if (itemList.isEmpty()) {
        return;
    }
    delRightItem(itemList[0]);
}

void DoubleListWidget::addRightItem(QListWidgetItem *item)
{
    rightListWidget->addItem(item);
    rightListWidget->setCurrentItem(item);
    toLeftButton->setEnabled(true);
    toLeftButtonAll->setEnabled(true);
    rightListString.append(item->text());
    updateMoveEnable();
    emit listCountChanged(leftListWidget->count(), rightListWidget->count());
}

void DoubleListWidget::delRightItem(QListWidgetItem *item)
{
    rightListWidget->takeItem(rightListWidget->row(item));
    rightListString.removeOne(item->text());
    delete item;
    updateMoveEnable();
    emit listCountChanged(leftListWidget->count(), rightListWidget->count());
}

const QList<QListWidgetItem *>DoubleListWidget::findLeftItem(const QString &text, enum Qt::MatchFlag flag)
{
    QList<QListWidgetItem *> itemList = leftListWidget->findItems(text, flag);

    return itemList;
}

const QList<QListWidgetItem *>DoubleListWidget::findRightItem(const QString &text, enum Qt::MatchFlag flag)
{
    QList<QListWidgetItem *> itemList = rightListWidget->findItems(text, flag);

    return itemList;
}

void DoubleListWidget::moveLeftItemToRight(const QString &text, enum Qt::MatchFlag flag)
{
    QListWidgetItem *item;
    QList<QListWidgetItem *> itemList = leftListWidget->findItems(text, flag);
    if (!itemList.isEmpty()) {
        item = leftListWidget->takeItem(leftListWidget->row(itemList[0]));
        rightListWidget->addItem(item);
        leftListString.removeOne(item->text());
        rightListString.append(item->text());
        updateMoveEnable();
        emit listCountChanged(leftListWidget->count(), rightListWidget->count());
    }
}

void DoubleListWidget::moveRightItemToLeft(const QString &text, enum Qt::MatchFlag flag)
{
    QListWidgetItem *item;
    QList<QListWidgetItem *> itemList = rightListWidget->findItems(text, flag);
    if (!itemList.isEmpty()) {
        item = rightListWidget->takeItem(rightListWidget->row(itemList[0]));
        leftListWidget->addItem(item);
        rightListString.removeOne(item->text());
        leftListString.append(item->text());
        updateMoveEnable();
        emit listCountChanged(leftListWidget->count(), rightListWidget->count());
    }
}

void DoubleListWidget::moveCurrentLeftItemToRight()
{
    QListWidgetItem *item;
    QListWidgetItem *leftItem = leftListWidget->currentItem();

    if (leftItem) {
        QList<QListWidgetItem *> itemList = rightListWidget->findItems(leftItem->text(), Qt::MatchExactly);
        item = leftListWidget->takeItem(leftListWidget->currentRow());
        leftListString.removeOne(item->text());
        if (itemList.isEmpty()) {
            rightListWidget->addItem(item);
            rightListWidget->setCurrentItem(item);
            rightListString.append(item->text());
        } else {
            delete item;
        }
        updateMoveEnable();
        emit listCountChanged(leftListWidget->count(), rightListWidget->count());
    }
}

void DoubleListWidget::moveCurrentLeftItemToRightAll()
{
    QListWidgetItem *item;

    if (leftListWidget->count()) {
        leftListWidget->setCurrentRow(leftListWidget->count() - 1);
        leftListString.clear();
        while (leftListWidget->count()) {
            item = leftListWidget->takeItem(leftListWidget->currentRow());
            rightListWidget->addItem(item);
            rightListString.append(item->text());
        }
        rightListWidget->setCurrentRow(0);
        updateMoveEnable();
        emit listCountChanged(0, rightListWidget->count());
    }
}

void DoubleListWidget::moveCurrentRightItemToLeft()
{
    QListWidgetItem *item;
    QListWidgetItem *rightItem = rightListWidget->currentItem();

    if (rightItem) {
        QList<QListWidgetItem *> itemList = leftListWidget->findItems(rightItem->text(), Qt::MatchExactly);
        item = rightListWidget->takeItem(rightListWidget->currentRow());
        rightListString.removeOne(item->text());
        if (itemList.isEmpty()) {
            leftListWidget->addItem(item);
            leftListWidget->setCurrentItem(item);
            leftListString.append(item->text());
        } else {
            delete item;
        }
        updateMoveEnable();
        emit listCountChanged(leftListWidget->count(), rightListWidget->count());
    }
}

void DoubleListWidget::moveCurrentRightItemToLeftAll()
{
    QListWidgetItem *item;

    if (rightListWidget->count()) {
        rightListWidget->setCurrentRow(rightListWidget->count() - 1);
        rightListString.clear();
        while (rightListWidget->count()) {
            item = rightListWidget->takeItem(rightListWidget->currentRow());
            leftListWidget->addItem(item);
            leftListString.append(item->text());
        }
        leftListWidget->setCurrentRow(0);
        updateMoveEnable();
        emit listCountChanged(leftListWidget->count(), 0);
    }
}

void DoubleListWidget::moveRightItemUp()
{
    QListWidgetItem *item = rightListWidget->currentItem();
    if (item) {
        int row = rightListWidget->row(item);
        if (row > 0) {
            rightListWidget->takeItem(row);
            rightListWidget->insertItem(row - 1, item);
            rightListWidget->setCurrentItem(item);
            rightListString.removeOne(item->text());
            rightListString.insert(row - 1, item->text());
        }
    }
}

void DoubleListWidget::moveRightItemDown()
{
    QListWidgetItem *item = rightListWidget->currentItem();
    if (item) {
        int row = rightListWidget->row(item);
        if (row < rightListWidget->count() - 1) {
            rightListWidget->takeItem(row);
            rightListWidget->insertItem(row + 1, item);
            rightListWidget->setCurrentItem(item);
            rightListString.removeOne(item->text());
            rightListString.insert(row + 1, item->text());
        }
    }
}

void DoubleListWidget::clearLeftList()
{
    if (leftListWidget->count()) {
        leftListWidget->setCurrentRow(leftListWidget->count() - 1);
        while (leftListWidget->count()) {
            QListWidgetItem *item = leftListWidget->takeItem(leftListWidget->currentRow());
            delete item;
        }
        leftListString.clear();
        updateMoveEnable();
        emit listCountChanged(0, rightListWidget->count());
    }
}

void DoubleListWidget::clearRightList()
{
    if (rightListWidget->count()) {
        rightListWidget->setCurrentRow(rightListWidget->count() - 1);
        while (rightListWidget->count()) {
            QListWidgetItem *item = rightListWidget->takeItem(rightListWidget->currentRow());
            delete item;
        }
        rightListString.clear();
        updateMoveEnable();
        emit listCountChanged(leftListWidget->count(), 0);
    }
}

void DoubleListWidget::clearList()
{
    clearLeftList();
    clearRightList();
    emit listCountChanged(0, 0);
}

const QListWidgetItem *DoubleListWidget::currentLeftItem() const
{
    return leftListWidget->currentItem();
}

const QListWidgetItem *DoubleListWidget::currentRightItem() const
{
    return rightListWidget->currentItem();
}

QString DoubleListWidget::leftTitle() const
{
    return leftTitleLabel->text();
}

void DoubleListWidget::setLeftTitle(const QString &text)
{
    leftTitleLabel->setText(text);
}

QString DoubleListWidget::rightTitle() const
{
    return rightTitleLabel->text();
}

void DoubleListWidget::setRightTitle(const QString &text)
{
    rightTitleLabel->setText(text);
}

size_t DoubleListWidget::leftListCount() const
{
    return leftListWidget->count();
}

size_t DoubleListWidget::rightListCount() const
{
    return rightListWidget->count();
}

bool DoubleListWidget::disableEdit() const
{
    return isEdit;
}

void DoubleListWidget::setDisableEdit()
{
    isEdit = false;
    toLeftButton->setEnabled(false);
    toLeftButtonAll->setEnabled(false);
    toRightButton->setEnabled(false);
    toRightButtonAll->setEnabled(false);
    upButton->setEnabled(false);
    downButton->setEnabled(false);
}

void DoubleListWidget::setEnableEdit()
{
    isEdit = true;
    toLeftButton->setEnabled(rightListWidget->count() != 0);
    toLeftButtonAll->setEnabled(rightListWidget->count() != 0);
    toRightButton->setEnabled(leftListWidget->count() != 0);
    toRightButtonAll->setEnabled(leftListWidget->count() != 0);
    upButton->setEnabled(true);
    downButton->setEnabled(true);
}

int DoubleListWidget::listMinimumWidth() const
{
    return leftListWidget->minimumWidth();
}

void DoubleListWidget::setListMinimumWidth(int width)
{
    leftListWidget->setMinimumWidth(width);
    rightListWidget->setMinimumWidth(width);
}

int DoubleListWidget::listMaximumWidth() const
{
    return leftListWidget->maximumWidth();
}

void DoubleListWidget::setListMaximumWidth(int width)
{
    leftListWidget->setMaximumWidth(width);
    rightListWidget->setMaximumWidth(width);
}

int DoubleListWidget::listMinimumHeight() const
{
    return leftListWidget->minimumHeight();
}

void DoubleListWidget::setListMinimumHeight(int height)
{
    leftListWidget->setMinimumHeight(height);
    rightListWidget->setMinimumHeight(height);
}

int DoubleListWidget::listMaximumHeight() const
{
    return leftListWidget->maximumHeight();
}

void DoubleListWidget::setListMaximumHeight(int height)
{
    leftListWidget->setMaximumHeight(height);
    rightListWidget->setMaximumHeight(height);
}

const QStringList &DoubleListWidget::leftListContent() const
{
    return leftListString;
}

const QStringList &DoubleListWidget::rightListContent() const
{
    return rightListString;
}

void DoubleListWidget::updateMoveEnable()
{
    toLeftButton->setEnabled(rightListWidget->count() != 0);
    toLeftButtonAll->setEnabled(rightListWidget->count() != 0);
    toRightButton->setEnabled(leftListWidget->count() != 0);
    toRightButtonAll->setEnabled(leftListWidget->count() != 0);
}

void DoubleListWidget::setLeftCurrentRow(int row)
{
    leftListWidget->setCurrentRow(row);
}

void DoubleListWidget::setRightCurrentRow(int row)
{
    leftListWidget->setCurrentRow(row);
}
