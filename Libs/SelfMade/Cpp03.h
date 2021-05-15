#ifndef _Cpp03_h_
#define _Cpp03_h_

#if __cplusplus >= 201103L
    // C++11 here

    // A macro to disallow the copy constructor and operator= functions
    // This should be used in the end of class
    #define DISALLOW_COPY_AND_ASSIGN(TypeName)  \
        public:                                     \
            TypeName(const TypeName&) = delete;     \
            TypeName& operator=(const TypeName&) = delete;

    #define DEFAULT_BODY  = default;
    #define HASH_NS std
    #define STD_MOVE(x) std::move(x)
    #define THROWS_EXCEPT noexcept(false)
#else
    // C++03 here
    // Define C++11 keywords
    #define nullptr NULL
    #define override
    // final is actually used somewhere in Shell, don’t have time to rename
    #define noexcept throw()
    #define constexpr

    // A macro to disallow the copy constructor and operator= functions
    // This should be used in the end of class
    #define DISALLOW_COPY_AND_ASSIGN(TypeName)  \
        private:                             \
            TypeName(const TypeName&);       \
            void operator=(const TypeName&);

    #define DEFAULT_BODY {}

    #ifdef __GNUC__
        #define HASH_NS __gnu_cxx
    #else
        #define HASH_NS std
    #endif
    #define unordered_set hash_set
    #define unordered_map hash_map
    #define STD_MOVE(x) (x)
    #define THROWS_EXCEPT
#endif

#if __cplusplus >= 201402L
    // C++14 here
    #define DEPRECATED(msg) [[deprecated(msg)]]
#else
    // C++03 or 11 here
    #define DEPRECATED(msg)
#endif

#if __cplusplus >= 201703L
    // C++17 here
    #define MAYBE_UNUSED [[maybe_unused]]
    #define UNCAUGHT_EXCEPTION (std::uncaught_exceptions())
#else
    // C++03, 11 or 14 here
    #define MAYBE_UNUSED
    #define UNCAUGHT_EXCEPTION (std::uncaught_exception())
#endif

#if __cplusplus>199711L || (__BORLANDC__ >= 1500)
    #define CPP_MOVE_SEMANTICS
#endif

#endif
