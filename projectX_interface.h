/**
	\file projectX_interface.h
	\author Ken Johnson and Nick Zwart
	\date created: Jan-Mar 2005

	\brief ProjectX provides an API platform to create AVS modules and standalone code. This file
		provides intelligible prototypes of projectX functions.

	\warning *** DO NOT INCLUDE THIS FILE! ***
	It is merely to have as an intelligible reference for what functions are availible in projectX

	This library was created with the intent to be able to make routines
	in AVS (as modules) that can be compiled apart from AVS in order to run on
	the scanner. Instead of creating functions that substitute for AVS calls,
	a new interface was created for several different reasons, the most obvious
	is a cleaner interface. Another reason for change in interface was for
	programing efficiency and debugability (especially in the get functions).

	Following is an outline to the interface and ways to invoke the library. Note
	that following are just examples, but not all examples. Substitute for float the
	following: short, int, double, char. And for 3 substitute: 1, 2, 4, 5, 6.


	\section interface Basic Interface
		- declaring a field
			AVSfield *foo = NULL;
			AVSfield_float *foo = NULL;

		- creating a field call
			foo = new_AVS_field (ndim, dim, veclen, AVS_TYPE_REAL); // _REAL for float others are _INT _DOUBLE _SHORT _BYTE
			foo = new_AVS_float (ndim, dim, veclen);
			foo = new_AVS_float_3v (Nx, Ny, Nz, veclen);

		- operations on fields  *note that the field must not be a AVSfield but AVSfield_float, etc.
		  The behavior depends on whether the DEBUG_FIELD is defined or not before #include <projectX.h>
			get3v(foo,x,y,z,v) = ...
			getNv(foo,index,v) = ...  // where index is a pointer to an int array 3 elements long (if 3d field)
			getas1v(foo,i,v) = ...
		  Also are versions always going through debug regardless of DEBUG_FIELD flag.
			get3v_debug
			getNv_debug
			getas1v_debug

		- misc
			getsize(foo)   returns the product of the elements of foo->dimensions[i]
			compareFields (foo, boo)   returns 0 if fields not same size or type and 1 otherwise
			sameSize (foo, ndim, dim, veclen, type)   returns 0 if fields not same size or type and 1 otherwise

		- deleting a field (deletes a field and sets it to NULL)
			AVS_free(foo);


	\section add_feat Additional features
		- creating a field in AVS that is not shared memory (ie field is specifically
		  not allocated through AVS but rather through standard malloc). Note that these
		  fields must be deleted using noAVS_free.
			foo = new_noAVS_field (ndim, dim, veclen, AVS_TYPE_REAL);
			foo = new_noAVS_float (ndim, dim, veclen);
			foo = new_noAVS_float_3v (Nx, Ny, Nz, veclen);

		- delete these noAVS_fields with (deletes a field and sets it to NULL)
			noAVS_free(foo)

		- other miscellaneous items (not field related)
			printv(somevar)
			printvf(somefloatORdouble)
			colors


	\section conventions Conventions
		- <AVSfield*> should be replaced by either
			AVSfield
			AVSfield_double
			AVSfield_float
			AVSfield_int
			AVSfield_short
			AVSfield_char

		- <dfisc> should be replaced by double float int short or
		  char (consistent throughout the line)

		  For example:
			<dfisc> get1v (AVSfield_<dfisc> *foo, int x1, int v);
		  can be substituted by one of the following:
			double  get1v (AVSfield_double *foo, int x1, int v);
			float   get1v (AVSfield_float  *foo, int x1, int v);
			int     get1v (AVSfield_int    *foo, int x1, int v);
			short   get1v (AVSfield_short  *foo, int x1, int v);
			char    get1v (AVSfield_char   *foo, int x1, int v);
*/

/// STRUCTURE DEFINITIONS
typedef struct {
	int ndim;
	int *dimensions;
	int veclen;
	int type;
	int size;
	field_union *field_data;
} AVSfield;
typedef struct {
	int ndim;
	int *dimensions;
	int veclen;
	int type;
	int size;
	<dfisc> *data;
} AVSfield_<dfisc>;


/// DATA ALLOCATION AND DEALLOCATION
// generic
AVSfield *new_AVS_field (int ndim, int *dim, int veclength, int type);

// specific
AVSfield_<dfisc> *new_AVS_<dfisc> (int ndim, int *dim, int veclength);
AVSfield_<dfisc> *new_AVS_<dfisc>_1v (int N1, int veclength);
AVSfield_<dfisc> *new_AVS_<dfisc>_2v (int N1, int N2, int veclength);
AVSfield_<dfisc> *new_AVS_<dfisc>_3v (int N1, int N2, int N3, int veclength);
AVSfield_<dfisc> *new_AVS_<dfisc>_4v (int N1, int N2, int N3, N4, int veclength);
AVSfield_<dfisc> *new_AVS_<dfisc>_5v (int N1, int N2, int N3, N4, int N5, int veclength);
AVSfield_<dfisc> *new_AVS_<dfisc>_6v (int N1, int N2, int N3, N4, int N5, int N6, int veclength);

// delete
void AVS_free (<AVSfield*> *foo);

// all of the above functions can have a prefix no added to AVS to create and delete data
// that specificly does not use AVS to allocate the data (and therefore not shared memory).
// for example new_noAVS_float would return the same type of pointer (AVSfield_float *) which
// must be freed using noAVS_free instead of AVS_free.


/// GETSIZE FUNCTION
int getsize (<AVSfield*> *foo);


int compareFields (<AVSfield*> *foo, <AVSfield*> *boo);
int sameSize (<AVSfield*> *foo, int ndim, int *dim, int veclen, int type);


/// GET FUNCTIONS
// standard and most common get
<dfisc> get1v   (AVSfield_<dfisc> *foo, int x1, int v);
<dfisc> get2v   (AVSfield_<dfisc> *foo, int x1, int x2, int v);
<dfisc> get3v   (AVSfield_<dfisc> *foo, int x1, int x2, int x3, int v);
<dfisc> get4v   (AVSfield_<dfisc> *foo, int x1, int x2, int x3, int x4, int v);
<dfisc> get5v   (AVSfield_<dfisc> *foo, int x1, int x2, int x3, int x4, int x5, int v);
<dfisc> get6v   (AVSfield_<dfisc> *foo, int x1, int x2, int x3, int x4, int x5, int x6, int v);
<dfisc> getNv   (AVSfield_<dfisc> *foo, int x[ndim], int v);
<dfisc> getas1v (AVSfield_<dfisc> *foo, int i, int v);

// gets that are for fields with only 1 vector
<dfisc> get1   (AVSfield_<dfisc> *foo, int x1);
<dfisc> get2   (AVSfield_<dfisc> *foo, int x1, int x2);
<dfisc> get3   (AVSfield_<dfisc> *foo, int x1, int x2, int x3);
<dfisc> get4   (AVSfield_<dfisc> *foo, int x1, int x2, int x3, int x4);
<dfisc> get5   (AVSfield_<dfisc> *foo, int x1, int x2, int x3, int x4, int x5);
<dfisc> get6   (AVSfield_<dfisc> *foo, int x1, int x2, int x3, int x4, int x5, int x6);
<dfisc> getN   (AVSfield_<dfisc> *foo, int x[ndim]);

// gets that explicitly use the debug version regardless of DEBUG_FIELD flag
<dfisc> get1v_debug   (AVSfield_<dfisc> *foo, int x1, int v);
<dfisc> get2v_debug   (AVSfield_<dfisc> *foo, int x1, int x2, int v);
<dfisc> get3v_debug   (AVSfield_<dfisc> *foo, int x1, int x2, int x3, int v);
<dfisc> get4v_debug   (AVSfield_<dfisc> *foo, int x1, int x2, int x3, int x4, int v);
<dfisc> get5v_debug   (AVSfield_<dfisc> *foo, int x1, int x2, int x3, int x4, int x5, int v);
<dfisc> get6v_debug   (AVSfield_<dfisc> *foo, int x1, int x2, int x3, int x4, int x5, int x6, int v);
<dfisc> getNv_debug   (AVSfield_<dfisc> *foo, int x[ndim], int v);
<dfisc> getas1v_debug (AVSfield_<dfisc> *foo, int i, int v);


/// DEFINES OF TYPE
#define AVS_TYPE_BYTE		0
#define AVS_TYPE_INTEGER	1
#define AVS_TYPE_REAL		2
#define AVS_TYPE_DOUBLE		3
#define AVS_TYPE_SHORT		4


/// MICELLANEOUS STUFF

// easy colors for print statements
#define RED		"\e[31m"
#define GRE		"\e[32m"
#define BLU		"\e[33m"
#define YEL		"\e[33m"
#define WHI		"\e[33m"
#define BLA		"\e[33m"
#define NOC		"\e[0m"

// easy printing of variables
void printv(<isc> var);
void printvf(<df> var);
