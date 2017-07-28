/**
 * snaske@77k.eu
 */
//just a few naive sketches 
#ifndef MATH_HELPERS_H
#define MATH_HELPERS_H
#include <limits>
namespace std {
	template<> class numeric_limits< unsigned __int128 >
	{
		public:
			static constexpr int digits = 128;
			static constexpr bool is_specialized = true;
			static constexpr unsigned __int128 max(){ return  -1;};
	};
}
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <bitset>
#include <array>
#include <cmath>
#include <memory>

#ifdef EMSCRIPTEN
#include <boost/multiprecision/integer.hpp>
typedef boost::multiprecision::uint128_t uint128_t;
typedef boost::multiprecision::uint256_t uint256_t;
#else
#include <boost/multiprecision/integer.hpp>
typedef __uint128_t uint128_t;
//typedef __m256i uint256_t;
//typedef boost::multiprecision::uint128_t uint128_t;
typedef boost::multiprecision::number<boost::multiprecision::cpp_int_backend<256, 256, boost::multiprecision::signed_magnitude, boost::multiprecision::unchecked, void> >   uint256_t;

//typedef boost::multiprecision::uint256_t uint256_t;
#endif
enum class ArchType: short
{
    GENERIC,
    AMD64,
    AARCH64,
};

enum class LinkType: bool
{
    Single,
    Double,
};

enum class AccessScheme: bool
{
    Pointer,
    Index,
};

template< bool _cond, class _then, class _else >
struct IF
{
        typedef _then RET;
};

template< class _then, class _else >
struct IF< false, _then, _else >
{
        typedef _else RET;
};

template< int N > struct ifact{ enum { eval = N * ifact< N - 1>::eval }; };
template <> struct ifact< 0 > { enum { eval = 1 }; };
template< int N, int K >
struct nchoosek{ 
    enum { 
        eval = ifact< N >::eval / ( ifact< K >::eval *  ifact< N - K >::eval)
    };
};

//todo generalize from uint64_t
template< uint64_t N, uint64_t M >
struct ipow
{
        enum : uint64_t { eval = N * ipow< N , M - 1 >::eval};
};

template < uint64_t N >
struct ipow< N, 0 >
{
        enum : uint64_t { eval = 1 };
};

template< uint64_t N >
struct ild
{
enum : uint64_t { eval = 1 + ild< N/2 >::eval};

};
template < >
struct ild< 2 >{ enum : uint64_t { eval = 1 }; };

template< int64_t _X, int64_t _N >
struct inroot
{
    template< bool _c, int64_t _d, int64_t _x, int64_t _e >
        struct _do { enum : int64_t {  N = _N, X = _X,
                                       d = (N - 1) * _x + ((X / ( ipow< _x, N - 1>::eval))) / N, x = _x + d,
                                       eval = _do< (x > _x), d, x, _e >::eval};};
    template< int64_t _d, int64_t _x, int64_t _e >
        struct _do< false, _d, _x, _e >
        {
                enum : int64_t { d = _d, x = _x, eval = _x };
        };
enum : uint64_t { eval = (_do< true, 0, 1, 1>::x)};
};

template <int64_t N > struct wilsons_primality_test 
{
enum : bool { eval = (ifact< N - 1>::eval % N - N == -1) };
};

template <int64_t N > struct fermats_little_primality_test 
{
enum : bool { eval = (ipow< 2, N - 1 >::eval % N == 1) };
};


template< typename _T, int _I >
struct simd_sum
{
        static inline _T eval(_T &a)
        {
                return a[_I] + simd_sum< _T, _I - 1 >::eval(a);
        }
};

template< typename _T >
struct simd_sum< _T, 0 >
{
        static inline _T eval(_T &a)
        {
                return a[0];
        }
};
template< typename _T, int _I >
struct simd_cp
{
        static inline void eval(_T &d, _T &s)
        {
                d[_I] = s[_I]; simd_cp< _T, _I - 1 >::eval(d, s);
        }
};
template< typename _T >
struct simd_cp< _T, 0 >
{
        static inline void eval(_T &d, _T &s)
        {
                return d[0] = s[0];
        }
};

template< int S > struct SimpleLargerInteger{
        unsigned long long values[S/64];
};

template< int S > struct IntegerChooser
{
        typedef typename 
        IF< S <= sizeof(uint8_t), uint8_t,
                IF< S <= sizeof(uint16_t), uint16_t,
                IF< S <= sizeof(uint32_t), uint32_t,
                IF< S <= sizeof(uint64_t), uint64_t,
                IF< S <= sizeof(uint128_t), uint128_t,
                IF< S <= sizeof(uint256_t), uint256_t,
        SimpleLargerInteger< S > > > > > > >::RET IntegerType;
};

template< > struct IntegerChooser< 3 >
{
    enum { size = 3 };
    typedef uint32_t IntegerType;
};

template< > struct IntegerChooser< 4 >
{
        enum { size = 4};
        typedef uint32_t IntegerType;
};


template< > struct IntegerChooser< 6 >
{
        enum { size = 6};
    typedef __attribute__ ((aligned (8))) uint64_t IntegerType;
};

template< > struct IntegerChooser< 8 >
{
        enum { size = 8};
        typedef __attribute__ ((aligned (8))) uint64_t IntegerType;
};


template< > struct IntegerChooser< 16 >
{
        enum { size = 16};
        typedef __attribute__ ((aligned (16))) uint128_t IntegerType;
};


template< > struct IntegerChooser< 32 >
{
        enum { size = 32};
        typedef __attribute__ ((aligned (32))) uint256_t IntegerType;
};

#endif
