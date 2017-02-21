#include "stdafx.h"
#include "PrefixSum.h"

void CalcPrefix_PreRec( const cl_mem clBuffA, cl_mem clBuffB, size_t nAlignedLength )
{
	const auto clContext = GetCLManager( )->GetContext( );
	const auto clCommandQueue = GetCLManager( )->GetCommandQueue( );

	const size_t nHelperLength = ( ( nAlignedLength / 256 ) + 255 ) / 256 * 256;

	cl_int status = 0;

	cl_mem clBuffC = clCreateBuffer( clContext, CL_MEM_READ_WRITE, nHelperLength * sizeof( cl_int ), nullptr, &status );
	cl_mem clBuffD = clCreateBuffer( clContext, CL_MEM_READ_WRITE, nHelperLength * sizeof( cl_int ), nullptr, &status );

	if( !CL_SUCCEEDED( status ) )
	{
		throw std::runtime_error( "clCreateBuffer failed" );
	}

	const auto pKernel = GetCLManager( )->GetKernelByName( L"calcPrefix256" );
	const auto pKernelFinalize = GetCLManager( )->GetKernelByName( L"finalize" );


	//> First pass to create field B
	status |= clSetKernelArg( pKernel->GetKernel( ), 0, sizeof( cl_mem ), &clBuffA );
	status |= clSetKernelArg( pKernel->GetKernel( ), 1, sizeof( cl_mem ), &clBuffB );
	status |= clSetKernelArg( pKernel->GetKernel( ), 2, sizeof( cl_mem ), &clBuffC );

	if( !CL_SUCCEEDED( status ) )
	{
		throw std::runtime_error( "clSetKernelArg failed" );
	}

	const size_t global_work_size[ 1 ] = { nAlignedLength };
	const size_t local_work_size[ 1 ] = { 256 };

	// Running the kernel.
	status = clEnqueueNDRangeKernel( clCommandQueue, pKernel->GetKernel( ), 1, NULL, global_work_size, local_work_size, 0, NULL, NULL );

	if( !CL_SUCCEEDED( status ) )
	{
		throw std::runtime_error( "clEnqueueNDRangeKernel failed" );
	}

	if( nAlignedLength > 256 )
	{
		//> create field D
		CalcPrefix_PreRec( clBuffC, clBuffD, nHelperLength );

		//> add all elements from D to B
		status |= clSetKernelArg( pKernelFinalize->GetKernel( ), 0, sizeof( cl_mem ), &clBuffB );
		status |= clSetKernelArg( pKernelFinalize->GetKernel( ), 1, sizeof( cl_mem ), &clBuffD );

		if( !CL_SUCCEEDED( status ) )
		{
			throw std::runtime_error( "clSetKernelArg (2) failed" );
		}

		// Running the kernel.
		const size_t global_work_size[ 1 ] = { nAlignedLength };
		const size_t local_work_size[ 1 ] = { 256 };
		status = clEnqueueNDRangeKernel( clCommandQueue, pKernelFinalize->GetKernel( ), 1, NULL, global_work_size, local_work_size, 0, NULL, NULL );

		if( !CL_SUCCEEDED( status ) )
		{
			throw std::runtime_error( "clEnqueueNDRangeKernel (2) failed" );
		}
	}

	status = clReleaseMemObject( clBuffD );
	status = clReleaseMemObject( clBuffC );

	if( !CL_SUCCEEDED( status ) )
	{
		throw std::runtime_error( "failed to release memory object" );
	}
}

void CalcPrefix_GPU( const int* pNumbers, int* pResult, size_t nLength )
{
	const auto clContext = GetCLManager( )->GetContext( );
	const auto clQueue = GetCLManager( )->GetCommandQueue( );

	const size_t nAlignedLength = ( nLength + 255 ) / 256 * 256;

	cl_int status = 0;

	cl_mem clBuffA = clCreateBuffer( clContext, CL_MEM_READ_ONLY, nAlignedLength * sizeof( cl_int ), nullptr, &status );
	cl_mem clBuffB = clCreateBuffer( clContext, CL_MEM_READ_WRITE, nAlignedLength * sizeof( cl_int ), nullptr, &status );

	if( !CL_SUCCEEDED( status ) )
	{
		throw std::runtime_error( "clCreateBuffer failed" );
	}

	status = clEnqueueWriteBuffer( 
		clQueue, 
		clBuffA,
		CL_TRUE, 
		0, 
		nLength * sizeof( cl_int ), 
		const_cast< cl_int* >( pNumbers ),
		0,
		nullptr,
		nullptr
	);

	if( !CL_SUCCEEDED( status ) )
	{
		throw std::runtime_error( "clEnqueueWriteBuffer failed" );
	}

	//> Recursive calc
	CalcPrefix_PreRec( clBuffA, clBuffB, nAlignedLength );


	//> Read Result
	status = clEnqueueReadBuffer( clQueue, clBuffB, CL_TRUE, 0, nLength * sizeof( int ), pResult, 0, nullptr, nullptr );

	if( !CL_SUCCEEDED( status ) )
	{
		std::wcout << __FUNCTIONW__ << " Error: copying data from output buffer to host" << std::endl;
		return;
	}

	status = clReleaseMemObject( clBuffB );
	status = clReleaseMemObject( clBuffA );

	if( !CL_SUCCEEDED( status ) )
	{
		throw std::runtime_error( "failed to release memory object" );
	}
}