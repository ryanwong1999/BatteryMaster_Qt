#include "connect485.h"
#include "ui_connect485.h"
#include <QStringList>
#include <QList>
#include <QtSerialPort/QSerialPort>         // 提供访问串口的功能
#include <QtSerialPort/QSerialPortInfo>     // 提供系统中存在的串口信息
#include <QMessageBox>
#include <QDebug>

bool connect485::serial_open_flag = false;
int max_devices = 1;

QSerialPort port;

connect485::connect485(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::connect485)
{
    ui->setupUi(this);
    this->setWindowTitle("选择设备");
    init_dialog();
    scan_serial();

    if(serial_open_flag)
    {
        ui->btn_open->setText(tr("关闭串口"));//改变按钮上的文字
        ui->comboBox_serial->setDisabled(true);//串口号下拉列表变灰
        ui->comboBox_baudrate->setDisabled(true);//波特率下接列表变灰
        ui->lineEdit_max->setDisabled(true);
        ui->comboBox_serial->setStyleSheet("background-color: #F2F2F2;");//按键颜色改变
        ui->comboBox_baudrate->setStyleSheet("background-color: #F2F2F2;");//按键颜色改变
        ui->lineEdit_max->setStyleSheet("background-color: #F2F2F2;");//按键颜色改变
        ui->lineEdit_max->setDisabled(true);
    }
    else
    {
        ui->lineEdit_max->setText(tr("8"));
    }
}

connect485::~connect485()
{
    delete ui;
}

void connect485::init_dialog()
{
    ui->comboBox_baudrate->addItem(tr("4800"));
    ui->comboBox_baudrate->addItem(tr("9600"));
    ui->comboBox_baudrate->addItem(tr("115200"));
    //设置波特率下拉菜单默认显示第三项
    ui->comboBox_baudrate->setCurrentIndex(1);
}

void connect485::scan_serial()
{
    ui->comboBox_serial->clear();
    QList<QSerialPortInfo> list  = QSerialPortInfo::availablePorts();
    foreach (const QSerialPortInfo &info, list)
    {
        ui->comboBox_serial->addItem(info.portName());
    }
}

// CRC16校验函数（Modbus算法）
bool checkCRC(QByteArray data)
{
    unsigned int length = data.length();
    unsigned int crc = 0xFFFF;

    for (unsigned int i = 0; i < length; i++) {
        crc ^= (unsigned char)data[i];

        for (int j = 0; j < 8; j++) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc = crc >> 1;
            }
        }
    }
    return (crc == 0);
}

void connect485::Send_Data(const QByteArray &data)
{
    time_t t = time(0);   // get time now
    struct tm * now = localtime( & t );
    char e [80];
    strftime(e,80,"%H:%M:%S",now);
    qDebug() << "send bbccdd" << e;
    qDebug() << "connect485::Send_Data: " << data.toHex() << " " << e;
    port.write(data);
}

//读取接收到的数据
void connect485::Read_Data()
{
    while (port.waitForReadyRead(10)) {
        // 接收数据
        QByteArray receiveData = port.readAll();
        // 校验数据
        if (checkCRC(receiveData)) {
            emit dataReceived(receiveData);
            qDebug() << "CRC ok:" << receiveData;
        } else {
            qDebug() << "CRC err:" << receiveData;
        }
    }
}

void connect485::port_close()
{
    port.close();
}

void connect485::sendSingnalFunc(int cmd)
{
    emit setDisableUI(cmd);
}

void connect485::sendMaxDevicesFunc(int num)
{
    emit setMaxDevices(num);
}

//延时毫秒
void connect485::Delay_MSec(unsigned int msec)
{
    QEventLoop loop;//定义一个新的事件循环
    QTimer::singleShot(msec, &loop, SLOT(quit()));//创建单次定时器，槽函数为事件循环的退出函数
    loop.exec();//事件循环开始执行，程序会卡在这里，直到定时时间到，本循环被退出
}

void connect485::on_btn_scan_clicked()
{
    scan_serial();
}

void connect485::on_btn_open_clicked()
{
    QString name = ui->btn_open->text();//获取按钮上的文字
    if(name == tr("打开串口"))
    {
        if(ui->lineEdit_max->text() == "")
        {
            QMessageBox::information(this, tr("提示"),
                tr("请输入级联数量"), QMessageBox::Ok);
            return;
        }
        port.setPortName(ui->comboBox_serial->currentText());//选择串口号
        if(!port.open(QIODevice::ReadWrite))//打开串口
        {
            QMessageBox::information(this, tr("提示"),
                tr("打开串口失败,请查看串口是否被占用"), QMessageBox::Ok);
            qDebug() << "打开串口失败";
            return;
        }
        ui->btn_open->setText(tr("关闭串口"));//改变按钮上的文字
        port.setBaudRate(ui->comboBox_baudrate->currentText().toInt());
        port.setDataBits(QSerialPort::Data8);//8位数据位
        port.setParity(QSerialPort::NoParity);//无检验
        port.setStopBits(QSerialPort::OneStop);//1位停止位
        port.setFlowControl(QSerialPort::NoFlowControl);//无硬件控制
        ui->comboBox_serial->setDisabled(true);//串口号下拉列表变灰
        ui->comboBox_baudrate->setDisabled(true);//波特率下接列表变灰
        ui->lineEdit_max->setDisabled(true);
        ui->comboBox_serial->setStyleSheet("background-color: #F2F2F2;");//按键颜色改变
        ui->comboBox_baudrate->setStyleSheet("background-color: #F2F2F2;");//按键颜色改变
        ui->lineEdit_max->setStyleSheet("background-color: #F2F2F2;");//按键颜色改变
        ui->lineEdit_max->setDisabled(true);
        serial_open_flag = true;
        sendSingnalFunc(1);
        int num = ui->lineEdit_max->text().toInt();
        if(num > 255) num =255;
        else if(num < 1) num = 1;
        sendMaxDevicesFunc(num);
        qDebug() << "打开串口成功" << serial_open_flag;
        //连接信号槽
        QObject::connect(&port, &QSerialPort::readyRead, this, &connect485::Read_Data);
    }
    else
    {
        port.close();//关闭串口
        ui->comboBox_serial->setEnabled(true);//串口号下拉列表变亮
        ui->comboBox_baudrate->setEnabled(true);//串口号下拉列表变亮
        ui->lineEdit_max->setEnabled(true);
        ui->comboBox_serial->setStyleSheet("background-color: #FFFFFF;");//按键颜色改变
        ui->comboBox_baudrate->setStyleSheet("background-color: #FFFFFF;");//按键颜色改变
        ui->lineEdit_max->setStyleSheet("background-color: #FFFFFF;");//按键颜色改变
        ui->lineEdit_max->setEnabled(true);
        ui->btn_open->setText(tr("打开串口"));
        serial_open_flag = false;
        sendSingnalFunc(0);
        qDebug() << "关闭串口";
    }
}
