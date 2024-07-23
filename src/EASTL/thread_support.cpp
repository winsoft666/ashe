///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////


#include <ashe/EASTL/internal/config.h>
#include <ashe/EASTL/internal/thread_support.h>
#include <ashe/EASTL/type_traits.h>
#include <ashe/EASTL/memory.h>

#if defined(EA_PLATFORM_MICROSOFT)
	EA_DISABLE_ALL_VC_WARNINGS();
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif
	#include <Windows.h>
	EA_RESTORE_ALL_VC_WARNINGS();
#endif

#ifndef EV_HAVE_DEBUG_OUTPUT
#define EV_HAVE_DEBUG_OUTPUT

#if EA_HAVE_CPP11_MUTEX
#pragma message(">>>>>>>>>>>>>>>>>>>>> EA_HAVE_CPP11_MUTEX")
#endif

#if EA_HAVE_CPP11_THREAD
#pragma message(">>>>>>>>>>>>>>>>>>>>> EA_HAVE_CPP11_THREAD")
#endif

#if EA_HAVE_CPP11_FUTURE
#pragma message(">>>>>>>>>>>>>>>>>>>>> EA_HAVE_CPP11_FUTURE")
#endif

#if EA_HAVE_CPP11_TYPE_TRAITS
#pragma message(">>>>>>>>>>>>>>>>>>>>> EA_HAVE_CPP11_TYPE_TRAITS")
#endif

#if EA_HAVE_CPP11_TUPLES
#pragma message(">>>>>>>>>>>>>>>>>>>>> EA_HAVE_CPP11_TUPLES")
#endif

#if EA_HAVE_CPP11_REGEX
#pragma message(">>>>>>>>>>>>>>>>>>>>> EA_HAVE_CPP11_REGEX")
#endif

#if EA_HAVE_CPP11_RANDOM
#pragma message(">>>>>>>>>>>>>>>>>>>>> EA_HAVE_CPP11_RANDOM")
#endif

#if EA_HAVE_CPP11_CHRONO
#pragma message(">>>>>>>>>>>>>>>>>>>>> EA_HAVE_CPP11_CHRONO")
#endif

#if EA_HAVE_CPP11_SCOPED_ALLOCATOR
#pragma message(">>>>>>>>>>>>>>>>>>>>> EA_HAVE_CPP11_SCOPED_ALLOCATOR")
#endif

#if EA_HAVE_CPP11_INITIALIZER_LIST
#pragma message(">>>>>>>>>>>>>>>>>>>>> EA_HAVE_CPP11_INITIALIZER_LIST")
#endif

#if EA_HAVE_CPP11_SYSTEM_ERROR
#pragma message(">>>>>>>>>>>>>>>>>>>>> EA_HAVE_CPP11_SYSTEM_ERROR")
#endif

#if EA_HAVE_CPP11_CODECVT
#pragma message(">>>>>>>>>>>>>>>>>>>>> EA_HAVE_CPP11_CODECVT")
#endif

#if EA_HAVE_CPP11_TYPEINDEX
#pragma message(">>>>>>>>>>>>>>>>>>>>> EA_HAVE_CPP11_TYPEINDEX")
#endif

#if EASTL_THREAD_SUPPORT_AVAILABLE
#pragma message(">>>>>>>>>>>>>>>>>>>>> EASTL_THREAD_SUPPORT_AVAILABLE")
#endif

#if EASTL_CPP11_MUTEX_ENABLED
#pragma message(">>>>>>>>>>>>>>>>>>>>> EASTL_CPP11_MUTEX_ENABLED")
#endif

#endif  // !EV_HAVE_DEBUG_OUTPUT




namespace eastl
{
	namespace Internal
	{
		#if EASTL_CPP11_MUTEX_ENABLED
			// We use the C++11 Standard Library mutex as-is.
		#else
			/////////////////////////////////////////////////////////////////
			// mutex
			/////////////////////////////////////////////////////////////////

			mutex::mutex()
			{
				#if defined(EA_PLATFORM_MICROSOFT)
					static_assert(sizeof(mMutexBuffer) == sizeof(CRITICAL_SECTION), "mMutexBuffer size failure");
					//static_assert(EA_ALIGN_OF(mMutexBuffer) >= EA_ALIGN_OF(CRITICAL_SECTION), "mMutexBuffer alignment failure"); // Enabling this causes the VS2012 compiler to crash.

					#if !defined(_WIN32_WINNT) || (_WIN32_WINNT < 0x0403)
						InitializeCriticalSection((CRITICAL_SECTION*)mMutexBuffer);
					#elif !EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)
						BOOL result = InitializeCriticalSectionEx((CRITICAL_SECTION*)mMutexBuffer, 10, 0);
						EASTL_ASSERT(result != 0); EA_UNUSED(result);
					#else
						BOOL result = InitializeCriticalSectionAndSpinCount((CRITICAL_SECTION*)mMutexBuffer, 10);
						EASTL_ASSERT(result != 0); EA_UNUSED(result);
					#endif

				#elif defined(EA_PLATFORM_POSIX)
					pthread_mutexattr_t attr;

					pthread_mutexattr_init(&attr);

					#if defined(EA_HAVE_pthread_mutexattr_setpshared_DECL)
						pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_PRIVATE);
					#endif

					pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
					pthread_mutex_init(&mMutex, &attr);
					pthread_mutexattr_destroy(&attr);
				#endif
			}

			mutex::~mutex()
			{
				#if defined(EA_PLATFORM_MICROSOFT)
					DeleteCriticalSection((CRITICAL_SECTION*)mMutexBuffer);
				#elif defined(EA_PLATFORM_POSIX)
					pthread_mutex_destroy(&mMutex);
				#endif
			}

			void mutex::lock()
			{
				#if defined(EA_PLATFORM_MICROSOFT)
					EnterCriticalSection((CRITICAL_SECTION*)mMutexBuffer);
				#elif defined(EA_PLATFORM_POSIX)
					pthread_mutex_lock(&mMutex);
				#else
					EASTL_FAIL_MSG("EASTL thread safety is not implemented yet. See EAThread for how to do this for the given platform.");
				#endif
			}

			void mutex::unlock()
			{
				#if defined(EA_PLATFORM_MICROSOFT)
					LeaveCriticalSection((CRITICAL_SECTION*)mMutexBuffer);
				#elif defined(EA_PLATFORM_POSIX)
					pthread_mutex_unlock(&mMutex);
				#endif
			}
		#endif


		/////////////////////////////////////////////////////////////////
		// shared_ptr_auto_mutex
		/////////////////////////////////////////////////////////////////

		// We could solve this by having single global mutex for all shared_ptrs, a set of mutexes for shared_ptrs, 
		// a single mutex for every shared_ptr, or have a template parameter that enables mutexes for just some shared_ptrs.
		eastl::late_constructed<mutex, true> gSharedPtrMutex;

		shared_ptr_auto_mutex::shared_ptr_auto_mutex(const void* /*pSharedPtr*/)
			: auto_mutex(*gSharedPtrMutex.get())
		{
		}


	} // namespace Internal

} // namespace eastl















