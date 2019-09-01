#pragma once

// TODO: добавь расчет хэша.
// Чтобы unordered_set мог понимать эти объекты

#include <QString>

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
    bool Empty() const { return (m_Name.size() == 0); }
    void Clear() { m_Name.clear(); }

};

