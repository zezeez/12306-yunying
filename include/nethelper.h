#ifndef NETHELPER_H
#define NETHELPER_H

#include <QObject>
#include <QMap>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>
#include <QTimer>
#include <QList>
#include <QNetworkCookieJar>
#include <cookieapi.h>
#include <frozentrain.h>

class QNetworkReply;

enum NETREQUESTTYPEENUM {
    EGETVARIFICATIONCODE = 1,
    EDOVARIFICATION,
    ELOGIN,
    EQUERYTICKET,
    EGETSTATIONNAMETXT,
    EPASSPORTUAMTK,
    EPASSPORTUAMTKCLIENT,
    EQUERYLOGINSTATUS,
    EGETPASSENGERINFO,
    EPASSENGERINITDC,
    ECHECKUSER,
    ESUBMITORDERREQUEST,
    ECHECKORDERINFO,
};

class ReqParam
{
public:
    void put(QString key, QString value);
    const QString &get() const;
    void finish();
    bool isEmpty() const;
private:
    QString data;
};

class NetHelper : public QObject
{
    Q_OBJECT
public:
    ~NetHelper();
    static NetHelper *instance();
    typedef void (NetHelper::*replyCallBack)(QNetworkReply *);
    void getCookieHeader(QNetworkReply *reply);
    void setCookieHeader(const QUrl &url, QNetworkRequest &request);
    void setHeader(const QUrl &url, QNetworkRequest &request);
    void post(const QUrl &url, ReqParam &param, replyCallBack rcb);
    void post(const QUrl &url, ReqParam &param, replyCallBack rcb, QList<std::pair<QString, QString>> &headers);
    void get(const QUrl &url, replyCallBack rcb);
    void get(const QUrl &url, const QString &ip, replyCallBack rcb);
    void get(const QUrl &url, replyCallBack rcb, QList<std::pair<QString, QString>> &headers);
    void ignoreReply(QNetworkReply *reply);
    void initLoginCookie();
    void initLoginCookieReply(QNetworkReply *reply);
    void getLoginConf();
    void getLoginConfReply(QNetworkReply *reply);
    void onLogin();
    void onSmsVerificationLogin(const QString &verification_code);
    void onLoginUamReply(QNetworkReply *reply);
    void isUamLogin();
    void maySetLocalTime(QNetworkReply *reply, int rttDelay);
    int caculateRTTDelay(QNetworkReply *reply, enum QNetworkReply::NetworkError errorNo);
    void isUamLoginReply(QNetworkReply *reply);
    int checkReplyOk(QNetworkReply *reply);
    int replyIsOk(QNetworkReply *reply, QVariantMap &varMap);
    void loginForUam(ReqParam &param);
    void loginForUamReply(QNetworkReply *reply);
    void loginForLocationPasscode(ReqParam &param);
    void loginForLocation();
    void loginForLocationPasscodeReply(QNetworkReply *reply);
    void loginIndex();
    void loginSuccess();
    void createQrCode();
    void createQrCodeReply(QNetworkReply *reply);
    void checkQrCodeReply(QNetworkReply *reply);
    void sendSmsRequest(const QString &idCardNumTail);
    void sendSmsRequestReply(QNetworkReply *reply);
    void queryTicket();
    void queryTicketForCDN();
    void queryTicketReply(QNetworkReply *reply);
    void queryDiffDateTicket(const QString &date);
    void queryDiffDateTicketReply(QNetworkReply *reply);
    void queryTrainStopStation(const QList<QString> &args);
    void queryTrainStopStationReply(QNetworkReply *reply);
    void passportUamtk();
    void passportUamtkReply(QNetworkReply *reply);
    void passportUamtkClient(const QString &apptk);
    void passportUamtkClientReply(QNetworkReply *reply);
    void userIsLogin();
    void getPassengerInfo();
    void getPassengerInfoReply(QNetworkReply *reply);
    void passengerInitDc();
    void checkUser();
    void checkUserReply(QNetworkReply *reply);
    void initDc();
    void initDcReply(QNetworkReply *reply);
    void handleError();
    void submitOrderRequest();
    void submitOrderRequestReply(QNetworkReply *reply);
    void checkOrderInfo();
    void checkOrderInfoReply(QNetworkReply *reply);
    void getQueueCount();
    void getQueueCountReply(QNetworkReply *reply);
    void confirmSingle();
    void confirmSingleReply(QNetworkReply *reply);
    // Async
    void confirmSingleForQueue();
    void confirmSingleForQueueReply(QNetworkReply *reply);
    void displayWaitTime();
    void queryOderWaitTime();
    void queryOderWaitTimeReply(QNetworkReply *reply);
    void resultOrderForDcQueue();
    void resultOrderForDcQueueReply(QNetworkReply *reply);
    void getStationNameTxt();
    void getStationNameTxtReply(QNetworkReply *);
    void saveStationNameFile(const QByteArray &nameText);
    void mayFrozenCurrentTrain(const QString &dispMsg);
    bool queryFrozenTrain(const QString &trainDesc);
    void grabTicketSuccess();
    void candidateTrain();
    void candidateTrainReply(QNetworkReply *reply);
    void showCandidateWarn(const QString &face_check_code, bool is_show_qrcode);
    void handlecandidateError();
    void candidateEntry(QList<QPair<QString, QChar>> &candidateSeatType);
    void chechFace(QList<QPair<QString, QChar>> &candidateSeatType);
    void checkFaceReply(QNetworkReply *reply);
    void submitCandidateOrderRequest();
    void submitCandidateOrderRequestReply(QNetworkReply *reply);
    void passengerInitApi();
    void passengerInitApiReply(QNetworkReply *reply);
    void getCandidateQueueNum();
    void getCandidateQueueNumReply(QNetworkReply *reply);
    QString generateCandidateTicketInfo();
    void confirmHB();
    void confirmHBReply(QNetworkReply *reply);
    void candidateQueryQueue();
    void candidateQueryQueueReply(QNetworkReply *reply);
    void lineUptoPayConfirm(const QString &reserve_no);

signals:
    void finished(QNetworkReply *reply);
public slots:

private:
    explicit NetHelper(QObject *parent = nullptr);
    NetHelper(const NetHelper &) Q_DECL_EQ_DELETE;
    NetHelper &operator=(const NetHelper rhs) Q_DECL_EQ_DELETE;

public:
    QNetworkAccessManager *nam;
    QMap<QNetworkReply *, replyCallBack> replyMap;
    QTimer *qrCodeRefreshTimer;
    QTimer *queryOrderTimer;
    QTimer *queryCandidateTimer;
    int orderWaitTime;
    int nextRequestOrderStatusTime;
    QVariantMap waitOrderInfo;
    CookieAPI *cookieJar;
    QMap<QNetworkReply *, qint64> rttMap;
    QVector<int> rttSamples;
    FrozenTrain frozenTrain;
    QVector<QPair<QString, bool>> candidateDiffDateStatus;
    QVector<QList<QPair<QString, QChar>>> candiateDiffDateData;
    QString queryLeftTicketUrl;
    QString chooseSeat;
    bool canChooseSeats;
};


#endif // NETHELPER_H
