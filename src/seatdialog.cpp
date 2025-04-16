#include "seatdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTabWidget>
#include "mainwindow.h"
#include "passengerdialog.h"

#define _ QStringLiteral
#define SEATTYPEMAX 5

extern MainWindow *w;

SeatDialog::SeatDialog(QWidget *parent) :
    QDialog(parent)
{
    QVBoxLayout *vlayout;
    QHBoxLayout *hlayout;
    QHBoxLayout *hlayout1;
    QHBoxLayout *hlayout2;
    QStringList desc = {
        _("A"),
        _("B"),
        _("C"),
        _("D"),
        _("F"),
    };
    QStringList itemDesc = {
        _("一等座"),
        _("二等座"),
        _("特等座"),
        _("商务座"),
        _("优选一等座")
    };
    seatTypeCB = new QComboBox;
    seatTypeCB->addItems(itemDesc);
    connect(seatTypeCB, &QComboBox::currentIndexChanged, this, &SeatDialog::showSeatType);

    seatDescLB.resize(10);
    seatVCL.resize(10);
    seatSelected.fill(false, 50);

    hlayout = new QHBoxLayout;
    hlayout->addWidget(seatTypeCB);
    hlayout->addStretch();
    hlayout1 = new QHBoxLayout;
    hlayout1->addStretch();
    // 第一排
    QLabel *label = new QLabel(tr("窗"));
    hlayout1->addWidget(label);
    for (int i = 0; i < 5; i++) {
        seatDescLB[i] = new QLabel(desc[i]);
        seatDescLB[i]->setAlignment(Qt::AlignCenter);
        seatVCL[i] = new ClickLabel;
        connect(seatVCL[i], &ClickLabel::clicked, this, [=] () {
            QPixmap px;
            int index = seatTypeCB->currentIndex();
            Q_ASSERT(index < SEATTYPEMAX);
            seatSelected[index * 10 + i] = !seatSelected[index * 10 + i];
            // 0-9 一等座
            // 10-19 二等座
            // 20-29 特等座
            // 30-39 商务座
            // 40-49 优选一等座
            if (seatSelected[index * 10 + i]) {
                px.load(_(":/icon/images/choosed_seat.png"));
                seatVCL[i]->setPixmap(px);
            } else {
                px.load(_(":/icon/images/unchoosed_seat.png"));
                seatVCL[i]->setPixmap(px);
            }
            updateSelectedTips();
        });
        vlayout = new QVBoxLayout;
        vlayout->addWidget(seatDescLB[i]);
        vlayout->addWidget(seatVCL[i]);
        hlayout1->addLayout(vlayout);
        if (i == 2) {
            QLabel *label = new QLabel(tr("过道"));
            hlayout1->addWidget(label);
        }
    }
    label = new QLabel(tr("窗"));
    hlayout1->addWidget(label);
    hlayout1->addStretch();
    hlayout2 = new QHBoxLayout;
    hlayout2->addStretch();
    // 第二排
    label = new QLabel(tr("窗"));
    hlayout2->addWidget(label);
    for (int i = 0; i < 5; i++) {
        seatDescLB[i + 5] = new QLabel(desc[i]);
        seatDescLB[i + 5]->setAlignment(Qt::AlignCenter);
        seatVCL[i + 5] = new ClickLabel;
        connect(seatVCL[i + 5], &ClickLabel::clicked, this, [=] () {
            QPixmap px;
            int index = seatTypeCB->currentIndex();
            Q_ASSERT(index < SEATTYPEMAX);
            seatSelected[index * 10 + i + 5] = !seatSelected[index * 10 + i + 5];
            // 0-9 一等座
            // 10-19 二等座
            // 20-29 特等座
            // 30-39 商务座
            // 40-49 优选一等座
            if (seatSelected[index * 10 + i + 5]) {
                px.load(_(":/icon/images/choosed_seat.png"));
                seatVCL[i + 5]->setPixmap(px);
            } else {
                px.load(_(":/icon/images/unchoosed_seat.png"));
                seatVCL[i + 5]->setPixmap(px);
            }
            updateSelectedTips();
        });
        vlayout = new QVBoxLayout;
        vlayout->addWidget(seatDescLB[i + 5]);
        vlayout->addWidget(seatVCL[i + 5]);
        hlayout2->addLayout(vlayout);
        if (i == 2) {
            QLabel *label = new QLabel(tr("过道"));
            hlayout2->addWidget(label);
        }
    }
    label = new QLabel(tr("窗"));
    hlayout2->addWidget(label);
    hlayout2->addStretch();
    vlayout = new QVBoxLayout;
    vlayout->addLayout(hlayout);
    vlayout->addLayout(hlayout1);
    vlayout->addLayout(hlayout2);
    vlayout->addStretch();

    QHBoxLayout *hLayout1 = new QHBoxLayout;
    hLayout1->addStretch();
    QPushButton *pb = new QPushButton(tr("清空"));
    connect(pb, &QPushButton::clicked, this, &SeatDialog::clearSelectedSeats);
    hLayout1->addWidget(pb);
    pb = new QPushButton(tr("确定"));
    connect(pb, &QPushButton::clicked, this, &SeatDialog::hide);
    hLayout1->addWidget(pb);
    hLayout1->addStretch();
    vlayout->addLayout(hLayout1);

    seatTypeCB->setCurrentIndex(1);

    setWindowTitle(_("选座/铺"));
    QTabWidget *tabWidget = new QTabWidget;
    QWidget *widget = new QWidget;
    widget->setLayout(vlayout);
    tabWidget->addTab(widget, _("选座"));

    bedWidget = new BedWidget;

    QVBoxLayout *vlayout1 = new QVBoxLayout;
    vlayout1->addWidget(bedWidget);

    hlayout1 = new QHBoxLayout;
    hlayout1->addStretch();
    pb = new QPushButton;
    pb->setText(_("清空"));
    connect(pb, &QPushButton::clicked, this, &SeatDialog::clearSelectedBeds);
    hlayout1->addWidget(pb);
    pb = new QPushButton;
    pb->setText(_("确定"));
    connect(pb, &QPushButton::clicked, this, &SeatDialog::hide);
    hlayout1->addWidget(pb);
    hlayout1->addStretch();
    vlayout1->addLayout(hlayout1);

    widget = new QWidget;
    widget->setLayout(vlayout1);
    tabWidget->addTab(widget, _("选铺"));
    tabWidget->setCurrentIndex(0);

    hlayout = new QHBoxLayout;
    hlayout->addWidget(tabWidget);
    setLayout(hlayout);
    resize(300, 280);
}

SeatDialog::~SeatDialog()
{

}

void SeatDialog::showSeatType(int index)
{
    QPixmap px, px2;
    px.load(_(":/icon/images/unchoosed_seat.png"));
    px2.load(_(":/icon/images/choosed_seat.png"));

    for (int i = 0; i < seatVCL.size(); i++) {
        seatVCL[i]->hide();
        seatDescLB[i]->hide();
    }
    switch (index) {
    // 一等座
    case 0:
        for (int i = 0; i < 10; i++) {
            if (i != 1 && i != 6) {
                if (seatSelected[i]) {
                    seatVCL[i]->setPixmap(px2);
                } else {
                    seatVCL[i]->setPixmap(px);
                }
                seatVCL[i]->show();
                seatDescLB[i]->show();
            }
        }
        break;
    // 二等座
    case 1:
        for (int i = 0; i < 10; i++) {
            if (seatSelected[i + 10]) {
                seatVCL[i]->setPixmap(px2);
            } else {
                seatVCL[i]->setPixmap(px);
            }
            seatVCL[i]->show();
            seatDescLB[i]->show();
        }
        break;
    // 特等座
    case 2:
        for (int i = 0; i < 10; i++) {
            if (i != 1 && i != 6 &&
                i != 3 && i != 8) {
                if (seatSelected[i + 20]) {
                    seatVCL[i]->setPixmap(px2);
                } else {
                    seatVCL[i]->setPixmap(px);
                }
                seatVCL[i]->show();
                seatDescLB[i]->show();
            }
        }
        break;
    // 商务座
    case 3:
        for (int i = 0; i < 10; i++) {
            if (i != 1 && i != 6 &&
                i != 3 && i != 8) {
                if (seatSelected[i + 30]) {
                    seatVCL[i]->setPixmap(px2);
                } else {
                    seatVCL[i]->setPixmap(px);
                }
                seatVCL[i]->show();
                seatDescLB[i]->show();
            }
        }
        break;
    // 优选一等座
    case 4:
        for (int i = 0; i < 10; i++) {
            if (i != 1 && i != 6) {
                if (seatSelected[i + 40]) {
                    seatVCL[i]->setPixmap(px2);
                } else {
                    seatVCL[i]->setPixmap(px);
                }
                seatVCL[i]->show();
                seatDescLB[i]->show();
            }
        }
        break;
    default:
        break;
    }
}

void SeatDialog::updateSelectedTips()
{
    int max = 0, t = 0;
    for (int i = 0; i < SEATTYPEMAX; i ++) {
        t = 0;
        for (int j = 0; j < 10; j++) {
            t += seatSelected[i * 10 + j];
        }
        max = t > max ? t : max;
    }
    QString tips = tr("已选%1/%2").arg(max).arg(w->passengerDialog->getSelectedPassenger().size());
    w->selectedSeatTipsLabel->setText(tips);
}

void SeatDialog::clearSelectedSeats()
{
    QPixmap px;
    px.load(_(":/icon/images/unchoosed_seat.png"));

    for (int i = 0; i < 10; i++) {
        seatSelected[i] = false;
        seatVCL[i]->setPixmap(px);
    }
    for (int i = 10; i < 50; i++) {
        seatSelected[i] = false;
    }
    QString tips = tr("已选0/%1").arg(w->passengerDialog->getSelectedPassenger().size());
    w->selectedSeatTipsLabel->setText(tips);
}

void SeatDialog::clearSelectedBeds()
{
    bedWidget->clearSelectedBeds();
}

QString SeatDialog::getChoosedSeats(QChar seatType)
{
    char c = seatType.toLatin1();
    const QStringList seatNo = {
        _("1A"),
        _("1B"),
        _("1C"),
        _("1D"),
        _("1F"),
        _("2A"),
        _("2B"),
        _("2C"),
        _("2D"),
        _("2F"),
    };
    QString selectedSeat;

    switch (c) {
    case 'M':
        for (int i = 0; i < 10; i++) {
            if (seatSelected[i]) {
                selectedSeat.append(seatNo[i]);
            }
        }
        break;
    case 'O':
        for (int i = 0; i < 10; i++) {
            if (seatSelected[i + 10]) {
                selectedSeat.append(seatNo[i]);
            }
        }
        break;
    case 'P':
        for (int i = 0; i < 10; i++) {
            if (seatSelected[i + 20]) {
                selectedSeat.append(seatNo[i]);
            }
        }
        break;
    case '9':
        for (int i = 0; i < 10; i++) {
            if (seatSelected[i + 30]) {
                selectedSeat.append(seatNo[i]);
            }
        }
        break;
    case 'D':
        for (int i = 0; i < 10; i++) {
            if (seatSelected[i + 40]) {
                selectedSeat.append(seatNo[i]);
            }
        }
        break;
    default:
        break;
    }
    return selectedSeat;
}

int SeatDialog::getChoosedBeds(enum BedPosition pos)
{
    return bedWidget->getChoosedBeds(pos);
}
