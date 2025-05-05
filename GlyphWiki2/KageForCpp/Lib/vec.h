#pragma once

#include <vector>
#include <algorithm>
#include <stdexcept>

namespace kage {

    template <class T>
    class Vec
    {
    private:
        using UnderVec = std::vector<T>;
    public:
        Vec() = default;
        Vec(size_t n) : data(n) {};

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

        void append(const Vec& poly);
        [[deprecated("Use append")]] void concat(const Vec& poly) { return append(poly); }

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
void kage::Vec<T>::reverse()
    { std::reverse(data.begin(), data.end()); }

template <class T>
void kage::Vec<T>::append(const Vec& x)
    { data.insert(data.end(), x.begin(), x.end()); }

template <class T>
T kage::Vec<T>::pop_front()
{
    if (data.empty())
        throw std::logic_error("[Polygon::shift] data is empty");
    auto r = data.front();
    data.erase(data.begin());
    return r;
}

template <class T>
size_t kage::Vec<T>::push_front(const T& x)
{
    data.insert(data.begin(), x);
    return size();
}
