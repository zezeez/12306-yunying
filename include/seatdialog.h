#ifndef SEATDIALOG_H
#define SEATDIALOG_H
#include <QObject>
#include <QDialog>
#include <QVector>
#include <QComboBox>
#include <QLabel>
#include "customlabel.h"
#include "bedwidget.h"

class SeatDialog : public QDialog
{
    Q_OBJECT
public:
    SeatDialog(QWidget *parent = nullptr);
    ~SeatDialog();
    void showSeatType(int index);
    void updateSelectedTips();
    void clearSelectedSeats();
    void clearSelectedBeds();
    QString getChoosedSeats(QChar seatType);
    int getChoosedBeds(enum BedPosition pos);

private:
    QVector<QPair<QChar, QVector<int>>> seatsVec;
    QComboBox *seatTypeCB;
    QVector<QLabel *> seatDescLB;
    QVector<ClickLabel *> seatVCL;
    QVector<bool> seatSelected;
    BedWidget *bedWidget;
};

#endif // SEATDIALOG_H
