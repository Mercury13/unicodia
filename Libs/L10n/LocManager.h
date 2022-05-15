#pragma once

#include "u_Vector.h"

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
        void add(Subject& x) { children.push_back(&x); }
    private:
        SafeVector <Subject*> children;
    };

    extern Manager man;

}   // namespace loc
