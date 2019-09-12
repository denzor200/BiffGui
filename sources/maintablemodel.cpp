#include "maintablemodel.h"
#include <algorithm>
#include <limits>

bool MainTableModelRegistry::AddPerson(const ActorName& person)
{
    if (m_Persons_ByName.find(person) == m_Persons_ByName.end())
    {
        Person NewObj;
        NewObj.name = person;
        m_Persons.push_front(std::move(NewObj));
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
        return true;
    }
    return false;
}

bool MainTableModelRegistry::AddActor(const ActorName& actor)
{
    if (m_Actors_ByName.find(actor) == m_Actors_ByName.end())
    {
        Actor NewObj;
        NewObj.name = actor;
        m_Actors.push_front(std::move(NewObj));
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
        return true;
    }
    return false;
}

bool MainTableModelRegistry::RenamePerson(size_t ID, const ActorName& newName)
{
    if (ID < m_Persons_ByID.size())
    {
        auto it = m_Persons_ByID[ID];
        Q_ASSERT(it != m_Persons.end());

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
    return false;
}

bool MainTableModelRegistry::RenameActor(size_t ID, const ActorName& newName)
{
    if (ID < m_Actors_ByID.size())
    {
        auto it = m_Actors_ByID[ID];
        Q_ASSERT(it != m_Actors.end());

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
        // TODO: можно ли вызывать std::remove на std::set??
        actorIt->persons.erase(
                    std::remove(actorIt->persons.begin(), actorIt->persons.end(), personIt),
                    actorIt->persons.end());
        personIt->actors.erase(
                    std::remove(personIt->actors.begin(), personIt->actors.end(), actorIt),
                    personIt->actors.end());
    }
    return true;
}


bool MainTableModelRegistry::Person_ChangeRelation(size_t personID, const ActorName& actor, bool State)
{
    auto actorIt = m_Actors_ByName.find(actor);
    if (actorIt != m_Actors_ByName.end() && personID < m_Persons_ByID.size())
    {
        PersonsList::iterator personIt = m_Persons_ByID[personID];
        Q_ASSERT(personIt != m_Persons.end());

        return Person_ChangeRelation(personIt, actorIt->second, State);
    }
    return false;
}
bool MainTableModelRegistry::Actor_ChangeRelation(size_t actorID, const ActorName& person, bool State)
{
    auto personIt = m_Persons_ByName.find(person);
    if (personIt != m_Persons_ByName.end() && actorID < m_Actors_ByID.size())
    {
        ActorsList::iterator actorIt = m_Actors_ByID[actorID];
        Q_ASSERT(actorIt != m_Actors.end());

        return Actor_ChangeRelation(actorIt, personIt->second, State);
    }
    return false;
}

ActorName MainTableModelRegistry::PersonGetName(size_t ID) const
{
    if (ID < m_Persons_ByID.size())
    {
        PersonsList::iterator findedItValue = m_Persons_ByID[ID];
        Q_ASSERT(findedItValue != m_Persons.end());

        return findedItValue->name;
    }
    throw MainTableModelRegistry_InvalidPersonID();
}

ActorName MainTableModelRegistry::ActorGetName(size_t ID) const
{
    if (ID < m_Actors_ByID.size())
    {
        ActorsList::iterator findedItValue = m_Actors_ByID[ID];
        Q_ASSERT(findedItValue != m_Actors.end());

        return findedItValue->name;
    }
    throw MainTableModelRegistry_InvalidActorID();
}

std::vector<ActorName> MainTableModelRegistry::PersonGetActors(size_t ID) const
{
    if (ID < m_Persons_ByID.size())
    {
        PersonsList::iterator findedItValue = m_Persons_ByID[ID];
        Q_ASSERT(findedItValue != m_Persons.end());

        std::vector<ActorName> Actors;
        Actors.reserve(findedItValue->actors.size());
        for (ActorsList::iterator it : findedItValue->actors)
        {
           Actors.push_back(it->name);
        }
        return Actors;
    }
    throw MainTableModelRegistry_InvalidPersonID();
}

std::vector<ActorName> MainTableModelRegistry::ActorGetPersons(size_t ID) const
{
    if (ID < m_Actors_ByID.size())
    {
        ActorsList::iterator findedItValue = m_Actors_ByID[ID];
        Q_ASSERT(findedItValue != m_Actors.end());

        std::vector<ActorName> Persons;
        Persons.reserve(findedItValue->persons.size());
        for (PersonsList::iterator it : findedItValue->persons)
        {
           Persons.push_back(it->name);
        }
        return Persons;
    }
    throw MainTableModelRegistry_InvalidActorID();
}

bool MainTableModelRegistry::PersonIsDenied(size_t ID) const
{
    if (ID < m_Persons_ByID.size())
    {
        PersonsList::iterator findedItValue = m_Persons_ByID[ID];
        Q_ASSERT(findedItValue != m_Persons.end());

        return findedItValue->deny;
    }
    throw MainTableModelRegistry_InvalidPersonID();
}

bool MainTableModelRegistry::ActorIsDenied(size_t ID) const
{
    if (ID < m_Actors_ByID.size())
    {
        ActorsList::iterator findedItValue = m_Actors_ByID[ID];
        Q_ASSERT(findedItValue != m_Actors.end());

        return findedItValue->deny;
    }
    throw MainTableModelRegistry_InvalidActorID();
}

bool MainTableModelRegistry::RemovePerson(size_t ID) noexcept
{
    if (ID < m_Persons_ByID.size())
    {
        PersonsList::iterator findedItValue = m_Persons_ByID[ID];
        Q_ASSERT(findedItValue != m_Persons.end());

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
    return false;
}

bool MainTableModelRegistry::RemoveActor(size_t ID) noexcept
{
    if (ID < m_Actors_ByID.size())
    {
        ActorsList::iterator findedItValue = m_Actors_ByID[ID];
        Q_ASSERT(findedItValue != m_Actors.end());

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
    return false;
}

void MainTableModelRegistry::ClearAllPersons() noexcept
{
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
      ( role != Qt::DisplayRole /*&& role != Qt::EditRole*/ )
        ) {
            return QVariant();
        }

    switch (index.column())
    {
    case 0:

        break;
    case 1:

        break;
    case 2:

        break;
    }
    return QVariant();
    //return m_persons[ index.row() ][ Column( index.column() ) ];
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
    Q_UNUSED(index);
    Q_UNUSED(role);
    return QVariant();
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
}
