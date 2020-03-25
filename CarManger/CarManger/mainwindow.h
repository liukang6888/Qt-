#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlTableModel>
#include "resetpwd.h"
//#include "dmoxml.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool connectDB(); // 连接数据库
    void initData(); // 初始化数据
    void initCacl(); //统计销售清单

    // 读取 config
    void ReadInitConfig(QString key,QString &value);
    void loadConfig();

    void SqlIsOk(bool isOk); // 判断sql语句是否执行成功
    void BrandChange(QString str);

private slots:
    void on_actionCar_triggered();

    void on_actionCalc_triggered();

    void on_comboBox_Factory_currentIndexChanged(const QString &arg1);

    void on_comboBox_Brand_currentIndexChanged(const QString &arg1);

    void on_spinBox_valueChanged(int arg1);

    void on_ButtonCacel_clicked();

    void on_ButtonSure_clicked();



    void on_ButtonAdd_clicked();

    void on_actionSystem_triggered();
    void slotPwd(QString OldPwd, QString NewPwd); // 修改密码的槽函数

    void on_actionExit_triggered();

    void on_toolBox_currentChanged(int index);

private:
    Ui::MainWindow *ui;
    QSqlDatabase db;
    QSqlTableModel *model;  // 数据库模型
    QString username; // 记录当前登录的用户名
    QString password; //记录当前用户的密码
    ResetPwd *repwd; // 修改密码类对象


};

#endif // MAINWINDOW_H
