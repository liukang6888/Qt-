#ifndef DOMXML_H
#define DOMXML_H
#include <QString>
#include <QFile>
#include <QStringList> // 字符串列表
#include <QDomDocument>
#include <QDomElement>

class DomXML
{
public:
    DomXML();
    // 创建xml空文件
    static void createXML(QString filePath);

    // list.at(0) = 二汽神龙
    static void appendXML(QString filePath, QStringList list);

    // 写有效数据
    static void writeXML(QDomDocument &doc, QDomElement &root, QStringList &list);

    // 读取数据
    static void readXML(QString filePath,
                        QStringList  &fList,  // 厂家
                        QStringList  &bList, // 品牌
                        QStringList  &pList, // 价格
                        QStringList  &nList, // 数量
                        QStringList  &tList  // 总价
                        );

    ~DomXML();
};

#endif // DOMXML_H
