// ===========================================================================
//	CommonCarbonPrefix.h		�1999-2000 Metrowerks Inc. All rights reserved.
// ===========================================================================
//	Common options for a Carbon Target

// ---------------------------------------------------------------------------
//	Carbon Target

#define PP_Target_Carbon		1

#define PP_Target_Classic		(!PP_Target_Carbon)

#define TARGET_API_MAC_CARBON	PP_Target_Carbon
#define TARGET_API_MAC_OS8		PP_Target_Classic


// ---------------------------------------------------------------------------
//	Options
	
#define PP_Uses_PowerPlant_Namespace	0
#define PP_Supports_Pascal_Strings		1
#define PP_StdDialogs_Option			PP_StdDialogs_NavServicesOnly

#define	PP_Uses_Old_Integer_Types		0
#define PP_Obsolete_AllowTargetSwitch	0
#define PP_Obsolete_ThrowExceptionCode	0
#define PP_Warn_Obsolete_Classes		1

#define PP_Suppress_Notes_21			1
