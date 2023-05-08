#ifndef CONNECT485_H
#define CONNECT485_H

#include <QDialog>
#include <QTimer>
#include <QMutex>
#include <QMutexLocker>
#include <QThread>
#include <QStringList>
#include <QList>
#include <QtSerialPort/QSerialPort>         // 提供访问串口的功能
#include <QtSerialPort/QSerialPortInfo>     // 提供系统中存在的串口信息
#include <QMessageBox>
#include <QDebug>


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
    void Send_Data(const QByteArray &data);
    void Delay_MSec(unsigned int msec);
    unsigned short crc16(const void *s, int n);
    static void port_close();
    static bool serial_open_flag;

signals:
    void dataReceived(const QByteArray &data);
    void setDisableUI(int cmd);

private slots:
    void on_btn_scan_clicked();
    void on_btn_open_clicked();
    void Read_Data();

private:
    Ui::connect485 *ui;
    QSerialPort *serial;
    QThread mThread;

};

#endif // CONNECT485_H
