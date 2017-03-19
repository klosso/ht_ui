#include "htmainwindow.h"
#include "ui_htmainwindow.h"

#include <QSerialPortInfo>

#include <QTime>

QT_USE_NAMESPACE

HTMainWindow::HTMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HTMainWindow),
    connected(false)
{
    ui->setupUi(this);


    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos)
        ui->comboBox->addItem(info.portName());

    connect( ui->pushButton, SIGNAL(clicked(bool)),this, SLOT(Connect()));
    connect(ui->getConfig, SIGNAL(clicked(bool)), this, SLOT(getCfg()));
}


HTMainWindow::~HTMainWindow()
{
    delete ui;
}


void HTMainWindow::Connect()
{
    if(connected)
    {
        serial.close();
        ui->pushButton->setText("Connect");

        connected = false;
    }
    else
    {
        ui->pushButton->setText("Dissconnect");
        serial.close();
        serial.setPortName(ui->comboBox->currentText());
        serial.setBaudRate(QSerialPort::Baud57600);

        if (serial.open(QIODevice::ReadWrite))
            connected = true;
        else
            ui->statusbar->showMessage(tr("Can't open %1, error code %2")
                       .arg(ui->comboBox->currentText()).arg(serial.error()), 1000);
    }

}

void HTMainWindow::getCfg()
{
    if(connected)
    {
    serial.write("$GSET");
    if (serial.waitForBytesWritten(200))
    {
        // read response
        if (serial.waitForReadyRead(200)) {
            QByteArray responseData = serial.readAll();
            while (serial.waitForReadyRead(10))
                responseData += serial.readAll();

            QString response(responseData);
            ui->textBrowser->append(response);
        } else
        {
            ui->statusbar->showMessage(tr("Wait read response timeout %1")
                         .arg(QTime::currentTime().toString()));
        }
        //! [9] //! [11]
        //!
     }
    }
}
