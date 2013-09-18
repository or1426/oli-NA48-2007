#ifndef __userh__
#define __userh__

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cmpio.h>
#include <reader.h>
#include <float.h>

#define _POWER2(x) ((x)*(x))

#define ENABLE_OUTPUT 1
#define MC_ENABLE_OUTPUT 1

extern FILE *fprt;
extern char gString[50];
extern char * dir;

#if ENABLE_OUTPUT
extern FILE * realFP;
#endif

#if MC_ENABLE_OUTPUT
extern FILE * ke3FP;
extern FILE * km3FP;
extern FILE * k2piFP;
#endif

float f3vmag2(float *v);
float f3vmag(float *v);
float f3vdot(float * v1, float * v2);

float f4vdot(float * v1, float * f2);

int crossProd(float * v1, float * v2, float * vOut);
int pointOfClosestApproach(float * point1, float * point2, float * v1, float * v2, float * distance, float * vertex); 


#define BREAK_ON_FAILED_CUT 1

#define ENABLE_BASIC_QUALITY_CUTS 1
#define ENABLE_TIMING_CUTS 1
#define ENABLE_CRAZY_LKR_ACC_CUT 1
#define ENABLE_MIN_BIAS_CUT 1
#define ENABLE_Z_COORD_CUT 1

#define MC_ENABLE_BASIC_QUALITY_CUTS 1
#define MC_ENABLE_TIMING_CUTS 0 //the timings in the MC are fucked
#define MC_ENABLE_CRAZY_LKR_ACC_CUT 1
#define MC_ENABLE_MIN_BIAS_CUT 0 //apparently all of the MC data fails this
#define MC_ENABLE_Z_COORD_CUT 1

#define SURVIVED 0
#define BASIC_QUALITY_CUTS 1
#define TIMING_CUTS 2
#define CRAZY_LKR_ACC_CUT 3
#define MIN_BIAS_CUT 4
#define Z_COORD_CUT 5

#define PI0_MASS 0.1349766 
#define PI1_MASS 0.13957018 //this is the pi+ mass but you can't use operators in c names
                            //it has a charge of 1 so the name seems fairly sensible
#define UNKNOWN_EVENT -1
#define KE3_EVENT 0
#define KM3_EVENT 1
#define K2P_EVENT 2

int mcEventType(superMcEvent * evt);
/*
 *
 *
 * : user.h,v $
 * Revision 1.2  2003/10/31 12:32:30  andrew
 * Added the -string option to main
 *
 * An arbitrary string (length STRING_MAX) can be passed to compact which is
 * saved in a global variable gString (C), COMMON/GSTRING/GSTRING (FORTRAN)
 *
 *
 * made -ndb the default. For people needin the compact database the -db option
 * was created
 *
 *
 *
 */

#endif //user.h
