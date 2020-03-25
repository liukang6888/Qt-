#include "mainwindow.h"
#include <QApplication>
#include "login.h"
#include <QDebug>
int main(int argc, char *argv[])
{
//    QApplication a(argc, argv);
//    MainWindow w;
//    Login l;
//    l.show();
//    w.show();


    QApplication a(argc, argv);
    Login l;
    if(l.exec() == QDialog::Accepted){
        MainWindow w;
        w.show();

        return a.exec();

    }else {
        return 0;
    }
    return a.exec();
}
