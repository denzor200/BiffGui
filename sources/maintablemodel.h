#ifndef MAINTABLEMODEL_H
#define MAINTABLEMODEL_H

#include <vector>
#include <list>
#include <unordered_map>

#include <ActorName.h>
#include <QAbstractTableModel>

class MainTableModelRegistry_InvalidPersonID : public std::exception
{
public:
    MainTableModelRegistry_InvalidPersonID() : std::exception("Invalid actor id")
    {
    }
};

class MainTableModelRegistry_InvalidActorID : public std::exception
{
public:
    MainTableModelRegistry_InvalidActorID() : std::exception("Invalid person id")
    {
    }
};

class MainTableModelRegistry
{
    struct Actor;
    struct Person;

    using ActorsList = std::list<Actor>;
    using PersonsList = std::list<Person>;

    struct Actor
    {
        std::vector<PersonsList::iterator> persons;
        ActorName name;
        bool deny = false;
    };
    struct Person
    {
        std::vector<ActorsList::iterator> actors;
        ActorName name;
        bool deny = false;
    };
public:
    MainTableModelRegistry() :
        NULL_ACTOR(m_Actors.end()),
        NULL_PERSON(m_Persons.end())
    {
    }

    bool ReserveNewPersonIndex();
    bool ReserveNewActorIndex();

    bool AddPerson(const ActorName& person, int ID = -1);
    bool AddActor(const ActorName& actor, int ID = -1);

    bool RenamePerson(int ID, const ActorName& newName);
    bool RenameActor(int ID, const ActorName& newName);

    bool Person_ChangeRelation(int personID, const ActorName& actor, bool State);
    bool Actor_ChangeRelation(int actorID, const ActorName& person, bool State);

    QString PersonGetName(int ID) const;
    QString ActorGetName(int ID) const;

    QList<QString> PersonGetActors(int ID) const;
    QList<QString> ActorGetPersons(int ID) const;

    bool PersonIsDenied(int ID) const;
    bool ActorIsDenied(int ID) const;

    bool RemovePerson(int ID) noexcept;
    bool RemoveActor(int ID) noexcept;

    void ClearAllPersons() noexcept;
    void ClearAllActors() noexcept;

    // TODO: fix warning
    int getPersonsCount() const noexcept { return m_Persons_ByID.size(); }
    int getActorsCount() const noexcept { return m_Actors_ByID.size(); }

private:
    bool Person_ChangeRelation(PersonsList::iterator personIt, ActorsList::iterator actorIt, bool State);
    bool Actor_ChangeRelation(ActorsList::iterator actorIt, PersonsList::iterator personIt, bool State);

private:
    // main storage
    ActorsList m_Actors;
    PersonsList m_Persons;

    // indices
    std::vector<ActorsList::iterator> m_Actors_ByID;
    std::vector<PersonsList::iterator> m_Persons_ByID;

    std::unordered_map<ActorName,ActorsList::iterator> m_Actors_ByName;
    std::unordered_map<ActorName,PersonsList::iterator> m_Persons_ByName;

    const ActorsList::iterator NULL_ACTOR;
    const PersonsList::iterator NULL_PERSON;
};

class MainTableModel;
class MainTableModel_Reversed;

class MainTableModelsManager : public QObject
{
    Q_OBJECT
public:
    MainTableModelsManager(QObject* parent = nullptr);

    MainTableModel*                     GetModel()                  { return m_Model; }
    const MainTableModel*               GetModel() const            { return m_Model; }

    MainTableModel_Reversed*            GetModelReversed()          { return m_ModelReversed; }
    const MainTableModel_Reversed*      GetModelReversed() const    { return m_ModelReversed; }

    bool PersonsInsertRow();
    bool ActorsInsertRow();

protected:
    friend class MainTableModel;
    friend class MainTableModel_Reversed;

    MainTableModelRegistry*         GetRegistry()       { return &m_Registry; }
    const MainTableModelRegistry*   GetRegistry() const { return &m_Registry; }

private:
    MainTableModelRegistry      m_Registry;
    MainTableModel*             m_Model;
    MainTableModel_Reversed*    m_ModelReversed;
};

class MainTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    MainTableModel(QObject* parent = nullptr);

    int rowCount( const QModelIndex& parent = QModelIndex()) const override;
    int columnCount( const QModelIndex& parent = QModelIndex()) const override;
    QVariant data( const QModelIndex& index, int role = Qt::DisplayRole) const override;

    QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
    friend class MainTableModelsManager;
    MainTableModelsManager* m_Mngr;
};

class MainTableModel_Reversed : public QAbstractTableModel
{
    Q_OBJECT
public:
    MainTableModel_Reversed(QObject* parent = nullptr);

    int rowCount( const QModelIndex& parent = QModelIndex()) const override;
    int columnCount( const QModelIndex& parent = QModelIndex()) const override;
    QVariant data( const QModelIndex& index, int role = Qt::DisplayRole) const override;

    QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
    friend class MainTableModelsManager;
    MainTableModelsManager* m_Mngr;
};

#endif // MAINTABLEMODEL_H
