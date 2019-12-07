#include "config.h"
#include "settingsregistry.h"
#include <QColor>
#include <QSettings>

ConfigStructure::ConfigStructure() :
    m_DefaultValues({
        {SettingsRegistry::BUSY_ROLES_FONT, "Arial"},
        {SettingsRegistry::BUSY_ROLES_SIZE, 9},
        {SettingsRegistry::BUSY_ROLES_COLOR, QColor(Qt::blue)},
        {SettingsRegistry::BUSY_ROLES_HIGHLIGHT, QColor(Qt::white)},
        {SettingsRegistry::BUSY_ROLES_BOLD, true},
        {SettingsRegistry::BUSY_ROLES_ITALIC, true},
        {SettingsRegistry::BUSY_ROLES_UNDERLINE, false}
    })
{
}

QVariant ConfigStructure::defaultValue(const QString &key) const
{
    auto it = m_DefaultValues.find(key);
    if (it != m_DefaultValues.end())
        return it->second;
    throw NotRegistered(key.toUtf8().data());
}

// Откат физического хранилища параметров к значениям по умолчанию
void ConfigStructure::MakeDefaultConfig() const
{
    for (const auto& pair : m_DefaultValues)
    {
        QSettings().setValue(pair.first,pair.second);
    }
}

void ConfigStructure::setValue(const QString &key, const QVariant &value) const
{
#ifdef _DEBUG
    defaultValue(key);
#endif // _DEBUG
    QSettings().setValue(key,value);
}

QVariant ConfigStructure::value(const QString &key) const
{
    QVariant def = defaultValue(key);
    return QSettings().value(key, def);
}

void ConfigHelpers::MakeDefaultConfig()
{
    if (auto inst = ConfigStructure::Instance().lock())
    {
        inst->MakeDefaultConfig();
        return;
    }
    throw ConfigStructure::NotAvailable();
}

void ConfigHelpers::setValue(const QString &key, const QVariant &value)
{
    if (auto inst = ConfigStructure::Instance().lock())
    {
        inst->setValue(key,value);
        return;
    }
    throw ConfigStructure::NotAvailable();
}

template <typename T>
static T GetValue(const QString &key)
{
    if (auto inst = ConfigStructure::Instance().lock())
    {
        QVariant def = inst->defaultValue(key);
        QVariant value = inst->value(key);
        if (value.canConvert<T>())
            return value.value<T>();
        return def.value<T>();
    }
    throw ConfigStructure::NotAvailable();
}

QColor ConfigHelpers::getColor(const QString &key)
{
    return GetValue<QColor>(key);
}

QString ConfigHelpers::getString(const QString &key)
{
    return GetValue<QString>(key);
}

bool ConfigHelpers::getBoolean(const QString &key)
{
    return GetValue<bool>(key);
}

double ConfigHelpers::getDouble(const QString &key)
{
    return GetValue<double>(key);
}

int ConfigHelpers::getInt(const QString &key)
{
    return GetValue<int>(key);
}
