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

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
    qDebug() << "Name : " << info.portName();
    qDebug() << "Description : " << info.description();
    qDebug() << "Manufacturer: " << info.manufacturer();
    }

}

MainWin::~MainWin()
{
    delete ui;
}

