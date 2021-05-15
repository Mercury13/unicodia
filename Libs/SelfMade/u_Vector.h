#ifndef U_VECTOR_H
#define U_VECTOR_H

#include "u_Array.h"
#include <vector>

// C++17 here!


///  Used in special situations:
///  collect, drop, collect, drop…
template <class T>
class EnlargingVector
{
public:
          T& at(size_t i)
    #ifdef AT_RANGE_CHECK
            ;
    #else
            { return fV[i]; }
    #endif

    const T& at(size_t i) const
    #ifdef AT_RANGE_CHECK
            ;
    #else
            noexcept { return fV[i]; }
    #endif

          T& operator[] (size_t i)       { return at(i); }
          T& operator() (size_t i)       { return at(i); }
    const T& operator[] (size_t i) const { return at(i); }
    const T& operator() (size_t i) const { return at(i); }

    using iterator = T*;
    using const_iterator =  const T*;

    template <class U>
    T& add(U&& x);
    size_t size() const noexcept { return fSize; }
    void moveToArray(Array1d<T>& r);
    void reserve(size_t n) { fV.reserve(n); }
    void clear() noexcept { fSize = 0; }

    ///  Clear + reserve
    void clear(size_t n) { fSize = 0; fV.reserve(n); }

    ///  Drops all storage
    void collapse() { fV.clear(); fSize = 0; }

    iterator begin() noexcept { return fV.data(); }
    iterator end() noexcept { return fV.data() + fSize; }
    const_iterator begin() const noexcept { return fV.data(); }
    const_iterator end() const noexcept { return fV.data() + fSize; }
    const_iterator cbegin() const noexcept { return fV.data(); }
    const_iterator cend() const noexcept { return fV.data() + fSize; }

    T* data() { return fV.data(); }
    const T* data() const { return fV.data(); }

    ///  Sets new size if it’s smaller than current one
    void shrink(size_t n) { if (n < fSize) fSize = n; }
private:
    std::vector<T> fV;
    size_t fSize = 0;
};

#ifdef AT_RANGE_CHECK
    template <class T>
    T& EnlargingVector<T>::at(size_t i)
    {
        if (i >= size())
            throw std::logic_error("[EnlargingVector.at]: index out of bounds");
        return fV[i];
    }

    template <class T>
    const T& EnlargingVector<T>::at(size_t i) const
    {
        if (i >= size())
            throw std::logic_error("[EnlargingVector.at]: index out of bounds");
        return fV[i];
    }
#endif

template <class T> template <class U>
T& EnlargingVector<T>::add(U&& x)
{
    if (fSize < fV.size()) {
        return (fV[fSize++] = std::forward<U>(x));
    } else {
        ++fSize;
        return fV.emplace_back(std::forward<U>(x));
    }
}

template <class T>
void EnlargingVector<T>::moveToArray(Array1d<T>& r)
{
    r.alloc(fSize);
    for (size_t i = 0; i < fSize; ++i)
        r[i] = std::move(fV[i]);
    fSize = 0;
}


namespace chk
{
    template<class T, class R, class ... Args>
    std::is_convertible<std::invoke_result_t<T, Args...>, R> is_invokable_test(int);

    template<class T, class R, class ... Args>
    std::false_type is_invokable_test(...);

    template<class T, class R, class ... Args>
    using is_invokable = decltype(is_invokable_test<T, R, Args...>(0));

    template<class T, class R, class ... Args>
    constexpr auto is_invokable_v = is_invokable<T, R, Args...>::value;

    template<class L, class R = L>
    using has_equality = is_invokable<std::equal_to<>, bool, L, R>;
    template<class L, class R = L>
    constexpr auto has_equality_v = has_equality<L, R>::value;

    template<class L, class R = L>
    using has_less = is_invokable<std::less<>, bool, L, R>;
    template<class L, class R = L>
    constexpr auto has_less_v = has_less<L, R>::value;
}


template <class T>
class SafeVector : public std::vector<T>
{
private:
    using Super = std::vector<T>;
public:
    using size_type = typename std::vector<T>::size_type;
    using value_type = T;
    using reference = typename std::vector<T>::reference;
    using const_reference = typename std::vector<T>::const_reference;

    using Super::empty;
    using Super::at;
    using Super::size;

    template <class U = T>
    inline auto data() { return Super::data(); }

    template <class U = T>
    inline auto data() const { return Super::data(); }

    SafeVector() = default;
    SafeVector(size_type count, const T& value) : Super(count, value) {}
    explicit SafeVector(size_type count ) : Super(count) {}

    template< class InputIt >
    SafeVector( InputIt first, InputIt last) : Super(first, last) {}

    SafeVector( const std::vector<T>& other ) : Super(other) {}
    SafeVector( std::vector<T>&& other ) : Super(std::move(other)) {}
    SafeVector( std::initializer_list<T> init) : Super(init) {}

#ifdef AT_RANGE_CHECK
    reference       operator[]( size_type pos ) { return Super::at(pos); }
    const_reference operator[]( size_type pos ) const { return Super::at(pos); }

    reference front();
    const_reference front() const;

    reference back();
    const_reference back() const;
#endif

    const T& safeGetC(size_t i, const T& def) const noexcept
        { return (i < size()) ? Super::operator[](i) : def; }
    T safeGetV(size_t i, T def) const noexcept
        { return (i < size()) ? Super::operator[](i) : T(def); }
    T* safeGetP(size_t i) noexcept
        { return (i < size()) ? &Super::operator[](i) : nullptr; }
    const T* safeGetP(size_t i) const noexcept
        { return (i < size()) ? &Super::operator[](i) : nullptr; }
    void safeSetV(size_t i, T v) noexcept
    {
        if (i < size())
            Super::operator[](i) = v;
    }

    template <class U = T>
    constexpr typename std::enable_if<chk::has_less_v<U> && chk::has_equality_v<U>, void>::type removeDuplicates()
    {
        std::sort(Super::begin(), Super::end());
        Super::erase(std::unique(Super::begin(), Super::end()), Super::end());
    }

    template<class LessOperator, class EqualityOperator>
    void removeDuplicates(const LessOperator& less, const EqualityOperator& equality)
    {
        std::sort(Super::begin(), Super::end(), less);
        Super::erase(std::unique(Super::begin(), Super::end(), equality), Super::end());
    }

};

#ifdef AT_RANGE_CHECK
    template <class T>
    typename SafeVector<T>::reference SafeVector<T>::front()
    {
        if (empty())
            throw std::logic_error("[SafeVector::front] Vector is empty!");
        return Super::front();
    }

    template <class T>
    typename SafeVector<T>::const_reference SafeVector<T>::front() const
    {
        if (empty())
            throw std::logic_error("[SafeVector::front const] Vector is empty!");
        return Super::front();
    }

    template <class T>
    typename SafeVector<T>::reference SafeVector<T>::back()
    {
        if (empty())
            throw std::logic_error("[SafeVector::back] Vector is empty!");
        return Super::back();
    }

    template <class T>
    typename SafeVector<T>::const_reference SafeVector<T>::back() const
    {
        if (empty())
            throw std::logic_error("[SafeVector::back const] Vector is empty!");
        return Super::back();
    }
#endif

template < class T, class Alloc,
           class = typename std::enable_if< std::is_same<typename std::vector<T>::const_reference, const T&>::value >::type >
inline Buf1d<T> toBuf(std::vector<T, Alloc>& x) noexcept
    { return { x.size(), x.data() }; }

template < class T, class Alloc,
           typename = typename std::enable_if< std::is_same<typename std::vector<T>::const_reference, const T&>::value >::type >
inline Buf1d<const T> toCBuf(const std::vector<T, Alloc>& x) noexcept
    { return { x.size(), x.data() }; }

template < class T >
inline Buf1d<T> one2buf(T& x) noexcept { return { 1, &x }; }

template < class T >
inline Buf1d<const T> one2cBuf(const T& x) noexcept { return { 1, &x }; }

#endif // U_VECTOR_H
