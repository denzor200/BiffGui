#include "xmlsmartkeysprovider.h"
#include <QtXml>

template <typename T>
static QList<QDomNode> elementsByTagPathRecursive(const T& doc, QStringList names)
{
    QList<QDomNode> Returned;
    Q_ASSERT(!names.empty());
    QString name = names[0];
    QDomNodeList nodes = doc.elementsByTagName(name);
    if (names.size()==1)
    {
        for (int x = 0; x < nodes.count(); x++)
        {
            QDomElement node = nodes.at(x).toElement();
            Returned += node;
        }
    }
    else {
        names.pop_front();
        for (int x = 0; x < nodes.count(); x++)
        {
            QDomElement node = nodes.at(x).toElement();
            Returned += elementsByTagPathRecursive(node, names);
        }
    }
    return Returned;
}

template <typename T>
static void writeTextElementRecursive(QDomDocument& doc, T &prev, QStringList &names, const QStringList &Values)
{
    Q_ASSERT(!names.empty());

    // Конкретно в моей модели конфига разрешается дублировать по ключу листья xml-дерева
    // Это например может потребоваться чтобы сохранить список из текстовых значений
    // Например, смотрите такой параметр как Root.AssParsing.NeededColumns.*

    // Все остальные ветви дублировать нежелательно

    if (names.size() == 1)
    {
        QDomElement node;
        for (const QString& value : Values)
        {
            node = doc.createElement(names[0]);
            prev.appendChild(node);

            QDomText t = doc.createTextNode(value);
            node.appendChild(t);
        }
    }
    else {
        QDomElement node;
        QDomNodeList nodes = prev.elementsByTagName(names[0]);
        if (nodes.size()==0)
        {
            node = doc.createElement(names[0]);
            prev.appendChild(node);
        }
        else {
            Q_ASSERT(nodes.size() == 1);
            node = nodes.at(0).toElement();
            //doc.removeChild(node);
        }
        names.pop_front();
        writeTextElementRecursive(doc, node, names, Values);
    }
}

QList<QDomNode> XmlSmartKeysProvider::elementsByTagPath(const QDomDocument& doc, const QString& Path)
{
    //qDebug() << "elementsByTagPath: " << Path;
    QStringList names = Path.split(".");
    return elementsByTagPathRecursive<QDomDocument>(doc, names);
}

void XmlSmartKeysProvider::writeTextElement(QDomDocument& domDoc, const QString &Path, const QStringList &Values)
{
    //qDebug() << "writeTextElement: " << Path;
    QStringList names = Path.split(".");
    writeTextElementRecursive(domDoc, domDoc, names, Values);
}
