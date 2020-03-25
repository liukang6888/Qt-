#ifndef RESETPWD_H
#define RESETPWD_H

#include <QDialog>

namespace Ui {
class ResetPwd;
}

class ResetPwd : public QDialog
{
    Q_OBJECT

public:
    explicit ResetPwd(QWidget *parent = 0);
    ~ResetPwd();
signals:
     void SignalPwd(QString OldPwd, QString NewPwd); // 定义个信号

private slots:


    void on_ButtonRePwdSubmit_clicked();

    void on_ButtonRePwdCancel_clicked();

private:
    Ui::ResetPwd *ui;

    QString OldPwd;
    QString NewPwd;
};

#endif // RESETPWD_H
