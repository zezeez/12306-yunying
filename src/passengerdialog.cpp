#include "passengerdialog.h"
#include "userdata.h"
#include <QWidget>
#include <QListWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QRadioButton>
#include <QButtonGroup>
#include <QCheckBox>
#include <QSettings>
#include "mainwindow.h"
#include "nethelper.h"

extern MainWindow *w;

PassengerDialog::PassengerDialog(QWidget *parent) :
    QDialog(parent)
{
    dListWidget = new DoubleListWidget;
}

PassengerDialog::~PassengerDialog()
{
}

void PassengerDialog::setUp()
{
    dListWidget->setListMinimumWidth(100);
    dListWidget->setLeftTitle(tr("未选中的乘车人："));
    dListWidget->setRightTitle(tr("已选中的乘车人："));
    connect(dListWidget, &DoubleListWidget::listCountChanged, this, &PassengerDialog::updateSelectedTips);

    QVBoxLayout *outvLayout = new QVBoxLayout;
    outvLayout->addWidget(dListWidget);

    QVBoxLayout *vLayout4 = new QVBoxLayout;
    QButtonGroup *btnGroup = new QButtonGroup;
    QRadioButton *rb;
    QLabel *label = new QLabel(QStringLiteral("余票不足时："));
    vLayout4->addWidget(label);
    rb = new QRadioButton(QStringLiteral("部分提交(按选中乘车人的顺序)"));
    connect(rb, &QRadioButton::toggled, this, [] (bool checked) {
        UserData *ud = UserData::instance();
        ud->grabSetting.ticketSetting.partialSubmit = checked;
        QSettings setting;
        setting.setValue(_("ticket/partial_submit"), checked);
    });
    btnGroup->addButton(rb);
    vLayout4->addWidget(rb);
    rb->setChecked(true);
    UserData *ud = UserData::instance();
    ud->grabSetting.ticketSetting.partialSubmit = true;
    rb = new QRadioButton(QStringLiteral("不提交"));

    btnGroup->addButton(rb);
    vLayout4->addWidget(rb);
#if 0
    QCheckBox *chkBox = new QCheckBox(QStringLiteral("余票无座时不提交"));
    connect(chkBox, &QCheckBox::clicked, this, [&](bool checked) {
        UserData *ud = UserData::instance();
        ud->grabSetting.ticketSetting.noSubmitNoSeat = checked;
    });
    vLayout4->addWidget(chkBox);
#endif

    outvLayout->addLayout(vLayout4);

    QHBoxLayout *hLayout1 = new QHBoxLayout;
    hLayout1->addStretch();
    refreshPassengerPb = new QPushButton(tr("刷新"));
    connect(refreshPassengerPb, &QPushButton::clicked, this, &PassengerDialog::refreshPassengerInfo);
    refreshPassengerPb->setToolTip(tr("若左侧乘车人未显示请刷新一遍"));
    hLayout1->addWidget(refreshPassengerPb);
    QPushButton *pb = new QPushButton(tr("确定"));
    connect(pb, &QPushButton::clicked, this, &PassengerDialog::hide);
    hLayout1->addWidget(pb);
    hLayout1->addStretch();

    outvLayout->addLayout(hLayout1);

    setLayout(outvLayout);
    setWindowTitle(tr("选择乘车人"));
}

void PassengerDialog::refreshPassengerInfo()
{
    NetHelper::instance()->getPassengerInfo();
}

void PassengerDialog::updateSelectedTips(int leftCount, int rightCount)
{
    QString tips = tr("已选%1/%2").arg(rightCount).arg(leftCount + rightCount);
    w->selectedPassengerTipsLabel->setText(tips);
}

const QList<QString> &PassengerDialog::getSelectedPassenger() const
{
    static QList<QString> passengerList;
    const QStringList &slist = dListWidget->rightListContent();
    passengerList.clear();
    passengerList.resize(slist.size());
    for (int i = 0; i < slist.size(); i++) {
        passengerList[i] = slist[i].split('(')[0];
    }
    return passengerList;
}

void PassengerDialog::addSelectedPassenger(const QString &passengerName)
{
    dListWidget->addRightItem(passengerName, Qt::MatchExactly);
}

void PassengerDialog::addUnSelectedPassenger(const QString &passengerName)
{
    dListWidget->addLeftItem(passengerName, Qt::MatchExactly);
}

void PassengerDialog::clearPassenger()
{
    dListWidget->clearList();
}

void PassengerDialog::enterGrabTicketMode()
{
    dListWidget->setDisableEdit();
    refreshPassengerPb->setEnabled(false);
}

void PassengerDialog::exitGrabTicketMode()
{
    dListWidget->setEnableEdit();
    refreshPassengerPb->setEnabled(true);
}
