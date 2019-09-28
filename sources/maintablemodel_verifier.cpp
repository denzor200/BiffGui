#include "maintablemodel.h"

#define ENABLE_MODEL_CONSISTENCY_VERIFYING

// Этот код не для продакшена..
void MainTableModelRegistry::VerifyModelConsistency() const
{
#ifdef _DEBUG
#ifdef ENABLE_MODEL_CONSISTENCY_VERIFYING
    // Проверяем корректность взаимоотношений
    for (const Actor& actor : m_Actors)
    {
        for (PersonsList::const_iterator personIt : actor.persons)
        {
            Q_ASSERT(personIt!=NULL_PERSON);
            bool CorrectLink = false;
            for (ActorsList::const_iterator actorIt : personIt->actors)
            {
                Q_ASSERT(actorIt!=NULL_ACTOR);
                const Actor& actorRef = *actorIt;
                if (&actorRef==&actor)
                {
                    CorrectLink = true;
                    break;
                }
            }
            Q_ASSERT(CorrectLink);
        }
    }
    for (const Person& person : m_Persons)
    {
        for (ActorsList::const_iterator actorIt : person.actors)
        {
            Q_ASSERT(actorIt!=NULL_ACTOR);
            bool CorrectLink = false;
            for (PersonsList::const_iterator personIt : actorIt->persons)
            {
                Q_ASSERT(personIt!=NULL_PERSON);
                const Person& personRef = *personIt;
                if (&personRef==&person)
                {
                    CorrectLink = true;
                    break;
                }
            }
            Q_ASSERT(CorrectLink);
        }
    }

    // Проверяем, что у всех хранимых значений есть индексы
    // А также проверяем отсутствие висячих индексов
    {
        size_t IndexesCount = 0;
        for (ActorsList::iterator actorIt : m_Actors_ByID)
        {
            if (actorIt!=NULL_ACTOR)
            {
                bool CorrectedIndex = false;
                for (const Actor& actor : m_Actors)
                {
                    const Actor& actorRef = *actorIt;
                    if (&actorRef == &actor)
                    {
                        CorrectedIndex = true;
                        break;
                    }
                }
                Q_ASSERT(CorrectedIndex);
                IndexesCount ++;
            }
        }
        Q_ASSERT(IndexesCount == m_Actors.size());
    }
    {
        size_t IndexesCount = 0;
        for (PersonsList::iterator personIt : m_Persons_ByID)
        {
            if (personIt!=NULL_PERSON)
            {
                bool CorrectedIndex = false;
                for (const Person& person : m_Persons)
                {
                    const Person& personRef = *personIt;
                    if (&personRef == &person)
                    {
                        CorrectedIndex = true;
                        break;
                    }
                }
                Q_ASSERT(CorrectedIndex);
                IndexesCount ++;
            }
        }
        Q_ASSERT(IndexesCount == m_Persons.size());
    }
    {
        size_t IndexesCount = 0;
        for (const auto& pair : m_Actors_ByName)
        {
            Q_ASSERT(pair.second != NULL_ACTOR);
            // Проверять текстовый индекс на соответствие тоже следует
            Q_ASSERT(pair.first == pair.second->name);
            bool CorrectedIndex = false;
            for (const Actor& actor : m_Actors)
            {
                const Actor& actorRef = *pair.second;
                if (&actorRef == &actor)
                {
                    CorrectedIndex = true;
                    break;
                }
            }
            Q_ASSERT(CorrectedIndex);
            IndexesCount ++;
        }
        Q_ASSERT(IndexesCount == m_Actors.size());
    }
    {
        size_t IndexesCount = 0;
        for (const auto& pair : m_Persons_ByName)
        {
            Q_ASSERT(pair.second != NULL_PERSON);
            // Проверять текстовый индекс на соответствие тоже следует
            Q_ASSERT(pair.first == pair.second->name);
            bool CorrectedIndex = false;
            for (const Person& person : m_Persons)
            {
                const Person& personRef = *pair.second;
                if (&personRef == &person)
                {
                    CorrectedIndex = true;
                    break;
                }
            }
            Q_ASSERT(CorrectedIndex);
            IndexesCount ++;
        }
        Q_ASSERT(IndexesCount == m_Persons.size());
    }

#endif // ENABLE_MODEL_CONSISTENCY_VERIFYING
#endif // _DEBUG
}
