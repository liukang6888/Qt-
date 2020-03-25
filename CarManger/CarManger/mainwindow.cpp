#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QSql>
#include <QMessageBox> // 提示对话框
#include <QSqlQueryModel>  // sql语句查询模型
#include <QSqlQuery>
#include <QDebug>
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QSettings>
#include <QDatetime>
#include "domxml.h"

#define cout qDebug() << "[" << __FILE__<< ":"<<__LINE__<<"]"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)

{
    ui->setupUi(this);
    // 这个设置mainWindow信息
    this->setWindowTitle("汽车管理系统");
    this->setWindowIcon(QIcon(":/new/prefix1/tubiao"));  // 设置小图标
    // 开始时候默认是车辆管理页面
    on_actionCar_triggered();

    loadConfig();
    connectDB();
    initData();
    initCacl();

    DomXML:: createXML("../demo.xml");

    // 提交，view动作
    connect(ui->ButtonSubmit, &QPushButton::clicked,
            [=]()
            {
                model->database().transaction(); // 开始事务操作
                if(model->submitAll()) // 如果提交成功
                {
                    model->database().commit();
//                    ui->textEdit_Sell_Information->append("操作成功");
                }
                else
                {
                    model->database().rollback(); //提交失败,事务回滚
                    QMessageBox::warning(this,tr("tableModel"),tr("数据库提交错误: %1")
                                         .arg(model->lastError().text()));
                    model->revertAll(); // 撤销修改
                }
            }
            );

    connect(ui->ButtonViewCancel, &QPushButton::clicked,[=]()
    {
        model->revertAll(); // view 撤销按钮
    });
    // view 删除操作
    connect(ui->ButtonDelte, &QPushButton::clicked,
            [=]()
            {
                // 获取选中的模型
                QItemSelectionModel *SelectModel = ui->tableView->selectionModel();
                // 取出索引
                QModelIndexList list  = SelectModel->selectedRows();
                // 删除所有选中的行
                for(int i = 0; i < list.size(); i++)
                {
                    model->removeRow(list.at(i).row());
                }
            }
            );
}

MainWindow::~MainWindow()
{
    delete ui;
}

// 销售管理那个菜单栏，切换页面
void MainWindow::on_actionCar_triggered()
{
    // 切换到车辆管理页面
    ui->stackedWidget->setCurrentWidget(ui->car);
    ui->label_Big->setText("车辆管理");
}

// 销售管理的销售统计菜单栏，切换页面  注意如何实现
void MainWindow::on_actionCalc_triggered()
{
    // 切换到 销售统计页面
    ui->stackedWidget->setCurrentWidget(ui->cacl);
    ui->label_Big->setText("销售统计");
}

bool MainWindow::connectDB()
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
    else
    {
//        QMessageBox::information(this, "正确", "数据库打开成功");
        return true;
    }
}

// 初始化数据
void MainWindow::initData()
{
    // 也可以用QSqlquery
    QSqlQueryModel *queryModel = new QSqlQueryModel(this); // 新建模型
    queryModel->setQuery("select name from factory");  // 执行sql语句
    ui->comboBox_Factory->setModel(queryModel);       // 下拉框初始化

    ui->comboBoxCaclFactory->setModel(queryModel);    // cacl下拉框初始化

    ui->label_Last->setText("0");   // 剩余数量初始化
    ui->lineEdit_TotalMoney->setEnabled(false);  // 金额初始化


    // 手动 设置模型
    model = new QSqlTableModel(this);
    model->setTable("brand"); // 指定用哪个表

    // 把model放在view里面
    ui->tableView->setModel(model);
    // 显示model里面的数据
    model->select();
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);  // 手动提交策略

    // 不允许修改， 可以改成管理员才能修改
//    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

}
// 读取配置文件内容
void MainWindow::ReadInitConfig(QString key, QString &value)
{
    value.clear();
    QString path = "login.ini";
    QSettings *config = new QSettings(path, QSettings::IniFormat);
    // 读取配置信息
    QVariant variant = config->value(QString("config/") + key);
    value = variant.value<QString>();
    delete config;
}

// 给当前用户和密码记录
void MainWindow::loadConfig()
{
    ReadInitConfig("username",username);
    ReadInitConfig("password",password);
//    cout << username << ": " << password;

}
// 跳转到修改密码界面
void MainWindow::on_actionSystem_triggered()
{
    repwd = new ResetPwd(this);
    repwd->show();
//    loadConfig();
//        cout << username << ": " << password;
    // 这里自定义的信号与槽，从不同的对象中传来的
    connect(repwd, &ResetPwd::SignalPwd, this, &MainWindow::slotPwd);
}
void MainWindow::slotPwd(QString OldPwd, QString NewPwd)
{
    if(OldPwd == password)
    {
        QSqlQuery query;
        QString sql = QString("update user set pwd = '%1' where user = '%2' ")
                        .arg(NewPwd)
                        .arg(username);
        query.exec(sql);
        QSqlDatabase::database().commit();
        password = NewPwd;

        QMessageBox::information(this, "正确", "密码修改成功");
        cout << password;
        delete repwd;
        return ;

    }
    else
    {
        QMessageBox::critical(this, "错误", "原密码错误");
        return ;
    }
}

// 厂家下拉框槽函数
void MainWindow::on_comboBox_Factory_currentIndexChanged(const QString &factoryName)
{
    if(factoryName == "请选择厂家")
    {
        // 下面内容清空
        ui->comboBox_Brand->clear(); // 品牌下拉框清空
        ui->lineEdit_Price->clear();// 报价清空
        ui->label_Last->setText("0"); // 剩余数量清空
        ui->lineEdit_TotalMoney->clear(); // 金额清空
        ui->spinBox->setValue(0);  // 数量清空
        ui->spinBox->setEnabled(false); // 数量刚开始不能选
        ui->ButtonSure->setEnabled(false); // 刚开始确定按钮不能按

    }
    else
    {
        // 这个一定要先清空
        ui->comboBox_Brand->clear();
        QSqlQuery query;
        QString sql = QString("select *from brand where fname = '%1'").arg(factoryName);

        // select *from brand where fname = '一汽大众' ; 和上面等价
        // 执行sql语句
        bool selectSqlIsOK = query.exec(sql);
        if(selectSqlIsOK)
        {
//            qDebug() << "sql语句查询成功";
            // 获取内容
            while(query.next())
            {
                QString name = query.value(1).toString();
                ui->comboBox_Brand->addItem(name);
            }
            ui->spinBox->setEnabled(true);
        }
        else
        {
            qDebug() <<"sql语句查询失败";
            QMessageBox::information(this, "错误","sql语句查询失败");
        }


    }
}

void MainWindow::on_comboBox_Brand_currentIndexChanged(const QString &brandName)
{
    // 初始值清空
    ui->lineEdit_Price->clear();
    ui->label_Last->clear();
    ui->spinBox->setValue(0);

    QSqlQuery query;
    // select *from brand where fname = '一汽大众' and name = '奥迪';
    QString sql = QString(
                "select price,last from brand where fname = '%1' and name = '%2'"
                ).arg(ui->comboBox_Factory->currentText()) // 厂商下拉框选中
                .arg(brandName); // 品牌

    // 执行sql语句
    bool selectSqlIsOK = query.exec(sql);
    if(selectSqlIsOK)
    {
//            qDebug() << "sql语句查询成功";
        // 获取内容
        while(query.next())
        {
            //设置报价和剩余车辆
            int price = query.value(0).toInt();
            int last_number = query.value(1).toInt();
            // 更新到对应的框
            ui->lineEdit_Price->setText(QString::number(price));
            ui->lineEdit_Price->setReadOnly(true);
            ui->label_Last->setText(QString::number(last_number));
        }
        ui->spinBox->setEnabled(true);

    }
    else
    {
        qDebug() <<"sql语句查询失败";
        QMessageBox::information(this, "错误","sql语句查询失败");
    }
}

// 数量选择框槽函数
void MainWindow::on_spinBox_valueChanged(int CurNum)
{

    // 更新剩余数量

    // 获取数据库剩余的车辆，
    QSqlQuery query;
    QString sql = QString("select sum, last from brand where name = '%1' and fname = '%2'")
                     .arg(ui->comboBox_Brand->currentText())
                     .arg(ui->comboBox_Factory->currentText());
    // 对确定按钮初始化
    if(0 == CurNum)
    {
        ui->ButtonSure->setEnabled(false);
    }
    else
        ui->ButtonSure->setEnabled(true);

    // 执行数据库
    query.exec(sql);

    int last;
    while(query.next())
    {
        last = query.value(1).toInt(); // 剩余
    }
    if(last < CurNum)
    {
        ui->spinBox->setValue(last);
        return ;
    }


    int LastNum = last - CurNum;
    ui->label_Last->setText(QString::number(LastNum));  // 设置剩余数量

    // 设置金额
    int price = ui->lineEdit_Price->text().toInt();
    int priceTotal = price * CurNum;

    ui->lineEdit_TotalMoney->setText(QString::number(priceTotal));
}

// 取消按钮
void MainWindow::on_ButtonCacel_clicked()
{
    QString factoryName = ui->comboBox_Factory->currentText();

    on_comboBox_Factory_currentIndexChanged(factoryName);
}

// 确定按钮
void MainWindow::on_ButtonSure_clicked()
{

           ui->textEdit_Sell_Information->clear();
    // 可以加个对话框，询问对话框

    // 获取信息, 当前要出售的车辆的数量,剩余量,厂商,品牌
    int NewSellNumber = ui->spinBox->text().toInt();
    int LastNumber = ui->label_Last->text().toInt();
    QString factorName = ui->comboBox_Factory->currentText();
    QString brandName = ui->comboBox_Brand->currentText();



//    qDebug() << NewSellNumber << " " << LastNumber;

    // 获取销量
    QSqlQuery query;
    QString sqlInfo = QString("select sell from brand where fname = '%1' and name = '%2'")
                            .arg(factorName)
                            .arg(brandName);

    query.exec(sqlInfo);

    int OldsellNum;

    while(query.next())
    {
        OldsellNum = query.value("sell").toInt();
    }


    int sellTotal = OldsellNum + NewSellNumber;

    // 更新数据库 剩余数量，销售数量
    QString sql = QString("update brand set sell = '%1',last = '%2'where fname = '%3' and name = '%4'")
                            .arg(sellTotal)
                            .arg(LastNumber)
                            .arg(factorName)
                            .arg(brandName);

   bool isOk =  query.exec(sql);

   if(isOk)
       qDebug() << "更新成功";
   else
       qDebug() << "更新失败";

   // 把确认后的数据更新到xml中
   QStringList list;
   list.append(factorName);
   list.append(brandName);
   list.append(ui->lineEdit_Price->text());
   list.append(QString::number(NewSellNumber));
   list.append(ui->lineEdit_TotalMoney->text());
   DomXML:: appendXML("../demo.xml",list);  // 出售的信息添加到xml

    qDebug() <<list;
   // 读取xml
   QStringList FactorList;
   QStringList BrandList;
   QStringList PriceList;
   QStringList NumList;
   QStringList TotalList;
   QStringList timeList;

   DomXML::readXML("../demo.xml",FactorList,BrandList,PriceList,NumList,TotalList);

   QDateTime current_time = QDateTime::currentDateTime();
   QString strCurTime = current_time.toString("yyyy-MM-dd");
   QString information = QString("%1  **日销售清单 ** \n").arg(strCurTime);
   ui->textEdit_Sell_Information->append(information);

   for(int i = 0; i < FactorList.size(); i++)
   {
       QString str = QString(" %1       : %2    : 卖出了%3辆,   单价为%4万,     销售额为%5万")
               .arg(FactorList.at(i))
               .arg(BrandList.at(i))
               .arg(NumList.at(i))
               .arg(PriceList.at(i))
               .arg(TotalList.at(i));

       ui->textEdit_Sell_Information->append(str);

//       qDebug() << str.toUtf8().data();
   }

   ui->ButtonSure->setEnabled(false);
   on_ButtonCacel_clicked(); // 在初始化一次


}
void MainWindow::SqlIsOk(bool isOk)
{
    if(isOk)
    {
         qDebug() << "更新成功";
    }
    else
    {
        qDebug() << "更新失败";
    }

}
// model view 添加
void MainWindow::on_ButtonAdd_clicked()
{
    // 添加空记录
    QSqlRecord record  = model->record(); // 获取空记录
    // 获取行号
    int row = model->rowCount();
    model->insertRecord(row, record);
    model->setData(model->index(row,0),row+1); // 在第0列上自动增加，因为是主键

}

// 统计销售清单
void MainWindow::initCacl()
{
    ui->wg_histgram->SetMaxValue(100);
    void (QComboBox::* sigItemChanged)(const QString &) = &QComboBox::currentIndexChanged;
    connect(ui->comboBoxCaclFactory, sigItemChanged,
           [=](const QString &name)
            {
                if(name == "请选择厂家")
                {
                    QColor color(255,255,255);
                    ui->wg_histgram->AddItem("",0, color);
                }
                ui->wg_histgram->Clear();
                ui->comboBoxCaclBrand->clear();


                QSqlQuery query;
                QString sql = QString("select name, fname,sell from brand where fname='%1'")
                        .arg(name);
                query.exec(sql);
                while(query.next())
                {
                    QString name = query.value(0).toString();
                    QString BrandName = query.value(0).toString();
                    ui->comboBoxCaclBrand->addItem(BrandName);
                    int sell = query.value(2).toInt();
                    QColor color(qrand()%255, qrand()%255,qrand()%255);
                    ui->wg_histgram->AddItem(name,sell,color);

                }
            }
            );

    // 品牌 pie使用
    connect(ui->comboBoxCaclBrand, sigItemChanged,
            [=]()
            {
              QSqlQuery query;
              QVector<SectorInfo> vec;
              QString sql = QString("select sell,last from brand where fname = '%1'and name = '%2'")
                      .arg(ui->comboBoxCaclFactory->currentText())
                      .arg(ui->comboBoxCaclBrand->currentText());
              bool isok  = query.exec(sql);
              if(!isok)
                  cout <<"error";
              while(query.next())
              {
                  SectorInfo info;
                  double sell = query.value(0).toInt();
                  double last = query.value(1).toInt();

                  double sum = sell + last;
                  info.description = "已出售";
                  info.percent = sell/sum*100;
                  vec.push_back(info);

                  info.description = "剩余";
                  info.percent = 100 - info.percent;
                  vec.push_back(info);

              }
              ui->wg_pie->setData(vec);
            }
            );

}

// 退出按钮
void MainWindow::on_actionExit_triggered()
{
    this->close();
}
// 切换stackpage时清空textedit
void MainWindow::on_toolBox_currentChanged(int index)
{
    if(index == 1)
    {
            ui->textEdit_Sell_Information->clear();
    }
}
