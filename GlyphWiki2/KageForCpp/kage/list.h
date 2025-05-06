#pragma once

#include <vector>
#include <algorithm>
#include <stdexcept>

namespace kage {

    template <class T>
    class FixedList
    {
    public:
        constexpr FixedList() noexcept = default;

        template <class... Args>
        constexpr FixedList(Args... args) : sz(sizeof...(Args)), data(args...)  {}

        using iterator = T*;
        using const_iterator = const T*;

        unsigned size() const noexcept { return sz; }
        iterator begin()              noexcept { return data; }
        iterator end()                noexcept { return data + sz; }
        const_iterator begin()  const noexcept { return data; }
        const_iterator end()    const noexcept { return data + sz; }
        const_iterator cbegin() const noexcept { return data; }
        const_iterator cend()   const noexcept { return data + sz; }

        constexpr T& at (unsigned i);
        constexpr const T& at (unsigned i) const;
        constexpr T& operator [] (size_t i) { return at(i); }
        constexpr const T& operator [] (size_t i) const { return at(i); }
    private:
        static constexpr unsigned SIZE = 10;
        unsigned sz = 0;
        T data[SIZE];
    };

    template <class T>
    class List
    {
    private:
        using UnderVec = std::vector<T>;
    public:
        List() = default;
        List(size_t n) : data(n) {};

        T& operator[](size_t i) { return data.at(i); }
        const T& operator[](size_t i) const { return data.at(i); }
        T& at(size_t i) { return data.at(i); }
        const T& at(size_t i) const { return data.at(i); }

        using iterator = UnderVec::iterator;
        using const_iterator = UnderVec::const_iterator;

        [[nodiscard]] auto begin()  noexcept { return data.begin(); }
        [[nodiscard]] auto end()    noexcept { return data.end(); }
        [[nodiscard]] auto begin() const noexcept { return data.begin(); }
        [[nodiscard]] auto end()   const noexcept { return data.end(); }
        [[nodiscard]] auto cbegin() const noexcept { return data.cbegin(); }
        [[nodiscard]] auto cend()   const noexcept { return data.cend(); }
        [[nodiscard]] bool empty() const noexcept { return data.empty(); }
        [[nodiscard]] bool isEmpty() const noexcept { return data.empty(); }
        [[nodiscard]] size_t size() const noexcept { return data.size(); }
        void clear() noexcept { data.clear(); }

        void reverse();
        void resize(size_t x) { data.resize(x); }
        void reserve(size_t x) { data.reserve(x); }

        void append(const List& poly);
        [[deprecated("Use append")]] void concat(const List& poly) { return append(poly); }

        T pop_front();
        [[deprecated("Use pop_front")]] auto shift() { return pop_front(); }

        /// @return new length
        size_t push_front(const T& x);
        [[deprecated("Use pop_front")]] auto unshift(const T& x) { return push_front(x); }
    protected:
        UnderVec data;
    };

}   // namespace kage


template <class T>
constexpr T& kage::FixedList<T>::at(unsigned i) {
    if (i >= sz) throw std::invalid_argument("[FixedList] range check");
    return data[i];
}

template <class T>
constexpr const T& kage::FixedList<T>::at(unsigned i) const {
    if (i >= sz) throw std::invalid_argument("[FixedList] range check");
    return data[i];
}


template <class T>
void kage::List<T>::reverse()
    { std::reverse(data.begin(), data.end()); }

template <class T>
void kage::List<T>::append(const List& x)
    { data.insert(data.end(), x.begin(), x.end()); }

template <class T>
T kage::List<T>::pop_front()
{
    if (data.empty())
        throw std::logic_error("[Polygon::shift] data is empty");
    auto r = data.front();
    data.erase(data.begin());
    return r;
}

template <class T>
size_t kage::List<T>::push_front(const T& x)
{
    data.insert(data.begin(), x);
    return size();
}
