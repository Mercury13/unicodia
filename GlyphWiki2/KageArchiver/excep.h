#pragma once

// NOLINTBEGIN(bugprone-macro-parentheses)
#define DEFINE_EXCEPTION_CLASS(CMe, CSuper) \
    class CMe : public CSuper { \
    private:                    \
        using Super = CSuper;   \
    public:                     \
        using Super::Super; };
// NOLINTEND(bugprone-macro-parentheses)
