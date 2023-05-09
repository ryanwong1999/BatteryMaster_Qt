#ifndef CONNECT485_H
#define CONNECT485_H

#include <QDialog>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTimer>
#include <QMutex>
#include <QMutexLocker>
#include <QThread>

//#include "batterywidget.h"

namespace Ui {
class connect485;
}

class connect485 : public QDialog
{
    Q_OBJECT

public:
    explicit connect485(QWidget *parent = nullptr);
    ~connect485();
    void scan_serial();
    void init_dialog();
    void sendSingnalFunc(int cmd);
    void sendMaxDevicesFunc(int num);
    void Send_Data(const QByteArray &data);
    unsigned short crc16(const void *s, int n);
    void Delay_MSec(unsigned int msec);
    static void port_close();
    static bool serial_open_flag;
    static int max_devices;

signals:
    void dataReceived(const QByteArray &data);
    void setDisableUI(int cmd);
    void setMaxDevices(int num);

private slots:
    void on_btn_scan_clicked();
    void on_btn_open_clicked();
    void Read_Data();

private:
    Ui::connect485 *ui;
    QSerialPort *serial;
    QThread mThread;
//    BatteryWidget m_battery_widget;

};

#endif // CONNECT485_H
