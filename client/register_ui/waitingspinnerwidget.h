#pragma once

#include <QWidget>
#include <QTimer>
#include <QColor>

class WaitingSpinnerWidget : public QWidget {
    Q_OBJECT
public:
    WaitingSpinnerWidget(QWidget *parent = 0,
                         bool centerOnParent = true,
                         bool disableParentWhenSpinning = true);

    WaitingSpinnerWidget(Qt::WindowModality modality,
                         QWidget *parent = 0,
                         bool centerOnParent = true,
                         bool disableParentWhenSpinning = true);

public slots:
    void start();
    void stop();

public:
    void setColor(QColor color);
    void setRoundness(qreal roundness);//圆角
    void setMinimumTrailOpacity(qreal minimumTrailOpacity);//最小轨迹不透明度
    void setTrailFadePercentage(qreal trail);//轨迹褪色百分比
    void setRevolutionsPerSecond(qreal revolutionsPerSecond);//每秒钟转数
    void setNumberOfLines(int lines);//线条数量
    void setLineLength(int length);//线条长度
    void setLineWidth(int width);//线条宽度
    void setInnerRadius(int radius);//内圆半径

    QColor color();
    qreal roundness();
    qreal minimumTrailOpacity();
    qreal trailFadePercentage();
    qreal revolutionsPersSecond();
    int numberOfLines();
    int lineLength();
    int lineWidth();
    int innerRadius();

    bool isSpinning() const;

private slots:
    void rotate();

protected:
    void paintEvent(QPaintEvent *paintEvent);

private:
    static int lineCountDistanceFromPrimary(int current, int primary,
                                            int totalNrOfLines);
    static QColor currentLineColor(int distance, int totalNrOfLines,
                                   qreal trailFadePerc, qreal minOpacity,
                                   QColor color);

    void initialize();
    void updateSize();
    void updateTimer();
    void updatePosition();

private:
    QColor  _color;
    qreal   _roundness; // 0..100
    qreal   _minimumTrailOpacity;
    qreal   _trailFadePercentage;
    qreal   _revolutionsPerSecond;
    int     _numberOfLines;
    int     _lineLength;
    int     _lineWidth;
    int     _innerRadius;

private:
    WaitingSpinnerWidget(const WaitingSpinnerWidget&);
    WaitingSpinnerWidget& operator=(const WaitingSpinnerWidget&);

    QTimer *_timer;
    bool    _centerOnParent;
    bool    _disableParentWhenSpinning;
    int     _currentCounter;
    bool    _isSpinning;
};
