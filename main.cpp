#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QFont>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(true);//最后一个窗口关闭后，程序退出
    //加载样式表
    QFile file(":/css/index.css");
    if (file.open(QFile::ReadOnly)) {
        QString qss = QLatin1String(file.readAll());
        qApp->setStyleSheet(qss);
        file.close();
    }

    //将QWidget实例创建到堆上
    MainWindow *w = new MainWindow();
    w->setAttribute(Qt::WA_DeleteOnClose);
    w->setWindowIcon(QIcon(":/image/batteryTopico.ico"));
    w->show();
    return a.exec();

//    //全局字体
//    QFont font("Microsoft YaHei", 10, 30);
//    a.setFont(font);
}
