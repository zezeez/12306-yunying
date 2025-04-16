#ifndef CUSTOMLABEL_H
#define CUSTOMLABEL_H
#include <QLabel>
#include <QGridLayout>

struct mapArea {
    bool active;
    bool selected;
    QPoint pos;
    QPixmap pixmap;
};

class VarCodeLabel : public QLabel
{
    Q_OBJECT
public:
    explicit VarCodeLabel(QWidget *parent = nullptr);
    ~VarCodeLabel() override;
    int getArea(int x, int y);
    QVector<mapArea> &getPoints();
    void clearSelected();

protected:
    void mouseMoveEvent(QMouseEvent *ev) override;
    void mousePressEvent(QMouseEvent *ev) override;
    void mouseReleaseEvent(QMouseEvent *ev) override;
    virtual void paintEvent(QPaintEvent* event) override;

private:
    QVector<mapArea> area;
    QLabel picLabel[9];
};

class ClickLabel : public QLabel
{
    Q_OBJECT
public:
    ClickLabel(QWidget *parent = nullptr);
    ~ClickLabel();

protected:
    void mousePressEvent(QMouseEvent *ev) override;
Q_SIGNALS:
    void clicked(int x, int y);
};

class CheckedLabel : public ClickLabel
{
    Q_OBJECT
public:
    explicit CheckedLabel(QWidget *parent = nullptr);
    ~CheckedLabel() override;
    void setChecked(bool checked);
    void reSetCheckedPos();

protected:
    void mousePressEvent(QMouseEvent *ev) override;
    //virtual void paintEvent(QPaintEvent* event) override;

private:
    QLabel checkedIndicator;
};

class MultiAreaLabel : public QWidget
{
    Q_OBJECT
public:
    explicit MultiAreaLabel(QWidget *parent = nullptr, int areaCount = 8, int areaWidth = 64, int areaHeight = 64);
    ~MultiAreaLabel();
    void setAreaCount(int c);
    int areaCount() const;
    void setAreaWidth(int w);
    int areaWidth() const;
    void setAreaHeight(int h);
    int areaHeight() const;
    void setAreaPixmap(int area, const QString &file);
    void setAreasPixmap(const QStringList &files);
    void show();
    void hide();
    void areaClicked();
    QGridLayout *layout();
    void setAreaSelected(int area);
    int areaSelected() const;

Q_SIGNALS:
    void clicked(int area);

private:
    int count;
    int width;
    int height;
    int areaSel;
    QGridLayout gridlayout;
    QList<CheckedLabel *> areaList;
};

#endif // CUSTOMLABEL_H
