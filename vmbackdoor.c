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

/*
 * Copyright (C) 2004 WIDE Project.
 * Copyright (C) 2004 Masafumi Oe <masa@fumi.org>
 * Copyright (C) 2004 Naoto Morishima <naoto@morishima.net>
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
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <assert.h>
#if defined(__NetBSD__)
#define DIAGASSERT(exp)	_DIAGASSERT(exp)
#else
#define DIAGASSERT(exp)	assert(exp)
#endif
#include <stdlib.h>

#include "vmbackdoor.h"

void
vmbackdoor(struct vmbackdoor_data *data)
{
    DIAGASSERT(data != NULL);

    __asm__(
#if defined(__i386__)
	"pushl %eax"			"\n\t"
	"pushl %ebx"			"\n\t"
	"pushl %ecx"			"\n\t"
	"pushl %edx"			"\n\t"
	"movl 8(%ebp), %edx"		"\n\t"
	"movl $0x564D5868, %eax"	"\n\t"
	"movl 4(%edx), %ebx"		"\n\t"
	"movl 8(%edx), %ecx"		"\n\t"
	"mov $0x5658, %dx"		"\n\t"
	"inl %dx, %eax"			"\n\t"
	"movl 8(%ebp), %edx"		"\n\t"
	"movl %eax, (%edx)"		"\n\t"
	"movl %ebx, 4(%edx)"		"\n\t"
	"movl %ecx, 8(%edx)"		"\n\t"
	"popl %edx"			"\n\t"
	"popl %ecx"			"\n\t"
	"popl %ebx"			"\n\t"
	"popl %eax"			"\n\t"
#elif defined(__x86_64__) /* XXX: NOT TESTED!! */
	"pushq %rax"			"\n\t"
	"pushq %rbx"			"\n\t"
	"pushq %rcx"			"\n\t"
	"pushq %rdx"			"\n\t"
	"movq 16(%rbp), %rdx"		"\n\t"
	"movq $0x564D5868, %rax"	"\n\t"
	"movq 8(%rdx), %rbx"		"\n\t"
	"movq 16(%rdx), %rcx"		"\n\t"
	"mov $0x5658, %dx"		"\n\t"
	"inl %dx, %eax"			"\n\t"
	"movq 16(%rbp), %rdx"		"\n\t"
	"movq %rax, (%rdx)"		"\n\t"
	"movq %rbx, 8(%rdx)"		"\n\t"
	"movq %rcx, 16(%rdx)"		"\n\t"
	"popq %rdx"			"\n\t"
	"popq %rcx"			"\n\t"
	"popq %rbx"			"\n\t"
	"popq %rax"			"\n\t"
#else
#error "unsupported CPU_ARCH"
#endif
    );
}
