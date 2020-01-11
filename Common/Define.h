#pragma once

#include <cstdio>
#include <d3d11.h>

#if defined(DEBUG) | defined(_DEBUG)
	#ifndef ASSERT
		#define ASSERT(expr, msg) \
		{ \
			if (!(expr)) \
			{ \
				fprintf(stderr, "%s, %s, %s(%d)", (msg), (#expr), __FILE__, __LINE__); \
				__asm { int 3 } \
			} \
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

#ifndef CHECK_CALL_COUNT
	#define CHECK_CALL_COUNT(x) \
	{ \
		static int calls = 0; \
		ASSERT(++calls <= (x), "이 함수는 엔진에만 사용되므로 유저 층에서는 호출할 수 없습니다."); \
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
			__asm { int 3 } \
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

