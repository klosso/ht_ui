#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets/QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QSerialPort>


QT_CHARTS_USE_NAMESPACE
QT_USE_NAMESPACE

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void Connect();
    void saveSettings();
    void startPlot();
    void sendCommand();
    void updatePanVals(int);
    void updateTiltVals(int);
    void updateRollVals(int);
    void readSerial();
    void writeSerial(qint64);
    void calibrate();
    void clearPlot();
private slots:
    void processTimeout( const QString &s );
    void processError( const QString &s );
    void getHTSettings();

private:
    const int CENTER_VAL = 2100;

    void switchEnable(bool enable);
    void settingsProceed(const QString &);

    void getFirmvareVer();
    void updatePortList();

    Ui::MainWindow *ui;
    QSerialPort* serial;

    int currentWaitTimeout;
    bool connected;
    bool plotRunning;

};

#endif // MAINWINDOW_H
