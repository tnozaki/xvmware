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

#ifndef _VMBACKDOOR_H_
#define _VMBACKDOOR_H_

/* backdoor commands */
#define VMBD_COMMAND_GET_POINTERPOS		0x4L
#define VMBD_COMMAND_SET_POINTERPOS		0x5L
#define VMBD_COMMAND_GET_CLIPBOARDLEN		0x6L
#define VMBD_COMMAND_GET_CLIPBOARDDATA		0x7L
#define VMBD_COMMAND_SET_CLIPBOARDLEN		0x8L
#define VMBD_COMMAND_SET_CLIPBOARDDATA		0x9L
#define VMBD_COMMAND_GET_VERSION		0xAL
#define VMBD_COMMAND_GET_PREFERENCES		0xDL
#define VMBD_COMMAND_SET_PREFERENCES		0xEL
#define VMBD_COMMAND_GET_HOSTSCREENSIZE		0xFL
#define VMBD_COMMAND_GET_HOSTTIMEOFDAY		0x17L

/* preferences */
/* grab when cursor enters window */
#define VMBD_PREFERENCE_GRAB			0x1L
/* ungrab when cursor leaves window */
#define VMBD_PREFERENCE_UNGRAB			0x2L
/* scroll when cursor approaches window edge */
#define VMBD_PREFERENCE_SCROLL			0x3L

#define VMBD_UNPACK_POINTER_X(pointer) \
		(((pointer) >> 16) & 0xFFFFL)
#define VMBD_UNPACK_POINTER_Y(pointer) \
		((pointer) & 0xFFFFL)
#define VMBD_PACK_POINTER(x, y) \
		((((x) & 0xFFFFL) << 16) | ((y) & 0xFFFFL))
#define VMBD_UNPACK_HOSTSCREENSIZE_X(size) \
		(((size) >> 16) & 0xFFFFL)
#define VMBD_UNPACK_HOSTSCREENSIZE_Y(size) \
		((size) & 0xFFFFL)

#endif /* _VMBACKDOOR_H_ */
