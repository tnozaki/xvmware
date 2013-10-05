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

#include <stdlib.h>
#include <stdio.h>

#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>
#include <X11/Shell.h>
#include <X11/StringDefs.h>
#include <X11/Xatom.h>
#include <X11/Xmu/Atoms.h>
#include <X11/Xmu/Converters.h>
#include "xvmware.h"
#include "xvmwareP.h"

#include "xlanginfo.h"
#include "vmbackdoor_wrapper.h"

static Atom XA_VMWARE;

static XtResource resources[] = {
    {
	XtNautogrub, XtCAutogrub, XtRBoolean, sizeof(Boolean),
	XtOffsetOf(XvmwareRec, xvmware.autogrub), XtRImmediate,
	(XtPointer)TRUE
    }, {
	XtNinterval, XtCInterval, XtRLong, sizeof(long),
	XtOffsetOf(XvmwareRec, xvmware.interval), XtRImmediate,
	(XtPointer)10
    }
};

static Boolean
ConvertSelection(Widget widget, Atom *selection, Atom *target,
	Atom *encoding, XtPointer *value, unsigned long *nitems, int *format)
{
	Display *display;
	Window window;
	XvmwarePart *self;
	const Atom *pa;
	XTextProperty tp;
	unsigned long len;
	char *s;
	XICCEncodingStyle style;

	display = XtDisplay(widget);
	window = XtWindow(widget);
	self = &((XvmwareWidget)widget)->xvmware;

	if (*target == XA_TARGETS(display)) {
		*encoding = XA_ATOM;
		*value = (XtPointer)XtMalloc(
		    (Cardinal)sizeof(self->atoms)
		);
		memcpy((void *)*value, (const void *)
		    self->atoms, sizeof(self->atoms));
		*nitems = XtNumber(self->atoms);
		*format = 32;
		return TRUE;
	}
	pa = XvmwarePart_match_encoding(self, *target);
	if (pa == NULL)
		return FALSE;
	XGetWindowProperty(
	    display, window, XA_VMWARE,
	    0L, 0L, False, self->atoms[0],
	    &tp.encoding, &tp.format,
	    &tp.nitems, &len, &tp.value
	);
	XtFree((char *)tp.value);
	if (tp.encoding != self->atoms[0])
		return FALSE;
	XGetWindowProperty(
	    display, window, XA_VMWARE,
	    0L, len, False, self->atoms[0],
	    &tp.encoding, &tp.format,
	    &tp.nitems, &len, &tp.value
	);
	if (*pa != self->atoms[0]) {
		s = (char *)tp.value;
		style = XvmwarePart_conversion_style(self, pa);
		XmbTextListToTextProperty(
		    display, &s, 1, style, &tp
		);
		XtFree(s);
	}
	*encoding = tp.encoding;
	*value = (XtPointer)tp.value;
	*nitems = tp.nitems;
	*format = tp.format;
	return TRUE;
}

static void
LoseSelection(Widget widget, Atom *selection)
{
	Display *display;
	Window window;

	display = XtDisplay(widget);
	window = XtWindow(widget);
	XDeleteProperty(display, window, XA_VMWARE);
}

static void
SelectionReceived(Widget widget,
	XtPointer client_data, Atom *selection, Atom *encoding,
	XtPointer value, unsigned long *nitems, int *format)
{
	XvmwarePart *self;
	Display *display;
	Window window;
	const Atom *pa;
	XTextProperty tp;
	char **list, *s;
	int nlist;
	size_t n;

	if (*encoding == XT_CONVERT_FAIL)
		return;
	if (nitems == NULL || *nitems == 0)
		return;
	self = &((XvmwareWidget)widget)->xvmware;
	display = XtDisplay(widget);
	window = XtWindow(widget);
	if (*encoding == XA_ATOM) {
		pa = XvmwarePart_best_encoding(self, (Atom *)value, *nitems);
		if (pa != NULL) {
			XtGetSelectionValue(
			    widget, *selection, *pa,
			    &SelectionReceived, NULL, CurrentTime
			);
		}
	} else {
		pa = XvmwarePart_match_encoding(self, *encoding);
		if (pa != NULL) {
			tp.encoding = *encoding;
			tp.nitems = *nitems;
			tp.value = (unsigned char *)value;
			tp.format = *format;
			XmbTextPropertyToTextList(
			    display, &tp, &list, &nlist
			);
			s = *list;
			n = strlen(s);
			XChangeProperty(
			    display, window,
			    XA_VMWARE, self->atoms[0], 8,
			    PropModeReplace,
			    (unsigned char *)s, (int)n
			);
			vmbackdoor_set_clipboard_by_enc(
			    (const char *)s, n, nl_langinfo(CODESET));
			XFreeStringList(list);
		}
		XtOwnSelection(
		    widget, XA_PRIMARY, CurrentTime,
		    &ConvertSelection, &LoseSelection, NULL
		);
	}
	XtFree((char *)client_data);
	XtFree((char *)value);
}

static void
WatchCursor(XtPointer client_data, XtIntervalId *id)
{
	Widget widget;
	XtAppContext app;
	XvmwarePart *self;
	Display *display;
	Window window, root, rroot, rchild;
	int guest_x, guest_y, host_x, host_y;
	unsigned int width, height, border, depth, mask;
	Boolean cursor_in;
	char *s;
	size_t n;

	widget = (Widget)client_data;
	app = XtWidgetToApplicationContext(widget);
	self = &((XvmwareWidget)widget)->xvmware;
	display = XtDisplay(widget);
	window = XtWindow(widget);
	root = DefaultRootWindow(display);
	XGetGeometry(
	    display, (Drawable)root, &rroot,
	    &guest_x, &guest_y,
	    &width, &height, &border, &depth
	);
	vmbackdoor_get_cursor(&host_x, &host_y);
	cursor_in = (0 <= host_x && host_x < width) &&
		    (0 <= host_y && host_y < height) ? TRUE : FALSE;
	if (cursor_in == TRUE) {
		if (self->cursor_in == TRUE) {
			XQueryPointer(
			    display, root, &rroot, &rchild,
			    &guest_x, &guest_y, &host_x, &host_y, &mask
			);
			vmbackdoor_set_cursor(guest_x, guest_y);
		} else {
			XWarpPointer(
			    display, None, root,
			    0, 0, 0, 0, host_x, host_y
			);
			if (vmbackdoor_get_clipboard_by_enc(
			    &s, &n, nl_langinfo(CODESET)) == 0) {
				XChangeProperty(
				    display, window,
				    XA_VMWARE, self->atoms[0], 8,
				    PropModeReplace,
				    (unsigned char *)s, (int)n
				);
				free(s);
			}
			if (XGetSelectionOwner(display, XA_PRIMARY) != window) {
				XtOwnSelection(
				    widget, XA_PRIMARY, CurrentTime,
				    &ConvertSelection, &LoseSelection, NULL
				);
			}
		}
	} else if (self->cursor_in == TRUE) {
		if (XGetSelectionOwner(display, XA_PRIMARY) != window) {
			XtGetSelectionValue(
			    widget, XA_PRIMARY, XA_TARGETS(display),
			    &SelectionReceived, NULL, CurrentTime
			);
		}
	}
	self->cursor_in = cursor_in;
	self->interval_id = XtAppAddTimeOut(
	    app, self->interval,
	    &WatchCursor, (XtPointer)widget
	);
}

static void
ClassInitialize()
{
	XtAddConverter(XtRString, XtRLong, XmuCvtStringToLong, NULL, 0);
}

static void
Initialize(Widget greq, Widget gnew, ArgList args, Cardinal *num_args)
{
	XvmwarePart *self;
	Display *display;

	self = &((XvmwareWidget)gnew)->xvmware;
	display = XtDisplay(gnew);

	XvmwarePart_init(display, self);

	vmbackdoor_set_autogrub(self->autogrub);
}

static void
Realize(Widget widget, XtValueMask *mask, XSetWindowAttributes *attr)
{
	XtAppContext app;
	XvmwarePart *self;

	app = XtWidgetToApplicationContext(widget);
	XtCreateWindow(
	    widget, (unsigned)InputOutput,
	    (Visual *)CopyFromParent, *mask, attr
	);
	self = &((XvmwareWidget)widget)->xvmware;
	self->interval_id = XtAppAddTimeOut(
	    app, self->interval,
	    &WatchCursor, (XtPointer)widget
	);
}

static void
Destroy(Widget widget)
{
	XvmwarePart *self;
	Display *display;
	Window window;

	display = XtDisplay(widget);
	window = XtWindow(widget);
	self = &((XvmwareWidget)widget)->xvmware;
	if (self->interval_id != (XtIntervalId)0)
		XtRemoveTimeOut(self->interval_id);
}

XvmwareClassRec xvmwareClassRec =
{
    {
/* superclass            */ &widgetClassRec,
/* class_name            */ "Xvmware",
/* size                  */ sizeof(XvmwareRec),
/* class_initialize      */ &ClassInitialize,
/* class_part_initialize */ NULL,
/* class_inited          */ FALSE,
/* initialize            */ &Initialize,
/* initialize_hook       */ NULL,
/* realize               */ &Realize,
/* actions               */ NULL,
/* num_actions           */ 0,
/* resources             */ &resources[0],
/* num_resources         */ XtNumber(resources),
/* xrm_class             */ NULLQUARK,
/* compress_motion       */ TRUE,
/* compress_exposure     */ TRUE,
/* compress_enterleave   */ TRUE,
/* visible_interest      */ FALSE,
/* destroy               */ &Destroy,
/* resize                */ NULL,
/* expose                */ NULL,
/* set_values            */ NULL,
/* set_values_hook       */ NULL,
/* set_values_almost     */ NULL,
/* get_values_hook       */ NULL,
/* accept_focus          */ NULL,
/* version               */ XtVersion,
/* callback_private      */ NULL,
/* tm_table              */ NULL,
/* query_geometry        */ XtInheritQueryGeometry
    }
};

WidgetClass xvmwareWidgetClass = (WidgetClass)&xvmwareClassRec;

static XrmOptionDescRec options[] = {
{ "-noautogrub", "*autogrub",  XrmoptionNoArg,     "false" },
{ "-interval",   "*interval",  XrmoptionSepArg,    NULL    },
};

int
main(int argc, char *argv[])
{
	Arg ac[3];
	XtAppContext app;
	Widget toplevel;
	Display *display;

	if (vmbakdoor_check_virtual_machine() == 0) {
		XtSetLanguageProc(NULL, (XtLanguageProc)NULL, NULL);
		XtSetArg(ac[0], XtNwidth, 1);
		XtSetArg(ac[1], XtNheight, 1);
		XtSetArg(ac[2], XtNmappedWhenManaged, False);
		toplevel = XtOpenApplication(
		    &app, "Xvmware",
		    &options[0], XtNumber(options),
		    &argc, argv,
		    NULL, sessionShellWidgetClass,
		    ac, XtNumber(ac)
		);
		display = XtDisplay(toplevel);
		XA_VMWARE = XInternAtom(display, "VMWARE", False);
		XtCreateManagedWidget(
		    "xvmware", xvmwareWidgetClass,
		    toplevel, NULL, 0
		);
		XtRealizeWidget(toplevel);
		XtAppMainLoop(app);
		/* NOTREACHED */
	}
	return 1;
}
