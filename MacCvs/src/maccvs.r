#define SystemSevenOrLater 1

#ifdef __MWERKS__
    #include <MacTypes.r>
    #include <AEUserTermTypes.r>
    #include <AEObjects.r>
    #ifndef MACHO
        #include "Python.r"
    #endif
#else
    #include <Carbon.r>
#endif

resource 'aete' (0, "MacCVS AppleEvent Suites") {
	0x01, 0x00, english, roman,
	{
		"required Suite", "Events supported by all applications", 'reqd', 1, 1,
		{},
		{},
		{},
		{},
		"MacCVS Suite", "Custom events", 'MCVS', 1, 1,
		{
			"Do Command", "Execute a CVS command", 'misc', 'dosc',
			'null', "", replyOptional, singleItem, notEnumerated, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, 
			'TEXT', "Command to execute", directParamRequired, listOfItems, notEnumerated,
			changesState, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, 
			{
				"Mode", 'MODE', 'MODE', "Mode (AE, File).", optional, singleItem, enumerated, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, 
				"Environment", 'ENVT', 'TEXT', "Environment variables.", optional, listOfItems, notEnumerated, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved,
				"Filename", 'FILE', 'TEXT', "Output file path.", optional, singleItem, notEnumerated, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, 
				"Pathway", 'SPWD', 'TEXT', "Starting pathway.", optional, singleItem, notEnumerated, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved,
				"NoLineBuffer", 'LBUF', 'bool', "if true, send each result line as separate AE.", optional, singleItem, notEnumerated, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, 
			},
			"cvs", "Execute a CVS command", 'mCVS', 'cmnd',
			'null', "", replyOptional, singleItem, notEnumerated, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, 
			'TEXT', "Command to execute", directParamRequired, listOfItems, notEnumerated,
			changesState, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, 
			{
				"Environment", 'ENVT', 'TEXT', "Environment variables.", optional, listOfItems, notEnumerated, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved,
				"Pathway", 'SPWD', 'TEXT', "Starting pathway.", optional, singleItem, notEnumerated, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved,
			}
		},
		{},
		{},
		{
			'MODE',
			{
				"AE", 'TOAE', "Redirect standard output to apple events",
				"File", 'FILE', "Redirect standard output to a file",
			}
		}
	}
};

