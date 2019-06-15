//  Copyright (c) 2019 Will Wray https://keybase.io/willwray
//
//  Distributed under the Boost Software License, Version 1.0.
//        http://www.boost.org/LICENSE_1_0.txt
//
//  Repo: https://github.com/willwray/type_name_pp

#ifndef LTL_TYPE_NAME_HPP
#define LTL_TYPE_NAME_HPP

#include <cstddef>
#include <cstdint>

/*
  "type_name_pp.hpp": pretty print output for types and NTTP values.
   ^^^^^^^^^^^^^^^^^

   Targets GCC, Clang and MSVC compilers with c++17 std flag.
 
   This header declares variable templates that give printable names
   of types and also 'names' of non-type template arguments (NTTPs):

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

   The output may include nested name qualifiers and template arguments.
   Sub-arrays can be sliced from the full pp output by providing one or
   two extra integer template arguments B and E to specify range [B,E):

     ltl::type_name_pp<T,B,E>; Subarray [B,E) of type_name_pp<T>.
     ltl::type_name_pp<T,B>;   Subarray [B,end), removes a prefix.
     (same for auto_name_pp)

   The [B,E) range indices are signed integers:
     * Negative values index backward from the end of the char array.
     * Positive values index forward from begin index 0 as usual.
   (-1 serves as end index here as all arrays are zero-terminated).

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
GCC   "std::__cxx11::basic_string<char>"
Clang "std::__1::basic_string<char>"
MSVC  "class std::basic_string<char,std::char_traits<char>,std::allocator<char> >"

      ltl::type_name_pu<std::string>   '_pu': remove qualifiers
      ------------------------------
GCC   "basic_string<char>"
Clang "basic_string<char>"
MSVC  "allocator<char> >"     (oops: final "::" in template args)

  Note that the '_pu' fail with MSVC is due to the over-simple search.
  For hints on crafting custom slicer see the '_pu' implementations or,
  for more complex slicing or composing use e.g. string_view or string.
*/

namespace ltl {

namespace impl {

// charz<N>: wrapper for zero-terminated char array
//
template <int32_t N>
struct charz
{
    char data[N];
};

template <int32_t N>
charz(char const(&)[N]) -> charz<N>;

// subcharz<B,E>(char[N]) returns subarray [B,E) of char array arg.
// The result is returned as charz<M> type with zero terminator added.
// The [B,E) indices are signed integers (c.f. Python slice indexing).
// The input char array argument is assumed to be zero terminated.
//
template <int32_t B = 0, int32_t E = INT32_MAX, int32_t N>
constexpr
auto
subcharz(char const (&a)[N])
{
    constexpr auto ind = [](int32_t i) -> int32_t {
        return i == INT32_MAX ? N : i < 0 ? N+i : i;
    };
    constexpr int32_t b = ind(B), e = ind(E), M = e - b;
    static_assert( 0 <= b && b <= e && e <= N && N <= INT32_MAX,
                   "index out of bounds");
    charz<M == N ? N : M + 1> chars{};
    for (int i = 0; i < M; ++i)
        chars.data[i] = a[b + i];
    return chars;
}

template <int32_t B = 0, int32_t E = INT32_MAX, int32_t N>
constexpr
auto
subcharz(charz<N> const& a) { return subcharz<B,E,N>(a.data); }

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
  return subcharz<PFT_prefix,-PFT_suffix>
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
  return subcharz<PFv_prefix,-PFv_suffix>
# if defined(__FUNCSIG__)
            (__FUNCSIG__);
# else
         (__PRETTY_FUNCTION__);
# endif
}

template <int32_t N>
constexpr
int32_t
last_qualifier_pos(const char (&a)[N])
{
    int32_t s{N};
    while (s != 0 && (a[s-1] != ':' ||
         (s-1 != 0 && a[s-2] != ':')))
      --s;
    return s;
}

} // namespace impl

// Undocumented
template <typename T, int32_t B = 0, int32_t E = -1>
inline constexpr auto PTTI = impl::subcharz<B,E>(impl::PTTI<T>());
// Undocumented
template <auto v, int32_t B = 0, int32_t E = -1>
inline constexpr auto PTvI = impl::subcharz<B,E>(impl::PTvI<v>());

// ltl::type_name_pp<T>; full 'pretty print' type name of T
// extracted from preprocessor 'pretty function' output.
// An NTBS constant.
//
template <typename T, int32_t B = 0, int32_t E = -1>
inline constexpr auto const& type_name_pp = PTTI<T,B,E>.data;

// ltl::auto_name_pp<v>; 'pretty print' output for NTTP value v
// extracted from preprocessor 'pretty function' output.
// An NTBS constant.
//
template <auto v, int32_t B = 0, int32_t E = -1>
inline constexpr auto const& auto_name_pp = PTvI<v,B,E>.data;

// ltl::type_name_pu<T>; suffix from final "::" in type_name_pp<T>
//
template <typename T>
inline constexpr auto const& type_name_pu = type_name_pp<T,
                   impl::last_qualifier_pos(type_name_pp<T>),-1>;

// ltl::auto_name_pu<v>; suffix from final "::" in auto_name_pp<T> 
//
template <auto v>
inline constexpr auto const& auto_name_pu = auto_name_pp<v,
                   impl::last_qualifier_pos(auto_name_pp<v>),-1>;

} // namespace ltl

#endif