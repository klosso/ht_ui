#ifndef MYCHARTVIEW_HPP
#define MYCHARTVIEW_HPP

#include "qcustomplot.h"

//QT_CHARTS_USE_NAMESPACE

class  MyChartView : public QCustomPlot
{
    Q_OBJECT
public:
    MyChartView(QWidget *parent = Q_NULLPTR);
    ~MyChartView(){}

    //    QLineSeries pan;
    //    QLineSeries tilt;
    //    QLineSeries roll;

    void addPoints( float _pan, float _tilt, float _roll);
    void clear();

    QTimer dataTimer;
private slots:
    void realtimeDataSlot();
private:

    float pan;
    float tilt;
    float roll;

};

#endif // MYCHARTVIEW_HPP
