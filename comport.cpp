#include "comport.h"
#include <qdebug.h>
#include <QDebug>
ComPort::ComPort(QObject *parent) :
    QObject(parent)
{
}
ComPort::~ComPort()
{
    qDebug("By in Thread!");
    emit finished_Port();//Сигнал о завершении работы
}
void ComPort :: process_Port(){//Выполняется при старте класса
    qDebug("Hello World in Thread!");
    connect(&thisPort,SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(handleError(QSerialPort::SerialPortError))); // подключаем проверку ошибок порта
    connect(&thisPort, SIGNAL(readyRead()),this,SLOT(ReadInPort()));//подключаем   чтение с порта по сигналу readyRead()
}
void ComPort :: Write_Settings_Port(QString name, int baudrate,int DataBits,
                         int Parity,int StopBits, int FlowControl){//заносим параметры порта в структуру данных
    SettingsPort.name = name;
    SettingsPort.baudRate = (QSerialPort::BaudRate) baudrate;
    SettingsPort.dataBits = (QSerialPort::DataBits) DataBits;
    SettingsPort.parity = (QSerialPort::Parity) Parity;
    SettingsPort.stopBits = (QSerialPort::StopBits) StopBits;
    SettingsPort.flowControl = (QSerialPort::FlowControl) FlowControl;
}
void ComPort :: ConnectPort(void){//процедура подключения
    thisPort.setPortName(SettingsPort.name);
    if (thisPort.open(QIODevice::ReadWrite)) {
        if (thisPort.setBaudRate(SettingsPort.baudRate)
                && thisPort.setDataBits(SettingsPort.dataBits)//DataBits
                && thisPort.setParity(SettingsPort.parity)
                && thisPort.setStopBits(SettingsPort.stopBits)
                && thisPort.setFlowControl(SettingsPort.flowControl))
        {
            if (thisPort.isOpen()){
                error_((SettingsPort.name+ " >> Открыт!\r").toLocal8Bit());
            }
        } else {
            thisPort.close();
            error_(thisPort.errorString().toLocal8Bit());
          }
    } else {
        thisPort.close();
        error_(thisPort.errorString().toLocal8Bit());
    }
}
void ComPort::handleError(QSerialPort::SerialPortError error)//проверка ошибок при работе
{
    if ( (thisPort.isOpen()) && (error == QSerialPort::ResourceError)) {
        error_(thisPort.errorString().toLocal8Bit());
        DisconnectPort();
    }
}//
void  ComPort::DisconnectPort(){//Отключаем порт
    if(thisPort.isOpen()){
        thisPort.close();
        error_(SettingsPort.name.toLocal8Bit() + " >> Закрыт!\r");
    }
}
void ComPort :: WriteToPort(QByteArray data){//Запись данных в порт
    if(thisPort.isOpen()){
        thisPort.write(data);
    }
}
//
void ComPort :: ReadInPort(){//Чтение данных из порта
    QByteArray data;
    data.append(thisPort.readAll());
    outPort(data);
    //((QString)(adr.toInt())).toLatin1().toHex()
}
