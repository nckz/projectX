/**	example.c
	Author: Ken Johnson
	created: Thur June 2, 2005
	see /usr/include/projectX_interface.h for a list of functions
*/

#include <avs/avs.h>
#include <avs/port.h>
#include <avs/field.h>

#define DEBUG_field
#include <projectX.h>

.
.
.
	in_port = AVScreate_input_port("input",
		"field 2D 1-vector uniform float", REQUIRED);
	out_port = AVScreate_output_port("output",
		"field 2D 2-vector uniform float");
.
.
.

int compute(AVSfield_float *input, AVSfield_float **out)	{
	int x,y,i;

	/*if (*out != NULL)
		AVS_free(*out);
	*out = new_AVS_float (input->ndim, input->dimensions, 2);
	//*out=new_AVS_float_2v(input->dimensions[0],input->dimensions[1],2);
	 */
	if (*out == NULL)
		*out = new_AVS_field (input->ndim, input->dimensions,2, AVS_TYPE_REAL);
	else if (!sameSize(*out, input->ndim, input->dimensions,2, AVS_TYPE_REAL))	{
	//OR: else if (!compareFields(*out, input)) use this if input is the same size+type as desired out
		AVS_free(*out);
		*out = new_AVS_field (input->ndim, input->dimensions,2, input->type);
	}

	for (x=0; x<input->dimensions[0]; x++)	{
		for (y=0; y<input->dimensions[1]; y++)	{
			get2v(*out, x, y, 0) = cos ( get2(input, x, y) );
			get2v(*out, x, y, 1) = sin ( get2v(input, x, y, 0) );
		}
	}
	//for (i=0; i<getsize(input); i++)	{
	//	getas1v(*out, i, 0) = cos ( getas1v(input, i, 0) );
	//	getas1v(*out, i, 1) = sin ( getas1v(input, i, 0) );
	//}
	return 1;
}