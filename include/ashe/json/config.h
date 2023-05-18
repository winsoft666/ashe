// Copyright 2007-2010 Baptiste Lepilleur
// Distributed under MIT license, or public domain if desired and
// recognized in your jurisdiction.
// See file LICENSE for detail or copy at http://jsoncpp.sourceforge.net/LICENSE

#ifndef JSON_CONFIG_H_INCLUDED
#define JSON_CONFIG_H_INCLUDED

#include "ashe/config.hpp"
#include "macros.hpp"

#if !defined(JSON_HAS_UNIQUE_PTR)
#if __cplusplus >= 201103L
#define JSON_HAS_UNIQUE_PTR (1)
#elif _MSC_VER >= 1600
#define JSON_HAS_UNIQUE_PTR (1)
#else
#define JSON_HAS_UNIQUE_PTR (0)
#endif
#endif

#if defined(_MSC_VER) && _MSC_VER <= 1200  // MSVC 6
// Microsoft Visual Studio 6 only support conversion from __int64 to double
// (no conversion from unsigned __int64).
#define JSON_USE_INT64_DOUBLE_CONVERSION 1
// Disable warning 4786 for VS6 caused by STL (identifier was truncated to '255'
// characters in the debug information)
// All projects I've ever seen with VS6 were using this globally (not bothering
// with pragma push/pop).
#pragma warning(disable : 4786)
#endif  // if defined(_MSC_VER)  &&  _MSC_VER < 1200 // MSVC 6

namespace Json {
typedef int Int;
typedef unsigned int UInt;
// For Microsoft Visual use specific types as long long is not supported
#if defined(_MSC_VER)  // Microsoft Visual Studio
typedef __int64 Int64;
typedef unsigned __int64 UInt64;
#else   // if defined(_MSC_VER) // Other platforms, use long long
typedef long long int Int64;
typedef unsigned long long int UInt64;
#endif  // if defined(_MSC_VER)
typedef Int64 LargestInt;
typedef UInt64 LargestUInt;
#define JSON_HAS_INT64
}  // end namespace Json

#endif  // JSON_CONFIG_H_INCLUDED
