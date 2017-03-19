#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "calibratedialog.h"
#include <QtSerialPort/QSerialPortInfo>
#include <QThread>

QT_USE_NAMESPACE

void MainWindow::updatePortList()
{
    const auto infos = QSerialPortInfo::availablePorts();
    ui->comboBox->clear();
    for (const QSerialPortInfo &info : infos)
        ui->comboBox->addItem(info.portName());
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    serial(new QSerialPort(this)),

    currentWaitTimeout(3000),
    connected(false),
    plotRunning(false)

{
    ui->setupUi(this);

    ui->gridLayoutWidget->setEnabled(connected);
    ui->ChartView->setEnabled(connected);


    updatePortList();


    connect(ui->pushButtonCalibrate, SIGNAL(clicked(bool)), this, SLOT(calibrate()));
    connect(ui->pushButtonConnect, SIGNAL(clicked(bool)), this, SLOT(Connect()));
    connect(ui->pushButtonSaveSett, SIGNAL(clicked(bool)), this, SLOT(saveSettings()));
    connect(ui->pushButtonPlot, SIGNAL(clicked(bool)), this, SLOT(startPlot()));
    connect(ui->pushButtonSend_2, SIGNAL(clicked(bool)), this, SLOT(sendCommand()));
    connect(ui->lineEditCmd, SIGNAL(returnPressed()), this , SLOT(sendCommand()));

    connect(ui->spinBoxPanCen, SIGNAL(valueChanged(int)), this , SLOT(updatePanVals(int)));
    connect(ui->spinBoxTiltCen, SIGNAL(valueChanged(int)), this , SLOT(updateTiltVals(int)));
    connect(ui->spinBoxRollCen, SIGNAL(valueChanged(int)), this , SLOT(updateRollVals(int)));

    connect(ui->pushButtonClear, SIGNAL(clicked(bool)), this, SLOT(clearPlot()));



}

MainWindow::~MainWindow()
{
    if (serial->isOpen())
        serial->close();
    delete ui;
    delete serial;
}


void MainWindow::Connect()
{
    updatePortList();
    ui->pushButtonConnect->setEnabled(false);
    if(!connected)
    {
        if(serial->isOpen())
            serial->close();
        serial->setPortName(ui->comboBox->currentText());
        serial->setBaudRate(QSerialPort::Baud57600);

        if (serial->open(QIODevice::ReadWrite))
        {
            connected = true;
            connect(serial, SIGNAL(readyRead()),this ,SLOT(readSerial()));
//            QThread::sleep(3);
            QTimer::singleShot(2000,this, SLOT(getHTSettings()));
//            getHTSettings();
         getFirmvareVer();
        }
        else
            ui->statusBar->showMessage(tr("Can't open %1, error code %2")
                                       .arg(ui->comboBox->currentText()).arg(serial->error()), 2000);
    }else
    {
        disconnect(serial, SIGNAL(readyRead()),this ,SLOT(readSerial()));
        serial->close();
        ui->ChartView->dataTimer.stop();
        connected = false;
    }

    switchEnable(connected);
    ui->pushButtonConnect->setEnabled(true);
}

void MainWindow::processError( const QString &s )
{
    ui->statusBar->showMessage("Serial open error: "+s);
}

void MainWindow::processTimeout( const QString &s )
{
    ui->textBrowser->append("Timeot:"+s);
}

void MainWindow::readSerial()
{
    QString str(serial->readAll());
    QStringList list = str.split(",");
    if(list.size()==3)
        ui->ChartView->addPoints(
                    (list.at(2).toFloat() -180) /100
                    ,(list.at(0).toFloat() -90) /100
                    ,(list.at(1).toFloat() -90) /100
                    );
    //    else if (str.startsWith("$SET$"))
    //    {
    //        ui->textBrowser->append("Settings "+str);
    //        settingsProceed(str);
    //    }
    else
        ui->textBrowser->append(str);
}

void MainWindow::writeSerial(qint64 bytes)
{
    ui->textBrowser->append("Write "+ QString::number(bytes)+"bytes");
}

void MainWindow::getFirmvareVer()
{
    serial->write("$VERS");
}


void MainWindow::calibrate()
{

    CalibrateDialog cdialog(serial);
    disconnect(serial, SIGNAL(readyRead()),this ,SLOT(readSerial()));
    cdialog.exec();
    connect(serial, SIGNAL(readyRead()),this ,SLOT(readSerial()));
}
void MainWindow::clearPlot()
{
    getHTSettings();
    //    getFirmvareVer();
    ui->ChartView->clear();
}

void MainWindow::updatePanVals(int val)
{

    ui->spinBoxPanMin->setValue(ui->spinBoxPanMin->value()+(CENTER_VAL-val));
    ui->spinBoxPanMax->setValue(ui->spinBoxPanMax->value()-(CENTER_VAL-val));
}

void MainWindow::updateTiltVals(int val)
{

    ui->spinBoxTiltMin->setValue(ui->spinBoxTiltMin->value()+(CENTER_VAL-val));
    ui->spinBoxTiltMax->setValue(ui->spinBoxTiltMax->value()-(CENTER_VAL-val));
}

void MainWindow::updateRollVals(int val)
{

    ui->spinBoxRollMin->setValue(ui->spinBoxRollMin->value()+(CENTER_VAL-val));
    ui->spinBoxRollMax->setValue(ui->spinBoxRollMax->value()-(CENTER_VAL-val));
}

void MainWindow::switchEnable(bool enable)
{
    ui->pushButtonCalibrate->setEnabled(enable);
    ui->pushButtonConnect->setText(enable?tr("Dissconnect"):tr("Connect"));
    ui->pushButtonSaveSett->setEnabled(enable);
    ui->pushButtonPlot->setEnabled(enable);
    ui->comboBox->setEnabled(!enable);

    ui->pushButtonSend_2->setEnabled(enable);
    ui->textBrowser->setEnabled(enable);
    ui->lineEditCmd->setEnabled(enable);
    ui->gridGroupBoxText->setEnabled(enable);

    ui->groupBox_2->setEnabled(enable);
    ui->gridLayoutWidget->setEnabled(enable);

}


void MainWindow::sendCommand()
{
    if(connected)
        serial->write(ui->lineEditCmd->text().toLocal8Bit());
}


void MainWindow::getHTSettings()
{
    if(connected)
    {
        ui->textBrowser->append("Bam");
        serial->blockSignals(true);
        serial->write("$GSET");
        if (serial->waitForBytesWritten(500))
        {
            // read response
            if (serial->waitForReadyRead(500))
            {
                QByteArray responseData = serial->readAll();
                while (serial->waitForReadyRead(10))
                    responseData += serial->readAll();

                QString response(responseData);

                settingsProceed(response);
            } else
            {
                ui->statusBar->showMessage(tr("Wait read response timeout %1")
                                           .arg(QTime::currentTime().toString()),5000);
            }
        }else
        {
            ui->statusBar->showMessage(tr("Wait read response timeout %1")
                                       .arg(QTime::currentTime().toString()),5000);
        }
        serial->blockSignals(false);
    }


}


void MainWindow::settingsProceed(const QString &str)
{
    if (str.startsWith("$SET$"))
    {
        QString inp = str;
        const int PAN_REV_BITMAST = 0x4;
        const int TILT_REV_BITMAST = 0x1;
        const int ROLL_REV_BITMAST = 0x2;

        inp.remove(0,5);
        auto line = inp.split("\n");
        auto items = line.at(0).split(",");
        if(items.size()<20)
        {
            ui->statusBar->showMessage("wrong settings "+ QString::number( items.size()),3000);
            ui->textBrowser->append("Wrong string received");
            return;
        }
        ui->spinBoxTiltRollBeta->setValue((int)(items.at(0).toFloat()));
        ui->spinBoxPanBeta->setValue((int)(items.at(1).toFloat()));
        ui->spinBoxTiltRollWeight->setValue((int)(items.at(2).toFloat()));
        ui->spinBoxPanWeight->setValue((int)(items.at(3).toFloat()));
        //Gain
        ui->spinBoxTiltGain->setValue((int)(items.at(4).toFloat()));
        ui->spinBoxPanGain->setValue((int)(items.at(5).toFloat()));
        ui->spinBoxRollGain->setValue((int)(items.at(6).toFloat()));

        //Reverse
        int mask = items.at(7).toInt();
        ui->checkBoxPanRev->setChecked(mask & PAN_REV_BITMAST );
        ui->checkBoxTiltRev->setChecked(mask & TILT_REV_BITMAST );
        ui->checkBoxRollRev->setChecked(mask & ROLL_REV_BITMAST );

        //Pan
        ui->spinBoxPanCen->setValue((int)(items.at(8).toFloat()));
        ui->spinBoxPanMin->setValue((int)(items.at(9).toFloat()));
        ui->spinBoxPanMax->setValue((int)(items.at(10).toFloat()));

        //Tilt
        ui->spinBoxTiltCen->setValue((int)(items.at(11).toFloat()));
        ui->spinBoxTiltMin->setValue((int)(items.at(12).toFloat()));
        ui->spinBoxTiltMax->setValue((int)(items.at(13).toFloat()));

        //Roll
        ui->spinBoxRollCen->setValue((int)(items.at(14).toFloat()));
        ui->spinBoxRollMin->setValue((int)(items.at(15).toFloat()));
        ui->spinBoxRollMax->setValue((int)(items.at(16).toFloat()));

        //Chanels
        ui->spinBoxPanCh->setValue((int)(items.at(17).toFloat()));
        ui->spinBoxTiltCh->setValue((int)(items.at(18).toFloat()));
        ui->spinBoxRollCh->setValue((int)(items.at(19).toFloat()));
    }

}

void MainWindow::saveSettings()
{
    if( connected)
    {
        int mask = (ui->checkBoxPanRev->isChecked() <<2) |
                (ui->checkBoxTiltRev->isChecked()) |
                (ui->checkBoxRollRev->isChecked()<<1);

        QString data = QString("$%1,%2,%3,%4,%5,%6,%7,%8,%9,%10,%11,%12,%13,%14,%15,%16,%17,%18,%19,%20HE")
                .arg(ui->spinBoxTiltRollBeta->value())
                .arg(ui->spinBoxPanBeta->value())
                .arg(ui->spinBoxTiltRollWeight->value())
                .arg(ui->spinBoxPanWeight->value())

                .arg(ui->spinBoxTiltGain->value())
                .arg(ui->spinBoxPanGain->value())
                .arg(ui->spinBoxRollGain->value())
                .arg(mask)
                .arg(ui->spinBoxPanCen->value())
                .arg(ui->spinBoxPanMin->value())
                .arg(ui->spinBoxPanMax->value())

                .arg(ui->spinBoxTiltCen->value())
                .arg(ui->spinBoxTiltMin->value())
                .arg(ui->spinBoxTiltMax->value())

                .arg(ui->spinBoxRollCen->value())
                .arg(ui->spinBoxRollMin->value())
                .arg(ui->spinBoxRollMax->value())
                //Chanels
                .arg(ui->spinBoxPanCh->value())
                .arg(ui->spinBoxTiltCh->value())
                .arg(ui->spinBoxRollCh->value());

        serial->write(data.toLocal8Bit());
        ui->textBrowser->append(data);

    }
}
void MainWindow::startPlot()
{
    if( connected)
    {
        if(!plotRunning)
        {
            serial->write("$PLST");
            ui->ChartView->dataTimer.start(10);
        }
        else
        {
            serial->write("$PLEN");
            ui->ChartView->dataTimer.stop();
        }
        plotRunning = !plotRunning;
        ui->pushButtonPlot->setText(plotRunning?"Stop Plot":"Start Plot");

    }
}
