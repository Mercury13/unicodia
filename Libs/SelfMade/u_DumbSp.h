#pragma once

#include <cstdint>
#include <atomic>

namespace dumb {

    template <class Target> ptrdiff_t addRef(Target& x) noexcept;
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

        friend ptrdiff_t addRef<SpTarget>(SpTarget& x) noexcept;
        friend ptrdiff_t subRef<SpTarget>(SpTarget& x) noexcept;
        friend ptrdiff_t refCount<SpTarget>(const SpTarget& x) noexcept;
    };

    template <> inline ptrdiff_t addRef<SpTarget>(SpTarget& x) noexcept { return ++x.fRefCount; }
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
        explicit operator bool() const noexcept { return x; }
        size_t refCount() const noexcept;
    private:
        std::atomic<Target*> x = nullptr;
        void assignRelease(Target* newX) noexcept;
        void add(Target* aX) noexcept;
    };  // Sp

}   // namespace dumb


///// Sp’s implementation //////////////////////////////////////////////////////

template <class Target>
void dumb::Sp<Target>::add(Target* aX) noexcept
{
    if(aX) {
        if (addRef(*aX) <= 0) {
            // Should not happen
            std::terminate();
        }
    }
}


template <class Target>
dumb::Sp<Target>::Sp(Target* aX) noexcept : x(aX)
    { add(aX); }


template <class Target>
auto dumb::Sp<Target>::operator = (const Sp& other) noexcept -> Sp&
{
    auto xnew = other.get();
    add(xnew);
    assignRelease(xnew);
    return *this;
}

template <class Target>
auto dumb::Sp<Target>::operator = (Sp&& other) noexcept -> Sp&
{
    assignRelease(other.x.exchange(nullptr));
    return *this;
}

template <class Target>
void dumb::Sp<Target>::assignRelease(Target* newX) noexcept
{
    if (auto xcopy = x.exchange(newX)) {
        auto v = subRef(*xcopy);
        if (v == 0) {
            delete xcopy;
        } else if (v < 0) {
            // Should not happen!
            std::terminate();
        }
    }
}

template <class Target>
size_t dumb::Sp<Target>::refCount() const noexcept
{
    if (auto xcopy = x.load()) {
        auto q = refCount(xcopy);
        if (q <= 0) {
            // Should not happen!
            std::terminate();
        }
        return q;
    } else {
        return 0;
    }
}
