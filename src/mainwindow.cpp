#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "userdata.h"
#include "completeedit.h"
#include "nethelper.h"
#include "logindialog.h"
#include <QDebug>
#include <QCompleter>
#include <QLabel>
#include <QAbstractAnimation>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDialog>
#include <QMessageBox>
#include <QDateEdit>
#include <QDate>
#include <QTableView>
#include <QTableWidget>
#include <QPushButton>
#include <QDockWidget>
#include <QHeaderView>
#include <QFormLayout>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonArray>
#include <QJsonObject>
#include <QCloseEvent>
#include "analysis.h"
#include <QTableWidget>
#include <QCheckBox>
#include <QGroupBox>
#include <QListWidget>
#include <QRandomGenerator>
#include <QScrollArea>
#include <QSettings>
#include <QAudioOutput>
#include <QDesktopServices>
#include "icondelegate.h"
#include "version.h"

using namespace Qt;

#define _ QStringLiteral

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QHBoxLayout *hLayout;
    QLabel *label;

    QWidget *widget = new QWidget(this);
    hLayout = new QHBoxLayout;
    //hLayout->setMargin(10);
    hLayout->setSpacing(20);

    player = nullptr;
    stopMusicTimer = new QTimer;
    connect(stopMusicTimer, &QTimer::timeout, this, [=] () {
        startOrStopPlayMusic();
    });

    hLayout->addWidget(label = new QLabel(tr("出发站: ")));
    label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    hLayout->addWidget(fromStationLe = new CompleteEdit);
    fromStationLe->setPlaceholderText(_("简拼/全拼/汉字"));
    fromStationLe->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    fromStationLe->setMaximumWidth(120);
    connect(fromStationLe, &CompleteEdit::editingFinished, this, &MainWindow::userStartStationChanged);

    hLayout->addWidget(swapStationPb = new QPushButton);
    swapStationPb->setIcon(QIcon(QStringLiteral(":/icon/images/swap.png")));
    connect(swapStationPb, &QPushButton::clicked, this, &MainWindow::swapStation);

    hLayout->addWidget(label = new QLabel(tr("到达站: ")));
    label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    hLayout->addWidget(toStationLe = new CompleteEdit);
    toStationLe->setPlaceholderText(_("简拼/全拼/汉字"));
    toStationLe->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    toStationLe->setMaximumWidth(120);
    connect(toStationLe, &CompleteEdit::editingFinished, this, &MainWindow::userEndStationChanged);

    hLayout->addWidget(new QLabel(tr("日期: ")));
    hLayout->addWidget(tourDateDe = new QDateEdit);
    tourDateDe->setMinimumWidth(105);

    tourDateDe->setCalendarPopup(true);
    connect(tourDateDe, &QDateEdit::dateChanged, this, &MainWindow::userTourDateChanged);
    hLayout->addStretch();

    hLayout->addWidget(queryTicketPb = new QPushButton);
    queryTicketPb->setText(tr("查询"));
    connect(queryTicketPb, &QPushButton::clicked, this, &MainWindow::queryTicket);
    hLayout->addWidget(switchTicketShowTypePb = new QPushButton);
    switchTicketShowTypePb->setText(tr("显示票价"));
    connect(switchTicketShowTypePb, &QPushButton::clicked, this, &MainWindow::switchTicketShowType);
    hLayout->addWidget(playMusicPb = new QPushButton);
    playMusicPb->setText(tr("试听音乐"));
    connect(playMusicPb, &QPushButton::clicked, this, &MainWindow::startOrStopPlayMusic);

    hLayout->addWidget(grabTicketPb = new QPushButton);
    grabTicketPb->setText(tr("开始"));
    connect(grabTicketPb, &QPushButton::clicked, this, &MainWindow::startOrStopGrabTicket);

    QVBoxLayout *vLayout = new QVBoxLayout;
    vLayout->addLayout(hLayout);
    setUpTableView();
    QHBoxLayout *hLayout2 = new QHBoxLayout;
    hLayout2->setSpacing(10);
    hLayout2->addWidget(tableView);
    vLayout->addLayout(hLayout2);

    passengerDialog = new PassengerDialog(this);
    trainNoDialog = new TrainNoDialog(this);
    seatTypeDialog = new SeatTypeDialog(this);
    seatDialog = new SeatDialog(this);
    loginDialog = new LoginDialog;
    selltimeDialog = new SellTimeQueryDialog(this);

    doGrabTicketTimer = new QTimer;
    connect(doGrabTicketTimer, &QTimer::timeout, this, &MainWindow::doGrabTicket);
    fixedTimeGrabTimer = new QTimer;
    connect(fixedTimeGrabTimer, &QTimer::timeout, this, [this]() {
        fixedTimeGrabTimer->stop();
        enterGrabMode();
    });
    updateProgressBarTimer = new QTimer;
    connect(updateProgressBarTimer, &QTimer::timeout, this, [=] {
        int incrVal = grabTicketInterval >> 3;
        nextRequestProgress->setValue(nextRequestProgress->value() + incrVal);
    });

    player = new QMediaPlayer;
    QAudioOutput *output = new QAudioOutput;
    player->setAudioOutput(output);
    player->setSource(QUrl::fromLocalFile(UserData::instance()->generalSetting.musicPath));
    player->setLoops(QMediaPlayer::Infinite);
    output->setVolume(60);

    statChart = new BarChartView(this);
    delayChart = new LineChartView(this);
    delayChart->setTitle(_("时延统计"));
    delayChart->setXSeriesTitle(_("过去/小时"));
    delayChart->setYSeriesTitle(_("ms"));

    skipMaintenanceTimer = new QTimer;
    connect(skipMaintenanceTimer, &QTimer::timeout, this, [this] () {
        startOrStopGrabTicket();
        skipMaintenanceTimer->setInterval(60 * 60 * 4 * 1000);
        skipMaintenanceTimer->start();
    });

    notifyMng = new NotifyManager(this);
    notifyMng->setCloseMode(ECLOSEMANUAL);

    reQueryMidOn = true;

    createDockWidget();

    widget->setLayout(vLayout);
    setCentralWidget(widget);

    createHistoryInfoDialog();

    createUiComponent();

    readSettings();

    setTabOrder(fromStationLe, toStationLe);

    statusBar()->showMessage(QStringLiteral("准备就绪"));
    resize(QSize(1200, 800));
}

void MainWindow::setUp()
{
    passengerDialog->setUp();
    trainNoDialog->setUp();
    seatTypeDialog->setUp();
    loginDialog->setUp();
    selltimeDialog->setup();
    selltimeDialog->setQueryText(UserData::instance()->userConfig.staFromName);
}

void MainWindow::updateLoginButtonStatus(bool isLogin)
{
    loginButton->disconnect(SIGNAL(pressed()));

    if (isLogin) {
        connect(loginButton, &QPushButton::pressed, this, &MainWindow::logout);
        loginButton->setText(_("点我注销"));
    } else {
        connect(loginButton, &QPushButton::pressed, this, &MainWindow::showLoginDialog);
        loginButton->setText(_("点我登陆"));
    }
}

void MainWindow::updateLatencyChart(int d)
{
    latencyChart->update(d);
}

void MainWindow::createDockWidget()
{
    QWidget *widget = new QWidget;
    QDockWidget *dock = new QDockWidget;
    dock->setFeatures(QDockWidget::DockWidgetMovable);
    dock->setFeatures(QDockWidget::DockWidgetClosable);

    addDockWidget(Qt::BottomDockWidgetArea, dock);
    dock->resize(dock->sizeHint());

    browser = new QTextBrowser;
    browser->setTextColor(QColor(0, 139, 139));
    browser->setOpenExternalLinks(true);

    latencyChart = new LineChartView;
    latencyChart->legendHide();

    //browser->setMinimumSize(QSize(200, 80));
    QHBoxLayout *layout = new QHBoxLayout;
    QGridLayout *gLayout = new QGridLayout;
    QPushButton *pb = new QPushButton;
    selectedPassengerTipsLabel = new QLabel(tr("已选0/0"));
    pb->setText(tr("选择乘车人"));
    connect(pb, &QPushButton::clicked, passengerDialog, &PassengerDialog::show);
    gLayout->addWidget(pb, 0, 0, 1, 1);
    gLayout->addWidget(selectedPassengerTipsLabel, 0, 1, 1, 1);
    pb = new QPushButton;
    selectedTrainTipsLabel = new QLabel(tr("已选0/0"));
    pb->setText(tr("选择车次"));
    connect(pb, &QPushButton::clicked, trainNoDialog, &TrainNoDialog::show);
    gLayout->addWidget(pb, 1, 0, 1, 1);
    gLayout->addWidget(selectedTrainTipsLabel, 1, 1, 1, 1);
    pb = new QPushButton;
    selectedSeatTypeTipsLabel = new QLabel(tr("已选0/0"));
    pb->setText(tr("选择席别"));
    connect(pb, &QPushButton::clicked, seatTypeDialog, &SeatTypeDialog::show);
    gLayout->addWidget(pb, 2, 0, 1, 1);
    gLayout->addWidget(selectedSeatTypeTipsLabel, 2, 1, 1, 1);
    pb = new QPushButton;
    selectedSeatTipsLabel = new QLabel(tr("已选0/0"));
    pb->setText(tr("选座/铺"));
    connect(pb, &QPushButton::clicked, seatDialog, &SeatDialog::show);
    gLayout->addWidget(pb, 3, 0, 1, 1);
    gLayout->addWidget(selectedSeatTipsLabel, 3, 1, 1, 1);

    QVBoxLayout *rightLayout = new QVBoxLayout;
    pb = new QPushButton(tr("起售查询"));
    connect(pb, &QPushButton::clicked, selltimeDialog, &SellTimeQueryDialog::show);
    rightLayout->addWidget(pb);
    loginButton = new QPushButton(tr("点我登陆"));
    connect(loginButton, &QPushButton::pressed, this, &MainWindow::showLoginDialog);
    rightLayout->addWidget(loginButton);

    settingDialog = new SettingDialog(this);
    settingDialog->setUp();
    pb = new QPushButton(tr("设置"));
    connect(pb, &QPushButton::clicked, settingDialog, &SettingDialog::show);
    rightLayout->addWidget(pb);

    layout->addLayout(gLayout);
    layout->addWidget(browser);
    layout->addWidget(latencyChart);
    layout->addLayout(rightLayout);
    widget->setLayout(layout);
    dock->setWidget(widget);
}

void MainWindow::createHistoryInfoDialog()
{
    historyInfoDialog = new QDialog(this);
    historyInfoBrower = new QTextBrowser;
    historyInfoBrower->setTextColor(QColor(0, 139, 139));
    historyInfoBrower->setOpenExternalLinks(true);
    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->addWidget(historyInfoBrower);
    QHBoxLayout *hlayout = new QHBoxLayout;
    QPushButton *pb = new QPushButton(tr("清空"));
    connect(pb, &QPushButton::clicked, historyInfoBrower, &QTextBrowser::clear);
    hlayout->addStretch();
    hlayout->addWidget(pb);
    hlayout->addStretch();
    vlayout->addLayout(hlayout);
    historyInfoDialog->setLayout(vlayout);
}

void MainWindow::createUiComponent()
{
    QAction *action;
    QMenu *menu;

    menu = menuBar()->addMenu(tr("&文件"));

    /*
    action = new QAction(tr("&注销..."), this);
    action->setStatusTip(tr("注销登陆"));
    connect(action, &QAction::triggered, this, &MainWindow::logout);
    menu->addAction(action);
    */

    action = new QAction(tr("&退出..."), this);
    action->setShortcut(tr("Ctrl+Q"));
    action->setStatusTip(tr("退出程序"));
    connect(action, &QAction::triggered, this, &MainWindow::close);
    menu->addAction(action);

    menu = menuBar()->addMenu(tr("&操作"));

    action = new QAction(tr("&立即同步时间"), this);
    action->setStatusTip(tr("立即从指定的时间服务器同步时间"));
    connect(action, &QAction::triggered, this, &MainWindow::syncTime);
    menu->addAction(action);

    action = new QAction(tr("&支付未支付的订单"), this);
    action->setStatusTip(tr("支付未支付的订单"));
    connect(action, &QAction::triggered, NetHelper::instance(), &NetHelper::queryNoCompleteOrder);
    menu->addAction(action);

    action = new QAction(tr("&支付未支付的候补订单"), this);
    action->setStatusTip(tr("支付未支付的候补订单"));
    connect(action, &QAction::triggered, NetHelper::instance(), &NetHelper::cqueryQueue);
    menu->addAction(action);

    menu = menuBar()->addMenu(tr("&显示"));

    action = new QAction(tr("&历史提交..."), this);
    action->setStatusTip(tr("查看历史提交记录"));
    connect(action, &QAction::triggered, historyInfoDialog, &QDialog::show);
    menu->addAction(action);

    action = new QAction(tr("&设置..."), this);
    action->setStatusTip(tr("打开设置界面"));
    connect(action, &QAction::triggered, settingDialog, &SettingDialog::show);
    menu->addAction(action);

    menu = menuBar()->addMenu(tr("&统计"));

    action = new QAction(tr("异常..."), this);
    action->setStatusTip(tr("显示异常统计图"));
    connect(action, &QAction::triggered, statChart, &BarChartView::show);
    menu->addAction(action);

    action = new QAction(tr("时延..."), this);
    action->setStatusTip(tr("显示时延统计图"));
    connect(action, &QAction::triggered, delayChart, &LineChartView::show);
    menu->addAction(action);

    menu = menuBar()->addMenu(tr("&帮助"));

    action = new QAction(tr("在线帮助..."), this);
    action->setStatusTip(tr("跳转到在线页面查看帮助"));
    connect(action, &QAction::triggered, this, &MainWindow::onlineHelp);
    menu->addAction(action);
    action = new QAction(tr("关于..."), this);
    action->setStatusTip(tr("显示版本信息"));
    connect(action, &QAction::triggered, this, &MainWindow::about);
    menu->addAction(action);
    action = new QAction(tr("关于Qt..."), this);
    action->setStatusTip(tr("显示Qt版本信息"));
    connect(action, &QAction::triggered, qApp, &QApplication::aboutQt);
    menu->addAction(action);

    initStatusBars();
}

void MainWindow::userStartStationChanged()
{
    UserData *ud = UserData::instance();
    struct UserConfig &uc = ud->getUserConfig();
    QString staFromStation = fromStationLe->text().trimmed();
    static QString laststaFromCode;

    uc.staFromName = staFromStation;
    uc.staFromCode = ud->getStaCode()->value(staFromStation);
    if (uc.staFromCode.isEmpty()) {
        formatOutput(_("抱歉，未找到出发站名称为'%1'的站点代码，请检查出发站的站点名称是否正确").arg(uc.staFromName));
        return;
    }
    if (uc.staFromCode != laststaFromCode) {
        trainNoDialog->clearUnSelectedTrain();
        laststaFromCode = uc.staFromCode;
        reQueryMidOn = true;  // 重新查询中间车站
    }

    QSettings setting;
    setting.setValue(_("query_ticket/from_station_name"), staFromStation);
}

void MainWindow::userEndStationChanged()
{
    UserData *ud = UserData::instance();
    struct UserConfig &uc = ud->getUserConfig();
    QString staToStation = toStationLe->text().trimmed();
    static QString lastStaToCode;

    uc.staToName = staToStation;
    uc.staToCode = ud->getStaCode()->value(staToStation);
    if (uc.staToCode.isEmpty()) {
        formatOutput(_("抱歉，未找到到达站名称为'%1'的站点代码，请检查到达站的站点名称是否正确").arg(uc.staToName));
        return;
    }
    if (uc.staToCode != lastStaToCode) {
        trainNoDialog->clearUnSelectedTrain();
        lastStaToCode = uc.staToCode;
        reQueryMidOn = true;  // 重新查询中间车站
    }

    QSettings setting;
    setting.setValue(_("query_ticket/to_station_name"), staToStation);
}

void MainWindow::userTourDateChanged(const QDate &date)
{
    QString strDate = date.toString(QStringLiteral("yyyy-MM-dd"));
    UserData::instance()->getUserConfig().tourDate = strDate;
    trainNoDialog->clearUnSelectedTrain();
    QSettings setting;
    setting.setValue(_("query_ticket/tour_date"), strDate);
    reQueryMidOn = true;  // 重新查询中间车站
}

void MainWindow::swapStation()
{
    QString tmp;
    UserData *ud = UserData::instance();
    UserConfig &uc = ud->getUserConfig();

    tmp = fromStationLe->text();
    fromStationLe->setText(toStationLe->text());
    toStationLe->setText(tmp);

    uc.staFromName.swap(uc.staToName);
    uc.staFromCode.swap(uc.staToCode);
    if (uc.staFromCode.isEmpty()) {
        formatOutput(_("抱歉，未找到出发站名称为'%1'的站点代码，请检查出发站的站点名称是否正确").arg(uc.staFromName));
        return;
    }
    if (uc.staToCode.isEmpty()) {
        formatOutput(_("抱歉，未找到到达站名称为'%1'的站点代码，请检查到达站的站点名称是否正确").arg(uc.staToName));
        return;
    }
    trainNoDialog->clearUnSelectedTrain();
    QSettings setting;
    setting.setValue(_("query_ticket/from_station_name"), uc.staFromName);
    setting.setValue(_("query_ticket/to_station_name"), uc.staToName);
}

void MainWindow::syncTime()
{
    UserData *ud = UserData::instance();
    if (ud->generalSetting.autoSyncServerTime) {
        ntp.syncTime(ud->generalSetting.timeServer);
    }
}

void MainWindow::uamLogined()
{
    statusBar()->showMessage(QStringLiteral("当前状态：已登陆"));
    showMainWindow();
    updateLoginButtonStatus(true);
}

void MainWindow::uamNotLogined()
{
    statusBar()->showMessage(QStringLiteral("当前状态：未登陆"));
    showLoginDialog();
    updateLoginButtonStatus(false);
}

void MainWindow::logout()
{
    if (grabTicketPb->text() == _("停止")) {
        QMessageBox::StandardButton clicked = QMessageBox::warning(this, tr("确认注销吗？"),
            _("当前正处于抢票模式，注销后将无法抢票！"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (clicked == QMessageBox::No)
            return;
    }
    NetHelper::instance()->logout();
}

void MainWindow::logoutSuccess()
{
    showLoginDialog();
    updateLoginButtonStatus(false);
}


void MainWindow::setRemainTicketColor(QString &remain, QStandardItem *item, bool canCandidate)
{
    if (remain.isEmpty())
        return;
    if (!remain.compare(QStringLiteral("有")))
        item->setForeground(QBrush(QColor(144, 238, 144)));
    else if (!remain.compare(QStringLiteral("无")))
        item->setForeground(QBrush(QColor(190, 190, 190)));
    else if (!remain.compare(_("--")))
        item->setForeground(Qt::black);
    else {
        item->setForeground(QBrush(QColor(238, 154, 73)));
        if (!canCandidate && !remain.compare(_("候补")))
            item->setForeground(QBrush(QColor(190, 190, 190)));
    }
}

void MainWindow::addTrainToSelected()
{
    QPushButton *button = dynamic_cast<QPushButton *>(sender());
    //QStandardItemModel *itemModel = static_cast<QStandardItemModel *>(tableView->model());
    //QStandardItem *item = itemModel->item(idx);
    trainNoDialog->addSelectedTrain(_("%1 (%2 %3").arg(button->property("trainCode").toString(),
                                                    button->property("fromStationName").toString(),
                                                    button->property("toStationName").toString()));
}

bool MainWindow::canAddNewTrain(const QString &trainTime)
{
    UserData *ud = UserData::instance();
    if (!ud->grabSetting.acceptNewTrain) {
        return false;
    }
    QStringList hourMinStr = trainTime.split(':');
    if (!hourMinStr.isEmpty()) {
        int hour = hourMinStr[0].toInt();
        if (hour >= ud->grabSetting.newTrainStartHour &&
            hour <= ud->grabSetting.newTrainEndHour) {
            return true;
        }
    }
    return false;
}


// 处理查询余票返回结果
void MainWindow::processQueryTicketReply(QVariantMap &data)
{
    QStandardItemModel *model = static_cast<QStandardItemModel *>(tableView->model());
    UserData *ud = UserData::instance();
    UserConfig &uc = ud->getUserConfig();
    QString staFromName = uc.staFromName;
    QString staToName = uc.staToName;
    QString tourDate = uc.tourDate;
    QVariantMap stationMap = data[_("map")].toMap();

    // 已经下单成功了，不处理后续返回的查询结果
    if (ud->lastRunSuccess) {
        return;
    }
    if (stationMap.isEmpty()) {
        //formatOutput(_("查询失败，返回站点信息为空"));
        formatOutput(_("%1->%2(%3) 共查询到 0 趟车次, 可预订 0 趟车次")
                         .arg(staFromName, staToName, tourDate));
        model->removeRows(0, model->rowCount());
        return;
    }
    QVariantList resultList = data[_("result")].toList();
    if (resultList.isEmpty()) {
        //formatOutput(_("查询失败，返回结果为空"));
        formatOutput(_("%1->%2(%3) 共查询到 0 趟车次, 可预订 0 趟车次")
                         .arg(staFromName, staToName, tourDate));
        model->removeRows(0, model->rowCount());
        return;
    }
    int trainListSize = resultList.size();
    int can_booking = 0;
    int i, itemIdx;
    int useTrainListSize = trainListSize;

    model->setRowCount(trainListSize);
    QVector<QStringList> allTrain;
    QVector<QStringList> availableTrain;
    QVector<QStringList> invalidTrain;
    QString fromStationName, toStationName;
    const QVector<QPair<int, enum TrainInfoEnum>> tableSeatTypeData = {
             QPair<int, enum TrainInfoEnum>(ESPECIALSEATCOL, ETZNUM),
             QPair<int, enum TrainInfoEnum>(ESPECIALSEATCOL, ESWZNUM),
             QPair<int, enum TrainInfoEnum>(EPRIFIRSTPRISEATCOL, EGGNUM),
             QPair<int, enum TrainInfoEnum>(EFIRSTPRISEATCOL, EZYNUM),
             QPair<int, enum TrainInfoEnum>(ESECONDPRISEATCOL, EZENUM),
             QPair<int, enum TrainInfoEnum>(EADVSOFTCROUCHCOL, EGRNUM),
             QPair<int, enum TrainInfoEnum>(ESOFTCROUCHCOL, ERWNUM),
             QPair<int, enum TrainInfoEnum>(EHARDCROUCHCOL, EYWNUM),
             QPair<int, enum TrainInfoEnum>(ESOFTSEATCOL, ERZNUM),
             QPair<int, enum TrainInfoEnum>(EHARDSEATCOL, EYZNUM),
             QPair<int, enum TrainInfoEnum>(ENOSEATCOL, EWZNUM),
             QPair<int, enum TrainInfoEnum>(EOTHERCOL, EQTNUM)
    };
    QVector<QStandardItem *> tableSeatTypeItems;
    QMap<char, QStandardItem *> tableSeatTypeItemsMap;
    QStringList timeStrList;
    QStringList timeStrList2;
    int spendDays, spendTime;
    int hour;

    // 抢票模式先分析车票再渲染，防止渲染浪费时间
    if (ud->runStatus == EGRABTICKET) {
        for (i = 0; i < trainListSize; i++) {
            QString train = resultList[i].toString();
            QStringList trainInfo = train.split('|');

            if (trainInfo.size() < ETRAININFOMAX) {
                continue;
            }
            if (trainInfo[ESECRETSTR].isEmpty()) {
                continue;
            }
            if (trainInfo[ESTATIONTRAINCODE].isEmpty()) {
                continue;
            }
            if (trainInfo[ECANWEBBUY] == _("Y")) {
                if (ud->runStatus == EGRABTICKET) {
                    availableTrain.push_back(trainInfo);
                    // 增开列车
                    QString trainDesc = _("%1 (%2 %3 %4-%5 %6)").arg(trainInfo[ESTATIONTRAINCODE],
                                                                                 fromStationName,
                                                                                 toStationName,
                                                                                 trainInfo[ESTARTTIME],
                                                                                 trainInfo[EARRIVETIME],
                                                                                 trainInfo[ESPENDTIME]);
                    if (!trainNoDialog->hasTrain(trainDesc)) {
                        if (canAddNewTrain(trainInfo[ESTARTTIME])) {
                            // 选了接受增开列车，自动加到已选列表
                            trainNoDialog->addTrain(trainDesc, trainInfo[ETRAINNO]);
                            trainNoDialog->addSelectedTrain(_("%1 (%2 %3").arg(trainInfo[ESTATIONTRAINCODE],
                                                                               fromStationName,
                                                                               toStationName));
                        }
                    }
                }
                can_booking++;
            }
            if (ud->candidateSetting.isCandidate) {
                allTrain.push_back(trainInfo);
            }
        }
        formatOutput(_("%1->%2(%3) 共查询到 %4 趟车次, 可预订 %5 趟车次").
                     arg(staFromName, staToName, tourDate).arg(trainListSize).arg(can_booking));
        Analysis candidateAnalysis(allTrain);
        int trainNoIdx = -1;
        bool hasCandidate = false;

        if (ud->candidateSetting.isCandidate &&
            ud->candidateSetting.onlyCandidate) {
            if (ud->candidateRunStatus == ESUBMITCANDIDATESUCCESS) {
                // 结束任务
                startOrStopGrabTicket();
            }
            candidateAnalysis.mayCandidate(stationMap, ud->getUserConfig().tourDate);
        } else {
            if (ud->candidateSetting.isCandidate && ud->candidateSetting.prioCandidate) {
                hasCandidate = candidateAnalysis.mayCandidate(stationMap, ud->getUserConfig().tourDate);
            }

            if (can_booking && !hasCandidate) {
                Analysis ana(availableTrain);
                std::pair<QString, QString> ticketStr;
                QVector<QPair<QString, QChar>> submitSeatType;
                trainNoIdx = ana.analysisTrain(ticketStr, submitSeatType, stationMap);
                if (trainNoIdx != -1) {
                    if (!ticketStr.first.isEmpty() && !ticketStr.second.isEmpty()) {
                        ud->submitTicketInfo.trainCode = availableTrain[trainNoIdx][ESTATIONTRAINCODE];
                        ud->submitTicketInfo.trainNo = availableTrain[trainNoIdx][ETRAINNO];
                        ud->submitTicketInfo.secretStr = availableTrain[trainNoIdx][ESECRETSTR];
                        ud->submitTicketInfo.ypDetailInfo = availableTrain[trainNoIdx][EYPINFO];
                        ud->submitTicketInfo.date = tourDate;
                        ud->submitTicketInfo.startSTationName = stationMap.value(availableTrain[trainNoIdx]
                                                                                               [ESTARTSTATIONTELECODE])
                                                                    .toString();
                        ud->submitTicketInfo.endStationName = stationMap.value(availableTrain[trainNoIdx]
                                                                                             [EENDSTATIONTELECODE])
                                                                  .toString();
                        ud->submitTicketInfo.fromStationCode = availableTrain[trainNoIdx][EFROMSTATIONTELECODE];
                        ud->submitTicketInfo.fromStationName = stationMap.value(availableTrain[trainNoIdx]
                                                                                              [EFROMSTATIONTELECODE])
                                                                   .toString();
                        ud->submitTicketInfo.toStationCode = availableTrain[trainNoIdx][ETOSTATIONTELECODE];
                        ud->submitTicketInfo.toStationName = stationMap.value(availableTrain[trainNoIdx]
                                                                                            [ETOSTATIONTELECODE])
                                                                 .toString();
                        ud->submitTicketInfo.fromTime = availableTrain[trainNoIdx][ESTARTTIME];
                        ud->submitTicketInfo.toTime = availableTrain[trainNoIdx][EARRIVETIME];
                        ud->submitTicketInfo.travelTime = availableTrain[trainNoIdx][ESPENDTIME];
                        ud->submitTicketInfo.submitSeatType = submitSeatType;
                        ud->submitTicketInfo.passengerTicketInfo = ticketStr.first;
                        ud->submitTicketInfo.oldPassengerTicketInfo = ticketStr.second;
                        ud->submitTicketInfo.bedLevelInfo = availableTrain[trainNoIdx][EBEDLEVELINFO];
                        ud->submitTicketInfo.seatDiscountInfo = availableTrain[trainNoIdx][ESEATDISCOUNTINFO];
                        ud->setRunStatus(ESUBMITORDER);
                        // 提交订单入口
                        NetHelper::instance()->checkUser();
                    } else {
                        formatOutput(_("生成车票提交信息失败"));
                    }
                } else {
                    if (ud->grabSetting.queryMidStation && reQueryMidOn) {
                        reQueryMidOn = false;
                        NetHelper *nh = NetHelper::instance();
                        if (!nh->midTrain.isEmpty()) {
                            nh->midTrain.clear();
                        }
                        nh->midTrain.initTrain();
                        nh->midTrain.start();
                    }
                }
            }
            if (ud->candidateSetting.isCandidate &&
                !ud->candidateSetting.prioCandidate &&
                trainNoIdx == -1) {
                candidateAnalysis.mayCandidate(stationMap, ud->getUserConfig().tourDate);
            }
        }
    }

    itemIdx = 0;
    // 开始显示到tableview
    for (i = 0; i < trainListSize; i++) {
        QString train = resultList[i].toString();
        QStringList trainInfo = train.split('|');
        QStandardItem  *item;
        QPushButton *button;
        QString curText;

        if (trainInfo.size() < ETRAININFOMAX) {
            useTrainListSize--;
            continue;
        }
        if (trainInfo[ESTATIONTRAINCODE].isEmpty()) {
            useTrainListSize--;
            continue;
        }
        QStringList dwFlag = trainInfo[EDWFLAG].split('#');
        bool isFunXing = dwFlag.length() > 1 && dwFlag[1] == '1';  // 复兴号
        bool isZiNeng = !dwFlag.isEmpty() && dwFlag[0] == '5';  // 智能列车
        bool isDongGan = dwFlag.length() > 5 && dwFlag[5] == 'D';  // 动感号

        char trainCodePrefix = trainInfo[ESTATIONTRAINCODE][0].toLatin1();
        if ((settingDialog->trainTypeShowVec['5'] && isZiNeng) ||
            (settingDialog->trainTypeShowVec['1'] && isFunXing) ||
            (settingDialog->trainTypeShowVec['d'] && isDongGan)) {

        } else {
            if (settingDialog->trainTypeShowVec[trainCodePrefix] == false) {
                if (settingDialog->trainTypeShowVec['O'] == false) {
                    useTrainListSize--;
                    continue;
                }
            }
        }
        if (ud->generalSetting.startTimeRange1 != 0 ||
            ud->generalSetting.startTimeRange2 != 24) {
            timeStrList = trainInfo[ESTARTTIME].split(':');
            if (!timeStrList.isEmpty()) {
                hour = timeStrList[0].toInt();
                if (hour < ud->generalSetting.startTimeRange1 ||
                    hour >= ud->generalSetting.startTimeRange2) {
                    useTrainListSize--;
                    continue;
                }
            }
        }

        fromStationName = stationMap.value(trainInfo[EFROMSTATIONTELECODE]).toString();
        toStationName = stationMap.value(trainInfo[ETOSTATIONTELECODE]).toString();

        if (ud->runStatus != EGRABTICKET) {
            if (trainInfo[ECANWEBBUY] == _("Y")) {
                can_booking++;
            }
        }
        if ((trainInfo[ECONTROLLEDTRAINFLAG] == _("1") || trainInfo[ECONTROLLEDTRAINFLAG] == _("2"))) {
            invalidTrain.push_back(trainInfo);
            //useTrainListSize--;
            continue;
        }

        curText = trainInfo[ESTATIONTRAINCODE].isEmpty() ?
                      "--" : trainInfo[ESTATIONTRAINCODE];
        item = model->item(itemIdx, ETRAINNOCOL);
        if (!item) {
            model->setItem(itemIdx, ETRAINNOCOL, item = new QStandardItem(curText));
            item->setTextAlignment(Qt::AlignCenter);
            item->setForeground(QBrush(QColor(99, 184, 255)));
        }
        if (item->text() != curText) {
            item->setText(curText);
            item->setToolTip(curText);
            item->setForeground(QBrush(QColor(99, 184, 255)));
        }
        item->setData(trainInfo[ETRAINNO], Qt::UserRole);

        item = model->item(itemIdx, EFROMSTATIONCOL);
        if (!item) {
            model->setItem(itemIdx, EFROMSTATIONCOL, item = new QStandardItem);
        }
        if (item->text() != fromStationName) {
            item->setText(fromStationName);
            item->setToolTip(fromStationName);
            item->setData(fromStationName, Qt::ToolTipRole);
        }
        //item->setTextAlignment(Qt::AlignCenter);
        /*item->setFont(QFont("Times", 10, QFont::Black));
        item->setForeground(QBrush(QColor(99, 184, 255)));*/
        item->setData(trainInfo[EFROMSTATIONTELECODE] == trainInfo[ESTARTSTATIONTELECODE], Qt::DecorationRole);
        item->setData(trainInfo[EFROMSTATIONTELECODE], Qt::UserRole);


        item = model->item(itemIdx, ETOSTATIONCOL);
        if (!item) {
            model->setItem(itemIdx, ETOSTATIONCOL, item = new QStandardItem);
            item->setTextAlignment(Qt::AlignCenter);
            item->setToolTip(toStationName);
            /*item->setFont(QFont("Times", 10, QFont::Black));
            item->setForeground(QBrush(QColor(99, 184, 255)));*/
        }
        if (item->text() != toStationName) {
            item->setText(toStationName);
            item->setToolTip(toStationName);
            item->setData(toStationName, Qt::ToolTipRole);
            //item->setForeground(QBrush(QColor(99, 184, 255)));
        }
        item->setData(trainInfo[ETOSTATIONTELECODE] == trainInfo[EENDSTATIONTELECODE], Qt::DecorationRole);
        item->setData(trainInfo[ETOSTATIONTELECODE], Qt::UserRole);

        item = model->item(itemIdx, ESTARTTIMECOL);
        if (!item) {
            model->setItem(itemIdx, ESTARTTIMECOL, item = new QStandardItem(trainInfo[ESTARTTIME]));
            item->setTextAlignment(Qt::AlignCenter);
            item->setForeground(QBrush(QColor(205, 104, 137)));

        }

        if (item->text() != trainInfo[ESTARTTIME]) {
            item->setText(trainInfo[ESTARTTIME]);
            item->setForeground(QBrush(QColor(205, 104, 137)));
        }

        item = model->item(itemIdx, EARRIVETIMECOL);
        if (!item) {
            model->setItem(itemIdx, EARRIVETIMECOL, item = new QStandardItem(trainInfo[EARRIVETIME]));
            item->setTextAlignment(Qt::AlignCenter);
            item->setForeground(QBrush(QColor(205, 104, 137)));

        }
        if (item->text() != trainInfo[EARRIVETIME]) {
            item->setText(trainInfo[EARRIVETIME]);
            item->setForeground(QBrush(QColor(205, 104, 137)));
        }

        if (ud->runStatus == EIDLE) {
            timeStrList = trainInfo[ESTARTTIME].split(':');
            timeStrList2 = trainInfo[ESPENDTIME].split(':');
            spendDays = 0;
            if (timeStrList.size() > 1 && timeStrList2.size() > 1) {
                spendTime = timeStrList[0].toInt() + timeStrList2[0].toInt();
                if (timeStrList[1].toInt() + timeStrList2[1].toInt() > 59) {
                    spendTime++;
                }
                while (spendTime - 24 >= 0) {
                    spendTime -= 24;
                    spendDays++;
                }
            }
            item->setData(spendDays, Qt::UserRole);
        }

        item = model->item(itemIdx, EUSEDTIMECOL);
        if (!item) {
            model->setItem(itemIdx, EUSEDTIMECOL, item = new QStandardItem(trainInfo[ESPENDTIME]));
            item->setTextAlignment(Qt::AlignCenter);
            item->setForeground(QBrush(QColor(122, 139, 139)));
        }
        if (item->text() != trainInfo[ESPENDTIME]) {
            item->setText(trainInfo[ESPENDTIME]);
            item->setForeground(QBrush(QColor(122, 139, 139)));
        }

        for (auto &seatTypeData : tableSeatTypeData) {
            item = model->item(itemIdx, seatTypeData.first);
            if (!item) {
                item = new QStandardItem;
                model->setItem(itemIdx, seatTypeData.first, item);
            }

            bool canCandidate = false;
            curText = trainInfo[seatTypeData.second].isEmpty() ? _("--") :
                              trainInfo[seatTypeData.second];
            if (seatTypeData.first == ESWZNUM) {
                if (trainInfo[seatTypeData.second].isEmpty() ||
                    trainInfo[seatTypeData.second] == _("无") ||
                    trainInfo[seatTypeData.second] == _("0")) {
                    tableSeatTypeItems.push_back(item);
                    continue;
                }
            }
            if (!trainInfo[ESECRETSTR].isEmpty() && trainInfo[ECANDIDATETRAINFLAG] == _("1") &&
                curText == _("无")) {
                if (seatTypeData.second != EWZNUM && seatTypeData.second != EQTNUM) {
                    curText = _("候补");
                    QChar c = seatTypeNoToCode(seatTypeData.second);
                    if (c != '0' && !trainInfo[ECANDIDATESEATLIMIT].contains(c)) {
                        canCandidate = true;
                    }
                }
            }
            int type = 0;
            SETCANCANDIDATE(type, canCandidate);
            setRemainTicketColor(curText, item, canCandidate);
            if (item->text() != curText) {
                item->setText(curText);
                item->setData(curText, Qt::WhatsThisRole);
                item->setTextAlignment(Qt::AlignCenter);
            }

            if (seatTypeData.first == EOTHERCOL) {
                SETFUXING(type, isFunXing);
                SETZINENG(type, isZiNeng);
                SETDONGGAN(type, isDongGan);
            }
            item->setData(type, Qt::UserRole);

            tableSeatTypeItems.push_back(item);
        }


        button = dynamic_cast<QPushButton *>(tableView->indexWidget(model->index(itemIdx, EREMARKCOL)));
        if (!button) {
            button = new QPushButton;
            connect(button, &QPushButton::clicked, this, &MainWindow::addTrainToSelected);
        }
        curText = trainInfo[ETEXTINFO];
        if (curText != _("预订")) {
            if (curText.endsWith(_("<br/>"))) {
                curText.remove(_("<br/>"));
            }
            if (curText.endsWith(_("起售"))) {
                curText.remove(_("起售"));
            }
            if (curText.length() > 4) {
                button->setToolTip(curText);
            }
        }
        button->setText(curText);
        if (ud->runStatus != EIDLE) {
            //button->setStyleSheet(QStringLiteral("QPushButton { background-color: #A9A9A9; color: #4F4F4F; }"));
            button->setEnabled(false);
        } else {
            //button->setStyleSheet(QStringLiteral("QPushButton { color: #1C86EE; }"));
            button->setProperty("fromStationName", fromStationName);
            button->setProperty("toStationName", toStationName);
            button->setProperty("trainCode", trainInfo[ESTATIONTRAINCODE]);
        }
        tableView->setIndexWidget(model->index(itemIdx, EREMARKCOL), button);

        // 席别余票分析，显示余票
        if (ud->runStatus == EIDLE) {
            QString &ypInfoNew = trainInfo[EYPINFONEW];
            int ypInfoNewSize = ypInfoNew.size();

            tableSeatTypeItemsMap.insert('9', tableSeatTypeItems[0]);
            tableSeatTypeItemsMap.insert('P', tableSeatTypeItems[1]);
            tableSeatTypeItemsMap.insert('D', tableSeatTypeItems[2]);
            tableSeatTypeItemsMap.insert('M', tableSeatTypeItems[3]);
            tableSeatTypeItemsMap.insert('O', tableSeatTypeItems[4]);
            tableSeatTypeItemsMap.insert('6', tableSeatTypeItems[5]);
            tableSeatTypeItemsMap.insert('4', tableSeatTypeItems[6]);
            tableSeatTypeItemsMap.insert('F', tableSeatTypeItems[6]);
            tableSeatTypeItemsMap.insert('I', tableSeatTypeItems[6]);
            tableSeatTypeItemsMap.insert('3', tableSeatTypeItems[7]);
            tableSeatTypeItemsMap.insert('J', tableSeatTypeItems[7]);
            tableSeatTypeItemsMap.insert('2', tableSeatTypeItems[8]);
            tableSeatTypeItemsMap.insert('1', tableSeatTypeItems[9]);

            /*for (auto &seatTypeData : tableSeatTypeData) {
                tableSeatTypeItems[seatTypeData.first - ESPECIALSEATCOL]->setData(
                    0, Qt::StatusTipRole);
            }
            for (auto &seatTypeData : tableSeatTypeData) {
                tableSeatTypeItems[seatTypeData.first - ESPECIALSEATCOL]->setData(
                    trainInfo[seatTypeData.second].isEmpty() ? _("--") :
                        trainInfo[seatTypeData.second], Qt::StatusTipRole);
            }*/
            for (int j = 0; j < ypInfoNewSize; j += 10) {
                if (j + 10 > ypInfoNewSize) {
                    break;
                }
                int price = ypInfoNew.sliced(j + 1, 5).toInt();
                float price2 = price / 10.0;
                int dd = ypInfoNew.sliced(j + 6, 4).toInt();
                char t = ypInfoNew[j].toLatin1();
                QMap<char, QStandardItem *>::ConstIterator it =
                    tableSeatTypeItemsMap.constFind(t);
                if (it != tableSeatTypeItemsMap.constEnd()) {
                    if (it.value()->text() != _("--")) {
                        it.value()->setToolTip(_("%1").arg(price2));
                        it.value()->setData(price2, Qt::ToolTipRole);
                        if (uc.showTicketPrice) {
                            it.value()->setText(_("%1").arg(price2));
                            it.value()->setForeground(QBrush(QColor(238, 118, 33)));
                        }
                    }
                } else {
                    // 其他
                    if (dd < 3000) {
                        if (tableSeatTypeItems.size() > 11 &&
                            tableSeatTypeItems[11]->text() != _("--")) {
                            tableSeatTypeItems[11]->setToolTip(_("%1").arg(price2));
                            tableSeatTypeItems[11]->setData(price2, Qt::ToolTipRole);
                            if (uc.showTicketPrice) {
                                tableSeatTypeItems[11]->setText(_("%1").arg(price2));
                                tableSeatTypeItems[11]->setForeground(QBrush(QColor(238, 118, 33)));
                            }
                        }
                    }
                }
                // 无座
                if (dd >= 3000) {
                    if (tableSeatTypeItems.size() > 10 &&
                        tableSeatTypeItems[10]->text() != _("--")) {
                        tableSeatTypeItems[10]->setToolTip(_("%1").arg(price2));
                        tableSeatTypeItems[10]->setData(price2, Qt::ToolTipRole);
                        if (uc.showTicketPrice) {
                            tableSeatTypeItems[10]->setText(_("%1").arg(price2));
                            tableSeatTypeItems[10]->setForeground(QBrush(QColor(238, 118, 33)));
                        }
                    }
                }
            }

            tableSeatTypeItemsMap.clear();

            trainNoDialog->addTrain(_("%1 (%2 %3 %4-%5 %6)").arg(trainInfo[ESTATIONTRAINCODE],
                                                                 fromStationName,
                                                                 toStationName,
                                                                 trainInfo[ESTARTTIME],
                                                                 trainInfo[EARRIVETIME],
                                                                 trainInfo[ESPENDTIME]), trainInfo[ETRAINNO]);
        }

        tableSeatTypeItems.clear();
        itemIdx++;
    }
    model->setRowCount(useTrainListSize);
    trainNoDialog->addTrainFinish();
    // 列车运行图调整的列车
    int invalidTrainSize = invalidTrain.size();
    for (i = 0; i < invalidTrainSize; i++) {
        QStringList &trainInfo = invalidTrain[i];
        QStandardItem *item;
        model->setItem(itemIdx, ETRAINNOCOL, item = new QStandardItem(trainInfo[ESTATIONTRAINCODE].isEmpty() ?
                                                                    "--" : trainInfo[ESTATIONTRAINCODE]));
        item->setTextAlignment(Qt::AlignCenter);
        item->setToolTip(trainInfo[ESTATIONTRAINCODE]);
        fromStationName = stationMap.value(trainInfo[EFROMSTATIONTELECODE]).toString();
        toStationName = stationMap.value(trainInfo[ETOSTATIONTELECODE]).toString();
        model->setItem(itemIdx, EFROMSTATIONCOL, item = new QStandardItem);
        item->setToolTip(fromStationName);
        model->setItem(itemIdx, ETOSTATIONCOL, item = new QStandardItem);
        item->setToolTip(toStationName);
        for (int k = ESTARTTIMECOL; k < EREMARKCOL; k++) {
            model->setItem(itemIdx, k, item = new QStandardItem(_("--")));
            item->setTextAlignment(Qt::AlignCenter);
        }
        QPushButton *button = dynamic_cast<QPushButton *>(tableView->indexWidget(model->index(itemIdx, EREMARKCOL)));
        if (button) {
            disconnect(button, &QPushButton::clicked, this, &MainWindow::addTrainToSelected);
            tableView->setIndexWidget(model->index(itemIdx, EREMARKCOL), nullptr);
        }
        model->setItem(itemIdx, EREMARKCOL, item = new QStandardItem(trainInfo[ETEXTINFO]));
        item->setTextAlignment(Qt::AlignCenter);
        item->setToolTip(trainInfo[ETEXTINFO]);
        itemIdx++;
    }

    if (ud->runStatus == EIDLE) {
        if (useTrainListSize == trainListSize) {
            formatOutput(_("%1->%2(%3) 共查询到 %4 趟车次, 可预订 %5 趟车次").
                         arg(staFromName, staToName, tourDate).arg(trainListSize).arg(can_booking));
        } else {
            formatOutput(_("%1->%2(%3) 共查询到 %4 趟车次, 已过滤 %5 趟车次, 可预订 %6 趟车次").
                         arg(staFromName, staToName, tourDate).arg(trainListSize)
                         .arg(trainListSize - useTrainListSize).arg(can_booking));
        }
    }
}

void MainWindow::setStationNameCompleter(const QByteArray &nameText)
{
    InputCompleter *ic, *ic2, *ic3;
    UserData *ud = UserData::instance();
    UserConfig &uc = ud->getUserConfig();

    ic = new InputCompleter(this);
    ic->setStationData(nameText);
    ic->setCaseSensitivity((Qt::CaseInsensitive));
    fromStationLe->setCompleter(ic);
    ic->popup()->setStyleSheet("background-color: #F5F5F5;\
                               color: #000000;\
                               border: 1px solid #BEBEBE;\
                               border-radius: 5px;\
                               padding: 2px 2px 2px 2px;\
                               min-width: 8px;\
                               font: 13px \"Arial\";");
    if (!uc.staFromName.isEmpty() && uc.staFromCode.isEmpty()) {
        uc.staFromCode = ud->getStaCode()->value(uc.staFromName);
    }

    ic2 = new InputCompleter(ic);
    *ic2 = *ic;
    ic2->setCaseSensitivity((Qt::CaseInsensitive));
    toStationLe->setCompleter(ic2);
    ic2->popup()->setStyleSheet("background-color: #F5F5F5;\
                               color: #000000;\
                               border: 1px solid #BEBEBE;\
                               border-radius: 5px;\
                               padding: 2px 2px 2px 2px;\
                               min-width: 8px;\
                               font: 13px \"Arial\";");
    if (!uc.staToName.isEmpty() && uc.staToCode.isEmpty()) {
        uc.staToCode = ud->getStaCode()->value(uc.staToName);
    }
    ic3 = new InputCompleter(ic);
    *ic3 = *ic;
    ic3->setCaseSensitivity((Qt::CaseInsensitive));
    selltimeDialog->setCompleter(ic3);
    ic3->popup()->setStyleSheet("background-color: #F5F5F5;\
                                color: #000000;\
                                border: 1px solid #BEBEBE;\
                                border-radius: 5px;\
                                padding: 2px 2px 2px 2px;\
                                min-width: 8px;\
                                font: 13px \"Arial\";");
}

bool MainWindow::hasStationNameCompleter()
{
    return fromStationLe->completer() != nullptr && toStationLe->completer() != nullptr;
}

void MainWindow::queryTicket()
{
    UserData *ud = UserData::instance();
    UserConfig &uc = ud->getUserConfig();

    if (uc.staFromCode.isEmpty()) {
        formatOutput(_("请先填写出发站！"));
        return;
    }
    if (uc.staToName.isEmpty()) {
        formatOutput(_("请先填写到达站！"));
        return;
    }

    if (!uc.staFromCode.isEmpty() && !uc.staToCode.isEmpty()) {
        if (ud->runStatus == EGRABTICKET || ud->runStatus == EIDLE) {
            NetHelper::instance()->queryTicket();
        }
    } else {
        formatOutput(_("出发站或到达站的站点识别码为空！"));
    }
}

void MainWindow::queryTrainStopStation()
{
    QItemSelectionModel *modelSelection = tableView->selectionModel();
    QModelIndexList indexList= modelSelection->selectedRows();
    QList<int> columns = {
        ETRAINNOCOL,
        EFROMSTATIONCOL,
        ETOSTATIONCOL
    };
    QList<QString> args;
    UserData *ud = UserData::instance();

    if (!indexList.isEmpty()) {
        const QStandardItemModel *itemModel;
        const QStandardItem *item;
        for (auto &col : columns) {
            itemModel = dynamic_cast<const QStandardItemModel *>(indexList[0].model());
            item = dynamic_cast<const QStandardItem *>(itemModel->item(indexList[0].row(), col));
            QString data = item->data(Qt::UserRole).toString();
            if (!data.isEmpty()) {
                args.push_back(data);
            }
        }

        if (args.size() == columns.size()) {
            if (!ud->getUserConfig().tourDate.isEmpty()) {
                args.push_back(ud->getUserConfig().tourDate);
                NetHelper::instance()->queryTrainStopStation(args, EQUERYSTOPSTATIONSHOW);
            }
        }
    }
}

void MainWindow::processStopStationReply(QVariantMap &data)
{
    QVariantMap data1 = data[_("data")].toMap();
    if (!data.isEmpty()) {
        QStringList headers = {
            _("站序"),
            _("站名"),
            _("到站时间"),
            _("出站时间"),
            _("停靠时间")
        };
        QStringList columnsData = {
                               _("station_no"),
            _("station_name"),
            _("arrive_time"),
            _("start_time"),
            _("stopover_time")
        };
        QString disp;
        QVariantList data2 = data1[_("data")].toList();
        QTableWidget *tw = new QTableWidget(data2.length(), headers.size());
        tw->setHorizontalHeaderLabels(headers);
        tw->setSelectionMode(QAbstractItemView::NoSelection);
        tw->setEditTriggers(QAbstractItemView::NoEditTriggers);

        for (int i = 0; i < data2.length(); i++) {
            QVariantMap data3 = data2[i].toMap();
            QString s;
            if (i == 0) {
                s = data3[_("station_train_code")].toString();
                disp += s + _("  ");
                s = data3[_("start_station_name")].toString();
                disp += s + _("->");
                s = data3[_("end_station_name")].toString();
                disp += s + _("  ");
                s = data3[_("train_class_name")].toString();
                disp += s.isEmpty() ? _("  ") : s + _("  ");
                s = data3[_("service_type")].toString();
                disp += s == '0' ? _("无空调") : _("有空调");
            }
            int j = 0;
            for (auto &column : columnsData) {
                s = data3[column].toString();
                QTableWidgetItem *item = new QTableWidgetItem(s);
                item->setToolTip(s);
                tw->setItem(i, j, item);
                j++;
            }
        }
        QDialog m(this);
        QLabel l(disp);
        QVBoxLayout vlayout;
        if (tw->rowCount()) {
            tw->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
            //tw->resizeColumnsToContents();
            vlayout.addWidget(&l);
            vlayout.addWidget(tw);
            m.setLayout(&vlayout);
            m.setWindowTitle(_("停靠站信息"));
            m.resize(350, 250);
            m.exec();
        }
        delete tw;
    }
}

void MainWindow::enterGrabTicketMode()
{
    QStandardItemModel *model = static_cast<QStandardItemModel *>(tableView->model());
    int rowcount = model->rowCount();
    QPushButton *pb;

    for (int i = 0; i < rowcount; i++) {
        pb = static_cast<QPushButton *>(tableView->indexWidget(model->index(i, EREMARKCOL)));
        if (pb) {
            pb->setEnabled(false);
        }
    }
}

void MainWindow::exitGrabTicketMode()
{
    QStandardItemModel *model = static_cast<QStandardItemModel *>(tableView->model());
    int rowcount = model->rowCount();
    QPushButton *pb;

    for (int i = 0; i < rowcount; i++) {
        pb = static_cast<QPushButton *>(tableView->indexWidget(model->index(i, EREMARKCOL)));
        if (pb) {
            pb->setEnabled(true);
        }
    }
}

void MainWindow::enterGrabMode()
{
    UserData *ud = UserData::instance();
    UserConfig &uc = ud->getUserConfig();
    ud->setRunStatus(EGRABTICKET);
    ud->candidateRunStatus = EGRABCANDIDATETICKET;
    grabTicketPb->setText(tr("停止"));
    fromStationLe->setEnabled(false);
    toStationLe->setEnabled(false);
    swapStationPb->setEnabled(false);
    tourDateDe->setEnabled(false);

    doGrabTicketTimer->setInterval(1000);
    doGrabTicketTimer->start();
    updateProgressBarTimer->start(100);
    grabTicketInterval = 1000;
    if (uc.showTicketPrice) {
        switchTicketShowType();
    }
    queryTicketPb->setEnabled(false);
    switchTicketShowTypePb->setEnabled(false);
    passengerDialog->enterGrabTicketMode();
    seatTypeDialog->enterGrabTicketMode();
    trainNoDialog->enterGrabTicketMode();
    tableView->setContextMenuPolicy(Qt::NoContextMenu);
    enterGrabTicketMode();

    QDateTime cur = QDateTime::currentDateTime();
    QDateTime end;
    if (cur.time().hour() != 0) {
        if (cur.date().dayOfWeek() != 2) {
            end = cur.addDays(1);
            end.setTime(QTime(1, 0, 0));
        } else {
            end.setTime(QTime(23, 30, 0));
        }
    } else {
        end = cur;
        end.setTime(QTime(1, 0, 0));
    }

    qint64 interval = cur.secsTo(end);
    if (interval > 0) {
        skipMaintenanceTimer->setInterval(interval * 1000);
        skipMaintenanceTimer->start();
    }
}

void MainWindow::exitGrabMode()
{
    UserData *ud = UserData::instance();
    ud->setRunStatus(EIDLE);
    ud->candidateRunStatus = EIDLE;
    grabTicketPb->setText(tr("开始"));
    fromStationLe->setEnabled(true);
    toStationLe->setEnabled(true);
    swapStationPb->setEnabled(true);
    tourDateDe->setEnabled(true);

    doGrabTicketTimer->stop();
    updateProgressBarTimer->stop();
    grabTicketInterval = 0;

    queryTicketPb->setEnabled(true);
    switchTicketShowTypePb->setEnabled(true);
    passengerDialog->exitGrabTicketMode();
    seatTypeDialog->exitGrabTicketMode();
    trainNoDialog->exitGrabTicketMode();
    tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    exitGrabTicketMode();

    if (skipMaintenanceTimer->isActive()) {
        skipMaintenanceTimer->stop();
    }
}

void MainWindow::prepareGrabTicket(bool status)
{
    UserData *ud = UserData::instance();

    if (status) {
        if (ud->grabSetting.fixedTimeGrab) {
            QDateTime now = QDateTime::currentDateTime();
            QDateTime end(now);
            QStringList dateList = ud->grabSetting.grabTicketDate.split('-');
            if (dateList.size() > 1) {
                end.setDate(QDate(now.date().year(), dateList[0].toInt(), dateList[1].toInt()));
            }
            end.setTime(QTime(ud->grabSetting.grabTicketHour,
                              ud->grabSetting.grabTicketMinute,
                              ud->grabSetting.grabTicketSecond));
            qint64 secs = now.secsTo(end);
            if (secs > 0) {
                fixedTimeGrabTimer->setInterval(secs * 1000);
                fixedTimeGrabTimer->start();
                formatOutput(_("定时抢票模式，已设置定时抢票时间为%1").arg(end.toString(_("yyyy-MM-dd hh:mm:ss"))));
                if (settingDialog->setQueryTicketMode(EFIXEDTIME)) {
                    formatOutput(_("刷票模式已自动切换为定时抢票模式"));
                }
                grabTicketPb->setText(tr("停止"));
                return;
            }
        }
        enterGrabMode();
        if (settingDialog->isFixedTimeMode() &&
            settingDialog->setQueryTicketMode(ESHORTINTERVAL)) {
            formatOutput(_("刷票模式已自动切换为默认模式"));
        }
        NetHelper::instance()->getLoginConf();
    } else {
        if (fixedTimeGrabTimer->isActive()) {
            fixedTimeGrabTimer->stop();
            grabTicketPb->setText(tr("开始"));
            return;
        }
        exitGrabMode();
    }
}

bool MainWindow::promptBeforeStartGrab()
{
    QString prompt;
    UserData *ud = UserData::instance();
    prompt += _("候补：");
    if (ud->candidateSetting.isCandidate) {
        prompt += _("<b>已启用</b><br>");
    } else {
        prompt += _("未启用<br>");
    }
    prompt += _("定时抢票：");
    if (ud->grabSetting.fixedTimeGrab) {
        prompt += _("<b>%1 %2:%3:%4</b><br><br>").arg(ud->grabSetting.grabTicketDate)
                      .arg(ud->grabSetting.grabTicketHour, 2, 10, QLatin1Char('0'))
                      .arg(ud->grabSetting.grabTicketMinute, 2, 10, QLatin1Char('0'))
                      .arg(ud->grabSetting.grabTicketSecond, 2, 10, QLatin1Char('0'));
    } else {
        prompt += _("未启用<br><br>");
    }

    prompt += _("日期：%1<br>").arg(ud->userConfig.tourDate);
    prompt += _("起始站：%1<br>").arg(ud->userConfig.staFromName);
    prompt += _("到达站：%1<br><br>").arg(ud->userConfig.staToName);
    prompt += _("乘车人：");
    const QList<QString> &passengers = passengerDialog->getSelectedPassenger();
    for (auto &p : passengers) {
        prompt += p + _("、");
    }
    prompt.truncate(prompt.length() - 1);

    prompt += _("<br>席别：");
    const QList<QString> &seatType = seatTypeDialog->getSelectedSeatType();
    for (auto &s : seatType) {
        prompt += s + _("、");
    }
    prompt.truncate(prompt.length() - 1);

    prompt += _("<br>车次：");
    const QList<QString> &train = trainNoDialog->getSelectedTrainList();
    int count = qMin(train.size(), 50);
    for (int i = 0; i < count; i++) {
        QString s = train[i];
        prompt += s.remove('(') + _("、");
    }
    if (count < train.size()) {
        prompt += _("...(更多已隐藏)");
    }
    prompt.truncate(prompt.length() - 1);
    prompt += _("<br>");

    /*QLabel la;
    la.setText(prompt);
    QDialog dialog;
    QVBoxLayout vlayout;
    vlayout.addWidget(&la);
    dialog.setLayout(&vlayout);
    return dialog.exec();*/
    QMessageBox msg;
    msg.setTextFormat(Qt::RichText);
    QMessageBox::StandardButton clicked = msg.information(this, tr("确认信息"), prompt,
                                            QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    return clicked == QMessageBox::Yes;
}

void MainWindow::startOrStopGrabTicket()
{
    static bool grabTicketPbStatus;

    UserData *ud = UserData::instance();
    UserConfig &uc = ud->getUserConfig();

    if (!grabTicketPbStatus) {
        if (uc.staFromCode.isEmpty()) {
            formatOutput(_("请先填写出发站！"));
            return;
        }
        if (uc.staToName.isEmpty()) {
            formatOutput(_("请先填写到达站！"));
        }
        const QStringList &selectedPassengerList = passengerDialog->getSelectedPassenger();
        if (selectedPassengerList.isEmpty()) {
            formatOutput(_("请先选择乘车人！"));
            return;
        }
        const QStringList &selectedTrainList = trainNoDialog->getSelectedTrainList();
        if (selectedTrainList.isEmpty()) {
            formatOutput(_("请先选择车次！"));
            return;
        }
        const QStringList &selectedSeatTypeList = seatTypeDialog->getSelectedSeatType();
        if (selectedSeatTypeList.isEmpty()) {
            formatOutput(_("请先选择席别！"));
            return;
        }
        if (!promptBeforeStartGrab()) {
            return;
        }
    }
    ud->lastRunSuccess = false;
    grabTicketPbStatus = !grabTicketPbStatus;
    prepareGrabTicket(grabTicketPbStatus);
}

void MainWindow::doGrabTicket()
{
    static int enterTimes;
    UserData *ud = UserData::instance();
    if (ud->runStatus == EGRABTICKET) {
        if (settingDialog->isShortMode()) {
            grabTicketInterval = 3000;
        } else if (settingDialog->isRandomMode()) {
            std::uniform_int_distribution<int> dist(2000, 6000);
            grabTicketInterval = dist(*QRandomGenerator::global());
        } else if (settingDialog->isFixedTimeMode()) {
            enterTimes++;
            if (enterTimes >= 15) {
                settingDialog->setQueryTicketMode(ESHORTINTERVAL);
                enterTimes = 0;
            }
            grabTicketInterval = 1000;
        } else if (settingDialog->isCustomMode()) {
            grabTicketInterval = ud->grabSetting.grabIntervalSeconds > 0 ?
                ud->grabSetting.grabIntervalSeconds * 1000 : 1000;
        } else {
            grabTicketInterval = 3000;
        }

        doGrabTicketTimer->setInterval(grabTicketInterval);
        updateProgressBarTimer->setInterval(grabTicketInterval >> 3);
        nextRequestProgress->setRange(0, grabTicketInterval);
        nextRequestProgress->reset();
        queryTicket();
    }
}

void MainWindow::switchTableTicketShowType(bool showTicketPrice)
{
    QStandardItemModel *model = static_cast<QStandardItemModel *>(tableView->model());
    int rowCount = model->rowCount();
    QVariant data;
    QString text;
    QStandardItem *item;
    int role;
    int type;

    if (showTicketPrice) {
        role = Qt::ToolTipRole;
    } else {
        role = Qt::WhatsThisRole;
    }

    for (int i = 0; i < rowCount; i++) {
        for (int j = ESPECIALSEATCOL; j < EREMARKCOL; j++) {
            data = model->index(i, j).data(role);
            item = model->item(i, j);

            if (data.isValid()) {
                text = data.toString();
                if (text != '0' && text != _("--")) {
                    item->setText(text);
                    if (showTicketPrice) {
                        item->setForeground(QBrush(QColor(238, 118, 33)));
                    } else {
                        type = item->data(Qt::UserRole).toInt();
                        setRemainTicketColor(text, item, CANCANDIDATE(type));
                    }
                } else {
                    item->setText(_("--"));
                    item->setForeground(Qt::black);
                }
            } else {
                item->setText(_("--"));
                item->setForeground(Qt::black);
            }
        }
    }
}

void MainWindow::switchTicketShowType()
{
    UserConfig &uc = UserData::instance()->getUserConfig();
    uc.showTicketPrice = !uc.showTicketPrice;
    if (uc.showTicketPrice) {
        switchTicketShowTypePb->setText(tr("显示余票"));
    } else {
        switchTicketShowTypePb->setText(tr("显示票价"));
    }
    switchTableTicketShowType(uc.showTicketPrice);
}

void MainWindow::resetLoginDialog()
{
    loginDialog->reset();
}

void MainWindow::showLoginDialog()
{
    resetLoginDialog();
    //hide();
    loginDialog->show();
}

void MainWindow::showMainWindow()
{
    loginDialog->hide();
    //show();
}

void MainWindow::playMusic()
{
    UserData *ud = UserData::instance();
    QString musicFile = ud->generalSetting.customMusic && !ud->generalSetting.customMusicPath.isEmpty()?
                            ud->generalSetting.customMusicPath : ud->generalSetting.musicPath;
    if (player->source() != musicFile) {
        player->setSource(musicFile);
    }
    player->play();
}

void MainWindow::stopPlayMusic()
{
    if (player) {
        player->stop();
    }
}

void MainWindow::startOrStopPlayMusic()
{
    static bool playing;
    UserData *ud = UserData::instance();

    if (playing) {
        stopPlayMusic();
        playMusicPb->setText(tr("试听音乐"));
        if (stopMusicTimer->isActive()) {
            stopMusicTimer->stop();
        }
    } else {
        playMusic();
        playMusicPb->setText(tr("停止音乐"));
        if (ud->generalSetting.stopAfterTime &&
            !stopMusicTimer->isActive()) {
            stopMusicTimer->start(10 * 60 * 1000);
        }
    }
    playing = !playing;
}

void MainWindow::setMusicPath(const QString &path)
{
    if (player) {
        player->setSource(path);
    }
}

#ifdef HAS_CDN
void MainWindow::updateAvaliableCdnNum(int num)
{
    cdnIndicatorLabel->setText(_("CDN: %1 ").arg(num));
}

void MainWindow::getCdn()
{
    UserData *ud = UserData::instance();
    if (ud->generalSetting.cdnEnable) {
        NetHelper::instance()->getCdn();
    }
}
#endif

void MainWindow::checkUpdateReply(const QVariantMap &varMap)
{
    // status = 0 不需要更新
    // status = 1 需要更新
    // status = 2 错误
    int status = varMap[_("status")].toInt();
    QString msg;
    //QString url;
    QVariantMap verInfo;
    //QMessageBox::StandardButton button;
    QLabel *msgLabel;
    QScrollArea *scrollArea;
    QDialog dialog;
    QVBoxLayout vlayout;
    QHBoxLayout hlayout;
    QPushButton yes(tr("是")), no(tr("否"));

    switch (status) {
    case 0:
        break;
    case 1:
        verInfo = varMap[_("verInfo")].toMap();
        upgradeMng.setNewVersion(verInfo[_("version")].toString());
        upgradeMng.setUpgradeUrl(verInfo[_("downloadUrl")].toString());
        msg += _("新版本可用，是否更新？\n");
        msg += _("新版本：\n") + verInfo[_("version")].toString() + _("\n");
        msg += _("当前版本：\n") + _(THISVERSION) + _("\n\n");
        msg += _("更新日志：\n") + verInfo[_("changeLog")].toString() + _("\n");
        msgLabel = new QLabel;
        msgLabel->setText(msg);
        msgLabel->setMargin(10);
        scrollArea = new QScrollArea;
        scrollArea->setWidget(msgLabel);
        vlayout.addWidget(scrollArea);
        connect(&yes, &QPushButton::clicked, this, [&dialog, this]() {
            dialog.close();
            confirmUpdate();
        });
        connect(&no, &QPushButton::clicked, &dialog, [&dialog]() {
            dialog.close();
        });
        hlayout.addStretch();
        hlayout.addWidget(&yes);
        hlayout.addWidget(&no);
        hlayout.addStretch();
        vlayout.addLayout(&hlayout);
        dialog.setLayout(&vlayout);
        dialog.setStyleSheet("QDialog { background-color: #EEE9E9; }"
                             "QLabel { background-color: #F5F5F5; } "
                             "QScrollArea { background-color: #F5F5F5; }");
        dialog.resize(300, 300);
        dialog.exec();

        // msgLabel would be destroyed when destroying scrollArea
        delete scrollArea;

        /*button = QMessageBox::information(this, _("版本更新"), msg, QMessageBox::Yes | QMessageBox::No);
        if (button == QMessageBox::Yes) {
            url = verInfo[_("downloadUrl")].toString();
            QDesktopServices::openUrl(QUrl(url));
        }*/
        break;
    case 2:
        msg = varMap[_("msg")].toString();
        if (!msg.isEmpty()) {
             QMessageBox::warning(this, _("更新失败"), msg);
        }
        break;
    default:
        break;
    }
}

void MainWindow::confirmUpdate()
{
    upgradeMng.prepareUpgrade();
}

void MainWindow::rightMenuSelectTrain()
{
    QItemSelectionModel *modelSelection = tableView->selectionModel();
    QModelIndexList indexList= modelSelection->selectedRows();

    foreach (QModelIndex index, indexList) {
        const QStandardItemModel *itemModel = dynamic_cast<const QStandardItemModel *>(index.model());
        const QStandardItem *item = dynamic_cast<const QStandardItem *>(itemModel->item(index.row(), 0));
        const QStandardItem *item1 = dynamic_cast<const QStandardItem *>(itemModel->item(index.row(), 1));
        const QStandardItem *item2 = dynamic_cast<const QStandardItem *>(itemModel->item(index.row(), 2));
        trainNoDialog->addSelectedTrain(_("%1 (%2 %3").arg(item->text(), item1->text(), item2->text()));
    }
}

void MainWindow::rightMenuUnselectTrain()
{
    QItemSelectionModel *modelSelection = tableView->selectionModel();
    QModelIndexList indexList= modelSelection->selectedRows();

    foreach (QModelIndex index, indexList) {
        const QStandardItemModel *itemModel = dynamic_cast<const QStandardItemModel *>(index.model());
        const QStandardItem *item = dynamic_cast<const QStandardItem *>(itemModel->item(index.row(), 0));
        const QStandardItem *item1 = dynamic_cast<const QStandardItem *>(itemModel->item(index.row(), 1));
        const QStandardItem *item2 = dynamic_cast<const QStandardItem *>(itemModel->item(index.row(), 2));
        trainNoDialog->removeSelectedTrain(_("%1 (%2 %3").arg(item->text(), item1->text(), item2->text()));
    }
}

void MainWindow::rightMenuSelectTrainAll()
{
    trainNoDialog->addSelectedTrainAll();
}

void MainWindow::rightMenuUnselectTrainAll()
{
    trainNoDialog->clearSelectedTrain();
}

void MainWindow::createRightMenu()
{
    rightMenu = new QMenu;
    QAction *select = new QAction("选中车次",this);
    QAction *unSelect = new QAction("取消选中车次",this);
    QAction *selectAll = new QAction("选中所有车次",this);
    QAction *unSelectAll = new QAction("取消选中所有车次",this);
    QAction *showStopStation = new QAction("显示停靠站信息",this);

    connect(select, &QAction::triggered, this, &MainWindow::rightMenuSelectTrain);
    connect(unSelect, &QAction::triggered, this, &MainWindow::rightMenuUnselectTrain);
    connect(selectAll, &QAction::triggered, this, &MainWindow::rightMenuSelectTrainAll);
    connect(unSelectAll, &QAction::triggered, this, &MainWindow::rightMenuUnselectTrainAll);
    connect(showStopStation, &QAction::triggered, this, &MainWindow::queryTrainStopStation);

    rightMenu->addAction(select);
    rightMenu->addAction(unSelect);
    rightMenu->addAction(selectAll);
    rightMenu->addAction(unSelectAll);
    rightMenu->addAction(showStopStation);
}

void MainWindow::setUpTableView()
{
    tableView = new QTableView;
    QStandardItemModel *model = new QStandardItemModel;
    tableView->verticalHeader()->setDefaultSectionSize(20);
    tableView->resize(tableView->sizeHint());
    //QCheckBox *checkBoxAll = new QCheckBox;
    //tableView->setCornerWidget(checkBoxAll);
    //tableView->setCornerButtonEnabled(true);
    const QStringList trainTableColumnDesc = { "车次", "出发站", "到达站", "出发时间",
                                               "到达时间", "用时", "商务/特等", "优一等座",
                                               "一等座", "二等座", "高级软卧", "软卧/动卧",
                                               "硬卧", "软座", "硬座", "无座",
                                               "其他", "备注"
    };
    Q_ASSERT(trainTableColumnDesc.size() == ETRAINTABLECOLUMNENDTOTAL);
    /*model->setColumnCount(ETRAINTABLECOLUMNENDTOTAL);
    for (size_t i = 0; i < ETRAINTABLECOLUMNENDTOTAL - 1; ++i) {
        model->setHeaderData(i, Qt::Horizontal, trainTableColumnDesc[i], Qt::DisplayRole);
    }
    model->setHeaderData(ETRAINTABLECOLUMNENDTOTAL - 1, Qt::Horizontal,
                         trainTableColumnDesc[ETRAINTABLECOLUMNENDTOTAL - 1], Qt::EditRole);*/
    model->setHorizontalHeaderLabels(trainTableColumnDesc);

    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableView->setSortingEnabled(true);
    tableView->verticalHeader()->hide();
    tableView->setContextMenuPolicy(Qt::CustomContextMenu);  //少这句，右键没有任何反应的。
    //列宽随窗口大小改变而改变，每列平均分配，充满整个表，但是此时列宽不能拖动进行改变
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    createRightMenu();  //创建一个右键菜单
    connect(tableView, &QTableView::customContextMenuRequested, this, [&] {
        if (tableView->model()->rowCount()) {
            rightMenu->exec(QCursor::pos());
        }
    });
    tableView->setModel(model);
    IconDelegate *iconDelegate = new IconDelegate(this);
    tableView->setItemDelegate(iconDelegate);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    /*int ret = QMessageBox::warning(this,
                                   tr("提示"),
                                   tr("退出程序吗？"),
                                   QMessageBox::Yes | QMessageBox::No);

    if (ret == QMessageBox::Yes) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }*/
    event->accept();
}

void MainWindow::updateNetQualityStatus(int ms)
{
    static int lastStatus = -1;
    int status;
    QPixmap pixmap;

    if (ms != -1) {
        if (ms < 100) {
            status = 0;
            if (status != lastStatus) {
                lastStatus = status;
                pixmap.load(_(":/icon/images/perfect.png"));
                netQualityIndicateLabel->setPixmap(pixmap);
            }
            netQualityIndicateDescLabel->setText(_("%1ms 非常好").arg(ms));
        } else if (ms < 500) {
            status = 1;
            if (status != lastStatus) {
                pixmap.load(_(":/icon/images/good.png"));
                netQualityIndicateLabel->setPixmap(pixmap);
            }
            netQualityIndicateDescLabel->setText(_("%1ms 良好").arg(ms));
        } else if (ms < 1000) {
            status = 2;
            if (status != lastStatus) {
                pixmap.load(_(":/icon/images/good.png"));
                netQualityIndicateLabel->setPixmap(pixmap);
            }
            netQualityIndicateDescLabel->setText(_("%1ms 差").arg(ms));
        } else {
            if (ms < 2000) {
                status = 3;
                netQualityIndicateDescLabel->setText(_("%1ms 非常差").arg(ms));
            } else {
                status = 4;
                netQualityIndicateDescLabel->setText(_("%1ms 非常差").arg(ms));
            }
            if (status != lastStatus) {
                lastStatus = status;
                pixmap.load(_(":/icon/images/bad.png"));
                netQualityIndicateLabel->setPixmap(pixmap);

            }
        }
        updateLatencyChart(ms);
    } else {
        status = 5;
        if (status != lastStatus) {
            lastStatus = status;
            pixmap.load(_(":/icon/images/bad.png"));
            netQualityIndicateLabel->setPixmap(pixmap);
            netQualityIndicateDescLabel->setText(tr("中断"));
        }
    }
}

void MainWindow::readSettings()
{
    UserConfig &uc = UserData::instance()->getUserConfig();
    QSettings setting;
    QString text = setting.value(_("query_ticket/from_station_name"), _("")).value<QString>();

    fromStationLe->setText(text);
    uc.staFromName = text;
    uc.staFromCode = UserData::instance()->getStaCode()->value(text);

    text = setting.value(_("query_ticket/to_station_name"), _("")).value<QString>();
    uc.staToName = text;
    uc.staToCode = UserData::instance()->getStaCode()->value(text);
    toStationLe->setText(text);

    text = setting.value(_("query_ticket/tour_date"), _("")).value<QString>();
    QDate curDate = QDate::currentDate();
    QDate setDate = QDate::fromString(text, _("yyyy-MM-dd"));

    if (!setDate.isValid() || setDate < curDate) {
        tourDateDe->setDate(curDate);
    } else {
        tourDateDe->setDate(setDate);
    }
    tourDateDe->setDateRange(curDate, curDate.addDays(14));
}

void MainWindow::initStatusBars()
{
    QPixmap pixmap;

#ifdef HAS_CDN
    cdnIndicatorLabel = new QLabel;
    cdnIndicatorLabel->setText(_("CDN: 0 "));
    statusBar()->addPermanentWidget(cdnIndicatorLabel);
#endif

    nextRequestProgress = new QProgressBar;
    nextRequestProgress->setMaximumSize(100, 20);
    statusBar()->addPermanentWidget(nextRequestProgress);

    netQualityIndicateLabel = new QLabel;
    pixmap.load(_(":/icon/images/perfect.png"));
    netQualityIndicateLabel->setPixmap(pixmap);
    statusBar()->addPermanentWidget(netQualityIndicateLabel);

    netQualityIndicateDescLabel = new QLabel;
    netQualityIndicateDescLabel->setText(_("非常好"));
    statusBar()->addPermanentWidget(netQualityIndicateDescLabel);

}

void MainWindow::showStatusBarMessage(const QString &message)
{
    statusBar()->showMessage(message);
}

void MainWindow::formatOutput(const QString &output)
{
    static QString textBuff;
    textBuff.clear();
    QDateTime date = QDateTime::currentDateTime();
    //textBuff += date.toString(Qt::ISODate);
    textBuff += date.toString(_("yyyy-MM-dd hh:mm:ss.zzz"));
    textBuff += QStringLiteral(" ") + output;
    browser->append(textBuff);
    UserData *ud = UserData::instance();
    if (ud->runStatus == ESUBMITORDER ||
        ud->candidateRunStatus == ESUBMITCANDIDATE) {
        historyInfoBrower->append(textBuff);
    }
}

void MainWindow::formatWithColorOutput(const QString &output, const QColor color)
{
    static QString textBuff;
    textBuff.clear();
    QDateTime date = QDateTime::currentDateTime();
    textBuff += date.toString(Qt::ISODate);
    textBuff += QStringLiteral(" ") + output;
    browser->setTextColor(color);
    browser->append(textBuff);
    browser->setTextColor(QColor(0, 0, 0));
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("关于云映"),
                       tr("<p>云映客户端版本 " THISVERSION "</p>"
                          "<p>本程序<a href=\"https://www.op9.top\">云映</a>仅限于个人使用，不可用于商业用途</p>"
                          "<p>本软件完全由个人靠业余时间开发，作者承诺软件不会上传或存储任何个人隐私信息，并且作者不会通过软件非法获利，请放心使用.</p>"
                          "<p>获取帮助&&问题反馈请发送邮件至gethelp@88.com</p>"
                          ));
}

void MainWindow::onlineHelp()
{
    QDesktopServices::openUrl(QUrl(_("https://op9.top/help.html#quick-start")));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete tableView->itemDelegate();
    delete tableView;
    delete stopMusicTimer;
}
