#include "domxml.h"
#include <QFile>
#include <QDomDocument> // 文件
#include <QDomProcessingInstruction> // 格式头部
#include <QDomElement>  // 元素
#include <QDebug>
#include <QTextStream> // 文本流
#include <QDateTime>  // 日期头文件

#define cout qDebug() <<"[" << __FILE__ << ": " << __LINE__ << "]"

DomXML::DomXML()
{

}

// 创建XML空文件
void DomXML:: createXML(QString filePath)
{
    // 文件关联名字
    QFile file(filePath);

    // 如果存在该文件, 不创建
    if (true == file.exists())
    {
//        cout << "文件已经存在";
        return ;
    }
    else
    {
        // 不存在 才创建
        // 只写方式打开
        bool isOk = file.open(QIODevice::WriteOnly);

        if (true == isOk)
        {
            // 创建XML文档对象
            QDomDocument doc;

            // XML头部格式<?xml version='1.0' encoding='utf-8'?>
            QDomProcessingInstruction ins;
            ins = doc.createProcessingInstruction
                    ("xml", "version=\'1.0\' encoding=\'utf-8\'");

            // 追加元素 相当于头和尾 <头部> <尾部>
            doc.appendChild(ins);

            // 根节点元素
            QDomElement root = doc.createElement("日销售清单");
            // 追加 元素
            doc.appendChild(root);


            // 保存
            QTextStream stream(&file); // 文本流关联文件
            //
            doc.save(stream, 4); // 4表示缩进字符

            // 关闭文件
            file.close();
        }
        else
        {
            cout << "WriteOnly Error";
        }

    }
}

void DomXML:: appendXML(QString filePath, QStringList list)
{
    QFile file(filePath);
    bool isOk = file.open(QIODevice::ReadOnly);

    // 打开成功
    if (true == isOk)
    {
        // 文件file和xml文档对象关联
        QDomDocument doc;
        bool isOk = doc.setContent(&file);

        // 如果关联成功
        if (true == isOk)
        {
            file.close(); // 关闭文件
            // 获取根节点元素
            QDomElement root = doc.documentElement();

             // 获取当前时间
            QDateTime date =  QDateTime::currentDateTime();
            // 把时间准换成字符串
            QString dateStr = date.toString("yyyy--MM--dd");// 2020-01-27

            // 判断根节点下有没有子节点
            if (root.hasChildNodes()) // 如果有子节点
            {
                // 查找最后一个子节点
                QDomElement lastEmt = root.lastChildElement();

                // 有没有当天日期
                if (lastEmt.attribute("date") == dateStr)
                {
                    // 有 就写有效数据
                    writeXML(doc, lastEmt, list);


                }
                else
                {// 如果没有当天日期

                    // 创建日期子节点元素
                    QDomElement dateEmt = doc.createElement("日期");
                    // 创建日期属性
                    QDomAttr dateAttr = doc.createAttribute("date");
                    // 设定属性的值
                    dateAttr.setNodeValue(dateStr);
                    // 把属性和节点关联
                    dateEmt.setAttributeNode(dateAttr);

                    // 把日期节点追加到根节点上
                    root.appendChild(dateEmt);

                    // 写有效数据
                    writeXML(doc, dateEmt, list);

                }


            }
            else // 如果没有子节点
            {
                // 创建日期子节点元素
                QDomElement dateEmt = doc.createElement("日期");
                // 创建日期属性
                QDomAttr dateAttr = doc.createAttribute("date");
                // 设定属性的值
                dateAttr.setNodeValue(dateStr);
                // 把属性和节点关联
                dateEmt.setAttributeNode(dateAttr);

                // 把日期节点追加到根节点上
                root.appendChild(dateEmt);

                // 写有效数据
                writeXML(doc, dateEmt, list);
            }

            // 保存文件
            bool isOk = file.open(QIODevice::WriteOnly);
            if (true == isOk)
            {
                QTextStream stream(&file);
                doc.save(stream, 4);
                file.close();
            }

        }
        else
        {
            cout << "setContent error";
        }

    }
    else
    {
        cout << "ReadOnly error";
    }
}
void DomXML:: writeXML(QDomDocument &doc, QDomElement &root, QStringList &list)
{
    // 当前时间获取
    QDateTime time  = QDateTime::currentDateTime();
    QString timeStr = time.toString("hh--mm--ss");  // 19--13--10 时--分--秒

    // 创建时间节点元素
    QDomElement timeEmt = doc.createElement("时间");

    // 创建属性
    QDomAttr timeAttr  = doc.createAttribute("time");
    // 给属性设置属性
    timeAttr.setNodeValue(timeStr);
    // 时间节点元素和属性关联
    timeEmt.setAttributeNode(timeAttr);
    // 把时间节点追加到日期节点后面
    root.appendChild(timeEmt);



    QDomElement factory = doc.createElement("厂家");
    QDomElement brand   = doc.createElement("品牌");
    QDomElement price   = doc.createElement("报价");
    QDomElement num     = doc.createElement("数量");
    QDomElement total   = doc.createElement("金额");

    QDomText text = doc.createTextNode(list.at(0));
    factory.appendChild(text);

    text = doc.createTextNode(list.at(1));
    brand.appendChild(text);

    text = doc.createTextNode(list.at(2));
    price.appendChild(text);

    text = doc.createTextNode(list.at(3));
    num.appendChild(text);

    text = doc.createTextNode(list.at(4));
    total.appendChild(text);

    // 追加 把工厂节点追加到时间节点后面
    timeEmt.appendChild(factory);
    timeEmt.appendChild(brand);
    timeEmt.appendChild(price);
    timeEmt.appendChild(num);
    timeEmt.appendChild(total);

}

// 读取数据
void DomXML:: readXML(QString filePath, QStringList  &fList, QStringList  &bList,
                            QStringList  &pList,QStringList  &nList,
                            QStringList  &tList)
{
    QFile file(filePath);
    bool isOk = file.open(QIODevice::ReadOnly);

    // 打开成功
    if (true == isOk)
    {
        // 文件file和xml文档对象关联
        QDomDocument doc;
        bool isOk = doc.setContent(&file);

        // 如果关联成功
        if (true == isOk)
        {
            // 获取根节点
            QDomElement root = doc.lastChildElement();

            file.close();

            // 获取当前时间
            QDateTime date = QDateTime::currentDateTime();
            QString dateStr = date.toString("yyyy--MM--dd");

            // 有没有子节点
            if (root.hasChildNodes())
            {
                // 找最后一个节点
                QDomElement lastEmt = root.lastChildElement();
                // 判断有没有当前日期
                if (lastEmt.attribute("date") == dateStr)
                {
                    // 找出当前日期下的所有时间子节点
                    QDomNodeList list = lastEmt.childNodes();

                    for (int i = 0; i < list.size(); i++)
                    {
                        // 转换为元素,找到时间节点下的所有子节点
                        QDomNodeList subList = list.at(i).toElement().childNodes();

                        // 厂家
                        QString factory = subList.at(0).toElement().text();
                        fList.append(factory);

                        QString brand = subList.at(1).toElement().text();
                        bList.append(brand);

                        QString price = subList.at(2).toElement().text();
                        pList.append(price);

                        QString num = subList.at(3).toElement().text();
                        nList.append(num);

                        QString total = subList.at(4).toElement().text();
                        tList.append(total);
                        // 自己另外加的
                    }
                }
                else
                {
                    cout << "没有当天日期";
                    return ;
                }

            }
            else
            {
                cout << "没有子节点";
                return ;
            }
        }
        else
        {
            // 养成一个习惯,作为出错判断
            cout << "Error setContent ";
        }
    }
    else
    {
        cout << "ReadOnly error";
        return ;
    }
}

DomXML::~DomXML()
{

}

