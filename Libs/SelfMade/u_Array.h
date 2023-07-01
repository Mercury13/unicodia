#ifndef u_ArrayH
#define u_ArrayH

///  Define AT_STL_UNUSED for GCC projects that don't use STL
///  before including this header. This will wipe all STL-connected
///  things from the header and make the resulting EXE more sleek
///  (bad, bad ld linker!).
#ifndef AT_STL_UNUSED
    #ifdef __BORLANDC__
        #pragma option push -pc
    #endif
    #include <iostream>
    #include <fstream>
    #ifdef __BORLANDC__
        #pragma option pop
    #endif
#endif

#include <cstddef>
#include <algorithm>
#include <limits>
#include <cstring>

#include "Cpp03.h"

#if __cplusplus >= 201103L
    #define AT_MULTI_MOVE(x,y,z) std::move((x),(y),(z))
    #define AT_MOVE_SEMANTIC
#else
    #define AT_MULTI_MOVE(x,y,z) std::copy((x),(y),(z))
#endif


/// Uncomment for simple range-check
//#define AT_RANGE_CHECK
//#define AT_VCL_EXCEPTIONS

/// Uncomment for so-called “canary” that tracks for array end
///   (need Embarcadero extensions)
//#define AT_CANARY

/// [+] Canary uses Win32 MessageBox()   [-] throws exceptions
//#define AT_WIN32_CANARY

// Include exceptions
#ifdef AT_RANGE_CHECK
    #ifdef AT_VCL_EXCEPTIONS
        // VCL exception
        #include <vcl.h>
        #define EArrayAt ERangeError
        #define ECanary EExternal
        #define _i2a(s) IntToStr(s)
    #else
        // STL exception
        #include <exception>
        #include <stdexcept>
        #define EArrayAt std::out_of_range
        #define ECanary std::runtime_error
        #if __cplusplus >= 201103L
            #define _i2a(s) std::to_string(s)
        #else
            #include "u_Strings.h"
            #define _i2a(s) str::i2a(s)
        #endif
    #endif
#endif

#ifdef AT_CANARY
    // Some sort of CANA LIVE
    #define AT_CANARY_DATA 0xCA2A718EUL
    #ifdef AT_VCL_EXCEPTIONS
        // VCL exception
        #include <vcl.h>
        #define ECanary EExternal
    #else
        // STL exception
        #include <exception>
        #include <stdexcept>
        #define ECanary std::runtime_error
    #endif

    #ifdef AT_WIN32_CANARY
        #include <windows.h>
    #endif
#endif


#if __cplusplus >= 201103L
    constexpr size_t NOT_FOUND = std::numeric_limits<size_t>::max();
#else
    #define NOT_FOUND (std::numeric_limits<size_t>::max())
#endif


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  Utils                                                                     //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


namespace arrutil {

#ifdef AT_RANGE_CHECK

    template <class Elem>
    Elem& fixAt(Elem* x, size_t N, size_t i)
    {
        if (i >= N) {
            throw EArrayAt("Fix1d::op[]: Range check error ["
                        +_i2a(i) + "/" + _i2a(N) + "]");
        }
        return *(x + i);
    }

#endif

template<class Elem>
size_t find1(const Elem* x, size_t N, const Elem& aValue, size_t aStart)
{
    for (size_t i = aStart; i < N; ++i)
        if (x[i] == aValue)
            return i;
    return NOT_FOUND;
}

#ifndef AT_STL_UNUSED
template<class Elem>
void dumpH(std::ostream& aOs, Elem* x, size_t N)
{
    for (size_t i = 0; i < N; ++i) {
        if (i != 0) aOs << '\t';
        aOs << x[i];
    }
    aOs << std::endl;
}

template<class Elem>
void dumpV(std::ostream& aOs, Elem* x, size_t N)
{
    for (size_t i = 0; i < N; ++i)
        aOs << x[i] << std::endl;
}
#endif

#ifdef AT_CANARY
    template <class Elem>
    size_t canarySize()
    {
        return (__is_trivial(Elem))
            ? ((sizeof(long) + sizeof(Elem) - 1) / sizeof(Elem))
            : 0;
    }
#else
    template <class Elem>
    inline size_t canarySize() { return 0; }
#endif


#ifdef AT_CANARY
    template <class Elem>
    void checkCanary(const Elem* ptr, size_t n)
    {
        if (__is_trivial(Elem)) {
            if (*reinterpret_cast<const unsigned long*>(ptr + n) != AT_CANARY_DATA
                || *reinterpret_cast<const unsigned long*>(ptr - canarySize<Elem>()) != AT_CANARY_DATA)
            {
            #ifdef AT_WIN32_CANARY
                MessageBoxA(0, "Canary is dead, bad code!", "Error", MB_ICONERROR);
            #endif
                throw ECanary("Canary is dead, bad code!");
            }
        }
    }

    template <class Elem>
    void rawDealloc(Elem* ptr, size_t n)
    {
        checkCanary<Elem>(ptr, n);
        delete[] (ptr - canarySize<Elem>());
    }

    #define AT_RAW_DEALLOC arrutil::rawDealloc(buf._ptr, buf._size);
    #define AT_CHECK_CANARY \
        { if (fAutoDestroy) \
           arrutil::checkCanary(buf._ptr, buf._size); }
#else
    #define AT_RAW_DEALLOC delete[] buf._ptr;
    #define AT_CHECK_CANARY
#endif


#if !defined(AT_CANARY)
    // -warn: new Elem[n] makes lots of warnings on GCC+LTO.
    template <class Elem>
    inline Elem* rawAlloc(size_t n) {
        if (n < static_cast<size_t>(PTRDIFF_MAX))
            return new Elem[n];
        else throw std::bad_alloc();
    }
#else
    template <class Elem>
    Elem* rawAlloc(size_t n)
    {
    #ifdef AT_CANARY
        size_t cs = canarySize<Elem>();
        Elem* r = new Elem[n + 2*cs];
        r += cs;

        if (__is_trivial(Elem)) {
            *reinterpret_cast<unsigned long*>(r + n) = AT_CANARY_DATA;
            *reinterpret_cast<unsigned long*>(r - cs) = AT_CANARY_DATA;
        }
    #else
        Elem* r = new Elem[n];
    #endif

        checkCanary(r, n);
        return r;
    }
#endif

}   // namespace arrutil


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  Buf1d                                                                     //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


///
///  @brief  One-dimensional array of everything.
///    The simplest thing possible.
///    It is changeable, but it has no own memory.
///    Just pointer and size.
///
///    To make an unchangeable array, write Buf1d<const int>.
///
template <class Elem>
struct Buf1d
{
    //------------------------------------------------------------------------//
    ///  Iterator type is what it needs to be a container
    ///
    typedef Elem* iterator;
    typedef const Elem* const_iterator;

    size_t _size;
    Elem* _ptr;

    Elem* buffer() const noexcept { return _ptr; }
    size_t size() const noexcept { return _size; }

    constexpr Buf1d() noexcept : _size(0), _ptr(NULL) {}
    constexpr Buf1d(size_t aSize, Elem* aBuffer) noexcept : _size(aSize), _ptr(aBuffer) {}
    void borrow(size_t aSize, Elem *aPtr) noexcept
        { _size = aSize;  _ptr = aPtr;  }

    template<size_t N>
    constexpr Buf1d(Elem(&x)[N]) noexcept : _size(N), _ptr(x) {}

    //------------------------------------------------------------------------//
    ///  Index operator
    ///
    Elem& operator[](size_t i) const
#ifdef AT_RANGE_CHECK
        ;
#else
    { return *(_ptr + i); }
#endif

    //------------------------------------------------------------------------//
    ///  The same as operator[], works as functor
    ///
    Elem& operator() (size_t i) const {  return operator[](i);  }

    //------------------------------------------------------------------------//
    ///  The same as operator[], buf->at(i)
    ///
    Elem& at(size_t i) const { return operator[](i); }

    void borrowSpan(const Buf1d<Elem>& aSrc, size_t aStart, size_t aEnd);

    //------------------------------------------------------------------------//
    ///  Copies an array to memory w/o reallocating
    ///  @pre  Current buffer and rPtr[_size()] shouldn’t intersect
    void copyTo(Elem *rPtr) const
    {
        if (_size==0) return;
        std::copy(_ptr, _ptr + _size, rPtr);
    }

    //------------------------------------------------------------------------//
    ///  The same - for arbitrary-type iterators
    ///  @pre  Current buffer and rPtr[_size()] shouldn’t intersect
    template <class FwdIterator>
    void copyTo(FwdIterator rPtr) const
    {
        if (_size==0) return;
        std::copy(_ptr, _ptr + _size, rPtr);
    }

    //------------------------------------------------------------------------//
    ///  @return  the last element
    ///
    Elem& last() const
    {
#ifdef AT_RANGE_CHECK
        if (_size == 0)
            throw EArrayAt("Buf1d::last() const: Array is empty!");
#endif
        return *(_ptr + _size - 1);
    }

    //------------------------------------------------------------------------//
    ///  the first element
    ///
    Elem& first() const
#ifdef AT_RANGE_CHECK
        ;
#else
        { return *_ptr; }
#endif

    //------------------------------------------------------------------------//
    ///  Returns i’th element from end, const version
    ///
    Elem& fromEnd(size_t i) const
#ifdef AT_RANGE_CHECK
        ;
#else
        { return *(_ptr + _size - 1 - i); }
#endif


    //------------------------------------------------------------------------//
    ///  @return  iterator to beginning
    ///
    iterator begin() const { return _ptr; }

    //------------------------------------------------------------------------//
    ///  @return  iterator to end
    ///
    iterator end() const { return _ptr + _size; }

    //------------------------------------------------------------------------//
    ///  If i is within limits, returns i’th element.
    ///  Otherwide returns aDefault.
    ///  @return    i’th element or aDefault
    ///  @warning   Uses references, so either assign to another object,
    ///             or make aDefault durable enough.
    ///
    const Elem& safeGetC(size_t i, const Elem& aDefault) const
            ///< @return  at(i) or aDefault
        { return (i < _size) ? *(_ptr + i) : aDefault; }

    Elem safeGetV(size_t i, const Elem aDefault) const
            ///< @return  at(i) or aDefault
        { return (i < _size) ? *(_ptr + i) : aDefault; }

    Elem& safeGet(size_t i, Elem& aDefault) const
            ///< @return  at(i) or aDefault
        { return (i < _size) ? *(_ptr + i) : aDefault; }

    //------------------------------------------------------------------------//
    ///  If i is within limits, returns i’th element.
    ///  Otherwide returns NULL
    ///  @return  at(i) or NULL
    Elem* safeGetPtr(size_t i) const
        { return (i < _size) ? _ptr + i : NULL; }

    void safeSet(size_t i, const Elem& aValue)
    {
        if (i < _size)
            _ptr[i] = aValue;
    }

//    operator const Buf1d<const Elem>&() const
//        { return reinterpret_cast<const Buf1d<const Elem>&>(*this); }
    // Removed type-puns
    operator Buf1d<const Elem>() const { return ::Buf1d<const Elem>(_size, _ptr); }

    //------------------------------------------------------------------------//
    ///  Fills the array with specified value
    ///
    void fill(const Elem& aV) const { std::fill_n(_ptr, _size, aV); }

    //------------------------------------------------------------------------//
    ///  Copies an array from memory w/o reallocating
    ///  @pre  The memory aPtr[size()] should be properly allocated, and
    ///        not intersect with current data
    void copyFrom(const Elem* aPtr)
    {
        std::copy(aPtr, aPtr + _size, _ptr);
    }

    //------------------------------------------------------------------------//
    ///  Copies an array from memory w/o reallocating
    ///  @pre  The memory aSrc[n] should be properly allocated, and not
    ///        intersect with current data
    ///  @pre  n <= size()
    ///  @return  dest_end
    Elem* copyNFrom(
            const size_t n,
            const Elem* aSrc)
    {
    #ifdef AT_RANGE_CHECK
        if (n>size())
            throw EArrayAt("Array1d::copyNFrom(): size exceeded ["
                    + _i2a(n) + "/" + _i2a(size()) + "]");
    #endif

        return std::copy(aSrc, aSrc + n, _ptr);
    }

    /// @overload
    Elem* copyNFrom(const size_t n, const Buf1d<const Elem>& aSrc)
    {
    #ifdef AT_RANGE_CHECK
        if (n>size())
            throw EArrayAt("Buf1d::copyNFrom(): my size exceeded ["
                    + _i2a(n) + "/" + _i2a(size()) + "]");
        if (n>aSrc.size())
            throw EArrayAt("Buf1d::copyNFrom(): src size exceeded ["
                    + _i2a(n) + "/" + _i2a(size()) + "]");
    #endif
        return std::copy(aSrc.buffer(), aSrc.buffer() + n, _ptr);
    }


    //------------------------------------------------------------------------//
    ///  Copies an array w/o reallocating
    ///  @overload
    ///  @pre  Buffers of *this and aSrc shouldn’t intersect
    void copyFrom(const Buf1d<Elem>& aSrc)
    {
    #ifdef AT_RANGE_CHECK
        if (aSrc.size()!=size())
            throw EArrayAt("Buf1d::copyFrom(): size mismatch [my="
                    + _i2a(size()) + ", his=" + _i2a(aSrc.size()) + "]");
    #endif
        copyFrom(aSrc.buffer());
    }

    bool isEmpty() const { return !_size; }

    Elem& front() const { return first(); }
    Elem& back() const { return last(); }

    //------------------------------------------------------------------------//
    ///  @return  # of bytes in array
    ///           (=size * sizeof(Elem)
    size_t nBytes() const { return _size * sizeof(Elem); }

    //------------------------------------------------------------------------//
    ///  @return  Buf1d of first n items
    ///  @warning  Never throws!
    Buf1d<Elem> sliceLeft(size_t n)
        { return Buf1d<Elem>(std::min(_size, n), _ptr); }

    Buf1d<const Elem> sliceLeft(size_t n) const
        { return Buf1d<const Elem>(std::min(_size, n), _ptr); }

    Buf1d<const Elem> sliceLeftC(size_t n) const
        { return Buf1d<const Elem>(std::min(_size, n), _ptr); }
};  // template Buf1d


#ifdef AT_RANGE_CHECK

#define AT_BUF1D_MSG(msg) \
    std::string("Buf1d<") + typeid(Elem).name() + ">::" msg

template <class Elem>
Elem& Buf1d<Elem>::operator[](size_t i) const
{
    if (i >= _size) {
        if (_size == 0)
            throw EArrayAt(AT_BUF1D_MSG("op[]: Indexing an empty array!"));
        throw EArrayAt(AT_BUF1D_MSG("op[]: Range check error [")
                    +_i2a(i) + "/" + _i2a(_size) + "]");
    }
    return *(_ptr + i);
}

template <class Elem>
Elem& Buf1d<Elem>::fromEnd(size_t i) const
{
    if (i >= _size) {
        if (_size==0)
            throw EArrayAt(AT_BUF1D_MSG("fromEnd(): Indexing an empty array!"));
        throw EArrayAt(AT_BUF1D_MSG("fromEnd(): Range check error [")
                    + _i2a(i) + "/" + _i2a(_size) + "]");
    }
    return *(_ptr + _size - 1 - i);
}


template <class Elem>
Elem& Buf1d<Elem>::first() const
{
    if (_size == 0)
        throw EArrayAt(AT_BUF1D_MSG("first(): Array is empty!"));
    return *_ptr;
}

#endif


template <class Elem>
void Buf1d<Elem>::borrowSpan(
        const Buf1d<Elem>& aSrc, size_t aStart, size_t aEnd)
{
#ifdef AT_RANGE_CHECK
    if (aStart>aEnd || aEnd>aSrc.size())
        throw EArrayAt(
            AT_BUF1D_MSG("borrowSpan(): range check; params=") + _i2a(aStart)
            + ".." + _i2a(aEnd) + ", allowed=0.." + _i2a(aSrc.size()));
#endif
    borrow(aEnd - aStart, aSrc.buffer() + aStart);
}


namespace arrutil {

    template <class Elem>
    void shiftRight(
            const Buf1d<Elem>& buf,
            const size_t aIndex,
            const size_t aCount)
    {
    #ifdef AT_RANGE_CHECK
        if (aCount > buf._size)
            throw EArrayAt(AT_BUF1D_MSG("shiftRight(): count exceeded [")
                + _i2a(aCount) + "/" + _i2a(buf._size) + "]");
        if (aIndex >= aCount)
            throw EArrayAt(AT_BUF1D_MSG("shiftRight(): range check [")
                + _i2a(aIndex) + "/" + _i2a(aCount) + "]");
    #endif

        Elem* _end = buf.buffer() + aCount;
        std::copy_backward(buf.buffer() + aIndex, _end - 1, _end);
    }


    template <class Elem>
    void shiftRightNe(
            const Buf1d<Elem>& buf,
            const size_t aIndex,
            const size_t aCount)
    {
    #ifdef AT_RANGE_CHECK
        if (aCount > buf._size)
            throw EArrayAt(AT_BUF1D_MSG("shiftRightNe(): count exceeded [")
                + _i2a(aCount) + "/" + _i2a(buf._size) + "]");
    #endif

        if (aIndex >= aCount) return;

        Elem* _end = buf.buffer() + aCount;
        std::copy_backward(buf.buffer() + aIndex, _end - 1, _end);
    }

//    template <class Other, class Elem, size_t N>
//    inline Other (&reinterpret(Elem (&in)[N]))[N]
//        { return reinterpret_cast <Other(&)[N]> (in); }

}   // namespace arrutil



////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  Fix1d helper class                                                        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


template <class Elem, class Other, size_t N, bool Enable> struct _Fix1dConverter;


template <class Elem, class Other, size_t N>
struct _Fix1dConverter<Elem, Other, N, true>
{
    static inline Other (&conv(Elem (&in)[N]))[N]
        { return reinterpret_cast <Other(&)[N]> (in); }
};



////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  Fix1d                                                                     //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

///  @brief  A wrapper around a fixed-size array.
///
template <class Elem, size_t N>
class Fix1d
{
public:
    typedef Elem Array[N];
    typedef const Elem ConstArray[N];
private:
    Array fData;

public:
    //------------------------------------------------------------------------//
    ///  Iterator type is what it needs to be a container
    ///
    typedef Elem* iterator;
    typedef const Elem* const_iterator;

    Fix1d() {}
    // cppcheck-suppress uninitMemberVar
    explicit Fix1d(const Elem& aValue) { fill(aValue); }

    Elem* buffer() { return fData; }
    const Elem* buffer() const { return fData; }
    constexpr size_t size() const { return N; }

    //  Cannot use these because of GCC warnings.
    //operator Elem*() { return fData; }
    //operator const Elem*() const { return fData; }

    //------------------------------------------------------------------------//
    ///  Index operator
    ///
    Elem& operator[](size_t i)
#ifdef AT_RANGE_CHECK
    { return arrutil::fixAt(fData, N, i); }
#else
    { return fData[i]; }
#endif

    const Elem& operator[](size_t i) const
#ifdef AT_RANGE_CHECK
    { return arrutil::fixAt(fData, N, i); }
#else
    { return fData[i]; }
#endif

    //------------------------------------------------------------------------//
    ///  The same as operator[], works as functor
    ///
    Elem& operator() (size_t i) {  return operator[](i);  }
    const Elem& operator() (size_t i) const {  return operator[](i);  }

    //------------------------------------------------------------------------//
    ///  The same as at, buf->at(i)
    ///
    Elem& at(size_t i) { return operator[](i); }
    const Elem& at(size_t i) const { return operator[](i); }

    void borrowSpan(const Buf1d<Elem>& aSrc, size_t aStart, size_t aEnd);

    //------------------------------------------------------------------------//
    ///  Copies an array to memory w/o reallocating
    ///  @pre  Current buffer and rPtr[_size()] shouldn’t intersect
    void copyTo(Elem *rPtr) const
        { std::copy(fData, fData + N, rPtr); }

    //------------------------------------------------------------------------//
    ///  The same - for arbitrary-type iterators
    ///  @pre  Current buffer and rPtr[_size()] shouldn’t intersect
    template <class FwdIterator>
    void copyTo(FwdIterator rPtr) const
        { std::copy(fData, fData + N, rPtr); }

    //------------------------------------------------------------------------//
    ///  Returns the last element
    ///
    Elem& last() { return *(fData + N - 1);  }
    const Elem& last() const { return *(fData + N - 1);  }

    //------------------------------------------------------------------------//
    ///  Returns the first element
    ///
    Elem& first() { return *fData; }
    const Elem& first() const { return *fData; }

    //------------------------------------------------------------------------//
    ///  Returns i’th element from end, const version
    ///
    Elem& fromEnd(size_t i)
#ifdef AT_RANGE_CHECK
        ;
#else
        { return *(fData + (N - 1) - i); }
#endif

    const Elem& fromEnd(size_t i) const
#ifdef AT_RANGE_CHECK
        ;
#else
        { return *(fData + (N - 1) - i); }
#endif


    //------------------------------------------------------------------------//
    ///  @return  iterator to beginning
    ///
    iterator begin() { return fData; }
    const_iterator begin() const { return fData; }

    //------------------------------------------------------------------------//
    ///  @return  iterator to end
    ///
    iterator end() { return fData + N; }
    const_iterator end() const { return fData + N; }

    //------------------------------------------------------------------------//
    ///  If i is within limits, returns i’th element.
    ///  Otherwide returns aDefault.
    ///  @return    i’th element or aDefault
    ///  @warning   Uses references, so either assign to another object,
    ///             or make aDefault durable enough.
    ///
    Elem& safeGet(size_t i, Elem& aDefault)
            ///< @return  at(i) or aDefault
        { return (i < N) ? fData[i] : aDefault; }

    const Elem& safeGetC(size_t i, const Elem& aDefault) const
            ///< @return  at(i) or aDefault
        { return (i < N) ? fData[i] : aDefault; }

    Elem safeGetV(size_t i, Elem aDefault) const
            ///< @return  at(i) or aDefault
        { return (i < N) ? fData[i] : aDefault; }

    void safeSet(size_t i, const Elem& aValue)
        { if (i < N) fData[i] = aValue; }

    void safeSetV(size_t i, Elem aValue)
        { if (i < N) fData[i] = aValue; }

#ifndef AT_STL_UNUSED
    //------------------------------------------------------------------------//
    ///  Dumps horizontally
    ///
    void dumpH(std::ostream& aOs, const size_t aCount)
            { arrutil::dumpH(aOs, fData, aCount); }
    void dumpH(std::ostream& aOs) const
            { arrutil::dumpH(aOs, fData, N); }
    void dumpH() const { arrutil::dumpH(std::cout, fData, N); }

    void dumpH(const char* aFname) const
    {
        std::ofstream os(aFname);
        dumpH(os);
    }

    //------------------------------------------------------------------------//
    ///  Dumps vertically
    ///
    void dumpV(std::ostream& aOs, const size_t aCount)
            { arrutil::dumpV(aOs, fData, aCount); }
    void dumpV(std::ostream& aOs) const
            { arrutil::dumpV(aOs, fData, N); }
    void dumpV() const { arrutil::dumpV(std::cout, fData, N); }

    void dumpV(const char* aFname) const
    {
        std::ofstream os(aFname);
        dumpV(os);
    }
#endif

    //------------------------------------------------------------------------//
    ///  Fills the array with specified value
    ///
    ///  static_cast<size_t> is for Embarcadero:
    ///  enum { N = 10 };   Fix1d<int, N> data
    ///  will instantiate fill_n as <int, enum, int&>.
    ///
    void fill(const Elem& aV)
            { std::fill_n(fData, static_cast<size_t>(N), aV); }

    //------------------------------------------------------------------------//
    /// Finds a value in the array (searching forward)
    ///  NOT_FOUND = not found.
    ///
    size_t find(const Elem& aValue, size_t aStart = 0) const
        { return arrutil::find1(fData, N, aValue, aStart); }

    Array& array() { return fData; }
    ConstArray& carray() const { return fData; }
    //operator Array&() { return fData; }
    //operator ConstArray&() const { return fData; }

    operator const Buf1d<Elem>() { return Buf1d<Elem>(N, fData); }
    operator const Buf1d<const Elem>() const
            { return Buf1d<const Elem>(N, fData); }

    template <class Other>
        Other (&reinterpret()) [N]
            { return _Fix1dConverter<Elem, Other, N, sizeof(Elem) == sizeof(Other)>::conv(fData); }
//    template <class Other>
//        const Other (&reinterpret() const) [N]
//            { return _Fix1dConverter<const Elem, const Other, N, sizeof(const Elem) == sizeof(const Other)>::conv(fData); }
//    template <class Other>
//        const Other (&creinterpret() const) [N]
//            { return _Fix1dConverter<const Elem, const Other, N, sizeof(const Elem) == sizeof(const Other)>::conv(fData); }

    size_t nBytes() const { return N * sizeof(Elem); }
};


#ifdef AT_RANGE_CHECK

template <class Elem, size_t N>
Elem& Fix1d<Elem, N>::fromEnd(size_t i)
{
    if (i >= N) {
        throw EArrayAt("Fix1d::fromEnd(): Range check error ["
                    + _i2a(i) + "/" + _i2a(N) + "]");
    }
    return *(fData + (N - 1) - i);
}


template <class Elem, size_t N>
const Elem& Fix1d<Elem, N>::fromEnd(size_t i) const
{
    if (i >= N) {
        throw EArrayAt("const Fix1d::fromEnd() const: Range check error ["
                    + _i2a(i) + "/" + _i2a(N) + "]");
    }
    return *(fData + N - 1 - i);
}

#endif


template <class Elem> class Array2d;

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  Array2dC                                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


/**
  @brief  Two-dimensional array of const.
    The simplest thing possible.
    It is unchangeable and doesn’t even have its own memory.
    Just pointer and dimensions.
*/
template <class Elem>
class Array2dC
{
protected:
    const Elem *fBuffer;
    size_t fnRows, fnCols;

    const Elem* fastRow(size_t i) const
    {
        return fBuffer + i*fnCols;
    }
public:
    //------------------------------------------------------------------------//
    ///  Iterator type is what it needs to be a container
    ///
    typedef const Elem* iterator;
    typedef const Elem* const_iterator;

    //------------------------------------------------------------------------//
    ///  Simple constructor
    ///
    Array2dC() : fBuffer(NULL), fnRows(0), fnCols(0)  {}

    Array2dC(size_t anRows, size_t anCols, const Elem *aPtr)
        : fBuffer(aPtr), fnRows(anRows), fnCols(anCols) {}

    Array2dC(const Array2d<Elem> &x)
        : fBuffer(x.buffer()), fnRows(x.nRows()), fnCols(x.nCols()) {}

    void borrow(size_t anRows, size_t anCols, const Elem *aPtr)
    {
        fnRows = anRows;
        fnCols = anCols;
        fBuffer = aPtr;
    }

    size_t size() const { return fnRows * fnCols; }
    size_t nRows() const { return fnRows; }
    size_t nCols() const { return fnCols; }
    const Elem* buffer() const { return fBuffer; }

    Array2dC& operator=(const Array2d<Elem> &x)
    {
        fBuffer = x.buffer();
        fnRows = x.nRows();
        fnCols = x.nCols();
        return *this;
    }

    // operator=, copy ctor are default
    void borrow(const Array2dC<Elem>& x) { *this = x; }

    void borrowRows(
            Array2d<Elem>& aSrc,
            size_t aStart,
            size_t aEnd)
    {
    #ifdef AT_RANGE_CHECK
        if (aStart > aEnd || aEnd > aSrc.nRows())
            throw EArrayAt(
                "Array2dC::borrowRows(Array2d): range check; params=" + _i2a(aStart)
                + ".." + _i2a(aEnd) + ", allowed=0.." + _i2a(aSrc.size()));
    #endif
        borrow(aEnd-aStart, aSrc.nRows(), aSrc.rowIter(aStart));
    }

    void borrowRows(
            Array2dC<Elem>& aSrc,
            size_t aStart,
            size_t aEnd)
    {
    #ifdef AT_RANGE_CHECK
        if (aStart > aEnd || aEnd > aSrc.nRows())
            throw EArrayAt(
                "Array1dC::borrowSpan(Array1dC): range check; params=" + _i2a(aStart)
                + ".." + _i2a(aEnd) + ", allowed=0.." + _i2a(aSrc.size()));
    #endif
        borrow(aEnd-aStart, &aSrc.fastRow(aStart));
    }

    //------------------------------------------------------------------------//
    ///  Access to an element
    ///
    const Elem& operator()(size_t i, size_t j) const
    {
#ifdef AT_RANGE_CHECK
        if (i >= fnRows || j >= fnCols)
        {
            if (fnRows == 0 && fnCols == 0)
                throw EArrayAt("Array2dC::op(): Indexing an empty array!");
            throw EArrayAt("Array2dC::op(): Range check error ["
                        + _i2a(i) + "," + _i2a(j) + "/"
                        + _i2a(fnRows) + "," + _i2a(fnCols) + "]");
        }
#endif
        return *(fBuffer + i*fnCols + j);
    }

    //------------------------------------------------------------------------//
    ///  The same, x->at(i,j)
    ///
    const Elem& at(size_t i, size_t j) const
        {  return operator()(i, j);  }

    //------------------------------------------------------------------------//
    ///  Copies an array to memory w/o reallocating
    ///  @pre  Current buffer and rPtr[size()] shouldn’t intersect
    void copyTo(Elem *rPtr) const
    {
        size_t sz = size();
        if (sz == 0) return;
        std::copy(fBuffer, fBuffer + sz, rPtr);
    }

    //------------------------------------------------------------------------//
    ///  The same - for arbitrary-type iterators
    ///  @pre  Current buffer and rPtr[size()] shouldn’t intersect
    template <class FwdIterator>
    void copyTo(
            FwdIterator rPtr) const
    {
        size_t sz = size();
        if (sz == 0) return;
        std::copy(fBuffer, fBuffer + sz, rPtr);
    }

    //------------------------------------------------------------------------//
    ///  Returns the last element
    ///
    const Elem& last() const
    {
#ifdef AT_RANGE_CHECK
        if (size() == 0)
            throw EArrayAt("Array2dC::last(): Array is empty!");
#endif
        return *(fBuffer + size() - 1);
    }

    //------------------------------------------------------------------------//
    ///  Returns the first element
    ///
    const Elem& first() const
    {
#ifdef AT_RANGE_CHECK
        if (size() == 0)
            throw EArrayAt("Array1dC::first(): Array is empty!");
#endif
        return *fBuffer;
    }

    //------------------------------------------------------------------------//
    ///  @return  iterator to beginning
    ///
    iterator begin() const { return fBuffer; }

    //------------------------------------------------------------------------//
    ///  @return  iterator to end
    ///
    iterator end()   const { return fBuffer + size(); }

    //------------------------------------------------------------------------//
    ///  If i,j are within limits, returns (i,j) element.
    ///  Otherwide returns aDefault.
    ///  @return    (i,j) element or aDefault
    ///  @warning   Uses references, so either assign to another object,
    ///             or make aDefault durable enough.
    ///
    const Elem& safeGet(size_t i, size_t j, const Elem& aDefault) const
    {
        return (i < fnRows && j < fnCols)
                ? fBuffer[i*fnCols + j]
                : aDefault;
    }

    const Elem* row(const size_t i) const
    {
#ifdef AT_RANGE_CHECK
        if (i >= fnRows)
        {
            throw EArrayAt("Array2dC::row(): Range check error ["
                + _i2a(i) + "/" + _i2a(fnRows) + "]");
        }
#endif
        return fastRow(i);
    }

    //------------------------------------------------------------------------//
    ///  Returns row iterator
    ///  Differs from row() only with range checking.
    ///
    const Elem* rowIter(size_t i) const
    {
#ifdef AT_RANGE_CHECK
        if (i > fnRows)
        {
            throw EArrayAt("Array2dC::rowIter() const: Range check error ["
                + _i2a(i) + "/" + _i2a(fnRows) + "]");
        }
#endif
        return fastRow(i);
    }

    void lendRowTo(size_t aIndex, Buf1d<const Elem>& rDest) const;

    size_t nBytes() const { return size() * sizeof(Elem); }
};


template <class Elem>
void Array2dC<Elem>::lendRowTo(size_t aIndex, Buf1d<const Elem>& rDest) const
{
#ifdef AT_RANGE_CHECK
    if (aIndex>=fnRows)
    {
        throw EArrayAt("Array2dC::lendRowTo(Buf1d) const: Range check error ["
            + _i2a(aIndex) + "/" + _i2a(fnRows) + "]");
    }
#endif

    rDest.borrow(nCols(), row(aIndex));
}


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  ARRAY1D                                                                   //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------//
///  One-dimensional array of everything
///
template <class Elem>
class Array1d
{
protected:
    Buf1d<Elem> buf;
    /// [+] has its own allocated memory
    bool fAutoDestroy;

    void dealloc();

public:
    void checkCanary() const
    {
        AT_CHECK_CANARY
    }

    //------------------------------------------------------------------------//
    ///  Iterator type is what it needs to be a container
    ///
    typedef Elem* iterator;
    typedef const Elem* const_iterator;


    //------------------------------------------------------------------------//
    ///  @return [+] true if the array owns its memory
    ///
    bool doesOwnMemory()
    {
        return fAutoDestroy;
    }


    //------------------------------------------------------------------------//
    ///  @return [+] array borrows memory from aOther
    ///      (from 0'th element)
    ///
    bool doesBorrowFrom0(const Buf1d<const Elem>& aOther)
        { return (buf._size != 0 && buf._ptr == aOther._ptr); }


    //------------------------------------------------------------------------//
    ///  @return [+] true if the array borrows memory from aOther
    ///      (anywhere)
    ///
    bool doesBorrowFrom(const Buf1d<const Elem>& aOther)
    {
        return (buf._size != 0
                && begin() >= aOther.begin()
                && end() <= aOther.end());
    }


    //------------------------------------------------------------------------//
    ///  Simple constructor
    ///
    Array1d() : fAutoDestroy(false) {}

    //------------------------------------------------------------------------//
    ///  Allocating constructor
    ///
    explicit Array1d(const size_t n) : fAutoDestroy(false)
        { alloc(n); }

#ifdef AT_MOVE_SEMANTIC
    Array1d(Array1d<Elem>&& aSrc) noexcept
        : buf(aSrc.buf), fAutoDestroy(aSrc.fAutoDestroy)
        { aSrc.fAutoDestroy = false; }
#endif


    //------------------------------------------------------------------------//
    ///  Copy constructor
    ///
    Array1d(const Array1d<Elem>& aSrc) : fAutoDestroy(false)
        { allocCopy(aSrc); }


    //------------------------------------------------------------------------//
    ///  Copy constructor
    ///
    explicit Array1d(const Buf1d<const Elem>& aSrc) : fAutoDestroy(false)
        { allocCopy(aSrc); }


    //------------------------------------------------------------------------//
    ///  Destructor
    ///
    ~Array1d()
    {
        dealloc();
    }


    //------------------------------------------------------------------------//
    ///  Makes the array own aSize elements.
    ///  @pre  if it  already owns aSize elements,
    ///      it will NEVER reallocate data. So don't rely on cleanness.
    ///  @warning   Also it won’t save existing information.
    ///
    void alloc(const size_t aSize);


    //------------------------------------------------------------------------//
    ///  The same, but ensures ctor call for non-POD datatypes.
    ///
    void allocCtor(const size_t aSize);


    //------------------------------------------------------------------------//
    ///  Makes the array own at least aWantedSize elements.
    ///  If reallocation occurs, allocates aReservedSize.
    ///  @pre     the call makes sense when aWantedSize<=aReservedSize.
    ///  @return  [+] reallocation occurred
    ///
    bool allocExpand(
            const size_t aWantedSize,
            const size_t aReservedSize);

    /// @overload
    bool allocExpand(
            const size_t aSize)
    {
        return allocExpand(aSize, aSize);
    }


    //------------------------------------------------------------------------//
    ///  Makes the array address aSize elements that aren't owned by Array1d.
    ///
    ///  @pre  The programmer should ensure himself that the memory aPre[aSize]
    ///        is properly allocated.
    ///  @pre  The memory should not be owned by *this.
    void borrow(const size_t aSize, Elem* aPtr)
    {
        dealloc();
        buf._size = aSize;
        buf._ptr = aPtr;
        fAutoDestroy = false;
    }


    //------------------------------------------------------------------------//
    ///  Makes the array address the memory that's owned by another Array1d.
    ///  @overload
    ///  @pre  aSrc isn't *this
    void borrow(const Buf1d<Elem>& aSrc) { borrow(aSrc.size(), aSrc.buffer()); }
    void borrow(Array1d<Elem>& aSrc)     { borrow(aSrc.size(), aSrc.buffer()); }


    //------------------------------------------------------------------------//
    ///  Makes the array address the memory that's used by aSrc.
    ///  @pre  aSrc isn't *this, [aStart..aEnd) should be within aSrc
    void borrowSpan(
            Array1d<Elem>& aSrc,
            size_t aStart,
            size_t aEnd)
    {
    #ifdef AT_RANGE_CHECK
        if (aStart>aEnd || aEnd>aSrc.size())
            throw EArrayAt(
                "Array1d::borrowSpan(): range check; params=" + _i2a(aStart)
                + ".." + _i2a(aEnd) + ", allowed=0.." + _i2a(aSrc.size()));
    #endif
        borrow(aEnd-aStart, &aSrc[aStart]);
    }

    //------------------------------------------------------------------------//
    ///  Copies an array from memory w/o reallocating
    ///  @pre  The memory aPtr[size()] should be properly allocated, and
    ///        not intersect with current data
    void copyFrom(
            const Elem* aPtr)
    {
        if (buf._size == 0) return;
        std::copy(aPtr, aPtr + buf._size, buf._ptr);
        AT_CHECK_CANARY
    }

    //------------------------------------------------------------------------//
    ///  Copies an array from memory w/o reallocating
    ///  @pre  The memory aSrc[n] should be properly allocated, and not
    ///        intersect with current data
    ///  @pre  n <= size()
    ///  @return  dest_end
    Elem* copyNFrom(
            const size_t n,
            const Elem* aSrc)
    {
    #ifdef AT_RANGE_CHECK
        if (n>size())
            throw EArrayAt("Array1d::copyNFrom(): size exceeded ["
                    + _i2a(n) + "/" + _i2a(size()) + "]");
    #endif

        return std::copy(aSrc, aSrc + n, buf._ptr);
        AT_CHECK_CANARY
    }

    /// @overload
    Elem* copyNFrom(
            const size_t n,
            const Array1d<Elem>& aSrc)
    {
    #ifdef AT_RANGE_CHECK
        if (n>size())
            throw EArrayAt("Array1d::copyNFrom(): my size exceeded ["
                    + _i2a(n) + "/" + _i2a(size()) + "]");
        if (n>aSrc.size())
            throw EArrayAt("Array1d::copyNFrom(): src size exceeded ["
                    + _i2a(n) + "/" + _i2a(size()) + "]");
    #endif
        return std::copy(aSrc.buffer(), aSrc.buffer() + n, buf._ptr);
        AT_CHECK_CANARY
    }


    //------------------------------------------------------------------------//
    ///  Copies an array w/o reallocating
    ///  @overload
    ///  @pre  Buffers of *this and aSrc shouldn't intersect
    void copyFrom(
            const Array1d<Elem>& aSrc)
    {
    #ifdef AT_RANGE_CHECK
        if (aSrc.size()!=size())
            throw EArrayAt("Array1d::copyFrom(): size mismatch [my="
                    + _i2a(size()) + ", his=" + _i2a(aSrc.size()) + "]");
    #endif
        copyFrom(aSrc.buffer());
    }

    //------------------------------------------------------------------------//
    ///  Copies an array to memory w/o reallocating
    ///  @pre  Current buffer and rPtr[size()] shouldn’t intersect
    void copyTo(Elem* rPtr) const  { return buf.copyTo(rPtr); }

    //------------------------------------------------------------------------//
    ///  The same - for arbitrary-type iterators
    ///  @pre  Current buffer and rPtr[size()] shouldn't intersect
    template <class FwdIterator>
    void copyTo(FwdIterator rPtr) const
        { return buf.copyTo(rPtr); }


    //------------------------------------------------------------------------//
    ///  Allocates a copy of an array
    ///  Version for an arbitrary memory region
    ///  @pre  aPre[aSize] should be properly allocated and not owned by *this.
    void allocCopy(
            const size_t aSize,    // element count
            const Elem* aPtr)            // pointer to the 1st element
    {
        alloc(aSize);
        copyFrom(aPtr);
    }


    ///  ... version for Array1d
    ///  @overload
    ///  @pre  aSrc shouldn't point to memory owned by *this.
    void allocCopy(const Buf1d<const Elem>& aSrc);
    void allocCopy(const Array1d<Elem>& aSrc);

    void allocCopySpan(
            const Buf1d<const Elem>& aSrc, size_t aStart, size_t aEnd);

    //------------------------------------------------------------------------//
    ///  Fills the array with specified value
    ///
    void fill(const Elem& aV)
    {
        buf.fill(aV);
        AT_CHECK_CANARY
    }

    //------------------------------------------------------------------------//
    ///  Returns a size of array, in items.
    ///
    size_t size() const { return buf._size; }

    /// @return [+] size == 0
    bool isEmpty() const { return !buf._size; }

    //------------------------------------------------------------------------//
    ///  Returns the element at i'th position, const-version
    ///
    const Elem& operator[](const size_t i) const
    {
        AT_CHECK_CANARY
        return buf.operator[](i);
    }


    Elem& operator[](const size_t i)
    {
        AT_CHECK_CANARY
        return buf.operator[](i);
    }

    //------------------------------------------------------------------------//
    ///  The same as operator[], works as functor
    ///
    const Elem& operator()(const size_t i) const { return buf.operator[](i); }
    Elem& operator() (const size_t i) {  return buf.operator[](i);  }

    const Elem& at(const size_t i) const {  return buf.operator[](i);  }
    Elem& at(const size_t i) {  return buf.operator[](i);  }

    //------------------------------------------------------------------------//
    ///  @return  the buffer
    ///
    const Elem* buffer() const { return buf._ptr; }
    Elem* buffer() { return buf._ptr; }

    //------------------------------------------------------------------------//
    ///  @return  the last element. Const version
    ///
    const Elem& last() const { return buf.last(); }
    Elem& last() { return buf.last(); }

    //------------------------------------------------------------------------//
    ///  @return  the first element.
    ///
    const Elem& first() const
    {
        AT_CHECK_CANARY
        return buf.first();
    }

    Elem& first()
    {
        AT_CHECK_CANARY
        return buf.first();
    }

    const Elem& front() const { return first(); }
    Elem& front() { return first(); }

    const Elem& back() const { return last(); }
    Elem& back() { return last(); }


    //------------------------------------------------------------------------//
    ///  Returns i'th element from end, const version
    ///
    const Elem& fromEnd(const size_t i) const
    {
        AT_CHECK_CANARY
        return buf.fromEnd(i);
    }
    Elem& fromEnd(const size_t i)
    {
        AT_CHECK_CANARY
        return buf.fromEnd(i);
    }

    //------------------------------------------------------------------------//
    ///  Swaps with another array
    ///
    void swap(Array1d<Elem>& aOther) noexcept;


    //------------------------------------------------------------------------//
    ///  Captures ownage. If it's impossible, just creates a copy.
    ///
    void capture(Array1d<Elem>& aOther);

    //------------------------------------------------------------------------//
    ///  operator= acts as allocCopy()
    ///
    Array1d<Elem>& operator= (const Array1d<Elem>& aSrc)
        {  allocCopy(aSrc);  return *this;  }
    Array1d<Elem>& operator= (const Buf1d<const Elem>& aSrc)
        {  allocCopy(aSrc);  return *this;  }
#ifdef AT_MOVE_SEMANTIC
    Array1d<Elem>& operator= (Array1d<Elem>&& aSrc)
    {
        if (this != &aSrc) {
//            bool t1 = aSrc.fAutoDestroy;
//            aSrc.fAutoDestroy = fAutoDestroy;
//            fAutoDestroy = t1;
//            Buf1d<Elem> t2 = aSrc.buf;
//            aSrc.buf = buf;
//            buf = t2;
            dealloc();
            fAutoDestroy = aSrc.fAutoDestroy;
            buf = aSrc.buf;
            aSrc.buf._size = 0;
            aSrc.fAutoDestroy = false;
        }
        return *this;
     }
#endif

    //------------------------------------------------------------------------//
    ///  Resizes an array without losing the "head"
    ///
    bool resize(const size_t aNewSize);
    bool resize(const size_t aNewSize, const Elem& aData);

    //------------------------------------------------------------------------//
    ///  Shrinks an array while leaving data in place.
    ///  (unless the size is 0 → in that case it deallocates everything)
    ///  e.g. we can make a data surplus for buggy API →
    ///    array is 1024, but allocated 1032
    ///
    void shrinkInPlace(size_t aNewSize);

    //------------------------------------------------------------------------//
    ///  Extends the array (resizes if the new size is greater)
    ///
    void extend(const size_t aNewSize)
    {
        if (aNewSize > buf._size) resize(aNewSize);
    }

    void extend(const size_t aNewSize, const Elem& aData)
    {
        if (aNewSize > buf._size) resize(aNewSize, aData);
    }

    //------------------------------------------------------------------------//
    ///  Shrinks the array (resizes if the new size is less)
    ///
    void shrink(const size_t aNewSize)
    {
        if (aNewSize < buf._size) resize(aNewSize);
    }

    //------------------------------------------------------------------------//
    ///  Deletes some elements from the array, creating some at the end.
    ///
    ///  @param [in]     aIndex     index of the 1st element
    ///  @param [in]     aCount     number of elements to delete
    ///  @param [in,out] aSize      number of elements that are really allocated
    ///                                Everything beyond aSize isn’t touched,
    ///                                aSize is subtracted by aCount
    bool del(
            const size_t aIndex,
            const size_t aCount,
            size_t& aSize);
                ///< @return true if OK

    ///  @overload  Simplified version: aSize=size, aCount may be omitted
    bool del(
            const size_t aIndex,
            const size_t aCount=1)
                ///< @return true if OK
    {
        size_t q = size();
        return del(aIndex, aCount, q);
    }

    //------------------------------------------------------------------------//
    ///  Shifts array’s content right by 1
    ///  @param [in] aIndex   index-from
    ///  @param [in] aCount   right boundary; item at(aCount-1) is discarded
    void shiftRight(
            const size_t aIndex,
            const size_t aCount)
        { arrutil::shiftRight(buf, aIndex, aCount); }

    //------------------------------------------------------------------------//
    ///  Shifts array’s content right by 1, w/o throwing
    ///    if aIndex>=aCount
    ///  @param [in] aIndex   index-from
    ///  @param [in] aCount   right boundary; item at(aCount-1) is discarded
    void shiftRightNe(
            const size_t aIndex,
            const size_t aCount)
        { arrutil::shiftRightNe(buf, aIndex, aCount); }

    //------------------------------------------------------------------------//
    ///  @overload  Shifts array's content right by 1
    ///  @param [in] aIndex   index-from
    void shiftRight(
            const size_t aIndex)
    {
        arrutil::shiftRight(buf, aIndex, size());
    }

    //------------------------------------------------------------------------//
    ///  Deletes some elements from the array, creating some at the end
    ///    (and posiibly reordering to prevent excess copying)
    ///
    ///  @param [in]     aIndex     index of the 1st element
    ///  @param [in]     aCount     number of elements to delete
    ///  @param [in,out] aSize      number of elements that are really allocated
    ///                                Everything beyond aSize isn't touched,
    ///                                aSize is subtracted by aCount
    bool smartDelete(
            const size_t aIndex,
            const size_t aCount,
            size_t& aSize);
                ///< @return true if OK

    ///  ... simplified version: aSize=size, aCount may be omitted
    bool smartDelete(
            const size_t aIndex,
            const size_t aCount=1)
                ///< @return true if OK
    {
        size_t q = size();
        return smartDelete(aIndex, aCount, q);
    }

    //------------------------------------------------------------------------//
    ///  Trims an array (removes head and tail, leaving only [aStart..aEndPlus)
    ///
    bool trim(
            const size_t aStart,
            const size_t aEndPlus);
                ///< @return true of OK

    //------------------------------------------------------------------------//
    ///  Trims/stuffs an array
    ///    Before: { B E F O R E }
    ///    start = 3, end = 8
    ///    After: { O R E * * }
    ///
    bool trimStuff(
            const ptrdiff_t aStart,
            const ptrdiff_t aEndPlus,
            const Elem& aStuffValue);
                ///< @return true of OK

    //------------------------------------------------------------------------//
    ///  Returns starting iterator
    ///
    iterator begin() { return buf.begin(); }
    const_iterator begin() const { return buf.begin(); }
    const_iterator cbegin() const { return buf.begin(); }

    //------------------------------------------------------------------------//
    ///  Returns ending iterator
    ///
    iterator end() { return buf.end(); }
    const_iterator end() const { return buf.end(); }
    const_iterator cend() const { return buf.end(); }

    //------------------------------------------------------------------------//
    ///  Gets an element, in case of "out of range" gets aDefault
    ///  @param [in]  i         index
    ///  @param [in]  aDefault  what to return if out of error
    Elem safeGet(size_t i, const Elem& aDefault) const
            ///< @return  at(i) or aDefault
        { return buf.safeGetV(i, aDefault); }

    Elem& safeGet(size_t i, Elem& aDefault)
        { return buf.safeGet(i, aDefault); }

    const Elem& safeGetC(size_t i, const Elem& aDefault) const
        { return buf.safeGetC(i, aDefault); }

    Elem* safeGetPtr(size_t i) { return buf.safeGetPtr(i); }
    const Elem* safeGetPtr(size_t i) const { return buf.safeGetPtr(i); }

    void safeSet(size_t i, const Elem& aValue)
        { buf.safeSet(i, aValue); }

    //------------------------------------------------------------------------//
    ///  Copies data using iterators (op- is required!!)
    ///
    template <class Iterator>
    void allocCopy(
            const Iterator aStart,
            const Iterator aEnd);

    //------------------------------------------------------------------------//
    ///  Swaps two items
    ///
    void swapTwo(size_t i, size_t j);

    //------------------------------------------------------------------------//
    ///  In release mode, does nothing.
    ///  In range check mode, it ensures that the buffer is within range.
    ///
#ifdef AT_RANGE_CHECK
    void touch(const Elem* aPtr, size_t aSize) const
    {
        size_t i = aPtr - begin();
        if (i > buf._size || i + aSize > buf._size)
            throw EArrayAt("Array1d::touch() const: Range check error ["
                + _i2a(i) + ":" + _i2a(aSize) + "/" + _i2a(buf._size) + "]");
    }
#else
    inline void touch(const Elem*, size_t) const {}
#endif

    operator const Buf1d<Elem>&() { return buf; }
    operator Buf1d<const Elem>() const
            { return Buf1d<const Elem>(buf._size, buf._ptr); }
    // Removed type-puns
    //operator Buf1d<const Elem>() const
    //    { return reinterpret_cast<const Buf1d<const Elem>&>(buf); }

    const Buf1d<Elem>& toBuf() { return buf; }
    Buf1d<const Elem> toBuf() const { return buf; }
    // Removed type-puns
    //const Buf1d<const Elem>& toBuf() const
    //    { return reinterpret_cast<const Buf1d<const Elem>&>(buf); }

    const Buf1d<Elem>& constCast() const { return buf; }

    size_t nBytes() const { return size() * sizeof(Elem); }

    Buf1d<Elem> sliceLeft(size_t n) { return buf.sliceLeft(n); }
    Buf1d<const Elem> sliceLeft(size_t n) const { return buf.sliceLeftC(n); }
    Buf1d<const Elem> sliceLeftC(size_t n) const { return buf.sliceLeftC(n); }
};


template <class Elem>
void Array1d<Elem>::shrinkInPlace(size_t aNewSize)
{
    if (aNewSize < size()) {
        if (aNewSize == 0) {
            alloc(0);
        } else {
            buf._size = aNewSize;
        }
    }
}


template <class Elem>
bool Array1d<Elem>::resize(const size_t aNewSize)
{
    AT_CHECK_CANARY
    if (buf._size == aNewSize) return false;
    // Array is borrowed, we shrink
    if (!fAutoDestroy && buf._size >= aNewSize)
    {
        buf._size = aNewSize;
        return true;
    }

    if (aNewSize==0)
    {
        alloc(0);
        return true;
    }

    if (aNewSize > size())
    {   // Extend!!
        Elem* data2 = arrutil::rawAlloc<Elem>(aNewSize);

        AT_MULTI_MOVE(buf._ptr, buf._ptr + buf._size, data2);

        if (fAutoDestroy)
            AT_RAW_DEALLOC

        buf._ptr = data2;
        buf._size = aNewSize;
        fAutoDestroy = true;
        AT_CHECK_CANARY
        return true;
    } else
    {   // Shrink!!
        return trim(0, aNewSize);
    }
}


template <class Elem>
bool Array1d<Elem>::resize(const size_t aNewSize, const Elem& aNewData)
{
    AT_CHECK_CANARY
    if (buf._size == aNewSize) return false;
    // Array is borrowed, we shrink
    if (!fAutoDestroy && buf._size >= aNewSize)
    {
        buf._size = aNewSize;
        return true;
    }

    if (aNewSize==0)
    {
        alloc(0);
        return true;
    }

    if (aNewSize > size()) {   // Extend!!
        size_t nExtend = aNewSize - size();
        Elem* data2 = arrutil::rawAlloc<Elem>(aNewSize);

        Elem* newData = std::copy(buf._ptr, buf._ptr + buf._size, data2);
        std::fill_n(newData, nExtend, aNewData);

        if (fAutoDestroy)
            AT_RAW_DEALLOC

        buf._ptr = data2;
        buf._size = aNewSize;
        fAutoDestroy = true;
        AT_CHECK_CANARY
        return true;
    } else
    {   // Shrink!!
        return trim(0, aNewSize);
    }
}


template <class Elem>
void Array1d<Elem>::swap(Array1d<Elem>& aOther) noexcept
{
    AT_CHECK_CANARY
    // Elem*
    Elem* t1 = buf._ptr;
        buf._ptr = aOther.buf._ptr;
        aOther.buf._ptr = t1;
    // bool
    bool t2 = fAutoDestroy;
        fAutoDestroy = aOther.fAutoDestroy;
        aOther.fAutoDestroy = t2;
    // size_t
    size_t t3 = buf._size;
        buf._size = aOther.buf._size;
        aOther.buf._size = t3;
}


template <class Elem>
void Array1d<Elem>::dealloc()
{
    // No autodestroy - no deallocation!
    if (!fAutoDestroy) return;
    // Call destruct, then delete
    AT_RAW_DEALLOC
    // Consequent Deallocs will do nothing.
    fAutoDestroy = false;
}


template <class Elem>
void Array1d<Elem>::alloc(
        const size_t aSize)
{
    // Is allocation-deallocation really needed?
    if (fAutoDestroy && size() == aSize)
        return;

    // Deallocate
    dealloc();

    // Allocate
    buf._size = aSize;
    if (aSize!=0)
    {
        // Allocate - construct.
        // And don't forget fAutoDestroy = true!
        buf._ptr = arrutil::rawAlloc<Elem>(aSize);
        fAutoDestroy = true;
    }
}


template <class Elem>
void Array1d<Elem>::allocCtor(
        const size_t aSize)
{
    alloc(0);
    alloc(aSize);

    // doesn't work!!
//    if (fAutoDestroy && fSize==aSize)
//    {
//        // Dtor in reverse order
//        for (size_t i=fSize; i!=0; )
//        {
//            --i;
//            fBuffer[i].~Elem();
//        }
//        // Ctor in straight order
//        for (size_t i=0; i<fSize; ++i)
//            fBuffer[i].Elem();
//    } else alloc(aSize);
}


template <class Elem>
void Array1d<Elem>::allocCopy(const Array1d<Elem>& aSrc)
{
    // Copying the array from itself isn't healthy!
    // So, check for identity!
    if (this == &aSrc) return;

    alloc(aSrc.size());
    copyFrom(aSrc.buffer());
}


template <class Elem>
void Array1d<Elem>::allocCopy(const Buf1d<const Elem>& aSrc)
{
    alloc(aSrc.size());
    copyFrom(aSrc.buffer());
}


template <class Elem>
bool Array1d<Elem>::del(
        const size_t aIndex,
        const size_t aCount,
        size_t& aSize)
{
    if (aIndex>=size()) return false;
    if (aSize>size()) return false;
    size_t cnt = std::min(aCount, aSize-aIndex);
    if (cnt<=0) return false;

    size_t lastplus = aIndex+cnt;
    if (lastplus<size())
        std::copy(&at(lastplus), &at(aSize), &at(aIndex));

    aSize -= cnt;
    AT_CHECK_CANARY;
    return true;
}

template <class Elem>
bool Array1d<Elem>::smartDelete(
        const size_t aIndex,
        const size_t aCount,
        size_t& aSize)
{
    if (aIndex>=size()) return false;
    if (aSize>size()) return false;
    size_t cnt = std::min(aCount, aSize-aIndex);
    if (cnt<=0) return false;

    //size_t lastplus = aIndex+cnt;
    std::copy(&at(aSize-cnt), &at(aSize), &at(aIndex));

    aSize -= cnt;
    AT_CHECK_CANARY
    return true;
}


template <class Elem>
bool Array1d<Elem>::trim(
        const size_t aStart,
        const size_t aEndPlus)
{
    AT_CHECK_CANARY
    if (aEndPlus > size() || aStart > aEndPlus)
        return false;
    size_t newsize = aEndPlus - aStart;
    if (newsize == size()) return true;

    if (fAutoDestroy) {
        Elem* const data2 = arrutil::rawAlloc<Elem>(newsize);
        AT_MULTI_MOVE(buffer() + aStart, buffer() + aEndPlus, data2);

        AT_RAW_DEALLOC
        buf._ptr = data2;
        buf._size = newsize;
    } else {
        buf._ptr += aStart;
        buf._size = newsize;
    }
    AT_CHECK_CANARY
    return true;
}

template<class Elem>
bool Array1d<Elem>::trimStuff(
        const ptrdiff_t aStart,
        const ptrdiff_t aEndPlus,
        const Elem& aStuffValue)
{
    AT_CHECK_CANARY
    if (aStart > aEndPlus) return false;

    const size_t newsize = aEndPlus - aStart;

    if (!fAutoDestroy && aStart>=0 && aEndPlus<static_cast<ptrdiff_t>(size())) {
        buf._ptr += aStart;
        buf._size = newsize;
        return true;
    }
    if (aStart==0 && aEndPlus==static_cast<ptrdiff_t>(size())) return true;

    size_t oldstart = 0;
    size_t oldend = size();

    // start
    if (aStart > 0) {
        oldstart = aStart;
    }

    // end
    if (aEndPlus < static_cast<ptrdiff_t>(size())) {
        oldend = aEndPlus;
    }

    const size_t newstart = oldstart - aStart;
    const size_t newend = oldend - aStart;

    Elem* const data2 = arrutil::rawAlloc<Elem>(newsize);
    std::fill_n(data2, newstart, aStuffValue);
    std::copy(buffer() + oldstart, buffer() + oldend, data2+newstart);
    std::fill_n(data2+newend, newsize-newend, aStuffValue);

    AT_RAW_DEALLOC
    buf._ptr = data2;
    buf._size = newsize;
    fAutoDestroy = true;

    AT_CHECK_CANARY
    return true;
}


template <class Elem>
template <class Iterator>
void Array1d<Elem>::allocCopy(
        const Iterator aStart,
        const Iterator aEnd)
{
    alloc(aEnd - aStart);
    std::copy(aStart, aEnd, begin());
    AT_CHECK_CANARY
}


template <class Elem>
bool Array1d<Elem>::allocExpand(
        const size_t aWantedSize,
        const size_t aReservedSize)
{
    if (aWantedSize==0)
        return false;
    if (!fAutoDestroy || size() < aWantedSize) {
        alloc(aReservedSize);
        return true;
    } else {
        return false;
    }
    AT_CHECK_CANARY
}


template <class Elem>
void Array1d<Elem>::capture(Array1d<Elem>& aOther)
{
    if (aOther.fAutoDestroy)
    {
        borrow(aOther);
        fAutoDestroy = true;
        aOther.fAutoDestroy = false;
    } else allocCopy(aOther);
}


template <class Elem>
void Array1d<Elem>::swapTwo(size_t i, size_t j)
{
    std::swap<Elem>(at(i), at(j));
}


template <class Elem>
void Array1d<Elem>::allocCopySpan(
        const Buf1d<const Elem>& aSrc, size_t aStart, size_t aEnd)
{
#ifdef AT_RANGE_CHECK
    if (aStart > aSrc.size() || aEnd > aSrc.size())
        throw EArrayAt("Array1d::allocCopySpan(): Range check error ["
            + _i2a(aStart) + ".." + _i2a(aEnd) + "/" + _i2a(aSrc.size()) + "]");
#endif
    if (aStart >= aEnd)
        alloc(0);
        else allocCopy(aEnd - aStart, aSrc.buffer() + aStart);
}



////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  ARRAY1DV                                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


//----------------------------------------------------------------------------//
///  One-dimensional array of everything
///
template <class Elem>
class Array1dV
{
protected:
    Buf1d<Elem> buf;
    /// [+] has its own allocated memory
    bool fAutoDestroy;

    //------------------------------------------------------------------------//
    ///  Called while allocating new elements, only when fAutoDestroy=true
    ///  May be used to implement memory ownage later.
    ///
    virtual void construct(
            size_t aMin, size_t aMaxPlus);

    //------------------------------------------------------------------------//
    ///  Called while destroying elements, only when fAutoDestroy=true
    ///  May be used to implement memory ownage later.
    ///
    virtual void destruct(
            size_t aMin, size_t aMaxPlus);

    //------------------------------------------------------------------------//
    ///  Called while resizing an array for range that's moved
    ///
    virtual void reconstruct(
            size_t aMin, size_t aMaxPlus);

    void dealloc();

public:
    void checkCanary()
    {
    #ifdef AT_CANARY
        if (__is_trivial(Elem))
            if (*reinterpret_cast<const unsigned long*>(end())!=AT_CANARY_DATA)
            {
            #ifdef AT_WIN32_CANARY
                MessageBoxA(0, "Canary is dead, bad code!", "Error", MB_ICONERROR);
            #endif
                throw ECanary("Canary is dead, bad code!");
            }
    #endif
    }

    //------------------------------------------------------------------------//
    ///  Iterator type is what it needs to be a container
    ///
    typedef Elem* iterator;
    typedef const Elem* const_iterator;


    //------------------------------------------------------------------------//
    ///  @return [+] true if the array owns its memory
    ///
    bool doesOwnMemory()
    {
        return fAutoDestroy;
    }


    //------------------------------------------------------------------------//
    ///  @return [+] array borrows memory from aOther
    ///      (from 0'th element)
    ///
    bool doesBorrowFrom0(const Array1dV<Elem>& aOther)
    {
        return (buf._size != 0 && buf._ptr == aOther.buf._ptr);
    }


    //------------------------------------------------------------------------//
    ///  @return [+] true if the array borrows memory from aOther
    ///      (anywhere)
    ///
    bool doesBorrowFrom(const Array1dV<Elem>& aOther)
    {
        return (buf._size != 0 && begin() >= aOther.begin() && end()<=aOther.end());
    }


    //------------------------------------------------------------------------//
    ///  Simple constructor
    ///
    Array1dV() : fAutoDestroy(false) {}

    //------------------------------------------------------------------------//
    ///  Allocating constructor
    ///
    explicit Array1dV(const size_t n) : fAutoDestroy(false)
        { alloc(n); }


    //------------------------------------------------------------------------//
    ///  Copy constructor
    ///
    explicit Array1dV(const Array1dV<Elem>& aSrc) : fAutoDestroy(false)
        { allocCopy(aSrc); }


    //------------------------------------------------------------------------//
    ///  Destructor
    ///
    virtual ~Array1dV()
    {
        dealloc();
    }


    //------------------------------------------------------------------------//
    ///  Makes the array own aSize elements.
    ///  @pre  if it  already owns aSize elements,
    ///      it will NEVER reallocate data. So don't rely on cleanness.
    ///  @warning   Also it won't save exisning information.
    ///
    void alloc(
            const size_t aSize);


    //------------------------------------------------------------------------//
    ///  The same, but ensures ctor call for non-POD datatypes.
    ///
    //  Untested right now!
    void allocCtor(
            const size_t aSize);


    //------------------------------------------------------------------------//
    ///  Makes the array own at least aWantedSize elements.
    ///  If reallocation occurs, allocates aReservedSize.
    ///  @pre  the call makes sense when aWantedSize<=aReservedSize.
    ///
    void allocExpand(
            const size_t aWantedSize,
            const size_t aReservedSize);

    /// @overload
    void allocExpand(
            const size_t aSize)
    {
        allocExpand(aSize, aSize);
    }


    //------------------------------------------------------------------------//
    ///  Makes the array address aSize elements that aren't owned by Array1dV.
    ///
    ///  @pre  The programmer should ensure himself that the memory aPre[aSize]
    ///        is properly allocated.
    ///  @pre  The memory should not be owned by *this.
    void borrow(const size_t aSize, Elem* aPtr)
    {
        dealloc();
        buf._size = aSize;
        buf._ptr = aPtr;
        fAutoDestroy = false;
    }


    //------------------------------------------------------------------------//
    ///  Makes the array address the memory that's owned by another Array1dV.
    ///  @overload
    ///  @pre  aSrc isn't *this
    void borrow(
            Array1dV<Elem>& aSrc)
    {
        borrow(aSrc.size(), aSrc.buffer());
    }


    //------------------------------------------------------------------------//
    ///  Makes the array address the memory that's used by aSrc.
    ///  @pre  aSrc isn't *this, [aStart..aEnd) should be within aSrc
    void borrowSpan(
            Array1dV<Elem>& aSrc,
            size_t aStart,
            size_t aEnd)
    {
    #ifdef AT_RANGE_CHECK
        if (aStart>aEnd || aEnd>aSrc.size())
            throw EArrayAt(
                "Array1dV::borrowSpan(): range check; params=" + _i2a(aStart)
                + ".." + _i2a(aEnd) + ", allowed=0.." + _i2a(aSrc.size()));
    #endif
        borrow(aEnd-aStart, &aSrc[aStart]);
    }

    //------------------------------------------------------------------------//
    ///  Copies an array from memory w/o reallocating
    ///  @pre  The memory aPtr[size()] should be properly allocated, and
    ///        not intersect with current data
    void copyFrom(
            const Elem* aPtr)
    {
        if (buf._size == 0) return;
        std::copy(aPtr, aPtr + buf._size, buf._ptr);
    }

    //------------------------------------------------------------------------//
    ///  Copies an array from memory w/o reallocating
    ///  @pre  The memory aSrc[n] should be properly allocated, and not
    ///        intersect with current data
    ///  @pre  n <= size()
    ///  @return  dest_end
    Elem* copyNFrom(
            const size_t n,
            const Elem* aSrc)
    {
    #ifdef AT_RANGE_CHECK
        if (n>size())
            throw EArrayAt("Array1dV::copyNFrom(): size exceeded ["
                    + _i2a(n) + "/" + _i2a(size()) + "]");
    #endif

        return std::copy(aSrc, aSrc + n, buf._ptr);
    }

    /// @overload
    Elem* copyNFrom(
            const size_t n,
            const Array1dV<Elem>& aSrc)
    {
    #ifdef AT_RANGE_CHECK
        if (n>size())
            throw EArrayAt("Array1dV::copyNFrom(): my size exceeded ["
                    + _i2a(n) + "/" + _i2a(size()) + "]");
        if (n>aSrc.size())
            throw EArrayAt("Array1dV::copyNFrom(): src size exceeded ["
                    + _i2a(n) + "/" + _i2a(size()) + "]");
    #endif
        return std::copy(aSrc.buffer(), aSrc.buffer() + n, buf._ptr);
    }


    //------------------------------------------------------------------------//
    ///  Copies an array w/o reallocating
    ///  @overload
    ///  @pre  Buffers of *this and aSrc shouldn't intersect
    void copyFrom(
            const Array1dV<Elem>& aSrc)
    {
    #ifdef AT_RANGE_CHECK
        if (aSrc.size()!=size())
            throw EArrayAt("Array1dV::copyFrom(): size mismatch [my="
                    + _i2a(size()) + ", his=" + _i2a(aSrc.size()) + "]");
    #endif
        copyFrom(aSrc.buffer());
    }

    //------------------------------------------------------------------------//
    ///  Copies an array to memory w/o reallocating
    ///  @pre  Current buffer and rPtr[size()] shouldn’t intersect
    void copyTo(Elem* rPtr) const  { return buf.copyTo(rPtr); }

    //------------------------------------------------------------------------//
    ///  The same - for arbitrary-type iterators
    ///  @pre  Current buffer and rPtr[size()] shouldn't intersect
    template <class FwdIterator>
    void copyTo(FwdIterator rPtr) const
        { return buf.copyTo(rPtr); }


    //------------------------------------------------------------------------//
    ///  Allocates a copy of an array
    ///  Version for an arbitrary memory region
    ///  @pre  aPre[aSize] should be properly allocated and not owned by *this.
    void allocCopy(
            const size_t aSize,    // element count
            const Elem* aPtr)            // pointer to the 1st element
    {
        alloc(aSize);
        copyFrom(aPtr);
    }


    ///  ... version for Array1dV
    ///  @overload
    ///  @pre  aSrc shouldn't point to memory owned by *this.
    void allocCopy(
            const Array1dV<Elem>& aSrc);

    //------------------------------------------------------------------------//
    ///  Fills the array with specified value
    ///
    void fill(const Elem& aV) { buf.fill(aV); }

    //------------------------------------------------------------------------//
    ///  Fills the array with 0's
    ///  @warning  This function is extremely fast, but don't use it for
    ///       non-POD types!
//    void fill0()
//    {
//        memset(fBuffer, 0, sizeof(Elem)*fSize);
//    }

    //------------------------------------------------------------------------//
    ///  Returns a size of array, in items.
    ///
    size_t size() const { return buf._size; }

    //------------------------------------------------------------------------//
    ///  Returns the element at i'th position, const-version
    ///
    const Elem& operator[](const size_t i) const { return buf.operator[](i); }
    Elem& operator[](const size_t i) { return buf.operator[](i); }

    //------------------------------------------------------------------------//
    ///  The same as operator[], works as functor
    ///
    const Elem& operator()(const size_t i) const { return buf.operator[](i); }
    Elem& operator() (const size_t i) {  return buf.operator[](i);  }

    const Elem& at(const size_t i) const {  return buf.operator[](i);  }
    Elem& at(const size_t i) {  return buf.operator[](i);  }

    //------------------------------------------------------------------------//
    ///  @return  the buffer
    ///
    const Elem* buffer() const { return buf._ptr; }
    Elem* buffer() { return buf._ptr; }

    //------------------------------------------------------------------------//
    ///  @return  the last element. Const version
    ///
    const Elem& last() const { return buf.last(); }
    Elem& last() { return buf.last(); }

    //------------------------------------------------------------------------//
    ///  @return  the first element.
    ///
    const Elem& first() const { return buf.first(); }
    Elem& first() { return buf.first(); }

    //------------------------------------------------------------------------//
    ///  Returns i'th element from end, const version
    ///
    const Elem& fromEnd(const size_t i) const { return buf.fromEnd(i); }
    Elem& fromEnd(const size_t i) { return buf.fromEnd(i); }

    //------------------------------------------------------------------------//
    ///  Swaps with another array
    ///
    void swap(Array1dV<Elem>& aOther) noexcept;


    //------------------------------------------------------------------------//
    ///  Captures ownage. If it's impossible, just creates a copy.
    ///
    void capture(Array1dV<Elem>& aOther);

    //------------------------------------------------------------------------//
    ///  operator= acts as allocCopy()
    ///
    Array1dV<Elem>& operator= (const Array1dV<Elem>& aSrc)
    {
        allocCopy(aSrc);
        return *this;
    }

    //------------------------------------------------------------------------//
    ///  Resizes an array without losing the "head"
    ///
    bool resize(const size_t aNewSize);

    //------------------------------------------------------------------------//
    ///  Extends the array (resizes if the new size is greater)
    ///
    void extend(const size_t aNewSize)
    {
        if (aNewSize > buf._size) resize(aNewSize);
    }

    //------------------------------------------------------------------------//
    ///  Shrinks the array (resizes if the new size is less)
    ///
    void shrink(const size_t aNewSize)
    {
        if (aNewSize < buf._size) resize(aNewSize);
    }

    //------------------------------------------------------------------------//
    ///  Deletes some elements from the array, creating some at the end.
    ///
    ///  @param [in]     aIndex     index of the 1st element
    ///  @param [in]     aCount     number of elements to delete
    ///  @param [in,out] aSize      number of elements that are really allocated
    ///                                Everything beyond aSize isn't touched,
    ///                                aSize is subtracted by aCount
    bool del(
            const size_t aIndex,
            const size_t aCount,
            size_t& aSize);
                ///< @return true if OK

    ///  @overload  Simplified version: aSize=size, aCount may be omitted
    bool del(
            const size_t aIndex,
            const size_t aCount=1)
                ///< @return true if OK
    {
        size_t q = size();
        return del(aIndex, aCount, q);
    }

    //------------------------------------------------------------------------//
    ///  Shifts array's content right by 1
    ///  @param [in] aIndex   index-from
    ///  @param [in] aCount   right boundary; item at(aCount-1) is discarded
    void shiftRight(
            const size_t aIndex,
            const size_t aCount);

    //------------------------------------------------------------------------//
    ///  Shifts array's content right by 1, w/o throwing
    ///    if aIndex>=aCount
    ///  @param [in] aIndex   index-from
    ///  @param [in] aCount   right boundary; item at(aCount-1) is discarded
    void shiftRightNe(
            const size_t aIndex,
            const size_t aCount);

    //------------------------------------------------------------------------//
    ///  @overload  Shifts array's content right by 1
    ///  @param [in] aIndex   index-from
    bool shiftRight(
            const size_t aIndex)
    {
        return shiftRight(aIndex, size());
    }

    //------------------------------------------------------------------------//
    ///  Deletes some elements from the array, creating some at the end
    ///    (and posiibly reordering to prevent excess copying)
    ///
    ///  @param [in]     aIndex     index of the 1st element
    ///  @param [in]     aCount     number of elements to delete
    ///  @param [in,out] aSize      number of elements that are really allocated
    ///                                Everything beyond aSize isn't touched,
    ///                                aSize is subtracted by aCount
    bool smartDelete(
            const size_t aIndex,
            const size_t aCount,
            size_t& aSize);
                ///< @return true if OK

    ///  ... simplified version: aSize=size, aCount may be omitted
    bool smartDelete(
            const size_t aIndex,
            const size_t aCount=1)
                ///< @return true if OK
    {
        size_t q = size();
        return smartDelete(aIndex, aCount, q);
    }

    //------------------------------------------------------------------------//
    ///  Trims an array (removes head and tail, leaving only [aStart..aEndPlus)
    ///
    bool trim(
            const size_t aStart,
            const size_t aEndPlus);
                ///< @return true of OK

    //------------------------------------------------------------------------//
    ///  Trims/stuffs an array
    ///    Before: { B E F O R E }
    ///    start = 3, end = 8
    ///    After: { O R E * * }
    ///
    bool trimStuff(
            const ptrdiff_t aStart,
            const ptrdiff_t aEndPlus,
            const Elem& aStuffValue);
                ///< @return true of OK

    //------------------------------------------------------------------------//
    /// Finds a value in the array (searching forward)
    ///  NOT_FOUND = not found.
    ///
    size_t find(
            const Elem& aValue,
            const size_t aStart = 0) const;
                ///< @return  NOT_FOUND = not found, pos = found

    //------------------------------------------------------------------------//
    ///  Returns starting iterator
    ///
    iterator begin() { return buf.begin(); }
    const_iterator begin() const { return buf.begin(); }

    //------------------------------------------------------------------------//
    ///  Returns ending iterator
    ///
    iterator end() { return buf.end(); }
    const_iterator end() const { return buf.end(); }

    //------------------------------------------------------------------------//
    ///  Gets an element, in case of "out of range" gets aDefault
    ///  @param [in]  i         index
    ///  @param [in]  aDefault  what to return if out of error
    const Elem& safeGet(size_t i, const Elem& aDefault) const
            ///< @return  at(i) or aDefault
        { return buf.safeGetC(i, aDefault); }

    Elem& safeGet(size_t i, Elem& aDefault)
        { return buf.safeGet(i, aDefault); }

    //------------------------------------------------------------------------//
    ///  Copies data using iterators (op- is required!!)
    ///
    template <class Iterator>
    void allocCopy(
            const Iterator aStart,
            const Iterator aEnd);

    //------------------------------------------------------------------------//
    ///  Swaps two items
    ///
    void swapTwo(size_t i, size_t j);

    //------------------------------------------------------------------------//
    ///  In release mode, does nothing.
    ///  In range check mode, it ensures that the buffer is within range.
    ///
#ifdef AT_RANGE_CHECK
    void touch(const Elem* aPtr, size_t aSize) const
    {
        size_t i = aPtr - begin();
        if (i > buf._size || i + aSize > buf._size)
            throw EArrayAt("Array1dV::touch() const: Range check error ["
                + _i2a(i) + ":" + _i2a(aSize) + "/" + _i2a(buf._size) + "]");
    }
#else
    void touch(const Elem*, size_t) const {}
#endif

#ifndef AT_STL_UNUSED
    //------------------------------------------------------------------------//
    ///  Dumps horizontally
    ///
    void dumpH(std::ostream& aOs, const size_t aCount) const
        { buf.dumpH(aOs, aCount); }
    void dumpH(std::ostream& aOs) const { buf.dumpH(aOs); }
    void dumpH(const char* aFname) const { buf.dumpH(aFname); }
#endif

    operator const Buf1d<Elem>&() { return buf; }
    operator const Buf1d<const Elem>&() const
        { return reinterpret_cast<const Buf1d<const Elem>&>(buf); }
};


template <class Elem>
void Array1dV<Elem>::construct(size_t, size_t) {}

template <class Elem>
void Array1dV<Elem>::destruct(size_t, size_t) {}

template <class Elem>
void Array1dV<Elem>::reconstruct(size_t, size_t) {}

template <class Elem>
bool Array1dV<Elem>::resize(const size_t aNewSize)
{
    if (buf._size == aNewSize) return false;
    if (!fAutoDestroy && size() >= aNewSize)
    {
        buf._size = aNewSize;
        return true;
    }

    if (aNewSize==0)
    {
        alloc(0);
        return true;
    }

    if (aNewSize > size())
    {   // Extend!!
        Elem* data2 = arrutil::rawAlloc<Elem>(aNewSize);

        std::copy(buf._ptr, buf._ptr + buf._size, data2);
        //memcpy(data2, fBuffer, aNewSize*sizeof(Elem));

        if (fAutoDestroy)
            AT_RAW_DEALLOC

        buf._ptr = data2;
        size_t oldsize = buf._size;    buf._size = aNewSize;
        reconstruct(0, oldsize);
        construct(oldsize, aNewSize);
        fAutoDestroy = true;
        return true;
    } else
    {   // Shrink!!
        return trim(0, aNewSize);
    }
}

template <class Elem>
void Array1dV<Elem>::swap(Array1dV<Elem>& aOther) noexcept
{
    // Elem*
    Elem* t1 = buf._ptr;
        buf._ptr = aOther.buf._ptr;
        aOther.buf._ptr = t1;
    // bool
    bool t2 = fAutoDestroy;
        fAutoDestroy = aOther.fAutoDestroy;
        aOther.fAutoDestroy = t2;
    // size_t
    size_t t3 = buf._size;
        buf._size = aOther.buf._size;
        aOther.buf._size = t3;
}


template <class Elem>
void Array1dV<Elem>::dealloc()
{
    // No autodestroy - no deallocation!
    if (!fAutoDestroy) return;
    AT_CHECK_CANARY;
    // Call destruct, then delete
    destruct(0, buf._size);
    AT_RAW_DEALLOC
    // Consequent Deallocs will do nothing.
    fAutoDestroy = false;
}


template <class Elem>
void Array1dV<Elem>::alloc(
        const size_t aSize)
{
    AT_CHECK_CANARY;
    // Is allocation-deallocation really needed?
    if (fAutoDestroy && size() == aSize)
    {
        destruct(0, aSize);
        construct(0, aSize);
        return;
    }

    // Deallocate
    dealloc();

    // Allocate
    buf._size = aSize;
    if (aSize!=0)
    {
        // Allocate - construct.
        // And don't forget fAutoDestroy = true!
        buf._ptr = arrutil::rawAlloc<Elem>(aSize);
        fAutoDestroy = true;
        AT_CHECK_CANARY;
        construct(0, aSize);
    }
    AT_CHECK_CANARY;
}


template <class Elem>
void Array1dV<Elem>::allocCtor(
        const size_t aSize)
{
    alloc(0);
    alloc(aSize);

    AT_CHECK_CANARY;

    // doesn't work!!
//    if (fAutoDestroy && fSize==aSize)
//    {
//        // Dtor in reverse order
//        for (size_t i=fSize; i!=0; )
//        {
//            --i;
//            fBuffer[i].~Elem();
//        }
//        // Ctor in straight order
//        for (size_t i=0; i<fSize; ++i)
//            fBuffer[i].Elem();
//    } else alloc(aSize);
}


template <class Elem>
void Array1dV<Elem>::allocCopy(
        const Array1dV<Elem>& aSrc)
{
    // Copying the array from itself isn't healthy!
    // So, check for identity!
    if (this == &aSrc) return;

    alloc(aSrc.size());
    copyFrom(aSrc.buffer());
}


template <class Elem>
bool Array1dV<Elem>::del(
        const size_t aIndex,
        const size_t aCount,
        size_t& aSize)
{
    if (aIndex>=size()) return false;
    if (aSize>size()) return false;
    size_t cnt = std::min(aCount, aSize-aIndex);
    if (cnt<=0) return false;

    destruct(aIndex, aIndex+cnt);

    size_t lastplus = aIndex+cnt;
    if (lastplus<size())
        std::copy(&at(lastplus), &at(aSize), &at(aIndex));

    construct(aSize-cnt, aSize);

    aSize -= cnt;
    return true;
}

template <class Elem>
bool Array1dV<Elem>::smartDelete(
        const size_t aIndex,
        const size_t aCount,
        size_t& aSize)
{
    if (aIndex>=size()) return false;
    if (aSize>size()) return false;
    size_t cnt = std::min(aCount, aSize-aIndex);
    if (cnt<=0) return false;

    destruct(aIndex, aIndex+cnt);

    //size_t lastplus = aIndex+cnt;
    std::copy(&at(aSize-cnt), &at(aSize), &at(aIndex));

    construct(aSize-cnt, aSize);

    aSize -= cnt;
    return true;
}


template <class Elem>
bool Array1dV<Elem>::trim(
        const size_t aStart,
        const size_t aEndPlus)
{
    if (aEndPlus>size() || aStart>aEndPlus) return false;
    size_t newsize = aEndPlus - aStart;
    if (newsize == size()) return true;
    if (fAutoDestroy) {
        bool b = true;
        // First destruct end, THEN beginning
        if (aEndPlus < size())
            { destruct(aEndPlus, size());  b = false; }
        if (aStart > 0)
            { destruct(0, aStart);  b = false; }

        if (b) return true;

        Elem* const data2 = arrutil::rawAlloc<Elem>(newsize);

        std::copy(buffer() + aStart, buffer() + aEndPlus, data2);
        //memcpy(data2, fBuffer+aStart, newsize*sizeof(Elem));

        AT_RAW_DEALLOC
        buf._ptr = data2;
        buf._size = newsize;
        reconstruct(0, size());
    } else {
        buf._ptr += aStart;
        buf._size = newsize;
    }
    return true;
}

template<class Elem>
bool Array1dV<Elem>::trimStuff(
        const ptrdiff_t aStart,
        const ptrdiff_t aEndPlus,
        const Elem& aStuffValue)
{
    if (aStart>aEndPlus) return false;

    const size_t newsize = aEndPlus - aStart;

    if (!fAutoDestroy && aStart>=0 && aEndPlus<static_cast<ptrdiff_t>(size())) {
        buf._ptr += aStart;
        buf._size = newsize;
        return true;
    }
    if (aStart==0 && aEndPlus==static_cast<ptrdiff_t>(size())) return true;

    size_t oldstart = 0;
    size_t oldend = size();

    // start
    if (aStart>0)
    {
        if (fAutoDestroy)
            destruct(0, aStart);
        oldstart = aStart;
    }

    // end
    if (aEndPlus < static_cast<ptrdiff_t>(size()))
    {
        if (fAutoDestroy)
            destruct(aEndPlus, size());
        oldend = aEndPlus;
    }

    const size_t newstart = oldstart - aStart;
    const size_t newend = oldend - aStart;

    Elem* const data2 = arrutil::rawAlloc<Elem>(newsize);
    std::fill_n(data2, newstart, aStuffValue);
    std::copy(buffer() + oldstart, buffer() + oldend, data2+newstart);
    std::fill_n(data2+newend, newsize-newend, aStuffValue);

    AT_RAW_DEALLOC
    buf._ptr = data2;
    buf._size = newsize;
    fAutoDestroy = true;
    reconstruct(0, size());

    return true;
}


template<class Elem>
size_t Array1dV<Elem>::find(
        const Elem& aValue,
        const size_t aStart) const
{
    for (size_t i=aStart; i<size(); i++)
        if (at(i)==aValue) return i;
    return NOT_FOUND;
}


template <class Elem>
template <class Iterator>
void Array1dV<Elem>::allocCopy(
        const Iterator aStart,
        const Iterator aEnd)
{
    alloc(aEnd - aStart);
    std::copy(aStart, aEnd, begin());
}


template <class Elem>
void Array1dV<Elem>::shiftRight(
        const size_t aIndex,
        const size_t aCount)
{
#ifdef AT_RANGE_CHECK
    if (aCount > buf._size)
        throw EArrayAt("Array1dV::ShiftRight(): count exceeded ["
            + _i2a(aCount) + "/" + _i2a(buf._size) + "]");
    if (aIndex >= aCount)
        throw EArrayAt("Array1dV::ShiftRight(): range check ["
            + _i2a(aIndex) + "/" + _i2a(aCount) + "]");
#endif

    Elem* _end = buffer() + aCount;
    std::copy_backward(buffer() + aIndex, _end - 1, _end);
}


template <class Elem>
void Array1dV<Elem>::shiftRightNe(
        const size_t aIndex,
        const size_t aCount)
{
#ifdef AT_RANGE_CHECK
    if (aCount > buf._size)
        throw EArrayAt("Array1dV::ShiftRightNe(): count exceeded ["
            + _i2a(aCount) + "/" + _i2a(buf._size) + "]");
#endif

    if (aIndex >= aCount) return;

    Elem* _end = buffer() + aCount;
    std::copy_backward(buffer() + aIndex, _end - 1, _end);
}


template <class Elem>
void Array1dV<Elem>::allocExpand(
        const size_t aWantedSize,
        const size_t aReservedSize)
{
    if (aWantedSize==0) return;
    if (!fAutoDestroy || size() > aWantedSize)
        alloc(aReservedSize);
}


template <class Elem>
void Array1dV<Elem>::capture(Array1dV<Elem>& aOther)
{
    if (aOther.fAutoDestroy)
    {
        borrow(aOther);
        fAutoDestroy = true;
        aOther.fAutoDestroy = false;
    } else allocCopy(aOther);
}


template <class Elem>
void Array1dV<Elem>::swapTwo(size_t i, size_t j)
{
    std::swap<Elem>(at(i), at(j));
}



////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  Array2d                                                                   //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


//----------------------------------------------------------------------------//
///  Two-dimensional array of everything
///
template <class Elem>
class Array2d : protected Array1d <Elem>
{
protected:
    size_t fnCols;
    size_t fnRows;

    using Array1d<Elem>::buf;

    using Array1d<Elem>::trim;
    using Array1d<Elem>::fAutoDestroy;

    void setSize(
            const size_t aRowCount,
            const size_t aColCount)
    {
        fnRows = aRowCount;
        fnCols = aColCount;
    }

    //------------------------------------------------------------------------//
    ///  Returns beginning of i'th row w/o triggering range-check.
    ///  Protected; used when we exactly know that we're in range.
    ///
    Elem* fastRow(size_t i)
        { return buf._ptr + i * fnCols; }

    ///  @overload  const version
    const Elem* fastRow(size_t i) const
        { return buf._ptr + i * fnCols; }

public:
    typedef Elem* iterator;
    typedef const Elem* const_iterator;

    //  Things inherited from Array1d
    using Array1d<Elem>::size;
    using Array1d<Elem>::first;
    using Array1d<Elem>::last;
    using Array1d<Elem>::fill;
    //using Array1d<Elem>::fill0;
    using Array1d<Elem>::doesOwnMemory;
    using Array1d<Elem>::doesBorrowFrom0;
    using Array1d<Elem>::doesBorrowFrom;
    using Array1d<Elem>::checkCanary;

    using Array1d<Elem>::begin;
    using Array1d<Elem>::end;

    using Array1d<Elem>::touch;

    //inline operator const Buf1d<const Elem>&() const
    //    { return reinterpret_cast<const Buf1d<const Elem>&>(buf); }
    // Removed type-puns
    inline operator const Buf1d<Elem>&() { return buf; }
    inline operator Buf1d<const Elem>() const
                { return Buf1d<const Elem>(buf._size, buf._ptr); }

    //bool doesBorrowFrom0(const Array2d<Elem>& aOther)
    //    {  return Array1d<Elem>::doesBorrowFrom0(aOther);  }

    bool doesBorrowFrom(const Buf1d<Elem>& aOther)
        {  return Array1d<Elem>::doesBorrowFrom(aOther);  }

    bool doesBorrowFrom0(const Buf1d<Elem>& aOther)
        {  return Array1d<Elem>::doesBorrowFrom0(aOther);  }

    bool doesBorrowFrom(const Array2d<Elem>& aOther)
        { return Array1d<Elem>::doesBorrowFrom(aOther.toBuf()); }

    bool doesBorrowFrom0(const Array2d<Elem>& aOther)
        { return Array1d<Elem>::doesBorrowFrom0(aOther.toBuf()); }

    void copyFrom(
            const Elem* aPtr)
    {
        Array1d<Elem>::copyFrom(aPtr);
    }

    //------------------------------------------------------------------------//
    ///  Copies an array w/o reallocating
    ///
    void copyFrom(const Array2d<Elem>& aSrc)
    {
    #ifdef AT_RANGE_CHECK
        if (aSrc.nRows()!=nRows() || aSrc.nCols()!=nCols())
            throw EArrayAt("Array2d::copyFrom(): size mismatch [my="
                + _i2a(nRows()) + "x" + _i2a(nCols()) + ", his="
                + _i2a(aSrc.nRows()) + "x" + _i2a(aSrc.nCols()) + "]");
    #endif
        Array1d<Elem>::copyFrom(aSrc.buffer());
    }

    //------------------------------------------------------------------------//
    /// Constructor - creates an empty object
    ///
    Array2d() : Array1d<Elem>()
    {
        fnRows = 0;
        fnCols = 0;
    }

    //------------------------------------------------------------------------//
    ///  Constructor - creates an allocated object
    ///
    Array2d(
            const size_t aRowCount,
            const size_t aColCount) : Array1d<Elem>()
    {
        alloc(aRowCount, aColCount);
    }

    //------------------------------------------------------------------------//
    ///  Constructor - creates an object parasiting on the buffer
    ///
    Array2d(
            const size_t aRowCount,
            const size_t aColCount,
            Elem* aPtr) : Array1d<Elem>()
    {
        borrow(aRowCount, aColCount, aPtr);
    }

    //------------------------------------------------------------------------//
    ///  Copy constructor
    ///
    Array2d(const Array2d<Elem>& a) : Array1d<Elem>()
    {
        allocCopy(a);
    }

#ifdef AT_MOVE_SEMANTIC
    Array2d(Array2d<Elem>&& x);
#endif

    //------------------------------------------------------------------------//
    ///  Resizes Array2d, purging its memory
    ///
    void alloc(size_t aRowCount, size_t aColCount)
    {
        Array1d<Elem>::alloc(aRowCount*aColCount);
        setSize(aRowCount, aColCount);
        AT_CHECK_CANARY
    }

#ifndef AT_STL_UNUSED
    //------------------------------------------------------------------------//
    ///  Dumps the Array2d to an STL bytestream
    ///
    ///  @param [in,out] aOs        output stream
    ///  @param [in]     aRowCount  row limit
    ///  @param [in]     aColCount  column limit
    ///  @param [in]     aDivider   what to put between matrix items
    ///
    void dump(
            std::ostream& aOs,
            size_t aRowCount,
            size_t aColCount,
            char aDivider = '\t') const;

    //------------------------------------------------------------------------//
    ///  Dumps the Array2d to an STL bytestream
    ///
    void dump(std::ostream& aOs, char aDivider = '\t') const
    {
        AT_CHECK_CANARY
        dump(aOs, fnRows, fnCols, aDivider);
    }

    //------------------------------------------------------------------------//
    ///  Dumps the Array2d to std::cout
    ///
    void dump(
            const size_t aRowCount,
            const size_t aColCount,
            char aDivider = '\t') const
    {
        AT_CHECK_CANARY
        dump(std::cout, aRowCount, aColCount, aDivider);
    }

    //------------------------------------------------------------------------//
    ///  Dumps the Array2d to std::cout
    ///
    void dump(char aDivider = '\t') const
    {
        AT_CHECK_CANARY
        dump(std::cout, aDivider);
    }

    void dump(
            const char* aFname,
            char aDivider = '\t') const
    {
        std::ofstream os(aFname);
        dump(os, aDivider);
    }
#endif

    //------------------------------------------------------------------------//
    ///  Makes the Array2d depend on some memory buffer.
    ///
    void borrow(
            const size_t aRowCount,
            const size_t aColCount,
            Elem* aPtr)
    {
        Array1d<Elem>::borrow(aRowCount*aColCount, aPtr);
        setSize(aRowCount, aColCount);
    }

    //------------------------------------------------------------------------//
    ///  Makes the array address other Array2d's rows.
    ///
    void borrowRows(
            Array2d<Elem>& aSrc,
            const size_t aStartRow,
            const size_t aEndRow)
    {
    #ifdef AT_RANGE_CHECK
        if (aStartRow>aEndRow || aEndRow>aSrc.nRows())
            throw EArrayAt(
                "Array2d::borrowRows(): range check; params=" + _i2a(aStartRow)
                + ".." + _i2a(aEndRow) + ", allowed=0.." + _i2a(aSrc.nRows()));
    #endif
        borrow(aEndRow-aStartRow, aSrc.nCols(), aSrc.row(aStartRow));
    }

    //------------------------------------------------------------------------//
    ///  Makes a clone of aSrc that depends on its memory
    ///  (and, of course, changes together with it).
    ///
    void borrow(
            Array2d<Elem>& aSrc)
    {
        borrow(aSrc.fnRows, aSrc.fnCols, aSrc.buf._ptr);
    }

    void lendRowTo(size_t aIndex, Array1d<Elem>& rDest);
    void lendRowTo(size_t aIndex, Buf1d<const Elem>& rDest) const;
    void lendRowTo(size_t aIndex, Buf1d<Elem>& rDest);

    void lendRowSpanTo(
            size_t aIndex,
            size_t aStart,
            size_t aEnd,
            Array1d<Elem>& rDest);

    using Array1d<Elem>::buffer;

    //------------------------------------------------------------------------//
    ///  Makes a complete clone of aSource
    ///
    void allocCopy(const Array2d<Elem> &aSource)
    {
        allocCopy(aSource.fnRows, aSource.fnCols, aSource.buf._ptr);
    }

    //------------------------------------------------------------------------//
    ///  Copies information from aSource buffer
    ///
    void allocCopy(
            const size_t aRowCount,
            const size_t aColCount,
            const Elem* aSource);

    //------------------------------------------------------------------------//
    /// Takes information from another Array2d and converts
    ///  using aColLayout.
    ///
    ///  @param [in]  aSource      source
    ///  @param [in]  aColCount    column count
    ///  @param [in]  aColLayout   column layout (<0 = empty, >=0 = source row)
    void takeFrom(
            const Array2d &aSource,
            const size_t aColCount,
            const ptrdiff_t* aColLayout);        // >=0 - fill;  -1 - empty

    //------------------------------------------------------------------------//
    /// Takes a sequence of rows from an Array2d.
    ///
    bool takeRowsFrom(
            const Array2d& aSource,
            size_t aRowStart,              // index of the 1st row
            size_t aRowEndPlus);           // index of last row + 1
                ///< @return  true if OK

    //------------------------------------------------------------------------//
    ///  Leaves just a part of rows from an ArrayObject.
    ///
    bool trimRows(
            const size_t aStart,      // index of 1st row
            const size_t aEndPlus);   // index of last row + 1

    //------------------------------------------------------------------------//
    ///  Fills a column with a specified value
    ///
    void fillColumn(
            const size_t aIndex,
            const Elem aFillValue);

    //------------------------------------------------------------------------//
    ///  Fills a row with a specified value
    ///
    void fillRow(
            const size_t aIndex,
            const Elem aFillValue);

    //------------------------------------------------------------------------//
    ///  Return Array2d's dimensions
    ///
    size_t nRows() const { return fnRows; }
    size_t nCols() const { return fnCols; }

    //------------------------------------------------------------------------//
    ///  Returns row iterator
    ///  Differs from row() only with range checking.
    ///
    const_iterator rowIter(size_t i) const
    {
#ifdef AT_RANGE_CHECK
        if (i > fnRows) {
            throw EArrayAt("Array2d::rowIter() const: Range check error ["
                + _i2a(i) + "/" + _i2a(fnRows) + "]");
        }
#endif
        AT_CHECK_CANARY
        return fastRow(i);
    }

    //------------------------------------------------------------------------//
    ///  Returns row iterator
    ///  Differs from Row() only with range checking.
    iterator rowIter(size_t i)
    {
#ifdef AT_RANGE_CHECK
        if (i > fnRows) {
            throw EArrayAt("Array2d::rowIter(): Range check error ["
                + _i2a(i) + "/" + _i2a(fnRows) + "]");
        }
#endif
        AT_CHECK_CANARY
        return fastRow(i);
    }

    //------------------------------------------------------------------------//
    ///  Returns i'th row. Literally the same as operator[].
    ///
    const Elem* row(const size_t i) const
    {
#ifdef AT_RANGE_CHECK
        if (i>=fnRows)
        {
            throw EArrayAt("Array2d::row() const: Range check error ["
                + _i2a(i) + "/" + _i2a(fnRows) + "]");
        }
#endif
        AT_CHECK_CANARY
        return fastRow(i);
    }

    //------------------------------------------------------------------------//
    ///  Returns i'th row. Literally the same as operator[].
    ///
    Elem* row(const size_t i)
    {
#ifdef AT_RANGE_CHECK
        if (i>=fnRows)
        {
            throw EArrayAt("Array2d::row(): Range check error ["
                + _i2a(i) + "/" + _i2a(fnRows) + "]");
        }
#endif
        return fastRow(i);
    }

    //------------------------------------------------------------------------//
    ///  Returns i'th row from end.
    ///
    const Elem* rowFromEnd(const size_t i) const
    {
        return row(fnRows - 1 - i);
    }

    //------------------------------------------------------------------------//
    ///  Returns i'th row from end.
    ///
    Elem* rowFromEnd(const size_t i)
    {
        return row(fnRows - 1 - i);
    }

    //------------------------------------------------------------------------//
    ///  Gets element in 0'th row: faster.
    ///  Well, such access function often come in pairs
    ///
    const Elem& row0(const size_t j) const
    {
#ifdef AT_RANGE_CHECK
        if (size()==0)
            throw EArrayAt("Array2d::row0() const: Indexing an empty array!");
        if (j>=fnCols)
            throw EArrayAt("Array2d::row0() const: Range check error ["
                + _i2a(j) + "/" + _i2a(fnCols) + "]");
#endif
        AT_CHECK_CANARY
        return *(buffer() + j);
    }

    Elem& row0(const size_t j)
    {
#ifdef AT_RANGE_CHECK
        if (size()==0)
            throw EArrayAt("Array2d::row0(): Indexing an empty array!");
        if (j>=fnCols)
            throw EArrayAt("Array2d::row0(): Range check error ["
                + _i2a(j) + "/" + _i2a(fnCols) + "]");
#endif
        AT_CHECK_CANARY
        return *(buffer() + j);
    }

    //------------------------------------------------------------------------//
    ///  Exchanges content with another array
    ///
    void swap(Array2d<Elem>& aOther) noexcept
    {
        Array1d<Elem>::swap(aOther);
        // size_t
        size_t t1 = fnRows;
            fnRows = aOther.fnRows;
            aOther.fnRows = t1;
        t1 = fnCols;
            fnCols = aOther.fnCols;
            aOther.fnCols = t1;
    }

    //------------------------------------------------------------------------//
    ///  operator= is allocCopy
    ///
    Array2d<Elem>& operator= (const Array2d<Elem>& aSrc)
    {
        allocCopy(aSrc);
        AT_CHECK_CANARY
        return *this;
    }

#ifdef AT_MOVE_SEMANTIC
    Array2d<Elem>& operator= (Array2d<Elem>&& src);
#endif

    //------------------------------------------------------------------------//
    ///  At is more preferable than a[i][j] because of range check.
    ///  Also useful as "a->At(i,j)".
    ///
    const Elem& at(const size_t i, const size_t j) const
    {
#ifdef AT_RANGE_CHECK
        AT_CHECK_CANARY
        if (i>=fnRows || j>=fnCols)
        {
            if (size()==0)
                throw EArrayAt("Array2d::const at(): Indexing an empty array!");
            throw EArrayAt("Array2d::const at(): Range check error ("
                     + _i2a(i) + "/" + _i2a(fnRows) + ", "
                     + _i2a(j) + "/" + _i2a(fnCols) + ")");
        }
#endif
        AT_CHECK_CANARY
        return buffer()[i*fnCols + j];
    }

    //------------------------------------------------------------------------//
    ///  At is more preferable than a[i][j] because of range check.
    ///  Also useful as "a->At(i,j)".
    ///
    Elem& at(const size_t i, const size_t j)
    {
#ifdef AT_RANGE_CHECK
        AT_CHECK_CANARY
        if (i>=fnRows || j>=fnCols)
        {
            if (size()==0)
                throw EArrayAt("Array2d::at(): Indexing an empty array!");
            throw EArrayAt("Array2d::at(): Range check error ("
                     + _i2a(i) + "/" + _i2a(fnRows) + ", "
                     + _i2a(j) + "/" + _i2a(fnCols) + ")");
        }
#endif
        AT_CHECK_CANARY
        return buffer()[i*fnCols + j];
    }

    //------------------------------------------------------------------------//
    ///  The same as at(), works as functor
    ///
    const Elem& operator() (const size_t i, const size_t j) const
        {  return at(i,j);  }

    //------------------------------------------------------------------------//
    ///  The same, non-const version
    ///  @overload
    ///
    Elem& operator() (const size_t i, const size_t j)
        {  return at(i,j);  }

    //------------------------------------------------------------------------//
    ///  Gets overall size of row, bytes
    ///
    size_t nBytesPerRow() const  { return sizeof(Elem)*nCols(); }
    size_t nBytes() const { return size() * sizeof(Elem); }

    //------------------------------------------------------------------------//
    ///  Copies a row from a non-overlapping memory region
    ///
    void copyRowFrom(
            size_t aRow,
            const Elem* aBuf)
    {
#ifdef AT_RANGE_CHECK
        if (aRow>=fnRows)
        {
            throw EArrayAt("Array2d::copyRowFrom: Range check error ["
                + _i2a(aRow) + "/" + _i2a(fnRows) + "]");
        }
#endif
        AT_CHECK_CANARY
        std::copy(aBuf, aBuf+fnCols, row(aRow));
        //memcpy(Row(aRow), aBuf, RowSize());
    }

    //------------------------------------------------------------------------//
    ///  Copies a row from another array.
    ///  If widths are inequal, the wider one is trimmed / left intact.
    ///
    void copyRowFrom(
            size_t aRow,
            const Array2d<Elem>& aSrc,
            size_t aSrcRow);

    //------------------------------------------------------------------------//
    ///  Copies n items to a row.
    ///  n <= nCols().
    ///
    void copyNToRow(
            const size_t aRow,
            const size_t n,
            const Elem* aSrc)
    {
#ifdef AT_RANGE_CHECK
        if (n>fnCols)
            throw EArrayAt("Array2d::copyNToRow: Size exceeded ["
                + _i2a(n) + "/" + _i2a(fnCols) + "]");
        if (aRow>=fnRows)
            throw EArrayAt("Array2d::copyNToRow: Range check error ["
                + _i2a(aRow) + "/" + _i2a(fnRows) + "]");
#endif
        AT_CHECK_CANARY
        std::copy(aSrc, aSrc+n, row(aRow));
    }

    //------------------------------------------------------------------------//
    ///  Copies a row to a non-overlapping memory region
    ///
    void copyRowTo(
            const size_t aRow,
            Elem* rBuf) const
    {
#ifdef AT_RANGE_CHECK
        if (aRow>=fnRows)
        {
            throw EArrayAt("Array2d::copyRowTo: Range check error ["
                + _i2a(aRow) + "/" + _i2a(fnRows) + "]");
        }
#endif

        const Elem* _row = row(aRow);
        std::copy(_row, _row+fnCols, rBuf);
        //memcpy(rBuf, Row(aRow), RowSize());
        AT_CHECK_CANARY
    }

    //------------------------------------------------------------------------//
    ///  Copies a row to a non-overlapping Array1d.
    ///  If the resulting array is larger/smaller,
    ///    it's underfull / partly copied
    void copyRowTo(
            const size_t aRow,
            Array1d<Elem>& rOut) const;

    void copyRowTo(
            const size_t aRow,
            Buf1d<Elem>& rOut) const;

    //------------------------------------------------------------------------//
    ///  Extracts a single row
    ///
    void extractRow(
            const size_t aRow,
            Array1d<Elem>& rResult) const;

    //------------------------------------------------------------------------//
    ///  Extracts a single column
    ///
    void extractColumn(
            const size_t aColumn,
            Array1d<Elem>& rResult) const;

    //------------------------------------------------------------------------//
    ///  Extracts a part of column
    ///
    void extractColumn(
            const size_t aColumn,
            const size_t aRowMin,
            const size_t aRowMaxPlus,
            Array1d<Elem>& rResult) const;

    //------------------------------------------------------------------------//
    ///  Swaps two rows
    ///
    void swapRows(
            const size_t i1, const size_t i2);


    //------------------------------------------------------------------------//
    ///  Fills a column incrementally (1, 2...)
    ///
    void enumerateColumn(
            const size_t aIndex,
            Elem aStart);

    //------------------------------------------------------------------------//
    ///  Shifts down by 1 row
    ///
    void shiftDown(
            const size_t aRow,
            const size_t aCount);

    void shiftDownNe(
            const size_t aRow,
            const size_t aCount);

    void shiftDown(
            const size_t aRow)
    {
        shiftDown(aRow, fnRows);
    }


    //------------------------------------------------------------------------//
    ///  Gets an element, w/o triggering a range check.
    ///  If we're out of range, returns aDefault.
    const Elem& safeGet(
            size_t aRow, size_t aCol, const Elem& aDefault) const
    {
        if (aRow<nRows() && aCol<nCols())
            return at(aRow, aCol);
            else return aDefault;
    }


    //------------------------------------------------------------------------//
    ///  Sets an element, w/o triggering a range check.
    ///  If we're out of range, does nothing.
    void safeSet(
            size_t aRow, size_t aCol, const Elem& aValue)
    {
        if (aRow<nRows() && aCol<nCols())
            at(aRow, aCol) = aValue;
    }


    //------------------------------------------------------------------------//
    ///  Copies column (probably with shift)
    ///  @param [in]  aCol        column index
    ///  @param [in]  aSource     source arrayobject
    ///  @param [in]  aSrcCol     source column index
    ///  @param [in]  aShift      shift (<0 = up, >0 = down).
    ///  @param [in]  aFillValue  value to fill the vacant cells with
    void copyColumnFrom(
            size_t aCol,
            const Array2d<Elem>& aSource,
            size_t aSrcCol,
            ptrdiff_t aShift,
            Elem aFillValue);


    //------------------------------------------------------------------------//
    ///  @overload   aShift==0
    void copyColumnFrom(
            size_t aCol,
            const Array2d<Elem>& aSource,
            size_t aSrcCol,
            Elem aFillValue)
    {
        copyColumnFrom(aCol, aSource, aSrcCol, 0, aFillValue);
    }


    //------------------------------------------------------------------------//
    ///  Captures ownage from aOther (if aOther really owns)
    ///  Otherwise, just makes a copy.
    ///
    void capture(Array2d<Elem>& aOther);

    //------------------------------------------------------------------------//
    ///  Resizes Array2d without losing filled upper-left corner.
    ///  The array automatically becomes "owned", not "borrowed".
    ///
    void resize(size_t aNewRows, size_t aNewCols);

    //------------------------------------------------------------------------//
    ///  Extends Array2d to at least aNewRows*aNewCols
    ///    without losing content.
    ///  @return [+] the array was reallocated.
    ///
    bool extend(size_t aNewRows, size_t aNewCols);

    //------------------------------------------------------------------------//
    ///  Resizes array to aNewRows*aNewCols.
    ///  If the array is auto-allocated, and we fit into existing buffer -
    ///    do not reallocate, use it instead!
    ///  Data is possibly lost.
    ///  @return [+] the array was actually reallocated.
    ///
    bool fitAlloc(size_t aNewRows, size_t aNewCols);

    //------------------------------------------------------------------------//
    ///  Assigns a transposed version of x.
    ///
    void transpose(const Array2d<Elem> &x);

    Array2d<Elem>& constCast() const
            { return const_cast<Array2d<Elem>&>(*this); }
};


template <class Elem>
void Array2d<Elem>::allocCopy(
        const size_t aRowCount,
        const size_t aColCount,
        const Elem* aSource)
{
    Array1d<Elem>::allocCopy(aRowCount*aColCount, aSource);
    setSize(aRowCount, aColCount);
}


#ifdef AT_MOVE_SEMANTIC
    template <class Elem>
    Array2d<Elem>::Array2d(Array2d<Elem>&& x)
        : Array1d<Elem>(std::move(x)),
          fnCols(x.fnCols), fnRows(x.fnRows)
    { x.fnRows = 0; x.fnCols = 0; }

    template <class Elem>
    Array2d<Elem>& Array2d<Elem>::operator= (Array2d<Elem>&& src)
    {
        Array1d<Elem>::operator=(std::move(src));
        fnRows = src.fnRows;   fnCols = src.fnCols;
        src.fnRows = 0;        src.fnCols = 0;
        return *this;
    }
#endif


#ifndef AT_STL_UNUSED
template <class Elem>
void Array2d<Elem>::dump(
        std::ostream& aOs,
        size_t aRowCount,
        size_t aColCount,
        char aDivider) const
{
    AT_CHECK_CANARY
    for (size_t i=0; i<aRowCount; i++)
    {
        for (size_t j=0; j<aColCount; j++)
        {
            if (j!=0) aOs << aDivider;
            aOs << at(i,j);
        }
        aOs << std::endl;
    }
}
#endif


template <class Elem>
void Array2d<Elem>::takeFrom(
        const Array2d &aSource,
        const size_t aColCount,
        const ptrdiff_t* aColLayout)   // >=0 - fill;  -1 - empty
{
    size_t src_rowcount = aSource.nRows();
    alloc(src_rowcount,aColCount);

    for (size_t j=0; j<aColCount; j++)
    {
        ptrdiff_t src_col;
        if ((src_col=aColLayout[j])<0) continue;

        for (size_t i=0; i<src_rowcount; i++)
            at(i,j) = aSource.at(i,src_col);
    }
    AT_CHECK_CANARY
}


template <class Elem>
bool Array2d<Elem>::takeRowsFrom(
        const Array2d<Elem>& aSource,
        const size_t aRowStart,
        const size_t aRowEndPlus)
{
    // Null?
    if (aSource.nRows()==0) return false;
    // Erroneous input??
    if (aRowEndPlus>aSource.nRows() || aRowEndPlus<=aRowStart)
        return false;

    allocCopy(aRowEndPlus-aRowStart, aSource.nCols(),
            aSource.row(aRowStart));
    return true;
}


/*template <class Elem>
bool Array2d<Elem>::TrimRows(
        const size_t aStart,
        const size_t aEndPlus)
{
    if (aStart==0 && aEndPlus==nRows()) return true;
    //std::cout << "Time to trim! " << aStart << '-' <<  aEndPlus
    //         << '/' << nRows() << std::endl;
    Array2d<Elem> a;
    if (!a.TakeRowsFrom(*this, aStart, aEndPlus))
        return false;
    swap(a);
    return true;
}*/


template <class Elem>
bool Array2d<Elem>::trimRows(
        const size_t aStart,
        const size_t aEndPlus)
{
    if (aStart==0 && aEndPlus==nRows()) return true;
    if (aEndPlus<aStart || aEndPlus>nRows()) return false;
    //std::cout << "Time to trim! " << aStart << '-' <<  aEndPlus
    //         << '/' << GetRowCount() << std::endl;
    bool b = trim(aStart*fnCols, aEndPlus*fnCols);
    if (b)
    {
        setSize(aEndPlus - aStart, fnCols);
    }

    AT_CHECK_CANARY
    return b;
}


template <class Elem>
void Array2d<Elem>::fillColumn(
        const size_t aIndex,
        const Elem aFillValue)
{
#ifdef AT_RANGE_CHECK
    if (aIndex>=fnCols)
    {
        throw EArrayAt("Array2d::fillColumn: Range check error ["
            + _i2a(aIndex) + "/" + _i2a(fnCols) + "]");
    }
#endif

    Elem* p = &row0(aIndex);
    for (size_t i=0; i<fnRows; i++)
    {
        *p = aFillValue;
        p += fnCols;
    }

    AT_CHECK_CANARY
}


template <class Elem>
void Array2d<Elem>::fillRow(
        const size_t aIndex,
        const Elem aFillValue)
{
#ifdef AT_RANGE_CHECK
    if (aIndex >= fnRows)
    {
        throw EArrayAt("Array2d::fillRow: Range check error ["
            + _i2a(aIndex) + "/" + _i2a(fnRows) + "]");
    }
#endif

    if (size() == 0)
        return;
    std::fill_n(fastRow(aIndex), fnCols, aFillValue);
    AT_CHECK_CANARY
}



template <class Elem>
void Array2d<Elem>::extractRow(
        const size_t aRow,
        Array1d<Elem>& rResult) const
{
#ifdef AT_RANGE_CHECK
        if (aRow>=fnRows)
        {
            throw EArrayAt("Array2d::extractRow: Range check error ["
                + _i2a(aRow) + "/" + _i2a(fnRows) + "]");
        }
#endif

    rResult.allocCopy(nCols(), row(aRow));
    AT_CHECK_CANARY
}


template <class Elem>
void Array2d<Elem>::extractColumn(
        const size_t aColumn,
        const size_t aRowMin,
        const size_t aRowMaxPlus,
        Array1d<Elem>& rResult) const
{
#ifdef AT_RANGE_CHECK
    if (aColumn>=fnCols || aRowMin>aRowMaxPlus || aRowMaxPlus>fnRows)
    {
        throw EArrayAt("Array2d::extractColumn(): Range check error [col="
            + _i2a(aColumn) + "/" + _i2a(fnCols) + ", row=" + _i2a(aRowMin) + ".."
            + _i2a(aRowMaxPlus) + "/" + _i2a(fnRows) + "]");
    }
#endif

    size_t len = aRowMaxPlus - aRowMin;
    rResult.alloc(len);
    for (size_t i=0; i<len; i++)
        rResult[i] = at(i+aRowMin, aColumn);
    AT_CHECK_CANARY
}


template <class Elem>
void Array2d<Elem>::extractColumn(
        const size_t aColumn,
        Array1d<Elem>& rResult) const
{
#ifdef AT_RANGE_CHECK
    if (aColumn>=fnCols)
    {
        throw EArrayAt("Array2d::extractColumn(): Range check error ["
            + _i2a(aColumn) + "/" + _i2a(fnCols) + "]");
    }
#endif

    rResult.alloc(nRows());
    for (size_t i=0; i<nRows(); i++)
        rResult[i] = at(i, aColumn);
    AT_CHECK_CANARY
}


template <class Elem>
void Array2d<Elem>::swapRows(
        const size_t i1, const size_t i2)
{
#ifdef AT_RANGE_CHECK
    if (i1>=fnRows || i2>=fnRows)
    {
        throw EArrayAt("Array2d::swapRows: Range check error ["
            + _i2a(i1) + "+" + _i2a(i2) + "/" + _i2a(fnRows) + "]");
    }
#endif
    AT_CHECK_CANARY

    if (i1==i2) return;

    // Dumb version
    Elem* start = row(i1);
    std::swap_ranges(start, start+fnCols, row(i2));
    AT_CHECK_CANARY
}


template <class Elem>
void Array2d<Elem>::enumerateColumn(
        const size_t aIndex,
        Elem aStart)
{
#ifdef AT_RANGE_CHECK
    if (aIndex>=fnCols)
    {
        throw EArrayAt("Array2d::enumerateColumn(): Range check error ["
            + _i2a(aIndex) + "/" + _i2a(fnCols) + "]");
    }
#endif
    AT_CHECK_CANARY

    Elem* p = &buf._ptr[aIndex];
    for (size_t i=0; i<fnRows; i++)
    {
        *p = aStart++;
        p += fnCols;
    }

    AT_CHECK_CANARY
}


template <class Elem>
void Array2d<Elem>::lendRowTo(size_t aIndex, Array1d<Elem>& rDest)
{
#ifdef AT_RANGE_CHECK
    if (aIndex>=fnRows)
    {
        throw EArrayAt("Array2d::lendRowTo(Array1d): Range check error ["
            + _i2a(aIndex) + "/" + _i2a(fnRows) + "]");
    }
#endif

    rDest.borrow(nCols(), row(aIndex));
}

template <class Elem>
void Array2d<Elem>::lendRowTo(size_t aIndex, Buf1d<const Elem>& rDest) const
{
#ifdef AT_RANGE_CHECK
    if (aIndex>=fnRows)
    {
        throw EArrayAt("Array2d::lendRowTo(Buf1d) const: Range check error ["
            + _i2a(aIndex) + "/" + _i2a(fnRows) + "]");
    }
#endif

    rDest.borrow(nCols(), row(aIndex));
}

template <class Elem>
void Array2d<Elem>::lendRowTo(size_t aIndex, Buf1d<Elem>& rDest)
{
#ifdef AT_RANGE_CHECK
    if (aIndex>=fnRows)
    {
        throw EArrayAt("Array2d::lendRowTo(Buf1d): Range check error ["
            + _i2a(aIndex) + "/" + _i2a(fnRows) + "]");
    }
#endif

    rDest.borrow(nCols(), row(aIndex));
}

template <class Elem>
void Array2d<Elem>::lendRowSpanTo(
        size_t aIndex,
        size_t aStart,
        size_t aEnd,
        Array1d<Elem>& rDest)
{
#ifdef AT_RANGE_CHECK
    if (aIndex>=fnRows || aEnd<aStart || aStart>fnCols)
    {
        throw EArrayAt("Array2d::lendRowSpanTo(): Range check error ["
            + _i2a(aIndex) + "/" + _i2a(fnRows) + ", "
            + _i2a(aStart) + "-" + _i2a(aEnd) + "/" + _i2a(fnCols) + "]");
    }
#endif

    rDest.borrow(aEnd-aStart, &at(aIndex, aStart));
}

template <class Elem>
void Array2d<Elem>::shiftDown(
        const size_t aRow,
        const size_t aCount)
{
#ifdef AT_RANGE_CHECK
    if (aCount>fnRows)
        throw EArrayAt("Array2d::shiftDown(): count exceeded ["
            + _i2a(aCount) + "/" + _i2a(fnRows) + "]");
    if (aRow>=aCount)
        throw EArrayAt("Array2d::shiftDown(): range check ["
            + _i2a(aRow) + "/" + _i2a(aCount) + "]");
#endif
    AT_CHECK_CANARY

    if (aRow+1>=aCount) return;
    std::copy_backward(rowIter(aRow), rowIter(aCount-1), rowIter(aCount));

    AT_CHECK_CANARY
}


template <class Elem>
void Array2d<Elem>::shiftDownNe(
        const size_t aRow,
        const size_t aCount)
{
#ifdef AT_RANGE_CHECK
    if (aCount>fnRows)
        throw EArrayAt("Array2d::shiftDown(): count exceeded ["
            + _i2a(aCount) + "/" + _i2a(fnRows) + "]");
#endif
    AT_CHECK_CANARY

    if (aRow+1>=aCount) return;
    std::copy_backward(rowIter(aRow), rowIter(aCount-1), rowIter(aCount));

    AT_CHECK_CANARY
}


template<class Elem>
void Array2d<Elem>::copyColumnFrom(
        size_t aCol,
        const Array2d<Elem>& aSource,
        size_t aSrcCol,
        ptrdiff_t aShift,
        Elem aFillValue)
{
#ifdef AT_RANGE_CHECK
    if (aCol>=nCols())
        throw EArrayAt("Array2d::copyColumn: aIndex range check ["
            + _i2a(aCol) + "/" + _i2a(nCols()) + "]");
    if (aSrcCol>=aSource.nCols())
        throw EArrayAt("Array2d::copyColumn(): aSrcIndex range check ["
            + _i2a(aSrcCol) + "/" + _i2a(aSource.nCols()) + "]");
#endif
    AT_CHECK_CANARY

    //throw std::logic_error("Test: copyColumn entered!");

    // Determine number count to copy
    size_t iFirstIn, iFirstOut, iLastOut;

    // Moving within bounds
    if (aShift<0)
    {
        iFirstIn = -aShift;
        iFirstOut = 0;
        if (iFirstIn>aSource.nRows())
        {
            iLastOut = aSource.nRows() + aShift;
        } else
        {
            iLastOut = 0;
        }
    } else
    {
        iFirstIn = 0;
        iFirstOut = std::min<size_t>(aShift, nRows());
        iLastOut = iFirstOut + aSource.nRows();
    }
    if (iLastOut>nRows())
        iLastOut = nRows();

    Elem* pOut = &at(0, aCol);

    // Fill before
    for (size_t i=0; i<iFirstOut; ++i)
    {
        *pOut = aFillValue;
        pOut += nCols();
    }

    // Fill within
    const Elem* pIn = aSource.fastRow(iFirstIn) + aSrcCol;
    for (size_t i=iFirstOut; i<iLastOut; ++i)
    {
        *pOut = *pIn;
        pIn += aSource.nCols();
        pOut += nCols();
    }

    // Fill after
    for (size_t i=iLastOut; i<nRows(); ++i)
    {
        *pOut = aFillValue;
        pOut += nCols();
    }

    AT_CHECK_CANARY
}   // Array2d::CopyColumn


template <class Elem>
void Array2d<Elem>::capture(Array2d<Elem>& aOther)
{
    AT_CHECK_CANARY
    if (aOther.fAutoDestroy)
    {
        borrow(aOther);
        fAutoDestroy = true;
        aOther.fAutoDestroy = false;
    } else allocCopy(aOther);
}


template<class Elem>
void Array2d<Elem>::copyRowFrom(
        size_t aRow,
        const Array2d<Elem>& aSrc,
        size_t aSrcRow)
{
#ifdef AT_RANGE_CHECK
    if (aRow>=fnRows)
    {
        throw EArrayAt("Array2d::copyRowFrom(s,A,s): Range 1 check error ["
            + _i2a(aRow) + "/" + _i2a(fnRows) + "]");
    }
    if (aSrcRow>=aSrc.nRows())
    {
        throw EArrayAt("Array2d::copyRowFrom(s,A,s): Range 2 check error ["
            + _i2a(aSrcRow) + "/" + _i2a(aSrc.nRows()) + "]");
    }
#endif
    const Elem* pStart = aSrc.fastRow(aSrcRow);
    std::copy(pStart, pStart+std::min(nCols(), aSrc.nCols()), fastRow(aRow));
}


template<class Elem>
void Array2d<Elem>::resize(size_t aNewRows, size_t aNewCols)
{
    AT_CHECK_CANARY
    // Former or latter size is 0
    if (aNewRows==0 || aNewCols==0 || size()==0)
    {
        alloc(aNewRows, aNewCols);
        return;
    }

    Array2d<Elem> a2(aNewRows, aNewCols);

    size_t minRows = std::min(nRows(), aNewRows);
    size_t minCols = std::min(nCols(), aNewCols);

    for (size_t iRow = 0; iRow < minRows; ++iRow)
    {
        // Slow
        //a2.copyRowFrom(iRow, *this, iRow);
        // Fast
        const Elem* pStart = fastRow(iRow);
        std::copy(pStart, pStart+minCols, a2.fastRow(iRow));
    }
    swap(a2);
    AT_CHECK_CANARY
}


template<class Elem>
bool Array2d<Elem>::extend(size_t aNewRows, size_t aNewCols)
{
    if (aNewRows<=nRows() && aNewCols<=nCols()) return false;

    resize(std::max(aNewRows, nRows()), std::max(aNewCols, nCols()));
    return true;
}


template<class Elem>
bool Array2d<Elem>::fitAlloc(size_t aNewRows, size_t aNewCols)
{
    AT_CHECK_CANARY
    size_t newSize = aNewRows * aNewCols;
    if (newSize==0 || (fAutoDestroy && size()>=newSize))
    {
        fnRows = aNewRows;
        fnCols = aNewCols;
        AT_CHECK_CANARY
        return false;
    } else
    {
        alloc(aNewRows, aNewCols);
        AT_CHECK_CANARY
        return true;
    }
}


template<class Elem>
void Array2d<Elem>::copyRowTo(
        const size_t aRow,
        Array1d<Elem>& rOut) const
{
#ifdef AT_RANGE_CHECK
    if (aRow>=fnRows)
    {
        throw EArrayAt("Array2d::copyRowTo(size_t, A1d): Range check error ["
            + _i2a(aRow) + "/" + _i2a(fnRows) + "]");
    }
#endif
    AT_CHECK_CANARY

    const Elem* _row = row(aRow);
    size_t sz = std::min(fnCols, rOut.size());
    std::copy(_row, _row+sz, rOut.buffer());

    AT_CHECK_CANARY
}


template<class Elem>
void Array2d<Elem>::copyRowTo(
        const size_t aRow,
        Buf1d<Elem>& rOut) const
{
#ifdef AT_RANGE_CHECK
    if (aRow>=fnRows)
    {
        throw EArrayAt("Array2d::copyRowTo(size_t, A1d): Range check error ["
            + _i2a(aRow) + "/" + _i2a(fnRows) + "]");
    }
#endif
    AT_CHECK_CANARY

    const Elem* _row = row(aRow);
    size_t sz = std::min(fnCols, rOut.size());
    std::copy(_row, _row+sz, rOut.buffer());

    AT_CHECK_CANARY
}


template<class Elem>
void Array2d<Elem>::transpose(const Array2d<Elem> &x)
{
    AT_CHECK_CANARY

    alloc(x.nCols(), x.nRows());
    Elem* p = buffer();
    for (size_t iRow = 0; iRow < fnRows; ++iRow)
        for (size_t iCol = 0; iCol < fnCols; ++iCol)
            *(p++) = x(iCol, iRow);

    AT_CHECK_CANARY
}



////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  ARRAY1DP                                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

template <class Elem>
struct ptr_iterator
{
    Elem* const* ptr;

    ptr_iterator() : ptr(NULL) {}
    explicit ptr_iterator(Elem* const* const aData) : ptr(aData) {}
    ptr_iterator(const ptr_iterator& aOther) : ptr(aOther.ptr) {}

    bool operator== (const ptr_iterator& aOther) const
    {  return (ptr==aOther.ptr);  }

    bool operator!= (const ptr_iterator& aOther) const
    {  return (ptr!=aOther.ptr);  }

    ptr_iterator& operator++()
    { ++ptr;  return *this; }

    ptr_iterator& operator--()
    { --ptr;  return *this; }

    ptr_iterator operator++(int)    // post-increment
    { ptr_iterator p = this; ++ptr;  return p; }

    ptr_iterator operator--(int)    // post-increment
    { ptr_iterator p = this; --ptr;  return p; }

    ptr_iterator& operator+=(int x)
    { ptr+=x;  return *this; }

    ptr_iterator& operator-=(int x)
    { ptr-=x;  return *this; }

    Elem& operator* () { return **ptr; }
    Elem* operator-> () { return *ptr; }
};

template <class Elem>
inline ptr_iterator<Elem> pbegin(const Array1d<Elem*>& aAr)
{  return ptr_iterator<Elem>(aAr.begin());  }

template <class Elem>
inline ptr_iterator<Elem> pend(const Array1d<Elem*>& aAr)
{  return ptr_iterator<Elem>(aAr.end());  }

//----------------------------------------------------------------------------//
///  Array1dP is a one-dimensional array of pointers that owns and destroys
///  everything that's assigned to it.
///
///  The rule of thumb is...
///  if it owns the memory, it also owns the objects.
///  If you don't want to own, use Array1d<Elem*> instead.
///
template <class Elem>
class Array1dP : public Array1dV<Elem*>
{
protected:

    using Array1dV<Elem*>::buf;
    using Array1dV<Elem*>::dealloc;

public:
    typedef Elem* PElem;

    //------------------------------------------------------------------------//
    ///  Construct fills the entire buffer with NULLs.
    ///  Destruct will delete everything not-NULL.
    ///
    virtual void construct(size_t aMin, size_t aMaxPlus);
    virtual void destruct(size_t aMin, size_t aMaxPlus);

    //------------------------------------------------------------------------//
    ///  Default constructor
    ///
    Array1dP() : Array1dV<PElem>() {}

    //------------------------------------------------------------------------//
    ///  Leaves a preallocated array
    ///
    explicit Array1dP(const size_t n) : Array1dV<PElem> ()
        { alloc(n); }

    //------------------------------------------------------------------------//
    ///  Copy constructor.
    ///
    ///  Deallocating a pointer twice is a common mistake.
    ///  Thus this version won't copy, it will parasite on another
    ///  array instead...
    Array1dP(Array1dP<Elem>& aSrc) : Array1dV<PElem> ()
    {
        this->borrow(aSrc);
    }

    //------------------------------------------------------------------------//
    ///  Destructor
    ///
    ~Array1dP()
    {
        dealloc();
    }

    Array1dP<Elem>& operator= (Array1dP<Elem>& aSrc)
    {
        this->borrow(aSrc);  return *this;
    }

    //------------------------------------------------------------------------//
    ///  Creates a blank object for every NULL entry
    ///
    void createObjects();

    //------------------------------------------------------------------------//
    ///  Alloc's an array, then creates a blank object for each entry
    ///
    void allocObjects(const size_t n);

    //------------------------------------------------------------------------//
    ///  These functions are required by G++
    ///
    using Array1dV<PElem>::at;

    const PElem& operator[](const size_t i) const
    {   return Array1dV<PElem>::operator[](i);   }

    PElem& operator[](const size_t i)
    {   return Array1dV<PElem>::operator[](i);   }

    using Array1dV<PElem>::size;
    using Array1dV<PElem>::alloc;

    //------------------------------------------------------------------------//
    ///  Array1dP uses special kind of iterator!
    ///
    const ptr_iterator<Elem> begin() const
    { return ptr_iterator<Elem>(Array1dV<Elem*>::begin()); }

    const ptr_iterator<Elem> end() const
    { return ptr_iterator<Elem>(Array1dV<Elem*>::end()); }
};

template <class Elem>
void Array1dP<Elem>::construct(
        size_t aMin, size_t aMaxPlus)
{
    std::fill(buf._ptr + aMin, buf._ptr + aMaxPlus,
            static_cast<Elem*>(NULL));
}

template <class Elem>
void Array1dP<Elem>::destruct(
        size_t aMin, size_t aMaxPlus)
{
    for (ptrdiff_t i=aMaxPlus-1; i>=static_cast<ptrdiff_t>(aMin); --i)
        delete buf[i];
}

template <class Elem>
void Array1dP<Elem>::createObjects()
{
    for (size_t i=0; i<size(); ++i)
        if (!at(i))
            at(i) = new Elem();
}

template <class Elem>
void Array1dP<Elem>::allocObjects(const size_t n)
{
    alloc(n);
    createObjects();
}


//#ifndef __BORLANDC__
//  It seems to me, Borland works now!
//  Then - commenting out and going on!

namespace std
{

template <class Elem>
inline void swap(Array1d<Elem>& a1, Array1d<Elem>& a2) noexcept
    { a1.swap(a2); }

template <class Elem>
inline void swap(Array2d<Elem>& a1, Array2d<Elem>& a2) noexcept
    { a1.swap(a2); }

}   // namespace std

//#endif


// Some concepts that do not fit into Array1d
namespace arr {

    //------------------------------------------------------------------------//
    /// Finds a value in the array (searching forward)
    /// @return  index or NOT_FOUND
    ///
    template <class Elem>
    inline size_t find(
            const Array1d<Elem>& a,
            const Elem& aValue,
            size_t aStart = 0)
        {  return arrutil::find1(a.buffer(), a.size(), aValue, aStart);  }

    template <class Elem>
    inline size_t find(
            const Buf1d<Elem>& a,
            const Elem& aValue,
            size_t aStart = 0)
        {  return arrutil::find1(a.buffer(), a.size(), aValue, aStart);  }

#ifndef AT_STL_UNUSED
    //------------------------------------------------------------------------//
    ///  Dumps horizontally
    ///
    template <class Elem>
    inline void dumpH(std::ostream& aOs, const Array1d<Elem>& a, const size_t aCount)
        { arrutil::dumpH(aOs, a.buffer(), aCount); }

    template <class Elem>
    inline void dumpH(std::ostream& aOs, const Array1d<Elem>& a)
        { arrutil::dumpH(aOs, a.buffer(), a.size()); }

    template <class Elem>
    inline void dumpH(const Array1d<Elem>& a)
        { arrutil::dumpH(std::cout, a.buffer(), a.size()); }

    template <class Elem>
    void dumpH(const Array1d<Elem>& a, const char* aFname)
    {
        std::ofstream os(aFname);
        dumpH(os, a);
    }

    //------------------------------------------------------------------------//
    ///  Dumps vertically
    ///
    template <class Elem>
    inline void dumpV(std::ostream& aOs, const Array1d<Elem>& a, const size_t aCount)
        { arrutil::dumpV(aOs, a.buffer(), aCount); }

    template <class Elem>
    inline void dumpV(std::ostream& aOs, const Array1d<Elem>& a)
        { arrutil::dumpV(aOs, a.buffer(), a.size()); }

    template <class Elem>
    inline void dumpV(const Array1d<Elem>& a)
        { arrutil::dumpV(std::cout, a.buffer(), a.size()); }

    template <class Elem>
    void dumpV(const Array1d<Elem>& a, const char* aFname)
    {
        std::ofstream os(aFname);
        dumpV(os, a);
    }
#endif

}   // namespace arr


template<class T, size_t N>
inline bool operator == (const Fix1d<T, N>& a, const Fix1d<T, N>& b)
{
    return std::equal(a.begin(), a.end(), b.begin());
}

template<class T, size_t N>
inline bool operator != (const Fix1d<T, N>& a, const Fix1d<T, N>& b)
{
    return !operator == (a, b);
}

// warn: to suppress stupid Builder's warning about incomplete header
inline void __e_ArrayTemplate_Dummy() {}

#endif
