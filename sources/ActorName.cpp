#include "ActorName.h"

ActorName::ActorName(const QString& Name)
    : m_Name(Name.trimmed())
{
}

// 1. trim(проходит на более ранней стадии)
// 2. upper
// Ну а что еще для безопасного сравнения имен нужно??

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

