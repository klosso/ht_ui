#ifndef HTMAINWINDOW_H
#define HTMAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QtSerialPort/QSerialPort>

namespace Ui {
class HTMainWindow;
}

class HTMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit HTMainWindow(QWidget *parent = 0);
    ~HTMainWindow();

public slots:
    void Connect();
    void getCfg();

private:
    Ui::HTMainWindow *ui;
    bool connected;
     QSerialPort serial;
};

#endif // HTMAINWINDOW_H
