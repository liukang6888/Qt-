#include "login.h"
#include "ui_login.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QSql>
#include <QMessageBox> // 提示对话框
#include <QSqlQueryModel>  // sql语句查询模型
#include <QSqlQuery>
#include <QDialog>
#include <QDebug>
#include <QSettings>  // 写配置文件

#define cout qDebug() << "[" << __FILE__<< ":"<<__LINE__<<"]"

Login::Login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
    this->setWindowTitle("汽车管理系统");
    this->setWindowIcon(QIcon(":/new/prefix1/tubiao"));  // 设置小图标
    ui->ButtonLogin->setStyleSheet("color:white;background-color:rgb(0,163,255)");
    ui->ButtonExit->setStyleSheet("color:white;background-color:rgb(0,163,255)");


    ui->EditPwd->setEchoMode(QLineEdit::Password);

    // 连接数据库
    connectDB();

}

Login::~Login()
{
    delete ui;
}
// 连接数据库
bool Login::connectDB()
{
    // 连接数据库
    db =  QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("127.0.0.1");
    db.setUserName("root");
    db.setPassword("root");
    db.setDatabaseName("car");

    if(!db.open())
    {
        // 显示错误信息
        QMessageBox::information(this, "错误", "数据库打开失败",db.lastError().text());
        return false;
    }

    return true;

}
// 注意如何实现
void Login::WriteInit(QString key,QString value)
{
    QString path = "login.ini";
    QSettings *config = new QSettings(path,QSettings::IniFormat);
    QVariant variant;   // 可以保存不同类型值
    variant.setValue(value);
    // 将登录信息写到配置文件中
    config->beginGroup("config");
    config->setValue(key,variant);
    config->endGroup();
    delete config;
}
void Login::on_ButtonLogin_clicked()
{
    name = ui->EditUser->text();
    pwd = ui->EditPwd->text();
    if(name.isEmpty())
    {
        QMessageBox::warning(this, "WARNING", "用户名不能为空");
        return ;
    }
    if(pwd.isEmpty())
    {
        QMessageBox::warning(this, "WARNING", "密码不能为空");
        return ;
    }

    bool flag = false;
    QSqlQuery query;
    QString sql = QString("select pwd from user where user = '%1'").arg(name);

    bool isOk = query.exec(sql);
    if(!isOk)
    {
        cout << "sql查询失败";
    }
    while(query.next())
    {
        if(pwd == query.value(0).toString())
        {
            flag = true;
            break;
        }
    }

    // 密码匹配正确
    if(flag)
    {
        WriteInit("username",name);
        WriteInit("password",pwd);
        QDialog::accept();   // 这里遇到了个坑
        db.close();
    }
    else
    {
        QMessageBox::critical(this,"ERROR","用户名或密码错误");
        ui->EditUser->clear();
        ui->EditPwd->clear();
        return;
    }
}

void Login::on_ButtonExit_clicked()
{
    QDialog::reject();
    db.close();
}
