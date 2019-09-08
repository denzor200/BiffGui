#include "maintablemodel.h"

bool MainTableModel::AddPerson(const ActorName& person)
{
    if (m_Persons.find(person) == m_Persons.end())
    {
        Person NewObj;
        NewObj.name = person;
        m_Persons[person] = NewObj;
        return true;
    }
    return false;
}

bool MainTableModel::AddActor(const ActorName& actor)
{
    if (m_Actors.find(actor) == m_Actors.end())
    {
        Actor NewObj;
        NewObj.name = actor;
        m_Actors[actor] = NewObj;
        return true;
    }
    return false;
}

bool MainTableModel::RenamePerson(const ActorName& oldName, const ActorName& newName)
{
    auto it = m_Persons.find(oldName);
    if (it != m_Persons.end())
    {
        Person NewObj = *it;
        NewObj.name = newName;
        m_Persons[newName] = NewObj;
        // TODO: нужен ли этот рефреш итератора после вставки??
        it = m_Persons.find(oldName);
        Q_ASSERT(it != m_Persons.end());
        m_Persons.erase(it);
        return true;
    }
    return false;
}

bool MainTableModel::RenameActor(const ActorName& oldName, const ActorName& newName)
{
    auto it = m_Actors.find(oldName);
    if (it != m_Actors.end())
    {
        Actor NewObj = *it;
        NewObj.name = newName;
        m_Actors[newName] = NewObj;
        it = m_Actors.find(oldName);
        Q_ASSERT(it != m_Actors.end());
        m_Actors.erase(it);
        return true;
    }
    return false;
}

bool MainTableModel::Person_ChangeStateActor(const ActorName& person, const ActorName& actor, bool State)
{
    return Actor_ChangeStatePerson(actor, person, State);
}

bool MainTableModel::Actor_ChangeStatePerson(const ActorName& actor, const ActorName& person, bool State)
{
    auto actorIt = m_Actors.find(actor);
    auto personIt = m_Persons.find(person);
    if (actorIt != m_Actors.end() && personIt != m_Persons.end())
    {
        Actor& refActor = *actorIt;
        Person& refPerson = *personIt;
        Actor* ptrActor = &refActor;
        Person* ptrPerson = &refPerson;
        // Говоря обобщенным языком..
        // Тут происходит или создание взаимной связи
        // или ее удаление
        if (State)
        {
            QSet<Person*> TempActorPersons = refActor.persons;
            QSet<Actor*> TempPersonActors = refPerson.actors;
            TempActorPersons.insert(&refPerson);
            TempPersonActors.insert(&refActor);
            // TODO: убедись, что std::swap не создает лишнего копирования в комбинации с QSet
            std::swap(refActor.persons, TempActorPersons);
            std::swap(refPerson.actors, TempPersonActors);
        }
        else {
            {
                auto it = refActor.persons.begin();
                while (it != refActor.persons.end())
                {
                    Person* PersonValue = *it;
                    if (PersonValue == ptrPerson)
                    {
                        it = refActor.persons.erase(it);
                    }
                    else {
                        ++it;
                    }
                }
            }
            {
                auto it = refPerson.actors.begin();
                while (it != refPerson.actors.end())
                {
                    Actor* ActorValue = *it;
                    if (ActorValue == ptrActor)
                    {
                        it = refPerson.actors.erase(it);
                    }
                    else {
                        ++it;
                    }
                }
            }
        }
        return true;
    }
    return false;
}

bool MainTableModel::RemovePerson(const ActorName& person) noexcept
{
    auto findedIt = m_Persons.find(person);
    if (findedIt != m_Persons.end())
    {
        Person& rFindedIt = *findedIt;
        Person* pFindedIt = &rFindedIt;

        // Снимаем все ссылки (checked)
        for (Actor* Value : pFindedIt->actors)
        {
            // Можно было-бы перебрать все значения из m_Actors..
            // Но зачем?
            // Имеет смысл перебирать только свои

            Q_ASSERT(Value);
            auto loopedIt = Value->persons.begin();
            while (loopedIt != Value->persons.end())
            {
                Person* PersonValue = *loopedIt;
                if (PersonValue==pFindedIt)
                {
                    loopedIt = Value->persons.erase(loopedIt);
                }
                else {
                    ++loopedIt;
                }
            }
        }

        // Удаляем непосредственно из хранилища
        m_Persons.erase(findedIt);
        return true;
    }
    return false;
}

bool MainTableModel::RemoveActor(const ActorName& actor) noexcept
{
    auto findedIt = m_Actors.find(actor);
    if (findedIt != m_Actors.end())
    {
        Actor& rFindedIt = *findedIt;
        Actor* pFindedIt = &rFindedIt;

        // Снимаем все ссылки (checked)
        for (Person* Value : pFindedIt->persons)
        {
            // Можно было-бы перебрать все значения из m_Persons..
            // Но зачем?
            // Имеет смысл перебирать только свои

            Q_ASSERT(Value);
            auto loopedIt = Value->actors.begin();
            while (loopedIt != Value->actors.end())
            {
                Actor* ActorValue = *loopedIt;
                if (ActorValue==pFindedIt)
                {
                    loopedIt = Value->actors.erase(loopedIt);
                }
                else {
                    ++loopedIt;
                }
            }
        }

        // Удаляем непосредственно из хранилища
        m_Actors.erase(findedIt);
        return true;
    }
    return false;
}

void MainTableModel::ClearAllPersons() noexcept
{
    // Снимаем все возможные ссылки на персонажей
    for (Actor& Value : m_Actors)
    {
        Value.persons.clear();
    }
    // Очищаем хранилище
    m_Persons.clear();
}

void MainTableModel::ClearAllActors() noexcept
{
    // Снимаем все возможные ссылки на актеров
    for (Person& Value : m_Persons)
    {
        Value.actors.clear();
    }
    // Очищаем хранилище
    m_Actors.clear();
}
