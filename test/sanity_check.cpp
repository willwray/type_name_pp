#include "type_name_pp.hpp"

#include <cassert>
#include <type_traits>

using namespace ltl;

static_assert( type_name_pp<int> == "int" );
static_assert( type_name_pp<signed int> == "int" );
static_assert( type_name_pp<unsigned int> == "unsigned int" );

static_assert( type_name_pp<char> == "char" );
static_assert( type_name_pp<signed char> == "signed char" );
static_assert( type_name_pp<unsigned char> == "unsigned char" );

constexpr auto a0 = auto_name_pp<0>;
// Test first and last char == '0' because MSVC outputs "0x0"
static_assert( a0[0] == '0' && a0[size(a0)-2] == '0');

// Simple enum printing
// fairly consistent since GCC 9 - disable for earlier versions

#if not defined(__clang__) and defined(__GNUG__)
#if __GNUC__ > 8

enum e { a, b };
static_assert( auto_name_pp<a> == "a" );
static_assert( auto_name_pp<e(1)> == "b" );

enum class E { m, n };
static_assert( auto_name_pp<E::m> == "E::m" );
static_assert( auto_name_pp<E(1)> == "E::n" );

enum C : char { y, z };
static_assert( auto_name_pp<y> == "y" );
static_assert( auto_name_pp<C{1}> == "z" );

#endif
#endif

int main() {}
