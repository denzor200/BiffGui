#ifndef XMLSMARTKEYSPROVIDER_H
#define XMLSMARTKEYSPROVIDER_H

#include <QList>

class QDomNode;
class QDomDocument;
class QXmlStreamWriter;

class XmlSmartKeysProvider
{
public:
    // Все эти функции в качестве параметра Path должны принимать только строки-константы,
    //    значение которых известно на этапе компиляции
    // В противном случае есть риск нарваться на stack overflow
    static QList<QDomNode> elementsByTagPath(const QDomDocument &doc, const QString &Path);
    static void writeTextElement(QDomDocument& domDoc, const QString &Path, const QStringList& Values);

};

#endif // XMLSMARTKEYSPROVIDER_H
