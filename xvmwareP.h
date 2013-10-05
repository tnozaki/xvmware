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

#ifndef _XVMWARE_PRIVATE_H_
#define _XVMWARE_PRIVATE_H_

#include <X11/CoreP.h>
#include "xvmware.h"

typedef struct _XvmwarePart {
	Boolean autogrub;
	unsigned long interval;

	Boolean cursor_in;
	XtIntervalId interval_id;

	Atom atoms[3];
	XICCEncodingStyle styles[3];
} XvmwarePart;

static __inline void
XvmwarePart_init(Display *display, XvmwarePart *self)
{
	self->cursor_in= FALSE;
	self->interval_id = (XtIntervalId)0;

	self->atoms[0] = XA_COMPOUND_TEXT(display);
	self->atoms[1] = XA_TEXT(display);
	self->atoms[2] = XA_STRING;

	self->styles[0] = XCompoundTextStyle;
	self->styles[1] = XStdICCTextStyle;
	self->styles[2] = XStringStyle;
}

static __inline const Atom *
XvmwarePart_match_encoding(XvmwarePart *self, Atom value)
{
	unsigned long i;

	for (i = 0; i < XtNumber(self->atoms); ++i) {
		if (self->atoms[i] == value)
			return (const Atom *)&self->atoms[i];
	}
	return NULL;
}

static __inline const Atom *
XvmwarePart_best_encoding(XvmwarePart *self, Atom *atoms, unsigned long n)
{
	unsigned long i;

	for (i = 0; i < XtNumber(self->atoms); ++i) {
		while (n-- > 0) {
			if (self->atoms[i] == *atoms++)
				return (const Atom *)&self->atoms[i];
		}
	}
	return NULL;
}

static __inline XICCEncodingStyle
XvmwarePart_conversion_style(XvmwarePart *self, const Atom *pa)
{
	return self->styles[pa - self->atoms];
}

typedef struct _XvmwareRec {
	CorePart core;
	XvmwarePart xvmware;
} XvmwareRec;

typedef struct _XvmwareClassPart {
	int dummy;
} XvmwareClassPart;

typedef struct _XvmwareClassRec {
	CoreClassPart core_class;
	XvmwareClassPart xvmware_class;
} XvmwareClassRec;

extern XvmwareClassRec xvmwareClassRec;

#endif /*_XVMWARE_PRIVATE_H_*/
