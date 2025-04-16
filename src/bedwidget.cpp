#include "bedwidget.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

#define _ QStringLiteral

BedWidget::BedWidget(QWidget *widget): QWidget(widget)
{
    for (int i = 0; i < 3; i++) {
        bedNumSbox[i] = new QSpinBox;
        bedNumSbox[i]->setMinimum(0);
        bedNumSbox[i]->setMaximum(5);
        bedNumSbox[i]->setValue(0);
    }
    QLabel *label = new QLabel;
    label->setText(_("上铺："));
    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(label);
    bedNumSbox[0]->setFixedSize(100, 25);
    hLayout->addWidget(bedNumSbox[0]);
    hLayout->addStretch();
    QVBoxLayout *vLayout = new QVBoxLayout;
    vLayout->addLayout(hLayout);

    label = new QLabel;
    label->setText(_("中铺："));
    hLayout = new QHBoxLayout;
    hLayout->addWidget(label);
    bedNumSbox[1]->setFixedSize(100, 25);
    hLayout->addWidget(bedNumSbox[1]);
    hLayout->addStretch();
    vLayout->addStretch();
    vLayout->addLayout(hLayout);
    vLayout->addStretch();

    label = new QLabel;
    label->setText(_("下铺："));
    hLayout = new QHBoxLayout;
    hLayout->addWidget(label);
    bedNumSbox[2]->setFixedSize(100, 25);
    hLayout->addWidget(bedNumSbox[2]);
    hLayout->addStretch();
    vLayout->addLayout(hLayout);
    vLayout->addStretch();

    setLayout(vLayout);
}

BedWidget::~BedWidget()
{

}

void BedWidget::clearSelectedBeds()
{
    for (int i = 0; i < 3; i++) {
        bedNumSbox[i]->setValue(0);
    }
}

int BedWidget::getChoosedBeds(enum BedPosition pos)
{
    if (pos < 0 || pos > BEDLOW) {
        return 0;
    }

    return bedNumSbox[pos]->value();
}
