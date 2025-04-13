#include "userdata.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QStandardPaths>
#include <QDir>

#define _ QStringLiteral

UserData::UserData(): staCode(new QHash<QString, QString>)
{
}

void UserData::setStationCode(const QByteArray &name, const QByteArray &code)
{
    staCode->insert(QString::fromUtf8(name), QString::fromUtf8(code));
}

struct PassengerInfo UserData::setPassengerInfo(QVariantMap &map)
{
    struct PassengerInfo pinfo;

    pinfo.allEncStr = map[QLatin1String("allEncStr")].toString();
    pinfo.passName = map[QLatin1String("passenger_name")].toString();
    pinfo.passIdTypeCode = map[QLatin1String("passenger_id_type_code")].toString();
    pinfo.passIdTypeName = map[QLatin1String("passenger_id_type_name")].toString();
    pinfo.passIdNo = map[QLatin1String("passenger_id_no")].toString();
    pinfo.passType = map[QLatin1String("passenger_type")].toString();
    pinfo.passTypeName = map[QLatin1String("passenger_type_name")].toString();
    pinfo.mobile = map[QLatin1String("mobile_no")].toString();
    pinfo.phone = map[QLatin1String("phone_no")].toString();
    pinfo.indexId = map[QLatin1String("index_id")].toString();

    return pinfo;
}

bool UserData::whatsSelect(bool onlyNormal)
{
    if (onlyNormal) {
        if (djPassenger.size() == 0)
            return true;
        return grabSetting.selectedPassenger.size() != 0;
    }
    return grabSetting.selectedDjPassenger.size() != 0;
}

bool UserData::isTimeInRange(int hour, int minute)
{
    int beginHour = grabSetting.trainPrio.timeRange.beginHour;
    int endHour = grabSetting.trainPrio.timeRange.endHour;
    if (hour >= beginHour && hour <= endHour) {
        int beginMinute = grabSetting.trainPrio.timeRange.beginMinute;
        int endMinute = grabSetting.trainPrio.timeRange.endMinute;
        if (hour == beginHour && hour == endHour) {
            return minute >= beginMinute && minute <= endMinute;
        } else if (hour == beginHour) {
            return minute >= beginMinute;
        } else if (hour == endHour) {
            return minute <= endMinute;
        } else {
            return true;
        }
    } else {
        return false;
    }
}

UserData::~UserData()
{
    delete staCode;
}

QString getAppDataPath()
{
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir;

    if (!dir.exists(dataPath)) {
        if (!dir.mkpath(dataPath)) {
            qWarning() << "Could not create data directory:" << dataPath;
            dataPath = _("./data");
            if (!dir.exists(dataPath) && !dir.mkpath(dataPath)) {
                return ".";
            }
        }
    }
    return dataPath;
}

QString getAppCachePath()
{
    QString cachePath = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    QDir dir;

    if (!dir.exists(cachePath)) {
        if (!dir.mkpath(cachePath)) {
            qWarning() << "Could not create data directory:" << cachePath;
            cachePath = _("./cache");
            if (!dir.exists(cachePath) && !dir.mkpath(cachePath)) {
                return ".";
            }
        }
    }
    return cachePath;
}
