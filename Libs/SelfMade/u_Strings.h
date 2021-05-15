#pragma once

#include <string>
#include <vector>
#include <utility>

namespace str {

    inline bool isBlank(char c) { return static_cast<unsigned char>(c) <= 32; }
    void trim(const char* &start, const char* &end);
    std::string_view trimSv(std::string_view s);
    std::vector<std::string_view> splitSv(std::string_view s, char comma, bool skipEmpty = true);

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

}

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
