#pragma once

#include <string>
#include <utility>
#include <charconv>

#include "u_Vector.h"

namespace str {
    static_assert(sizeof(char) == sizeof(char8_t), "Strange machine with char != char8");

    inline bool isBlank(char c) { return static_cast<unsigned char>(c) <= 32; }
    inline bool isBlank(wchar_t c) { return (c <= 32); }
    inline bool isBlank(char8_t c) { return (c <= 32); }
    inline bool isBlank(char16_t c) { return (c <= 32); }
    inline bool isBlank(char32_t c) { return (c <= 32); }
    void trim(const char* &start, const char* &end);
    void trim(const char8_t* &start, const char8_t* &end);
    std::string_view trimSv(std::string_view s);
    [[nodiscard]] SafeVector<std::string_view> splitSv(std::string_view s, char comma, bool skipEmpty = true);
    [[nodiscard]] SafeVector<std::u8string_view> splitSv(std::u8string_view s, char comma, bool skipEmpty = true);

    /// @return [+] is letter digits+ [letter]
    ///       (used for Linear B etc. where chars are marked with these indexes)
    bool isIndex(std::string_view s);

    ///
    /// @return   # of replacements
    ///
    template <class C, class T, class A>
    size_t replace(
            std::basic_string<C, T, A>& haystack,
            std::basic_string_view<C, T> needle,
            std::basic_string_view<C, T> byWhat)
    {
        using Str = std::basic_string<C, T, A>;
        const size_t szNeedle = needle.length();
        const size_t szByWhat = byWhat.length();
        size_t p = 0, r = 0;
        while ((p = haystack.find(needle.data(), p, needle.size())) != Str::npos) {
            haystack.replace(p, szNeedle, byWhat);
            p += szByWhat;
            ++r;
        }
        return r;
    }

    ///
    /// @return   # of replacements
    ///
    template <class C, class T, class A>
    size_t replace(
            std::basic_string<C, T, A>& haystack,
            C needle,
            std::basic_string_view<C, T> byWhat)
    {
        using Str = std::basic_string<C, T, A>;
        const size_t szByWhat = byWhat.length();
        size_t p = 0, r = 0;
        while ((p = haystack.find(needle, p)) != Str::npos) {
            haystack.replace(p, 1, byWhat);
            p += szByWhat;
            ++r;
        }
        return r;
    }

    namespace detail {

        template <class T>
        concept Svable = requires (const T& t) {
            std::basic_string_view{t};
        };

        template<Svable T>
        inline auto toSv(const T& x) { return std::basic_string_view{x}; }

        template<class C, class T, class A>
        inline std::basic_string_view<C, T> toSv(
                const std::basic_string<C, T, A>& x)
            { return x; }
    }

    ///
    /// @return   # of replacements
    ///
    template <class C, class T, class A, class B>
    inline size_t replace(
            std::basic_string<C, T, A>& haystack,
            C needle,
            const B& byWhat)
        { return replace(
                    haystack, needle, detail::toSv(byWhat)); }

    ///
    /// @return   # of replacements
    ///
    template <class C, class T, class A, class N, class B>
    inline size_t replace(
            std::basic_string<C, T, A>& haystack,
            const N& needle,
            const B& byWhat)
        { return replace(
                    haystack, detail::toSv(needle), detail::toSv(byWhat)); }

    ///
    /// @return   # of replacements
    ///
    template <class C, class T, class A>
    size_t replace(
            std::basic_string<C, T, A>& s,
            C what,
            C byWhat)
    {
        size_t r = 0;
        for (auto& v : s) {
            if (v == what) {
                v = byWhat;
                ++r;
            }
        }
        return r;
    }

    /// Two versions only: normal and u8
    template <class T>
    inline auto fromChars(std::string_view s, T& v, int base = 10)
        { return std::from_chars(s.data(), s.data() + s.size(), v, base); }

    template <class T>
    inline auto fromChars(std::u8string_view s, T& v, int base = 10)
        { return std::from_chars(
                    reinterpret_cast<const char*>(s.data()),
                    reinterpret_cast<const char*>(s.data() + s.size()), v, base); }

    template <class T>
    inline std::string_view toChars(char* start, char* end, T v, int base = 10)
    {
        auto res = std::to_chars(start, end, v, base);
        if (res.ec != std::errc())
            return {};
        return { start, res.ptr };
    }

    template <class T, size_t N>
    inline std::string_view toChars(char (&buf)[N], T v, int base = 10)
    {
        return toChars(std::begin(buf), std::end(buf), v, base);
    }

    template <class T>
    inline std::u8string_view toCharsU8(char* start, char* end, T v, int base = 10)
    {
        auto r = toChars<T>(start, end, v, base);
        return { reinterpret_cast<const char8_t*>(r.data()), r.size() };
    }

    template <class T, size_t N>
    inline std::u8string_view toCharsU8(char (&buf)[N], T v, int base = 10)
    {
        auto r = toChars<T,N>(buf, v, base);
        return { reinterpret_cast<const char8_t*>(r.data()), r.size() };
    }

    void toUpperInPlace(std::u8string& x);
    inline std::u8string toUpper(std::u8string_view x) {
        std::u8string r{x};
        toUpperInPlace(r);
        return r;
    }

    inline std::string_view toSv(std::u8string_view x)
        { return { reinterpret_cast<const char*>(x.data()), x.length() }; }

    inline std::u8string_view toU8sv(const char* x)
    {
        if (!x)
            return {};
        return reinterpret_cast<const char8_t*>(x);
    }

    inline const char* toC(const std::u8string& x)
        { return reinterpret_cast<const char*>(x.c_str()); }

    inline const char* toC(const char8_t* x)
        { return reinterpret_cast<const char*>(x); }

    template <class T> struct string_traits;

    template <class C, class T, class A>
    struct string_traits<std::basic_string<C,T,A>> {
        using Ch = C;
        using Tr = T;
    };

    template <class C, class T>
    struct string_traits<std::basic_string_view<C,T>> {
        using Ch = C;
        using Tr = T;
    };

    template <class C>
    struct string_traits<const C*> {
        using Ch = std::remove_cv_t<C>;
        using Tr = std::char_traits<C>;
    };

    template <class C, size_t N>
    struct string_traits<C[N]> {
        using Ch = std::remove_cv_t<C>;
        using Tr = std::char_traits<Ch>;
    };

    namespace trait {
        template<class S>
        using Ch = typename string_traits<std::remove_cvref_t<S>>::Ch;

        template<class S>
        using Tr = typename string_traits<std::remove_cvref_t<S>>::Tr;

        template<class S>
        using Sv = std::basic_string_view<Ch<S>, Tr<S>>;

        template<class S, class A>
        using Str = std::basic_string<Ch<S>, Tr<S>, A>;
    }

    namespace detail {
        template <class Sv>
        Sv remainderSv(Sv s, Sv prefix, Sv suffix)
        {
            if (s.length() <= prefix.length() + suffix.length()
                    || !s.starts_with(prefix)
                    || !s.ends_with(suffix))
                return {};
            return s.substr(prefix.length(), s.length() - prefix.length() - suffix.length());
        }

        template <class Sv>
        Sv remainderSv(Sv s, Sv prefix)
        {
            if (s.length() <= prefix.length()
                    || !s.starts_with(prefix))
                return {};
            return s.substr(prefix.length(), s.length() - prefix.length());
        }

        template <class Sv, class A>
        Sv replaceSv(Sv haystack, Sv needle, Sv byWhat,
                trait::Str<Sv,A>& cache)
        {
            if (haystack.find(needle) == Sv::npos)
                return haystack;
            cache = haystack;
            str::replace(cache, needle, byWhat);
            return cache;
        }

        template <class Sv>
        bool containsWord(Sv haystack, Sv needle)
        {
            if (needle.empty())
                return false;

            size_t start = 0;
            while (true){
                auto pos = haystack.find(needle, start);
                if (pos == std::string_view::npos)
                    return false;

                if (auto rt = pos + needle.size();
                        (rt >= haystack.size() || isBlank(haystack[rt]))
                        && (pos == 0 || haystack[pos - 1] == ' ')) {
                    return true;
                }
                start = pos + 1;
            }
        }
    }

    /// @brief remainderSv
    ///    Chops from s prefix and suffix, and returns what remains
    ///         remainderSv("string", "st", "g") = "rin"
    ///    No prefix and/or suffix → returns ⌀
    ///         remainderSv("string", "a", "g") = ""
    template <class S>
    inline auto remainderSv(const S& s, trait::Sv<S> prefix, trait::Sv<S> suffix) -> trait::Sv<S>
        { return detail::remainderSv<trait::Sv<S>>(s, prefix, suffix); }

    /// @brief remainderSv
    ///    Same for prefix only
    template <class S>
    inline auto remainderSv(const S& s, trait::Sv<S> prefix) -> trait::Sv<S>
        { return detail::remainderSv<trait::Sv<S>>(s, prefix); }

    /// @return   haystack or cache
    template <class Sv, class A>
    Sv replaceSv(Sv haystack,
            typename Sv::value_type needle,
            typename Sv::value_type byWhat,
            trait::Str<Sv,A>& cache)
    {
        // Check whether exists
        if (haystack.find(needle) == Sv::npos)
            return haystack;
        // Resize
        if (cache.length() < haystack.length())
            cache.resize(haystack.length());
        // Replace
        auto p = cache.data();
        for (auto v : haystack) {
            *(p++) = (v == needle) ? byWhat : v;
        }
        return { cache.data(), p };
    }

    /// @return   haystack or cache
    template <class S, class A>
    inline auto replaceSv(const S& s,
            trait::Sv<S> needle, trait::Sv<S> byWhat,
            trait::Str<S,A>& cache) -> trait::Sv<S>
        { return detail::replaceSv<trait::Sv<S>, A>(s, needle, byWhat, cache); }

    /// @return [+] string contains word
    template <class S>
    inline bool containsWord(const S& haystack, trait::Sv<S> needle)
        { return detail::containsWord<trait::Sv<S>>(haystack, needle); }

}   // namespace str

namespace detail {
    // A large part of the imlementation was taken from http://stackoverflow.com/a/15912824/3161376 which solved the problems that I had in the old implementation.

    // The string struct will hold our data

        // The declaration of our string struct that will contain the character array
        template<char... str>
        struct string
        {
            // The characters are immediately converted to lowercase when they are put in the array
            static constexpr const char chars[sizeof...(str)+1] = {((str >= 'a' && str <= 'z') ? str - ('a' - 'A') : str)..., '\0'};
            static constexpr size_t length() { return sizeof ...(str); }
            static constexpr std::string_view sv() { return { chars, length() }; }
        };

        // The definition of the above array
        template<char... str>
        constexpr const char string<str...>::chars[sizeof...(str)+1];


    // The apply_range exists so that we can create a structure Class<Indices...> where the amount of indices are based on a count value that is passed
    template<unsigned count, // Amount of indices to still generate
             template<unsigned...> class meta_functor, // The class to which we will apply the indices
             unsigned... indices> // Indices that we generated so far
    struct apply_range
    {
        typedef typename apply_range<count-1, meta_functor, count-1, indices...>::result result; // Add one index and recursively add the others
    };

    template<template<unsigned...> class meta_functor, // The class to which we will apply the indices
             unsigned... indices> // The generated indices
    struct apply_range<0, meta_functor, indices...>
    {
        typedef typename meta_functor<indices...>::result result; // Apply the indices to the passed class and get the string that it produced
    };


    // This is where all the things come together and the string is produced.
    // The lambda_str_type is a struct containing the literal string.
    // The produce struct is what will be given to apply_range to fill in the indices.
    // When result which will be returned from apply_range is the one from here which is the lowercase char array.
    template<typename lambda_str_type>
    struct string_builder
    {
        template<unsigned... indices>
        struct produce
        {
            typedef string<lambda_str_type{}.chars[indices]...> result;
        };
    };

}   // namespace detail

// The way to call it in the code is too complex to be used directly in the code.
// Calling it from a function is also not possible because then the string is a parameter and not a compile time string literal.
// So we use a define to keep the code simple and still put that complex expression directly in the code
// Returning the const char* from this function will still happen at runtime, but the actual conversion to lowercase is fully done on compile time.
#define TOUPPER(string_literal) \
    []{ \
        struct constexpr_string_type { const char * chars = string_literal; }; \
        return detail::apply_range<sizeof(string_literal)-1, detail::string_builder<constexpr_string_type>::produce>::result::chars; \
    }()

#define TOUPPER_SV(string_literal) \
    []{ \
        struct constexpr_string_type { const char * chars = string_literal; }; \
        return detail::apply_range<sizeof(string_literal)-1, detail::string_builder<constexpr_string_type>::produce>::result::sv(); \
    }()
