#ifndef MAINWIN_H
#define MAINWIN_H

#include <QMainWindow>
#include "nmea_parser.h"
#include "comport.h"
#include <QThread>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWin; }
QT_END_NAMESPACE

class MainWin : public QMainWindow
{
    Q_OBJECT

public:
    MainWin(QWidget *parent = nullptr);
    ~MainWin();


    NMEA_UART::Parser *gpsParser;
signals:
    void savesettings(QString name, int baudrate, int DataBits, int Parity, int StopBits, int FlowControl);
    void writeData(QByteArray data);

private slots:
    void ui_GPS_NRMC_update();
    void ui_GPS_NVTG_update();
    void ui_GPS_NGGA_update();
    void ui_GPS_PGSV_update();
    void ui_GPS_LGSV_update();
    void ui_GPS_GNGSA_update();
    void on_pushBtnCOMSearch_clicked();
    void checkCustomBaudRatePolicy(int idx);
    void Print(QByteArray data);
    void on_pushBtnComSaveSett_clicked();

private:
    Ui::MainWin *ui;
};
#endif // MAINWIN_H
