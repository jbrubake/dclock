/*
 * Copyright (c) 1988 Dan Heller <argv@sun.com>
 * dclock -- program to demonstrate how to use the digital-clock widget.
 * To specify a date, the date format is a string corresponding to the
 * syntax of strftime(3).
 * To specify seconds to be displayed, use "-seconds" or use the resource
 * manager: *Dclock.seconds: on
 */
#include <stdio.h>
#include <locale.h>
#include <X11/Intrinsic.h>
#include "Dclock.h"

static XrmOptionDescRec options[] = {
    {"-date",	   "*Dclock.date",	  XrmoptionSepArg, NULL    },
    {"-dateup",    "*Dclock.dateUp",	  XrmoptionNoArg,  "TRUE"  },
    {"-nodateup",  "*Dclock.dateUp",	  XrmoptionNoArg,  "FALSE" },
    {"-seconds",   "*Dclock.seconds",	  XrmoptionNoArg,  "TRUE"  },
    {"-miltime",   "*Dclock.militaryTime",XrmoptionNoArg,  "TRUE"  },
    {"-nomiltime", "*Dclock.militaryTime",XrmoptionNoArg,  "FALSE" },
    {"-stdtime",   "*Dclock.militaryTime",XrmoptionNoArg,  "FALSE" },
    {"-utc",       "*Dclock.utcTime",     XrmoptionNoArg,  "TRUE"  },
    {"-noutc",     "*Dclock.utcTime",     XrmoptionNoArg,  "FALSE" },
    {"-disptime",  "*Dclock.displayTime", XrmoptionNoArg,  "TRUE"  },
    {"-audioPlay", "*Dclock.audioPlay",	  XrmoptionSepArg, NULL    },
    {"-bell",	   "*Dclock.bell",	  XrmoptionNoArg,  "TRUE"  },
    {"-nobell",	   "*Dclock.bell",	  XrmoptionNoArg,  "FALSE" },
    {"-bellFile",  "*Dclock.bellFile",	  XrmoptionSepArg, NULL    },
    {"-alarm",     "*Dclock.alarm",	  XrmoptionNoArg,  "TRUE"  },
    {"-noalarm",   "*Dclock.alarm",	  XrmoptionNoArg,  "FALSE" },
    {"-persist",   "*Dclock.alarmPersist",XrmoptionNoArg,  "TRUE"  },
    {"-nopersist", "*Dclock.alarmPersist",XrmoptionNoArg,  "FALSE" },
    {"-alarmTime", "*Dclock.alarmTime",	  XrmoptionSepArg, NULL    },
    {"-alarmFile", "*Dclock.alarmFile",	  XrmoptionSepArg, NULL    },
    {"-blink",     "*Dclock.blink",	  XrmoptionNoArg,  "TRUE"  },
    {"-noblink",   "*Dclock.blink",	  XrmoptionNoArg,  "FALSE" },
    {"-tails",     "*Dclock.tails",	  XrmoptionNoArg,  "TRUE"  },
    {"-notails",   "*Dclock.tails",	  XrmoptionNoArg,  "FALSE" },
    {"-scroll",    "*Dclock.scroll",	  XrmoptionNoArg,  "TRUE"  },
    {"-noscroll",  "*Dclock.scroll",	  XrmoptionNoArg,  "FALSE" },
    {"-fade",	   "*Dclock.fade",	  XrmoptionNoArg,  "TRUE"  },
    {"-nofade",	   "*Dclock.fade",	  XrmoptionNoArg,  "FALSE" },
    {"-fadeRate",  "*Dclock.fadeRate",	  XrmoptionSepArg, NULL    },
    {"-slope",	   "*Dclock.angle",	  XrmoptionSepArg, NULL    },
    {"-thickness", "*Dclock.widthFactor", XrmoptionSepArg, NULL    },
    {"-smallsize", "*Dclock.smallRatio",  XrmoptionSepArg, NULL    },
    {"-spacing",   "*Dclock.spaceFactor", XrmoptionSepArg, NULL    },
    {"-second_gap","*Dclock.secondGap",	  XrmoptionSepArg, NULL    },
    {"-led_off",   "*Dclock.led_off",	  XrmoptionSepArg, NULL    },
#ifdef XFT_SUPPORT
    {"-fontName",  "*Dclock.fontName",	  XrmoptionSepArg, NULL    },
#endif
};

static void
Usage(name, args)
String name, *args;
{
    static char *help_message[] = {
	"where options include:",
	"    -help			print this help text",
	"    -bg color			field background color",
	"    -fg color			segment foreground color",
	"    -fn font			font name",
	"    -geometry geom		size of mailbox",
	"    -display host:dpy		X server to contact",
	"    -led_off color		segment background color",
	"    -date \"date format\"	show the date in strftime(3) format",
	"    -[no]dateup 		[don't]	put the date up at the top",
	"    -[no]seconds 		[don't]	display seconds",
	"    -[no]miltime 		[don't]	display time in 24 hour format",
	"    -[no]utc			[don't] display the UTC time instead of local",
	"    -[no]blink			[don't] blink the colon",
	"    -[no]scroll 		turn on [off] scrolling of numbers",
	"    -[no]tails			draw [remove] tails on digits 6 and 9",
	"    -[no]fade			[don't] fade numbers",
	"    -[no]bell 			[don't] ring bell each half hour",
	"    -[no]persist		[don't] leave in reverse video after alarm",
	"    -bellFile filename		sound file for bell sound",
	"    -[no]alarm 		turn on/off alarm",
	"    -alarmTime hh:mm		time alarm goes off",
	"    -alarmFile filename	sound file for alarm sound",
        "    -audioPlay	filename	executable to use to play bell and alarm",
	"    -fadeRate int_val		wait between fade steps (in msec)",
	"    -slope float_val		set angle of the digits",
	"    -smallsize float_val	set size ratio of the small to large digits",
	"    -second_gap float_val	set spacing between minutes and seconds digits",
	"    -thickness	float_val	set segment thickness as ratio to digit width",
	"    -spacing float_val		set digit spacing as ratio to digit width",
#ifdef XFT_SUPPORT
	"    -fontName name		name of freefont font to use for date string",
#endif
	NULL
    };
    char **cpp;

    fprintf(stderr, "Invalid Args:");
    while (*args)
	fprintf(stderr, " \"%s\"", *args++);
    fputc('\n', stderr);
    fprintf(stderr, "usage: %s [-options ...]\n", name);
    for (cpp = help_message; *cpp; cpp++)
	fprintf(stderr, "%s\n", *cpp);
    exit(1);
}

static void
quit()
{
    exit(0);
}

static XtActionsRec actionsList[] = {
    { "quit",	quit },
};

main(argc, argv)
char *argv[];
{
    XtAppContext app;
    Widget toplevel, clock_w;
    char *name, *rindex();
    XWMHints     *wmhints;       /* for proper input focus */

    if (name = rindex(argv[0], '/'))
	name++;
    else
	name = argv[0];

    setlocale(LC_TIME, "");

    toplevel = XtAppInitialize(&app, "Dclock", options, XtNumber(options),
			&argc, argv, (String *)NULL, (ArgList)NULL, 0);
    XtAppAddActions(app, actionsList, 1);

    if (argc != 1)
	Usage(name, argv+1);

    clock_w = XtCreateManagedWidget(name, dclockWidgetClass, toplevel, NULL, 0);
    XtOverrideTranslations(clock_w, XtParseTranslationTable("<Key>q: quit()"));

    XtRealizeWidget(toplevel);

    wmhints = XGetWMHints(XtDisplay(toplevel), XtWindow(toplevel));
    wmhints->input = True;
    wmhints->flags |= InputHint;
    XSetWMHints(XtDisplay(toplevel), XtWindow(toplevel), wmhints);
    XFree(wmhints);
    XtAppMainLoop(app);
    return 0;
}
