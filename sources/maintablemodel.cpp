#include "maintablemodel.h"
#include <algorithm>
#include <limits>
#include <QDebug>

#define CHECK_COND(c) \
    if (!(c)) \
        return false;

#define CHECK_COND_THROW(c,e) \
    if (!(c)) \
        throw e();

#define TO_SZ(v) (static_cast<size_t>(v))

bool MainTableModelRegistry::ReserveNewPersonIndex()
{
    qDebug() << "[MainTableModelRegistry::ReserveNewPersonIndex]";
    CHECK_COND(m_Persons_ByID.size() < TO_SZ(std::numeric_limits<int>::max()));
    m_Persons_ByID.push_back(NULL_PERSON);
    return true;
}

bool MainTableModelRegistry::ReserveNewActorIndex()
{
    qDebug() << "[MainTableModelRegistry::ReserveNewActorIndex]";
    CHECK_COND(m_Actors_ByID.size() < TO_SZ(std::numeric_limits<int>::max()));
    m_Actors_ByID.push_back(NULL_ACTOR);
    return true;
}

bool MainTableModelRegistry::PersonIndexIsReserved(int ID) const
{
    qDebug() << "[MainTableModelRegistry::PersonIndexIsReserved]: " << ID;
    CHECK_COND_THROW(ID >= 0, MainTableModelRegistry_InvalidPersonID);
    if (TO_SZ(ID) < m_Persons_ByID.size())
    {
        return (m_Persons_ByID[TO_SZ(ID)] == NULL_PERSON);
    }
    throw MainTableModelRegistry_InvalidPersonID();
}

bool MainTableModelRegistry::ActorIndexIsReserved(int ID) const
{
    qDebug() << "[MainTableModelRegistry::ActorIndexIsReserved]: " << ID;
    CHECK_COND_THROW(ID >= 0, MainTableModelRegistry_InvalidActorID);
    if (TO_SZ(ID) < m_Actors_ByID.size())
    {
        return (m_Actors_ByID[TO_SZ(ID)] == NULL_ACTOR);
    }
    throw MainTableModelRegistry_InvalidActorID();
}

bool MainTableModelRegistry::AddPerson(const ActorName& person, int ID)
{
    qDebug() << "[MainTableModelRegistry::AddPerson]: " << person << ", " << ID;
    CHECK_COND(ID < 0 || m_Persons_ByID[TO_SZ(ID)] == NULL_PERSON);
    CHECK_COND(m_Persons_ByID.size() < TO_SZ(std::numeric_limits<int>::max()));
    if (m_Persons_ByName.find(person) == m_Persons_ByName.end())
    {
        Person NewObj;
        NewObj.name = person;
        m_Persons.push_front(std::move(NewObj));

        if (ID < 0)
        {
            try {
                m_Persons_ByID.push_back(m_Persons.begin());
            } catch (...) {
                m_Persons.pop_front();
                throw ;
            }
            try {
                m_Persons_ByName[person] = m_Persons.begin();
            } catch (...) {
                m_Persons_ByID.pop_back();
                m_Persons.pop_front();
                throw;
            }
        }
        else { // ID < 0
            m_Persons_ByID[TO_SZ(ID)] = m_Persons.begin();
            try {
                m_Persons_ByName[person] = m_Persons.begin();
            } catch (...) {
                m_Persons_ByID[TO_SZ(ID)] = NULL_PERSON;
                m_Persons.pop_front();
                throw;
            }

        } // ID < 0
        return true;
    }
    return false;
}


bool MainTableModelRegistry::AddActor(const ActorName& actor, int ID)
{
    qDebug() << "[MainTableModelRegistry::AddActor]: " << actor << ", " << ID;
    CHECK_COND(ID < 0 || m_Actors_ByID[TO_SZ(ID)] == NULL_ACTOR);
    CHECK_COND(m_Actors_ByID.size() < TO_SZ(std::numeric_limits<int>::max()));
    if (m_Actors_ByName.find(actor) == m_Actors_ByName.end())
    {
        Actor NewObj;
        NewObj.name = actor;
        m_Actors.push_front(std::move(NewObj));

        if (ID < 0)
        {
            try {
                m_Actors_ByID.push_back(m_Actors.begin());
            } catch (...) {
                m_Actors.pop_front();
                throw ;
            }
            try {
                m_Actors_ByName[actor] = m_Actors.begin();
            } catch (...) {
                m_Actors_ByID.pop_back();
                m_Actors.pop_front();
                throw;
            }
        }
        else { // ID < 0
            m_Actors_ByID[TO_SZ(ID)] = m_Actors.begin();
            try {
                m_Actors_ByName[actor] = m_Actors.begin();
            } catch (...) {
                m_Actors_ByID[TO_SZ(ID)] = NULL_ACTOR;
                m_Actors.pop_front();
                throw;
            }

        } // ID < 0
        return true;
    }
    return false;
}


bool MainTableModelRegistry::RenamePerson(int ID, const ActorName& newName)
{
    qDebug() << "[MainTableModelRegistry::RenamePerson]: " << ID << ", " << newName;
    CHECK_COND(ID >= 0);
    if (TO_SZ(ID) < m_Persons_ByID.size())
    {
        auto it = m_Persons_ByID[TO_SZ(ID)];
        if (it != m_Persons.end())
        {
            ActorName OldName = it->name;
            auto inserted = m_Persons_ByName.insert({newName, it});
            if (inserted.second)
            {
                try {
                    it->name = newName;
                } catch (...) {
                    m_Persons_ByName.erase(inserted.first);
                    throw;
                }
                m_Persons_ByName.erase(OldName);
                return true;
            }
        }
    }
    return false;
}

bool MainTableModelRegistry::RenameActor(int ID, const ActorName& newName)
{
    qDebug() << "[MainTableModelRegistry::RenameActor]: " << ID << ", " << newName;
    CHECK_COND(ID >= 0);
    if (TO_SZ(ID) < m_Actors_ByID.size())
    {
        auto it = m_Actors_ByID[TO_SZ(ID)];
        if (it != m_Actors.end())
        {
            ActorName OldName = it->name;
            auto inserted = m_Actors_ByName.insert({newName, it});
            if (inserted.second)
            {
                try {
                    it->name = newName;
                } catch (...) {
                    m_Actors_ByName.erase(inserted.first);
                    throw;
                }
                m_Actors_ByName.erase(OldName);
                return true;
            }
        }
    }
    return false;
}


// internal method
bool MainTableModelRegistry::Person_ChangeRelation(PersonsList::iterator personIt, ActorsList::iterator actorIt, bool State)
{
    return Actor_ChangeRelation(actorIt, personIt, State);
}

// internal method
bool MainTableModelRegistry::Actor_ChangeRelation(ActorsList::iterator actorIt, PersonsList::iterator personIt, bool State)
{
    if (State)
    {
        std::vector<PersonsList::iterator> TempActorPersons = actorIt->persons;
        std::vector<ActorsList::iterator> TempPersonActors = personIt->actors;
        if (std::find(TempActorPersons.begin(),TempActorPersons.end(), personIt)==TempActorPersons.end())
            TempActorPersons.push_back(personIt);
        if (std::find(TempPersonActors.begin(),TempPersonActors.end(), actorIt)==TempPersonActors.end())
            TempPersonActors.push_back(actorIt);
        std::swap(actorIt->persons, TempActorPersons);
        std::swap(personIt->actors, TempPersonActors);
    }
    else {
        actorIt->persons.erase(
                    std::remove(actorIt->persons.begin(), actorIt->persons.end(), personIt),
                    actorIt->persons.end());
        personIt->actors.erase(
                    std::remove(personIt->actors.begin(), personIt->actors.end(), actorIt),
                    personIt->actors.end());
    }
    return true;
}


bool MainTableModelRegistry::Person_ChangeRelation(int personID, const ActorName& actor, bool State)
{
    qDebug() << "[MainTableModelRegistry::Person_ChangeRelation]: " << personID << ", " << actor << ", " << State;
    CHECK_COND(personID >= 0);
    auto actorIt = m_Actors_ByName.find(actor);
    if (actorIt != m_Actors_ByName.end() && TO_SZ(personID) < m_Persons_ByID.size())
    {
        PersonsList::iterator personIt = m_Persons_ByID[TO_SZ(personID)];
        if (personIt != m_Persons.end())
            return Person_ChangeRelation(personIt, actorIt->second, State);
    }
    return false;
}
bool MainTableModelRegistry::Actor_ChangeRelation(int actorID, const ActorName& person, bool State)
{
    qDebug() << "[MainTableModelRegistry::Actor_ChangeRelation]: " << actorID << ", " << person << ", " << State;
    CHECK_COND(actorID >= 0);
    auto personIt = m_Persons_ByName.find(person);
    if (personIt != m_Persons_ByName.end() && TO_SZ(actorID) < m_Actors_ByID.size())
    {
        ActorsList::iterator actorIt = m_Actors_ByID[TO_SZ(actorID)];
        if (actorIt != m_Actors.end())
            return Actor_ChangeRelation(actorIt, personIt->second, State);
    }
    return false;
}

#define DISABLE_DEBUG_GETTERS
#ifndef DISABLE_DEBUG_GETTERS
#define GETTER_DEBUG qDebug
#else
#define GETTER_DEBUG QT_NO_QDEBUG_MACRO
#endif // DISABLE_DEBUG_GETTERS

// TODO: геттеры слишком жирные
// нужна общая функция, которая выполняла бы сама все нужные проверки
QString MainTableModelRegistry::PersonGetName(int ID) const
{
    GETTER_DEBUG() << "[MainTableModelRegistry::PersonGetName]: " << ID;
    CHECK_COND_THROW(ID >= 0, MainTableModelRegistry_InvalidPersonID);
    if (TO_SZ(ID) < m_Persons_ByID.size())
    {
        PersonsList::iterator findedItValue = m_Persons_ByID[TO_SZ(ID)];
        if (findedItValue != m_Persons.end())
            return findedItValue->name.Get();
    }
    throw MainTableModelRegistry_InvalidPersonID();
}

QString MainTableModelRegistry::ActorGetName(int ID) const
{
    GETTER_DEBUG() << "[MainTableModelRegistry::ActorGetName]: " << ID;
    CHECK_COND_THROW(ID >= 0, MainTableModelRegistry_InvalidActorID);
    if (TO_SZ(ID) < m_Actors_ByID.size())
    {
        ActorsList::iterator findedItValue = m_Actors_ByID[TO_SZ(ID)];
        if (findedItValue != m_Actors.end())
            return findedItValue->name.Get();
    }
    throw MainTableModelRegistry_InvalidActorID();
}

QList<QString> MainTableModelRegistry::PersonGetActors(int ID) const
{
    GETTER_DEBUG() << "[MainTableModelRegistry::PersonGetActors]: " << ID;
    CHECK_COND_THROW(ID >= 0, MainTableModelRegistry_InvalidPersonID);
    if (TO_SZ(ID) < m_Persons_ByID.size())
    {
        PersonsList::iterator findedItValue = m_Persons_ByID[TO_SZ(ID)];
        if (findedItValue != m_Persons.end())
        {
            QList<QString> Actors;
            Q_ASSERT(findedItValue->actors.size() < TO_SZ(std::numeric_limits<int>::max()));
            Actors.reserve(static_cast<int>(findedItValue->actors.size()));
            for (ActorsList::iterator it : findedItValue->actors)
            {
               Actors.push_back(it->name.Get());
            }
            return Actors;
        }
    }
    throw MainTableModelRegistry_InvalidPersonID();
}

QList<QString> MainTableModelRegistry::ActorGetPersons(int ID) const
{
    GETTER_DEBUG() << "[MainTableModelRegistry::ActorGetPersons]: " << ID;
    CHECK_COND_THROW(ID >= 0, MainTableModelRegistry_InvalidActorID);
    if (TO_SZ(ID) < m_Actors_ByID.size())
    {
        ActorsList::iterator findedItValue = m_Actors_ByID[TO_SZ(ID)];
        if (findedItValue != m_Actors.end())
        {
            QList<QString> Persons;
            Q_ASSERT(findedItValue->persons.size() < TO_SZ(std::numeric_limits<int>::max()));
            Persons.reserve(static_cast<int>(findedItValue->persons.size()));
            for (PersonsList::iterator it : findedItValue->persons)
            {
               Persons.push_back(it->name.Get());
            }
            return Persons;
        }
    }
    throw MainTableModelRegistry_InvalidActorID();
}

bool MainTableModelRegistry::PersonIsDenied(int ID) const
{
    GETTER_DEBUG() << "[MainTableModelRegistry::PersonIsDenied]: " << ID;
    CHECK_COND_THROW(ID >= 0, MainTableModelRegistry_InvalidPersonID);
    if (TO_SZ(ID) < m_Persons_ByID.size())
    {
        PersonsList::iterator findedItValue = m_Persons_ByID[TO_SZ(ID)];
        if (findedItValue != m_Persons.end())
            return findedItValue->deny;
    }
    throw MainTableModelRegistry_InvalidPersonID();
}

bool MainTableModelRegistry::ActorIsDenied(int ID) const
{
    GETTER_DEBUG() << "[MainTableModelRegistry::ActorIsDenied]: " << ID;
    CHECK_COND_THROW(ID >= 0, MainTableModelRegistry_InvalidActorID);
    if (TO_SZ(ID) < m_Actors_ByID.size())
    {
        ActorsList::iterator findedItValue = m_Actors_ByID[TO_SZ(ID)];
        if (findedItValue != m_Actors.end())
            return findedItValue->deny;
    }
    throw MainTableModelRegistry_InvalidActorID();
}

bool MainTableModelRegistry::RemovePerson(int ID) noexcept
{
    qDebug() << "[MainTableModelRegistry::RemovePerson]: " << ID;
    CHECK_COND(ID >= 0);
    if (TO_SZ(ID) < m_Persons_ByID.size())
    {
        PersonsList::iterator findedItValue = m_Persons_ByID[TO_SZ(ID)];
        if (findedItValue != m_Persons.end())
        {
            // Снимаем все ссылки на конкретный элемент
            // Нет смысла перебирать все значения из m_Persons, достаточно перебрать только свои
            for (ActorsList::iterator Value : findedItValue->actors)
            {
                Q_ASSERT(Value != m_Actors.end());
                // TODO: можно ли вызывать std::remove на std::set??
                Value->persons.erase(
                            std::remove(Value->persons.begin(), Value->persons.end(), findedItValue),
                            Value->persons.end());
            }

            // Удаляем непосредственно из хранилищ
            m_Persons_ByID.erase(
                        std::remove(m_Persons_ByID.begin(), m_Persons_ByID.end(), findedItValue),
                        m_Persons_ByID.end());
            m_Persons_ByName.erase(findedItValue->name);
            m_Persons.erase(findedItValue);
            return true;
        }
    }
    return false;
}

bool MainTableModelRegistry::RemoveActor(int ID) noexcept
{
    qDebug() << "[MainTableModelRegistry::RemoveActor]: " << ID;
    CHECK_COND(ID >= 0);
    if (TO_SZ(ID) < m_Actors_ByID.size())
    {
        ActorsList::iterator findedItValue = m_Actors_ByID[TO_SZ(ID)];
        if (findedItValue != m_Actors.end())
        {
            // Снимаем все ссылки на конкретный элемент
            // Нет смысла перебирать все значения из m_Persons, достаточно перебрать только свои
            for (PersonsList::iterator Value : findedItValue->persons)
            {
                Q_ASSERT(Value != m_Persons.end());
                // TODO: можно ли вызывать std::remove на std::set??
                Value->actors.erase(
                            std::remove(Value->actors.begin(), Value->actors.end(), findedItValue),
                            Value->actors.end());
            }

            // Удаляем непосредственно из хранилищ
            m_Actors_ByID.erase(
                        std::remove(m_Actors_ByID.begin(), m_Actors_ByID.end(), findedItValue),
                        m_Actors_ByID.end());
            m_Actors_ByName.erase(findedItValue->name);
            m_Actors.erase(findedItValue);
            return true;
        }
    }
    return false;
}

void MainTableModelRegistry::ClearAllPersons() noexcept
{
    qDebug() << "[MainTableModelRegistry::ClearAllPersons]";
    // Снимаем все возможные ссылки на персонажей
    for (Actor& Value : m_Actors)
    {
        Value.persons.clear();
    }
    // Очищаем все хранилища
    m_Persons_ByID.clear();
    m_Persons_ByName.clear();
    m_Persons.clear();
}

void MainTableModelRegistry::ClearAllActors() noexcept
{
    qDebug() << "[MainTableModelRegistry::ClearAllActors]";
    // Снимаем все возможные ссылки на актеров
    for (Person& Value : m_Persons)
    {
        Value.actors.clear();
    }
    // Очищаем все хранилища
    m_Actors_ByID.clear();
    m_Actors_ByName.clear();
    m_Actors.clear();
}

// Актер как ключ
MainTableModel::MainTableModel( QObject *parent) :
    QAbstractTableModel(parent),
    m_Mngr(qobject_cast<MainTableModelsManager*>(parent))
{
    Q_ASSERT(m_Mngr);
}

int MainTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_Mngr->GetRegistry()->getActorsCount();
}

int MainTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 3;
}

QVariant MainTableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() ||
      m_Mngr->GetRegistry()->getActorsCount() <= index.row() ||
      ( role != Qt::DisplayRole && role != Qt::EditRole )
        ) {
            return QVariant();
        }

    try {
        switch (index.column())
        {
        case 0:
            return m_Mngr->GetRegistry()->ActorGetName(index.row());
        case 1:
        {
            // TODO: refact it
            QString ret;
            auto v=m_Mngr->GetRegistry()->ActorGetPersons(index.row());
            for (auto t:v)
            {
                ret += t + ", ";
            }
            return ret;
        }
        case 2:
            return m_Mngr->GetRegistry()->ActorIsDenied(index.row());
        }
    }
    catch (const MainTableModelRegistry_InvalidActorID&)
    {
    }
    return QVariant();
}

Qt::ItemFlags MainTableModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QAbstractTableModel::flags( index );
    if( index.isValid() ) {
        flags |= Qt::ItemIsEditable;
    }
    return flags;
}

bool MainTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if( !index.isValid() || role != Qt::EditRole || m_Mngr->GetRegistry()->getActorsCount() <= index.row() ) {
        return false;
    }

    bool Changed = false;
    switch (index.column())
    {
    case 0:
        if (m_Other)
            m_Other->beginResetModel();
        try {
            // Возвращаемое значение от SetActor можно игнорировать
            MainTableModelUtils::SetActor(*m_Mngr->GetRegistry(), index.row(), ActorName(value.toString()));
            Changed = true;
        }
        catch (const ActorNameStringEmpty&) {
        }
        catch (const MainTableModelRegistry_InvalidActorID&) {
        }
    }
    if (Changed)
    {
        emit dataChanged( index, index );
        if (m_Other)
            m_Other->endResetModel();
        return true;
    }
    return false;
}

QVariant MainTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if( role != Qt::DisplayRole ) {
            return QVariant();
        }

        if( orientation == Qt::Vertical ) {
            return section;
        }

        switch( section ) {
        case 0:
            return QString::fromUtf8( "Актер" );
        case 1:
            return QString::fromUtf8( "Персонажи" );
        case 2:
            return QString::fromUtf8( "Исключить" );
        }

        return QVariant();
}


// Персонаж как ключ
MainTableModel_Reversed::MainTableModel_Reversed(QObject *parent) :
    QAbstractTableModel(parent),
    m_Mngr(qobject_cast<MainTableModelsManager*>(parent))
{
    Q_ASSERT(m_Mngr);
}

int MainTableModel_Reversed::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_Mngr->GetRegistry()->getPersonsCount();
}

int MainTableModel_Reversed::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 3;
}

QVariant MainTableModel_Reversed::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() ||
      m_Mngr->GetRegistry()->getPersonsCount() <= index.row() ||
      ( role != Qt::DisplayRole && role != Qt::EditRole )
        ) {
            return QVariant();
        }

    try {
        switch (index.column())
        {
        case 0:
            return m_Mngr->GetRegistry()->PersonGetName(index.row());
        case 1:
        {
            // TODO: refact it
            QString ret;
            auto v=m_Mngr->GetRegistry()->PersonGetActors(index.row());
            for (auto t:v)
            {
                ret += t + ", ";
            }
            return ret;
        }
        case 2:
            return m_Mngr->GetRegistry()->PersonIsDenied(index.row());
        }
    }
    catch (const MainTableModelRegistry_InvalidPersonID&)
    {
    }
    return QVariant();
}

bool MainTableModel_Reversed::setData(const QModelIndex &index, const QVariant &value, int role)
{
    // TODO: implement this
    return false;
}

QVariant MainTableModel_Reversed::headerData(int section, Qt::Orientation orientation, int role) const
{
    if( role != Qt::DisplayRole ) {
            return QVariant();
        }

        if( orientation == Qt::Vertical ) {
            return section;
        }

        switch( section ) {
        case 0:
            return QString::fromUtf8( "Персонаж" );
        case 1:
            return QString::fromUtf8( "Актеры" );
        case 2:
            return QString::fromUtf8( "Исключить" );
        }

        return QVariant();
}

MainTableModelsManager::MainTableModelsManager(QObject *parent) :
    QObject(parent)
{
    m_Model = new MainTableModel(this);
    m_ModelReversed = new MainTableModel_Reversed(this);
    m_Model->SetOther(m_ModelReversed);
    m_ModelReversed->SetOther(m_Model);

   // Testing..
   m_Registry.AddActor(ActorName("Черсков"));
   m_Registry.AddActor(ActorName("Корш"));

   m_Registry.AddPerson(ActorName("Персонаж 1"));
   m_Registry.AddPerson(ActorName("Персонаж 2"));
   m_Registry.AddPerson(ActorName("Персонаж 3"));
   m_Registry.AddPerson(ActorName("Персонаж 4"));

   m_Registry.Actor_ChangeRelation(0, ActorName("Персонаж 1"), true);
   m_Registry.Actor_ChangeRelation(0, ActorName("Персонаж 2"), true);

   m_Registry.Actor_ChangeRelation(1, ActorName("Персонаж 2"), true);
   m_Registry.Actor_ChangeRelation(1, ActorName("Персонаж 3"), true);
}

bool MainTableModelsManager::PersonsInsertRow()
{
    int row = m_Registry.getPersonsCount();
    m_ModelReversed->beginInsertRows( QModelIndex(), row, row );
    if (m_Registry.ReserveNewPersonIndex())
    {
        m_ModelReversed->endInsertRows();
        return true;
    }
    return false;
}

bool MainTableModelsManager::ActorsInsertRow()
{
    int row = m_Registry.getActorsCount();
    m_Model->beginInsertRows( QModelIndex(), row, row );
    if (m_Registry.ReserveNewActorIndex())
    {
        m_Model->endInsertRows();
        return true;
    }
    return false;
}

bool MainTableModelsManager::PersonsRemoveRow(int row)
{
    CHECK_COND(row < m_Registry.getPersonsCount());
    m_ModelReversed->beginRemoveRows( QModelIndex(), row, row );
    m_Model->beginResetModel();
    if (m_Registry.RemovePerson(row))
    {
        m_ModelReversed->endRemoveRows();
        m_Model->endResetModel();
        return true;
    }
    return false;
}

bool MainTableModelsManager::ActorsRemoveRow(int row)
{
    CHECK_COND(row < m_Registry.getActorsCount());
    m_Model->beginRemoveRows( QModelIndex(), row, row );
    m_ModelReversed->beginResetModel();
    if (m_Registry.RemoveActor(row))
    {
        m_Model->endRemoveRows();
        m_ModelReversed->endResetModel();
        return true;
    }
    return false;
}

bool MainTableModelUtils::SetPerson(MainTableModelRegistry &R, int ID, const ActorName &Name)
{
    if (R.PersonIndexIsReserved(ID))
        return R.AddPerson(Name, ID);
    else {
        return R.RenamePerson(ID, Name);
    }
}

bool MainTableModelUtils::SetActor(MainTableModelRegistry &R, int ID, const ActorName &Name)
{
    if (R.ActorIndexIsReserved(ID))
        return R.AddActor(Name, ID);
    else {
        return R.RenameActor(ID, Name);
    }
}
