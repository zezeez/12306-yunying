#ifndef USERDATA_H
#define USERDATA_H
#include <QHash>
#include <QMultiHash>
#include <QStringList>
#include <QMap>
#include <QIODevice>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QString>
#include <QVariantMap>

enum TrainTableColumnEnum {
    ETRAINNOCOL = 0,
    EFROMSTATIONCOL,
    ETOSTATIONCOL,
    ESTARTTIMECOL,
    EARRIVETIMECOL,
    EUSEDTIMECOL,
    ESPECIALSEATCOL,
    EPRIFIRSTPRISEATCOL,
    EFIRSTPRISEATCOL,
    ESECONDPRISEATCOL,
    EADVSOFTCROUCHCOL,
    ESOFTCROUCHCOL,
    EHARDCROUCHCOL,
    ESOFTSEATCOL,
    EHARDSEATCOL,
    ENOSEATCOL,
    EOTHERCOL,
    EREMARKCOL,
    ETRAINTABLECOLUMNENDTOTAL
};

enum TrainInfoEnum {
    ESECRETSTR = 0,
    ETEXTINFO,
    ETRAINNO,
    ESTATIONTRAINCODE,
    ESTARTSTATIONTELECODE,
    EENDSTATIONTELECODE,
    EFROMSTATIONTELECODE,
    ETOSTATIONTELECODE,
    ESTARTTIME,
    EARRIVETIME,
    ESPENDTIME,
    ECANWEBBUY,
    EYPINFO,
    ESTARTTRAINDATE,
    ETRAINSEATFEATURE,
    ELOCATIONCODE,
    EFROMSTATIONNO,
    ETOSTATIONNO,
    EISSUPPORTCARD,
    ECONTROLLEDTRAINFLAG,
    EGGNUM,  // GG: "D_优选一等座"
    EGRNUM,  // GR: "6_高级软卧"
    EQTNUM,  // QT: "H_其他"
    ERWNUM,  // RW: "4_软卧"
    ERZNUM,  // RZ: "2_软座"
    ETZNUM,  // TZ: "P_特等座"
    EWZNUM,  // WZ: "1_无座"
    EYBNUM,  //
    EYWNUM,  // YW: "3_硬卧"
    EYZNUM,  // YZ: "1_硬座"
    EZENUM,  // ZE: "O_二等座"
    EZYNUM,  // ZY: "M_一等座"
    ESWZNUM,  // SWZ: "9_商务座"
    ESRRBNUM,  // SRRB: "F_动卧"
    EYPEX,
    ESEATTYPES,
    EEXCHANGETRAINFLAG,
    ECANDIDATETRAINFLAG,
    ECANDIDATESEATLIMIT,
    EYPINFONEW,
    EDWFLAG = 46,
    ESTOPCHECKTIME = 48,
    ECOUNTRYFLAG,
    ELOCALARRIVETIME,
    ELOCALSTARTTIME,
    EBEDLEVELINFO = 53,
    ESEATDISCOUNTINFO,
    ESALETIME,
    ETRAININFOMAX
};

enum SHOWTRAINTYPE {
    EALL,
    EGTRAIN,
    EDTRAIN,
    ECTRAIN,
    EZTRAIN,
    ETTRAIN,
    EKTRAIN,
    EOTRAIN
};

enum RUNSTATUSE {
    EINIT,
    EIDLE,
    EGRABTICKET,
    ESUBMITORDER,
    ESUBMITORDERSUCCESS,
    EGRABCANDIDATETICKET,
    ESUBMITCANDIDATE,
    EWAITDIFFDATEDATA,
    EGETDIFFDATEDATAFAILED,
    ESUBMITCANDIDATESUCCESS
};

enum GRABTICKETMODEE {
    ESHORTINTERVAL,
    ELONGINTERVAL,
    ESHORTANDLONG,
    ERANDOM,
    EFIXEDTIME,
    ECUSTOM
};

struct UserConfig {
    QString staFromName;
    QString staToName;
    QString staFromCode;
    QString staToCode;
    QString tourDate;
    bool showTicketPrice;
};

#define MAX_ALPHA_NUM 26
#define ISFUXING(x) (((x) & (1 << 0)) != 0)
#define ISZINENG(x) (((x) & (1 << 1)) != 0)
#define ISDONGGAN(x) (((x) & (1 << 2)) != 0)
#define CANCANDIDATE(x) (((x) & (1 << 3)) != 0)

#define SETFUXING(x, b) ((x) = (x) | ((b) << 0))
#define SETZINENG(x, b) ((x) = (x) | ((b) << 1))
#define SETDONGGAN(x, b) ((x) = (x) | ((b) << 2))
#define SETCANCANDIDATE(x, b) ((x) = (x) | ((b) << 3))

struct PassengerInfo {
    inline bool operator == (const struct PassengerInfo &info) const {
        if (!passName.compare(info.passName) && !passIdNo.compare(info.passIdNo))
            return true;
        return false;
    }
    QString allEncStr;
    QString passName;
    QString code;
    QString passIdTypeName;
    QString passIdTypeCode;
    QString passIdNo;
    QString passType;
    QString passTypeName;
    QString mobile;
    QString phone;
    QString indexId;
};

struct UserLoginInfo {
    QString account;
    QString passwd;
    QString userName;
};

struct SubmitTicketInfo {
    QString trainCode;
    QString trainNo;
    QString secretStr;
    QString ypDetailInfo;
    QString date;
    QString startSTationName;
    QString fromStationName;
    QString fromStationCode;
    QString toStationName;
    QString toStationCode;
    QString endStationName;
    QString fromTime;
    QString toTime;
    QString travelTime;
    QVector<QPair<QString, QChar>> submitSeatType;
    bool isAsync;
    bool submitSuccess;
    QString passengerTicketInfo;
    QString oldPassengerTicketInfo;
    QString repeatSubmitToken;
    QString leftTicketStr;
    QString purposeCodes;
    QString trainLocation;
    QString keyCheckIsChange;
    QString bedLevelInfo;
    QString seatDiscountInfo;
};

struct SeatTypePrioSetting {
    bool strictSeatTypePrio;
    bool sufficientSeatTypePrio;
    bool priceLowSeatTypePrio;
    bool priceHighSeatTypePrio;
    bool priceLowSeatTypePrioWhensufficient;
};

struct TimeRange {
    int beginHour;
    int beginMinute;
    int endHour;
    int endMinute;
};

struct TrainPrioSetting {
    bool strictTrainPrio;
    bool strictStartTimePrio;
    bool sufficientTicketPrio;
    bool travelTimeShortPrio;
    bool preferGPrio;
    bool preferDPrio;
    bool preferTimeRangePrio;
    struct TimeRange timeRange;
};

struct TicketSetting {
    bool partialSubmit;
};

struct CandidateSetting {
    int selectedEndCandidateMinutes;
    int endCandidateMinutes;
    bool isCandidate;
    bool prioCandidate;
    bool onlyCandidate;
    bool forceCandidate;
    bool acceptNoSeat;
    bool acceptNewTrain;
    bool extraCandidateDateEn;
    bool extraCandidateDate2En;
    QString extraCandidateDate;
    QString extraCandidateDate2;
    int extraCandidateStartHour;
    int extraCandidateEndHour;
};

struct CandidateTrainInfo
{
    QString trainCode;
    QString secretStr;
    QVector<QChar> seatType;
};

struct CandidateDateInfo {
    QVector<struct CandidateTrainInfo> train;
    QString date;
    bool hasUpdate;
};

struct CandidateInfo {
    QVector<struct CandidateDateInfo> diffDateTrain;
    QString submitSecretStr;
    QVector<QString> passengers;
    QVector<QChar> allSeatType;
};

struct EmailNofity {
    bool notifyEmailEnable;
    QString senderEmailAddress;
    QString authCode;
    QVector<QString> receiverEmailAddress;
    QString emailServer;
    quint16 emailPort;
    bool enableSsl;
    bool keepAuthCode;
    bool advanceMode;
};

struct WxNotify {
    bool enable;
    bool keepSendKey;
    QString sendKey;
};

struct NotifySetting {
    struct EmailNofity emailNotify;
    struct WxNotify wxNotify;
};

struct GrabTicketSetting {
    QVector<struct PassengerInfo> selectedPassenger;  //  选中的旅客信息
    QVector<struct PassengerInfo> selectedDjPassenger;  // 选中的受让人旅客信息
    QVector<QChar> seatTypeCode;
    struct SeatTypePrioSetting seatTypePrio;
    struct TrainPrioSetting trainPrio;
    struct TicketSetting ticketSetting;
    bool autoFrozenTrain;
    bool fixedTimeGrab;
    bool acceptNewTrain;
    bool isStudent;
    bool queryMidStation;
    int frozenSeconds;
    QString grabTicketDate;
    int grabTicketHour;
    int grabTicketMinute;
    int grabTicketSecond;
    enum GRABTICKETMODEE grabMode;
    int grabIntervalSeconds;
    int newTrainStartHour;
    int newTrainEndHour;
};

struct GeneralSetting {
    bool autoSyncServerTime;
    bool playMusic;
    bool stopAfterTime;
    bool customMusic;
#ifdef HAS_CDN
    bool cdnEnable;
#endif
    QString musicPath;
    QString customMusicPath;
    QString timeServer;
    int startTimeRange1;
    int startTimeRange2;
};

enum PayMethodE {
    EALIPAY,
    EWXPAY,
    EUNIONPAY,
    EGONGSHANGYHPAY,
    ENONGYEYHPAY,
    EZHONGGUOYHPAY,
    EJIANSHEYHPAY,
    EZHAOSHANGYHPAY,
    EYOUCHUYHPAY,
    EJIAOTONGYHPAY,
    EZHONGTIEYTPAY,
    EVISAPAY,
    EPAYMETHODMAX
};

struct PaySetting {
    bool activePay;
    enum PayMethodE payMethod;
};

class UserData
{
public:
    ~UserData();
    void setStationCode(const QByteArray &staName, const QByteArray &staCode);

    static inline UserData *instance()
    {
        static UserData ud;
        return &ud;
    }

    inline struct UserConfig &getUserConfig()
    {
        return userConfig;
    }
    inline struct UserLoginInfo &getUserLoginInfo()
    {
        return loginInfo;
    }
    inline void setUserLoginName(QString name)
    {
        loginInfo.userName = name;
    }
    inline void setUserLoginPaswd(QString passwd)
    {
        loginInfo.passwd = passwd;
    }
    inline struct GrabTicketSetting &getGrabTicketSetting()
    {
        return grabSetting;
    }
    inline QHash<QString, QString> *getStaCode()
    {
        return staCode;
    }
    QString getApptk()
    {
        return apptk;
    }
    void setApptk(QString tk)
    {
        apptk = tk;
    }
    QString getUamtk()
    {
        return uamtk;
    }
    void setUamtk(QString tk)
    {
        uamtk = tk;
    }

    struct PassengerInfo setPassengerInfo(QVariantMap &map);
    bool whatsSelect(bool onlyNormal);
    const QString getpassengerTickets();
    bool isTimeInRange(int hour, int minute);

    void setRunStatus(enum RUNSTATUSE status) {
        lastRunStatus = runStatus;
        runStatus = status;
    }
    void recoverRunStatus() {
        runStatus = lastRunStatus;
    }

    QString errorString() const;
private:
    explicit UserData();
    UserData(const UserData &) Q_DECL_EQ_DELETE;
    UserData &operator=(const UserData &rhs) Q_DECL_EQ_DELETE;
public:
    QHash<QString, QString> *staCode;
    struct UserConfig userConfig;
    struct UserLoginInfo loginInfo;

    struct GrabTicketSetting grabSetting;
    struct GeneralSetting generalSetting;
    struct SubmitTicketInfo submitTicketInfo;
    struct CandidateSetting candidateSetting;
    struct CandidateInfo candidateInfo;
    struct NotifySetting notifySetting;
    struct PaySetting paySetting;
    QVector<struct PassengerInfo> passenger;  // 旅客信息
    QVector<struct PassengerInfo> djPassenger;  //  冻结旅客信息
    QString apptk;
    QString uamtk;
    enum RUNSTATUSE runStatus;
    enum RUNSTATUSE lastRunStatus;
    enum RUNSTATUSE candidateRunStatus;
    bool lastRunSuccess;
};

QString seatTypeSubmtiCodeTransToDesc(QChar seatType);
QChar seatTypeEnumTransToSubmitCode(enum TrainInfoEnum seatType);
QString getAppDataPath();
QString getAppCachePath();

#endif // USERDATA_H
