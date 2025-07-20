#ifndef ANALYSIS_H
#define ANALYSIS_H
#include <QNetworkReply>
#include <QVector>
#include <QString>
#include "userdata.h"

class Analysis
{
public:
    explicit Analysis(QVector<QStringList> &trainVec);
    ~Analysis();
    void setAnalysisTrain(QVector<QStringList> &trainVec);
    int sufficientTicketPrioSelectTrain(const QVariantMap &stationMap);
    int strictTrainPrioSelectTrain(const QVariantMap &stationMap);
    int strictTravelTimePrioSelectTrain(const QVariantMap &stationMap);
    int strictStartTimePrioSelectTrain(const QVariantMap &stationMap, const QString &trainCode);
    QVector<std::pair<int, int>> trainSelectSeatType(int trainNoIdx, bool silent);
    std::pair<QString, QString> generateSubmitTicketInfo(int trainNoIdx, QVector<std::pair<int, int>> &seatVec, QVector<QPair<QString, QChar>> &submitSeatType);
    void initTrainTicketInfo();
    void sufficientSeatTypePrioSelectSeatType(const QVector<int> &selectSeatType,
                                                        QVector<std::pair<int, int>> &seatVec, int trainNoIdx, int passengerSize);
    void strictPriceLowToHighSelectSeatType(const QVector<int> &selectSeatType, QVector<int> priceLowToHigh,
                                                      QVector<std::pair<int, int>> &seatVec, int trainNoIdx, int passengerSize);
    int analysisOneTrain(QVector<std::pair<int, int>> &seatVec, const QVariantMap &stationMap, const QString &trainCode);
    int analysisTrain(std::pair<QString, QString> &ticketStr, QVector<QPair<QString, QChar>> &submitSeatType, const QVariantMap &stationMap);
    bool mayCandidate(const QVariantMap &stationMap, const QString &date);

private:
    QVector<QStringList> &trainInfoVec;
    QVector<QVector<int>> trainTicketInfo;
};

int seatTypeNameToTicketIndex(const QList<QString> &seatTypeStrList, QVector<int> &seatType);
QString ticketIndexToSeatTypeName(int ticketIndex);
QChar ticketIndexToCommitCode(int ticketIndex);
QChar seatTypeNoToCode(enum TrainInfoEnum seatTypeNo);
QChar seatTypeNameToCode(const QString &seatTypeName);
QString seatTypeCodeToName(QChar commitCode);

#endif // ANALYSIS_H
