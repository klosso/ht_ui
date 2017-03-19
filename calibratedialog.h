#ifndef CALIBRATEDIALOG_H
#define CALIBRATEDIALOG_H

#include <QDialog>
#include <QSerialPort>

namespace Ui {
class CalibrateDialog;
}

class CalibrateDialog : public QDialog
{
    Q_OBJECT

public:

    struct offsetT{
        float x;
        float y;
        float z;
    };

    using MinMaxT= std::pair<float, float> ;

    explicit CalibrateDialog(QSerialPort *_serial,QWidget *parent = 0);
    ~CalibrateDialog();

public slots:
    void nextAction();
    void readSerial();

private:
    void setSerial( QSerialPort*);
    void StoreAccelCal(const int x, const int y, const int z);
    void StoreMagCal(const int x, const int y, const int z);
    void caliGyroCmd();
    void startAccData();
    void stopAccData();
    void calcOffsets(offsetT accel, offsetT mag);
    float calcOffset(MinMaxT &inp, const float val);

    Ui::CalibrateDialog *ui;
    QSerialPort *serial;
    int state;

    offsetT magOffset;
    offsetT accOffset;
    MinMaxT minMaxAccX;
    MinMaxT minMaxAccY;
    MinMaxT minMaxAccZ;
    MinMaxT minMaxMagX;
    MinMaxT minMaxMagY;
    MinMaxT minMaxMagZ;

    void updateUiValues(offsetT accel, offsetT mag);
    void clearMinMaxOffsets();
};

#endif // CALIBRATEDIALOG_H
