#include "type_name_pp.hpp"

#include <cassert>
#include <type_traits>

using namespace ltl;

/* 
   Not really tests.
   More a catalogue of differences.
   Let these serve as a warning.

   Output is highly dependent on:

   * compiler,
   * compiler version, 
   * std library version (for std types).
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

// constexpr char array vs charz comparison *** SKIPS SPACES ***
// 
template <int32_t A, int32_t B>
constexpr bool operator==(char const (&a)[A], charz<B> const& cb)
{
    const auto& b = cb.data;
    for (int32_t ai = 0, bi = 0; ai != A && bi != B; ++ai, ++bi)
    {
        while (a[ai] == ' ') ++ai;
        while (b[bi] == ' ') ++bi;
        if (a[ai] != b[bi])
            return false;
    }
    return true;
}

static_assert( "int" == charz{"int"} );
static_assert( "in t" == charz{"i n t"} ); // Spaces ignored

static_assert(std::is_same_v<decltype(type_name_pp<char>), char const(&)[5]>);
static_assert( type_name_pp<char> == charz{"char"} );

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
//PP_SAME_T(const volatile int);

PP_SAME_T(const int&);
PP_SAME_T(const int*);
PP_SAME_T(int *const);

struct ch { char c; };
PP_SAME_T(char ch::*);
static_assert(type_name_pp<decltype(&ch::c)> == charz{ "char ch::*" });

// Simple types with platform-dependent output

static_assert(type_name_pp<const volatile int> ==
	charz{ GCM("const volatile int"      // GCC
			  ,"const volatile int"      // Clang
			  ,"volatile const int") }); // MSVC

static_assert(type_name_pp<long> ==
              charz{GCM(  "long int"  // GCC
                         ,"long"      // Clang
                         ,"long")});  // MSVC

static_assert(type_name_pp<std::nullptr_t> ==
              charz{GCM(  "std::nullptr_t"     // GCC
                              ,"nullptr_t"     // Clang
                              ,"nullptr")});   // MSVC ??

const volatile char abc[1][2][3]{};
static_assert( type_name_pp<decltype(abc)> ==
               charz{GCM("const volatile char[1][2][3]"     // GCC
                        ,"char const volatile[1][2][3]"     // Clang
                        ,"volatile const char[1][2][3]")}); // MSVC

// Hello world

namespace Hello { struct World; }

static_assert( ltl::type_name_pp<Hello::World> ==
	charz{ GCM("Hello::World"             // GCC
			  ,"Hello::World"             // Clang
			  ,"struct Hello::World") }); // MSVC

// Non-type template arg tests
// Non-types with platform-dependent output


constexpr auto zero_pp = charz{ GCM("0"    // GCC
								  , "0"    // Clang
								  , "0x0"  // MSVC
) };
// Different Integral types give the same output (not good)
static_assert( auto_name_pp<0>       == zero_pp );
static_assert( auto_name_pp<0U>      == zero_pp );
static_assert( auto_name_pp<short{}> == zero_pp );
static_assert( auto_name_pp<long{}>  == zero_pp );

constexpr auto one_pp = charz{ GCM("1"    // GCC
								 , "1"    // Clang
								 , "0x1"  // MSVC
) };
static_assert( auto_name_pp<1>        == one_pp );
static_assert( auto_name_pp<1U>       == one_pp );
static_assert( auto_name_pp<short{1}> == one_pp );
static_assert( auto_name_pp<long{1}>  == one_pp );


// Printable char, e.g. '0'
static_assert( auto_name_pp<'0'> == charz{ GCM("'0'"    // GCC
	                                          ,"'0'"    // Clang
	                                          ,"0x30")}); // MSVC
// Non-printable char, e.g. 0
static_assert( auto_name_pp<char{}> ==
charz{GCM (""          // GCC>=9, "'\000'" GCC<9
      , R"('\x00')"    // Clang
         , "0x0")});   // MSVC

constexpr char c{};
static_assert( auto_name_pp<&c> == charz{GCM("(& c)"
	                                       , "&c"
	                                       , "& c")});


static_assert(auto_name_pp<&ch::c> == charz{ GCM("&ch::c"
										   , "&ch::c"
										   , "pointer-to-member(0x0)") });


// Enums
// Simple cases are fairly consistent (since GCC9).

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
        , "class std::basic_string<char, struct std::char_traits<char>, class std::allocator<char> >")});

int main()
{
}
