#include "seattypedialog.h"
#include "userdata.h"
#include "12306.h"
#include <QWidget>
#include <QListWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QRadioButton>
#include <QButtonGroup>
#include <QTimer>
#include "mainwindow.h"
#include <QPainter>
#include <QSettings>

#define _ QStringLiteral

extern MainWindow *w;

SeatTypeDialog::SeatTypeDialog(QWidget *parent) :
    QDialog(parent)
{
    dListWidget = new DoubleListWidget;
}

SeatTypeDialog::~SeatTypeDialog()
{
}

void SeatTypeDialog::setUp()
{
    dListWidget->setListMinimumWidth(100);
    //dListWidget->setListMinimumHeight(210);
    dListWidget->setLeftTitle(tr("未选中席别："));
    dListWidget->setRightTitle(tr("已选中席别："));

    QList<QString> seatTypeList = {
        QStringLiteral("优选一等座"),
        QStringLiteral("一等座"),
        QStringLiteral("二等座"),
        QStringLiteral("商务座"),
        QStringLiteral("特等座"),
        QStringLiteral("高级软卧"),
        QStringLiteral("软卧"),
        QStringLiteral("动卧"),
        QStringLiteral("硬卧"),
        QStringLiteral("硬座"),
        QStringLiteral("无座"),
        QStringLiteral("其他"),
    };

    for (auto &seatType : seatTypeList) {
        dListWidget->addLeftItem(seatType, Qt::MatchExactly);
    }
    dListWidget->setLeftCurrentRow(0);

    QSettings setting;
    QList<QString> selSeatTypeList = setting.value(_("seat_type/selected_seat_type")).toStringList();
    for (auto &seatType : selSeatTypeList) {
        dListWidget->moveLeftItemToRight(seatType, Qt::MatchExactly);
    }
    QString tips = tr("已选%1/%2").arg(dListWidget->rightListCount())
                                 .arg(dListWidget->leftListCount() + dListWidget->rightListCount());
    w->selectedSeatTypeTipsLabel->setText(tips);

    connect(dListWidget, &DoubleListWidget::listCountChanged, this, &SeatTypeDialog::updateSelectedTips);

#if 0
    QImage *image = new QImage(32, 32, QImage::Format_RGB32);

    QPainter painter(image); // 创建一个绘制器并将它与 Image 关联起来
    painter.setRenderHint(QPainter::Antialiasing);  //抗锯齿
    painter.setRenderHints(QPainter::SmoothPixmapTransform);  //平滑像素图变换

    //painter.drawRoundedRect(0, 0, 32, 32, 50, 50);
    painter.fillRect(image->rect(), QBrush(QColor(255,127,36))); // 橙色
    //painter.fillRect(image->rect(), QBrush(QColor(255,153,51))); // 淡橙色
    //painter.fillRect(image->rect(), QBrush(Qt::red)); // 红色
    //painter.fillRect(image->rect(), QBrush(QColor(0,205,205))); // cyan
    //painter.fillRect(image->rect(), QBrush(QColor(0,139,139)));

    //QPen pen(Qt::white, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    QPen pen(Qt::white, 1);
    painter.setPen(pen);

    //painter.drawLine(50, 250, 200, 100);  // 绘制直线
    //painter.drawRect(0, 0, 15, 15);  // 绘制矩形
    //painter.drawEllipse(250, 50, 100, 150);  // 绘制椭圆
    QFont font;
    font.setPointSize(18);
    font.setFamily("文泉驿点阵正黑");
    //font.setBold(true);
    painter.setFont(font);

    QString text = tr("动");
    QRect rect = painter.boundingRect(1, 1, 32, 32, Qt::AlignCenter, text);

    painter.drawText(rect, text);


    painter.end();    // 和上面一样，必须要调用 end() 结束绘画
    image->save("1.ico", "ICO");
    delete image;
#endif

    QVBoxLayout *outvLayout = new QVBoxLayout;
    outvLayout->addWidget(dListWidget);

    QVBoxLayout *vLayout4 = new QVBoxLayout;
    QButtonGroup *btnGroup = new QButtonGroup;
    QRadioButton *rb = new QRadioButton(tr("按选中席别的顺序提交"));
    connect(rb, &QRadioButton::toggled, this, [&](bool checked) {
        UserData *ud = UserData::instance();
        ud->grabSetting.seatTypePrio.strictSeatTypePrio = checked;
        QSettings setting;
        setting.setValue(_("seat_type/strict_seat_type"), checked);
    });
    bool checked = setting.value(_("seat_type/strict_seat_type"), false).value<bool>();
    rb->setChecked(checked);
    btnGroup->addButton(rb);
    vLayout4->addWidget(rb);
    rb = new QRadioButton(tr("余票充足的席别优先提交"));
    connect(rb, &QRadioButton::toggled, this, [&](bool checked) {
        UserData *ud = UserData::instance();
        ud->grabSetting.seatTypePrio.sufficientSeatTypePrio = checked;
        QSettings setting;
        setting.setValue(_("seat_type/sufficient_seat_type"), checked);
    });
    checked = setting.value(_("seat_type/sufficient_seat_type"), false).value<bool>();
    rb->setChecked(checked);
    btnGroup->addButton(rb);
    vLayout4->addWidget(rb);
    rb = new QRadioButton(tr("余票充足时价格低的席别优先提交"));
    connect(rb, &QRadioButton::toggled, this, [&](bool checked) {
        UserData *ud = UserData::instance();
        ud->grabSetting.seatTypePrio.priceLowSeatTypePrioWhensufficient = checked;
        QSettings setting;
        setting.setValue(_("seat_type/price_low_sufficient"), checked);
    });
    checked = setting.value(_("seat_type/price_low_sufficient"), true).value<bool>();
    rb->setChecked(checked);
    btnGroup->addButton(rb);
    vLayout4->addWidget(rb);
    //rb->setChecked(true);
    rb = new QRadioButton(tr("价格低的席别优先提交"));
    connect(rb, &QRadioButton::toggled, this, [&](bool checked) {
        UserData *ud = UserData::instance();
        ud->grabSetting.seatTypePrio.priceLowSeatTypePrio = checked;
        QSettings setting;
        setting.setValue(_("seat_type/price_low_seat_type"), checked);
    });
    checked = setting.value(_("seat_type/price_low_seat_type"), false).value<bool>();
    rb->setChecked(checked);
    btnGroup->addButton(rb);
    vLayout4->addWidget(rb);
    rb = new QRadioButton(tr("价格高的席别优先提交"));
    connect(rb, &QRadioButton::toggled, this, [&](bool checked) {
        UserData *ud = UserData::instance();
        ud->grabSetting.seatTypePrio.priceHighSeatTypePrio = checked;
        QSettings setting;
        setting.setValue(_("seat_type/price_high_seat_type"), checked);
    });
    checked = setting.value(_("seat_type/price_high_seat_type"), false).value<bool>();
    rb->setChecked(checked);
    btnGroup->addButton(rb);
    vLayout4->addWidget(rb);

    outvLayout->addLayout(vLayout4);

    QPushButton *pb = new QPushButton(tr("确定"));
    connect(pb, &QPushButton::clicked, this, &SeatTypeDialog::hide);
    QHBoxLayout *hLayout1 = new QHBoxLayout;
    hLayout1->addStretch();
    hLayout1->addWidget(pb);
    hLayout1->addStretch();

    outvLayout->addLayout(hLayout1);

    setLayout(outvLayout);
    setWindowTitle(tr("选择席别"));
}

void SeatTypeDialog::updateSelectedTips(int leftCount, int rightCount)
{
    QString tips = tr("已选%1/%2").arg(rightCount).arg(leftCount + rightCount);
    w->selectedSeatTypeTipsLabel->setText(tips);
    QSettings setting;
    setting.setValue(_("seat_type/selected_seat_type"), getSelectedSeatType().toList());
}

const QList<QString> &SeatTypeDialog::getSelectedSeatType() const
{
    static QList<QString> seatTypeList;
    const QStringList &slist = dListWidget->rightListContent();
    seatTypeList.clear();
    seatTypeList.resize(slist.size());
    for (int i = 0; i < slist.size(); i++) {
        seatTypeList[i] = slist[i];
    }

    return seatTypeList;
}

void SeatTypeDialog::enterGrabTicketMode()
{
    dListWidget->setDisableEdit();
}

void SeatTypeDialog::exitGrabTicketMode()
{
    dListWidget->setEnableEdit();
}
