#ifndef ACTORSLIST_H
#define ACTORSLIST_H

#include <vector>
#include <memory>
#include <ActorName.h>

class ActorsList
{
    typedef std::vector<ActorName> NativeType;
    NativeType m_Native;
public:

    operator NativeType() const { return m_Native; }
    operator NativeType() { return m_Native; }

    const NativeType& Native() const {return m_Native; }
    NativeType& Native() {return m_Native; }

    static std::weak_ptr<ActorsList> Inctance()
    {
        static std::shared_ptr<ActorsList> st(new ActorsList);
        return st;
    }

private:
    ActorsList();
};

#endif // ACTORSLIST_H
