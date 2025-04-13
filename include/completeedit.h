#ifndef COMPLETEEDIT_H
#define COMPLETEEDIT_H

#include <QLineEdit>
#include <QStringList>
#include <QStringListModel>
#include <QString>
#include <QCompleter>
#include <QStack>
#include <QPixmap>

#define MAX_ALPHA_NUM 26

class stationData: public QSharedData
{
public:
    stationData()
    {

    }
    ~stationData()
    {

    }
    void addData(const QPair<QByteArray, QString> &data)
    {
        d.push_back(data);
    }
    QVector<QPair<QByteArray, QString>> data() const
    {
        return d;
    }

private:
    QVector<QPair<QByteArray, QString>> d;
};

class InputCompleterData
{
public:
    InputCompleterData();
    InputCompleterData(const InputCompleterData &othre);
    ~InputCompleterData();
    void addStationName(const QByteArray &staName, const QByteArray &staFullPinYin);
    void addStationFullPinYin(const QByteArray &staName,
                              const QByteArray &staFullPinYin);
    void addStationSimplePinYin(const QByteArray &staName,
                                const QByteArray &staSimplePinYin,
                                const QByteArray &staFullPinYin);

    // 一级索引，长度256
    QVector<int> stationNameIndexLevel1;
    // 二级索引，长度256
    QVector<int> stationNameIndexLevel2;
    // 数据
    QVector<QSharedDataPointer<stationData>> stationNameData;
    QVector<int> stationFullPinYinIndexLevel1;
    QVector<int> stationFullPinYinIndexLevel2;
    QVector<QSharedDataPointer<stationData>> stationFullPinYinData;
    QVector<int> stationSimplePinYinIndexLevel1;
    QVector<int> stationSimplePinYinIndexLevel2;
    QVector<QSharedDataPointer<stationData>> stationSimplePinYinData;
};

class InputCompleter : public QCompleter
{
    Q_OBJECT

public:
    InputCompleter(QObject *parent = nullptr);
    InputCompleter &operator=(const InputCompleter &other);
    ~InputCompleter();

    void metaFilter(const QByteArray &word,
                                    QVector<QPair<QByteArray, QString>> &meta,
                                    QStringList &result);

    void update(const QByteArray &word);
    void setStationData(const QByteArray &nameText);

    inline QString word()
    {
        return m_word;
    }

private:
    InputCompleterData d;
    QStack<QVector<QPair<QByteArray, QString>>> keyWordStack;
    QStringListModel m_model;
    QByteArray m_word;
    bool isAppend;
};

class CompleteEdit : public QLineEdit
{
    Q_OBJECT

public:
    CompleteEdit(QWidget *parent = nullptr);
    ~CompleteEdit();

    void setCompleter(InputCompleter *c);
    InputCompleter *completer() const;
    void aa(const QString &);

protected:
    void keyPressEvent(QKeyEvent *e) override;
    void inputMethodEvent(QInputMethodEvent *) override;

private slots:
    void insertCompletion(const QString &completion);

private:
    InputCompleter *c;
};

#endif // COMPLETEEDIT_H
