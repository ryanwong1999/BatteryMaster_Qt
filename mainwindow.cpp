#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStringList>

QStringList myList;
QByteArray OnOff = "";
bool btnPauseResume = false;
int shutdown_Dialog=0;
int i_0a = 0, i_0c = 0;
int fill_cnt = 0;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //默认选择485模式
    ui->radioBtn_485->setChecked(true);
    //485连接成功后禁用combobox ui，确保不会蓝牙和485同时连接
    connect(&m_connect485, &connect485::setDisableUI, this, &MainWindow::DisableUI);
    connect(&m_connect485, &connect485::dataReceived, this, &MainWindow::dataReceived);
    //初始化曲线图
    initChart();
    //初始化定时器
    timer1 = new QTimer(this);
    timer2 = new QTimer(this);
    timer3 = new QTimer(this);
    timer4 = new QTimer(this);
    timer5 = new QTimer(this);
    timer6 = new QTimer(this);
    //连接定时器
    connect(timer1,SIGNAL(timeout()),this,SLOT(GetStatus1()));
    connect(timer2,SIGNAL(timeout()),this,SLOT(GetStatus2()));
    connect(timer3,SIGNAL(timeout()),this,SLOT(GetStatus3()));
    connect(timer4,SIGNAL(timeout()),this,SLOT(changeChart()));
    connect(timer5,SIGNAL(timeout()),this,SLOT(writeToCSV()));
    connect(timer6,SIGNAL(timeout()),this,SLOT(deRate()));
    //开始倒计时
    timer1->start(900);
    timer2->start(500);
    timer3->start(1100);

    //给表格先填充0
    for(int i = 0; i < 8; i++)
    {
        for(int j = 0; j < 8; j++)
        {
            ui->infoTable->setItem(i,j,new QTableWidgetItem("0"));
        }
    }

    OnOff = crc16Hex("000301");
}

//析构
MainWindow::~MainWindow()
{
    //暂停
    OnOff = crc16Hex("000300");
    m_connect485.Send_Data(OnOff);
    Delay_MSec(100);
    //返回主界面
    QByteArray gohome = crc16Hex("00010100");
    m_connect485.Send_Data(gohome);
    Delay_MSec(100);
    m_connect485.Send_Data(gohome);
    Delay_MSec(100);
    //关闭串口
    connect485::port_close();
    delete ui;
    delete timer1;
    delete timer2;
    delete timer3;
    delete timer4;
    delete timer5;
    delete timer6;
    connect485::port_close();
}

//定时器结束1
void MainWindow::GetStatus1()
{
    if(ui->radioBtn_blue->isChecked() == true)
    {

    }
    else if(ui->radioBtn_485->isChecked() == true)
    {
        if(i_0a == 0)
        {
            QByteArray read_0a = crc16Hex("000a");
            m_connect485.Send_Data(read_0a);
            i_0a = 1;
        }
        else if(i_0a == 1 && currentState != 1)
        {
            m_connect485.Send_Data(OnOff);
            i_0a = 0;
            //显示暂停状态
            if(!btnPauseResume)
            {
                ui->btnPauseResume->setStyleSheet("background-color: #00C5CD;"
                                                  "color: #FFFFFF;");
                if(TesterType == 5 || TesterType == 6)
                {
                    if(currentState == 1) ui->labelCurrentState->setText(tr("检测"));
                    else if(currentState == 2) ui->labelCurrentState->setText(tr("充电"));
                    else if(currentState == 3) ui->labelCurrentState->setText(tr("放电"));
                    else if(currentState == 4) ui->labelCurrentState->setText(tr("维护"));
                }
            }
            else
            {
                ui->btnPauseResume->setStyleSheet("background-color: #DD2222;"
                                                  "color: #FFFFFF;");
                if(TesterType == 5 || TesterType == 6)
                    ui->labelCurrentState->setText(tr("暂停"));
            }
        }
    }
}

//定时器结束2
void MainWindow::GetStatus2()
{
    if(ui->radioBtn_blue->isChecked() == true){

    }else if(ui->radioBtn_485->isChecked() == true)
    {
        if(i_0c == 0)
        {
            QByteArray read_0c = crc16Hex("010c");
            m_connect485.Send_Data(read_0c);
            i_0c = 1;
        }
        else if(i_0c == 1)
        {
            QByteArray read_0c = crc16Hex("020c");
            m_connect485.Send_Data(read_0c);
            i_0c = 2;
        }
        else if(i_0c == 2)
        {
            QByteArray read_0c = crc16Hex("030c");
            m_connect485.Send_Data(read_0c);
            i_0c = 3;
        }
        else if(i_0c == 3)
        {
            QByteArray read_0c = crc16Hex("040c");
            m_connect485.Send_Data(read_0c);
            i_0c = 4;
        }
        else if(i_0c == 4)
        {
            QByteArray read_0c = crc16Hex("050c");
            m_connect485.Send_Data(read_0c);
            i_0c = 5;
        }
        else if(i_0c == 5)
        {
            QByteArray read_0c = crc16Hex("060c");
            m_connect485.Send_Data(read_0c);
            i_0c = 6;
        }
        else if(i_0c == 6)
        {
            QByteArray read_0c = crc16Hex("070c");
            m_connect485.Send_Data(read_0c);
            i_0c = 7;
        }
        else if(i_0c == 7)
        {
            QByteArray read_0c = crc16Hex("080c");
            m_connect485.Send_Data(read_0c);
            i_0c = 0;
        }
    }
}

//定时器结束3
void MainWindow::GetStatus3()
{
    if(ui->radioBtn_blue->isChecked() == true)
    {

    }
    else if(ui->radioBtn_485->isChecked() == true)
    {
        //禁用主控
        QByteArray disableMainCtrl = crc16Hex("bbccdd");
        m_connect485.Send_Data(disableMainCtrl);
    }
}

//降频处理
void MainWindow::deRate()
{
    bool bStatus = false;
    qDebug() << "过温降频!!!!!!!!!!!";
    if((TesterType == 5 || TesterType == 6) && (temp1 >= over_temp || temp2 >= over_temp) && Derate_Flag != 1)
    {
        float CA_1 = ChargeA.toInt(&bStatus,16) /10;
        float SA = StopA.toInt(&bStatus,16) / 10;
        qDebug() <<"降频处理deRate()"<< "CA_1 =" << CA_1 << "SA =" << SA;
        if(CA_1 >= SA * 2)
        {
            CA_1 *= 0.9;//之后每次降10%
            int CA = CA_1 * 10;
            ChargeA = QString("%1").arg(CA, 4, 16, QLatin1Char('0'));

            QString dataString = "0002" + BatteryType + BatteryV + BatteryAh + ChargeV +
                    ChargeA + BatteryOmega + DischargeA + DischargeRate + ProtectT + StopA;
            qDebug() <<"过温降频总字符"<< dataString;
            //将设置发送到单片机
            QByteArray sendData = crc16Hex(dataString);
            m_connect485.Send_Data(sendData);
            Delay_MSec(100);
            m_connect485.Send_Data(sendData);
            Delay_MSec(100);
            m_connect485.Send_Data(sendData);
            //更新左边显示
            QByteArray read_0b = crc16Hex("000b");
            m_connect485.Send_Data(read_0b);
        }
    }
}

//清空图表
void MainWindow::clearChart()
{
    //清空坐标轴
    value_time = 0;
    range_V_max = 0;
    range_V_min = 0;
    range_A_max = 0;
    range_A_min = 0;
    //清空曲线
    series1->clear();
    series2->clear();
    if (connectMulti == 1)
    {
        series3->clear();
        series4->clear();
        series5->clear();
        series6->clear();
        series7->clear();
        series8->clear();
        series9->clear();
        series10->clear();
        series11->clear();
        series12->clear();
        series13->clear();
        series14->clear();
        series15->clear();
        series16->clear();
    }
    //开始倒计时
    timer4->start(5000);
}

//初始化曲线图
void MainWindow::initChart()
{
    QColor color1(0, 0, 0);
    QColor colorY(0, 0, 0);
    QColor color3(0, 0, 0);
    QColor colorBackground(242, 241, 245);

    QLineSeries *series = new QLineSeries();
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->legend()->hide();
    chart->setBackgroundVisible(false);
    //横坐标
    QValueAxis *axisX = new QValueAxis;
    axisX->setTitleText("Time/Minute");
    axisX->setTitleBrush(QBrush(Qt::AlignRight));
    QFont labelsFont;
    labelsFont.setPixelSize(18);
    axisX->setTitleFont(labelsFont);
    axisX->setTitleBrush(QBrush(Qt::black));
    axisX->setLabelsColor(color1);
    axisX->setLabelFormat("%.1f");
    axisX->setRange(0, 10);
    axisX->setTickCount(9);
    axisX->setGridLineColor(QColor(62,62,62));
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    //纵坐标1
    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("U/V");
    axisY->setTitleFont(labelsFont);
    axisY->setLabelsColor(colorY);
    axisY->setTitleBrush(QBrush(Qt::black));
    axisY->setTickCount(6);
    axisY->setRange(0, 10);
    axisY->setGridLineColor(QColor(62,62,62));
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
    //纵坐标2
    QValueAxis *axisY3 = new QValueAxis();
    axisY3->setTitleText("I/A");
    axisY3->setTitleFont(labelsFont);
    axisY3->setLabelsColor(color3);
    axisY3->setTitleBrush(QBrush(Qt::black));
    axisY3->setTickCount(6);
    axisY3->setRange(0, 10);
    axisY3->setGridLineColor(QColor(62,62,62));
    chart->addAxis(axisY3, Qt::AlignRight);
    series->attachAxis(axisY3);

    QChartView *ChartView = new QChartView(chart);
    ChartView->setRenderHint(QPainter::Antialiasing);
    ChartView->setBackgroundBrush(QBrush(colorBackground));
    chart->layout()->setContentsMargins(0, 0, 0, 0);//设置外边界全部为0
    chart->setMargins(QMargins(0, 0, 0, 0));//设置内边界全部为0
    chart->setBackgroundRoundness(0); //设置背景区域无圆角
    chart->setBackgroundBrush(QBrush(QColor(23,23,23)));
    ui->LayoutChart->addWidget(ChartView);
}

//绘制曲线图
void MainWindow::changeChart()
{
    float v1=0,v2=0, v3=0, v4=0, v5=0, v6=0, v7=0, v8=0,
            a1=0, a2=0, a3=0, a4=0, a5=0, a6=0, a7=0, a8=0;
    QColor colorm(0, 0, 0);
    QColor color1(0, 0, 0);
    QColor color2(0, 0, 0);
    QColor color3(0, 0, 0);
    QColor colorBackground(242, 241, 245);

    if(!run_chart_flag) return;
    QLayoutItem *child;
    while ((child = ui->LayoutChart->takeAt(0)) != 0)
    {
        //setParent为NULL，防止删除之后界面不消失
        if(child->widget())
            child->widget()->setParent(NULL);
        delete child;
    }
    //从表格中获取数据
    v1 = ui->infoTable->item(0,1)->text().toFloat();
    a1 = ui->infoTable->item(0,2)->text().toFloat();
    if (connectMulti == 1)
    {
        v2 = ui->infoTable->item(1,1)->text().toFloat();
        a2 = ui->infoTable->item(1,2)->text().toFloat();
        v3 = ui->infoTable->item(2,1)->text().toFloat();
        a3 = ui->infoTable->item(2,2)->text().toFloat();
        v4 = ui->infoTable->item(3,1)->text().toFloat();
        a4 = ui->infoTable->item(3,2)->text().toFloat();
        v5 = ui->infoTable->item(4,1)->text().toFloat();
        a5 = ui->infoTable->item(4,2)->text().toFloat();
        v6 = ui->infoTable->item(5,1)->text().toFloat();
        a6 = ui->infoTable->item(5,2)->text().toFloat();
        v7 = ui->infoTable->item(6,1)->text().toFloat();
        a7 = ui->infoTable->item(6,2)->text().toFloat();
        v8 = ui->infoTable->item(7,1)->text().toFloat();
        a8 = ui->infoTable->item(7,2)->text().toFloat();
    }
    //轴根据最大最小值改变范围
    if(range_V_max == 0 && range_V_min == 0)
    {
        range_V_max = v1;
        range_V_min = v1;
    }
    if(range_A_max == 0 && range_A_min == 0)
    {
        range_A_max = a1;
        range_A_min = a1;
    }
    if(v1 > range_V_max) range_V_max = v1;
    if(v1 < range_V_min) range_V_min = v1;
    if(a1 > range_A_max) range_A_max = a1;
    if(a1 < range_A_min) range_A_min = a1;

    if (connectMulti == 1)
    {
        float arr_v[] = {v2, v3, v4, v5, v6, v7, v8};
        float arr_a[] = {a2, a3, a4, a5, a6, a7, a8};
        for(int i = 0; i < 6; i++)
        {
            if(arr_v[i] > range_V_max) range_V_max = arr_v[i];
            if(arr_a[i] > range_A_max) range_A_max = arr_a[i];
            if(arr_v[i] < range_V_min) range_V_min = arr_v[i];
            if(arr_a[i] < range_A_min) range_A_min = arr_a[i];
        }
    }
    //每一路都有电压电流两条曲线
    series1->append(value_time,v1); series1->setName("V1");
    series2->append(value_time,a1); series2->setName("A1");
    series1->setColor(QColor(255,0,0));
    series2->setColor(QColor(255,102,0));
    if (connectMulti == 1)
    {
        series3->append(value_time,v2); series3->setName("V2");
        series4->append(value_time,a2); series4->setName("A2");
        series5->append(value_time,v3); series5->setName("V3");
        series6->append(value_time,a3); series6->setName("A3");
        series7->append(value_time,v4); series7->setName("V4");
        series8->append(value_time,a4); series8->setName("A4");
        series9->append(value_time,v5); series9->setName("V5");
        series10->append(value_time,a5); series10->setName("A5");
        series11->append(value_time,v6); series11->setName("V6");
        series12->append(value_time,a6); series12->setName("A6");
        series13->append(value_time,v7); series13->setName("V7");
        series14->append(value_time,a7); series14->setName("A7");
        series15->append(value_time,v8); series15->setName("V8");
        series16->append(value_time,a8); series16->setName("A8");
        //每条曲线的颜色都要不一样
        series3->setColor(QColor(255,255,0));
        series4->setColor(QColor(51,255,0));
        series5->setColor(QColor(0,255,204));
        series6->setColor(QColor(0,153,205));
        series7->setColor(QColor(153,0,255));
        series8->setColor(QColor(255,0,204));
        series9->setColor(QColor(0,0,255));
        series10->setColor(QColor(176,48,96));
        series11->setColor(QColor(30,144,255));
        series12->setColor(QColor(255,192,203));
        series13->setColor(QColor(128,110,82));
        series14->setColor(QColor(255,69,0));
        series15->setColor(QColor(0,199,140));
        series16->setColor(QColor(255,0,255));
    }
    value_time = value_time + float(1/12.0);
    qDebug()<<"value_time "<<value_time;
    chart = new QChart();
    chart->addSeries(series1);
    chart->addSeries(series2);
    chart->legend()->setVisible(true);
    chart->setBackgroundVisible(false);//背景不可见
    chart->setContentsMargins(0, 0, 0, 0);//设置外边界全部为0
    chart->setMargins(QMargins(0, 0, 0, 0));//设置内边界全部为0
    if (connectMulti == 1)
    {
        chart->addSeries(series3);
        chart->addSeries(series4);
        chart->addSeries(series5);
        chart->addSeries(series6);
        chart->addSeries(series7);
        chart->addSeries(series8);
        chart->addSeries(series9);
        chart->addSeries(series10);
        chart->addSeries(series11);
        chart->addSeries(series12);
        chart->addSeries(series13);
        chart->addSeries(series14);
        chart->addSeries(series15);
        chart->addSeries(series16);
    }

    QLegendMarker *marker1 = chart->legend()->markers(series1).at(0);
    QBrush brush1 = marker1->labelBrush();
    brush1.setColor(colorm);
    marker1->setLabelBrush(brush1);

    QLegendMarker *marker2 = chart->legend()->markers(series2).at(0);
    QBrush brush2 = marker2->labelBrush();
    brush2.setColor(colorm);
    marker2->setLabelBrush(brush2);

    if(connectMulti == 1)
    {
        QLegendMarker *marker3 = chart->legend()->markers(series3).at(0);
        QBrush brush3 = marker3->labelBrush();
        brush3.setColor(colorm);
        marker3->setLabelBrush(brush3);

        QLegendMarker *marker4 = chart->legend()->markers(series4).at(0);
        QBrush brush4 = marker4->labelBrush();
        brush4.setColor(colorm);
        marker4->setLabelBrush(brush4);

        QLegendMarker *marker5 = chart->legend()->markers(series5).at(0);
        QBrush brush5 = marker5->labelBrush();
        brush5.setColor(colorm);
        marker5->setLabelBrush(brush5);

        QLegendMarker *marker6 = chart->legend()->markers(series6).at(0);
        QBrush brush6 = marker6->labelBrush();
        brush6.setColor(colorm);
        marker6->setLabelBrush(brush6);

        QLegendMarker *marker7 = chart->legend()->markers(series7).at(0);
        QBrush brush7 = marker7->labelBrush();
        brush7.setColor(colorm);
        marker7->setLabelBrush(brush7);

        QLegendMarker *marker8 = chart->legend()->markers(series8).at(0);
        QBrush brush8 = marker8->labelBrush();
        brush8.setColor(colorm);
        marker8->setLabelBrush(brush8);

        QLegendMarker *marker9 = chart->legend()->markers(series9).at(0);
        QBrush brush9 = marker9->labelBrush();
        brush9.setColor(colorm);
        marker9->setLabelBrush(brush9);

        QLegendMarker *marker10 = chart->legend()->markers(series10).at(0);
        QBrush brush10 = marker10->labelBrush();
        brush10.setColor(colorm);
        marker10->setLabelBrush(brush10);

        QLegendMarker *marker11 = chart->legend()->markers(series11).at(0);
        QBrush brush11 = marker11->labelBrush();
        brush11.setColor(colorm);
        marker11->setLabelBrush(brush11);

        QLegendMarker *marker12 = chart->legend()->markers(series12).at(0);
        QBrush brush12 = marker12->labelBrush();
        brush12.setColor(colorm);
        marker12->setLabelBrush(brush12);

        QLegendMarker *marker13 = chart->legend()->markers(series13).at(0);
        QBrush brush13 = marker13->labelBrush();
        brush13.setColor(colorm);
        marker13->setLabelBrush(brush13);

        QLegendMarker *marker14 = chart->legend()->markers(series14).at(0);
        QBrush brush14 = marker14->labelBrush();
        brush14.setColor(colorm);
        marker14->setLabelBrush(brush14);

        QLegendMarker *marker15 = chart->legend()->markers(series15).at(0);
        QBrush brush15 = marker15->labelBrush();
        brush15.setColor(colorm);
        marker15->setLabelBrush(brush15);

        QLegendMarker *marker16 = chart->legend()->markers(series16).at(0);
        QBrush brush16 = marker16->labelBrush();
        brush16.setColor(colorm);
        marker16->setLabelBrush(brush16);
    }

    axisX = new QValueAxis();
    //axisX->setTitleText("Time/Minute");
    QFont labelsFont;
    labelsFont.setPixelSize(18);
    axisX->setTitleFont(labelsFont);
    axisX->setTitleBrush(QBrush(Qt::black));
    axisX->setLabelsColor(color1);
    axisX->setTickCount(9);

    if (value_time < 8) axisX->setLabelFormat("%.1f");
    else axisX->setLabelFormat("%d");

    axisX->setRange(0, value_time);
    axisX->setGridLineColor(QColor(62,62,62));
    series1->attachAxis(axisX);
    series2->attachAxis(axisX);
    chart->addAxis(axisX, Qt::AlignBottom);

    if (connectMulti == 1)
    {
        series3->attachAxis(axisX);
        series4->attachAxis(axisX);
        series5->attachAxis(axisX);
        series6->attachAxis(axisX);
        series7->attachAxis(axisX);
        series8->attachAxis(axisX);
        series9->attachAxis(axisX);
        series10->attachAxis(axisX);
        series11->attachAxis(axisX);
        series12->attachAxis(axisX);
        series13->attachAxis(axisX);
        series14->attachAxis(axisX);
        series15->attachAxis(axisX);
        series16->attachAxis(axisX);
    }

    axisY = new QValueAxis();
    axisY->setTitleText("U/V");
    axisY->setTitleFont(labelsFont);
    axisY->setTitleBrush(QBrush(Qt::black));
    axisY->setLabelsColor(color2);
    axisY->setTickCount(6);
    axisY->setRange(range_V_min*0.8,range_V_max*1.2);
    axisY->setGridLineColor(QColor(62,62,62));
    chart->addAxis(axisY, Qt::AlignLeft);

    series1->attachAxis(axisY);

    if (connectMulti == 1)
    {
        series3->attachAxis(axisY);
        series5->attachAxis(axisY);
        series7->attachAxis(axisY);
        series9->attachAxis(axisY);
        series11->attachAxis(axisY);
        series13->attachAxis(axisY);
        series15->attachAxis(axisY);
    }

    axisY3 = new QValueAxis();
    axisY3->setTitleText("I/A");
    axisY3->setTitleFont(labelsFont);
    axisY3->setLabelsColor(color3);
    axisY3->setTitleBrush(QBrush(Qt::black));
    axisY3->setTickCount(6);
    axisY3->setRange(range_A_min*0.7,range_A_max*1.8);
    axisY3->setGridLineColor(QColor(62,62,62));
    chart->addAxis(axisY3, Qt::AlignRight);

    series2->attachAxis(axisY3);
    if (connectMulti == 1)
    {
        series4->attachAxis(axisY3);
        series6->attachAxis(axisY3);
        series8->attachAxis(axisY3);
        series10->attachAxis(axisY3);
        series12->attachAxis(axisY3);
        series14->attachAxis(axisY3);
        series16->attachAxis(axisY3);
    }

    QChartView *ChartView = new QChartView(chart);
    ChartView->setRenderHint(QPainter::Antialiasing);
//    ChartView->setBackgroundBrush(QBrush(QColor("black")));
    ChartView->setBackgroundBrush(QBrush(colorBackground));
    chart->layout()->setContentsMargins(0, 0, 0, 0);//设置外边界全部为0
    chart->setMargins(QMargins(0, 0, 0, 0));//设置内边界全部为0
    chart->setBackgroundRoundness(0); //设置背景区域无圆角
    chart->setBackgroundBrush(QBrush(colorBackground));

    ui->LayoutChart->addWidget(ChartView);
}

//从蓝牙或485接收到数据
void MainWindow::dataReceived(QByteArray data)
{
    bool bStatus = false;
    qDebug() <<"接收到的数据dataReceived:" << data.toHex();
    if(ui->radioBtn_blue->isChecked() == true)  //蓝牙模式
    {

    }
    else if(ui->radioBtn_485->isChecked() == true)  //485模式
    {
        //读取设置参数
        if (data.toHex().mid(2,2) == "0a")
        {
            qDebug()<<"Get 0a";
            switch(data.toHex().mid(8,2).toInt(&bStatus,16)) {
                //TesterType = 1：BM70-5、 2：BM70-18、 3：BM200-5-D、 BM200-32-D
              case 01: ui->labelProductModel->setText("BM70-5"); TesterType=1; break;
              case 02: ui->labelProductModel->setText("BM70-18"); TesterType=2; break;
              case 03: ui->labelProductModel->setText("BM200-5-D");
                      ui->labelChDisV->setText(tr("最大 5.0V"));
                      ui->labelChDisA->setText(tr("最大 30.0A"));
                      ui->labelChargeRate->setText(tr("最大 150W"));
                      ui->labelDisChargeRate->setText(tr("最大 90W"));
                      ui->label_18->setText(tr("温度截至"));
                      ui->label_20->setText(tr("°C"));
                      ui->infoTable->setColumnHidden(7, false);
                      TesterType=3; break;
              case 04: ui->labelProductModel->setText("BM200-32-D");
                      ui->labelChDisV->setText(tr("最大 32.0V"));
                      ui->labelChDisA->setText(tr("最大 8.0A"));
                      ui->labelChargeRate->setText(tr("最大 200W"));
                      ui->labelDisChargeRate->setText(tr("最大 90W"));
                      ui->label_18->setText(tr("温度截至"));
                      ui->label_20->setText(tr("°C"));
                      ui->infoTable->setColumnHidden(7, false);
                      TesterType=4; break;
              case 05: ui->labelProductModel->setText("BM1200-05-D");
                      ui->labelChDisV->setText(tr("最大 5.0V"));
                      ui->labelChDisA->setText(tr("最大 8.0A"));
                      ui->labelChargeRate->setText(tr("最大 1200W"));
                      ui->labelDisChargeRate->setText(tr("最大 90W"));
                      ui->label_18->setText(tr("停止电流"));
                      ui->label_27->setText(tr("A"));
                      ui->infoTable->setColumnHidden(7, true);
                      connectMulti = 1;
                      TesterType=5; break;
              case 06: ui->labelProductModel->setText("BM1200-18-D");
                      ui->labelChDisV->setText(tr("最大 18.0V"));
                      ui->labelChDisA->setText(tr("最大 8.0A"));
                      ui->labelChargeRate->setText(tr("最大 1200W"));
                      ui->labelDisChargeRate->setText(tr("最大 90W"));
                      ui->label_18->setText(tr("停止电流"));
                      ui->label_20->setText(tr("A"));
                      ui->infoTable->setColumnHidden(7, true);
                      connectMulti = 1;
                      TesterType=6; break;
            }
            temp1 = data.toHex().mid(10,2).toInt(&bStatus,16);
            temp2 = data.toHex().mid(12,2).toInt(&bStatus,16);
            qDebug()<<"Temp 1 =" << temp1 << "  Temp 2 =" << temp2;
            //bm1200过温降频
            if((TesterType == 5 || TesterType == 6) && (temp1 >= over_temp || temp2 >= over_temp))
            {
                //判断降频标志位
                if(Derate_Flag == 0)
                {
                    QByteArray read_0b = crc16Hex("000b");
                    m_connect485.Send_Data(read_0b);
                    Delay_MSec(100);
                    m_connect485.Send_Data(read_0b);
                    Delay_MSec(100);
                    m_connect485.Send_Data(read_0b);
                    Delay_MSec(100);
                    m_connect485.Send_Data(read_0b);
                    Delay_MSec(200);

                    float CA_1 = ChargeA.toInt(&bStatus,16) /10;
                    qDebug() <<"降频前CA_1 ="<< CA_1 << "ChargeA =" << ChargeA;
                    CA_1 *= 0.7;//第一次降到70%
                    int CA = CA_1 * 10;
                    ChargeA = QString("%1").arg(CA, 4, 16, QLatin1Char('0'));
                    qDebug() <<"降频ChargeA ="<< ChargeA << " 降频后CA_1 =" << CA_1;
                    Derate_Flag += 1;

                    QString dataString = "0002" + BatteryType + BatteryV + BatteryAh + ChargeV +
                            ChargeA + BatteryOmega + DischargeA + DischargeRate  + ProtectT + StopA;
                    qDebug() <<"过温降频总字符"<< dataString;
                    //将设置发送到单片机
                    QByteArray sendData = crc16Hex(dataString);
                    m_connect485.Send_Data(sendData);
                    Delay_MSec(100);
                    m_connect485.Send_Data(sendData);
                    Delay_MSec(100);
                    m_connect485.Send_Data(sendData);
                    Delay_MSec(100);
                    //QByteArray read_0b = crc16Hex("000b");
                    m_connect485.Send_Data(read_0b);
                }
                //继续降频
                else if(Derate_Flag == 1)
                {
                    qDebug() <<"过温降频Derate_Flag";
                    if(Derate_Flag <= 2) timer6->start(60003);
                    Derate_Flag += 1;
                }
            }
        }
        else if (data.toHex().mid(2,2) == "0b")
        {
            qDebug()<<"Get 0b";
            //第几路设备
            int device_num = data.toHex().mid(0,2).toInt(&bStatus,16);
            if(device_num == 1)
            {
                ui->boxBatteryV->clear();
                float result = data.toHex().mid(6,4).toInt(&bStatus,16);
                BatteryV = data.toHex().mid(6,4);
                int BatteryType_b = data.toHex().mid(4,2).toInt(&bStatus,16);
                BatteryType = data.toHex().mid(4,2);
                //TesterType = 1：BM70-5、 2：BM70-18、 3：BM200-5-D、 4：BM200-32-D、 5：BM1200-5、 6：BM1200-32
                if(TesterType == 3)
                {
                    //根据不同电池类型
                    if(BatteryType_b == 1)
                    {
                        float BatteryV_b = result * 3.7;
                        ui->boxBatteryType->setCurrentIndex(1);
                        ui->boxBatteryV->addItem(QString::number(BatteryV_b));
                        ui->boxBatteryV->setCurrentIndex(0);
                        ui->boxBatteryV->addItems({"3.7"});
                    }
                    else if(BatteryType_b == 2)
                    {
                        float BatteryV_b = result * 3.2;
                        ui->boxBatteryType->setCurrentIndex(2);
                        ui->boxBatteryV->addItem(QString::number(BatteryV_b));
                        ui->boxBatteryV->setCurrentIndex(0);
                        ui->boxBatteryV->addItems({"3.2"});
                    }
                    else if(BatteryType_b == 3)
                    {
                        float BatteryV_b = result * 2.0;
                        ui->boxBatteryType->setCurrentIndex(3);
                        ui->boxBatteryV->addItem(QString::number(BatteryV_b));
                        ui->boxBatteryV->setCurrentIndex(0);
                        ui->boxBatteryV->addItems({"2.0","4.0"});
                    }
                    else if(BatteryType_b == 4)
                    {
                        float BatteryV_b = result * 1.2;
                        ui->boxBatteryType->setCurrentIndex(4);
                        ui->boxBatteryV->addItem(QString::number(BatteryV_b));
                        ui->boxBatteryV->setCurrentIndex(0);
                        ui->boxBatteryV->addItems({"1.2","2.4","3.6","4.8"});
                    }
                }
                else if(TesterType == 4)
                {
                    //根据不同电池类型
                    if(BatteryType_b == 1)
                    {
                        float BatteryV_b = result * 3.7;
                        ui->boxBatteryType->setCurrentIndex(1);
                        ui->boxBatteryV->addItem(QString::number(BatteryV_b));
                        ui->boxBatteryV->setCurrentIndex(0);
                        ui->boxBatteryV->addItems({"3.7","7.4","11.1","14.8","18.5","22.2","25.9"});
                    }
                    else if(BatteryType_b == 2)
                    {
                        float BatteryV_b = result * 3.2;
                        ui->boxBatteryType->setCurrentIndex(2);
                        ui->boxBatteryV->addItem(QString::number(BatteryV_b));
                        ui->boxBatteryV->setCurrentIndex(0);
                        ui->boxBatteryV->addItems({"3.2","6.4","9.6","12.8","16","19.2","22.4","25.6"});
                    }
                    else if(BatteryType_b == 3)
                    {
                        float BatteryV_b = result * 2.0;
                        ui->boxBatteryType->setCurrentIndex(3);
                        ui->boxBatteryV->addItem(QString::number(BatteryV_b));
                        ui->boxBatteryV->setCurrentIndex(0);
                        ui->boxBatteryV->addItems({"2.0","4.0","6.0","8.0","10.0","12.0","14.0","16.0","18.0","20.0","22.0","24.0"});
                    }
                    else if(BatteryType_b == 4)
                    {
                        float BatteryV_b = result * 1.2;
                        ui->boxBatteryType->setCurrentIndex(4);
                        ui->boxBatteryV->addItem(QString::number(BatteryV_b));
                        ui->boxBatteryV->setCurrentIndex(0);
                        ui->boxBatteryV->addItems({"1.2","2.4","3.6","4.8","6.0","7.2","8.4","9.6","10.8","12.0","13.2","14.4","15.6","16.8","18.0","19.2","20.4","21.6"});
                    }
                }
                else if(TesterType == 5)
                {
                    //根据不同电池类型
                    if(BatteryType_b == 1)
                    {
                        float BatteryV_b = result * 3.7;
                        ui->boxBatteryType->setCurrentIndex(1);
                        ui->boxBatteryV->addItem(QString::number(BatteryV_b));
                        ui->boxBatteryV->setCurrentIndex(0);
                        ui->boxBatteryV->addItems({"3.7"});
                    }
                    else if(BatteryType_b == 2)
                    {
                        float BatteryV_b = result * 3.2;
                        ui->boxBatteryType->setCurrentIndex(2);
                        ui->boxBatteryV->addItem(QString::number(BatteryV_b));
                        ui->boxBatteryV->setCurrentIndex(0);
                        ui->boxBatteryV->addItems({"3.2"});
                    }
                    else if(BatteryType_b == 3)
                    {
                        float BatteryV_b = result * 2.0;
                        ui->boxBatteryType->setCurrentIndex(3);
                        ui->boxBatteryV->addItem(QString::number(BatteryV_b));
                        ui->boxBatteryV->setCurrentIndex(0);
                        ui->boxBatteryV->addItems({"2.0","4.0"});
                    }
                    else if(BatteryType_b == 4)
                    {
                        float BatteryV_b = result * 1.2;
                        ui->boxBatteryType->setCurrentIndex(4);
                        ui->boxBatteryV->addItem(QString::number(BatteryV_b));
                        ui->boxBatteryV->setCurrentIndex(0);
                        ui->boxBatteryV->addItems({"1.2","2.4","3.6","4.8"});
                    }
                }
                else if(TesterType == 6)
                {
                    //根据不同电池类型
                    if(BatteryType_b == 1)
                    {
                        float BatteryV_b = result * 3.7;
                        ui->boxBatteryType->setCurrentIndex(1);
                        ui->boxBatteryV->addItem(QString::number(BatteryV_b));
                        ui->boxBatteryV->setCurrentIndex(0);
                        ui->boxBatteryV->addItems({"3.7","7.4","11.1","14.8","18.5","22.2","25.9"});
                    }
                    else if(BatteryType_b == 2)
                    {
                        float BatteryV_b = result * 3.2;
                        ui->boxBatteryType->setCurrentIndex(2);
                        ui->boxBatteryV->addItem(QString::number(BatteryV_b));
                        ui->boxBatteryV->setCurrentIndex(0);
                        ui->boxBatteryV->addItems({"3.2","6.4","9.6","12.8","16","19.2","22.4","25.6"});
                    }
                    else if(BatteryType_b == 3)
                    {
                        float BatteryV_b = result * 2.0;
                        ui->boxBatteryType->setCurrentIndex(3);
                        ui->boxBatteryV->addItem(QString::number(BatteryV_b));
                        ui->boxBatteryV->setCurrentIndex(0);
                        ui->boxBatteryV->addItems({"2.0","4.0","6.0","8.0","10.0","12.0","14.0","16.0","18.0","20.0","22.0","24.0"});
                    }
                    else if(BatteryType_b == 4)
                    {
                        float BatteryV_b = result * 1.2;
                        ui->boxBatteryType->setCurrentIndex(4);
                        ui->boxBatteryV->addItem(QString::number(BatteryV_b));
                        ui->boxBatteryV->setCurrentIndex(0);
                        ui->boxBatteryV->addItems({"1.2","2.4","3.6","4.8","6.0","7.2","8.4","9.6","10.8","12.0","13.2","14.4","15.6","16.8","18.0","19.2","20.4","21.6"});
                    }
                }
                //电池容量
                float BatteryAh_b = data.toHex().mid(10,4).toInt(&bStatus,16);
                BatteryAh = data.toHex().mid(10,4);
                ui->EditBatteryAh->setText(QString::number(BatteryAh_b));
                //充电电压
                int ChargeV_b = data.toHex().mid(14,4).toInt(&bStatus,16);
                ChargeV = data.toHex().mid(14,4);
                ui->EditChargeV->setText(QString::number(ChargeV_b/100));
                //充电电流
                float ChargeA_b = data.toHex().mid(18,4).toInt(&bStatus,16);
                ChargeA = data.toHex().mid(18,4);
                ui->EditChargeA->setText(QString::number(ChargeA_b/10));
                //电池内阻
                float BatteryOmega_b = data.toHex().mid(22,4).toInt(&bStatus,16);
                BatteryOmega = data.toHex().mid(22,4);
                ui->EditBatteryOmega->setText(QString::number(BatteryOmega_b));
                //放电电流
                float DischargeA_b = data.toHex().mid(26,4).toInt(&bStatus,16);
                DischargeA = data.toHex().mid(26,4);
                ui->EditDischargeA->setText(QString::number(DischargeA_b/10));
                //停止电压
                float DischargeRate_b = data.toHex().mid(30,4).toInt(&bStatus,16);
                DischargeRate = data.toHex().mid(30,4);
                ui->EditDischargeRate->setText(QString::number(DischargeRate_b/10));
                //保护温度  如果(TesterType == 5 || TesterType == 6)为停止电流
                float ProtectT_b = data.toHex().mid(34,2).toInt(&bStatus,16);
                float StopA_b = data.toHex().mid(36,2).toInt(&bStatus,16);
                ProtectT = data.toHex().mid(34,2);
                StopA = data.toHex().mid(36,2);
                if(TesterType == 5 || TesterType == 6) ui->EditProtectT->setText(QString::number(StopA_b/10));
                else ui->EditProtectT->setText(QString::number(ProtectT_b));
            }
        }
        //读取运行参数信息
        else if (data.toHex().mid(2,2) == "0c")
        {
            //qDebug()<<"Get 0c";
            if((ui->EditChargeV->text() == "" && ui->EditBatteryAh->text() == "" && TesterType != 0 )||
                    (ui->EditDischargeRate->text() == "" && ui->EditProtectT->text() == "" && TesterType != 0 ))
            {
                fill_cnt++;
                if(fill_cnt > 2)
                {
                    QByteArray read_0b = crc16Hex("000b");
                    m_connect485.Send_Data(read_0b);
                    fill_cnt = 0;
                    getvalue_0b=1;
                }
            }
            int device_num = data.toHex().mid(0,2).toInt(&bStatus,16);
            device_num -= 1;
            //进入485模式用于处理"0c"读取运行参数信息的函数
            get_0C_Info(data, device_num);
        }
    }
}

//485模式用于处理"0c"读取运行参数信息的函数
void MainWindow::get_0C_Info(QByteArray data, int device_num)
{
    //开始绘制曲线图
    run_chart_flag = true;
    bool bStatus = false;
    //设备号
    int num = data.toHex().mid(0,2).toInt(&bStatus,16);
    ui->infoTable->setItem(device_num,0,new QTableWidgetItem(QString::number(num)));
    //SOC
    float SOC = data.toHex().mid(4,2).toInt(&bStatus,16);
    ui->infoTable->setItem(device_num,4,new QTableWidgetItem(QString::number(SOC)));
    //SOH
    int SOH_flag = data.toHex().mid(6,2).toInt(&bStatus,16);
    if(SOH_flag == 1)
    {
        ui->infoTable->setItem(device_num,5,new QTableWidgetItem("---"));
    }
    else if(SOH_flag == 2)
    {
        float SOH = data.toHex().mid(8,2).toInt(&bStatus,16);
        ui->infoTable->setItem(device_num,5,new QTableWidgetItem(QString::number(SOH)+"% R"));
    }
    else if(SOH_flag == 3)
    {
        float SOH = data.toHex().mid(8,2).toInt(&bStatus,16);
        ui->infoTable->setItem(device_num,5,new QTableWidgetItem(QString::number(SOH)+"%"));
    }
    //市电接入标志位
    switch(data.toHex().mid(10,2).toInt(&bStatus,16))
    {
        case 0: ui->labelPowerConnect->setText(tr("未接入")); break;
        case 1: ui->labelPowerConnect->setText(tr("接入")); break;
    }
    //电池连接标志位
    switch(data.toHex().mid(12,2).toInt(&bStatus,16))
    {
        case 0: ui->labelBatteryConnect->setText(tr("未连接")); break;
        case 1: ui->labelBatteryConnect->setText(tr("连接")); break;
    }
    if(TesterType != 5 && TesterType != 6)
    {
        //运行状态标志位
        switch(data.toHex().mid(14,2).toInt(&bStatus,16)) {
          case 0: ui->labelCurrentState->setText(tr("待机")); break;
          case 1: ui->labelCurrentState->setText(tr("充电中")); break;
          case 2: ui->labelCurrentState->setText(tr("暂停充电")); break;
          case 3: ui->labelCurrentState->setText(tr("充电完成")); break;
          case 4: ui->labelCurrentState->setText(tr("空电充满")); break;
          case 5: ui->labelCurrentState->setText(tr("非空起充")); break;
          case 6: ui->labelCurrentState->setText(tr("放电中")); break;
          case 7: ui->labelCurrentState->setText(tr("暂停放电")); break;
          case 8: ui->labelCurrentState->setText(tr("放电完成")); break;
          case 9: ui->labelCurrentState->setText(tr("满电放空")); break;
          case 10: ui->labelCurrentState->setText(tr("半电放空")); break;
          case 11: ui->labelCurrentState->setText(tr("检测中")); break;
          case 12: ui->labelCurrentState->setText(tr("检测完成")); break;
          case 13: ui->labelCurrentState->setText(tr("维护充电")); break;
          case 14: ui->labelCurrentState->setText(tr("维护放电")); break;
          case 15: ui->labelCurrentState->setText(tr("暂停维护")); break;
          case 16: ui->labelCurrentState->setText(tr("维护完成")); break;
          case 17: ui->labelCurrentState->setText(tr("短路")); break;
          case 18: ui->labelCurrentState->setText(tr("过压")); break;
          case 19: ui->labelCurrentState->setText(tr("过流")); break;
          case 20: ui->labelCurrentState->setText(tr("过温")); break;
          case 21: ui->labelCurrentState->setText(tr("风扇异常")); break;
          case 22: ui->labelCurrentState->setText(tr("请接市电")); break;
          case 23: ui->labelCurrentState->setText(tr("电池异常")); break;
        }
    }
    //电池内阻
    float Omega = data.toHex().mid(16,4).toInt(&bStatus,16);
    ui->infoTable->setItem(device_num,3,new QTableWidgetItem(QString::number(Omega)));
    //运行容量
    float ChargeAh = data.toHex().mid(20,4).toInt(&bStatus,16);
    ui->infoTable->setItem(device_num,6,new QTableWidgetItem(QString::number(ChargeAh/10)));
    //电压
    float v1  = data.toHex().mid(24,4).toInt(&bStatus,16);
    ui->infoTable->setItem(device_num,1,new QTableWidgetItem(QString::number(v1/1000)));
    //电流
    float a1  = data.toHex().mid(28,4).toInt(&bStatus,16);
    ui->infoTable->setItem(device_num,2,new QTableWidgetItem(QString::number(a1/1000)));
    //运行时间 分钟
    minute = data.toHex().mid(32,4).toInt(&bStatus,16);
    QString runState = QString("%1").arg(minute, 2, 10, QChar('0')) + "min";
    ui->labelRunTime->setText(runState);
    //触点温度
    if(TesterType != 5 && TesterType != 6)
    {
        float T = data.toHex().mid(36,2).toInt(&bStatus,16);
        ui->infoTable->setItem(device_num,7,new QTableWidgetItem(QString::number(T)));
    }
}

//改变按键颜色
void MainWindow::ButtonSetColor()
{
    QList<QPushButton*> btns = ui->ButtonWidget->findChildren<QPushButton*>();
    foreach(QPushButton* btn, btns)
    {
        btn->setStyleSheet("background-color: #f2f2f2;"
                           "color: #000000;");
    }
}

//快速检测模式
void MainWindow::on_btnQuickCheck_clicked()
{
    if (getvalue_0b == 1/* && currentState != 1*/)
    {
        currentState = 1;
        ButtonSetColor();
        ui->btnQuickCheck->setStyleSheet("background-color: #00C5CD;"
                                         "color: #FFFFFF;");
        //蓝牙模式
        if(ui->radioBtn_blue->isChecked() == true)
        {

        }
        //485模式
        else if(ui->radioBtn_485->isChecked() == true)
        {
            if(TesterType == 5 || TesterType == 6)
                ui->labelCurrentState->setText(tr("检测"));
            //显示运行参数预览界面
            QByteArray sendData = crc16Hex("00010301");
            m_connect485.Send_Data(sendData);
            Delay_MSec(2000);
            sendData = crc16Hex("00010401");
            m_connect485.Send_Data(sendData);
            Delay_MSec(50);
            m_connect485.Send_Data(sendData);
            Delay_MSec(50);
            m_connect485.Send_Data(sendData);
            //启动一次充电
            QByteArray OnOff1 = crc16Hex("000301");
            m_connect485.Send_Data(OnOff1);
        }
    }
}

//单次充电模式
void MainWindow::on_btnChargeOnce_clicked()
{
    if (getvalue_0b == 1 && currentState != 2 )
    {
        currentState = 2;
        ButtonSetColor();
        ui->btnChargeOnce->setStyleSheet("background-color: #00C5CD;"
                                         "color: #FFFFFF;");
        //蓝牙模式
        if(ui->radioBtn_blue->isChecked() == true)
        {

        }
        //485模式
        else if(ui->radioBtn_485->isChecked() == true)
        {
            if(TesterType == 5 || TesterType == 6)
                ui->labelCurrentState->setText(tr("充电"));
            clearChart();
            //显示运行参数预览界面
            QByteArray sendData = crc16Hex("00010301");
            m_connect485.Send_Data(sendData);
            Delay_MSec(2000);
            sendData = crc16Hex("00010402");
            m_connect485.Send_Data(sendData);
            Delay_MSec(50);
            m_connect485.Send_Data(sendData);
            Delay_MSec(50);
            m_connect485.Send_Data(sendData);
        }
    }
}

//单次放电模式
void MainWindow::on_btnDischargeOnce_clicked()
{
    if (getvalue_0b == 1 && currentState != 3)
    {
        currentState = 3;
        ButtonSetColor();
        ui->btnDischargeOnce->setStyleSheet("background-color: #00C5CD;"
                                            "color: #FFFFFF;");
        //蓝牙模式
        if(ui->radioBtn_blue->isChecked() == true)
        {

        }
        //485模式
        else if(ui->radioBtn_485->isChecked() == true)
        {
            if(TesterType == 5 || TesterType == 6)
                ui->labelCurrentState->setText(tr("放电"));
            clearChart();
            //显示运行参数预览界面
            QByteArray sendData = crc16Hex("00010301");
            m_connect485.Send_Data(sendData);
            Delay_MSec(2000);
            sendData = crc16Hex("00010403");
            m_connect485.Send_Data(sendData);
            Delay_MSec(50);
            m_connect485.Send_Data(sendData);
            Delay_MSec(50);
            m_connect485.Send_Data(sendData);
        }
    }
}

//维护模式
void MainWindow::on_btnMaintain_clicked()
{
    if (getvalue_0b == 1 && currentState != 4)
    {
        currentState = 4;
        ButtonSetColor();
        ui->btnMaintain->setStyleSheet("background-color: #00C5CD;"
                                       "color: #FFFFFF;");
        //蓝牙模式
        if(ui->radioBtn_blue->isChecked() == true)
        {

        }
        //485模式
        else if(ui->radioBtn_485->isChecked() == true)
        {
            if(TesterType == 5 || TesterType == 6)
                ui->labelCurrentState->setText(tr("维护"));
            clearChart();
            //显示运行参数预览界面
            QByteArray sendData = crc16Hex("00010301");
            m_connect485.Send_Data(sendData);
            Delay_MSec(2000);
            sendData = crc16Hex("00010404");
            m_connect485.Send_Data(sendData);
            Delay_MSec(50);
            m_connect485.Send_Data(sendData);
            Delay_MSec(50);
            m_connect485.Send_Data(sendData);
        }
    }
}

//暂停或开始
void MainWindow::on_btnPauseResume_clicked()
{
    if (getvalue_0b == 1)
    {
        QByteArray sendData;
        //蓝牙模式
        if(ui->radioBtn_blue->isChecked() == true)
        {

        }
        //485模式
        else if(ui->radioBtn_485->isChecked() == true)
        {
            if(btnPauseResume)
            {
                 OnOff = crc16Hex("000301");
                 btnPauseResume = false;
            }
            else
            {
                OnOff = crc16Hex("000300");
                btnPauseResume = true;
            }
            qDebug() << "OnOff" << OnOff;
        }
    }
}

//点击保存按键
void MainWindow::on_btnConfirm_clicked()
{
    if (getvalue_0b == 1)
    {
        //电池类型
        switch(ui->boxBatteryType->currentIndex())
        {
          case 1: BatteryType = "01"; break;
          case 2: BatteryType = "02"; break;
          case 3: BatteryType = "03"; break;
          case 4: BatteryType = "04"; break;
        }
        //电池电压
        int BV = ui->boxBatteryV->currentText().toFloat();
        if(ui->boxBatteryType->currentIndex() == 1) BV /= 3.7;
        else if(ui->boxBatteryType->currentIndex() == 2) BV /= 3.2;
        else if(ui->boxBatteryType->currentIndex() == 3) BV /= 2.0;
        else if(ui->boxBatteryType->currentIndex() == 4) BV /= 1.2;
        BatteryV = QString("%1").arg(BV, 4, 16, QLatin1Char('0'));
        //电池容量
        int BAh = ui->EditBatteryAh->text().toFloat();
        BatteryAh = QString("%1").arg(BAh, 4, 16, QLatin1Char('0'));
        //充电电压
        int CV = ui->EditChargeV->text().toFloat()*100;
        ChargeV = QString("%1").arg(CV, 4, 16, QLatin1Char('0'));
        //充电电流
        int CA = ui->EditChargeA->text().toFloat()*10;
        ChargeA = QString("%1").arg(CA, 4, 16, QLatin1Char('0'));
        //电池内阻
        int BOmega = ui->EditBatteryOmega->text().toFloat();
        BatteryOmega = QString("%1").arg(BOmega, 4, 16, QLatin1Char('0'));
        //放电电流
        int DA = ui->EditDischargeA->text().toFloat()*10;
        DischargeA = QString("%1").arg(DA, 4, 16, QLatin1Char('0'));
        //放电截至
        int DR = ui->EditDischargeRate->text().toFloat()*10;
        DischargeRate = QString("%1").arg(DR, 4, 16, QLatin1Char('0'));
        //保护温度 停止电流
        int PT = 0;
        if(TesterType == 4 || TesterType == 5) PT = ui->EditProtectT->text().toInt()*10;
        else PT = ui->EditProtectT->text().toInt();
        ProtectT = QString("%1").arg(PT, 2, 16, QLatin1Char('0'));
        //停止电流
        int SA = ui->EditProtectT->text().toInt()*10;
        StopA = QString("%1").arg(SA, 2, 16, QLatin1Char('0'));
        //蓝牙模式
        if(ui->radioBtn_blue->isChecked() == true)
        {

        }
        //485模式
        else if(ui->radioBtn_485->isChecked() == true)
        {
            QString dataString = "";
            if(TesterType == 4 || TesterType == 5) dataString = "0002" + BatteryType + BatteryV + BatteryAh + ChargeV + ChargeA + BatteryOmega + DischargeA + DischargeRate  + ProtectT + StopA;
            else dataString = "0002" + BatteryType + BatteryV + BatteryAh + ChargeV + ChargeA + BatteryOmega + DischargeA + DischargeRate  + ProtectT;
            qDebug() <<"总字符"<< dataString;
            //将设置发送到单片机
            QByteArray sendData = crc16Hex(dataString);
            m_connect485.Send_Data(sendData);
            Sleep(500);
            //显示运行参数预览界面
            sendData = crc16Hex("000B");
            m_connect485.Send_Data(sendData);
            Sleep(500);
            //显示运行参数预览界面
            sendData = crc16Hex("00010301");
            m_connect485.Send_Data(sendData);
        }
        //改变标题显示
        QString a = ui->boxBatteryV->currentText();
        QString b = ui->EditBatteryAh->text();
        QString c = ui->boxBatteryType->currentText();
        QString TopString = a + "V " + b + "Ah " + c + tr("电池") + " " + QString::number(cap) + " " + tr("串级联");
        ui->labelTop->setText(TopString);
        QMessageBox::information(this, "Save param", "Saved");
    }
}

//延时毫秒
void MainWindow::Delay_MSec(unsigned int msec)
{
    QEventLoop loop;//定义一个新的事件循环
    QTimer::singleShot(msec, &loop, SLOT(quit()));//创建单次定时器，槽函数为事件循环的退出函数
    loop.exec();//事件循环开始执行，程序会卡在这里，直到定时时间到，本循环被退出
}

//电池类型下拉框改变
void MainWindow::on_boxBatteryType_currentIndexChanged(int index)
{
    ui->boxBatteryV->clear();
    //TesterType = 1：BM70-5、 2：BM70-18、 3：BM200-5-D、 BM200-32-D
    if (setValue==1 && TesterType==3)
    {
        switch(index)
        {
          case 1: ui->boxBatteryV->addItem("3.7"); break;
          case 2: ui->boxBatteryV->addItem("3.2"); break;
          case 3: ui->boxBatteryV->addItems({"2.0","4.0"}); break;
          case 4: ui->boxBatteryV->addItems({"1.2","2.4","3.6"}); break;

        }
    }
    else if (setValue==1 && TesterType==4)
    {
        switch(index)
        {
          case 1: ui->boxBatteryV->addItems({"3.7","7.4","11.1","14.8","18.5","22.2","25.9"}); break;
          case 2: ui->boxBatteryV->addItems({"3.2","6.4","9.6","12.8","16","19.2","22.4","25.6"}); break;
          case 3: ui->boxBatteryV->addItems({"2.0","4.0","6.0","8.0","10.0","12.0","14.0","16.0","18.0","20.0","22.0","24.0"}); break;
          case 4: ui->boxBatteryV->addItems({"1.2","2.4","3.6","4.8","6.0","7.2","8.4","9.6","10.8","12.0","13.2","14.4","15.6","16.8","18.0","19.2","20.4","21.6"}); break;
        }
    }
    else if (setValue==1 && TesterType==5)
    {
        switch(index)
        {
            case 1: ui->boxBatteryV->addItems({"3.7"}); break;
            case 2: ui->boxBatteryV->addItems({"3.2"}); break;
            case 3: ui->boxBatteryV->addItems({"2.0","4.0"}); break;
            case 4: ui->boxBatteryV->addItems({"1.2","2.4","3.6","4.8"}); break;
        }
    }
    else if (setValue==1 && TesterType==6)
    {
        switch(index)
        {
            case 1: ui->boxBatteryV->addItems({"3.7","7.4","11.1","14.8","18.5","22.2","25.9"}); break;
            case 2: ui->boxBatteryV->addItems({"3.2","6.4","9.6","12.8","16","19.2","22.4","25.6"}); break;
            case 3: ui->boxBatteryV->addItems({"2.0","4.0","6.0","8.0","10.0","12.0","14.0","16.0","18.0","20.0","22.0","24.0"}); break;
            case 4: ui->boxBatteryV->addItems({"1.2","2.4","3.6","4.8","6.0","7.2","8.4","9.6","10.8","12.0","13.2","14.4","15.6","16.8","18.0","19.2","20.4","21.6"}); break;
        }
    }
}

//电池电压下拉框改变
void MainWindow::on_boxBatteryV_currentIndexChanged(int index)
{
    //TesterType = 1：BM70-5、 2：BM70-18、 3：BM200-5-D、 BM200-32-D
    if (setValue==1 && TesterType==3)
    {
        if(ui->boxBatteryType->currentIndex()==1)
        {
            ui->EditChargeV->setText("4.2");
            ui->EditDischargeRate->setText("3.14");
            setChargeV=4.2;
        }
        else if(ui->boxBatteryType->currentIndex()==2)
        {
            ui->EditChargeV->setText("3.65");
            ui->EditDischargeRate->setText("2.72");
            setChargeV=3.6;
        }
        else if(ui->boxBatteryType->currentIndex()==3)
        {
            if(ui->boxBatteryV->currentText().toFloat()==2.0)
            {
                ui->EditChargeV->clear();
                ui->EditChargeV->setText(QString::number(2.46));
                ui->EditDischargeRate->setText("1.8");
                qDebug()<<"boxBatteryV "+ui->boxBatteryV->currentText();
                setChargeV=2.46;
            }
            if (ui->boxBatteryV->currentText().toFloat()==4.0)
            {
                ui->EditChargeV->clear();
                ui->EditChargeV->setText(QString::number(4.93));
                ui->EditDischargeRate->setText("3.6");
                setChargeV=4.93;
            }
        }
        else if(ui->boxBatteryType->currentIndex()==4)
        {
            switch(ui->boxBatteryV->currentIndex())
            {
              case 0:
                ui->EditChargeV->setText("1.6");
                ui->EditDischargeRate->setText("1.0");
                break;
              case 1:
                ui->EditChargeV->setText("3.2");
                ui->EditDischargeRate->setText("1.9");
                break;
              case 2:
                ui->EditChargeV->setText("4.8");
                ui->EditDischargeRate->setText("2.9");
                break;
            }
        }
    }
    else if (TesterType==4 && setValue==1)
    {
        if(ui->boxBatteryType->currentIndex()==1) //锂电
        {
            ui->EditChargeV->setText(QString::number(ui->boxBatteryV->currentText().toFloat()/3.7*4.2));
            ui->EditDischargeRate->setText(QString::number(ui->boxBatteryV->currentText().toFloat()*0.85));
        }
        else if(ui->boxBatteryType->currentIndex()==2) //铁锂
        {
            ui->EditChargeV->setText(QString::number(ui->boxBatteryV->currentText().toFloat()/3.2 * 3.65));
            ui->EditDischargeRate->setText(QString::number(ui->boxBatteryV->currentText().toFloat()*0.85));
        }
        else if(ui->boxBatteryType->currentIndex()==3) //铅酸
        {
            ui->EditChargeV->setText(QString::number(ui->boxBatteryV->currentText().toFloat()/2 * 14.8/12*2,'f',2));
            ui->EditDischargeRate->setText(QString::number(ui->boxBatteryV->currentText().toFloat()* 0.9));
        }
        else if(ui->boxBatteryType->currentIndex()==4)
        {
            switch(ui->boxBatteryV->currentIndex())
            {
              case 0: ui->EditChargeV->setText("1.6");
                      ui->EditDischargeRate->setText("1.0");
                      break;
              case 1: ui->EditChargeV->setText("3.2");
                      ui->EditDischargeRate->setText("1.9");
                      break;
              case 2: ui->EditChargeV->setText("4.8");
                      ui->EditDischargeRate->setText("2.9");
                      break;
            }
        }
        if(ui->EditChargeV->text().toFloat()*ui->EditChargeA->text().toFloat()>200)
            ui->EditChargeA->setText(QString::number(200/ui->EditChargeV->text().toFloat(),'f',1));
    }
    setChargeV=ui->EditChargeV->text().toFloat();
    if (setValue==0) setValue=1;

    float omega=0.0;
    if(ui->boxBatteryType->currentIndex()==1)
    {
        omega = (ui->boxBatteryV->currentText().toFloat()/3.7) * (1 + 400/ui->EditBatteryAh->text().toFloat());
        if (omega > 100) omega = 100;
        else if (omega < 1) omega = 1;
        ui->EditBatteryOmega->setText(QString::number(omega, 'f', 2));
    }
    else if(ui->boxBatteryType->currentIndex()==2)
    {
        omega = (ui->boxBatteryV->currentText().toFloat()/3.2) * (1 + 400/ui->EditBatteryAh->text().toFloat());
        if (omega > 100) omega = 100;
        else if (omega < 1) omega = 1;
        ui->EditBatteryOmega->setText(QString::number(omega, 'f', 2));
    }
    else if(ui->boxBatteryType->currentIndex()==3)
    {
        omega = ((ui->boxBatteryV->currentText().toFloat()/12) * (1 + 300/ui->EditBatteryAh->text().toFloat()));
        if (omega > 100) omega = 100;
        else if (omega < 1) omega = 1;
        ui->EditBatteryOmega->setText(QString::number(omega, 'f', 2));
    }
    else if(ui->boxBatteryType->currentIndex()==4)
    {
        omega = cap * (1 + 200/ui->EditBatteryAh->text().toFloat());
        if (omega > 100) omega = 100;
        else if (omega < 1) omega = 1;
        ui->EditBatteryOmega->setText(QString::number(omega, 'f', 2));
    }

    if(ui->boxBatteryType->currentIndex()==1) //锂电
    {
        ui->EditChargeV->setText(QString::number(ui->boxBatteryV->currentText().toFloat()/3.7 * 4.2));
        ui->EditDischargeRate->setText(QString::number(ui->boxBatteryV->currentText().toFloat()*0.85));
    }
    else if(ui->boxBatteryType->currentIndex()==2) //铁锂
    {
        ui->EditChargeV->setText(QString::number(ui->boxBatteryV->currentText().toFloat()/3.2 * 3.6));
        ui->EditDischargeRate->setText(QString::number(ui->boxBatteryV->currentText().toFloat()*0.85));
    }
    else if(ui->boxBatteryType->currentIndex()==3) //铅酸
    {
        ui->EditChargeV->setText(QString::number(ui->boxBatteryV->currentText().toFloat()/2 * 2.45));
        ui->EditDischargeRate->setText(QString::number(ui->boxBatteryV->currentText().toFloat()* 0.9));
    }
    else if(ui->boxBatteryType->currentIndex()==4) //镍氢
    {
        ui->EditChargeV->setText(QString::number(ui->boxBatteryV->currentText().toFloat()/1.2 * 1.5));
        ui->EditDischargeRate->setText(QString::number(ui->boxBatteryV->currentText().toFloat()* 0.9));
    }
    setChargeV = ui->EditChargeV->text().toFloat();
}

//电池安时数改变
void MainWindow::on_EditBatteryAh_textChanged(const QString &arg1)
{
    QString a = ui->boxBatteryV->currentText();
    QString b = ui->EditBatteryAh->text();
    QString c = ui->boxBatteryType->currentText();
    //改变标题显示
    QString TopString = a + "V " + b + "Ah " + c + tr("电池")+ " " + QString::number(cap) + " " + tr("串级联");
    ui->labelTop->setText(TopString);
    //TesterType = 1：BM70-5、 2：BM70-18、 3：BM200-5-D、 BM200-32-D
    if (TesterType==3)
    {
        if(ui->EditChargeA->text().toFloat()>30)
            ui->EditChargeA->setText("30");
        else if(ui->EditChargeA->text().toFloat()<2)
            ui->EditChargeA->setText("2");

        if(ui->EditDischargeA->text().toFloat()>25)
            ui->EditDischargeA->setText("25");
        else if(ui->EditDischargeA->text().toFloat()<0.5)
            ui->EditDischargeA->setText("0.5");

        if(ui->EditDischargeA->text().toFloat() > 90/ui->EditChargeV->text().toFloat())
        {
            float dischargeA=90/ui->EditChargeV->text().toFloat();
            ui->EditDischargeA->setText(QString::number(dischargeA, 'f', 1));
        }
    }
    else if (TesterType==4)
    {
        if(ui->EditChargeA->text().toFloat()>8)
            ui->EditChargeA->setText("8");
        else if(ui->EditChargeA->text().toFloat()<1)
            ui->EditChargeA->setText("1");

        if(ui->EditDischargeA->text().toFloat()>6)
            ui->EditDischargeA->setText("6");
        else if(ui->EditDischargeA->text().toFloat()<0.5)
            ui->EditDischargeA->setText("0.5");

        if(ui->EditChargeA->text().toFloat() > 200/ui->EditChargeV->text().toFloat())
        {
            float chargeA=200/ui->EditChargeV->text().toFloat();
            ui->EditChargeA->setText(QString::number(chargeA, 'f', 1));
        }

        if(ui->EditDischargeA->text().toFloat()>6)
            ui->EditDischargeA->setText("6");

        if(ui->EditDischargeA->text().toFloat() > 90/ui->EditChargeV->text().toFloat())
        {
            float dischargeA=90/ui->EditChargeV->text().toFloat();
            ui->EditDischargeA->setText(QString::number(dischargeA, 'f', 1));
        }
    }
    if (ui->EditChargeA->text().toFloat()<1)
        ui->EditChargeA->setText("1");

    if (ui->EditDischargeA->text().toFloat()<1)
        ui->EditDischargeA->setText("1");

    float omega=0.0;
    if(ui->boxBatteryType->currentIndex()==1)   //锂电
    {
        omega = (ui->boxBatteryV->currentText().toFloat()/3.7) * (1 + 400/ui->EditBatteryAh->text().toFloat());
        if (omega > 100) omega = 100;
        else if (omega < 1) omega = 1;
        ui->EditBatteryOmega->setText(QString::number(omega, 'f', 2));
    }
    else if(ui->boxBatteryType->currentIndex()==2)  //铁锂
    {
        omega = (ui->boxBatteryV->currentText().toFloat()/3.2) * (1 + 400/ui->EditBatteryAh->text().toFloat());
        if (omega > 100) omega = 100;
        else if (omega < 1) omega = 1;
        ui->EditBatteryOmega->setText(QString::number(omega, 'f', 2));
    }
    else if(ui->boxBatteryType->currentIndex()==3)  //铅酸
    {
        omega = ((ui->boxBatteryV->currentText().toFloat()/12) * (1 + 300/ui->EditBatteryAh->text().toFloat()));
        if (omega > 100) omega = 100;
        else if (omega < 1) omega = 1;
        ui->EditBatteryOmega->setText(QString::number(omega, 'f', 2));
    }
    else if(ui->boxBatteryType->currentIndex()==4)  //镍氢
    {
        omega = cap * (1 + 200/ui->EditBatteryAh->text().toFloat());    //这里应该不是用cap，应该是多少串电池才对
        if (omega > 100) omega = 100;
        else if (omega < 1) omega = 1;
        ui->EditBatteryOmega->setText(QString::number(omega, 'f', 2));
    }
    if (TesterType==4)
    {
        if(ui->boxBatteryType->currentIndex()==4)
        {
            switch(ui->boxBatteryV->currentIndex())
            {
              case 0: ui->EditChargeV->setText("1.6");
                      ui->EditDischargeRate->setText("1.0");
                      break;
              case 1: ui->EditChargeV->setText("3.2");
                      ui->EditDischargeRate->setText("1.9");
                      break;
              case 2: ui->EditChargeV->setText("4.8");
                      ui->EditDischargeRate->setText("2.9");
                      break;
            }
        }
    }
    if(ui->boxBatteryType->currentIndex()==1) //锂电
    {
        ui->EditChargeV->setText(QString::number(ui->boxBatteryV->currentText().toFloat()/3.7 * 4.2));
        ui->EditDischargeRate->setText(QString::number(ui->boxBatteryV->currentText().toFloat()*0.85));
    }
    else if(ui->boxBatteryType->currentIndex()==2) //铁锂
    {
        ui->EditChargeV->setText(QString::number(ui->boxBatteryV->currentText().toFloat()/3.2 * 3.6));
        ui->EditDischargeRate->setText(QString::number(ui->boxBatteryV->currentText().toFloat()*0.85));
    }
    else if(ui->boxBatteryType->currentIndex()==3) //铅酸
    {
        ui->EditChargeV->setText(QString::number(ui->boxBatteryV->currentText().toFloat()/2 * 2.45));
        ui->EditDischargeRate->setText(QString::number(ui->boxBatteryV->currentText().toFloat()* 0.9));
    }
    else if(ui->boxBatteryType->currentIndex()==4) //镍氢
    {
        ui->EditChargeV->setText(QString::number(ui->boxBatteryV->currentText().toFloat()/1.2 * 1.5));
        ui->EditDischargeRate->setText(QString::number(ui->boxBatteryV->currentText().toFloat()* 0.9));
    }
    setChargeV = ui->EditChargeV->text().toFloat();
}

//电池参数设置校对按键按下
void MainWindow::on_btncheckText_clicked()
{
    //蓝牙模式
    if(ui->radioBtn_blue->isChecked() == true)
    {

    }
    //485模式
    else if(ui->radioBtn_485->isChecked() == true)
    {
        //暂停充电指令
        QByteArray send= crc16Hex("000300");
        m_connect485.Send_Data(send);
        Delay_MSec(1000);
        //显示设置界面
        QByteArray sendData = crc16Hex("00010201");
        m_connect485.Send_Data(sendData);
        Delay_MSec(50);
        m_connect485.Send_Data(sendData);
        Delay_MSec(50);
        m_connect485.Send_Data(sendData);
    }
}

//电池安时数编辑
void MainWindow::on_EditBatteryAh_textEdited(const QString &arg1)
{
    ui->EditChargeA->setText(QString::number(ui->EditBatteryAh->text().toInt()*0.2));
    ui->EditDischargeA->setText(QString::number(ui->EditBatteryAh->text().toInt()*0.2));
    float omega=0.0;
    if(ui->boxBatteryType->currentIndex()==1 || ui->boxBatteryType->currentIndex()==2)
    {
        omega = cap * (1 + 400/ui->EditBatteryAh->text().toFloat());
        if (omega > 100) omega = 100;
        else if (omega < 1) omega = 1;
        ui->EditBatteryOmega->setText(QString::number(omega, 'f', 2));
    }
    else if(ui->boxBatteryType->currentIndex()==3)
    {
        omega = ((cap/6) * (1 + 300/ui->EditBatteryAh->text().toFloat()));
        if (omega > 100) omega = 100;
        else if (omega < 1) omega = 1;
        ui->EditBatteryOmega->setText(QString::number(omega, 'f', 2));
    }
    else if(ui->boxBatteryType->currentIndex()==4)
    {
        omega = cap * (1 + 200/ui->EditBatteryAh->text().toFloat());
        if (omega > 100) omega = 100;
        else if (omega < 1) omega = 1;
        ui->EditBatteryOmega->setText(QString::number(omega, 'f', 2));
    }
}

//电池电压
void MainWindow::on_boxBatteryV_editTextChanged(const QString &arg1)
{
    ui->EditDischargeRate->setText(QString::number(ui->boxBatteryV->currentText().toInt()*0.8));
}

//放电倍率
void MainWindow::on_EditDischargeRate_editingFinished()
{
    if(ui->EditDischargeRate->text().toFloat()>ui->boxBatteryV->currentText().toFloat())
        ui->EditDischargeRate->setText(ui->boxBatteryV->currentText());
    else if (ui->EditDischargeRate->text().toFloat()<(ui->boxBatteryV->currentText().toFloat()*0.7))
        ui->EditDischargeRate->setText(QString::number(ui->boxBatteryV->currentText().toFloat()*0.7,'f',1));
}

//放电电流
void MainWindow::on_EditDischargeA_textEdited(const QString &arg1)
{
    //TesterType = 1：BM70-5、 2：BM70-18、 3：BM200-5-D、 BM200-32-D
    if (TesterType==3)
    {
        if(ui->EditDischargeA->text().toFloat()>25)
            ui->EditDischargeA->setText("25");
        else if (ui->EditDischargeA->text().toFloat()<1)
            ui->EditDischargeA->setText("1");
    }
}

//连接设备按键
void MainWindow::on_conbluetooth_clicked()
{
    //蓝牙模式
    if(ui->radioBtn_blue->isChecked() == true)
    {

    }
    //485模式
    if(ui->radioBtn_485->isChecked() == true)
    {
        m_connect485.exec();  //模态对话框
    }
}

//保护温度限制
void MainWindow::on_EditProtectT_editingFinished()
{
    if(!(TesterType==5 || TesterType==6))
    {
        if(ui->EditProtectT->text().toFloat()<40)
            ui->EditProtectT->setText("40");
        else if(ui->EditProtectT->text().toFloat()>80)
            ui->EditProtectT->setText("80");
    }
}

//电池内阻限制
void MainWindow::on_EditBatteryOmega_editingFinished()
{
    //TesterType = 1：BM70-5、 2：BM70-18、 3：BM200-5-D、 BM200-32-D
    if(TesterType==3)
    {
        if(ui->EditBatteryOmega->text().toInt()>100)
            ui->EditBatteryOmega->setText("100");
    }
    else if(TesterType==4)
    {
        if(ui->EditBatteryOmega->text().toInt()>150)
            ui->EditBatteryOmega->setText("150");
    }

    if(ui->EditBatteryOmega->text().toInt()<1)
        ui->EditBatteryOmega->setText("1");
}

//电池容量编辑完成
void MainWindow::on_EditBatteryAh_editingFinished()
{
    //TesterType = 1：BM70-5、 2：BM70-18、 3：BM200-5-D、 BM200-32-D
    if (TesterType==3)
    {
        if(ui->EditBatteryAh->text().toFloat()<2)
            ui->EditBatteryAh->setText("2");
        else if (ui->EditBatteryAh->text().toFloat()>400)
            ui->EditBatteryAh->setText("400");

        if(ui->EditChargeA->text().toFloat()>30)
            ui->EditChargeA->setText("30");

        if(ui->EditDischargeA->text().toFloat()>25)
            ui->EditDischargeA->setText("25");

        if(ui->EditDischargeA->text().toFloat() > 90/ui->EditChargeV->text().toFloat())
        {
            float dischargeA=90/ui->EditChargeV->text().toFloat();
            ui->EditDischargeA->setText(QString::number(dischargeA, 'f', 1));
        }
    }
    else if (TesterType==4)
    {
        if(ui->EditBatteryAh->text().toFloat()<2)
            ui->EditBatteryAh->setText("2");
        else if (ui->EditBatteryAh->text().toFloat()>100)
            ui->EditBatteryAh->setText("100");

        if(ui->EditChargeA->text().toFloat()>8)
            ui->EditChargeA->setText("8");

        if(ui->EditDischargeA->text().toFloat()>6)
            ui->EditDischargeA->setText("6");

        if(ui->EditDischargeA->text().toFloat() > 90/ui->EditChargeV->text().toFloat())
        {
            float dischargeA=90/ui->EditChargeV->text().toFloat();
            ui->EditDischargeA->setText(QString::number(dischargeA, 'f', 1));
        }
    }
    if (ui->EditChargeA->text().toFloat()<1)
        ui->EditChargeA->setText("1");

    if (ui->EditDischargeA->text().toFloat()<1)
        ui->EditDischargeA->setText("1");

    float omega=0.0;
    if(ui->boxBatteryType->currentIndex()==1)
    {
        omega = (ui->boxBatteryV->currentText().toFloat()/3.7) * (1 + 400/ui->EditBatteryAh->text().toFloat());
        if (omega > 100) omega = 100;
        else if (omega < 1) omega = 1;
        ui->EditBatteryOmega->setText(QString::number(omega, 'f', 2));
    }
    else if(ui->boxBatteryType->currentIndex()==2)
    {
        omega = (ui->boxBatteryV->currentText().toFloat()/3.2) * (1 + 400/ui->EditBatteryAh->text().toFloat());
        if (omega > 100) omega = 100;
        else if (omega < 1) omega = 1;
        ui->EditBatteryOmega->setText(QString::number(omega, 'f', 2));
    }
    else if(ui->boxBatteryType->currentIndex()==3)
    {
        omega = ((ui->boxBatteryV->currentText().toFloat()/12) * (1 + 300/ui->EditBatteryAh->text().toFloat()));
        if (omega > 100) omega = 100;
        else if (omega < 1) omega = 1;
        ui->EditBatteryOmega->setText(QString::number(omega, 'f', 2));
    }
    else if(ui->boxBatteryType->currentIndex()==4)
    {
        omega = cap * (1 + 200/ui->EditBatteryAh->text().toFloat());
        if (omega > 100) omega = 100;
        else if (omega < 1) omega = 1;
        ui->EditBatteryOmega->setText(QString::number(omega, 'f', 2));
    }
}

//充电电流编辑完成
void MainWindow::on_EditChargeA_editingFinished()
{
    //TesterType = 1：BM70-5、 2：BM70-18、 3：BM200-5-D、 BM200-32-D
    if (TesterType==3)
    {
        if(ui->EditChargeA->text().toFloat()*ui->EditChargeV->text().toFloat()>200)
            ui->EditChargeA->setText(QString::number(200/ui->EditChargeV->text().toFloat(),'f',2));

        if(ui->EditChargeA->text().toFloat()>30)
            ui->EditChargeA->setText("30");
        else if(ui->EditChargeA->text().toFloat()<2)
            ui->EditChargeA->setText("2");
    }
    else if (TesterType==4)
    {
        if(ui->EditChargeA->text().toFloat()>8)
            ui->EditChargeA->setText("8");
        else if(ui->EditChargeA->text().toFloat()<1)
            ui->EditChargeA->setText("1");

        if(ui->EditChargeA->text().toFloat() > 200/ui->EditChargeV->text().toFloat())
        {
            float chargeA=200/ui->EditChargeV->text().toFloat();
            ui->EditChargeA->setText(QString::number(chargeA));
        }
    }
    if (ui->EditChargeA->text().toFloat()<1)
        ui->EditChargeA->setText("1");
}

//充电电压编辑完成
void MainWindow::on_EditChargeV_editingFinished()
{
    //TesterType = 1：BM70-5、 2：BM70-18、 3：BM200-5-D、 BM200-32-D
    if(TesterType==3)
    {
        if(ui->EditChargeV->text().toFloat()<1)
            ui->EditChargeV->setText("1");
        else if(ui->EditChargeV->text().toFloat()>5)
            ui->EditChargeV->setText("5");
    }
    else if(TesterType==4)
    {
        if(ui->EditChargeV->text().toFloat()<1)
            ui->EditChargeV->setText("1");
        else if(ui->EditChargeV->text().toFloat()>32)
            ui->EditChargeV->setText("32");
    }
    if(ui->EditDischargeA->text().toFloat()*ui->EditChargeV->text().toFloat()>90)
        ui->EditDischargeA->setText(QString::number(90/ui->EditChargeV->text().toFloat(),'f',1));
}

//放电电流
void MainWindow::on_EditDischargeA_editingFinished()
{
    //TesterType = 1：BM70-5、 2：BM70-18、 3：BM200-5-D、 BM200-32-D
    if (TesterType==3)
    {
        if(ui->EditDischargeA->text().toFloat()>25)
            ui->EditDischargeA->setText("25");
        if(ui->EditDischargeA->text().toFloat()<1)
            ui->EditDischargeA->setText("1");
    }
    else if(TesterType==4)
    {
        if(ui->EditDischargeA->text().toFloat()>6)
            ui->EditDischargeA->setText("6");
        if(ui->EditDischargeA->text().toFloat()<0.5)
            ui->EditDischargeA->setText("0.5");
    }
}

//禁用选择连接方式
void MainWindow::DisableUI(int cmd)
{
    switch (cmd) {
        case 0:
            ui->mode_select_box->setEnabled(true);
            break;
        case 1:
            ui->mode_select_box->setDisabled(true);
            break;
        default:
            break;
    }
}

//crc校验
QByteArray MainWindow::crc16Hex(QString originData)
{
    auto data = QByteArray::fromHex(originData.toLatin1());
    auto crc16ForModbus = JQChecksum::crc16ForModbus( data );
    QString n = QString::number( crc16ForModbus, 16 );
    QString temp1 = n.mid(0, 2);
    QString temp2 = n.mid(2, 2);
    QString result = temp2 + temp1;
    QString all = originData + result;
    qDebug() << all;
    QByteArray allData = QByteArray::fromHex(all.toLatin1());
    return allData;
}
