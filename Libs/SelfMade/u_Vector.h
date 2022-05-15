#ifndef U_VECTOR_H
#define U_VECTOR_H

#include "u_Array.h"
#include <vector>


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
