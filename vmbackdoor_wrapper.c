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
#include <iconv.h>
#include <limits.h>
#include <setjmp.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "iconv_wrapper.h"
#include "vmbackdoor.h"
#include "vmbackdoor_wrapper.h"

static sigjmp_buf vmbackdoor_segv_jmp;

static void
/*ARGSUSED*/
vmbackdoor_segv_handler(int signo)
{
	siglongjmp(vmbackdoor_segv_jmp, 1);
}

int
vmbakdoor_check_virtual_machine()
{
	struct sigaction act, old;
	struct vmbackdoor_data v;
	int ret;

	act.sa_handler = &vmbackdoor_segv_handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	sigaction(SIGSEGV, &act, &old);
	ret = sigsetjmp(vmbackdoor_segv_jmp, 1);
	if (ret == 0) {
		v.ecx = VMBD_COMMAND_GET_VERSION;
		vmbackdoor(&v);
	}
	sigaction(SIGSEGV, &old, NULL);
	return ret;
}

void
vmbackdoor_set_autogrub(int mode)
{
	struct vmbackdoor_data v;

	v.ecx = VMBD_COMMAND_GET_PREFERENCES;
	vmbackdoor(&v);
	v.ecx = VMBD_COMMAND_SET_PREFERENCES;
	v.ebx = v.eax;
	if (mode)
		v.ebx |= VMBD_PREFERENCE_AUTOGRUB;
	else
		v.ebx &= ~VMBD_PREFERENCE_AUTOGRUB;
	vmbackdoor(&v);
}

void
vmbackdoor_get_cursor(int * __restrict x, int * __restrict y)
{
	struct vmbackdoor_data v;

	DIAGASSERT(x != NULL);
	DIAGASSERT(y != NULL);

	v.ecx = VMBD_COMMAND_GET_POINTERPOS;
	vmbackdoor(&v);
	*x = VMBD_UNPACK_POINTER_X(v.eax);
	*y = VMBD_UNPACK_POINTER_Y(v.eax);
}

void
vmbackdoor_set_cursor(int x, int y)
{
	struct vmbackdoor_data v;

	v.ecx = VMBD_COMMAND_SET_POINTERPOS;
	v.ebx = VMBD_PACK_POINTER(x, y);
	vmbackdoor(&v);
}

/* XXX FIXME */
static __inline int
vmbackdoor_get_clipboard_nc(char ** __restrict ps, int * __restrict pn)
{
	struct vmbackdoor_data v;
	int n;
	char *s, *t;
	const char *p;

	v.ecx = VMBD_COMMAND_GET_CLIPBOARDLEN;
	vmbackdoor(&v);
	n = v.eax;
	if (n <= 0)
		return 1;
	s = malloc(n + 1);
	if (s == NULL)
		return 1; /* XXX: FIXME */
	t = s;
	v.ecx = VMBD_COMMAND_GET_CLIPBOARDDATA;
	while (n >= sizeof(v.eax)) {
		vmbackdoor(&v);
		p = (const char *)&v.eax;
		memcpy((void *)t, (const void *)p, sizeof(v.eax));
		t += sizeof(v.eax), n -= sizeof(v.eax);
	}
	if (n > 0) {
		vmbackdoor(&v);
		p = (const char *)&v.eax;
		memcpy((void *)t, (const void *)p, (size_t)n);
		t += n;
	}
	*t++ = '\0';
	*ps = s;
	*pn = t - s;
	return 0;
}

int
vmbackdoor_get_clipboard_by_enc(char ** __restrict ps, size_t * __restrict pn,
    const char * __restrict enc)
{
	int ret;
	iconv_t cd;
	char *src;
	int src_len;

	DIAGASSERT(ps != NULL);
	DIAGASSERT(pn != NULL);
	DIAGASSERT(enc != NULL);

	ret = 1;
	if (vmbackdoor_get_clipboard_nc(&src, &src_len) == 0) {
		cd = iconv_open(enc, VMBD_CLIPBOARD_ENCODING);
		if (cd != (iconv_t)-1) {
			ret = iconv_wrapper(cd,
			    src, (size_t)src_len, ps, pn, NULL);
			iconv_close(cd);
		}
		free(src);
	}
	return ret;
}

/* XXX FIXME */
static __inline void
vmbackdoor_set_clipboard_nc(const char *s, int n)
{
	struct vmbackdoor_data v;

	v.ecx = VMBD_COMMAND_SET_CLIPBOARDLEN;
	v.ebx = n;
	vmbackdoor(&v);
	v.ecx = VMBD_COMMAND_SET_CLIPBOARDDATA;
	while (n >= sizeof(v.ebx)) {
		memcpy((void *)&v.ebx, (const void *)s, sizeof(v.ebx));
		vmbackdoor(&v);
		s += sizeof(v.ebx), n -= sizeof(v.ebx);
	}
	if (n > 0) {
		memcpy((void *)&v.ebx, (const void *)s, n);
		vmbackdoor(&v);
	}
}

void
vmbackdoor_set_clipboard_by_enc(const char * __restrict s, size_t n,
    const char * __restrict enc)
{
	iconv_t cd;
	char *dst;
	size_t dst_len;

	DIAGASSERT(s != NULL);
	DIAGASSERT(n > 0);
	DIAGASSERT(enc != NULL);

	cd = iconv_open(VMBD_CLIPBOARD_ENCODING, enc);
	if (cd != (iconv_t)-1) {
		if (iconv_wrapper(cd, s, n, &dst, &dst_len, NULL) == 0) {
			if (dst_len <= INT_MAX)
				vmbackdoor_set_clipboard_nc(dst, dst_len);
			free(dst);
		}
		iconv_close(cd);
	}
}
