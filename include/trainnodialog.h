#ifndef TRAINNODIALOG_H
#define TRAINNODIALOG_H

#include <QDialog>
#include <QStandardItemModel>
#include <QListWidget>
#include <QPushButton>
#include <QSet>
#include "doublelistwidget.h"

class TrainNoDialog : public QDialog
{
    Q_OBJECT
public:
    TrainNoDialog(QWidget *parent = nullptr);
    ~TrainNoDialog();
    void setUp();
    void updateSelectedTips(int leftCount, int rightCount);
    bool hasTrain(const QString &trainInfo);
    void addTrain(const QString &trainInfo);
    void addTrainFinish();
    void addSelectedTrain(const QString &trainInfo);
    void removeSelectedTrain(const QString &trainInfo);
    void addSelectedTrainAll();
    void clearSelectedTrain();
    void clearUnSelectedTrain();
    const QList<QString> &getSelectedTrainList() const;
    const QSet<QString> &getSelectedTrainSet() const;
    const QSet<QString> &getAllTrainSet() const;
    void setSelectedTrainNo();
    void setUnselectedTrainNo();
    void moveUpTrain();
    void moveDownTrain();
    void enterGrabTicketMode();
    void exitGrabTicketMode();

public:
    QSet<QString> trainSet;
    QSet<QString> selectedTrainSet;
    DoubleListWidget *dListWidget;
};

#endif // TRAINNODIALOG_H
