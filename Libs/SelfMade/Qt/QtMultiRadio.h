#pragma once

#include <QAbstractButton>
#include <QAction>

template <class T>
class Zptr {
public:
    constexpr Zptr() noexcept : v(nullptr) {}
    constexpr Zptr(T* x) noexcept : v(x) {}
    constexpr Zptr& operator = (T* x) { v = x; return *this; }
    constexpr T& operator * () const noexcept { return *v; }
    constexpr T* operator-> () const noexcept { return v;  }
    constexpr explicit operator bool() const { return v; }
    constexpr operator T* () const { return v; }
private:
    T* v;
};

template <class Cmp>
concept Checkable = requires(Cmp& x) {
    bool (x.isChecked());
    x.setChecked(true);
};


template <class Cmp> requires Checkable<Cmp>
class UintRadio
{
public:
    void setRadio(unsigned index, Cmp* button);
    void set(unsigned value);
    void unset();
    unsigned get(unsigned def = 0) const;
    Cmp* buttonAt(unsigned value) const;
private:
    std::vector<Zptr<Cmp>> v;
};

template <class Ec, class Cmp = QAbstractButton> requires std::is_enum_v<Ec>
class EcRadio : protected UintRadio<Cmp>
{
private:
    using Super = UintRadio<Cmp>;
public:
    void setRadio(Ec index, Cmp* button)
        { Super::setRadio(static_cast<int>(index), button); }
    void set(Ec value) { Super::set(static_cast<int>(value)); }
    using Super::unset;
    Ec get(Ec def = static_cast<Ec>(0)) const
        { return static_cast<Ec>(Super::get(static_cast<int>(def))); }
    Cmp* buttonAt(Ec value) const
        { return Super::buttonAt(static_cast<int>(value)); }
    UintRadio<Cmp>& unified() { return *this; }
    const UintRadio<Cmp>& unified() const { return *this; }
};


/// Implementations

template <class Cmp> requires Checkable<Cmp>
void UintRadio<Cmp>::setRadio(unsigned index, Cmp* button)
{
    auto sz = index + 1;
    if (sz > v.size())
        v.resize(sz);
    v[index] = button;
}


template <class Cmp> requires Checkable<Cmp>
Cmp* UintRadio<Cmp>::buttonAt(unsigned value) const
{
    if (value >= v.size())
        return nullptr;
    return v[value];
}


template <class Cmp> requires Checkable<Cmp>
void UintRadio<Cmp>::set(unsigned value)
{
    if (auto btn = buttonAt(value)) {
        btn->setChecked(true);
    } else {
        unset();
    }
}


template <class Cmp> requires Checkable<Cmp>
void UintRadio<Cmp>::unset()
{
    for (auto x : v) {
        if (x) {
            x->setChecked(false);
        }
    }
}


template <class Cmp> requires Checkable<Cmp>
unsigned UintRadio<Cmp>::get(unsigned def) const
{
    for (size_t i = 0; i < v.size(); ++i) {
        auto x = v[i];
        if (x && x->isChecked())
            return i;
    }
    return def;
}

extern template class UintRadio<QAbstractButton>;
extern template class UintRadio<QAction>;
