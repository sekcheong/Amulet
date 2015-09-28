/**********************************************************

    Wide.h

	Author:
		Dale Semchishen, 1996

    Description:
        Function Prototypes for the Wide library

		If this library is built without the QuickDraw GX
		includes on a 680x0 then all routines will be linked in.

		If this library is built with QuickDraw GX on a 680x0
		then only WideInit(), WideAssign32(), WideAdd32(),
		WideSub32(), WideToDecStr() and WideBitShift() will
		be linked in. The other Wide routines are supported
		by QuickDraw GX.

		To build this library for a QuickDraw GX environment
		include the file <GXTypes.h> or <GXMath.h> before "Wide.h"

 **********************************************************/

#ifndef __WIDE__
#define __WIDE__

#ifndef __TYPES__
#include <Types.h>
#endif


/* IF generating code for 680x0 CPUs --------------------*/
#if GENERATING68K

#ifndef __SANE__
#include <SANE.h>
#endif

/* IF compiling with MetroWorks, map SANE decimal type because 
   they don't follow the name defined in PowerPC Numerics */
#ifdef __MWERKS__
typedef Decimal decimal;
#endif

/* IF Quickdraw GX return values have not been defined */
#ifndef __GXMATH__
#ifndef __cplusplus
#define fixed Fixed
#endif
#define gxPositiveInfinity ((Fixed) 0x7FFFFFFFL)
#define gxNegativeInfinity ((Fixed) 0x80000000L)
#endif

/* ELSE generating code for PowerPC ---------------------*/
#else

#ifndef __FP__
#include <fp.h>
#endif

/* IF Quickdraw GX return values have not been defined */
#ifndef __GXMATH__
#define gxPositiveInfinity ((long) 0x7FFFFFFFL)
#define gxNegativeInfinity ((long) 0x80000000L)
#endif

#endif


#ifdef __cplusplus
extern "C" {
#endif



/*------------------ Function Prototypes ------------------*/

void WideInit( void );

wide *WideAssign32
(
    wide    *target_ptr,		/* out: 64 bits to be assigned */
    long          value			/* in:  assignment value */
);

wide *WideAdd32
(
    wide    *target_ptr,    	/* out: 64 bits to be added to */
    long     value          	/* in:  addition value */
);

wide *WideSubtract32
(
    wide    *target_ptr,		/* out: 64 bits to be subtracted from */
    long     value    			/* in:  subtraction value */
);

void WideToDecStr
(
        decimal *decstr_ptr,	/* out: decimal output string */
    const wide  *source_ptr		/* in:  64 bit int to convert */
);


/* IF QuickDraw GX is not included */
#ifndef __GXMATH__

short WideScale
(
    const wide  *bigint_ptr		/* in: 64 bits */
);

#endif


/* IF generating code for 680x0 CPUs */
#if GENERATING68K

wide *WideBitShift
(
    wide    *target_ptr,		/* in/out: 64 bits to be shifted */
    short       amount			/* in:     shift amount (+ right, - left) */
);


/* IF QuickDraw GX is not included */
#ifndef __GXMATH__

wide *WideAdd
(
          wide  *target_ptr,	/* out: 64 bits to be added to */
    const wide  *source_ptr		/* in:  addition value */
);

short WideCompare
(
    const wide	*target_ptr,	/* in: first value */
    const wide	*source_ptr		/* in: second value */
);

long WideDivide
(
	const wide *dividend_ptr,	/* in:  64 bits to be divided */
	long		divisor,		/* in:  value to divide by */
	long	   *remainder_ptr	/* out: the remainder of the division */
);

wide *WideWideDivide
(
	wide	*dividend_ptr,		/* in/out:  64 bits to be divided */
	long	divisor,			/* in:  	value to divide by */
	long   *remainder_ptr		/* out: 	the remainder of the division */
);

wide *WideMultiply
(
    long	multiplicand,		/* in:  first value to multiply */
    long	multiplier,			/* in:  second value to multiply */
    wide   *target_ptr			/* out: 64 bits to be assigned */
);

wide *WideNegate
(
    wide	*target_ptr			/* in/out: 64 bit integer to be negated */
);

wide *WideShift
(
    wide    *target_ptr,    	/* in/out: 64 bits to be shifted */
    short       amount      	/* in:     shift amount (+ right, - left) */
);

unsigned long WideSquareRoot
(
    const wide  *source_ptr		/* in:  value to take the square root of */
);

wide *WideSubtract
(
          wide  *target_ptr,	/* out: 64 bit int to be subtracted from */
    const wide  *bigint2_ptr	/* in:  subtraction value */
);

#endif

#endif

#ifdef __cplusplus
}
#endif

#endif