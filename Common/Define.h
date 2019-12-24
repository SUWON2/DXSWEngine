#pragma once

#include <cstdio>

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
