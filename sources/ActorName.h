#pragma once

// TODO: добавь расчет хэша.
// Чтобы unordered_set мог понимать эти объекты

#include <QString>

class ActorNameStringEmpty : public std::exception
{
public:
    ActorNameStringEmpty() : std::exception("ActorName can't be an empty string")
    {
    }
};

class ActorName
{
    QString m_Name;
public:
	ActorName() = default;
    explicit ActorName(const QString& Name);

	bool operator==(const ActorName& Other) const;
    bool operator!=(const ActorName& Other) const;
	bool operator<(const ActorName& Other) const;
    bool operator>(const ActorName& Other) const;
	bool operator<=(const ActorName& Other) const;
    bool operator>=(const ActorName& Other) const;

    const QString& Get() const { return m_Name; }

};

