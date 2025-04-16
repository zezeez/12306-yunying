#include "customlabel.h"
#include <QKeyEvent>

VarCodeLabel::VarCodeLabel(QWidget *parent) :
    QLabel(parent), area(9)
{
    QVector<mapArea>::iterator it;
    for (it = area.begin(); it != area.end(); ++it) {
        (*it).active = false;
        (*it).selected = false;
    }

    for (int i = 0; i < 9; i++) {
        picLabel[i].setParent(this);
        picLabel[i].hide();
        picLabel[i].setFixedSize(64, 64);
        picLabel[i].setPixmap(QPixmap(QStringLiteral(":/icon/images/selected.svg")));
    }
}

void VarCodeLabel::mouseMoveEvent(QMouseEvent *ev)
{
    QLabel::mouseMoveEvent(ev);
}

void VarCodeLabel::mousePressEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton) {
        QPoint point;
        //point.setX(ev->x() + 10);
        //point.setY(ev->y() - 35);
        //point = ev->pos();
        point.setX(ev->position().x() + 20);
        point.setY(ev->position().y() - 35);
        int ar = getArea(ev->position().x(), ev->position().y());
        if (ar > 0 && ar <= 8) {
            struct mapArea &ma = area[ar];
            ma.pos = point;
            if (!picLabel[ar].isVisible()) {
                picLabel[ar].move(ev->position().x() - 30, ev->position().y() - 45);
                picLabel[ar].show();
            } else {
                picLabel[ar].hide();
            }
            ma.selected = !ma.selected;
        }
        qDebug() << "(x, y) = " << "(" << point.rx() << ", " << point.ry() << ")" << Qt::endl;
    }
}

void VarCodeLabel::mouseReleaseEvent(QMouseEvent *ev)
{
    QLabel::mouseReleaseEvent(ev);
}

void VarCodeLabel::paintEvent(QPaintEvent *ev)
{
    QLabel::paintEvent(ev);
}

int VarCodeLabel::getArea(int x, int y)
{
    int w = width();
    int h = height();
    int xstep = w / 4;
    int ystep = (h - 45) / 2;
    int i, j;

    for (i = 0; i < 2; i++) {
        for (j = 0; j < 4; j++) {
            int xmax = xstep * (j + 1);
            int xmin = xstep * j;
            int ymax = ystep * (i + 1);
            int ymin = ystep * i;
            if (x >= xmin && x < xmax && y >= ymin + 45 && y < ymax + 42) {
                return i * 4 + j + 1;
            }
        }
    }
    return 0;
}

void VarCodeLabel::clearSelected()
{
    for (int i = 0; i < area.size(); i++) {
        area[i].selected = false;
    }
    for (int i = 0; i < 9; i++) {
        picLabel[i].hide();
    }
}

QVector<mapArea> &VarCodeLabel::getPoints()
{
    return area;
}

VarCodeLabel::~VarCodeLabel()
{

}


ClickLabel::ClickLabel(QWidget *parent) :
    QLabel(parent)
{

}

ClickLabel::~ClickLabel()
{

}

void ClickLabel::mousePressEvent(QMouseEvent *ev)
{
    QLabel::mousePressEvent(ev);
    emit clicked(ev->position().x(), ev->position().y());
}

CheckedLabel::CheckedLabel(QWidget *parent) : ClickLabel(parent)
{
    checkedIndicator.setParent(this);
    //checkedIndicator.setFixedSize(64, 64);
    checkedIndicator.setPixmap(QPixmap(QStringLiteral(":/icon/images/selected.svg")));
}

CheckedLabel::~CheckedLabel()
{

}

void CheckedLabel::mousePressEvent(QMouseEvent *ev)
{
    checkedIndicator.show();
    ClickLabel::mousePressEvent(ev);
}

void CheckedLabel::setChecked(bool checked)
{
    if (checked) {
        checkedIndicator.show();
    } else {
        checkedIndicator.hide();
    }
}

void CheckedLabel::reSetCheckedPos()
{
    checkedIndicator.move(35, 0);
}

MultiAreaLabel::MultiAreaLabel(QWidget *parent, int areaCount, int areaWidth, int areaHeight) :
    QWidget(parent)
{
    count = areaCount;
    width = areaWidth;
    height = areaHeight;
    areaList.resize(areaCount);
    int row = 0, col = 0;
    for (int i = 0; i < areaCount; i++) {
        areaList[i] = new CheckedLabel;
        areaList[i]->setChecked(false);
        connect(areaList[i], &CheckedLabel::clicked, this, &MultiAreaLabel::areaClicked);
        gridlayout.addWidget(areaList[i], row, col);
        col++;
        if (col >= 4) {
            row++;
            col = 0;
        }
    }
    areaList[0]->setChecked(true);
    areaSel = 0;
}

MultiAreaLabel::~MultiAreaLabel()
{
    for (int i = 0; i < count; i++) {
        delete areaList[i];
    }
}

void MultiAreaLabel::setAreaCount(int c)
{
    count = c;
}

int MultiAreaLabel::areaCount() const
{
    return count;
}

void MultiAreaLabel::setAreaWidth(int w)
{
    width = w;
}

int MultiAreaLabel::areaWidth() const
{
    return width;
}

void MultiAreaLabel::setAreaHeight(int h)
{
    height = h;
}

int MultiAreaLabel::areaHeight() const
{
    return height;
}

void MultiAreaLabel::setAreaPixmap(int area, const QString &file)
{
    if (area >= count) {
        return;
    }

    areaList[area]->setPixmap(QPixmap(file));
    areaList[area]->reSetCheckedPos();
}

void MultiAreaLabel::setAreasPixmap(const QStringList &files)
{
    int n = qMin(count, files.size());
    for (int i = 0; i < n; i++) {
        setAreaPixmap(i, files[i]);
    }
}

void MultiAreaLabel::show()
{
    for (int i = 0; i < count; i++) {
        areaList[i]->show();
    }
}

void MultiAreaLabel::hide()
{
    for (int i = 0; i < count; i++) {
        areaList[i]->hide();
    }
}

void MultiAreaLabel::areaClicked()
{
    CheckedLabel *l = dynamic_cast<CheckedLabel *>(sender());
    for (int i = 0; i < count; i++) {
        if (areaList[i] != l) {
            areaList[i]->setChecked(false);
        } else {
            areaSel = i;
        }
    }
    emit clicked(areaSel);
}

QGridLayout *MultiAreaLabel::layout()
{
    return &gridlayout;
}

void MultiAreaLabel::setAreaSelected(int area)
{
    if (area < count) {
        for (int i = 0; i < count; i++) {
            areaList[i]->setChecked(false);
        }
        areaList[area]->setChecked(true);
    }
}

int MultiAreaLabel::areaSelected() const
{
    return areaSel;
}
