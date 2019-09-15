#ifndef MAINTABLEMODEL_H
#define MAINTABLEMODEL_H

#include <vector>
#include <list>
#include <unordered_map>

#include <ActorName.h>
#include <QAbstractTableModel>
#include <QPair>
#include <QDebug>

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

    bool PersonIndexIsReserved(int ID) const;
    bool ActorIndexIsReserved(int ID) const;

    bool AddPerson(const ActorName& person, int ID = -1);
    bool AddActor(const ActorName& actor, int ID = -1);

    bool RenamePerson(int ID, const ActorName& newName);
    bool RenameActor(int ID, const ActorName& newName);

    bool ChangeDenyPerson(int ID, bool State);
    bool ChangeDenyActor(int ID, bool State);

    bool PersonClearAllRelations(int ID);
    bool ActorClearAllRelations(int ID);

    bool Person_ChangeRelation(int personID, const ActorName& actor, bool State);
    bool Actor_ChangeRelation(int actorID, const ActorName& person, bool State);

    QString PersonGetName(int ID) const;
    QString ActorGetName(int ID) const;

    QPair<QStringList, QList<QVariant>> PersonGetActors(int ID) const;
    QPair<QStringList, QList<QVariant>> ActorGetPersons(int ID) const;

    bool PersonIsDenied(int ID) const;
    bool ActorIsDenied(int ID) const;

    bool RemovePerson(int ID) noexcept;
    bool RemoveActor(int ID) noexcept;

    void ClearAllPersons() noexcept;
    void ClearAllActors() noexcept;

    int getPersonsCount() const noexcept { return static_cast<int>(m_Persons_ByID.size()); }
    int getActorsCount() const noexcept { return static_cast<int>(m_Actors_ByID.size()); }

private:
    bool Person_ChangeRelation(PersonsList::iterator personIt, ActorsList::iterator actorIt, bool State);
    bool Actor_ChangeRelation(ActorsList::iterator actorIt, PersonsList::iterator personIt, bool State);

    void RemoveAllLinksToPerson(PersonsList::iterator personIt);
    void RemoveAllLinksToActor(ActorsList::iterator actorIt);

    template <typename RET_T, typename EX_T, typename IT_T, typename FUNC_T>
    RET_T BaseGetter(
            const std::vector<IT_T>& Storage,
            int ID,
            IT_T NULL_IT,
            const FUNC_T& Func) const;


    template <typename IT_T, typename FUNC_T>
    bool BaseSetter(
            std::vector<IT_T>& Storage,
            int ID,
            IT_T NULL_IT,
            const FUNC_T& Func);


    template <typename RET_T, typename FUNC_T>
    RET_T ActorsBaseGetter(
            int ID,
            const FUNC_T& Func) const;

    template <typename RET_T, typename FUNC_T>
    RET_T PersonsBaseGetter(
            int ID,
            const FUNC_T& Func) const;


    template <typename FUNC_T>
    bool ActorsBaseSetter(
            int ID,
            const FUNC_T& Func);

    template <typename FUNC_T>
    bool PersonsBaseSetter(
            int ID,
            const FUNC_T& Func);

    template <typename FUNC_T>
    void ForeachActors(const FUNC_T& Func) const;

    template <typename FUNC_T>
    void ForeachPersons(const FUNC_T& Func) const;

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

template<typename FUNC_T>
void MainTableModelRegistry::ForeachPersons(const FUNC_T &Func) const
{
    int index = 0;
    for (PersonsList::iterator it : m_Persons_ByID)
    {
        if (it != NULL_PERSON)
        {
            if (!Func(index, it))
                break;
        }
        index++;
    }
}

template<typename FUNC_T>
void MainTableModelRegistry::ForeachActors(const FUNC_T &Func) const
{
    int index = 0;
    for (ActorsList::iterator it : m_Actors_ByID)
    {
        if (it != NULL_ACTOR)
        {
            if (!Func(index, it))
                break;
        }
        index++;
    }
}

template <typename RET_T, typename EX_T, typename IT_T, typename FUNC_T>
RET_T MainTableModelRegistry::BaseGetter(
        const std::vector<IT_T>& Storage,
        int ID,
        IT_T NULL_IT,
        const FUNC_T& Func) const
{
    if (ID < 0)
        throw EX_T();
    if (static_cast<size_t>(ID) < Storage.size())
    {
        IT_T it = Storage[static_cast<size_t>(ID)];
        if (it != NULL_IT)
            return Func(it);
    }
    throw EX_T();
}


template <typename IT_T, typename FUNC_T>
bool MainTableModelRegistry::BaseSetter(
        std::vector<IT_T>& Storage,
        int ID,
        IT_T NULL_IT,
        const FUNC_T& Func)
{
    if (ID < 0)
        return false;
    if (static_cast<size_t>(ID) < Storage.size())
    {
        auto it = Storage[static_cast<size_t>(ID)];
        if (it != NULL_IT)
        {
            return Func(it);
        }
    }
    return false;
}

template <typename FUNC_T>
bool MainTableModelRegistry::ActorsBaseSetter(
        int ID,
        const FUNC_T& Func)
{
    return BaseSetter(m_Actors_ByID, ID, NULL_ACTOR, Func);
}

template <typename FUNC_T>
bool MainTableModelRegistry::PersonsBaseSetter(
        int ID,
        const FUNC_T& Func)
{
    return BaseSetter(m_Persons_ByID, ID, NULL_PERSON, Func);
}



template <typename RET_T, typename FUNC_T>
RET_T MainTableModelRegistry::ActorsBaseGetter(
        int ID,
        const FUNC_T& Func) const
{
    return BaseGetter<RET_T, MainTableModelRegistry_InvalidActorID>(m_Actors_ByID, ID, NULL_ACTOR, Func);
}

template <typename RET_T, typename FUNC_T>
RET_T MainTableModelRegistry::PersonsBaseGetter(
        int ID,
        const FUNC_T& Func) const
{
    return BaseGetter<RET_T, MainTableModelRegistry_InvalidPersonID>(m_Persons_ByID, ID, NULL_PERSON, Func);
}

namespace MainTableModelUtils
{
    bool SetPerson(MainTableModelRegistry& R, int ID, const ActorName& Name);
    bool SetActor(MainTableModelRegistry& R, int ID, const ActorName& Name);
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

    bool PersonsRemoveRow(int ID);
    bool ActorsRemoveRow(int ID);

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

    void SetOther(MainTableModel_Reversed* Other) { m_Other = Other; }

    int rowCount( const QModelIndex& parent = QModelIndex()) const override;
    int columnCount( const QModelIndex& parent = QModelIndex()) const override;
    QVariant data( const QModelIndex& index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags( const QModelIndex& index ) const override;

    bool setData( const QModelIndex& index, const QVariant& value, int role ) override;
    QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
    friend class MainTableModelsManager;
    friend class MainTableModel_Reversed;
    MainTableModelsManager* m_Mngr;
    MainTableModel_Reversed* m_Other = nullptr;
};

class MainTableModel_Reversed : public QAbstractTableModel
{
    Q_OBJECT
public:
    MainTableModel_Reversed( QObject* parent = nullptr);

    void SetOther(MainTableModel* Other) { m_Other = Other; }

    int rowCount( const QModelIndex& parent = QModelIndex()) const override;
    int columnCount( const QModelIndex& parent = QModelIndex()) const override;
    QVariant data( const QModelIndex& index, int role = Qt::DisplayRole) const override;

    bool setData( const QModelIndex& index, const QVariant& value, int role ) override;
    QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
    friend class MainTableModelsManager;
    friend class MainTableModel;
    MainTableModelsManager* m_Mngr;
    MainTableModel* m_Other = nullptr;
};

#endif // MAINTABLEMODEL_H
