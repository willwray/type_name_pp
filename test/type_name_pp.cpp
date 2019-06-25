#include "type_name_pp.hpp"

#include <cassert>
#include <type_traits>

using namespace ltl;

/* 
   More a catalogue of differences than test cases;
   they are unlikely to all pass on different compiler releases.
   Output is highly dependent on:

   * compiler,
   * compiler version, 
   * std library version (for std types).

   Let these serve as a warning.
*/

/* Tested on these compiler versions:

     * GCC 9
     * Clang 8 -stdlib=libc++
     * MSVC 19.22.27724 for x86
*/

// Some tests known to fail on GCC<9 (enum and char)
// MSVC has also changed output significantly

// GCM(g,c,m) macro; 3-way switch based on compiler
//
#if   defined(__clang__)
#     define             GCM(gnu,clang,msvc) clang
#elif defined(__GNUG__)
#     define             GCM(gnu,clang,msvc) gnu
#elif defined(_MSC_VER)
#     define             GCM(gnu,clang,msvc) msvc
#endif

static_assert(std::is_same_v<decltype(type_name_pp<char>), const ntbs::array<5>>);
static_assert( type_name_pp<char> == ntbs::array{"char"} );

#define PP_SAME_T(type) \
static_assert(type_name_pp<type> == #type)

#define PP_GCM_T(type,G,C,M) \
static_assert(type_name_pp<type> == GCM(#G,#C,#M))

// Simple type template args
PP_SAME_T(char);
PP_SAME_T(signed char);
PP_SAME_T(unsigned char);

PP_SAME_T(int);
PP_SAME_T(unsigned int);
static_assert(type_name_pp<signed int> == ntbs::array{"int"});

PP_SAME_T(const int);
PP_SAME_T(volatile int);

PP_GCM_T(long
        ,long int  // GCC
        ,long      // Clang
        ,long);    // MSVC

PP_GCM_T(const volatile int
        ,const volatile int      // GCC
        ,const volatile int      // Clang
        ,volatile const int);    // MSVC

PP_GCM_T(int&
        ,int&    // GCC
        ,int &   // Clang
        ,int &); // MSVC

PP_GCM_T(int*
        ,int*    // GCC
        ,int *   // Clang
        ,int *); // MSVC

PP_GCM_T(const int&
        ,const int&    // GCC
        ,const int &   // Clang
        ,const int &); // MSVC

PP_GCM_T(const int*
        ,const int*    // GCC
        ,const int *   // Clang
        ,const int *); // MSVC

PP_GCM_T(int* const
        ,int* const    // GCC
        ,int *const    // Clang
        ,int * const); // MSVC

struct ch { char c; };
PP_SAME_T(char ch::*);
static_assert(type_name_pp<decltype(&ch::c)> == "char ch::*");

PP_GCM_T(std::nullptr_t
        ,std::nullptr_t    // GCC
        ,     nullptr_t    // Clang
        ,     nullptr);    // MSVC ??

const volatile char abc[1][2][3]{};
PP_GCM_T(decltype(abc)
        ,const volatile char [1][2][3]   // GCC
        ,char const volatile[1][2][3]    // Clang
        ,volatile const char[1][2][3]);  // MSVC

// Hello world

namespace Hello { struct World; }

PP_GCM_T(Hello::World
        ,Hello::World           // GCC
        ,Hello::World           // Clang
        ,struct Hello::World);  // MSVC

// Non-type template arg tests
// Non-types with platform-dependent output


constexpr auto zero_pp = ntbs::array{ GCM("0"    // GCC
                                        , "0"    // Clang
                                        , "0x0"  // MSVC
) };

// Different Integral types give the same output (not good)
static_assert( auto_name_pp<0>       == zero_pp );
static_assert( auto_name_pp<0U>      == zero_pp );
static_assert( auto_name_pp<short{}> == zero_pp );
static_assert( auto_name_pp<long{}>  == zero_pp );

constexpr auto one_pp = ntbs::array{ GCM("1"    // GCC
                                       , "1"    // Clang
                                       , "0x1"  // MSVC
) };
static_assert( auto_name_pp<1>        == one_pp );
static_assert( auto_name_pp<1U>       == one_pp );
static_assert( auto_name_pp<short{1}> == one_pp );
static_assert( auto_name_pp<long{1}>  == one_pp );


// Printable char, e.g. '0'
static_assert( auto_name_pp<'0'> == GCM("'0'"     // GCC
                                       ,"'0'"     // Clang
                                       ,"0x30")); // MSVC
// Non-printable char, e.g. 0
static_assert( auto_name_pp<char{}> ==
     GCM (""           // GCC>=9, "'\000'" GCC<9
      , R"('\x00')"    // Clang
         , "0x0"));   // MSVC

constexpr char c{};
static_assert( auto_name_pp<&c> == GCM("(& c)"
                                      , "&c"
                                      , "& c"));


static_assert(auto_name_pp<&ch::c> == GCM("&ch::c"
                                        , "&ch::c"
                                        , "pointer-to-member(0x0)"));


// Enums
// Simple cases are fairly consistent (since GCC9).

enum e { a, b };
static_assert( auto_name_pp<a> == ntbs::array{"a"} );
static_assert( auto_name_pp<b> == ntbs::array{"b"} );
enum class E { m, n };
static_assert( auto_name_pp<E::m> == ntbs::array{"E::m"} );
static_assert( auto_name_pp<E::n> == ntbs::array{"E::n"} );

enum C : char { y, z };
static_assert( auto_name_pp<y> == ntbs::array{"y"} );
static_assert( auto_name_pp<z> == ntbs::array{"z"} );

#include <string>
static_assert( type_name_pp<std::string> ==
  GCM("std::__cxx11::basic_string<char>"
    , "std::__1::basic_string<char>"
    , "class std::basic_string<char, struct std::char_traits<char>,"
                                     "class std::allocator<char> >"));

int main()
{
}
