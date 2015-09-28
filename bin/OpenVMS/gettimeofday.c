/* purloined from source code for Xanim, from Mark Podlipec
   not clear if this was the original of this code

   mangled to kill the timezone pieces */
/*      
 *      Provide the UNIX gettimeofday() function for VMS C.
 *      The timezone is currently unsupported.
 */

#include <time.h>

int sys$gettim();
int lib$subx();
int lib$ediv();

int gettimeofday( struct timeval *tp, int * tzp)
{
   long curr_time[2];   /* Eight byte VAX time variable */
   long jan_01_1970[2] = { 0x4BEB4000,0x7C9567} ;
   long diff[2];
   long result;
   long vax_sec_conv = 10000000;
 
   result = sys$gettim( &curr_time );
 
   result = lib$subx( &curr_time, &jan_01_1970, &diff);
 
   if ( tp != 0) {
       result = lib$ediv( &vax_sec_conv, &diff,
                          &(tp->tv_sec), &(tp->tv_usec) );
       tp->tv_usec = tp->tv_usec / 10;  /* convert 1.e-7 to 1.e-6 */
   }
   return ( 0);
}

