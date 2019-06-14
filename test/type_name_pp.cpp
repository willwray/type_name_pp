#include "type_name_pp.hpp"

#include <type_traits>

using namespace ltl;

/*
 *** PP output is highly dependent on compiler,
 *** compiler version, and, 
 *** for std types, the std library version.
*/

// Tested on GCC9, Clang8 -stdlib=libc++, MSVC 14.2
// Some tests fail on GCC<9 (enum and char)

// GCM(g,c,m) macro; 3-way switch based on compiler
//
#if   defined(__clang__)
#     define             GCM(gnu,clang,msvc) clang
#elif defined(__GNUG__)
#     define             GCM(gnu,clang,msvc) gnu
#elif defined(_MSC_VER)
#     define             GCM(gnu,clang,msvc) msvc
#endif

template <std::size_t N> struct charz { char data[N]; };
template <std::size_t N> charz(char const(&)[N]) -> charz<N>;

// constexpr char array vs charz comparison *** SKIPS SPACES ***
// 
template <size_t A, size_t B>
constexpr bool operator==(char const (&a)[A], charz<B> const& cb)
{
    const auto& b = cb.data;
    for (size_t ai = 0, bi = 0; ai != A && bi != B; ++ai, ++bi)
    {
        while (a[ai]==' ') ++ai;
        while (b[bi]==' ') ++bi;
        if (a[ai] != b[bi])
            return false;
    }
    return true;
}

static_assert( "int" == charz{"int"} );
static_assert( "in t" == charz{"i n t"} ); // Spaces ignored

#define PP_SAME_T(type) \
static_assert(type_name_pp<type> == charz{#type})

// Simple type template args

PP_SAME_T(char);
PP_SAME_T(signed char);
PP_SAME_T(unsigned char);

PP_SAME_T(int);
PP_SAME_T(unsigned int);
static_assert(type_name_pp<signed int> == charz{"int"});

PP_SAME_T(int&);
PP_SAME_T(int*);

PP_SAME_T(const int);
PP_SAME_T(volatile int);
PP_SAME_T(const volatile int);

PP_SAME_T(const int&);
PP_SAME_T(const int*);
PP_SAME_T(int *const);

// Simple types with platform-dependent output

static_assert(type_name_pp<long int> ==
              charz{GCM(  "long int"  // GCC
                         ,"long"      // Clang
                         ,"long")});  // MSVC

static_assert(type_name_pp<std::nullptr_t> ==
              charz{GCM(  "std::nullptr_t"     // GCC
                              ,"nullptr_t"     // Clang
                              ,"nullptr_t")}); // MSVC

const volatile char abc[1][2][3]{};
static_assert( type_name_pp<decltype(abc)> ==
               charz{GCM("const volatile char[1][2][3]"     // GCC
                        ,"char const volatile[1][2][3]"     // Clang
                        ,"const volatile char[1][2][3]")}); // MSVC

// Hello world

namespace Hello { struct World; }
static_assert( ltl::type_name_pp<Hello::World> == charz{"Hello::World"});

// Non-type template arg tests

// Different Integral types give same output (not good)
static_assert( auto_name_pp<0>       == charz{"0"});
static_assert( auto_name_pp<0U>      == charz{"0"});
static_assert( auto_name_pp<short{}> == charz{"0"});
static_assert( auto_name_pp<long{}>  == charz{"0"});

static_assert( auto_name_pp<1>        == charz{"1"});
static_assert( auto_name_pp<1U>       == charz{"1"});
static_assert( auto_name_pp<short{1}> == charz{"1"});
static_assert( auto_name_pp<long{1}>  == charz{"1"});

// char c, output as 'c' for printable c
static_assert( auto_name_pp<'0'> == charz{"'0'"});
// else escape sequence or value
static_assert( auto_name_pp<char{}> ==
charz{GCM (""             // GCC>=9, "'\000'" GCC<9
         , R"('\x00')"
         , R"('\x00')")});

constexpr char c{};
static_assert( auto_name_pp<&c> == charz{GCM("(& c)", "&c", "&c")});

struct ch { char c; };
static_assert( auto_name_pp<&ch::c> == charz{"&ch::c"});

static_assert( type_name_pp<decltype(&ch::c)> == charz{"char ch::*"});

// Enums

enum e { a, b };
static_assert( auto_name_pp<a> == charz{"a"} );
static_assert( auto_name_pp<b> == charz{"b"} );
enum class E { m, n };
static_assert( auto_name_pp<E::m> == charz{"E::m"} );
static_assert( auto_name_pp<E::n> == charz{"E::n"} );

enum C : char { y, z };
static_assert( auto_name_pp<y> == charz{"y"} );
static_assert( auto_name_pp<z> == charz{"z"} );

#include <string>
static_assert( type_name_pp<std::string> ==
charz{GCM("std::__cxx11::basic_string<char>"
        , "std::__1::basic_string<char>"
        , "std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >")});

int main()
{
}
