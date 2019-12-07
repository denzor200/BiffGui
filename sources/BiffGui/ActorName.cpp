#include "ActorName.h"
#include <QMessageBox>
#include <QApplication>
#include <sstream>

// Все что нужно для безопасного хранения и сравнения имен
// 1. trim
// 2. Проверка на пустоту
// 3. Проверка на наличие запрещенных символов
// 4. upper

ActorName::ActorName(const QString& Name)
    : m_Name(Name.trimmed())
{
    if (m_Name.size()==0)
        throw ActorNameStringEmpty();
    if (FindForbidenSymbols(m_Name))
        throw ActorNameForbiddenSymbols();
}

bool ActorName::operator==(const ActorName& Other) const
{
    return m_Name.toUpper() == Other.m_Name.toUpper();
}

bool ActorName::operator!=(const ActorName& Other) const
{
    return m_Name.toUpper() != Other.m_Name.toUpper();
}

bool ActorName::operator<(const ActorName& Other) const
{
    return m_Name.toUpper() < Other.m_Name.toUpper();
}

bool ActorName::operator>(const ActorName& Other) const
{
    return m_Name.toUpper() > Other.m_Name.toUpper();
}

bool ActorName::operator<=(const ActorName& Other) const
{
    return m_Name.toUpper() <= Other.m_Name.toUpper();
}

bool ActorName::operator>=(const ActorName& Other) const
{
    return m_Name.toUpper() >= Other.m_Name.toUpper();
}

bool ActorName::FindForbidenSymbols(const QString &Name)
{
    if (Name.indexOf(',') != -1 || Name.indexOf('-') != -1 || Name.indexOf('"') != -1)
        return true;
    return false;
}

void ActorName::ShowForbidenSymbolsError(const QString& header)
{
    std::stringstream ss;
    ss << "Имя не должно содержать следующих знаков:" << std::endl;
    ss << "\t\t- , \"";
    if (header.size() !=0 )
        ss << std::endl << header.toUtf8().data();
    QMessageBox::warning(QApplication::activeWindow(), "Предупреждение", ss.str().c_str());
}

