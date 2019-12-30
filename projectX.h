/**
	\file projectX.h
	\author Ken Johnson and Nick Zwart
	\date created: Jan-Mar 2005

	\brief ProjectX provides an API platform to create AVS modules and standalone code.

	SEE projectX_interface.h for details and usage

	\todo
		- memalign - done 2005.12 using valloc
		- make most things const that can be - to eliminate picky errors
		- omit I?DV and make non [] vector access  ie get2v(f,x,y,v) -> *(f+v+(x+y*Nx)*Nv)  vs (f+(x+y*Nx)*Nv)[v]
		- move install destination to /usr/local/include and /usr/local/lib to follow
			standards better.

	Copyright 2009 Ken Johnson and Nick Zwart
	This code is released under the terms of the GNU General Public License (GPL).
	see gpl.txt for more information.
*/

#ifndef PROJECT_X_H
#define PROJECT_X_H

#ifdef DEBUG_field
#define DEBUG_FIELD
#endif

#define RED		"\e[31m"
#define GRE		"\e[32m"
#define YEL		"\e[33m"
#define BLU		"\e[34m"
#define MAG		"\e[35m"
#define CYA		"\e[36m"
#define WHI		"\e[37m"
#define BLA		"\e[30m"
#define NOC		"\e[39m"


// #define RESET        	"\e[0m"
// #define BOLD         	"\e[1m"
// #define BOLD_OFF     	"\e[22m"
// #define UNDERLINE    	"\e[4m"
// #define UNDERLINE_OFF	"\e[24m"
// #define BLINK        	"\e[5m"
// #define BLINK_OFF    	"\e[25m"
//
// #define BAK_RED     	"\e[41m"
// #define BAK_GRE     	"\e[42m"
// #define BAK_YEL     	"\e[43m"
// #define BAK_BLU     	"\e[44m"
// #define BAK_MAG     	"\e[45m"
// #define BAK_CYA     	"\e[46m"
// #define BAK_WHI     	"\e[47m"
// #define BAK_BLA     	"\e[40m"
// #define BAK_NOC     	"\e[49m"

#ifndef  assert
#include <assert.h>
#endif // #ifndef assert

#include <stdlib.h>

#include <stdio.h>					// for printf etc.
#include <malloc.h>					// for memalign

#define printv(var) 	printf(#var " = %d\n",(int) var)
#define printvf(var)	printf(#var " = %f\n", (float) var)


#ifdef __cplusplus		// this makes it possible to use the library in cpp code
extern "C"
{
#endif	// __cplusplus

#ifdef _AVS_H_defined
#define AVS
#endif

////////////////////////////////////////////////////////////
// Fake AVS only stuff
////////////////////////////////////////////////////////////
#ifndef AVS

#define AVS_TYPE_BYTE		0
#define AVS_TYPE_INTEGER	1
#define AVS_TYPE_REAL		2
#define AVS_TYPE_DOUBLE		3
#define AVS_TYPE_SHORT		4

#define UNIFORM		0
#define RECTILINEAR	1
#define IRREGULAR	2

// These structure replicate necessary types in AVS
typedef struct {
	int ndim;
	int *dimensions;
	int veclen;
	int type;
	int size;
	float *data;
} AVSfield_float;
typedef struct {
	int ndim;
	int *dimensions;
	int veclen;
	int type;
	int size;
	unsigned char *data;
} AVSfield_char;
typedef struct {
	int ndim;
	int *dimensions;
	int veclen;
	int type;
	int size;
	double *data;
} AVSfield_double;
typedef struct {
	int ndim;
	int *dimensions;
	int veclen;
	int type;
	int size;
	int *data;
} AVSfield_int;
typedef struct {
	int ndim;
	int *dimensions;
	int veclen;
	int type;
	int size;
	short *data;		// dummy variable (ie contains no data)
} AVSfield_short;
typedef struct {
   int ndim;			/* Number of dimensions in the field */
   int *dimensions;		/* dimension along each axis, len is ndim */
   int veclen;			/* number of components at each point */
   int type;			/* data type (see below for values) */
   int size;			/* size of each element */
   union {			/* the field itself */
	   unsigned char *field_data_char;
	   float         *field_data_float_u;
	   double        *field_data_double_u;
	   int           *field_data_int_u;
	   short         *field_data_short_u;
   } field_union;
} AVSfield;

#define __dim(__fo,__i)	((__fo)->dimensions[__i])
#define __vs(__fo)	((__fo)->veclen)

#define _raw1Dvec (__fo, __a, __v) \
	( (__foo)->data[__v + __vs(__fo) * (__a)] )
#define _raw2Dvec (__fo, __a, __b, __v) \
	( (__foo)->data[__v+__vs(__fo)*(__a+__dim(__fo,0)*(__b))] )
#define _raw3Dvec (__fo, __a, __b, __c, __v) \
	( (__foo)->data[__v+__vs(__fo)*(__a+__dim(__fo,0)*(__b+__dim(__fo,2)*(__c)))] )
#define _raw4Dvec (__fo, __a, __b, __c, __d, __v) \
	( (__foo)->data[__v+__vs(__fo)*(__a+__dim(__fo,0)*(__b+__dim(__fo,2)*(__c+__dim(__fo,3)*(__d))))] )
#define _raw5Dvec (__fo, __a, __b, __c, __d, __e, __v) \
	( (__foo)->data[__v+__vs(__fo)*(__a+__dim(__fo,0)*(__b+__dim(__fo,2)*(__c+__dim(__fo,3)*(__d+__dim(__fo,4)*(__e)))))] )
#define _raw6Dvec (__fo, __a, __b, __c, __d, __e, __f, __v) \
	( (__foo)->data[__v+__vs(__fo)*(__a+__dim(__fo,0)*(__b+__dim(__fo,2)*(__c+__dim(__fo,3)*(__d+__dim(__fo,4)*(__e+__dim(__fo,5)*(__f))))))] )

#define _raw1D (__fo, __a) \
	( (__foo)->data[(__a)] )
#define _raw2D (__fo, __a, __b) \
	( (__foo)->data[(__a+__dim(__fo,0)*(__b))] )
#define _raw3D (__fo, __a, __b, __c) \
	( (__foo)->data[(__a+__dim(__fo,0)*(__b+__dim(__fo,2)*(__c)))] )
#define _raw4D (__fo, __a, __b, __c, __d) \
	( (__foo)->data[(__a+__dim(__fo,0)*(__b+__dim(__fo,2)*(__c+__dim(__fo,3)*(__d))))] )
#define _raw5D (__fo, __a, __b, __c, __d, __e) \
	( (__foo)->data[(__a+__dim(__fo,0)*(__b+__dim(__fo,2)*(__c+__dim(__fo,3)*(__d+__dim(__fo,4)*(__e)))))] )
#define _raw6D (__fo, __a, __b, __c, __d, __e, __f) \
	( (__foo)->data[(__a+__dim(__fo,0)*(__b+__dim(__fo,2)*(__c+__dim(__fo,3)*(__d+__dim(__fo,4)*(__e+__dim(__fo,5)*(__f))))))] )

#define I2D(field, i, j) \
	(((field)->data)[(field)->dimensions[0]*(j) + (i)])

#define I3D(field, i, j, k) \
	(((field)->data)[ ((field)->dimensions[0] * \
				  ((field)->dimensions[1]*(k) + (j))) + (i) ])

#define I4D(field, i, j, k, l) \
	(((field)->data)[ ((field)->dimensions[0] * \
			   (((field)->dimensions[1] * \
			     ((field)->dimensions[2]*(l) + (k))) + (j))) + (i)])

/* These are the vector field versions of the indexing macros */

#define I1DV(field, i) ((field)->data + (field)->veclen * (i))

#define I2DV(field, i, j) \
    (((field)->data) + ((field)->veclen * ((field)->dimensions[0]*(j) + (i))))

#define I3DV(field, i, j, k) \
    (((field)->data)+ ((field)->veclen * (((field)->dimensions[0] * \
					   ((field)->dimensions[1]*(k) + (j)))\
					  + (i)) ))

#define I4DV(field, i, j, k, l) \
	(((field)->data) + ((field)->veclen * (((field)->dimensions[0] * \
					       (((field)->dimensions[1] * \
						((field)->dimensions[2]*(l) \
						 + (k))) + (j))) + (i))))

//void noAVSfield_free (AVSfield_generic *foo);
/*void noAVSfield_free_float (AVSfield_float *foo);
void noAVSfield_free_char (AVSfield_char *foo);
void noAVSfield_free_double (AVSfield_double *foo);
void noAVSfield_free_int (AVSfield_int *foo);
void noAVSfield_free_short (AVSfield_short *foo);*/

// destroy unnecessary avs calls
#define AVSmodfptr_parameter(a,b,c,d)
#define AVSerror(format, args...)	
#define AVSmodule_status(x1,x2)		printf(x1 " is %2.0f%% done\r",x2);fflush(stdout);

#define noAVS_free		AVS_free

#define AVS_free(foo)		do{	\
								if (NULL == foo)	{	\
									printf(YEL __FILE__":%d" RED " -- AVS_free ("#foo"): "#foo" already NULL\n"NOC,__LINE__);	\
									exit(1);	\
								}	\
								noAVSfield_free((AVSfield*)foo);	\
								(foo)=NULL;	\
							}while(0)

#define new_AVS_field(nd,di,ve,ty)							new_noAVS_field(nd,di,ve,ty)

#define new_AVS_float(nd,di,ve)								new_noAVS_float(nd,di,ve)
#define new_AVS_float_1v(n1, vec)							new_noAVS_float_1v (n1, vec)
#define new_AVS_float_2v(n1, n2, v)							new_noAVS_float_2v (n1, n2, v)
#define new_AVS_float_3v(n1, n2, n3, v)						new_noAVS_float_3v (n1, n2, n3, v)
#define new_AVS_float_4v(n1, n2, n3, n4, v)					new_noAVS_float_4v (n1, n2, n3, n4, v)
#define new_AVS_float_5v(n1, n2, n3, n4, n5, v)				new_noAVS_float_5v (n1, n2, n3, n4, n5, v)
#define new_AVS_float_6v(n1, n2, n3, n4, n5, n6, v)			new_noAVS_float_6v (n1, n2, n3, n4, n5, n6, v)

#define new_AVS_double(nd,di,ve)							new_noAVS_double(nd,di,ve)
#define new_AVS_double_1v(n1, vec)							new_noAVS_double_1v (n1, vec)
#define new_AVS_double_2v(n1, n2, v)						new_noAVS_double_2v (n1, n2, v)
#define new_AVS_double_3v(n1, n2, n3, v)					new_noAVS_double_3v (n1, n2, n3, v)
#define new_AVS_double_4v(n1, n2, n3, n4, v)				new_noAVS_double_4v (n1, n2, n3, n4, v)
#define new_AVS_double_5v(n1, n2, n3, n4, n5, v)			new_noAVS_double_5v (n1, n2, n3, n4, n5, v)
#define new_AVS_double_6v(n1, n2, n3, n4, n5, n6, v)		new_noAVS_double_6v (n1, n2, n3, n4, n5, n6, v)

#define new_AVS_char(nd,di,ve)								new_noAVS_char(nd,di,ve)
#define new_AVS_char_1v(n1, vec)							new_noAVS_char_1v (n1, vec)
#define new_AVS_char_2v(n1, n2, v)							new_noAVS_char_2v (n1, n2, v)
#define new_AVS_char_3v(n1, n2, n3, v)						new_noAVS_char_3v (n1, n2, n3, v)
#define new_AVS_char_4v(n1, n2, n3, n4, v)					new_noAVS_char_4v (n1, n2, n3, n4, v)
#define new_AVS_char_5v(n1, n2, n3, n4, n5, v)				new_noAVS_char_5v (n1, n2, n3, n4, n5, v)
#define new_AVS_char_6v(n1, n2, n3, n4, n5, n6, v)			new_noAVS_char_6v (n1, n2, n3, n4, n5, n6, v)

#define new_AVS_int(nd,di,ve)								new_noAVS_int(nd,di,ve)
#define new_AVS_int_1v(n1, vec)								new_noAVS_int_1v (n1, vec)
#define new_AVS_int_2v(n1, n2, v)							new_noAVS_int_2v (n1, n2, v)
#define new_AVS_int_3v(n1, n2, n3, v)						new_noAVS_int_3v (n1, n2, n3, v)
#define new_AVS_int_4v(n1, n2, n3, n4, v)					new_noAVS_int_4v (n1, n2, n3, n4, v)
#define new_AVS_int_5v(n1, n2, n3, n4, n5, v)				new_noAVS_int_5v (n1, n2, n3, n4, n5, v)
#define new_AVS_int_6v(n1, n2, n3, n4, n5, n6, v)			new_noAVS_int_6v (n1, n2, n3, n4, n5, n6, v)

#define new_AVS_short(nd,di,ve)								new_noAVS_short(nd,di,ve)
#define new_AVS_short_1v(n1, vec)							new_noAVS_short_1v (n1, vec)
#define new_AVS_short_2v(n1, n2, v)							new_noAVS_short_2v (n1, n2, v)
#define new_AVS_short_3v(n1, n2, n3, v)						new_noAVS_short_3v (n1, n2, n3, v)
#define new_AVS_short_4v(n1, n2, n3, n4, v)					new_noAVS_short_4v (n1, n2, n3, n4, v)
#define new_AVS_short_5v(n1, n2, n3, n4, n5, v)				new_noAVS_short_5v (n1, n2, n3, n4, n5, v)
#define new_AVS_short_6v(n1, n2, n3, n4, n5, n6, v)			new_noAVS_short_6v (n1, n2, n3, n4, n5, n6, v)

////////////////////////////////////////////////////////////
// End of Fake AVS only stuff
////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////
// AVS only stuff
////////////////////////////////////////////////////////////
#else	//AVS

AVSfield *new_yesAVS_free (int ndim, int *dims, int veclength, int type);

AVSfield *new_yesAVS_field (int ndim, int *dims, int veclength, int type);

AVSfield_float *new_yesAVS_float (int ndim, int *dims, int veclength);
AVSfield_float *new_yesAVS_float_1v (int N_1, int veclength);
AVSfield_float *new_yesAVS_float_2v (int N_1, int N_2, int veclength);
AVSfield_float *new_yesAVS_float_3v (int N_1, int N_2, int N_3, int veclength);
AVSfield_float *new_yesAVS_float_4v (int N_1, int N_2, int N_3, int N_4, int veclength);
AVSfield_float *new_yesAVS_float_5v (int N_1, int N_2, int N_3, int N_4, int N_5, int veclength);
AVSfield_float *new_yesAVS_float_6v (int N_1, int N_2, int N_3, int N_4, int N_5, int N_6, int veclength);

AVSfield_char *new_yesAVS_char (int ndim, int *dims, int veclength);
AVSfield_char *new_yesAVS_char_1v (int N_1, int veclength);
AVSfield_char *new_yesAVS_char_2v (int N_1, int N_2, int veclength);
AVSfield_char *new_yesAVS_char_3v (int N_1, int N_2, int N_3, int veclength);
AVSfield_char *new_yesAVS_char_4v (int N_1, int N_2, int N_3, int N_4, int veclength);
AVSfield_char *new_yesAVS_char_5v (int N_1, int N_2, int N_3, int N_4, int N_5, int veclength);
AVSfield_char *new_yesAVS_char_6v (int N_1, int N_2, int N_3, int N_4, int N_5, int N_6, int veclength);

AVSfield_short *new_yesAVS_short (int ndim, int *dims, int veclength);
AVSfield_short *new_yesAVS_short_1v (int N_1, int veclength);
AVSfield_short *new_yesAVS_short_2v (int N_1, int N_2, int veclength);
AVSfield_short *new_yesAVS_short_3v (int N_1, int N_2, int N_3, int veclength);
AVSfield_short *new_yesAVS_short_4v (int N_1, int N_2, int N_3, int N_4, int veclength);
AVSfield_short *new_yesAVS_short_5v (int N_1, int N_2, int N_3, int N_4, int N_5, int veclength);
AVSfield_short *new_yesAVS_short_6v (int N_1, int N_2, int N_3, int N_4, int N_5, int N_6, int veclength);

AVSfield_double *new_yesAVS_double (int ndim, int *dims, int veclength);
AVSfield_double *new_yesAVS_double_1v (int N_1, int veclength);
AVSfield_double *new_yesAVS_double_2v (int N_1, int N_2, int veclength);
AVSfield_double *new_yesAVS_double_3v (int N_1, int N_2, int N_3, int veclength);
AVSfield_double *new_yesAVS_double_4v (int N_1, int N_2, int N_3, int N_4, int veclength);
AVSfield_double *new_yesAVS_double_5v (int N_1, int N_2, int N_3, int N_4, int N_5, int veclength);
AVSfield_double *new_yesAVS_double_6v (int N_1, int N_2, int N_3, int N_4, int N_5, int N_6, int veclength);

AVSfield_int *new_yesAVS_int (int ndim, int *dims, int veclength);
AVSfield_int *new_yesAVS_int_1v (int N_1, int veclength);
AVSfield_int *new_yesAVS_int_2v (int N_1, int N_2, int veclength);
AVSfield_int *new_yesAVS_int_3v (int N_1, int N_2, int N_3, int veclength);
AVSfield_int *new_yesAVS_int_4v (int N_1, int N_2, int N_3, int N_4, int veclength);
AVSfield_int *new_yesAVS_int_5v (int N_1, int N_2, int N_3, int N_4, int N_5, int veclength);
AVSfield_int *new_yesAVS_int_6v (int N_1, int N_2, int N_3, int N_4, int N_5, int N_6, int veclength);

									//printf(YEL " "  __FILE__ " : " __LINE__" " RED "-- AVS_free (" #foo "): " #foo " already NULL\n"NOC);

#define AVS_free(foo)		do{	\
								if (NULL == foo)	{	\
									printf(YEL __FILE__":%d" RED " -- AVS_free ("#foo"): "#foo" already NULL\n"NOC,__LINE__);	\
									exit(1);	\
								}	\
								AVSfield_free((AVSfield *) (foo));	\
								(foo)=NULL;	\
							}while(0);

#define noAVS_free(foo)		do{	\
								if (NULL == foo)	{	\
									printf(YEL __FILE__":%d" RED " -- noAVS_free ("#foo"): "#foo" already NULL\n"NOC,__LINE__);	\
									exit(1);	\
								}	\
								noAVSfield_free((AVSfield*)(foo));	\
								(foo)=NULL;	\
							}while(0);


#define new_AVS_field(nd,di,ve,ty)							new_yesAVS_field(nd,di,ve,ty)

#define new_AVS_float(nd,di,ve)								new_yesAVS_float(nd,di,ve)
#define new_AVS_float_1v(n1, vec)							new_yesAVS_float_1v (n1, vec)
#define new_AVS_float_2v(n1, n2, v)							new_yesAVS_float_2v (n1, n2, v)
#define new_AVS_float_3v(n1, n2, n3, v)						new_yesAVS_float_3v (n1, n2, n3, v)
#define new_AVS_float_4v(n1, n2, n3, n4, v)					new_yesAVS_float_4v (n1, n2, n3, n4, v)
#define new_AVS_float_5v(n1, n2, n3, n4, n5, v)				new_yesAVS_float_5v (n1, n2, n3, n4, n5, v)
#define new_AVS_float_6v(n1, n2, n3, n4, n5, n6, v)			new_yesAVS_float_6v (n1, n2, n3, n4, n5, n6, v)

#define new_AVS_double(nd,di,ve)							new_yesAVS_double(nd,di,ve)
#define new_AVS_double_1v(n1, vec)							new_yesAVS_double_1v (n1, vec)
#define new_AVS_double_2v(n1, n2, v)						new_yesAVS_double_2v (n1, n2, v)
#define new_AVS_double_3v(n1, n2, n3, v)					new_yesAVS_double_3v (n1, n2, n3, v)
#define new_AVS_double_4v(n1, n2, n3, n4, v)				new_yesAVS_double_4v (n1, n2, n3, n4, v)
#define new_AVS_double_5v(n1, n2, n3, n4, n5, v)			new_yesAVS_double_5v (n1, n2, n3, n4, n5, v)
#define new_AVS_double_6v(n1, n2, n3, n4, n5, n6, v)		new_yesAVS_double_6v (n1, n2, n3, n4, n5, n6, v)

#define new_AVS_char(nd,di,ve)								new_yesAVS_char(nd,di,ve)
#define new_AVS_char_1v(n1, vec)							new_yesAVS_char_1v (n1, vec)
#define new_AVS_char_2v(n1, n2, v)							new_yesAVS_char_2v (n1, n2, v)
#define new_AVS_char_3v(n1, n2, n3, v)						new_yesAVS_char_3v (n1, n2, n3, v)
#define new_AVS_char_4v(n1, n2, n3, n4, v)					new_yesAVS_char_4v (n1, n2, n3, n4, v)
#define new_AVS_char_5v(n1, n2, n3, n4, n5, v)				new_yesAVS_char_5v (n1, n2, n3, n4, n5, v)
#define new_AVS_char_6v(n1, n2, n3, n4, n5, n6, v)			new_yesAVS_char_6v (n1, n2, n3, n4, n5, n6, v)

#define new_AVS_int(nd,di,ve)								new_yesAVS_int(nd,di,ve)
#define new_AVS_int_1v(n1, vec)								new_yesAVS_int_1v (n1, vec)
#define new_AVS_int_2v(n1, n2, v)							new_yesAVS_int_2v (n1, n2, v)
#define new_AVS_int_3v(n1, n2, n3, v)						new_yesAVS_int_3v (n1, n2, n3, v)
#define new_AVS_int_4v(n1, n2, n3, n4, v)					new_yesAVS_int_4v (n1, n2, n3, n4, v)
#define new_AVS_int_5v(n1, n2, n3, n4, n5, v)				new_yesAVS_int_5v (n1, n2, n3, n4, n5, v)
#define new_AVS_int_6v(n1, n2, n3, n4, n5, n6, v)			new_yesAVS_int_6v (n1, n2, n3, n4, n5, n6, v)

#define new_AVS_short(nd,di,ve)								new_yesAVS_short(nd,di,ve)
#define new_AVS_short_1v(n1, vec)							new_yesAVS_short_1v (n1, vec)
#define new_AVS_short_2v(n1, n2, v)							new_yesAVS_short_2v (n1, n2, v)
#define new_AVS_short_3v(n1, n2, n3, v)						new_yesAVS_short_3v (n1, n2, n3, v)
#define new_AVS_short_4v(n1, n2, n3, n4, v)					new_yesAVS_short_4v (n1, n2, n3, n4, v)
#define new_AVS_short_5v(n1, n2, n3, n4, n5, v)				new_yesAVS_short_5v (n1, n2, n3, n4, n5, v)
#define new_AVS_short_6v(n1, n2, n3, n4, n5, n6, v)			new_yesAVS_short_6v (n1, n2, n3, n4, n5, n6, v)



////////////////////////////////////////////////////////////
// End of AVS only stuff
////////////////////////////////////////////////////////////

#endif	// AVS

#define AVS_TYPE_CHAR		AVS_TYPE_BYTE
#define AVS_TYPE_FLOAT		AVS_TYPE_REAL

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// for a temporary memory allocation test
void noAVSfield_free (AVSfield *foo);

AVSfield *new_noAVS_field (int ndim, int *dims, int veclength, int type);

AVSfield_float *new_noAVS_float (int ndim, int *dims, int veclength);
AVSfield_float *new_noAVS_float_1v (int N_1, int veclength);
AVSfield_float *new_noAVS_float_2v (int N_1, int N_2, int veclength);
AVSfield_float *new_noAVS_float_3v (int N_1, int N_2, int N_3, int veclength);
AVSfield_float *new_noAVS_float_4v (int N_1, int N_2, int N_3, int N_4, int veclength);
AVSfield_float *new_noAVS_float_5v (int N_1, int N_2, int N_3, int N_4, int N_5, int veclength);
AVSfield_float *new_noAVS_float_6v (int N_1, int N_2, int N_3, int N_4, int N_5, int N_6, int veclength);

AVSfield_char *new_noAVS_char (int ndim, int *dims, int veclength);
AVSfield_char *new_noAVS_char_1v (int N_1, int veclength);
AVSfield_char *new_noAVS_char_2v (int N_1, int N_2, int veclength);
AVSfield_char *new_noAVS_char_3v (int N_1, int N_2, int N_3, int veclength);
AVSfield_char *new_noAVS_char_4v (int N_1, int N_2, int N_3, int N_4, int veclength);
AVSfield_char *new_noAVS_char_5v (int N_1, int N_2, int N_3, int N_4, int N_5, int veclength);
AVSfield_char *new_noAVS_char_6v (int N_1, int N_2, int N_3, int N_4, int N_5, int N_6, int veclength);

AVSfield_short *new_noAVS_short (int ndim, int *dims, int veclength);
AVSfield_short *new_noAVS_short_1v (int N_1, int veclength);
AVSfield_short *new_noAVS_short_2v (int N_1, int N_2, int veclength);
AVSfield_short *new_noAVS_short_3v (int N_1, int N_2, int N_3, int veclength);
AVSfield_short *new_noAVS_short_4v (int N_1, int N_2, int N_3, int N_4, int veclength);
AVSfield_short *new_noAVS_short_5v (int N_1, int N_2, int N_3, int N_4, int N_5, int veclength);
AVSfield_short *new_noAVS_short_6v (int N_1, int N_2, int N_3, int N_4, int N_5, int N_6, int veclength);

AVSfield_double *new_noAVS_double (int ndim, int *dims, int veclength);
AVSfield_double *new_noAVS_double_1v (int N_1, int veclength);
AVSfield_double *new_noAVS_double_2v (int N_1, int N_2, int veclength);
AVSfield_double *new_noAVS_double_3v (int N_1, int N_2, int N_3, int veclength);
AVSfield_double *new_noAVS_double_4v (int N_1, int N_2, int N_3, int N_4, int veclength);
AVSfield_double *new_noAVS_double_5v (int N_1, int N_2, int N_3, int N_4, int N_5, int veclength);
AVSfield_double *new_noAVS_double_6v (int N_1, int N_2, int N_3, int N_4, int N_5, int N_6, int veclength);

AVSfield_int *new_noAVS_int (int ndim, int *dims, int veclength);
AVSfield_int *new_noAVS_int_1v (int N_1, int veclength);
AVSfield_int *new_noAVS_int_2v (int N_1, int N_2, int veclength);
AVSfield_int *new_noAVS_int_3v (int N_1, int N_2, int N_3, int veclength);
AVSfield_int *new_noAVS_int_4v (int N_1, int N_2, int N_3, int N_4, int veclength);
AVSfield_int *new_noAVS_int_5v (int N_1, int N_2, int N_3, int N_4, int N_5, int veclength);
AVSfield_int *new_noAVS_int_6v (int N_1, int N_2, int N_3, int N_4, int N_5, int N_6, int veclength);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Stuff for all purposes
////////////////////////////////////////////////////////////
#define I1D(field,i)	(field)->data[i]
#define I5DV(field, i, j, k, l, m) \
                                                ( \
                                                ((field)->data) + \
                                                ((field)->veclen *  \
                                                (((field)->dimensions[0] * \
                                                (((field)->dimensions[1] * \
                                                (((field)->dimensions[2] * \
                                                ((field)->dimensions[3] * \
                                                (m) + (l))) + (k))) + (j))) + (i))) \
                                                )


#define I6DV(field, i, j, k, l, m, n) \
                                                ( \
                                                ((field)->data) + \
                                                ((field)->veclen *  \
                                                (((field)->dimensions[0] * \
                                                (((field)->dimensions[1] * \
                                                (((field)->dimensions[2] * \
                                                (((field)->dimensions[3] * \
                                                ((field)->dimensions[4] * \
                                                (n) + (m))) + (l))) + (k))) + (j))) + (i))) \
                                                )
#define I5D(field, i, j, k, l, m) \
                                                ( \
                                                ((field)->data) + \
                                                (  \
                                                (((field)->dimensions[0] * \
                                                (((field)->dimensions[1] * \
                                                (((field)->dimensions[2] * \
                                                ((field)->dimensions[3] * \
                                                (m) + (l))) + (k))) + (j))) + (i))) \
                                                )[0]


#define I6D(field, i, j, k, l, m, n) \
                                                ( \
                                                ((field)->data) + \
                                                (  \
                                                (((field)->dimensions[0] * \
                                                (((field)->dimensions[1] * \
                                                (((field)->dimensions[2] * \
                                                (((field)->dimensions[3] * \
                                                ((field)->dimensions[4] * \
                                                (n) + (m))) + (l))) + (k))) + (j))) + (i))) \
                                                )[0]

// returns product of dimensions array
unsigned long getsize (const void *foo);

unsigned long INDV_index (AVSfield *in, int *x);

// returns 0 if ndim dimensions veclen or type are different, 1 otherwise
int compareFields (const void *foo, const void *boo);
// similar to compareFields but params instead of a second field
int sameSize (const void *foo, int ndim, int *dim, int veclen, int type);

void copy_field (const void *in, const void *out);
void reset_field (const void *in, const double c);
void reset_field_f (AVSfield_float *in, const float c);
void reset_field_d (AVSfield_double *in, const double c);
void reset_field_i (AVSfield_int *in, const int c);
void reset_field_c (AVSfield_char *in, const unsigned char c);
void reset_field_s (AVSfield_short *in, const short c);

void *get_1Dv_ (void *foo, int x, int i, const char *filename, int line, const char *var);
void *get_2Dv_(void *foo, int x, int y, int i, const char *filename, int line, const char *var);
void *get_3Dv_(void *foo, int x, int y, int z, int i, const char *filename, int line, const char *var);
void *get_4Dv_(void *foo, int x1, int x2, int x3, int x4, int i, const char *filename, int line, const char *var);
void *get_5Dv_(void *foo, int x1, int x2, int x3, int x4, int x5, int i, const char *filename, int line, const char *var);
void *get_6Dv_(void *foo, int x1, int x2, int x3, int x4, int x5, int x6, int i, const char *filename, int line, const char *var);
void *get_NDv_(void *foo, int *x, int v, const char *filename, int line, const char *var);
void *get_as1Dv_ (void *foo, unsigned long x, int i, const char *filename, int line, const char *var);


#define get1v_debug(foo,x1,i)               	(*((typeof((foo)->data))get_1Dv_(foo,x1,i,__FILE__,__LINE__,#foo)))
#define get2v_debug(foo,x1,x2,i)            	(*((typeof((foo)->data))get_2Dv_(foo,x1,x2,i,__FILE__,__LINE__,#foo)))
#define get3v_debug(foo,x1,x2,x3,i)         	(*((typeof((foo)->data))get_3Dv_(foo,x1,x2,x3,i,__FILE__,__LINE__,#foo)))
#define get4v_debug(foo,x1,x2,x3,x4,i)      	(*((typeof((foo)->data))get_4Dv_(foo,x1,x2,x3,x4,i,__FILE__,__LINE__,#foo)))
#define get5v_debug(foo,x1,x2,x3,x4,x5,i)   	(*((typeof((foo)->data))get_5Dv_(foo,x1,x2,x3,x4,x5,i,__FILE__,__LINE__,#foo)))
#define get6v_debug(foo,x1,x2,x3,x4,x5,x6,i)	(*((typeof((foo)->data))get_6Dv_(foo,x1,x2,x3,x4,x5,x6,i,__FILE__,__LINE__,#foo)))
#define getNv_debug(foo,x,i)                	(*((typeof((foo)->data))get_NDv_(foo,x,i,__FILE__,__LINE__,#foo)))
#define getas1v_debug(foo,x1,i)             	(*((typeof((foo)->data))get_as1Dv_(foo,x1,i,__FILE__,__LINE__,#foo)))



////////////////////////////////////////////////////////////
// Debug or not
////////////////////////////////////////////////////////////
#ifdef DEBUG_FIELD

//------------------------------------------------------------------
#define get1(_foo,x1)                  		get1v_debug(_foo,x1,0)
#define get2(_foo,x1,x2)               		get2v_debug(_foo,x1,x2,0)
#define get3(_foo,x1,x2,x3)            		get3v_debug(_foo,x1,x2,x3,0)
#define get4(_foo,x1,x2,x3,x4)         		get4v_debug(_foo,x1,x2,x3,x4,0)
#define get5(_foo,x1,x2,x3,x4,x5)      		get5v_debug(_foo,x1,x2,x3,x4,x5,0)
#define get6(_foo,x1,x2,x3,x4,x5,x6)   		get6v_debug(_foo,x1,x2,x3,x4,x5,x6,0)
#define getN(__foo,__x)                		getNv_debug(__foo,__x,0)
#define getas1(__foo,__x)              		getas1v_debug(__foo,__x,0)

#define get1v(_foo,x1,i)               		get1v_debug(_foo,x1,i)
#define get2v(_foo,x1,x2,i)            		get2v_debug(_foo,x1,x2,i)
#define get3v(_foo,x1,x2,x3,i)         		get3v_debug(_foo,x1,x2,x3,i)
#define get4v(_foo,x1,x2,x3,x4,i)      		get4v_debug(_foo,x1,x2,x3,x4,i)
#define get5v(_foo,x1,x2,x3,x4,x5,i)   		get5v_debug(_foo,x1,x2,x3,x4,x5,i)
#define get6v(_foo,x1,x2,x3,x4,x5,x6,i)		get6v_debug(_foo,x1,x2,x3,x4,x5,x6,i)
#define getNv(__foo,__x,__i)           		getNv_debug(__foo,__x,__i)
#define getas1v(__foo,__x,__i)         		getas1v_debug(__foo,__x,__i)

#else  // noDEBUG..................
//------------------------------------------------------------------
#define get1(foo,x1)                	I1D(foo,x1)
#define get2(foo,x1,x2)             	I2D(foo,x1,x2)
#define get3(foo,x1,x2,x3)          	I3D(foo,x1,x2,x3)
#define get4(foo,x1,x2,x3,x4)       	I4D(foo,x1,x2,x3,x4)
#define get5(foo,x1,x2,x3,x4,x5)    	I5D(foo,x1,x2,x3,x4,x5)
#define get6(foo,x1,x2,x3,x4,x5,x6) 	I6D(foo,x1,x2,x3,x4,x5,x6)
#define getN(foo,x)                 	I1D(foo,INDV_index(foo,x))
#define getas1(__foo,__x)           	((__foo)->data[__x])

#define get1v(foo,x1,i)               	I1DV(foo,x1)[i]
#define get2v(foo,x1,x2,i)            	I2DV(foo,x1,x2)[i]
#define get3v(foo,x1,x2,x3,i)         	I3DV(foo,x1,x2,x3)[i]
#define get4v(foo,x1,x2,x3,x4,i)      	I4DV(foo,x1,x2,x3,x4)[i]
#define get5v(foo,x1,x2,x3,x4,x5,i)   	I5DV(foo,x1,x2,x3,x4,x5)[i]
#define get6v(foo,x1,x2,x3,x4,x5,x6,i)	I6DV(foo,x1,x2,x3,x4,x5,x6)[i]
#define getNv(foo,x,i)                	I1DV(foo,INDV_index((AVSfield*)(foo),x))[i]
#define getas1v(foo,x1,i)             	I1DV(foo,x1)[i]


#endif	// DEBUG..................


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif // PROJECT_X_H
