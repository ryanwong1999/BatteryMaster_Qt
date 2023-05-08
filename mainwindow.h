#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QtCharts/QChartView>
#include <QTimer>
#include <QtCharts>
#include <QTranslator>
#include <QtCharts>
#include <QTranslator>
#include <QDebug>
#include <synchapi.h>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChartView>
#include <QtCharts/QSplineSeries>
#include <cstdlib>
#include <time.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <functional>
#include "connect485.h"
#include "JQChecksum.h"
#include <vector>

using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void ButtonSetColor();
    QByteArray crc16Hex(QString);
    QTranslator	m_qtTs;
    void Delay_MSec(unsigned int);
    void get_0C_Info(QByteArray data, int device_num);
    void initChart();
    void clearChart();
    int addline;
    float value_time=0;
    float range_V_max=0;
    float range_V_min=0;
    float range_A_max=0;
    float range_A_min=0;
    QString filename_data;
    int initCSV=0;
    int connectMulti=0;
    int connectState=0;
    int minute=0;
    int langState=0;
    char buffer[80];
    int cap=1;
    int over_temp = 100;
    int temp1 = 0, temp2 = 0;
    bool run_chart_flag = false;
    //降频标志位
    int Derate_Flag = 0;
    //保存用的
    QString BatteryType;
    QString BatteryV;
    QString BatteryAh;
    QString ChargeV;
    QString ChargeA;
    QString BatteryOmega;
    QString DischargeA;
    QString DischargeRate;
    QString ProtectT;
    QString StopA;

    QChart *chart;
    QValueAxis *axisX;
    QValueAxis *axisY;
    QValueAxis *axisY3;
    QLineSeries *series1 = new QLineSeries();
    QLineSeries *series2 = new QLineSeries();
    QLineSeries *series3 = new QLineSeries();
    QLineSeries *series4 = new QLineSeries();
    QLineSeries *series5 = new QLineSeries();
    QLineSeries *series6 = new QLineSeries();
    QLineSeries *series7 = new QLineSeries();
    QLineSeries *series8 = new QLineSeries();
    QLineSeries *series9 = new QLineSeries();
    QLineSeries *series10 = new QLineSeries();
    QLineSeries *series11 = new QLineSeries();
    QLineSeries *series12 = new QLineSeries();
    QLineSeries *series13 = new QLineSeries();
    QLineSeries *series14 = new QLineSeries();
    QLineSeries *series15 = new QLineSeries();
    QLineSeries *series16 = new QLineSeries();

private slots:
    void on_btnQuickCheck_clicked();
    void on_btnChargeOnce_clicked();
    void on_btnDischargeOnce_clicked();
    void on_btnMaintain_clicked();
    void on_btnPauseResume_clicked();
//    void on_btnBlueTooth_clicked();
    void on_btnConfirm_clicked();
//    void on_bleScanButton_clicked();
//    void on_connectButton_clicked();
//    void on_sendButton_clicked();
//    void on_servicesComboBox_currentIndexChanged(int index);
//    void on_backToHomePage_clicked();
    void DisableUI(int cmd);
    void GetStatus1();
    void GetStatus2();
    void GetStatus3();
    void changeChart();
//    void writeToCSV();
    void deRate();
//    void connectDevice();
    void on_boxBatteryType_currentIndexChanged(int index);
    void on_boxBatteryV_currentIndexChanged(int index);
//    void on_btnLanguage_clicked();
    void on_EditBatteryAh_textChanged(const QString &arg1);
    void on_btncheckText_clicked();
    void on_EditBatteryAh_textEdited(const QString &arg1);
    void on_boxBatteryV_editTextChanged(const QString &arg1);
    void on_EditDischargeRate_editingFinished();
    void on_conbluetooth_clicked();
    void on_EditDischargeA_textEdited(const QString &arg1);
    void on_EditProtectT_editingFinished();
    void on_EditBatteryOmega_editingFinished();
    void on_EditBatteryAh_editingFinished();
    void on_EditChargeA_editingFinished();
    void on_EditChargeV_editingFinished();
    void on_EditDischargeA_editingFinished();

private:
    Ui::MainWindow *ui;
    void dataReceived(QByteArray data);
    void getChart();
    QTimer *timer1;
    QTimer *timer2;
    QTimer *timer3;
    QTimer *timer4;
    QTimer *timer5;
    QTimer *timer6;
    connect485 m_connect485;
    QVector<float> vec_V;
    QVector<float> vec_A;
    QVector<float> vec_Time;
    int TesterType=0;
    int getDataState=0;
    int currentState;
    int batteryNum;
    int chartStart=0;
    int getvalue_0b=0;
    int setValue=0;
    float ChargeV_value=0;
    float setChargeV=0;
};
#endif // MAINWINDOW_H
