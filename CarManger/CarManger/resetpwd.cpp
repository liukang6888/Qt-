#include "resetpwd.h"
#include "ui_resetpwd.h"
#include <QMessageBox>
#include <QDebug>
#define cout qDebug() << "[" << __FILE__<< ":"<<__LINE__<<"]"

ResetPwd::ResetPwd(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ResetPwd)
{
    ui->setupUi(this);
    this->setWindowTitle("修改密码");
    this->setWindowIcon(QIcon(":/new/prefix1/tubiao"));  // 设置小图标
}

ResetPwd::~ResetPwd()
{
    delete ui;
}

// 修改密码
void ResetPwd::on_ButtonRePwdSubmit_clicked()
{
    OldPwd = ui->EditOldPwd->text();
    NewPwd = ui->EditNewPwd->text();
    QString SurePwd = ui->EditSurePwd->text();
    if(OldPwd.isEmpty() || NewPwd.isEmpty() || SurePwd.isEmpty())
    {
        QMessageBox::critical(this,"错误","密码不能为空");
        return ;
    }
    if (NewPwd != SurePwd)
    {
        QMessageBox::critical(this, "错误", "两次密码不一致");
        ui->EditOldPwd->clear();
        ui->EditSurePwd->clear();
        return ;
    }
    emit SignalPwd(OldPwd,NewPwd);  // 发送一个信号广播，让主窗口Mainwindow接收
}

void ResetPwd::on_ButtonRePwdCancel_clicked()
{
    this->close();
}
