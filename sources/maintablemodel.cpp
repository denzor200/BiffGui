#include "maintablemodel.h"
#include <algorithm>
#include <limits>
#include <QDebug>
#include <QFile>
#include <sstream>
#include <QMessageBox>
#include <QApplication>

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
        m_Persons.emplace_front(person);

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
        m_Actors.emplace_front(actor);

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
    return PersonsBaseSetter(ID, [&](PersonsList::iterator Value)
    {
        ActorName OldName = Value->name;
        auto inserted = m_Persons_ByName.insert({newName, Value});
        if (inserted.second)
        {
            try {
                Value->name = newName;
            } catch (...) {
                m_Persons_ByName.erase(inserted.first);
                throw;
            }
            m_Persons_ByName.erase(OldName);
            return true;
        }
        return false;
    });
}

bool MainTableModelRegistry::RenameActor(int ID, const ActorName& newName)
{
    qDebug() << "[MainTableModelRegistry::RenameActor]: " << ID << ", " << newName;
    return ActorsBaseSetter(ID, [&](ActorsList::iterator Value)
    {
        ActorName OldName = Value->name;
        auto inserted = m_Actors_ByName.insert({newName, Value});
        if (inserted.second)
        {
            try {
                Value->name = newName;
            } catch (...) {
                m_Actors_ByName.erase(inserted.first);
                throw;
            }
            m_Actors_ByName.erase(OldName);
            return true;
        }
        return false;
    });
}

bool MainTableModelRegistry::ChangeDenyPerson(int ID, bool State)
{
    qDebug() << "[MainTableModelRegistry::ChangeDenyPerson]: " << ID << ", " << State;
    return PersonsBaseSetter(ID, [&](PersonsList::iterator Value)
    {
        Value->deny = State;
        return true;
    });
}

bool MainTableModelRegistry::ChangeDenyActor(int ID, bool State)
{
    qDebug() << "[MainTableModelRegistry::ChangeDenyActor]: " << ID << ", " << State;
    return ActorsBaseSetter(ID, [&](ActorsList::iterator Value)
    {
        Value->deny = State;
        return true;
    });
}

bool MainTableModelRegistry::PersonClearAllRelations(int ID)
{
    qDebug() << "[MainTableModelRegistry::PersonClearAllRelations]: " << ID;
    return PersonsBaseSetter(ID, [&](PersonsList::iterator Value)
    {
        RemoveAllLinksToPerson(Value);
        Value->actors.clear();
        return true;
    });
}

bool MainTableModelRegistry::ActorClearAllRelations(int ID)
{
    qDebug() << "[MainTableModelRegistry::ActorClearAllRelations]: " << ID;
    return ActorsBaseSetter(ID, [&](ActorsList::iterator Value)
    {
        RemoveAllLinksToActor(Value);
        Value->persons.clear();
        return true;
    });
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

// internal method
void MainTableModelRegistry::RemoveAllLinksToPerson(PersonsList::iterator personIt)
{
    for (ActorsList::iterator actorIt : personIt->actors)
    {
        Q_ASSERT(actorIt != m_Actors.end());
        actorIt->persons.erase(
                    std::remove(actorIt->persons.begin(), actorIt->persons.end(), personIt),
                    actorIt->persons.end());
    }
}

// internal method
void MainTableModelRegistry::RemoveAllLinksToActor(ActorsList::iterator actorIt)
{
    for (PersonsList::iterator personIt : actorIt->persons)
    {
        Q_ASSERT(personIt != m_Persons.end());
        personIt->actors.erase(
                    std::remove(personIt->actors.begin(), personIt->actors.end(), actorIt),
                    personIt->actors.end());
    }
}

// internal method
bool MainTableModelRegistry::PersonHasLink(PersonsList::iterator personIt, ActorsList::iterator actorIt) const
{
    for (ActorsList::iterator it : personIt->actors)
    {
        if (it==actorIt)
            return true;
    }
    return false;
}

// internal method
bool MainTableModelRegistry::ActorHasLink(ActorsList::iterator actorIt, PersonsList::iterator personIt) const
{
    for (PersonsList::iterator it : actorIt->persons)
    {
        if (it==personIt)
            return true;
    }
    return false;
}

// internal method
bool MainTableModelRegistry::PersonHasOtherLink(PersonsList::iterator personIt, ActorsList::iterator actorIt) const
{
    for (ActorsList::iterator it : personIt->actors)
    {
        if (it!=actorIt)
            return true;
    }
    return false;
}

// internal method
bool MainTableModelRegistry::ActorHasOtherLink(ActorsList::iterator actorIt, PersonsList::iterator personIt) const
{
    for (PersonsList::iterator it : actorIt->persons)
    {
        if (it!=personIt)
            return true;
    }
    return false;
}


bool MainTableModelRegistry::Person_ChangeRelation(int personID, const ActorName& actor, bool State)
{
    qDebug() << "[MainTableModelRegistry::Person_ChangeRelation]: " << personID << ", " << actor << ", " << State;
    return PersonsBaseSetter(personID, [&](PersonsList::iterator Value)
    {
        auto actorIt = m_Actors_ByName.find(actor);
        if (actorIt != m_Actors_ByName.end())
            return Person_ChangeRelation(Value, actorIt->second, State);
        return false;
    });
}
bool MainTableModelRegistry::Actor_ChangeRelation(int actorID, const ActorName& person, bool State)
{
    qDebug() << "[MainTableModelRegistry::Actor_ChangeRelation]: " << actorID << ", " << person << ", " << State;
    return ActorsBaseSetter(actorID, [&](ActorsList::iterator Value)
    {
        auto personIt = m_Persons_ByName.find(person);
        if (personIt != m_Persons_ByName.end())
            return Actor_ChangeRelation(Value, personIt->second, State);
        return false;
    });
}

bool MainTableModelRegistry::Person_ChangeRelation(const ActorName &person, const ActorName &actor, bool State)
{
    qDebug() << "[MainTableModelRegistry::Person_ChangeRelation]: " << person << ", " << actor << ", " << State;
    auto personIt = m_Persons_ByName.find(person);
    auto actorIt = m_Actors_ByName.find(actor);
    if (personIt != m_Persons_ByName.end() && actorIt != m_Actors_ByName.end() )
        return Person_ChangeRelation(personIt->second, actorIt->second, State);
    return false;
}

bool MainTableModelRegistry::Actor_ChangeRelation(const ActorName &actor, const ActorName &person, bool State)
{
    qDebug() << "[MainTableModelRegistry::Actor_ChangeRelation]: " << actor << ", " << person << ", " << State;
    auto personIt = m_Persons_ByName.find(person);
    auto actorIt = m_Actors_ByName.find(actor);
    if (personIt != m_Persons_ByName.end() && actorIt != m_Actors_ByName.end() )
        return Actor_ChangeRelation(actorIt->second, personIt->second, State);
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
    return PersonsBaseGetter<QString>(ID,[](PersonsList::iterator Value) -> QString
    {
        return Value->name.Get();
    });
}

QString MainTableModelRegistry::ActorGetName(int ID) const
{
    GETTER_DEBUG() << "[MainTableModelRegistry::ActorGetName]: " << ID;
    return ActorsBaseGetter<QString>(ID,[](ActorsList::iterator Value) -> QString
    {
        return Value->name.Get();
    });
}

MainTableModelRegistry::PersonsInfo
    MainTableModelRegistry::PersonGetActors(int ID, bool DisableDenied) const
{
    GETTER_DEBUG() << "[MainTableModelRegistry::PersonGetActors]: " << ID;
    auto Handler = [&](PersonsList::iterator Value) -> PersonsInfo
    {
        PersonsInfo Actors;
        Q_ASSERT(m_Actors_ByID.size() < TO_SZ(std::numeric_limits<int>::max()));
        Actors.Names.reserve(static_cast<int>(m_Actors_ByID.size()));
        int index = 0;
        // Двойной проход по актерам пришлось сделать чтобы рассчитать индексы
        ForeachActors([&](int, ActorsList::iterator actorValue) -> bool
        {
            bool EnableDenied = !DisableDenied;
            if (EnableDenied || !actorValue->deny)
            {
                Actors.Names.push_back(actorValue->name.Get());
                if (Value != NULL_PERSON)
                {
                    if (ActorHasLink(actorValue, Value))
                        Actors.SelectedIds.push_back(index);
                    if (ActorHasOtherLink(actorValue, Value)) {
                        Actors.SelectedOtherIds.push_back(index);
                    }
                }
            }
            index++;
            return true;
        });
        return Actors;
    };
    if (ID >= 0)
        return PersonsBaseGetter<PersonsInfo>(ID,Handler);
    else {
        return Handler(NULL_PERSON);
    }
}

MainTableModelRegistry::PersonsInfo
    MainTableModelRegistry::ActorGetPersons(int ID, bool DisableDenied) const
{
    GETTER_DEBUG() << "[MainTableModelRegistry::ActorGetPersons]: " << ID;
    auto Handler = [&](ActorsList::iterator Value) -> PersonsInfo
    {
        PersonsInfo Persons;
        Q_ASSERT(m_Persons_ByID.size() < TO_SZ(std::numeric_limits<int>::max()));
        Persons.Names.reserve(static_cast<int>(m_Persons_ByID.size()));
        int index = 0;
        ForeachPersons([&](int, PersonsList::iterator personValue) -> bool
        {
            bool EnableDenied = !DisableDenied;
            if (EnableDenied || !personValue->deny)
            {
                Persons.Names.push_back(personValue->name.Get());
                if (Value != NULL_ACTOR)
                {
                    if (PersonHasLink(personValue, Value))
                        Persons.SelectedIds.push_back(index);
                    if (PersonHasOtherLink(personValue, Value)) {
                        Persons.SelectedOtherIds.push_back(index);
                    }
                }
            }
            index++;
            return true;
        });
       return Persons;
    };
    if (ID >= 0)
        return ActorsBaseGetter<PersonsInfo>(ID,Handler);
    else {
        return Handler(NULL_ACTOR);
    }
}

bool MainTableModelRegistry::PersonIsDenied(int ID) const
{
    GETTER_DEBUG() << "[MainTableModelRegistry::PersonIsDenied]: " << ID;
    return PersonsBaseGetter<bool>(ID,[](PersonsList::iterator Value) -> bool
    {
        return Value->deny;
    });
}

bool MainTableModelRegistry::ActorIsDenied(int ID) const
{
    GETTER_DEBUG() << "[MainTableModelRegistry::ActorIsDenied]: " << ID;
    return ActorsBaseGetter<bool>(ID,[](ActorsList::iterator Value) -> bool
    {
        return Value->deny;
    });
}

bool MainTableModelRegistry::RemovePerson(int ID) noexcept
{
    qDebug() << "[MainTableModelRegistry::RemovePerson]: " << ID;
    return PersonsBaseSetter(ID, [&](PersonsList::iterator Value)
    {
        // Сначала снимаем все ссылки на конкретный элемент
        // Затем удаляем элемент непосредственно из хранилищ
        if (Value != NULL_PERSON)
        {
            RemoveAllLinksToPerson(Value);
            m_Persons_ByName.erase(Value->name);
            m_Persons.erase(Value);
        }
        m_Persons_ByID.erase(m_Persons_ByID.begin() + ID);
        return true;
    }, static_cast<uint32_t>(SETTER_FLAGS::ALLOW_NULL));
}

bool MainTableModelRegistry::RemoveActor(int ID) noexcept
{
    qDebug() << "[MainTableModelRegistry::RemoveActor]: " << ID;
    return ActorsBaseSetter(ID, [&](ActorsList::iterator Value)
    {
        // Сначала снимаем все ссылки на конкретный элемент
        // Затем удаляем элемент непосредственно из хранилищ
        if (Value != NULL_ACTOR)
        {
            RemoveAllLinksToActor(Value);
            m_Actors_ByName.erase(Value->name);
            m_Actors.erase(Value);
        }
        m_Actors_ByID.erase(m_Actors_ByID.begin() + ID);
        return true;
    }, static_cast<uint32_t>(SETTER_FLAGS::ALLOW_NULL));
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

MainTableModelRegistry::ReadingStats MainTableModelRegistry::ReadInStream(QTextStream &in)
{
    qDebug() << "[MainTableModelRegistry::ReadInStream]";
    ReadingStats Stats;
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList pair = line.split("-", QString::SkipEmptyParts);

        // Предварительные проверки..
        if (pair.size()>2)
        {
            Stats.UnrecognisedLines += line;
            continue;
        }
        if (pair.size()==0)
            continue;

        try {
            auto actorObj = ActorName(pair[0]);

            // Что мы можем извлечь: актер и несколько персонажей
            if (pair.size()==2)
            {
                QStringList persons = pair[1].split(",", QString::SkipEmptyParts);
                AddActor(actorObj);
                for (const QString& person : persons)
                {
                    // Оборачиваем имя персонажа в объект..
                    try {
                        auto personObj = ActorName(person);
                        AddPerson(personObj);
                        Actor_ChangeRelation(actorObj, personObj, true);
                    }
                    catch (const ActorNameStringEmpty&)
                    {
                    }
                    catch (const ActorNameForbiddenSymbols&)
                    {
                        Stats.UnrecognisedPersons += person;
                    }
                }
            }

            // Что мы можем извлечь: только актер
            else if (pair.size()==1)
            {
                // TODO: exception
                AddActor(actorObj);
            }
        }
        catch (const ActorNameStringEmpty&)
        {
        }
        catch (const ActorNameForbiddenSymbols&)
        {
            Stats.UnrecognisedActors += pair[0];
        }
    }
    return Stats;
}

void MainTableModelRegistry::WriteToStream(QTextStream &Out, bool DisableDenied) const
{
    qDebug() << "[MainTableModelRegistry::WriteToStream]";
    for (const Actor& Value : m_Actors)
    {
        if (DisableDenied && Value.deny)
            continue;
        Out << Value.name.Get();
        QString Persons;
        for (PersonsList::iterator personIt : Value.persons)
        {
            if (DisableDenied && personIt->deny)
                continue;
            if (Persons.size() == 0)
                Persons += " - ";
            else {
                Persons += ", ";
            }
            Persons += personIt->name.Get();
        }
        Out << Persons << endl;
    }
}

// Актер как ключ
MainTableModel::MainTableModel( QObject *parent) :
    QAbstractTableModel(parent),
    m_Mngr(qobject_cast<MainTableModelsManager*>(parent))
{
    Q_ASSERT(m_Mngr);
}

bool MainTableModel::InsertRow()
{
    int row = m_Mngr->GetRegistry()->getActorsCount();
    beginInsertRows( QModelIndex(), row, row );
    if (m_Mngr->GetRegistry()->ReserveNewActorIndex())
    {
        endInsertRows();
        return true;
    }
    return false;
}

bool MainTableModel::RemoveRow(int row)
{
    CHECK_COND(row < m_Mngr->GetRegistry()->getActorsCount());
    beginRemoveRows( QModelIndex(), row, row );
    m_Other->beginResetModel();
    if (m_Mngr->GetRegistry()->RemoveActor(row))
    {
        endRemoveRows();
        m_Other->endResetModel();
        return true;
    }
    return false;
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
            auto p = m_Mngr->GetRegistry()->ActorGetPersons(index.row(), false);
            QList<QVariant> list;
            list.reserve(3);
            list.push_back(std::move(p.Names));
            list.push_back(std::move(p.SelectedIds));
            list.push_back(std::move(p.SelectedOtherIds));
            return list;
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

    // Setting Actor Name..
    case 0:
        if (value.canConvert<QString>())
        {
            if (m_Other)
                m_Other->beginResetModel();
            try {
                // Возвращаемое значение от SetActor можно и проигнорировать
                MainTableModelUtils::SetActor(*m_Mngr->GetRegistry(), index.row(), ActorName(value.toString()));
                Changed = true;
            }
            catch (const ActorNameStringEmpty&) {
            }
			catch (const ActorNameForbiddenSymbols&)
			{
				ActorName::ShowForbidenSymbolsError("");
			}
            catch (const MainTableModelRegistry_InvalidActorID&) {
            }
        }
        break;

    // Setting Persons list..
    case 1:
        if (value.canConvert<QStringList>())
        {
            if (m_Other)
                m_Other->beginResetModel();
            try {
                QStringList list = value.toStringList();
                m_Mngr->GetRegistry()->ActorClearAllRelations(index.row());
                for (const QString& Value : list)
                {
                    m_Mngr->GetRegistry()->Actor_ChangeRelation(index.row(), ActorName(Value), true);
                }
                Changed = true;
            }
            catch (const ActorNameStringEmpty&) {
            }
			catch (const ActorNameForbiddenSymbols&)
			{
				ActorName::ShowForbidenSymbolsError("");
			}
        }
        break;

    // Setting "deny" flag..
    case 2:
        if (value.canConvert<bool>())
        {
            // Возвращаемое значение от ChangeDenyActor можно и проигнорировать
            m_Mngr->GetRegistry()->ChangeDenyActor(index.row(), value.toBool());
            Changed = true;
        }
        break;

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
            return section + 1;
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

bool MainTableModel_Reversed::InsertRow()
{
    int row = m_Mngr->GetRegistry()->getPersonsCount();
    beginInsertRows( QModelIndex(), row, row );
    if (m_Mngr->GetRegistry()->ReserveNewPersonIndex())
    {
        endInsertRows();
        return true;
    }
    return false;
}

bool MainTableModel_Reversed::RemoveRow(int row)
{
    CHECK_COND(row < m_Mngr->GetRegistry()->getPersonsCount());
    beginRemoveRows( QModelIndex(), row, row );
    m_Other->beginResetModel();
    if (m_Mngr->GetRegistry()->RemovePerson(row))
    {
        endRemoveRows();
        m_Other->endResetModel();
        return true;
    }
    return false;
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
            auto p = m_Mngr->GetRegistry()->PersonGetActors(index.row(), false);
            QList<QVariant> list;
            list.reserve(3);
            list.push_back(std::move(p.Names));
            list.push_back(std::move(p.SelectedIds));
            list.push_back(std::move(p.SelectedOtherIds));
            return list;
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

Qt::ItemFlags MainTableModel_Reversed::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QAbstractTableModel::flags( index );
    if( index.isValid() ) {
        flags |= Qt::ItemIsEditable;
    }
    return flags;
}

// TODO: слишком много копипаста
bool MainTableModel_Reversed::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if( !index.isValid() || role != Qt::EditRole || m_Mngr->GetRegistry()->getPersonsCount() <= index.row() ) {
        return false;
    }

    bool Changed = false;
    switch (index.column())
    {

    // Setting Actor Name..
    case 0:
        if (value.canConvert<QString>())
        {
            if (m_Other)
                m_Other->beginResetModel();
            try {
                // Возвращаемое значение от SetPerson можно и проигнорировать
                MainTableModelUtils::SetPerson(*m_Mngr->GetRegistry(), index.row(), ActorName(value.toString()));
                Changed = true;
            }
            catch (const ActorNameStringEmpty&) {
            }
            catch (const MainTableModelRegistry_InvalidPersonID&) {
            }
			catch (const ActorNameForbiddenSymbols&)
			{
				ActorName::ShowForbidenSymbolsError("");
			}
        }
        break;

    // Setting Persons list..
    case 1:
        if (value.canConvert<QStringList>())
        {
            if (m_Other)
                m_Other->beginResetModel();
            try {
                QStringList list = value.toStringList();
                m_Mngr->GetRegistry()->PersonClearAllRelations(index.row());
                for (const QString& Value : list)
                {
                    m_Mngr->GetRegistry()->Person_ChangeRelation(index.row(), ActorName(Value), true);
                }
                Changed = true;
            }
            catch (const ActorNameStringEmpty&) {
            }
			catch (const ActorNameForbiddenSymbols&)
			{
				ActorName::ShowForbidenSymbolsError("");
			}
        }
        break;

    // Setting "deny" flag..
    case 2:
        if (value.canConvert<bool>())
        {
            // Возвращаемое значение от ChangeDenyPerson можно и проигнорировать
            m_Mngr->GetRegistry()->ChangeDenyPerson(index.row(), value.toBool());
            Changed = true;
        }
        break;

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

QVariant MainTableModel_Reversed::headerData(int section, Qt::Orientation orientation, int role) const
{
    if( role != Qt::DisplayRole ) {
        return QVariant();
    }

    if( orientation == Qt::Vertical ) {
        return section + 1;
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
}

void MainTableModelsManager::LoadPersons(const QStringList &Persons)
{
    QStringList unrecognised;

    m_Model->beginResetModel();
    m_ModelReversed->beginResetModel();
    for (const QString& person : Persons) {
        try {
            m_Registry.AddPerson(ActorName(person));
        }
        catch (const ActorNameStringEmpty&) {
        }
        catch (const ActorNameForbiddenSymbols&)
        {
            unrecognised.push_back(person);
        }
    }
    m_Model->endResetModel();
    m_ModelReversed->endResetModel();
    HandleUnrecognisedPersons(unrecognised);
}

bool MainTableModelsManager::SavePersons(const QString &Path, bool DisableDenied) const
{
    QFile fileOut(Path);
    if (!fileOut.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    // Все текстовые файлы мы сохраняем только в UTF-8 with BOM
    QTextStream streamFileOut(&fileOut);
    streamFileOut.setCodec("UTF-8");
    streamFileOut.setGenerateByteOrderMark(true);
    auto data = m_Registry.ActorGetPersons(-1, DisableDenied);
    for ( const QString& Value : data.Names)
    {
        if (ActorName::FindForbidenSymbols(Value))
        {
            std::stringstream ss;
            // TODO: объединяй все в одно сообщение..
            // Не надо грузить пользователя пачкой мессаг
            ss << "Персонаж с именем '" << Value.toUtf8().data() << "' будет проигнорирован.";
            ActorName::ShowForbidenSymbolsError(ss.str().c_str());
            continue;
        }
        streamFileOut << Value << endl;
    }
    streamFileOut.flush();
    fileOut.close();
    return true;
}

void MainTableModelsManager::ClearAll()
{
    m_Model->beginResetModel();
    m_ModelReversed->beginResetModel();

    m_Registry.ClearAllActors();
    m_Registry.ClearAllPersons();

    m_Model->endResetModel();
    m_ModelReversed->endResetModel();
}

void MainTableModelsManager::HandleUnrecognisedPersons(const QStringList &persons)
{
    // TODO: test it two cases
    if (persons.size() > 0)
    {
        std::stringstream ss;
        if (persons.size() == 1)
        {
            ss << "Персонаж с именем '" << persons[0].toUtf8().data() << "' будет проигнорирован.";
        }
        else {
            ss << "Следующие персонажи будут проигнорированы:" << std::endl;
            ss << "\t" << persons.join("\n\t").toUtf8().data();
        }
        ActorName::ShowForbidenSymbolsError(ss.str().c_str());
    }
}

bool MainTableModelsManager::OpenTable(const QString &Path)
{
    QFile file(Path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream in(&file);
    in.setCodec("UTF-8"); // change the file codec to UTF-8.

    m_Model->beginResetModel();
    m_ModelReversed->beginResetModel();
    auto Stats = m_Registry.ReadInStream(in);
    m_Model->endResetModel();
    m_ModelReversed->endResetModel();

    if (!Stats.UnrecognisedLines.empty() || !Stats.UnrecognisedActors.empty() || !Stats.UnrecognisedPersons.empty())
    {
        std::stringstream ss;
        ss << "В ходе открытия таблицы из файла произошли некоторые ошибки." << std::endl;
        int index = 1;
        if (!Stats.UnrecognisedLines.empty())
        {
            ss << index << ". Следующие строки не были разобраны:" << std::endl;
            for (const QString& Value : Stats.UnrecognisedLines)
            {
                ss << "\t\t" << Value.toUtf8().data() << std::endl;
            }
            index++;
        }
        if (!Stats.UnrecognisedActors.empty())
        {
            ss << index << ". Следующие имена актеров были отклонены:" << std::endl;
            QString Actors;
            for (const QString& Value : Stats.UnrecognisedActors)
            {
                if (Actors.size()!=0)
                    Actors += ", ";
                Actors += Value;
            }
            ss << Actors.toUtf8().data() << std::endl;
            ss << "Возможно они содержат неразрешенные символы" << std::endl;
            index++;
        }
        if (!Stats.UnrecognisedPersons.empty())
        {
            ss << index << ". Следующие имена персонажей были отклонены:" << std::endl;
            QString Persons;
            for (const QString& Value : Stats.UnrecognisedPersons)
            {
                if (Persons.size()!=0)
                    Persons += ", ";
                Persons += Value;
            }
            ss << Persons.toUtf8().data() << std::endl;
            ss << "Возможно они содержат неразрешенные символы" << std::endl;
            index++;
        }
        QMessageBox::warning(QApplication::activeWindow(), "Предупреждение", ss.str().c_str());
    }
    return true;
}

bool MainTableModelsManager::SaveTable(const QString &Path, bool DisableDenied) const
{
    QFile fileOut(Path);
    if (fileOut.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream streamFileOut(&fileOut);
        streamFileOut.setCodec("UTF-8");
        streamFileOut.setGenerateByteOrderMark(true);
        m_Registry.WriteToStream(streamFileOut, DisableDenied);
        streamFileOut.flush();
        fileOut.close();
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
