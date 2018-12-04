/*
** Resources for the sys.path initialization, the Python options
** and the preference filename
*/
#include "MacTypes.r"
#include "patchlevel.h"
#include "pythonresources.h"

/* A few resource type declarations */

type 'Popt' {
	literal byte version = POPT_VERSION_CURRENT;
	byte noInspect = 0, inspect = 1;
	byte noVerbose = 0, verbose = 1;
	byte noOptimize = 0, optimize = 1;
	byte noUnbuffered = 0, unbuffered = 1;
	byte noDebugParser = 0, debugParser = 1;
	byte unused_0 = 0, unused_1 = 1;
	byte closeAlways = POPT_KEEPCONSOLE_NEVER,
	     noCloseOutput = POPT_KEEPCONSOLE_OUTPUT,
	     noCloseError = POPT_KEEPCONSOLE_ERROR,
	     closeNever = POPT_KEEPCONSOLE_ALWAYS;
	byte interactiveOptions = 0, noInteractiveOptions = 1;
	byte argcArgv = 0, noArgcArgv = 1;
	byte newStandardExceptions = 0, oldStandardExceptions = 1;
	byte sitePython = 0, noSitePython = 1;
	byte navService = 0, noNavService = 1;
	byte noDelayConsole = 0, delayConsole = 1;
	byte noDivisionWarning = 0, divisionWarning = 1;
	byte noUnixNewlines = 0, unixNewlines = 1;
};

type 'TMPL' {
	wide array {
		pstring;
		literal longint;
	};
};

/* The resources themselves */

/* Popt template, for editing them in ResEdit */

resource 'TMPL' (PYTHONOPTIONS_ID, "Popt") {
	{
		"preference version",			'DBYT',
		"Interactive after script",		'DBYT',
		"Verbose import",				'DBYT',
		"Optimize",						'DBYT',
		"Unbuffered stdio",				'DBYT',
		"Debug parser",					'DBYT',
		"Keep window on normal exit",	'DBYT',
		"Keep window on error exit",	'DBYT',
		"No interactive option dialog",	'DBYT',
		"No argc/argv emulation",		'DBYT',
		"Old standard exceptions",		'DBYT',
		"No site-python support",		'DBYT',
		"No NavServices in macfs",		'DBYT',
		"Delay console window",			'DBYT',
		"Warnings for old-style division",	'DBYT',
		"Allow unix newlines on textfile input",'DBYT',
	}
};
