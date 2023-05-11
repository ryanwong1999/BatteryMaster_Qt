#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QFont>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //加载样式表
    QFile file(":/css/index.css");
    if (file.open(QFile::ReadOnly)) {
        QString qss = QLatin1String(file.readAll());
        qApp->setStyleSheet(qss);
        file.close();
    }

    MainWindow w;
    w.show();
    return a.exec();

//    //全局字体
//    QFont font("Microsoft YaHei", 10, 30);
//    a.setFont(font);
}
