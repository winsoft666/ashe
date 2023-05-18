#ifndef ASHE_CONFIG_HPP__
#define ASHE_CONFIG_HPP__
#pragma once

#define ASHE_VERSION_MAJOR 1
#define ASHE_VERSION_MINOR 0
#define ASHE_VERSION_PATCH 1

#ifdef ASHE_STATIC
#define ASHE_API
#else
#if defined(ASHE_EXPORTS)
#if defined(_MSC_VER)
#define ASHE_API __declspec(dllexport)
#else
#define ASHE_API
#endif
#else
#if defined(_MSC_VER)
#define ASHE_API __declspec(dllimport)
#else
#define ASHE_API
#endif
#endif
#endif



#endif //!ASHE_CONFIG_HPP__
