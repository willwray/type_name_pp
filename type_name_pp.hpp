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

   Targets GCC & Clang  with -std=c++17, MSVC with /std:c++latest.

   Depends on the ltl "ntbs.hpp" 'null-terminated byte strings' lib
   for constexpr C-string-slicing ntbs::cut function.
 
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
   qualifiers leaving the string that follows any final "::" separator,
   using a simple search back for "::" (see below for a failure case).

     ltl::type_name_pu<T>; type name with leading qualifiers stripped.
     ltl::auto_name_pu<v>; auto name with leading qualifiers stripped
                           (useful e.g. for enumerator id output).

 Example: std::string
 =======
       ltl::type_name_pp<std::string>   '_pp': full pretty print
       ------------------------------
GCC9   "std::__cxx11::basic_string<char>"
Clang8 "std::__1::basic_string<char>"
MSVC(v "class std::basic_string<char, struct std::char_traits<char>, ...
19.22.)                           ... class std::allocator<char> >"

       ltl::type_name_pu<std::string>   '_pu': remove qualifiers
       ------------------------------
GCC9   "basic_string<char>"
Clang8 "basic_string<char>"
MSVC(v "allocator<char> >"     (oops: final "::" in template args)
19.22.)

  Note that the '_pu' fail with MSVC is due to the over-simple search.
  For hints on crafting custom slicer see the '_pu' implementations or,
  for more complex slicing or composing use e.g. string_view or string.

  The included ntbs::cut function can slice from the full pp output by
  providing one or two extra integer template arguments for range [B,E):

   The [B,E) range indices are signed integers:
     * Negative values index backward from the end of the char array.
     * Positive values index forward from begin index 0 as usual.
   (-1 serves as end index here as all arrays are zero-terminated).

*/

namespace ltl {

namespace impl {

template <typename T>
constexpr
auto
PTTS()
{
# if defined(__FUNCSIG__)
    return sizeof __FUNCSIG__;
# else
    return sizeof __PRETTY_FUNCTION__;
# endif
}

template <auto v>
constexpr
auto
PTvS()
{
# if defined(__FUNCSIG__)
    return sizeof __FUNCSIG__;
# else
    return sizeof __PRETTY_FUNCTION__;
# endif
}

constexpr int32_t PFT_suffix = []{
# if defined(__FUNCSIG__)
    return sizeof ">(void)";
# else
    return sizeof "]";
# endif
}();

constexpr int32_t PFv_suffix =
                  PFT_suffix;

constexpr int32_t PFT_prefix = []{
  return PTTS<int>()
# if defined(__FUNCSIG__)
    - sizeof "int>(void)";
# else
    - sizeof "int]";
# endif
}();

constexpr int32_t PFv_prefix = []{
  return PTvS<0>()
# if defined(__FUNCSIG__)
    - sizeof "0x0>(void)";
# else
    - sizeof "0]";
# endif
}();

template <typename T>
constexpr
auto
PTTI()
{
  return ntbs::cut<PFT_prefix,-PFT_suffix>
# if defined(__FUNCSIG__)
            (__FUNCSIG__);
# else
         (__PRETTY_FUNCTION__);
# endif
}

template <auto e>
constexpr
auto
PTvI()
{
  return ntbs::cut<PFv_prefix,-PFv_suffix>
# if defined(__FUNCSIG__)
            (__FUNCSIG__);
# else
         (__PRETTY_FUNCTION__);
# endif
}

template <typename A>
constexpr
int32_t
last_qualifier_pos(A const& a)
{
    int32_t s{ntbs::extent_v<A>};
    while (s != 0 && (a[s-1] != ':' ||
         (s-1 != 0 && a[s-2] != ':')))
      --s;
    return s;
}

} // namespace impl

// ltl::type_name_pp<T>; full 'pretty print' type name of T
// extracted from preprocessor 'pretty function' output.
// An NTBS constant.
//
template <typename T>
inline constexpr auto const& type_name_pp = impl::PTTI<T>();

// ltl::auto_name_pp<v>; 'pretty print' output for NTTP value v
// extracted from preprocessor 'pretty function' output.
// An NTBS constant.
//
template <auto v>
inline constexpr auto const& auto_name_pp = impl::PTvI<v>();

// ltl::type_name_pu<T>; suffix from final "::" in type_name_pp<T>
//
template <typename T>
inline constexpr auto const& type_name_pu = ntbs::cut<
                   impl::last_qualifier_pos(type_name_pp<T>)>
                                           (type_name_pp<T>);

// ltl::auto_name_pu<v>; suffix from final "::" in auto_name_pp<T> 
//
template <auto v>
inline constexpr auto const& auto_name_pu = ntbs::cut<
                   impl::last_qualifier_pos(auto_name_pp<v>)>
                                           (auto_name_pp<v>);

} // namespace ltl

#endif