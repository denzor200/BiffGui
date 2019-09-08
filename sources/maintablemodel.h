#ifndef MAINTABLEMODEL_H
#define MAINTABLEMODEL_H

#include <QSet>
#include <ActorName.h>

class MainTableModel
{
    struct Actor;
    struct Person;

    struct Actor
    {
        QSet<Person*> persons;
        ActorName name;
        bool deny = false;
    };
    struct Person
    {
        QSet<Actor*> actors;
        ActorName name;
        bool deny = false;
    };
public:
    MainTableModel() = default;

    bool AddPerson(const ActorName& person);
    bool AddActor(const ActorName& actor);

    bool RenamePerson(const ActorName& oldName, const ActorName& newName);
    bool RenameActor(const ActorName& oldName, const ActorName& newName);

    bool Person_ChangeStateActor(const ActorName& person, const ActorName& actor, bool State);
    bool Actor_ChangeStatePerson(const ActorName& actor, const ActorName& person, bool State);

    bool RemovePerson(const ActorName& person) noexcept;
    bool RemoveActor(const ActorName& actor) noexcept;

    void ClearAllPersons() noexcept;
    void ClearAllActors() noexcept;

private:
    QHash<ActorName,Actor> m_Actors;
    QHash<ActorName,Person> m_Persons;
};

#endif // MAINTABLEMODEL_H
