/*******************************************************************************
*    C++ Common Library
*    ---------------------------------------------------------------------------
*    Copyright (C) 2020~2024 winsoft666 <winsoft666@outlook.com>.
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#ifndef ASHE_ARCH_HPP__
#define ASHE_ARCH_HPP__

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#define ASHE_WIN 1
#if (defined _WIN64) || (defined WIN64)
#define ASHE_WIN64 1
#else
#define ASHE_WIN32 1
#endif
#elif defined(__APPLE__)
#define ASHE_MACOS 1
#elif defined(__linux__) || defined(linux) || defined(__linux)
#define ASHE_LINUX 1
#else
#error Unknown OS
#endif

#if defined(ASHE_MACOS) || defined(ASHE_LINUX)
#define ASHE_POSIX 1
#define ASHE_UNIX 1
#endif

// Processor architecture detection.
// For more info on what's defined, see:
//   http://msdn.microsoft.com/en-us/library/b0084kay.aspx
//   http://www.agner.org/optimize/calling_conventions.pdf
//   or with gcc, run: "echo | gcc -E -dM -"
#if defined(_M_X64) || defined(__x86_64__)
#define ASHE_ARCH_X86_FAMILY 1
#define ASHE_ARCH_X86_64 1
#define ASHE_ARCH_64_BITS 1
#define ASHE_ARCH_LITTLE_ENDIAN 1
#elif defined(_M_ARM64) || defined(__aarch64__)
#define ASHE_ARCH_ARM_FAMILY 1
#define ARCH_ARCH_ARM64 1
#define ASHE_ARCH_64_BITS 1
#define ASHE_ARCH_LITTLE_ENDIAN 1
#elif defined(_M_IX86) || defined(__i386__)
#define ASHE_ARCH_X86_FAMILY 1
#define ASHE_ARCH_X86 1
#define ASHE_ARCH_32_BITS 1
#define ASHE_ARCH_LITTLE_ENDIAN 1
#elif defined(__ARMEL__)
#define ASHE_ARCH_ARM_FAMILY 1
#define ASHE_ARCH_32_BITS 1
#define ASHE_ARCH_LITTLE_ENDIAN 1
#elif defined(__MIPSEL__)
#define ASHE_ARCH_MIPS_FAMILY 1
#if defined(__LP64__)
#define ASHE_ARCH_64_BITS 1
#else
#define ASHE_ARCH_32_BITS 1
#endif
#define ASHE_ARCH_LITTLE_ENDIAN 1
#elif defined(__pnacl__)
#define ASHE_ARCH_32_BITS 1
#define ASHE_ARCH_LITTLE_ENDIAN 1
#elif defined(__EMSCRIPTEN__)
#define ASHE_ARCH_32_BITS 1
#define ASHE_ARCH_LITTLE_ENDIAN 1
#else
#error Please add support for your architecture in ashe/arch.h
#endif

#if !(defined(ASHE_ARCH_LITTLE_ENDIAN) ^ defined(ASHE_ARCH_BIG_ENDIAN))
#error Define either ASHE_ARCH_LITTLE_ENDIAN or ASHE_ARCH_BIG_ENDIAN
#endif

#endif  // !ASHE_ARCH_HPP__