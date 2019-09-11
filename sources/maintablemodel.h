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

private:
    QHash<ActorName,Actor> m_Actors;
    QHash<ActorName,Person> m_Persons;
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

};

#endif // MAINTABLEMODEL_H
