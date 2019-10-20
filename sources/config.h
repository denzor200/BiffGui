#ifndef CONFIG_H
#define CONFIG_H

#include <QString>
#include <QVariant>
#include <map>
#include <memory>

class ConfigStructure
{
public:

    class NotAvailable : public std::exception
    {
    public:
        NotAvailable() : std::exception("ConfigStructure is not available")
        {
        }
    };

    class NotRegistered : public std::exception
    {
        char Buffer[256];
    public:
        NotRegistered(const char* KeyName) : std::exception(MakeErrorString(KeyName))
        {
        }
    private:
        const char* MakeErrorString(const char* KeyName)
        {
            memset(Buffer, 0, sizeof(Buffer));
            strcat(Buffer, "Key '");
            strcat(Buffer, KeyName);
            strcat(Buffer, "' not registered in ConfigStructure");
            return Buffer;
        }
    };

private:
    const std::map<QString, QVariant> m_DefaultValues;

public:
    static std::weak_ptr<ConfigStructure> Instance()
    {
        static std::shared_ptr<ConfigStructure> inst(new ConfigStructure);
        return inst;
    }

    void MakeDefaultConfig() const;

    QVariant defaultValue(const QString& key) const;

    void setValue(const QString& key, const QVariant& value) const;
    QVariant value(const QString& key) const;

private:
    ConfigStructure();

};

namespace ConfigHelpers
{
    void setValue(const QString& key, const QVariant& value);

    QColor getColor(const QString& key);
    QString getString(const QString& key);
    bool getBoolean(const QString& key);
    double getDouble(const QString& key);
    int getInt(const QString& key);
}

#endif // CONFIG_H
