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
static_assert( type_name_pp<char> == "char" );

#define PP_SAME_T(type) \
static_assert(type_name_pp<type> == #type)

#define PP_GCM_T(type,G,C,M) \
static_assert(type_name_pp<type> == GCM(G,C,M))

// Simple type template args
PP_SAME_T(char);
PP_SAME_T(signed char);
PP_SAME_T(unsigned char);

PP_SAME_T(int);
PP_SAME_T(unsigned int);

static_assert(type_name_pp<signed int> == "int");

PP_SAME_T(const int);
PP_SAME_T(volatile int);

PP_GCM_T(long
       ,"long int" // GCC
       ,"long"     // Clang
       ,"long");   // MSVC

PP_GCM_T(const volatile int
       ,"const volatile int"   // GCC
       ,"const volatile int"   // Clang
       ,"volatile const int"); // MSVC

PP_GCM_T(int&
      , "int&"   // GCC
      , "int &"  // Clang
      , "int&"); // MSVC

PP_GCM_T(int*
       ,"int*"   // GCC
       ,"int *"  // Clang
       ,"int*"); // MSVC

PP_GCM_T(const int&
       ,"const int&"   // GCC
       ,"const int &"  // Clang
       ,"const int&"); // MSVC

PP_GCM_T(const int*
       ,"const int*"   // GCC
       ,"const int *"  // Clang
       ,"const int*"); // MSVC

PP_GCM_T(int* const
       ,"int* const"   // GCC
       ,"int *const"   // Clang
       ,"int*const "); // MSVC (spacing error)

struct ch { char c; };
PP_GCM_T(char ch::*
       ,"char ch::*"   // GCC
       ,"char ch::*"   // Clang
       ,"charch::* ");  // MSVC (spacing error)

PP_GCM_T(std::nullptr_t
       ,"std::nullptr_t"  // GCC
       ,     "nullptr_t"  // Clang
       ,     "nullptr");  // MSVC (missing _t)

const volatile char abc[1][2][3]{};
PP_GCM_T(decltype(abc)
       ,"const volatile char [1][2][3]"   // GCC
       ,"char const volatile[1][2][3]"    // Clang
       ,"volatile const char[1][2][3]");  // MSVC

// Hello world

namespace Hello { struct World; }

PP_GCM_T(Hello::World
       ,"Hello::World"          // GCC
       ,"Hello::World"          // Clang
       ,"struct Hello::World"); // MSVC

// Non-type template arg tests
// Non-types with platform-dependent output


constexpr auto& zero_pp = GCM("0"      // GCC
                            , "0"      // Clang
                            , "0x0");  // MSVC


// Different Integral types give the same output (not good)
static_assert( auto_name_pp<0>       == zero_pp );
static_assert( auto_name_pp<0U>      == zero_pp );
static_assert( auto_name_pp<short{}> == zero_pp );
static_assert( auto_name_pp<long{}>  == zero_pp );

constexpr auto& one_pp = GCM("1"      // GCC
                           , "1"      // Clang
                           , "0x1");  // MSVC

static_assert( auto_name_pp<1>        == one_pp );
static_assert( auto_name_pp<1U>       == one_pp );
static_assert( auto_name_pp<short{1}> == one_pp );
static_assert( auto_name_pp<long{1}>  == one_pp );


// Printable char, e.g. '0'
static_assert( auto_name_pp<'0'> == GCM("'0'"       // GCC
                                       ,"'0'"       // Clang
                                       , "0x30") ); // MSVC
// Non-printable char, e.g. 0
static_assert( auto_name_pp<char{}> == GCM (""          // GCC
                                         , R"('\x00')"  // Clang
                                         , "0x0") );    // MSVC

constexpr char c{};
static_assert( auto_name_pp<&c> == GCM("(& c)"    // GCC
                                      , "&c"      // Clang
                                      , "& c") ); // MSVC


static_assert(auto_name_pp<&ch::c> == GCM("&ch::c"                     // GCC
                                        , "&ch::c"                     // Clang
                                        , "pointer-to-member(0x0)") ); // MSVC


// Enums
// Simple cases are fairly consistent (since GCC9).

enum e { a, b };
static_assert( auto_name_pp<a> == "a" );
static_assert( auto_name_pp<b> == "b" );
enum class E { m, n };
static_assert( auto_name_pp<E::m> == "E::m" );
static_assert( auto_name_pp<E::n> == "E::n" );

enum C : char { y, z };
static_assert( auto_name_pp<y> == "y" );
static_assert( auto_name_pp<z> == "z" );

#include <string>
static_assert( type_name_pp<std::string> ==
  GCM("std::__cxx11::basic_string<char>"   // GCC
    , "std::__1::basic_string<char>"          // Clang
    , "class std::basic_string<char,struct std::char_traits<char>,"   // MSVC
                                   "class std::allocator<char> >") ); // ...

#include <cstdio>
int main()
{
}
