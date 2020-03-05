#include "mainwin.h"
#include "ui_mainwin.h"


#include <QDebug>
#include <QSerialPort>
#include <QSerialPortInfo>

MainWin::MainWin(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWin)
{
    ui->setupUi(this);
    gpsParser = new NMEA_UART::Parser();


    connect(ui->comboBoxBaudRate, SIGNAL(currentIndexChanged(int)) ,this, SLOT(checkCustomBaudRatePolicy(int)));
    ui->comboBoxBaudRate->addItem(QLatin1String("9600"), QSerialPort::Baud9600);
    ui->comboBoxBaudRate->addItem(QLatin1String("19200"), QSerialPort::Baud19200);
    ui->comboBoxBaudRate->addItem(QLatin1String("38400"), QSerialPort::Baud38400);
    ui->comboBoxBaudRate->addItem(QLatin1String("115200"), QSerialPort::Baud115200);
    ui->comboBoxBaudRate->addItem(QLatin1String("Custom"));
    ui->comboBoxBaudRate->setCurrentIndex(3);
    // fill data bits
    ui->comboBoxComDatabits->addItem(QLatin1String("5"), QSerialPort::Data5);
    ui->comboBoxComDatabits->addItem(QLatin1String("6"), QSerialPort::Data6);
    ui->comboBoxComDatabits->addItem(QLatin1String("7"), QSerialPort::Data7);
    ui->comboBoxComDatabits->addItem(QLatin1String("8"), QSerialPort::Data8);
    ui->comboBoxComDatabits->setCurrentIndex(3);
    // fill parity
    ui->comboBoxComParity->addItem(QLatin1String("None"), QSerialPort::NoParity);
    ui->comboBoxComParity->addItem(QLatin1String("Even"), QSerialPort::EvenParity);
    ui->comboBoxComParity->addItem(QLatin1String("Odd"), QSerialPort::OddParity);
    ui->comboBoxComParity->addItem(QLatin1String("Mark"), QSerialPort::MarkParity);
    ui->comboBoxComParity->addItem(QLatin1String("Space"), QSerialPort::SpaceParity);
    // fill stop bits
    ui->comboBoxComStopbits->addItem(QLatin1String("1"), QSerialPort::OneStop);
#ifdef Q_OS_WIN
    ui->comboBoxComStopbits->addItem(QLatin1String("1.5"), QSerialPort::OneAndHalfStop);
#endif
    ui->comboBoxComStopbits->addItem(QLatin1String("2"), QSerialPort::TwoStop);
    // fill flow control
    ui->comboBoxComFlowControl->addItem(QLatin1String("None"), QSerialPort::NoFlowControl);
    ui->comboBoxComFlowControl->addItem(QLatin1String("RTS/CTS"), QSerialPort::HardwareControl);
    ui->comboBoxComFlowControl->addItem(QLatin1String("XON/XOFF"), QSerialPort::SoftwareControl);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    QThread *thread_New = new QThread;//Создаем поток для порта платы
    ComPort *PortNew = new ComPort();//Создаем обьект по классу
    PortNew->moveToThread(thread_New);//помешаем класс  в поток
    PortNew->thisPort.moveToThread(thread_New);//Помещаем сам порт в поток

    connect(PortNew, SIGNAL(error_(QString)), this, SLOT(Print(QString)));//Лог ошибок

    connect(thread_New, SIGNAL(started()), PortNew, SLOT(process_Port()));//Переназначения метода run
    connect(PortNew, SIGNAL(finished_Port()), thread_New, SLOT(quit()));//Переназначение метода выход
    connect(thread_New, SIGNAL(finished()), PortNew, SLOT(deleteLater()));//Удалить к чертям поток
    connect(PortNew, SIGNAL(finished_Port()), thread_New, SLOT(deleteLater()));//Удалить к чертям поток

    connect(this,SIGNAL(savesettings(QString,int,int,int,int,int)),PortNew,SLOT(Write_Settings_Port(QString,int,int,int,int,int)));//Слот - ввод настроек!

    connect(ui->pushBtnComConnect, SIGNAL(clicked()),PortNew,SLOT(ConnectPort()));//по нажатию кнопки подключить порт
    connect(ui->pushBtnComDisconnect, SIGNAL(clicked()),PortNew,SLOT(DisconnectPort()));//по нажатию кнопки отключить порт

    connect(PortNew, SIGNAL(outPort(QByteArray)), this, SLOT(Print(QByteArray)));//вывод в текстовое поле считанных данных
    //    connect(this,SIGNAL(writeData(QByteArray)),PortNew,SLOT(WriteToPort(QByteArray)));//отправка в порт данных
    thread_New->start();
    on_pushBtnCOMSearch_clicked();
    ui->comboBoxComPorts->setCurrentIndex(1);
    on_pushBtnComSaveSett_clicked();


    ui->MainPlot->addGraph(); // blue line
    ui->MainPlot->graph(0)->setPen(QPen(QColor(40, 110, 255)));
    ui->MainPlot->addGraph(ui->MainPlot->xAxis, ui->MainPlot->yAxis2); // red line
    ui->MainPlot->graph(1)->setPen(QPen(QColor(255, 110, 40)));
ui->MainPlot->setInteraction(QCP::Interaction::iRangeZoom,true);
ui->MainPlot->setInteraction(QCP::Interaction::iRangeDrag,true);

ui->MainPlot->yAxis2->setVisible(true);
ui->MainPlot->yAxis2->setTickLabels(true);
//ui->MainPlot->graph(0)->addData({1,2,3},{2,3,5} );
//ui->MainPlot->graph(1)->addData({1,2,3},{1,2,3} );

    // make left and bottom axes transfer their ranges to right and top axes:
   // connect( ui->MainPlot->xAxis, SIGNAL(rangeChanged(QCPRange)),  ui->MainPlot->xAxis2, SLOT(setRange(QCPRange)));
   // connect( ui->MainPlot->yAxis, SIGNAL(rangeChanged(QCPRange)),  ui->MainPlot->yAxis2, SLOT(setRange(QCPRange)));

    // setup a timer that repeatedly calls MainWindow::realtimeDataSlot:


}

MainWin::~MainWin()
{
    delete ui;
}


void MainWin::on_pushBtnCOMSearch_clicked()
{
    ui->comboBoxComPorts->clear();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        ui->comboBoxComPorts->addItem(info.portName());
        qDebug() << "Name : " << info.portName();
        qDebug() << "Description : " << info.description();
        qDebug() << "Manufacturer: " << info.manufacturer();

    }
}

void MainWin::on_pushBtnComSaveSett_clicked()
{
    qDebug()<<"Save sett";
    savesettings(ui->comboBoxComPorts->currentText(), ui->comboBoxBaudRate->currentText().toInt(),ui->comboBoxComDatabits->currentText().toInt(),
                 ui->comboBoxComParity->currentText().toInt(), ui->comboBoxComStopbits->currentText().toInt(), ui->comboBoxComFlowControl->currentText().toInt());
}
//+++++++++++++[Процедура вывода данных в консоль]++++++++++++++++++++++++++++++++++++++++
void MainWin::Print(QByteArray data)
{
    foreach (char byte,  data)
    {
        uint package  = gpsParser->charParser(byte);

        if(package!=0){

            //         GPS_NULL = 0,
            //         GPS_PRMC = 1,
            //         GPS_NRMC,
            //         GPS_PGGA,
            //         GPS_NGGA,
            //         GPS_PGSV,
            //         GPS_LGSV,
            //         GPS_PVTG,
            //         GPS_NVTG,
            //         GPS_GPGSA
            QString packageType="";
            switch (package) {

            case 0:
                packageType="";
                break;
            case 1:
                packageType="GPS_PRMC";
                ui_GPS_NRMC_update();
                break;
            case 2:
                packageType="GPS_NRMC";
                ui_GPS_NRMC_update();
                break;
            case 3:
                packageType="GPS_PGGA";

                ui_GPS_NGGA_update();
                break;
            case 4:
                packageType="GPS_NGGA";
                ui_GPS_NGGA_update();
                break;
            case 5:
                packageType="GPS_PGSV";
                ui_GPS_PGSV_update();
                break;
            case 6:
                packageType="GPS_LGSV";
                ui_GPS_LGSV_update();
                break;
            case 7:
                packageType="GPS_PVTG";
                ui_GPS_NVTG_update();
                break;
            case 8:
                packageType="GPS_NVTG";
                ui_GPS_NVTG_update();
                break;
            case 9:
                packageType="GPS_GPGSA";
                ui_GPS_GNGSA_update();
                break;
            case 10:
                packageType="GPS_GNGSA";
                ui_GPS_GNGSA_update();
                break;
            default:
                packageType="";
                break;
            }
         //   qDebug()<<packageType;
        }
    }
    //  qDebug()<<data<<endl; // Вывод текста в консоль
}
void MainWin::checkCustomBaudRatePolicy(int idx)
{
    bool isCustomBaudRate = !ui->comboBoxBaudRate->itemData(idx).isValid();
    ui->comboBoxBaudRate->setEditable(isCustomBaudRate);
    if (isCustomBaudRate) {
        ui->comboBoxBaudRate->clearEditText();
    }
}


void MainWin::ui_GPS_NRMC_update(){

    float lon,lat;
    lon =  QString(gpsParser->SLongitude).toFloat()/100;
    lat =  QString(gpsParser->SLatitude).toFloat()/100;
    ui->lEdRMCLat->setText(QString::number(lat));
    ui->lEdRMCLon->setText(QString::number(lon));
    ui->lEdRMCSpeedKn->setText( gpsParser->SpeedKnot);
    ui->lEdRMCCourseOG->setText( gpsParser->CourseOverGround);
    QString time(gpsParser->UTCtime);
    ui->lEdRMCtime->setTime(QTime::fromString(time.left(6),"hhmmss"));//hhmmss.sss

    QDate date =  QDate::fromString(gpsParser->UTCdate,"ddMMyy");
    if (date.year()<1980) {
        date = date.addYears(100);
    }
    ui->datEditDate->setDate(date);//xxxxxx

    ui->lEdRMCstatus->setText( gpsParser->Status);
    ui->lEdRMCNS->setText( gpsParser->NS);
    ui->lEdRMCEW->setText( gpsParser->EW);
    ui->lEdRMCIndication->setText( gpsParser->ModeIndicator);
}

void MainWin::ui_GPS_NVTG_update(){
 ui->lEdVTG_SpeedKn->setText( gpsParser->SpeedKnot);
     ui->lEdVTG_SpeedKmh->setText(gpsParser->SpeedKmh );
          ui->lEdVTG_indicator->setText( gpsParser->ModeIndicator);
          ui->lEdVTG_CourseTrue->setText( gpsParser->CourseTrue);
          ui->lEdVTG_CourseMagnetic->setText( gpsParser->CourseMagnetic);
 ui->MainPlot->graph(1)->addData(QString(gpsParser->UTCtime).toDouble(),QString(gpsParser->SpeedKmh).toDouble() );
  ui->MainPlot->xAxis->setRange(QString(gpsParser->UTCtime).toDouble()-30,QString(gpsParser->UTCtime).toDouble());
   ui->MainPlot->yAxis2->rescale();
  ui->MainPlot->replot();
}
void MainWin::ui_GPS_NGGA_update(){
 ui->lEdGGA_NS->setText( gpsParser->NS);
  ui->lEdGGA_WE->setText(gpsParser->EW);
  float lon,lat;
  lon =  QString(gpsParser->SLongitude).toFloat()/100;
  lat =  QString(gpsParser->SLatitude).toFloat()/100;
  ui->lEdGGA_lat->setText(   QString::number(lat));
  ui->lEdGGA_lon->setText(  QString::number(lon));
  ui->lEdGGA_HDOP->setText(gpsParser->HDOP);
  ui->lEdGGA_time->setText(gpsParser->UTCtime);
 ui->lEdGGA_work->setText(gpsParser->QualityIndicator);
 ui->lEdGGA_satCount->setText(gpsParser->SatCount);
  ui->lEdGGA_AltitudeOfSea->setText(gpsParser->AltitudeMSL);
 ui->lEdGGA_AltitudeOfGeoid->setText(gpsParser->GeoidalSeparation);


   ui->MainPlot->graph(0)->addData(QString(gpsParser->UTCtime).toDouble(),QString(gpsParser->AltitudeMSL).toDouble() );
   ui->MainPlot->yAxis->rescale();
 //  ui->MainPlot->replot();
}

void MainWin::ui_GPS_PGSV_update(){
ui->lEdGSV_GPS_sat_count->setText(gpsParser->ViewSatellite);
}
void MainWin::ui_GPS_LGSV_update(){
ui->lEdGSV_glonas_sat_count->setText(gpsParser->ViewSatellite);
}
void MainWin::ui_GPS_GNGSA_update(){
    ui->lEdGSA_HDOP->setText(gpsParser->HDOP);
    ui->lEdGSA_PDOP->setText(gpsParser->PDOP);
    ui->lEdGSA_VDOP->setText(gpsParser->VDOP);
    ui->lEdGSA_Mode->setText(gpsParser->GSAMode);
    ui->lEdGSA_Type->setText(gpsParser->FixType);
}
