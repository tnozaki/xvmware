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

#include <assert.h>
#if defined(__NetBSD__)
#define DIAGASSERT(exp)	_DIAGASSERT(exp)
#else
#define DIAGASSERT(exp)	assert(exp)
#endif
#include <errno.h>
#include <iconv.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef BUFSIZ
#define BUFSIZ		1024
#endif

#include "iconv_wrapper.h"

int
iconv_wrapper(iconv_t cd,
	const char * __restrict src, size_t src_len,
	char ** __restrict pdst, size_t * __restrict pdst_len,
	size_t * __restrict pirr)
{
	size_t irr, dst_len, tmp_len, n;
	char *dst, *tmp;

	DIAGASSERT(src != NULL);
	DIAGASSERT(pdst != NULL);
	DIAGASSERT(pdst_len != NULL);
	/* pirr may be null */

	irr = (size_t)0;
	dst = malloc(BUFSIZ);
	if (dst == NULL)
		return 1;
	tmp = dst;
	dst_len = tmp_len = BUFSIZ;
	while (src_len > 0) {
		n = iconv(cd, (const char **)&src, &src_len,
		    &tmp, &tmp_len);
		if (n != (size_t)-1) {
			irr += n;
		} else {
			switch (errno) {
			case E2BIG:
				n = dst_len;
				dst_len *= 2;
				if (dst_len <= n)
					goto err;
				n = tmp - dst;
				tmp = realloc(dst, dst_len);
				if (tmp != NULL) {
					dst = tmp;
					tmp += n;
					tmp_len = dst_len - n;
					break;
				}
			/*FALLTHROUGH*/
			default:
				goto err;
			}
		}
	}
	for (;;) {
		n = iconv(cd, NULL, NULL, &tmp, &tmp_len);
		if (n != (size_t)-1) {
			irr += n;
			break;
		}
		switch (errno) {
		case E2BIG:
			n = dst_len;
			dst_len *= 2;
			if (dst_len <= n)
				goto err;
			n = tmp - dst;
			tmp = realloc(dst, dst_len);
			if (tmp != NULL) {
				dst = tmp;
				tmp += n;
				tmp_len = dst_len - n;
				break;
			}
		/*FALLTHROUGH*/
		default:
			goto err;
		}
	}
	n = tmp - dst;
	DIAGASSERT(dst_len >= n);
	tmp = realloc(dst, n);
	if (tmp != NULL) {
		*pdst = tmp;
		*pdst_len = n;
		if (pirr != NULL)
			*pirr = irr;
		return 0;
	}
err:
	free(dst);
	return 1;
}
