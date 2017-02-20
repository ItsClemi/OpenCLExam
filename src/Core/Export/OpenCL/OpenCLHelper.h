#pragma once



namespace cl
{
	namespace
	{
		inline void clThrowIfFailed( cl_int status )
		{
			if( status != CL_SUCCESS )
			{
// 				std::ostringstream os;
// 				{
// 					os << "cl failed " << status;
// 				}
				throw std::runtime_error( "cl failed" );
			}
		}
	}


// 	inline void clSetKernelArg( 
// 		cl_kernel kernel,
// 		cl_uint arg_index,
// 		size_t  arg_size,
// 		const void* arg_value 
// 	)
// 	{
// 		clThrowIfFailed( 
// 			clSetKernelArg( kernel, arg_index, arg_size, arg_value )
// 		);
// 	}


}