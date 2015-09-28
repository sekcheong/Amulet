/**********************************************************

    Wide.c

	Author:
		Dale Semchishen, 1996

    Description:
        A 64 bit integer library for the 680x0 and PowerPC processors

		While this library is self-initializing you can choose
		to call WideInit() during application startup so that
		the first time you call a math routine it does not incur
		the extra time required for initialization.

		Most of the 64 bit routines in this library are available in
		QuickDraw GX or on the PowerPC. On the 680x0 if you include
		<GXTypes.h> or <GXMath.h> before "Wide.h" and compile this library,
		the QuickDraw GX traps will be used instead of the routines
		marked with "(gx)".


    Global functions:
        WideInit()			- Initialize the Wide library (optional)

        WideAdd() 		 	- (gx) Add two 64 bit ints
        WideAdd32()			-      Add 32 bits to a 64 bit int
	    WideAssign32()		-      Assign 32 bits to 64 bit int
        WideBitShift()		-      Shift a 64 bit number
        WideCompare()		- (gx) Compare two 64 bits ints
        WideDivide()		- (gx) Divide 32 bit int into 64 bit int with a 32 bit result
        WideMultiply()		- (gx) Multiply two 32 bits ints for a 64 bit result
        WideNegate()		- (gx) Negative value of a 64 bit int
        WideScale()			- (gx) Highest order nonzero bit in a 64 bit number
        WideShift()			- (gx) Shift a 64 bit number and round up
        WideSquareRoot()	- (gx) return 32 bit square root of an unsigned 64 bit number
        WideSubtract32()	-      Subtract 32 bits from a 64 bit int
        WideSubtract()		- (gx) Subtract a 64 bits int from a 64 bit int
        WideToDecStr()		-      Convert 64 bit int to a SANE 'decimal' string
        WideWideDivide()	- (gx) Divide 32 bit int into 64 bit int with a 64 bit result

    Note:
 		This library has been compiled under the following
 		development systems:
 			- Symantec C 68K 7.0.4
 			- CodeWarrior 6 Lite (68K and PPC)
 
 **********************************************************/


#include "Wide.h"

#include <ToolUtils.h>
#include <Gestalt.h>


/*------------------- Local Constants --------------------*/

#define LONG_SIGN_BIT 0x80000000


/*------------------- Local Variables --------------------*/

/* Is the Wide math library initialized ? */
static short gWide_Initialized = false;

/* Are the MC680x0 64 bit multiply and divide instructions available ? */
static short gWide_64instr = false;


/*---------------- Compiler Dependancies -----------------*/

/* IF generating code for 680x0 CPUs */
#if GENERATING68K

/* IF MetroWorks compiler */
#ifdef __MWERKS__

typedef extended80   Extended_80;
typedef DecForm      decform;
#define FIXEDDECIMAL FixedDecimal

#define ASM_FUNC_HEAD	asm
#define ASM_BEGIN		LINK A6,#0
#define ASM_END			UNLK A6
#define ASM_FUNC_TAIL 	RTS

#define WIDE_HI 0
#define WIDE_LO 4

/* ELSE IF Symantec C */
#elif THINK_C

typedef extended	Extended_80;

#define ASM_FUNC_HEAD
#define ASM_BEGIN		asm{
#define ASM_END			}
#define ASM_FUNC_TAIL

#define WIDE_LO wide.lo
#define WIDE_HI wide.hi

#else
#error "Wide library has not been ported to this 68K environment"
#endif

/* ELSE generating code for PowerPC */
#else

/* IF the MetroWorks compiler */
#ifdef __MWERKS__

typedef struct _extended80  Extended_80;

#else
#error "Wide library has not been ported to this PowerPC environment"
#endif

#endif



/**********************************************************

    WideInit - Initialize the Wide math library

    Description:
        This routine will set the internal flags used by the
        Wide math library. These flags tested by the multiply
        and divide functions in order to determine if there
        are 680x0 64 bit math instructions that can be used
        instead of a software algorithm.

		While this library is self-initializing you can choose
		to call WideInit() during application startup so that
		the first time you call a math routine it does not incur
		the extra time required for initialization.

    Return value:
        none

**********************************************************/
void WideInit( void )
{
	long   processor_type;
	

	/* IF able to determine processor type */
	if( Gestalt( gestaltProcessorType, &processor_type ) == noErr )
	{
		processor_type &= 0xFFFF;
	}
	/* ELSE assume we have the oldest CPU */
	else
	{
		processor_type = gestalt68000;
	}

	/* 64 bit mult and div instructions available if CPU is 68020 to 68040 */
	gWide_64instr = (processor_type >= gestalt68020) &&
					(processor_type <= gestalt68040);

    gWide_Initialized = true;
}



/**********************************************************

    WideAssign32 - Assign 32 bits to 64 bit int

    Description:
        This routine will assign a signed 32 bit integer to
        a signed 64 bit integer

    Return value:
        the target pointer passed into this function

**********************************************************/
wide *WideAssign32
(
    wide    *target_ptr,    /* out: 64 bits to be assigned */
    long     value    		/* in:  assignment value */
)
{
	/* initialize Wide library if not already done */
	if( !gWide_Initialized ) WideInit();

    target_ptr->lo = value;
    target_ptr->hi = (value >= 0) ? 0: -1;

	return( target_ptr );
}



/**********************************************************

    WideAdd32 - Add 32 bits to a 64 bit int

    Description:
        This routine will add a signed 32 bit integer to
        a signed 64 bit integer

    Return value:
        the target pointer passed into this function

**********************************************************/
wide *WideAdd32
(
    wide    *target_ptr,    /* out: 64 bits to be added to */
    long     value          /* in:  addition value */
)
{
	wide	temp;


	/* note: library initialization check done by WideAdd() */

	/* convert value to 64 bits */
    temp.lo = value;
    temp.hi = (value >= 0) ? 0: -1;

	/* do the add */
	return( WideAdd( target_ptr, &temp ) );
}



/**********************************************************

    WideSubtract32 - Subtract 32 bits from a 64 bit int

    Description:
        This routine will subtract a signed 32 bit integer from
        a signed 64 bit integer

    Return value:
        the target pointer passed into this function

**********************************************************/
wide *WideSubtract32
(
    wide    *target_ptr,    /* out: 64 bits to be subtracted from */
    long     value  	    /* in:  subtraction value */
)
{
	wide	temp;


	/* note: library initialization check done by WideSubtract() */

	/* convert value to 64 bits */
    temp.lo = value;
    temp.hi = (value >= 0) ? 0: -1;

	/* do the subtract */
	return( WideSubtract( target_ptr, &temp ) );
}



/* IF QuickDraw GX is not included */
#ifndef __GXMATH__

/**********************************************************

    WideScale - (gx) Highest order nonzero bit in a 64 bit number

    Description:
		This routine will return the bit number of the
		highest-order nonzero bit in a 64 bit number.

    Return value:
        0 to 63 (bit position of highest-order nonzero bit)
        or
        -1 if all bits are zero

**********************************************************/
short WideScale
(
    const wide  *bigint_ptr     /* in: 64 bits to test */
)
{
register short		   rv;
register long		   accum_hi;
register unsigned long accum_lo;


	/* initialize Wide library if not already done */
	if( !gWide_Initialized ) WideInit();

	rv = 63;
	accum_hi = bigint_ptr->hi;
	accum_lo = bigint_ptr->lo;

	/* WHILE we have not found the left-most 1 bit */
	while( ((accum_hi & LONG_SIGN_BIT) == 0) && (rv >= 0) )
	{
		--rv;

		/* shift 64 bits left once */
		accum_hi <<= 1;
		if( accum_lo & LONG_SIGN_BIT )
		{
			accum_hi |= 1;
		}
		accum_lo <<= 1;
	}

	return( rv );
}
#endif



/**********************************************************

    Wide_ToExtended - internal 68K routine
    Wide_ToDouble   - internal PPC routine

    Description:
		An internal routine that converts a signed
		'wide' value to a signed 'extended' or Double value.

    Return value:
		none

**********************************************************/

/* IF generating code for 680x0 CPUs */
#if GENERATING68K
static void Wide_ToExtended
(
	Extended_80 *target_ptr,    /* out: extended number */
    const  wide *source_ptr     /* in:  64 bits to convert */
)
{
	short	sign_bit;
	short	left_most;
	wide	work_int = *source_ptr;
	struct _extended80 *outp = (struct _extended80 *) target_ptr;


	/* IF negative */
	sign_bit = (work_int.hi & LONG_SIGN_BIT) >> 16;
	if( sign_bit )
    {
		/* convert number to positive */
		WideNegate( &work_int );
	}

	/* determine left-most 1 bit */
	left_most = WideScale( &work_int );

	/* IF there are no 1 bits */
	if( left_most < 0 )
	{
		/* the answer is zero */
        outp->exp = 0;
        outp->man[0] = 0;
        outp->man[1] = 0;
        outp->man[2] = 0;
        outp->man[3] = 0;
	}
	/* ELSE a non-zero number */
	else
	{
        /* left justify the bits */
        WideBitShift( &work_int, -(63 - left_most) );

		/* output the 'extended' number */
        outp->exp = sign_bit | (0x3FFF + left_most);
        (*(long *) &outp->man[0]) = work_int.hi;
        (*(long *) &outp->man[2]) = work_int.lo;
	}
}

/* ELSE generating code for PowerPC */
#else
static void Wide_ToDouble
(
	double_t	*target_ptr,    /* out: double number */
    const  wide *source_ptr     /* in:  64 bits to convert */
)
{
	long			left_most;
	unsigned long	sign_bit;
	wide	work_int = *source_ptr;
	union
	{
		double_t		dbl_num;
		unsigned long	half[2];
	} work;


	/* IF negative */
	sign_bit = work_int.hi & LONG_SIGN_BIT;
	if( sign_bit )
    {
		/* convert number to positive */
		WideNegate( &work_int );
	}

	/* determine left-most 1 bit */
	left_most = WideScale( &work_int );

	/* IF there are no 1 bits */
	if( left_most < 0 )
	{
		/* build the double number */
		work.half[0] = 0;
		work.half[1] = 0;
	}
	/* ELSE a non-zero number */
	else
	{
        /* left justify the bits and toss the msb (so we normalize the fraction) */
        WideBitShift( &work_int, -(64 - left_most) );

		/* build the double number */
		work.half[0] = sign_bit | ((0x3FF + left_most) << 20);
		work.half[0] |= (work_int.hi >> 12) & 0x000FFFFF;
		work.half[1] = (work_int.hi << 20) | ((work_int.lo >> 12) & 0x000FFFFF);
	}

	/* output the result */
	*target_ptr = work.dbl_num;
}
#endif



/**********************************************************

    WideToDecStr - Convert 64 bit int to a SANE 'decimal' string

    Description:
        This routine will convert a signed 64 bit integer
        to the 'decimal' type string supported by the SANE library.

    Return value:
        none

**********************************************************/
void WideToDecStr
(
          decimal   *decstr_ptr,    /* out: 'decimal' output string */
    const wide		*source_ptr     /* in:  64 bits to convert */
)
/* IF generating code for 680x0 CPUs */
#if GENERATING68K
{
    decform     	format;
    Extended_80 	ext_number;


	/* initialize Wide library if not already done */
	if( !gWide_Initialized ) WideInit();

	/* convert 'wide' number to 'extended' format */
	Wide_ToExtended( &ext_number, source_ptr );

	/* define how num2dec() will generate the string */
    format.style = FIXEDDECIMAL;
    format.digits = 0;

	/* pre-initialize to zero because for Symantec SANE (possibly   */
	/* others) decimal.exp will be uninitialized if ext_number is 0 */
    decstr_ptr->exp = 0;

	/* IF 68K MetroWorks compiler, the second parameter of num2dec()
	   is a pointer instead of a value as defined in PowerPC Numerics */
#ifdef __MWERKS__
    /* generate the 'decimal' string */
	Num2Dec( &format, &ext_number, decstr_ptr );
#else
    /* generate the 'decimal' string */
    num2dec( &format, ext_number, decstr_ptr );
#endif
}
/* ELSE generating code for PowerPC */
#else
{
    decform     format;
    double_t	dbl_number;


	/* initialize Wide library if not already done */
	if( !gWide_Initialized ) WideInit();

	/* convert 'wide' number to 'double' format */
	Wide_ToDouble( &dbl_number, source_ptr );

    format.style = FIXEDDECIMAL;
    format.digits = 0;

    /* generate the 'decimal' string */
    num2dec( &format, dbl_number, decstr_ptr );
}
#endif



/* IF generating code for 680x0 CPUs */
#if GENERATING68K

/**********************************************************

    WideBitShift - Shift a 64 bit number

    Description:
        This routine will perform an arithmetic shift on a
        64 bit number.

        If the shift amount is 0, then the resulting value
        will be the same as the input value.

		When the shift amount is positive, the 64 bit number
		will be shifted to the right (decreasing magnitude). 

		When the shift amount is negative, the 64 bit number
		will be shifted to the left (decreasing increasing). 

    Return value:
        the target pointer passed into this function

**********************************************************/
wide *WideBitShift
(
    wide    *target_ptr,    /* in/out: 64 bits to be shifted */
    short       amount      /* in:     shift amount (+ right, - left) */
)
{
register long hi;
register unsigned long lo;
register short shift_amount;


	/* initialize Wide library if not already done */
	if( !gWide_Initialized ) WideInit();

	hi = target_ptr->hi;
	lo = target_ptr->lo;
	shift_amount = amount;

	/* IF shifting right more than 31 bits */
	if( shift_amount > 31 )
	{
		lo = hi >> (shift_amount - 32);
		hi = 0;
	}
	/* ELSE IF shifting right 1-31 bits */
	else if( shift_amount > 0 )
	{
		lo = (lo >> shift_amount) | (hi << (32 - shift_amount));
		hi >>= shift_amount;
	}
	/* ELSE IF shifting left more than 31 bits */
	else if( shift_amount < -31 )
	{
		hi = lo << -(shift_amount + 32);
		lo = 0;
	}
	/* ELSE IF shifting left 1-31 bits */
	else if( shift_amount < 0 )
	{
		hi = (hi << -shift_amount) | (lo >> (32 + shift_amount));
		lo <<= -shift_amount;
	}

	target_ptr->hi = hi;
	target_ptr->lo = lo;

	return( target_ptr );
}



/* IF QuickDraw GX is not included */
#ifndef __GXMATH__


/**********************************************************

    WideAdd - (gx) Add two 64 bit ints

    Description:
        This routine will add a signed 64 bit integer to
        a signed 64 bit integer.

		The source integer will be added to the target integer.

    Return value:
        the target pointer passed into this function

**********************************************************/
wide *WideAdd
(
          wide  *target_ptr,	/* out: 64 bits to be added to */
    const wide  *source_ptr		/* in:  addition value */
)
{
register unsigned long accum_lo;
register long		   accum_hi;
register wide *destp;


	/* initialize Wide library if not already done */
	if( !gWide_Initialized ) WideInit();

	/* perform a partial add */
	destp = target_ptr;
	accum_lo = destp->lo + source_ptr->lo;
	accum_hi = destp->hi + source_ptr->hi;

	/* IF the low long has overflowed */
	if( (accum_lo < destp->lo) ||
		(accum_lo < source_ptr->lo) )
	{
		/* propagate the carry */
		accum_hi += 1;
	}

	destp->hi = accum_hi;
	destp->lo = accum_lo;

	return( destp );
}




/**********************************************************

    WideCompare - (gx) Compare two 64 bits ints

    Description:
        This routine will compare a signed 64 bit integer
        with a signed 64 bit integer.

    Return value:
        1  if target number is greater than the source number
        0  if the two numbers are equal
       -1  if target is less than the source number
        
**********************************************************/
short WideCompare
(
    const wide	*target_ptr,    /* in: target number */
    const wide	*source_ptr     /* in: source number */
)
{
	wide  work;


	/* note: library initialization check done by WideSubtract() */

	work = *target_ptr;
	WideSubtract( &work, source_ptr );

	if( (work.hi == 0) && (work.lo == 0) )
	{
		return( 0 );
	}

	if( work.hi < 0 )
	{
		return( -1 );
	}

	return( 1 );
}



/**********************************************************

    Wide_DivideU - internal routine

    Description:
		A 680x0 assembly language routine that performs an
		performs an unsigned 64 bit division.

		The algorithm is a binary version of the paper and
		pencil method of division you learned in school.
		It will loop once for each bit in the sizeof the
		divisor (which is 32).

    Return value:
		none

**********************************************************/
ASM_FUNC_HEAD static void Wide_DivideU
(
	wide	*dividend_ptr,		/* in/out:  64 bits to be divided */
	long	 divisor,			/* in:      value to divide by */
	long	*remainder_ptr		/* out:     the remainder of the division */
)
{
#define DIVIDEND_PTR   8
#define DIVISOR       12
#define REMAINDER_PTR 16

ASM_BEGIN
		MOVEM.L D2-D7,-(SP)			// save work registers
		CLR.L	D0					//
		CLR.L	D1					// D0-D1 is the quotient accumulator
		MOVE.L	DIVIDEND_PTR(A6),A0	//
		MOVE.L	WIDE_HI(A0),D2      //
		MOVE.L	WIDE_LO(A0),D3      // D2-D3 is the remainder accumulator
		CLR.L	D4					//
		MOVE.L	D2,D5				// D5 = copy of dividend.hi
		MOVE.L	DIVISOR(A6),D6		// D6 = copy of divisor

		MOVEQ.L #31,D7				// FOR number of bits in divisor (see @div99:)
@divloop:
        LSL.L   #1,D0               // shift quotient.hi accumulator left once
        LSL.L   #1,D1               // shift quotient.lo accumulator left once
		LSL.L	#1,D4				//
        LSL.L   #1,D3               // shift remainder accumulator left once
        BCC		@div29				// IF CC, a zero bit shifted out
        LSL.L   #1,D2               //
		BSET	#0,D2				//
		BRA		@div30
@div29:
        LSL.L   #1,D2               //
@div30:
		SUB.L	D6,D2				// remainder -= divisor
		BCS		@div50				// IF CS, remainder is negative
		BSET	#0,D1				// quotient.lo |= 1
		BRA.S	@div77				//
@div50:
		ADD.L	D6,D2				// remainder += divisor
@div77:
		BTST	D7,D5				//
		BEQ		@div90				// IF EQ, bit not set in dividend.hi
		BSET	#0,D4				//
@div90:
		CMP.L	D6,D4				//
		BCS		@div99				// IF CS, divisor < D4
		SUB.L	D6,D4				// D4 -= divisor
		BSET	#0,D0				// quotient.hi |= 1
@div99:
		DBF		D7,@divloop			// loop until D7 == -1

		MOVE.L	DIVIDEND_PTR(A6),A0 // output the remainder
		MOVE.L	D0,WIDE_HI(A0)      //
		MOVE.L	D1,WIDE_LO(A0)      //
		MOVE.L	REMAINDER_PTR(A6),A0// output the remainder
		MOVE.L	D2,(A0)             //
		MOVEM.L (SP)+,D2-D7			// restore work registers
ASM_END
ASM_FUNC_TAIL
}



/**********************************************************

    Wide_DivideS - internal routine

    Description:
		An internal routine that will divide a signed 32 bit
		number into a signed 64 bit number and produce a
		signed 64 bit result.

    Return value:
		none

**********************************************************/
static void Wide_DivideS
(
	wide	*dividend_ptr,		/* in/out:  64 bits to be divided */
	long	 divisor,			/* in:      value to divide by */
	long	*remainder_ptr		/* out:     the remainder of the division */
)
{
	wide	dividend;
	long	remainder;
	long	negative_result;


	dividend = *dividend_ptr;
	
	/* determine if result of the divide will be positive or negative */
	negative_result = (dividend_ptr->hi & LONG_SIGN_BIT) ^ (divisor & LONG_SIGN_BIT);

	/* IF dividend is negative */
	if( dividend_ptr->hi < 0 )
	{
		/* divide algorithm is unsigned */
		WideNegate( &dividend );;
	}

	/* IF divisor is negative */
	if( divisor < 0 )
	{
		/* divide algorithm is unsigned */
		divisor = -divisor;
	}

	/* perform an unsigned division */
	Wide_DivideU( &dividend, divisor, &remainder );

	/* IF negative dividend */
	if( dividend_ptr->hi < 0 )
	{
		*remainder_ptr = -remainder;
	}
	else
	{
		*remainder_ptr = remainder;
	}

	/* IF negative quotient */
	if( negative_result )
	{
		/* correct the sign */
		WideNegate( &dividend );
	}

	*dividend_ptr = dividend;
}




/**********************************************************

    WideWideDivide - (gx) Divide 32 bit int into 64 bit int with a 64 bit result

    Description:
        This routine will divide a signed 32 bit integer into
        a signed 64 bit integer and return a signed 64 bit quotient
        and a 32 bit remainder.

		When this function returns the dividend will be replaced
		by the quotient

		If the divisor is zero, then the quotient will be set to the
		largest positive or negative number, as appropriate.
		And the remainder will be gxNegativeInfinity.

		If the 'remainder_ptr' parameter is NULL or (long *)-1
		then no remainder will be returned.

    Return value:
        pointer to the quotient

**********************************************************/
wide *WideWideDivide
(
	wide	*dividend_ptr,		/* in/out:  64 bits to be divided */
	long	 divisor,			/* in:  	value to divide by */
	long	*remainder_ptr		/* out: 	the remainder of the division */
)
{
	long	remainder;


	/* initialize Wide library if not already done */
	if( !gWide_Initialized ) WideInit();

	/* IF dividing by zero */
	if( divisor == 0 )
	{
		remainder = gxNegativeInfinity;

		/* IF dividend is negative */
		if( (dividend_ptr->hi < 0) )
		{
			dividend_ptr->hi = gxNegativeInfinity;
			dividend_ptr->lo = 0;
		}
		/* ELSE dividend is positive */
		else
		{
			dividend_ptr->hi = gxPositiveInfinity;
			dividend_ptr->lo = ~0;
		}
	}
	/* ELSE overflow is not possible */
	else
	{
		/* do the divide */
		Wide_DivideS( dividend_ptr, divisor, &remainder );
	}


	/* IF the user wants a remainder */
	if( (remainder_ptr != NULL) &&
		(remainder_ptr != (long *) -1) )
	{
		*remainder_ptr = remainder;
	}

	return( dividend_ptr );
}



/**********************************************************

    Wide_DivS64 - internal routine

    Description:
		A 680x0 assembly language routine that performs a
		signed 64 bit division by using the DIVS.L instruction.

		The 64 bit DIVS.L instruction is only available on the
		68020 to 68040 processors.

    Return value:
    	32 bit quotient

**********************************************************/
ASM_FUNC_HEAD static long Wide_DivS64
(
	const wide	*dividend_ptr,		/* in/out:  64 bits to be divided */
	long	     divisor,			/* in:      value to divide by */
	long	    *remainder_ptr,		/* out:     the remainder of the division */
	short	    *overflow_ptr		/* out:     flag indicating if overflow occured */
)
{
#define DIVIDEND_PTR   8
#define DIVISOR       12
#define REMAINDER_PTR 16
#define OVERFLOW_PTR  20

ASM_BEGIN
        MOVE.L  D2,-(SP)
        MOVEQ.L	#1,D2					// assume there is overflow
        MOVE.L  DIVIDEND_PTR(A6),A0     // A0 -> the dividend
        MOVE.L	WIDE_LO(A0),D0			//
        MOVE.L	WIDE_HI(A0),D1			// D1-D0 = 64 bit dividend
        DC.W    0x4C6E,0x0C01,0x000C    // DIVS.L divisor(A6),D1-D0  (64bit signed divide)
		BVS		@divcont				// IF VS, we're overflowed
		CLR.W	D2						// ELSE, clear the overflow flag
@divcont:
		MOVE.L	REMAINDER_PTR(A6),A0	// output remainder
		MOVE.L	D1,(A0)					//
		MOVE.L	OVERFLOW_PTR(A6),A0		// output overflow flag
		MOVE.W	D2,(A0)					//
        MOVE.L  (SP)+,D2
ASM_END
ASM_FUNC_TAIL
}



/**********************************************************

    WideDivide - (gx) Divide 32 bit int into 64 bit int with a 32 bit result

    Description:
        This routine will divide a signed 32 bit integer into
        a signed 64 bit integer and return a signed 32 bit quotient
        and a 32 bit remainder.

		Since the return value is a 32 bit integer, overflow is possible.
		If a positive overflow occurs, the return value will be gxPositiveInfinity
		If a negative overflow occurs, the return value will be gxNegativeInfinity
		In both cases *remainder_ptr will be set to gxNegativeInfinity.

		If a NULL pointer is passed for the 'remainder_ptr' parameter
		then no remainder will be returned.

		If the pointer passed to 'remainder_ptr' is (long *)-1
		then no remainder will be returned but in the case of an
		overflow gxNegativeInfinity will be the return value.

    Return value:
		quotient (the integer result of the division)
		or
		gxNegativeInfinity if negative overflow
		or
		gxPositiveInfinity if positive overflow

**********************************************************/
long WideDivide
(
	const wide *dividend_ptr,	/* in:  64 bits to be divided */
	long		divisor,		/* in:  value to divide by */
	long	   *remainder_ptr	/* out: the remainder of the division */
)
{
	long	rv;
	long	remainder;
	wide	work;
	short	overflow;


	/* initialize Wide library if not already done */
	if( !gWide_Initialized ) WideInit();

	/* IF not dividing by zero */
	if( divisor != 0 )
	{
	    /* IF the 68k 64bit divide instruction is not available */
	    if( !gWide_64instr )
	    {
	        /* use a software subroutine for the division */
	        work = *dividend_ptr;
			Wide_DivideS( &work, divisor, &remainder );
			rv = work.lo;

			/* determine if result overflowed a long */
			overflow = (work.hi != 0) && (work.hi != -1);
	    }
	    /* ELSE the 64bit division instruction is available */
		else
		{
			/* use an assembly language instruction to do the division */
			rv = Wide_DivS64( dividend_ptr, divisor, &remainder, &overflow );
		}


		/* IF the user wants a remainder */
		if( (remainder_ptr != NULL) &&
			(remainder_ptr != (long *) -1) )
		{
			*remainder_ptr = remainder;
		}
	}


	/* IF there was an overflow */
	if( overflow )
	{
		/* IF user wants a single overflow indication in the return value */
		if( remainder_ptr == (long *) -1 )
		{
			rv = gxNegativeInfinity;
		}
		else
		{
			/* IF there is a remainder pointer */
			if( remainder_ptr != NULL )
			{
				*remainder_ptr = gxNegativeInfinity;
			}

			/* set overflow return value based on sign of the result */
			if( (dividend_ptr->hi & LONG_SIGN_BIT) ^ (divisor & LONG_SIGN_BIT) )
			{
				rv = gxNegativeInfinity;
			}
			else
			{
				rv = gxPositiveInfinity;
			}
		}
	}

	return( rv );
}



/**********************************************************

    Wide_MulS64 - internal routine

    Description:
		A 680x0 assembly language routine that performs a
		signed 64 bit division by using the MULS.L instruction.

		The 64 bit MULS.L instruction is only available on the
		68020 to 68040 processors.

    Return value:
    	none

**********************************************************/
ASM_FUNC_HEAD static void Wide_MulS64
(
    long	multiplicand,		/* in:  first value to multiply */
    long	multiplier,			/* in:  second value to multiply */
    wide   *out_ptr				/* out: 64 bits to be assigned */
)
{
#define MULTIPLICAND   8
#define MULTIPLIER    12
#define OUT_PTR       16

ASM_BEGIN
        MOVE.L  MULTIPLICAND(A6),D0     //
        DC.W    0x4C2E,0x0C01,0x000C    // MULS.L multiplier(A6),D1-D0; 64bit signed multiply
        MOVE.L  OUT_PTR(A6),A0          //
        MOVE.L  D0,WIDE_LO(A0)  		// WIDE_LO defined earlier
        MOVE.L  D1,WIDE_HI(A0)  		// WIDE_HI defined earlier
ASM_END
ASM_FUNC_TAIL
}



/**********************************************************

    WideMultiply - (gx) Multiply two 32 bits ints for a 64 bit result

    Description:
        This routine will multiply two signed 32 bit values
        and assign the result to a signed 64 bit integer

    Return value:
        the target pointer passed into this function

**********************************************************/
wide *WideMultiply
(
    long	multiplicand,		/* in:  first value to multiply */
    long	multiplier,			/* in:  second value to multiply */
    wide   *target_ptr			/* out: 64 bits to be assigned */
)
{
	/* initialize Wide library if not already done */
	if( !gWide_Initialized ) WideInit();

    /* IF the 64bit multiply instruction is available */
    if( gWide_64instr )
    {
		/* execute the assembly language instruction MULS.L */
		Wide_MulS64( multiplicand, multiplier, target_ptr );
    }
	else
	{
        /* call toolbox to perform the multiply */
        LongMul( multiplicand, multiplier, (Int64Bit *) target_ptr );
	}

	return( target_ptr );
}




/**********************************************************

    WideNegate - (gx) Negative value of a 64 bit int

    Description:
        This routine will calculate the negative value of
        a signed 64 bit integer

    Return value:
        the target pointer passed into this function

**********************************************************/
wide *WideNegate
(
    wide    *target_ptr     /* in/out: 64 bits to be negated */
)
{
register long		   accum_hi;
register unsigned long accum_lo;
register wide *destp;


	/* initialize Wide library if not already done */
	if( !gWide_Initialized ) WideInit();

	destp = target_ptr;
	accum_lo = destp->lo;
	accum_hi = destp->hi;

	/* 1's complement */
	accum_lo = ~accum_lo;
	accum_hi = ~accum_hi;

	/* 2's complement */
	accum_lo += 1;
	if( accum_lo == 0 )
	{
		/* propagate the carry */
		accum_hi += 1;
	}

	destp->hi = accum_hi;
	destp->lo = accum_lo;

	return( destp );
}




/**********************************************************

    WideShift() - (gx) Shift a 64 bit number and round up

    Description:
        This routine will perform an arithmetic shift on a
        64 bit number with rounding.

		The shift is be similar to the WideBitShift() routine
		except that the result is rounded by adding half and
		truncating the remainder.

		(eg) Performing a shift of +3 on a value of 0x0C will
			 return 2 with WideShift() and 1 with WideBitShift()

    Return value:
        the target pointer passed into this function

**********************************************************/
wide *WideShift
(
    wide    *target_ptr,    /* in/out: 64 bits to be shifted */
    short       amount      /* in:     shift amount (+ right, - left) */
)
{
	long	shifted_out;


	/* initialize Wide library if not already done */
	if( !gWide_Initialized ) WideInit();

	/* IF shifting to the left */
	if( amount <= 0 )
	{
		/* perform an arithmetic shift */
		WideBitShift( target_ptr, amount );
	}
	/* ELSE shifting to the right */
	else
	{
		/* IF bit shifted out is in .lo */
		if( amount <= 32 )
		{
			shifted_out = target_ptr->lo & (1L << (amount-1));
		}
		/* ELSE IF bit shifted out is in .hi */
		else if( amount <= 64 )
		{
			shifted_out = target_ptr->hi & (1L << (amount-33));
		}
		else
		{
			shifted_out = 0;
		}
	
		/* perform an arithmetic shift */
		WideBitShift( target_ptr, amount );
	
		/* IF there was a bit shifted out */
		if( shifted_out != 0 )
		{
			/* add the bit that was shifted out */
			WideAdd32( target_ptr, 1 );
		}
	}

	return( target_ptr );
}



/**********************************************************

    Wide_FromExtended - internal routine

    Description:
		An internal routine that converts an unsigned 'extended'
		value to an unsigned 'wide' value.

    Return value:
		none

**********************************************************/
static void Wide_FromExtended
(
        wide		*target_ptr,    /* out: unsigned 64 bits */
  const Extended_80	*source_ptr     /* in:  unsigned extended number to convert */
)
{
register short	       shift;
register unsigned long hi;
register unsigned long lo;
const struct _extended80 *inp = (struct _extended80 *) source_ptr;


	/* IF bits should be right justified */
	shift =  63 - (inp->exp - 0x3FFF);
	if( (shift > 0) && (shift < 64) )
	{
		/* get copy of 'extended' number */
		hi = (*(long *) &inp->man[0]);
		lo = (*(long *) &inp->man[2]);

		/* perform a logical shift to the right */
		while( shift-- > 0 )
		{
			lo >>= 1;
			if( hi & 1 )
				lo |= LONG_SIGN_BIT;
			hi >>= 1;
		}
	}
	else
	{
		hi = 0;
		lo = 0;
	}

	/* output the result */
	target_ptr->hi = hi;
	target_ptr->lo = lo;
}



/**********************************************************

    WideSquareRoot - (gx) return 32 bit square root of an unsigned 64 bit number

    Description:
        This routine will calculate the unsigned 32 bit
        square root of an unsigned 64 bit number.

		Since the source number must be unsigned it's range
		can be from 0 to 2^64 - 1

		Overflow of the return value is not possible.

    Return value:
        32 bit unsigned square root

**********************************************************/
unsigned long WideSquareRoot
(
    const wide  *source_ptr		/* in: unsigned value to take the square root of */
)
{
	wide		 work_int;
    Extended_80  ext_number;


	/* initialize Wide library if not already done */
	if( !gWide_Initialized ) WideInit();

	/* convert 'wide' number to 'extended' format */
	Wide_ToExtended( &ext_number, source_ptr );

	/* calculate the square root of a positive number using SANE */

	/* IF compiling with MetroWorks, the parameter of sqrt() is a
	   pointer instead of a value as defined in PowerPC Numerics */
#ifdef __MWERKS__
	Sqrt( &ext_number );
#else
	ext_number = sqrt( ext_number );
#endif

	/* convert 'extended' format to 'wide' number */
	Wide_FromExtended( &work_int, &ext_number );

	/* ok to ignore work_int.hi because its always zero */
	return( work_int.lo );
}




/**********************************************************

    WideSubtract - (gx) Subtract a 64 bits int from a 64 bit int

    Description:
        This routine will subtract a signed 64 bit integer
        from a signed 64 bit integer

    Return value:
        the target pointer passed into this function

**********************************************************/
wide *WideSubtract
(
          wide  *target_ptr,    /* out: 64 bits to be subtracted from */
    const wide  *source_ptr		/* in:  subtraction value */
)
{
register unsigned long accum_lo;
register long		   accum_hi;
register wide *destp;


	/* initialize Wide library if not already done */
	if( !gWide_Initialized ) WideInit();

	/* perform a partial subtract */
	destp = target_ptr;
	accum_lo = destp->lo - source_ptr->lo;
	accum_hi = destp->hi - source_ptr->hi;

	/* IF the low long has overflowed */
	if( destp->lo < source_ptr->lo )
	{
		/* propagate the borrow */
		accum_hi -= 1;
	}

	destp->hi = accum_hi;
	destp->lo = accum_lo;

	return( destp );
}

#endif

#endif