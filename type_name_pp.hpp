//  Copyright (c) 2019 Will Wray https://keybase.io/willwray
//
//  Distributed under the Boost Software License, Version 1.0.
//        http://www.boost.org/LICENSE_1_0.txt
//
//  Repo: https://github.com/willwray/type_name_pp

#ifndef LTL_TYPE_NAME_HPP
#define LTL_TYPE_NAME_HPP

#include "ntbs.hpp"

/*
  "type_name_pp.hpp": Pretty print output for types and NTTP values.
   ^^^^^^^^^^^^^^^^^

   Targets GCC & Clang with -std=c++17, MSVC with /std:c++17.

   Depends on ltl lib "ntbs.hpp" 'null-terminated byte strings'
   for constexpr C-string-slicing; ntbs::cut function.
 
   This header declares variable templates for printable type names
   and also 'names' of non-type template arguments/('parameters'):

     ltl::type_name_pp<T>; 'Pretty print' name for type T.
     ltl::auto_name_pp<v>; 'Pretty print' output for auto/NTTP value v.

   The resulting null-terminated char arrays are usable as C-strings.

   The names are sliced from preprocessor 'pretty function' extensions:

           __FUNCSIG__         on MSVC
           __PRETTY_FUNCTION__ on GCC, Clang

   These are constexpr-usable strings of unspecified format that varies
   between compilers and compiler versions; this method is not backward
   or forward compatible and does not provide portable names.
   Test for your use-case and target platforms.

   The output may include nested name qualifiers for scoped types & ids
   and/or template arguments for templated types.

   For convenience, 'pu' versions are provided to strip any nested name
   qualifiers leaving the string that follows any final "::" separator
   (using a simple search back for "::" - see below for a failure case).

     ltl::type_name_pu<T>; type name with leading qualifiers stripped.
     ltl::auto_name_pu<v>; auto name with leading qualifiers stripped
                           (useful e.g. for enumerator id output).

 Example: std::string
 =======
       ltl::type_name_pp<std::string>   '_pp': full pretty print
       ------------------------------
GCC9   "std::__cxx11::basic_string<char>"
Clang8 "std::__1::basic_string<char>"
MSVC   "class std::basic_string<char, struct std::char_traits<char>, ...
 v19.22...                       ... class std::allocator<char> >"

       ltl::type_name_pu<std::string>   '_pu': remove qualifiers
       ------------------------------
GCC9   "basic_string<char>"
Clang8 "basic_string<char>"
MSVC   "allocator<char> >"     (oops: final "::" in template args)
 v19.22...

  Note that the '_pu' fail with MSVC is due to an over-simple search.

 Post-processing
 ---------------
  The included ntbs::cut function can slice a 'constexpr C-string' by
  providing one or two extra integer template arguments for range [B,E):

   The [B,E) range indices are signed integers:
     * Negative values index backward from the end of the char array.
     * Positive values index forward from begin index 0 as usual.
   (-1 serves as end index here as all arrays are zero-terminated).

  See the '_pu' implementations for hints on crafting custom slicers or
  for more complex searching and slicing use std algos & string_view.

*/

namespace ltl {

namespace impl {

constexpr
bool
isPF()
{
#if defined(__FUNCSIG__)
# define PP __FUNCSIG__
  return false;
#else
# define PP __PRETTY_FUNCTION__
  return true;
# endif
}

template <typename T>
constexpr
auto
PPTS()
{
  return sizeof PP;
}

template <auto v>
constexpr
auto
PPvS()
{
  return sizeof PP;
}

inline constexpr int32_t PPT_suffix = isPF() ? sizeof "]"
                                             : sizeof ">(void)";

inline constexpr int32_t PPv_suffix = PPT_suffix;

inline constexpr int32_t PPT_prefix =             PPTS<int>()
                                   - (isPF() ? sizeof "int]"
                                             : sizeof "int>(void)");

inline constexpr int32_t PPv_prefix =             PPvS<0>()
                                   - (isPF() ? sizeof "0]"
                                             : sizeof "0x0>(void)");

template <typename T>
constexpr
auto
PPTN()
{
  return ntbs::cut<PPT_prefix,-PPT_suffix>(PP);
}

template <auto v>
constexpr
auto
PPvN()
{
  return ntbs::cut<PPv_prefix,-PPv_suffix>(PP);
}

template <typename A>
constexpr
int32_t
last_qualifier_pos(A const& a)
{
    int32_t s{ntbs::extent_v<A>};
    while ( s != 0 && ( a[s-1] != ':' ||
          ( s-1 != 0 && a[s-2] != ':' ) ) )
      --s;
    return s;
}

} // namespace impl

// ltl::type_name_pp<T>; full 'pretty print' type name of T
// extracted from preprocessor 'pretty function' output.
// An NTBS constant.
//
template <typename T>
inline constexpr auto type_name_pp = impl::PPTN<T>();

// ltl::auto_name_pp<v>; 'pretty print' output for NTTP value v
// extracted from preprocessor 'pretty function' output.
// An NTBS constant.
//
template <auto v>
inline constexpr auto auto_name_pp = impl::PPvN<v>();

// ltl::type_name_pu<T>; suffix from final "::" in type_name_pp<T>
//
template <typename T>
inline constexpr auto type_name_pu = ntbs::cut<
                      impl::last_qualifier_pos(type_name_pp<T>)>
                                              (type_name_pp<T>);

// ltl::auto_name_pu<v>; suffix from final "::" in auto_name_pp<T> 
//
template <auto v>
inline constexpr auto auto_name_pu = ntbs::cut<
                      impl::last_qualifier_pos(auto_name_pp<v>)>
                                              (auto_name_pp<v>);

} // namespace ltl

#undef PP

#endif