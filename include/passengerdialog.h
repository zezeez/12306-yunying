#ifndef PASSENGERDIALOG_H
#define PASSENGERDIALOG_H
#include <QDialog>
#include <QPushButton>
#include "doublelistwidget.h"

class PassengerDialog : public QDialog
{
    Q_OBJECT
public:
    PassengerDialog(QWidget *parent = nullptr);
    ~PassengerDialog();
    void setUp();
    void refreshPassengerInfo();
    void updateSelectedTips(int leftCount, int rightCount);
    const QList<QString> &getSelectedPassenger() const;
    void setSelectedPassenger();
    void setUnselectedPassenger();
    void addSelectedPassenger(const QString &passengerName);
    void addUnSelectedPassenger(const QString &passengerName);
    void clearUnSelectedPassenger();
    void clearSelectedPassenger();
    void clearPassenger();
    void moveUpPassenger();
    void moveDownPassenger();
    void enterGrabTicketMode();
    void exitGrabTicketMode();

public:
    DoubleListWidget *dListWidget;
    QPushButton *refreshPassengerPb;
};

#endif // PASSENGERDIALOG_H
