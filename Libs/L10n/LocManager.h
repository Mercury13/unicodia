#pragma once

#include <list>
#include <unordered_map>

namespace loc {

    class Subject {     // interface
    public:
        virtual void translateMe() {};
        virtual ~Subject() = default;
    };

    class Manager : public Subject
    {
    public:
        void translateMe() override;
        void add(Subject& x);
        bool erase(Subject* x);
        bool erase(Subject& x) { return erase(&x); }
        void stopErasing() noexcept { doesErase = false; }
    private:
        using List = std::list<Subject*>;
        List inOrder;
        std::unordered_map<Subject*, List::iterator> bySubj;
        bool wasEverTranslated = false;
        bool doesErase = true;
    };

    extern Manager man;

    class AutoStop
    {
    public:
        AutoStop() = default;
        ~AutoStop() { man.stopErasing(); }
    };

}   // namespace loc

extern template class std::list<loc::Subject*>;
extern template class std::unordered_map<loc::Subject*, loc::Manager::List::iterator>;
