#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include <QDialog>
#include <QSqlDatabase>
namespace Ui {
class Login;
}

class Login : public QDialog
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = 0);
    ~Login();
    bool connectDB(); // 连接数据库
    void WriteInit(QString key,QString value);  // 将当前的登录用户和密码写到配置文件中

private slots:
    void on_ButtonLogin_clicked();

    void on_ButtonExit_clicked();

private:
    Ui::Login *ui;
    QSqlDatabase db;
    QString name;
    QString pwd;
};

#endif // LOGIN_H
