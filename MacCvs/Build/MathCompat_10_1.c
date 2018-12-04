/*
  The purpose of this file is to work around a problem occuring when building an
  application on Mac OS X 10.2 and deploying it on Mac OS X 10.1:
  <math.h> in 10.2 is different from <math.h> in 10.1 with the result that 
  applications built on 10.2 that use certain math functionality (e.g. isnan(), isinf())
  won't run on 10.1 since in the 10.2 headers these are declared to call external 
  functions which have been introduced in 10.2's version of libSystem.
  The workaround here is to implement the missing functions locally and avoid having
  them imported from the system framework.
  While this certainly could be a potential problem because there are now two different
  implementations, depending which code uses them, this problem should be rather academic
  unless on really relies on floating point functionality.
  However, in the case of MacCvs, the functions are mostly used by MSL's C++ library, which
  mostly needs them in the stream classes to in/output floating point values and for those,
  the usage of a local implementation should be fine...
  
  2/21/2003  Jens Miltner   <jum@mac.com>
*/

#if defined(MAC_OS_X_VERSION_MIN_REQUIRED)  &&  MAC_OS_X_VERSION_MIN_REQUIRED > MAC_OS_X_VERSION_10_0

#ifdef __cplusplus
extern "C" {
#endif

extern long __float_nan[];
extern float  __nan( void );
extern long  __isinfd( double x );
extern long  __fpclassifyd( double x );
extern long  __fpclassifyf( float x );
extern long  __fpclassify( long double x );
#if __TYPE_LONGDOUBLE_IS_DOUBLE
  #ifdef __cplusplus
    inline long __fpclassify( long double x ) { return __fpclassifyd((double)( x )); }
  #else
    #define __fpclassify( x ) (__fpclassifyd((double)( x )))
  #endif
#endif

#ifdef __cplusplus
}
#endif

#define FP_INFINITE  2  /*   + or - infinity      */
#define      fpclassify( x )    ( ( sizeof ( x ) == sizeof(double) ) ?           \
                              __fpclassifyd  ( x ) :                           \
                                ( sizeof ( x ) == sizeof( float) ) ?            \
                              __fpclassifyf ( x ) :                            \
                              __fpclassify  ( x ) )


float  __nan( void )
{
	return (*(float*)      __float_nan);
}

extern long  __isinfd( double x )
{
	return (fpclassify(x) == FP_INFINITE);
}

#endif // MAC_OS_X_VERSION_MIN_REQUIRED > MAC_OS_X_VERSION_10_0
