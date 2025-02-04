#pragma once

#include <cstdio>
#include <stdint.h>
#include <d3d11.h>

typedef size_t ID;

#if defined(DEBUG) | defined(_DEBUG)
	#ifndef ASSERT
		#define ASSERT(expr, msg) \
		if (!(expr)) \
		{ \
			fprintf(stderr, "%s, %s, %s(%d)", (msg), (#expr), __FILE__, __LINE__); \
			__debugbreak(); \
		}
	#endif
#else
	#ifndef ASSERT
		#define ASSERT(expr, msg) ((void)0)
	#endif
#endif

#ifndef RELEASE
	#define RELEASE(x) \
	if ((x) != nullptr) \
	{ \
		delete (x); \
		(x) = nullptr; \
	}
#endif

#ifndef RELEASE_ARRAY
	#define RELEASE_ARRAY(x) \
	if ((x) != nullptr) \
	{ \
		delete[] (x); \
		(x) = nullptr; \
	}
#endif

#if defined(DEBUG) | defined(_DEBUG)
	#ifndef HR
		#define HR(x) \
		if (FAILED((x))) \
		{ \
			LPVOID errorLog = nullptr; \
			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ALLOCATE_BUFFER, \
				nullptr, (x), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), \
				reinterpret_cast<LPSTR>(&errorLog), 0, nullptr); \
			fprintf(stderr, "%s", static_cast<char*>(errorLog)); \
			LocalFree(errorLog); \
			__debugbreak(); \
		}
	#endif
#else
	#ifndef HR
		#define	HR(x) (x);
	#endif
#endif

#ifndef RELEASE_COM
	#define RELEASE_COM(x) \
	if ((x) != nullptr) \
	{ \
		(x)->Release(); \
		(x) = nullptr; \
	}
#endif

