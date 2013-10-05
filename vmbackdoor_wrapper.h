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
#ifndef VMBACKDOOR_WRAPPER_H_
#define VMBACKDOOR_WRAPPER_H_

#define VMBD_CLIPBOARD_ENCODING	"UTF-8"
#define VMBD_PREFERENCE_AUTOGRUB (VMBD_PREFERENCE_GRAB|VMBD_PREFERENCE_UNGRAB)

int	vmbakdoor_check_virtual_machine();
void	vmbackdoor_set_autogrub(int);
void	vmbackdoor_get_cursor(int *, int *);
void	vmbackdoor_set_cursor(int, int);
int	vmbackdoor_get_clipboard_by_enc(char **, size_t *, const char *);
void	vmbackdoor_set_clipboard_by_enc(const char *, size_t, const char *);

#endif /*!VMBACKDOOR_WRAPPER_H_*/
