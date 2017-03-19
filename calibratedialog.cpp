#include "calibratedialog.h"
#include "ui_calibratedialog.h"


CalibrateDialog::CalibrateDialog(QSerialPort *_serial,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CalibrateDialog),
    serial(_serial),
    state(0)
{
    ui->setupUi(this);

    connect(ui->buttonBox,SIGNAL(accepted()),this, SLOT(nextAction()));
    connect(serial, SIGNAL(readyRead()), this, SLOT(readSerial()));
    StoreAccelCal(0, 0, 0);
    StoreMagCal(0, 0, 0);

}

CalibrateDialog::~CalibrateDialog()
{
    delete ui;
}

void CalibrateDialog::clearMinMaxOffsets()
{
    offsetT clearOff;
    MinMaxT clearMinMax;
    minMaxAccX = clearMinMax;
    minMaxAccY = clearMinMax;
    minMaxAccZ = clearMinMax;
    minMaxMagX = clearMinMax;
    minMaxMagY = clearMinMax;
    minMaxMagZ = clearMinMax;
    accOffset = clearOff;
    magOffset = clearOff;
}

void CalibrateDialog::nextAction()
{

    switch (state)
    {
    case 0:
        // Start mag and accel output from device for calibration. This call will
        // also stop any other stream data.
        startAccData();
        break;
    case 1:
        clearMinMaxOffsets();
        caliGyroCmd();
        break;
    case 5:
        // Store values
        stopAccData();
        StoreAccelCal(accOffset.x, accOffset.y, accOffset.z);
        StoreMagCal(magOffset.x, magOffset.y, magOffset.z);
        break;
    case 6:
        close();
        break;
    default:
        break;
    }
        ui->stackedWidget->setCurrentIndex(++state);
}

void CalibrateDialog::setSerial(QSerialPort *_serial)
{
    serial = _serial;
}

void CalibrateDialog::readSerial()
{
    const float DEF_OFFSET_READ = -3000;
    offsetT accel;
    offsetT mag;
    QString line = serial->readLine();
    QStringList values = line.split(",");
    if(values.size() == 6 )
    {
        accel.x =       values.at(0).toFloat() + DEF_OFFSET_READ;
        accel.y =       values.at(2).toFloat() + DEF_OFFSET_READ;
        accel.z =       values.at(1).toFloat() + DEF_OFFSET_READ;
        mag.x =         values.at(3).toFloat() + DEF_OFFSET_READ;
        mag.y =         values.at(4).toFloat() + DEF_OFFSET_READ;
        mag.z =         values.at(5).toFloat() + DEF_OFFSET_READ;
        calcOffsets( accel, mag );
        updateUiValues(accel, mag);
    }

}

void CalibrateDialog::updateUiValues(offsetT accel, offsetT mag)
{
    ui->labelAccX->setNum(accel.x);
    ui->labelAccY->setNum(accel.y);
    ui->labelAccZ->setNum(accel.z);

    ui->labelMagX->setNum(mag.x);
    ui->labelMagY->setNum(mag.y);
    ui->labelMagZ->setNum(mag.z);

    ui->labelOffX->setNum(accOffset.x);
    ui->labelOffY->setNum(accOffset.y);
    ui->labelOffZ->setNum(accOffset.z);

    ui->labelOffMagX->setNum(magOffset.x);
    ui->labelOffMagY->setNum(magOffset.y);
    ui->labelOffMagZ->setNum(magOffset.z);
}

void CalibrateDialog::calcOffsets(offsetT accel, offsetT mag)
{
    //acc X
    accOffset.x = -calcOffset(minMaxAccX, accel.x);
    accOffset.y = -calcOffset(minMaxAccY, accel.y);
    accOffset.z = -calcOffset(minMaxAccZ, accel.z);
    magOffset.x = calcOffset(minMaxMagX, mag.x);
    magOffset.y = calcOffset(minMaxMagY, mag.y);
    magOffset.z = calcOffset(minMaxMagZ, mag.z);
}

float CalibrateDialog::calcOffset(MinMaxT &inp, const float val)
{
    QList<float> list = {inp.first, inp.second, val};
    auto iter = std::minmax_element(list.begin(), list.end());
    inp.first = *iter.first;
    inp.second = *iter.second;
    return (inp.first + inp.second)/2;
}

void CalibrateDialog::StoreAccelCal(const int x, const int y, const int z)
{
    const int DEF_OFFSET=2000;
    if(serial)
        serial->write(QString("$%1,%2,%3ACC").arg(x+DEF_OFFSET).arg(y+DEF_OFFSET).arg(z+DEF_OFFSET).toLocal8Bit() );
}

void CalibrateDialog::StoreMagCal(const int x, const int y, const int z)
{
    const int DEF_OFFSET=2000;
    if(serial)
        serial->write(QString("$%1,%2,%3MAG").arg(x+DEF_OFFSET).arg(y+DEF_OFFSET).arg(z+DEF_OFFSET).toLocal8Bit() );
}
void CalibrateDialog::startAccData()
{
    if(serial)
        serial->write("$CMAS");
}

void CalibrateDialog::stopAccData()
{
    if(serial)
        serial->write("$CMAE");
}


void CalibrateDialog::caliGyroCmd()
{
    if(serial)
        serial->write("$CALG");
}
