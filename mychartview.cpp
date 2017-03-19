#include "mychartview.h"


MyChartView::MyChartView(QWidget *parent):
    QCustomPlot(parent)
{
    addGraph(); // blue line
    graph(0)->setName("Pan");
    graph(0)->setPen(QPen(QColor(40, 110, 255)));
    addGraph(); // red line
    graph(1)->setName("Tilt");
    graph(1)->setPen(QPen(QColor(255, 110, 40)));
    addGraph(); // red green
    graph(2)->setName("Roll");
    graph(2)->setPen(QPen(QColor(110, 255, 40)));

    legend->setVisible(true);
    legend->setBrush(QBrush(QColor(255,255,255,150)));
    axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft|Qt::AlignTop); // make legend align in top left corner or axis rect


    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    xAxis->setTicker(timeTicker);
    axisRect()->setupFullAxesBox();
    setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    //    yAxis->setRange(-1.2, 1.2);

    // make left and bottom axes transfer their ranges to right and top axes:
    connect(this->xAxis, SIGNAL(rangeChanged(QCPRange)), this->xAxis2, SLOT(setRange(QCPRange)));
    connect(this->yAxis, SIGNAL(rangeChanged(QCPRange)), this->yAxis2, SLOT(setRange(QCPRange)));

    // setup a timer that repeatedly calls MainWindow::realtimeDataSlot:
    connect(&dataTimer, SIGNAL(timeout()), this , SLOT(realtimeDataSlot()));
    //    dataTimer.start(0); // Interval 0 means to refresh as fast as possible

}

void MyChartView::addPoints( float _pan, float _tilt, float _roll)
{
    static QTime time(QTime::currentTime());
    double key = time.elapsed()/1000.0; // time elapsed since start of demo, in seconds
    graph(0)->addData(key, _pan);
    graph(1)->addData(key, _tilt);
    graph(2)->addData(key, _roll);
    // rescale value (vertical) axis to fit the current data:
    graph(0)->rescaleValueAxis();
    graph(1)->rescaleValueAxis(true);
    graph(2)->rescaleValueAxis(true);
    //    pan = _pan;
    //    tilt = _tilt;
    //    roll = _roll;
}

void MyChartView::clear()
{
    graph(0)->data()->clear();
    graph(1)->data()->clear();
    graph(2)->data()->clear();
}


void MyChartView::realtimeDataSlot()
{
    static QTime time(QTime::currentTime());
    double key = time.elapsed()/1000.0; // time elapsed since start of demo, in seconds
     xAxis->setRange(key, 8, Qt::AlignRight);
    replot();
}
