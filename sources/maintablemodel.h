#ifndef MAINTABLEMODEL_H
#define MAINTABLEMODEL_H

#include <QSet>
#include <ActorName.h>
#include <QAbstractTableModel>

class MainTableModelRegistry
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
    MainTableModelRegistry() = default;

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

    int getActorsCount() const noexcept { return m_Actors.size(); }
    int getPersonsCount() const noexcept { return m_Persons.size(); }
private:
    QHash<ActorName,Actor> m_Actors;
    QHash<ActorName,Person> m_Persons;
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
    MainTableModelsManager* m_Mngr;
};

#endif // MAINTABLEMODEL_H
