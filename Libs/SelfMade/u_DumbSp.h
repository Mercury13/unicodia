#pragma once

#include <cstdint>
#include <atomic>

namespace dumb {

    ///
    /// @brief The SpTarget class
    ///    Sample target for dumb::Sp
    ///
    class SpTarget
    {
    protected:
        std::atomic<ptrdiff_t> fRefCount = 0;

        friend ptrdiff_t addRef(SpTarget& x) noexcept;
        friend ptrdiff_t subRef(SpTarget& x) noexcept;
        friend ptrdiff_t refCount(const SpTarget& x) noexcept;
    };

    inline ptrdiff_t addRef(SpTarget& x) noexcept { return ++x.fRefCount; }
    inline ptrdiff_t subRef(SpTarget& x) noexcept { return --x.fRefCount; }
    inline ptrdiff_t refCount(const SpTarget& x) noexcept { return x.fRefCount; }

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
        explicit Sp(Target* aX) noexcept : x(aX)
            { add(aX); }
        Sp(const Sp& other) noexcept : Sp(other.x) {}
        constexpr Sp(Sp&& other) noexcept : x(other.x.exchange(nullptr)) {}
        Sp& operator = (const Sp& other) noexcept;
        Sp& operator = (Sp&& other) noexcept {
            assignRelease(other.x.exchange(nullptr));
            return *this;
        }
        ~Sp() noexcept { assignRelease(nullptr); }
        void reset() noexcept { assignRelease(nullptr); }
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


    template <class Target, class... Args>
    inline Sp<Target> makeSp(Args&&... args)
    {
        return Sp<Target>(new Target(std::forward<Args>(args)...));
    }

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
auto dumb::Sp<Target>::operator = (const Sp& other) noexcept -> Sp&
{
    auto xnew = other.get();
    add(xnew);
    assignRelease(xnew);
    return *this;
}

template <class Target>
void dumb::Sp<Target>::assignRelease(Target* newX) noexcept
{
    if (Target* xcopy = x.exchange(newX)) {
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
    if (Target* xcopy = x.load()) {
        auto q = dumb::refCount(*xcopy);
        if (q <= 0) {
            // Should not happen!
            std::terminate();
        }
        return q;
    } else {
        return 0;
    }
}
