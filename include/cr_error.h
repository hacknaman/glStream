#ifndef CR_ERROR_H
#define CR_ERROR_H

#ifndef __GNUC__
#define NORETURN_PRINTF
#define PRINTF
#else
#define NORETURN_PRINTF __attribute__ ((__noreturn__,format(printf,1,2)))
#define PRINTF __attribute__ ((format(printf,1,2)))
#endif

void crDebug( char *format, ... ) PRINTF;
void crWarning( char *format, ... ) PRINTF;

void crError( char *format, ... ) NORETURN_PRINTF;

#ifndef NDEBUG
#define CRASSERT( PRED ) ((PRED)?(void)0:crError( "Assertion failed: %s, file %s, line %d", #PRED, __FILE__, __LINE__))
#else
#define CRASSERT( PRED ) ((void)0)
#endif

#endif /* CR_ERROR_H */
