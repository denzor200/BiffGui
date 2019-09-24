#pragma once

#include <QString>
#include <QHash>
#include <QDebug>

class ActorNameException : public std::exception
{
protected:
    ActorNameException(const char* what) : std::exception(what)
    {
    }
};

class ActorNameStringEmpty : public ActorNameException
{
public:
    ActorNameStringEmpty() : ActorNameException("ActorName can't be an empty string")
    {
    }
};

class ActorNameForbiddenSymbols : public ActorNameException
{
public:
    ActorNameForbiddenSymbols() : ActorNameException("ActorName must not contain characters 'dash' and 'comma'")
    {
    }
};

class ActorName
{
    QString m_Name;
public:
    explicit ActorName(const QString& Name);

	bool operator==(const ActorName& Other) const;
    bool operator!=(const ActorName& Other) const;
	bool operator<(const ActorName& Other) const;
    bool operator>(const ActorName& Other) const;
	bool operator<=(const ActorName& Other) const;
    bool operator>=(const ActorName& Other) const;

    static bool FindForbidenSymbols(const QString& Name);
    static void ShowForbidenSymbolsError(const QString& header);

    const QString& Get() const { return m_Name; }

};

inline uint qHash(const ActorName &key, uint seed = 0)
{
    return qHash(key.Get().toUpper(), seed);
}

// for std::unordered_map in MainTableModel
namespace std {
template<>
class hash<ActorName> {
public:
    size_t operator()(const ActorName &key) const
    {
        return static_cast<size_t>(qHash(key));
    }
};
}

inline QDebug & operator<< (QDebug & os, const ActorName& Value)
{
    os << Value.Get();
    return os;
}
