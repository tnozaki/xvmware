/*-
 * Copyright (c)2008 Takehiko NOZAKI,
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#if defined(__NetBSD__)
#include <sys/cdefs.h>
#define UNCONST(ptr)	__UNCONST(ptr)
#else
#define UNCONST(ptr)	((void *)(unsigned long)(const void *)ptr)
#endif
#include <assert.h>
#if defined(__NetBSD__)
#define DIAGASSERT(exp)	_DIAGASSERT(exp)
#else
#define DIAGASSERT(exp)	assert(exp)
#endif
#include <stdlib.h>

#include "xlanginfo.h"

#if defined(X_LOCALE)

#if 0
#include "Xlcint.h"
#include "XlcPublic.h"
#else
#define XlcNCodeset "codeset"
typedef struct _XLCd *XLCd;
extern XLCd _XlcCurrentLC (void);
extern char *_XGetLCValues(XLCd, ...);
#endif

char *
Xnl_langinfo(Xnl_item item)
{
	XLCd lcd;
	char *s;

	switch (item) {
	case XCODESET:
		lcd = _XlcCurrentLC();
		DIAGASSERT(lcd != NULL);
		s = NULL;
		_XGetLCValues(lcd, XlcNCodeset, &s, NULL);
		if (s == NULL)
			s = "US-ASCII";
		break;
	default:
		s = "";
	}

	return s;
}

#elif defined(USE_GNU_LIBICONV)

#include <libcharset.h>

char *
Xnl_langinfo(Xnl_item item)
{
	const char *s;

	switch (item) {
	case XCODESET:
		s = locale_charset();
		DIAGASSERT(s != NULL);
		break;
	default:
		s = "";
	}

	return UNCONST(s);
}
#endif
