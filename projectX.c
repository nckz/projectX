/**
	\file projectX.c
	\author Ken Johnson and Nick Zwart
	\date created: Jan-Mar 2005

	\brief ProjectX provides an API platform to create AVS modules and standalone code.

	see projectX_interface.h for details

	Copyright 2009 Ken Johnson and Nick Zwart
	This code is released under the terms of the GNU General Public License (GPL).
	see gpl.txt for more information.
**/

#ifdef DO_AVS
#include <avs/avs.h>
#include <avs/port.h>
#include <avs/field.h>
#endif // ifdef DO_AVS
// #define DEBUG_FIELD
#include "projectX.h"

#include <limits.h>
#include <string.h>


#ifndef DO_AVS
#define field_data 	     field_union.field_data_char
#define field_data_float     field_union.field_data_float_u
#define field_data_double    field_union.field_data_double_u
#define field_data_int	     field_union.field_data_int_u
#define field_data_short     field_union.field_data_short_u
#endif // ifndef DO_AVS


void noAVSfield_free (AVSfield *foo);
void noAVSfield_free_float (AVSfield_float *foo);
void noAVSfield_free_char (AVSfield_char *foo);
void noAVSfield_free_double (AVSfield_double *foo);
void noAVSfield_free_int (AVSfield_int *foo);
void noAVSfield_free_short (AVSfield_short *foo);


void copy_field (const void *in1, const void *out1)	{
	const AVSfield *in = (const AVSfield *) in1, *out = (const AVSfield *) out1;

	assert (in != NULL);
	assert (out != NULL);

	assert(compareFields(in, out) );

	memcpy ((*out).field_union.field_data_char, (*in).field_union.field_data_char,  getsize(in) * in->veclen * in->size);
}
void reset_field (const void *in, const double c)	{
	assert (in != NULL);
	switch(((AVSfield *)in)->type)	{
		case AVS_TYPE_REAL:
			reset_field_f((AVSfield_float *)in,(float)c);
			return;
		case AVS_TYPE_DOUBLE:
			reset_field_d((AVSfield_double *)in,c);
			return;
		case AVS_TYPE_INTEGER:
			reset_field_i((AVSfield_int *)in,(int)c);
			return;
		case AVS_TYPE_BYTE:
			reset_field_c((AVSfield_char *)in,(unsigned char)c);
			return;
		case AVS_TYPE_SHORT:
			reset_field_s((AVSfield_short *)in,(short)c);
			return;
		default:
			fprintf(stderr, RED"ERROR: reset_field: bad avs type\nexiting\n"NOC);
			AVSerror ("reset_field: bad avs type. exiting");
			exit(1);
	}
}
void reset_field_f (AVSfield_float *in, const float c)	{
	unsigned long i;
	assert (in != NULL);
	assert (in->type == AVS_TYPE_REAL);
// 	for (i=0; i<getsize(in)*in->veclen; i++)	{
// 		in->data[i] = c;
// 	}
	unsigned long size, memcpy_size, v;
	for(v=0; v<in->veclen; v++)	{
		getas1v(in, 0, v) = c;
	}
	memcpy_size = in->veclen*in->size;
	size = getsize(in);
	i = 1;
	while (2*i < size)	{
		memcpy (&getas1v(in, i, 0), &getas1v(in, 0, 0), memcpy_size);
		i *= 2;
		memcpy_size *= 2;
	}
	memcpy (&getas1v(in, i, 0), &getas1v(in, 0, 0), (size-i)*in->veclen*in->size);
}
void reset_field_d (AVSfield_double *in, const double c)	{
	unsigned long i;
	assert (in != NULL);
	assert (in->type == AVS_TYPE_DOUBLE);
	for (i=0; i<getsize(in)*in->veclen; i++)	{
		in->data[i] = c;
	}
}
void reset_field_i (AVSfield_int *in, const int c)	{
	unsigned long i;
	assert (in != NULL);
	assert (in->type == AVS_TYPE_INTEGER);
	for (i=0; i<getsize(in)*in->veclen; i++)	{
		in->data[i] = c;
	}
}
void reset_field_c (AVSfield_char *in, const unsigned char c)	{
	unsigned long i;
	assert (in != NULL);
	assert (in->type == AVS_TYPE_BYTE);
	for (i=0; i<getsize(in)*in->veclen; i++)	{
		in->data[i] = c;
	}
}
void reset_field_s (AVSfield_short *in, const short c)	{
	unsigned long i;
	assert (in != NULL);
	assert (in->type == AVS_TYPE_SHORT);
	for (i=0; i<getsize(in)*in->veclen; i++)	{
		in->data[i] = c;
	}
}
// returns product of dimensions array
unsigned long getsize (const void *foo)	{
	const AVSfield *f = (const AVSfield *) foo;
	int i;
	unsigned long s = 1;
	for (i=0; i<f->ndim; i++)
		s *= f->dimensions[i];
	return s;
}
// returns 0 if ndim dimensions veclen or type are different, 1 otherwise
int compareFields (const void *foo, const void *boo)	{
	const AVSfield *f = (const AVSfield *) foo;
	const AVSfield *b = (const AVSfield *) boo;
	int i;

	if (f->ndim != b->ndim || f->veclen != b->veclen || f->type != b->type)
		return 0;
	for (i=0; i<f->ndim; i++)	{
		if (f->dimensions[i] != b->dimensions[i])
			return 0;
	}
	return 1;
}
// similar to compareFields but params instead of a second field
int sameSize (const void *foo, int ndim, int *dim, int veclen, int type)	{
	const AVSfield *f = (const AVSfield *) foo;
	int i;

	if (f->ndim != ndim || f->veclen != veclen || f->type != type)
		return 0;
	for (i=0; i<f->ndim; i++)	{
		if (f->dimensions[i] != dim[i])
			return 0;
	}
	return 1;
}
unsigned long INDV_index (AVSfield *in, int *x)	{
	int i;
	// gets the one-dimensional index to the n-dimensional input dataset
	unsigned long index = x[in->ndim-1];
	for (i=in->ndim-2; i>=0; i--)	{
		index *= in->dimensions[i];
		index += x[i];
	}
	return ( index );
}


#define check_null(fooo,f,l,v)				if(fooo == NULL)	{	\
		fprintf(stderr, RED "Error %s:%d> " NOC "%s is NULL pointer\nexiting\n",filename, line, var); \
		AVSerror ("Error %s:%d> %s is NULL pointer",filename, line, var);	\
		exit(1);	}
#define check_ptr_tree_null(fooo,f,l,v)				if(fooo->ptr_tree == NULL)	{	\
		fprintf(stderr, RED "Error %s:%d> " NOC "%s has NULL pointer for ptr_tree\nexiting\n",filename, line, var); \
		AVSerror ("Error %s:%d> %s has NULL pointer for ptr_tree\nexiting\n",filename, line, var); \
		exit(1);	}
#define check_data_null(fooo,f,l,v)				if(fooo->field_union.field_data_char == NULL)	{	\
		fprintf(stderr, RED "Error %s:%d> " NOC "%s has NULL pointer for data\nexiting\n",filename, line, var); \
		AVSerror ("Error %s:%d> %s has NULL pointer for data\nexiting\n",filename, line, var); \
		exit(1);	}
#define check_dim_null(fooo,f,l,v)				if(fooo->dimensions == NULL)	{	\
		fprintf(stderr, RED "Error %s:%d> " NOC "%s has NULL pointer for dimensions\nexiting\n",filename, line, var); \
		AVSerror ("Error %s:%d> %s has NULL pointer for dimensions\nexiting\n",filename, line, var); \
		exit(1);	}
#define check_ndim(fooo,nd,f,l,v)			if((fooo)->ndim != nd)	{	\
		fprintf(stderr, RED "Error %s:%d> " NOC  "The field '%s' is not being called by the correct 'get' function. (get%dv for a %d-D field)\n",f,l,v,nd,(fooo)->ndim);	\
		AVSerror ("Error %s:%d> The field '%s' is not being called by the correct 'get' function. (get%dv for a %d-D field)\n",f,l,v,nd,(fooo)->ndim);	\
		exit(1); 	}
#define check_dim(fooo,d,x,f,l,v)			if((fooo)->dimensions[d] <= x || x < 0)	{	\
		fprintf(stderr, RED "Error %s:%d> " NOC  "Dimension-%d of field:%s is out of bounds (%d) (limit %d)\nexiting\n",f,l,d,v,x,(fooo)->dimensions[d]);	\
		AVSerror ("Error %s:%d> Dimension-1 of %s is out of bounds %d vs %d\nexiting\n",f,l,v,x,(fooo)->dimensions[d]);	\
		exit(1); 	}
#define check_vec(fooo,i,f,l,v)				if((fooo)->veclen <= i || i < 0)	{	\
		fprintf(stderr, RED "Error %s:%d> " NOC  "Vector of field:%s is out of bounds (%d) (limit %d)\nexiting\n",f,l,v,(fooo)->veclen,i);	\
		AVSerror ("Error %s:%d> Vector of %s is out of bounds %d vs %d\nexiting\n",f,l,v,(fooo)->veclen,i);	\
		exit(1);	}


void *get_1Dv_(void *foo, int x, int i, const char *filename, int line, const char *var)	{// check for errors
	AVSfield *ffoo = (AVSfield *) foo;
	check_null(ffoo,filename,line,var);
	check_data_null(ffoo,filename,line,var);
	check_dim_null(ffoo,filename,line,var);
	check_ndim(ffoo,1,filename,line,var);
	check_dim(ffoo,0,x,filename,line,var);
	check_vec(ffoo,i,filename,line,var);

	//return the pointer
	switch(ffoo->type)	{
		case AVS_TYPE_BYTE:
			return( &(I1DV((AVSfield_char *) foo,x)[i])  );
		case AVS_TYPE_INTEGER:
			return( &(I1DV((AVSfield_int *) foo,x)[i])  );
		case AVS_TYPE_REAL:
			return( &(I1DV((AVSfield_float *) foo,x)[i])  );
		case AVS_TYPE_DOUBLE:
			return( &(I1DV((AVSfield_double *) foo,x)[i])  );
		case AVS_TYPE_SHORT:
			return( &(I1DV((AVSfield_short *) foo,x)[i])  );
		default:
			fprintf(stderr, RED "Error %s:%d> " NOC " type of %s is not set right\n", filename, line, var);
			exit(1);
	}
}
void *get_2Dv_(void *foo, int x, int y, int i, const char *filename, int line, const char *var)	{// check for errors
	AVSfield *ffoo = (AVSfield *) foo;
	check_null(ffoo,filename,line,var);
	check_data_null(ffoo,filename,line,var);
	check_dim_null(ffoo,filename,line,var);
	check_ndim(ffoo,2,filename,line,var);
	check_dim(ffoo,0,x,filename,line,var);
	check_dim(ffoo,1,y,filename,line,var);
	check_vec(ffoo,i,filename,line,var);

	//return the pointer
	switch(ffoo->type)	{
		case AVS_TYPE_BYTE:
			return( &(I2DV((AVSfield_char *) foo,x,y)[i])  );
		case AVS_TYPE_INTEGER:
			return( &(I2DV((AVSfield_int *) foo,x,y)[i])  );
		case AVS_TYPE_REAL:
			return( &(I2DV((AVSfield_float *) foo,x,y)[i])  );
		case AVS_TYPE_DOUBLE:
			return( &(I2DV((AVSfield_double *) foo,x,y)[i])  );
		case AVS_TYPE_SHORT:
			return( &(I2DV((AVSfield_short *) foo,x,y)[i])  );
		default:
			fprintf(stderr, RED "Error %s:%d> " NOC " type of %s is not set right\n", filename, line, var);
			exit(1);
	}
}
void *get_3Dv_(void *foo, int x, int y, int z, int i, const char *filename, int line, const char *var)	{// check for errors
	AVSfield *ffoo = (AVSfield *) foo;
	check_null(ffoo,filename,line,var);
	check_ndim(ffoo,3,filename,line,var);
	check_data_null(ffoo,filename,line,var);
	check_dim_null(ffoo,filename,line,var);
	check_dim(ffoo,0,x,filename,line,var);
	check_dim(ffoo,1,y,filename,line,var);
	check_dim(ffoo,2,z,filename,line,var);
	check_vec(ffoo,i,filename,line,var);

	//return the pointer
	switch(ffoo->type)	{
		case AVS_TYPE_BYTE:
			return( &(I3DV((AVSfield_char *) foo,x,y,z)[i])  );
		case AVS_TYPE_INTEGER:
			return( &(I3DV((AVSfield_int *) foo,x,y,z)[i])  );
		case AVS_TYPE_REAL:
			return( &(I3DV((AVSfield_float *) foo,x,y,z)[i])  );
		case AVS_TYPE_DOUBLE:
			return( &(I3DV((AVSfield_double *) foo,x,y,z)[i])  );
		case AVS_TYPE_SHORT:
			return( &(I3DV((AVSfield_short *) foo,x,y,z)[i])  );
		default:
			fprintf(stderr, RED "Error %s:%d> " NOC " type of %s is not set right\n", filename, line, var);
			exit(1);
	}
}
void *get_4Dv_(void *foo, int x1, int x2, int x3, int x4, int i, const char *filename, int line, const char *var)	{// check for errors
	AVSfield *ffoo = (AVSfield *) foo;
	check_null(ffoo,filename,line,var);
	check_ndim(ffoo,4,filename,line,var);
	check_data_null(ffoo,filename,line,var);
	check_dim_null(ffoo,filename,line,var);
	check_dim(ffoo,0,x1,filename,line,var);
	check_dim(ffoo,1,x2,filename,line,var);
	check_dim(ffoo,2,x3,filename,line,var);
	check_dim(ffoo,3,x4,filename,line,var);
	check_vec(ffoo,i,filename,line,var);

	//return the pointer
	switch(ffoo->type)	{
		case AVS_TYPE_BYTE:
			return( &(I4DV((AVSfield_char *) foo,x1,x2,x3,x4)[i])  );
		case AVS_TYPE_INTEGER:
			return( &(I4DV((AVSfield_int *) foo,x1,x2,x3,x4)[i])  );
		case AVS_TYPE_REAL:
			return( &(I4DV((AVSfield_float *) foo,x1,x2,x3,x4)[i])  );
		case AVS_TYPE_DOUBLE:
			return( &(I4DV((AVSfield_double *) foo,x1,x2,x3,x4)[i])  );
		case AVS_TYPE_SHORT:
			return( &(I4DV((AVSfield_short *) foo,x1,x2,x3,x4)[i])  );
		default:
			fprintf(stderr, RED "Error %s:%d> " NOC " type of %s is not set right\n", filename, line, var);
			exit(1);
	}
}

void *get_5Dv_(void *foo, int x1, int x2, int x3, int x4, int x5, int i, const char *filename, int line, const char *var)	{
	AVSfield *ffoo = (AVSfield *) foo;
	check_null(ffoo,filename,line,var);
	check_ndim(ffoo,5,filename,line,var);
	check_data_null(ffoo,filename,line,var);
	check_dim_null(ffoo,filename,line,var);
	check_dim(ffoo,0,x1,filename,line,var);
	check_dim(ffoo,1,x2,filename,line,var);
	check_dim(ffoo,2,x3,filename,line,var);
	check_dim(ffoo,3,x4,filename,line,var);
	check_dim(ffoo,4,x5,filename,line,var);
	check_vec(ffoo,i,filename,line,var);

	//return the pointer
	switch(ffoo->type)	{
		case AVS_TYPE_BYTE:
			return( &(I5DV((AVSfield_char *) foo,x1,x2,x3,x4,x5)[i])  );
		case AVS_TYPE_INTEGER:
			return( &(I5DV((AVSfield_int *) foo,x1,x2,x3,x4,x5)[i])  );
		case AVS_TYPE_REAL:
			return( &(I5DV((AVSfield_float *) foo,x1,x2,x3,x4,x5)[i])  );
		case AVS_TYPE_DOUBLE:
			return( &(I5DV((AVSfield_double *) foo,x1,x2,x3,x4,x5)[i])  );
		case AVS_TYPE_SHORT:
			return( &(I5DV((AVSfield_short *) foo,x1,x2,x3,x4,x5)[i])  );
		default:
			fprintf(stderr, RED "Error %s:%d> " NOC " type of %s is not set right\n", filename, line, var);
			exit(1);
	}
}
void *get_6Dv_(void *foo, int x1, int x2, int x3, int x4, int x5, int x6, int i, const char *filename, int line, const char *var)	{
	AVSfield *ffoo = (AVSfield *) foo;
	check_null(ffoo,filename,line,var);
	check_ndim(ffoo,6,filename,line,var);
	check_data_null(ffoo,filename,line,var);
	check_dim_null(ffoo,filename,line,var);
	check_dim(ffoo,0,x1,filename,line,var);
	check_dim(ffoo,1,x2,filename,line,var);
	check_dim(ffoo,2,x3,filename,line,var);
	check_dim(ffoo,3,x4,filename,line,var);
	check_dim(ffoo,4,x5,filename,line,var);
	check_dim(ffoo,5,x6,filename,line,var);
	check_vec(ffoo,i,filename,line,var);

	//return the pointer
	switch(ffoo->type)	{
		case AVS_TYPE_BYTE:
			return( &(I6DV((AVSfield_char *) foo,x1,x2,x3,x4,x5,x6)[i])  );
		case AVS_TYPE_INTEGER:
			return( &(I6DV((AVSfield_int *) foo,x1,x2,x3,x4,x5,x6)[i])  );
		case AVS_TYPE_REAL:
			return( &(I6DV((AVSfield_float *) foo,x1,x2,x3,x4,x5,x6)[i])  );
		case AVS_TYPE_DOUBLE:
			return( &(I6DV((AVSfield_double *) foo,x1,x2,x3,x4,x5,x6)[i])  );
		case AVS_TYPE_SHORT:
			return( &(I6DV((AVSfield_short *) foo,x1,x2,x3,x4,x5,x6)[i])  );
		default:
			fprintf(stderr, RED "Error %s:%d> " NOC " type of %s is not set right\n", filename, line, var);
			exit(1);
	}
}
void *get_NDv_(void *foo, int *x, int v, const char *filename, int line, const char *var)	{// check for errors
	int i;

	AVSfield *ffoo = (AVSfield*) foo;
	check_null(ffoo,filename,line,var);
	//check_ndim(ffoo,3,filename,line,var);
	if (x == NULL)	{
		fprintf(stderr, RED "Error %s:%d> " NOC " index array is NULL\nexiting!\n", filename, line);
		exit(1);
	}
	for (i=0; i<ffoo->ndim; i++)
		check_dim(ffoo,i,x[i],filename,line,var);
	check_vec(ffoo,v,filename,line,var);

	//return the pointer
	i = INDV_index (ffoo,x);
	switch(ffoo->type)	{
		case AVS_TYPE_BYTE:
			return( &(I1DV((AVSfield_char *) foo,i)[v])  );
			//return( INDV_pointer((AVSfield_char *) foo,x,i)  );
		case AVS_TYPE_INTEGER:
			return( &(I1DV((AVSfield_int *) foo,i)[v])  );
		case AVS_TYPE_REAL:
			return( &(I1DV((AVSfield_float *) foo,i)[v])  );
		case AVS_TYPE_DOUBLE:
			return( &(I1DV((AVSfield_double *) foo,i)[v])  );
		case AVS_TYPE_SHORT:
			return( &(I1DV((AVSfield_short *) foo,i)[v])  );
		default:
			fprintf(stderr, RED "Error %s:%d> " NOC " type of %s is not set right\n", filename, line, var);
			exit(1);
	}
}

void *get_as1Dv_(void *foo, unsigned long x, int i, const char *filename, int line, const char *var)	{// check for errors
	AVSfield *ffoo = (AVSfield *) foo;
	check_null(ffoo,filename,line,var);
	check_data_null(ffoo,filename,line,var);
	check_dim_null(ffoo,filename,line,var);
	//check_ndim(ffoo,1,filename,line,var);
	//check_dim(ffoo,0,x,filename,line,var);
	check_vec(ffoo,i,filename,line,var);
	//printf(RED"not entirely implemented\n"NOC);
	if(x < 0)
    {
    	fprintf(stderr, RED "Error %s:%d> " NOC "index of field:%s is out of bounds (index < 0). (%lu vs limit %lu)\nexiting\n",filename, line, var, x, getsize(foo));
		exit(1);
    }
      
    if(x >= getsize(foo))	
    {
		fprintf(stderr, RED "Error %s:%d> " NOC "index of field:%s is out of bounds (index >= getsize(%s)). (%lu vs limit %lu)\nexiting\n",filename, line, var, var, x, getsize(foo));
		exit(1);
	}

	//return the pointer
	switch(ffoo->type)	{
		case AVS_TYPE_BYTE:
			return( &(I1DV((AVSfield_char *) foo,x)[i])  );
		case AVS_TYPE_INTEGER:
			return( &(I1DV((AVSfield_int *) foo,x)[i])  );
		case AVS_TYPE_REAL:
			return( &(I1DV((AVSfield_float *) foo,x)[i])  );
		case AVS_TYPE_DOUBLE:
			return( &(I1DV((AVSfield_double *) foo,x)[i])  );
		case AVS_TYPE_SHORT:
			return( &(I1DV((AVSfield_short *) foo,x)[i])  );
		default:
			fprintf(stderr, RED "Error %s:%d> " NOC "getas1v(...): field is corrupt\n", filename, line);
			exit(1);
	}
}





#ifdef DO_AVS

AVSfield *new_yesAVS_field (int ndim, int *dim, int veclength, int type)	{
	AVSfield *tmp = NULL;
	int i;
	char field_str[100];

	// this checks the input
	// type must be: 0-4
	if (type < 0 || type > 4)	{
		fprintf (stderr, RED"new_AVS_float: type is wrong\n"NOC);
		AVSerror("new_AVS_float: type is wrong");
		exit(1);
}
	if (ndim <= 0)	{
		fprintf (stderr, RED"new_AVS_float: ndim (%d) is <= 0\n"NOC, ndim);
		AVSerror("new_AVS_float: ndim is <= 0");
		exit(1);
}
	if (dim == NULL)	{
		fprintf (stderr, RED"new_AVS_float: dimsions is NULL\n"NOC);
		AVSerror("new_AVS_float: dimsions is NULL");
		exit(1);
}
	for (i=0; i<ndim; i++)	{
		if (dim[i] <= 0 || dim[i] > INT_MAX)	{
			fprintf (stderr, RED"new_AVS_float: Dimsion-%d (%d) is out of bounds\n"NOC,i,dim[i]);
			AVSerror("new_AVS_float: dimsions is NULL");
			exit(1);
}
}

	// create template field
	switch (type)	{
		case AVS_TYPE_BYTE:
			sprintf(field_str,"field %dD %d-vector uniform byte",ndim, veclength);
			break;
		case AVS_TYPE_INTEGER:
			sprintf(field_str,"field %dD %d-vector uniform int",ndim, veclength);
			break;
		case AVS_TYPE_REAL:
			sprintf(field_str,"field %dD %d-vector uniform float",ndim, veclength);
			break;
		case AVS_TYPE_DOUBLE:
			sprintf(field_str,"field %dD %d-vector uniform double",ndim, veclength);
			break;
		case AVS_TYPE_SHORT:
			sprintf(field_str,"field %dD %d-vector uniform short",ndim, veclength);
			break;
		default:
			fprintf (stderr, RED"new_AVS_float: type is wrong\n"NOC);
			AVSerror("new_AVS_float: type is wrong");
			exit(1);
	}

	// allocate memory for new field
	tmp = (AVSfield *) AVSdata_alloc(field_str, dim);
    //AVSfield_invalid_minmax (tmp);
	assert (tmp != NULL);

	reset_field (tmp, 0.0); //should already be taken care of with AVS

	return tmp;
}


AVSfield_float *new_yesAVS_float (int ndim, int *dim, int veclength)	{
	return (AVSfield_float *) new_yesAVS_field(ndim,dim,veclength,AVS_TYPE_REAL);
}
AVSfield_double *new_yesAVS_double (int ndim, int *dim, int veclength)	{
	return (AVSfield_double *) new_yesAVS_field(ndim,dim,veclength,AVS_TYPE_DOUBLE);
}
AVSfield_int *new_yesAVS_int (int ndim, int *dim, int veclength)	{
	return (AVSfield_int *) new_yesAVS_field(ndim,dim,veclength,AVS_TYPE_INTEGER);
}
AVSfield_char *new_yesAVS_char (int ndim, int *dim, int veclength)	{
	return (AVSfield_char *) new_yesAVS_field(ndim,dim,veclength,AVS_TYPE_BYTE);
}
AVSfield_short *new_yesAVS_short (int ndim, int *dim, int veclength)	{
	return (AVSfield_short *) new_yesAVS_field(ndim,dim,veclength,AVS_TYPE_SHORT);
}


#else //ifdef DO_AVS
#define liberr()		fprintf(stderr, RED"You linked with the wrong library, try -lprojectX"NOC"\nexiting\n");exit(1)

AVSfield_float *new_yesAVS_float (int ndim, int *dims, int veclength)	{
	liberr();
	return NULL;
}
AVSfield_char *new_yesAVS_char (int ndim, int *dims, int veclength)	{
	liberr();
	return NULL;
}
AVSfield_double *new_yesAVS_double (int ndim, int *dims, int veclength)	{
	liberr();
	return NULL;
}
AVSfield_short *new_yesAVS_short (int ndim, int *dims, int veclength)	{
	liberr();
	return NULL;
}
AVSfield_int *new_yesAVS_int (int ndim, int *dims, int veclength)	{
	liberr();
	return NULL;
}

#endif // ifdef DO_AVS else






AVSfield *new_noAVS_field (int ndims, int *dim, int veclength, int type)	{
	AVSfield *tmp = NULL;
	int i;
	unsigned long array_length;

	// type must be: 0-4
	if (type < 0 || type > 4)	{
		fprintf (stderr, RED"new_noAVS_field: type is wrong\n"NOC);
        fprintf (stderr,"line:%d of projectX.c\n",__LINE__);
		exit(1);
	}
	if (ndims <= 0)	{
		fprintf (stderr, RED"new_noAVS_field: ndim (%d) is <= 0\n"NOC, ndims);
        fprintf (stderr,"line:%d of projectX.c\n",__LINE__);
		exit(1);
	}
	if (dim == NULL)	{
		fprintf (stderr, RED"new_noAVS_field: dimsions is NULL\n"NOC);
        fprintf (stderr,"line:%d of projectX.c\n",__LINE__);
		exit(1);
	}
	for (i=0; i<ndims; i++)	{
		if ((dim[i] <= 0) || (dim[i] > INT_MAX))	
        {
			fprintf (stderr, RED"new_noAVS_field: dimension[%d] == %d (out of bounds).\n"NOC,i,dim[i]);
            fprintf (stderr,"line:%d of projectX.c\n",__LINE__);
			exit(1);
		}
	}

	// allocate memory for struct AVSfield_float
	tmp = (AVSfield *) malloc (sizeof(AVSfield));
	assert (tmp != NULL);

	// fill in struct with necessary values
	tmp->ndim = ndims;
	tmp->veclen = veclength;
	tmp->type = type;
	//tmp->size = sizeof(float);
	switch (type)	{
		case AVS_TYPE_BYTE:
			tmp->size = sizeof(char);
			break;
		case AVS_TYPE_INTEGER:
			tmp->size = sizeof(int);
			break;
		case AVS_TYPE_REAL:
			tmp->size = sizeof(float);
			break;
		case AVS_TYPE_DOUBLE:
			tmp->size = sizeof(double);
			break;
		case AVS_TYPE_SHORT:
			tmp->size = sizeof(short);
			break;
		default:
			fprintf (stderr, RED"new_AVS_float: type is wrong\n"NOC);
			AVSerror("new_AVS_float: type is wrong");
			exit(1);
	}
	tmp->dimensions = (int *) malloc((ndims)* sizeof(int));
	assert (tmp->dimensions != NULL);
	for(i=0;i<ndims;i++)
		tmp->dimensions[i] = dim[i];

	array_length = veclength;
	for(i=0;i<ndims;i++)
		array_length *= dim[i];

	//tmp->field_data = malloc(array_length*tmp->size);
	//tmp->field_data = memalign(16, array_length*tmp->size); /// changed from malloc
	//tmp->field_data = valloc((unsigned long) (array_length*tmp->size)); /// changed from malloc
	tmp->field_data = (typeof(tmp->field_data)) calloc((unsigned long) array_length, tmp->size); /// changed from valloc
	/// \todo check if the compiler option "-malign-double" does the same during malloc
	assert (tmp->field_data != NULL);

	//reset_field (tmp, 0.0);

	return(tmp);
}
AVSfield_float *new_noAVS_float (int ndim, int *dim, int veclength)	{
	return (AVSfield_float *) new_noAVS_field(ndim,dim,veclength,AVS_TYPE_REAL);
}
AVSfield_double *new_noAVS_double (int ndim, int *dim, int veclength)	{
	return (AVSfield_double *) new_noAVS_field(ndim,dim,veclength,AVS_TYPE_DOUBLE);
}
AVSfield_int *new_noAVS_int (int ndim, int *dim, int veclength)	{
	return (AVSfield_int *) new_noAVS_field(ndim,dim,veclength,AVS_TYPE_INTEGER);
}
AVSfield_char *new_noAVS_char (int ndim, int *dim, int veclength)	{
	return (AVSfield_char *) new_noAVS_field(ndim,dim,veclength,AVS_TYPE_BYTE);
}
AVSfield_short *new_noAVS_short (int ndim, int *dim, int veclength)	{
	return (AVSfield_short *) new_noAVS_field(ndim,dim,veclength,AVS_TYPE_SHORT);
}



#define new1(NO_YES,TYPE)	AVSfield_ ## TYPE *new_ ## NO_YES ## AVS_ ## TYPE ## _1v (int N_1, int veclength)	{ \
	int *dims; \
	AVSfield_ ## TYPE *out; \
	dims = (int *) malloc (sizeof(int) * 1); \
	dims[0] = N_1; \
	out = new_ ## NO_YES ## AVS_ ## TYPE (1,dims,veclength); \
	free(dims); dims = NULL; \
	return out; \
}
new1(no,float)
new1(yes,float)
new1(no,short)
new1(yes,short)
new1(no,double)
new1(yes,double)
new1(no,char)
new1(yes,char)
new1(no,int)
new1(yes,int)


#define new2(NO_YES,TYPE)	AVSfield_ ## TYPE *new_ ## NO_YES ## AVS_ ## TYPE ## _2v \
	(int N_1, int N_2, int veclength)	{ \
	int *dims; \
	AVSfield_ ## TYPE *out; \
	dims = (int *) malloc (sizeof(int) * 2); \
	dims[0] = N_1;	\
	dims[1] = N_2;	\
	out =  new_ ## NO_YES ## AVS_ ## TYPE (2,dims,veclength); \
	free(dims); dims = NULL; \
	return out; \
}
#define new3(NO_YES,TYPE)	AVSfield_ ## TYPE *new_ ## NO_YES ## AVS_ ## TYPE ## _3v \
	(int N_1, int N_2, int N_3, int veclength)	{	\
	int *dims; \
	AVSfield_ ## TYPE *out; \
	dims = (int *) malloc (sizeof(int) * 3); \
	dims[0] = N_1;	\
	dims[1] = N_2;	\
	dims[2] = N_3;	\
	out =  new_ ## NO_YES ## AVS_ ## TYPE (3,dims,veclength); \
	free(dims); dims = NULL; \
	return out; \
}
#define new4(NO_YES,TYPE)	AVSfield_ ## TYPE *new_ ## NO_YES ## AVS_ ## TYPE ## _4v \
	(int N_1, int N_2, int N_3, int N_4, int veclength)	{	\
	int *dims; \
	AVSfield_ ## TYPE *out; \
	dims = (int *) malloc (sizeof(int) * 4); \
	dims[0] = N_1;	\
	dims[1] = N_2;	\
	dims[2] = N_3;	\
	dims[3] = N_4;	\
	out = new_ ## NO_YES ## AVS_ ## TYPE (4,dims,veclength); \
	free(dims); dims = NULL; \
	return out; \
}
#define new5(NO_YES,TYPE)	AVSfield_ ## TYPE *new_ ## NO_YES ## AVS_ ## TYPE ## _5v \
	(int N_1, int N_2, int N_3, int N_4, int N_5, int veclength)	{	\
	int *dims; \
	AVSfield_ ## TYPE *out; \
	dims = (int *) malloc (sizeof(int) * 5); \
	dims[0] = N_1;	\
	dims[1] = N_2;	\
	dims[2] = N_3;	\
	dims[3] = N_4;	\
	dims[4] = N_5;	\
	out = new_ ## NO_YES ## AVS_ ## TYPE (5,dims,veclength); \
	free(dims); dims = NULL; \
	return out; \
}
#define new6(NO_YES,TYPE)	AVSfield_ ## TYPE *new_ ## NO_YES ## AVS_ ## TYPE ## _6v \
	(int N_1, int N_2, int N_3, int N_4, int N_5, int N_6, int veclength)	{	\
	int *dims; \
	AVSfield_ ## TYPE *out; \
	dims = (int *) malloc (sizeof(int) * 6); \
	dims[0] = N_1;	\
	dims[1] = N_2;	\
	dims[2] = N_3;	\
	dims[3] = N_4;	\
	dims[4] = N_5;	\
	dims[5] = N_6;	\
	out = new_ ## NO_YES ## AVS_ ## TYPE (6,dims,veclength); \
	free(dims); dims = NULL; \
	return out; \
}

new2(no,float)
new2(yes,float)
new2(no,short)
new2(yes,short)
new2(no,double)
new2(yes,double)
new2(no,char)
new2(yes,char)
new2(no,int)
new2(yes,int)



new3(no,float)
new3(yes,float)
new3(no,short)
new3(yes,short)
new3(no,double)
new3(yes,double)
new3(no,char)
new3(yes,char)
new3(no,int)
new3(yes,int)


new4(no,float)
new4(yes,float)
new4(no,short)
new4(yes,short)
new4(no,double)
new4(yes,double)
new4(no,char)
new4(yes,char)
new4(no,int)
new4(yes,int)



new5(no,float)
new5(yes,float)
new5(no,short)
new5(yes,short)
new5(no,double)
new5(yes,double)
new5(no,char)
new5(yes,char)
new5(no,int)
new5(yes,int)


new6(no,float)
new6(yes,float)
new6(no,short)
new6(yes,short)
new6(no,double)
new6(yes,double)
new6(no,char)
new6(yes,char)
new6(no,int)
new6(yes,int)



void noAVSfield_free (AVSfield *foo)	{
	// check if the field has been allocated
	if (NULL == foo)	{
		fprintf (stderr, RED"AVSfield_free: field already deleted\n"NOC);
		return;
	}

	// delete data
	switch (foo->type)	{
		case AVS_TYPE_BYTE:
			noAVSfield_free_char((AVSfield_char *) foo);
			break;
		case AVS_TYPE_REAL:
			noAVSfield_free_float((AVSfield_float *) foo);
			break;
		case AVS_TYPE_DOUBLE:
			noAVSfield_free_double((AVSfield_double *) foo);
			break;
		case AVS_TYPE_INTEGER:
			noAVSfield_free_int((AVSfield_int *) foo);
			break;
		case AVS_TYPE_SHORT:
			noAVSfield_free_short((AVSfield_short *) foo);
			break;
		default:
			fprintf (stderr, RED"AVSfield_free: type error: undefined type\n"NOC);
	}

	// delete dimensions and insert invalid values that would be caught with DEBUG_FIELD
	foo->dimensions[0] = -1;
	free (foo->dimensions); foo->dimensions = NULL;
	foo->type = -2343;
	foo->size = -1243;
	foo->veclen = -68;

	// delete field struct
	free(foo);
}
void noAVSfield_free_float (AVSfield_float *foo)	{
	free (foo->data); foo->data = NULL;
}
void noAVSfield_free_char (AVSfield_char *foo)	{
	free (foo->data); foo->data = NULL;
}
void noAVSfield_free_double (AVSfield_double *foo)	{
	free (foo->data); foo->data = NULL;
}
void noAVSfield_free_int (AVSfield_int *foo)	{
	free (foo->data); foo->data = NULL;
}
void noAVSfield_free_short (AVSfield_short *foo)	{
	free (foo->data); foo->data = NULL;
}


