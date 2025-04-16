#ifndef SEATTYPEDIALOG_H
#define SEATTYPEDIALOG_H

#include <QDialog>
#include <QMap>
#include <QListWidget>
#include <QPushButton>
#include "doublelistwidget.h"

class SeatTypeDialog : public QDialog
{
    Q_OBJECT
public:
    SeatTypeDialog(QWidget *parent = nullptr);
    ~SeatTypeDialog();
    void setUp();
    void updateSelectedTips(int leftCount, int rightCount);
    const QList<QString> &getSelectedSeatType() const;
    void setSelectedSeatType();
    void setUnselectedSeatType();
    void clearSelectedSeatType();
    void clearUnSelectedSeatType();
    void moveUpSeatType();
    void moveDownSeatType();
    void enterGrabTicketMode();
    void exitGrabTicketMode();

public:
    DoubleListWidget *dListWidget;
};

#endif // SEATTYPEDIALOG_H
