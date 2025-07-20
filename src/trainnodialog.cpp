#include "trainnodialog.h"
#include "userdata.h"
#include <QWidget>
#include <QListWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QStandardItemModel>
#include <QRadioButton>
#include <QButtonGroup>
#include <QGroupBox>
#include <QTimeEdit>
#include <QSettings>
#include "mainwindow.h"

#define _ QStringLiteral

extern MainWindow *w;

TrainNoDialog::TrainNoDialog(QWidget *parent) :
    QDialog(parent)
{
    dListWidget = new DoubleListWidget;
}

TrainNoDialog::~TrainNoDialog()
{
}

void TrainNoDialog::setUp()
{
    QFont font;

    dListWidget->setMinimumWidth(300);
    dListWidget->setLeftTitle(tr("未选中车次："));
    dListWidget->setRightTitle(tr("已选中车次："));
    connect(dListWidget, &DoubleListWidget::listCountChanged, this, &TrainNoDialog::updateSelectedTips);

    QVBoxLayout *outvLayout = new QVBoxLayout;
    outvLayout->addWidget(dListWidget);

    font.setPointSize(10);
    QVBoxLayout *vLayout4 = new QVBoxLayout;
    QButtonGroup *btnGroup = new QButtonGroup;
    QRadioButton *rb = new QRadioButton(tr("按选中车次的顺序提交"));
    rb->setFont(font);
    connect(rb, &QRadioButton::toggled, this, [&](bool checked) {
        UserData *ud = UserData::instance();
        ud->grabSetting.trainPrio.strictTrainPrio = checked;
        QSettings setting;
        setting.setValue(_("train/strict_train"), checked);
    });
    QSettings setting;
    bool checked = setting.value(_("train/strict_train"), false).value<bool>();
    rb->setChecked(checked);
    rb->setFont(font);
    btnGroup->addButton(rb);
    vLayout4->addWidget(rb);
    rb = new QRadioButton(tr("按列车发车时间顺序提交"));
    rb->setFont(font);
    connect(rb, &QRadioButton::toggled, this, [&](bool checked) {
        UserData *ud = UserData::instance();
        ud->grabSetting.trainPrio.strictStartTimePrio = checked;
        QSettings setting;
        setting.setValue(_("train/strict_start_time"), checked);
    });
    checked = setting.value(_("train/strict_start_time"), false).value<bool>();
    rb->setChecked(checked);
    rb->setFont(font);
    btnGroup->addButton(rb);
    vLayout4->addWidget(rb);
    rb = new QRadioButton(tr("行程时间短的车次优先提交"));
    connect(rb, &QRadioButton::toggled, this, [&](bool checked) {
        UserData *ud = UserData::instance();
        ud->grabSetting.trainPrio.travelTimeShortPrio = checked;
        QSettings setting;
        setting.setValue(_("train/travel_time_short"), checked);
    });
    checked = setting.value(_("train/travel_time_short"), false).value<bool>();
    rb->setChecked(checked);
    rb->setFont(font);
    btnGroup->addButton(rb);
    vLayout4->addWidget(rb);

    QRadioButton *suffientRb = new QRadioButton(tr("余票充足的车次优先提交"));
    QRadioButton *defaultRb = new QRadioButton(tr("默认"));
    QRadioButton *preferGRb = new QRadioButton(tr("G开头的优先提交"));
    QRadioButton *preferDRb = new QRadioButton(tr("D开头的优先提交"));
    QRadioButton *timeRangeRb = new QRadioButton(tr("以下时间段优先提交"));
    QButtonGroup *btnGroup2 = new QButtonGroup;

    connect(suffientRb, &QRadioButton::toggled, this, [=](bool checked) {
        UserData *ud = UserData::instance();
        ud->grabSetting.trainPrio.sufficientTicketPrio = checked;
        defaultRb->setEnabled(checked);
        preferGRb->setEnabled(checked);
        preferDRb->setEnabled(checked);
        timeRangeRb->setEnabled(checked);
        QSettings setting;
        setting.setValue(_("train/sufficient_ticket"), checked);
    });
    checked = setting.value(_("train/sufficient_ticket"), true).value<bool>();
    suffientRb->setChecked(checked);
    suffientRb->setFont(font);
    defaultRb->setEnabled(checked);
    preferGRb->setEnabled(checked);
    preferDRb->setEnabled(checked);
    timeRangeRb->setEnabled(checked);

    btnGroup->addButton(suffientRb);
    vLayout4->addWidget(suffientRb);
    //rb->setChecked(true);

    QVBoxLayout *vLayout5 = new QVBoxLayout;

    connect(defaultRb, &QRadioButton::toggled, this, [](bool checked) {
        QSettings setting;
        setting.setValue(_("train/prefer_def"), checked);
    });
    checked = setting.value(_("train/prefer_def"), true).value<bool>();
    defaultRb->setChecked(checked);
    defaultRb->setFont(font);
    btnGroup2->addButton(defaultRb);
    vLayout5->addWidget(defaultRb);

    connect(preferGRb, &QRadioButton::toggled, this, [](bool checked) {
        UserData *ud = UserData::instance();
        ud->grabSetting.trainPrio.preferGPrio = checked;
        QSettings setting;
        setting.setValue(_("train/prefer_g"), checked);
    });

    checked = setting.value(_("train/prefer_g"), false).value<bool>();
    preferGRb->setChecked(checked);
    preferGRb->setFont(font);

    btnGroup2->addButton(preferGRb);
    vLayout5->addWidget(preferGRb);

    connect(preferDRb, &QRadioButton::toggled, this, [](bool checked) {
        UserData *ud = UserData::instance();
        ud->grabSetting.trainPrio.preferDPrio = checked;
        QSettings setting;
        setting.setValue(_("train/prefer_d"), checked);
    });
    checked = setting.value(_("train/prefer_d"), false).value<bool>();
    preferDRb->setChecked(checked);
    preferDRb->setFont(font);
    btnGroup2->addButton(preferDRb);
    vLayout5->addWidget(preferDRb);

    connect(timeRangeRb, &QRadioButton::toggled, this, [](bool checked) {
        UserData *ud = UserData::instance();
        ud->grabSetting.trainPrio.preferTimeRangePrio = checked;
        QSettings setting;
        setting.setValue(_("train/prefer_time_range"), checked);
    });
    checked = setting.value(_("train/prefer_time_range"), false).value<bool>();
    timeRangeRb->setChecked(checked);
    timeRangeRb->setFont(font);
    btnGroup2->addButton(timeRangeRb);
    vLayout5->addWidget(timeRangeRb);

    vLayout4->setSpacing(1);
    vLayout5->setContentsMargins(2, 2, 2, 2);
    vLayout4->addStretch();
    outvLayout->addLayout(vLayout4);

    QHBoxLayout *hLayout2 = new QHBoxLayout;
    QTimeEdit *timeEdit1 = new QTimeEdit;
    QTimeEdit *timeEdit2 = new QTimeEdit;
    connect(timeEdit1, &QDateTimeEdit::timeChanged, this, [=] (QTime time) {
        UserData *ud = UserData::instance();
        ud->grabSetting.trainPrio.timeRange.beginHour = time.hour();
        ud->grabSetting.trainPrio.timeRange.beginMinute = time.minute();
        QSettings setting;
        if (time >= timeEdit2->time()) {
            timeEdit2->setTime(QTime(time.hour() + 1, timeEdit2->time().minute(),
                                     timeEdit2->time().second()));
            setting.setValue(_("train/end_hour"), time.hour() + 1);
            setting.setValue(_("train/end_minute"), timeEdit2->time().minute());
        }
        setting.setValue(_("train/begin_hour"), time.hour());
        setting.setValue(_("train/begin_minute"), time.minute());
    });

    connect(timeEdit2, &QDateTimeEdit::timeChanged, this, [=] (QTime time) {
        UserData *ud = UserData::instance();
        ud->grabSetting.trainPrio.timeRange.endHour = time.hour();
        ud->grabSetting.trainPrio.timeRange.endMinute = time.minute();
        QSettings setting;

        if (time <= timeEdit1->time()) {
            timeEdit1->setTime(QTime(time.hour() - 1, timeEdit1->time().minute(),
                                     timeEdit1->time().second()));
            setting.setValue(_("train/begin_hour"), time.hour() - 1);
            setting.setValue(_("train/begin_minute"), timeEdit1->time().minute());
        }
        setting.setValue(_("train/end_hour"), time.hour());
        setting.setValue(_("train/end_minute"), time.minute());
    });
    timeEdit1->setDisplayFormat(QStringLiteral("hh:mm"));
    timeEdit1->setTime(QTime(8, 0));
    timeEdit1->setMaximumTime(QTime(22, 59, 0));
    timeEdit2->setDisplayFormat(QStringLiteral("hh:mm"));
    timeEdit2->setTime(QTime(12, 0));
    timeEdit2->setMaximumTime(QTime(23, 59, 0));
    timeEdit2->setMinimumTime(QTime(1, 0, 0));
    int hour = setting.value(_("train/begin_hour"), 8).value<int>();
    int minute = setting.value(_("train/begin_minute"), 0).value<int>();
    timeEdit1->setTime(QTime(hour, minute));
    hour = setting.value(_("train/end_hour"), 12).value<int>();
    minute = setting.value(_("train/end_minute"), 0).value<int>();
    timeEdit2->setTime(QTime(hour, minute));

    hLayout2->addWidget(timeEdit1);
    QLabel *label = new QLabel(QStringLiteral("-"));
    hLayout2->addWidget(label);
    hLayout2->addWidget(timeEdit2);
    hLayout2->addStretch();
    hLayout2->setContentsMargins(30, 2, 2, 2);
    QGroupBox *groupBox = new QGroupBox;
    groupBox->setLayout(vLayout5);
    groupBox->setFlat(true);
    vLayout5->setSpacing(1);
    vLayout5->setContentsMargins(2, 2, 2, 2);
    vLayout5->addStretch();

    outvLayout->addWidget(groupBox);
    outvLayout->addLayout(hLayout2);
    outvLayout->addStretch();

    QPushButton *pb = new QPushButton(tr("确定"));
    connect(pb, &QPushButton::clicked, this, &TrainNoDialog::hide);
    QHBoxLayout *hLayout1 = new QHBoxLayout;
    hLayout1->addStretch();
    hLayout1->addWidget(pb);
    hLayout1->addStretch();

    outvLayout->addLayout(hLayout1);

    setLayout(outvLayout);
    setWindowTitle(tr("选择车次"));
}

void TrainNoDialog::updateSelectedTips(int leftCount, int rightCount)
{
    QString tips = tr("已选%1/%2").arg(rightCount).arg(leftCount + rightCount);
    QStringList part;

    w->selectedTrainTipsLabel->setText(tips);
    selectedTrainSet.clear();
    const QStringList &trainList = dListWidget->rightListContent();
    for (const QString &train : trainList) {
        part = train.split(' ');
        selectedTrainSet.insert(part[0] + ' ' + part[1] + ' ' + part[2]);
    }
}

bool TrainNoDialog::hasTrain(const QString &trainInfo)
{
    return trainSet.contains(trainInfo);
}

void TrainNoDialog::addTrain(const QString &trainInfo, const QString &trainNo)
{
    if (!hasTrain(trainInfo)) {
        dListWidget->addLeftItem(trainInfo, Qt::MatchStartsWith);
        trainSet.insert(trainInfo);
        trainNoMap.insert(trainInfo.split(' ')[0], trainNo);
    }
}

void TrainNoDialog::addTrainFinish()
{
    dListWidget->setLeftCurrentRow(0);
}

void TrainNoDialog::addSelectedTrain(const QString &trainInfo)
{
    if (!trainInfo.isEmpty()) {
        dListWidget->moveLeftItemToRight(trainInfo + ' ', Qt::MatchStartsWith);
        selectedTrainSet.insert(trainInfo);
    }
}

void TrainNoDialog::removeSelectedTrain(const QString &trainInfo)
{
    if (!trainInfo.isEmpty()) {
        dListWidget->moveRightItemToLeft(trainInfo + ' ', Qt::MatchStartsWith);
        selectedTrainSet.remove(trainInfo);
    }
}

void TrainNoDialog::addSelectedTrainAll()
{
    dListWidget->moveCurrentLeftItemToRightAll();
}

void TrainNoDialog::clearSelectedTrain()
{
    dListWidget->moveCurrentRightItemToLeftAll();
}

void TrainNoDialog::clearUnSelectedTrain()
{
    dListWidget->clearLeftList();
    trainSet.clear();
    trainNoMap.clear();
}

const QList<QString> &TrainNoDialog::getSelectedTrainList() const
{
    static QList<QString> trainNoList;
    const QStringList &slist = dListWidget->rightListContent();
    trainNoList.clear();
    trainNoList.resize(slist.size());
    int idx = 0;
    for (int i = 0; i < slist.size(); i++) {
        const QString &s = slist[i];
        int j, k;
        for (j = 0, k = 0; j < s.size(); j++) {
            if (s[j] == ' ')
                k++;
            if (k == 3)
                break;
        }
        if (j < s.size()) {
            trainNoList[idx++] = s.first(j);
        }
    }
    return trainNoList;
}

const QSet<QString> &TrainNoDialog::getSelectedTrainSet() const
{
    return selectedTrainSet;
}

const QSet<QString> &TrainNoDialog::getAllTrainSet() const
{
    return trainSet;
}

const QMap<QString, QString> &TrainNoDialog::getTrainNoMap() const
{
    return trainNoMap;
}

void TrainNoDialog::enterGrabTicketMode()
{
    dListWidget->setDisableEdit();
}

void TrainNoDialog::exitGrabTicketMode()
{
    dListWidget->setEnableEdit();
}
