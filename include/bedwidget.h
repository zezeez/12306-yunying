#ifndef BEDDIAG_H
#define BEDDIAG_H
#include <QObject>
#include <QWidget>
#include <QSpinBox>

enum BedPosition {
    BEDHIGH,  // 上铺
    BEDMIDDLE,  // 中铺
    BEDLOW  // 下铺
};

class BedWidget : public QWidget
{
    Q_OBJECT
public:
    BedWidget(QWidget *wdiget = nullptr);
    ~BedWidget();
    void clearSelectedBeds();
    int getChoosedBeds(enum BedPosition pos);

private:
    QSpinBox *bedNumSbox[3];
};

#endif // BEDDIAG_H
