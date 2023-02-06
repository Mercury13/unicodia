#pragma once

#include <cstdint>
#include <atomic>

namespace dumb {

    template <class Target> void addRef(Target& x) noexcept;
    template <class Target> ptrdiff_t subRef(Target& x) noexcept;
    template <class Target> ptrdiff_t refCount(const Target& x) noexcept;

    ///
    /// @brief The SpTarget class
    ///    Sample target dor dumb::Sp
    ///
    class SpTarget
    {
    protected:
        std::atomic<ptrdiff_t> fRefCount = 0;

        friend void addRef<SpTarget>(SpTarget& x) noexcept;
        friend ptrdiff_t subRef<SpTarget>(SpTarget& x) noexcept;
        friend ptrdiff_t refCount<SpTarget>(const SpTarget& x) noexcept;
    };

    template <> inline void addRef<SpTarget>(SpTarget& x) noexcept { ++x.fRefCount; }
    template <> inline ptrdiff_t subRef<SpTarget>(SpTarget& x) noexcept { return --x.fRefCount; }
    template <> inline ptrdiff_t refCount<SpTarget>(const SpTarget& x) noexcept { return x.fRefCount; }

    ///
    /// @brief The Sp class
    ///   Dumb shared pointer
    /// @tparam Target  pointer’s target class
    ///
    template <class Target>
    class Sp
    {
    public:
        constexpr Sp() noexcept = default;
        constexpr Sp(std::nullptr_t) noexcept {}
        explicit Sp(Target* aX) noexcept;
        Sp(const Sp& other) noexcept : Sp(other.x) {}
        Sp(Sp&& other) noexcept : x(other.x.exchange(nullptr)) {}
        Sp& operator = (const Sp& other) noexcept;
        Sp& operator = (Sp&& other) noexcept;
        ~Sp() noexcept { assignRelease(nullptr); }
        Target* get() const noexcept { return x.load(); }
        Target& operator * () const noexcept { return *x.load(); }
        Target* operator -> () const noexcept { return x.load(); }
    private:
        std::atomic<Target*> x = nullptr;
        void assignRelease(Target* newX) noexcept;
    };  // Sp

}   // namespace dumb


///// Sp’s implementation //////////////////////////////////////////////////////

template <class Target>
dumb::Sp<Target>::Sp(Target* aX) noexcept : x(aX)
{
    if(aX) {
        addRef(*aX);
    }
}

template <class Target>
auto dumb::Sp<Target>::operator = (const Sp& other) noexcept -> Sp&
{
    auto xnew = other.get();
    if (xnew) {
        addRef(*xnew);
    }
    assignRelease(xnew);
    return *this;
}

template <class Target>
auto dumb::Sp<Target>::operator = (Sp&& other) noexcept -> Sp&
{
    auto xnew = other.x.exchange(nullptr);
    assignRelease(xnew);
    return *this;
}

template <class Target>
void dumb::Sp<Target>::assignRelease(Target* newX) noexcept
{
    if (auto xcopy = x.exchange(newX)) {
        if (subRef(*xcopy) <= 0) {
            delete xcopy;
        }
    }
}
