#include <QPropertyAnimation>
#include <QTimer>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>
#include <QPixmap>
#include <QFontMetrics>
#include <QDesktopServices>

#include "notify.h"

Notify::Notify (int displayTime, QWidget *parent) : QWidget(parent),
    closeMode(ECLOSEAUTO),
    displayTime(displayTime)
{

    this->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowSystemMenuHint| Qt::Tool | Qt::WindowStaysOnTopHint);
    this->setAttribute(Qt::WA_NoSystemBackground, true);
    this->setAttribute(Qt::WA_TranslucentBackground,true);

    backgroundLabel = new QLabel(this);
    backgroundLabel->move(0, 0);
    backgroundLabel->setObjectName("notify-background");


    QVBoxLayout *mainLayout = new QVBoxLayout(backgroundLabel);
    QHBoxLayout *contentLayout = new QHBoxLayout();

    iconLabel = new QLabel(backgroundLabel);
    iconLabel->setFixedWidth(40);
    iconLabel->setAlignment(Qt::AlignCenter);

    titleLabel = new QLabel(backgroundLabel);
    titleLabel->setObjectName("notify-title");
    titleLabel->setFixedHeight(14);

    bodyLabel = new QLabel(backgroundLabel);
    bodyLabel->setObjectName("notify-body");
    QFont font = bodyLabel->font();
    font.setPixelSize(12);
    bodyLabel->setFont(font);
    bodyLabel->setWordWrap(true);

    contentLayout->addWidget(iconLabel);
    contentLayout->addWidget(bodyLabel);

    mainLayout->addWidget(titleLabel);
    mainLayout->addLayout(contentLayout);

    closeBtn = new QPushButton(backgroundLabel);
    closeBtn->setObjectName("notify-close-btn");
    closeBtn->setFixedSize(20, 20);
    closeBtn->setIcon(QIcon("://img/close.svg"));
    connect(closeBtn, &QPushButton::clicked, this, [this]{
        Q_EMIT disappeared();
    });

    this->setStyleSheet("#notify-background {"
                        "border: 1px solid #ccc;"
                        "background:white;"
                        "border-radius: 4px;"
                        "} "
                        "#notify-title{"
                        "font-weight: bold;"
                        "font-size: 12px;"
                        "}"
                        "#notify-close-btn{ "
                        "border: 0;"
                        "color: #999;"
                        "}");

}


void Notify::showGriant()
{
    this->show();

    titleLabel->setText(title);
    QPixmap tempPix = QPixmap(this->icon);
    tempPix = tempPix.scaled(QSize(40, 40), Qt::KeepAspectRatio);
    iconLabel->setPixmap(tempPix);

    backgroundLabel->setFixedSize(this->size());
    closeBtn->move(backgroundLabel->width() - closeBtn->width(), 0);

    bodyLabel->setText(this->body);
    bodyLabel->adjustSize();

    QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity", this);
    animation->setStartValue(0);
    animation->setEndValue(1);
    animation->setDuration(200);
    animation->start();

    connect(animation, &QPropertyAnimation::finished, this, [animation, this](){
        animation->deleteLater();
        if (closeMode == ECLOSEAUTO) {
            QTimer::singleShot(displayTime, this, [this](){
                this->hideGriant();
            });
        }
    });
}

void Notify::hideGriant()
{
    QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity", this);
    animation->setStartValue(this->windowOpacity());
    animation->setEndValue(0);
    animation->setDuration(200);
    animation->start();

    connect(animation, &QPropertyAnimation::finished, this, [animation, this](){
        this->hide();
        animation->deleteLater();
        Q_EMIT disappeared();
    });
}

void Notify::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton) {
        if(!url.isEmpty()){
            QDesktopServices::openUrl(url);
        }
        hideGriant();
    }
}

void Notify::setUrl(const QString &value)
{
    url = value;
}


void Notify::setBody(const QString &value)
{
    body = value;
}

void Notify::setTitle(const QString &value)
{
    title = value;
}

void Notify::setIcon(const QString &value)
{
    icon = value;
}
