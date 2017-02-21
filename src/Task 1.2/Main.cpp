#include "stdafx.h"

#include <gdiplus.h>
#include <chrono>
#include <memory>

using namespace std;


void CalcHistogram_GPU(
	const shared_ptr< SBitmapData > pData,

	const shared_ptr< COpenCLKernel > pStatistic,
	array< cl_int, 256 >& arrHistogram
)
{
	std::wcout << L"Performing GPU calc" << std::endl;

	const auto clContext = GetCLManager( )->GetContext( );
	const auto clCommandQueue = GetCLManager( )->GetCommandQueue( );


	//size_t global_worksize = ( pData->m_nPixelCount + 8191 ) / 8192 * 8192 / 256 
	size_t global_worksize = ( ( pData->m_nPixelCount + 8191 ) / 8192 ) * 32;
	size_t nr_workgroups = global_worksize / 32; 	//> num of real active kernels


	//> Create a cleared placeholder buffer 
	cl_int* pResultBuffer = new cl_int[ 256 ];
	memset( pResultBuffer, 0, sizeof( cl_int ) * 256 );
	

	cl_int status = 0;
	cl_mem clPixelBuffer = clCreateBuffer( clContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, pData->m_nBuffLen, pData->m_pBuffer, &status );
	cl_mem clResultBuffer = clCreateBuffer( clContext, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof( cl_int ) * 256, pResultBuffer, &status );

	if( !CL_SUCCEEDED( status ) )
	{
		std::wcout << __FUNCTIONW__ L" failed with " << std::hex << status << std::endl;
		return;
	}

	//> type cast
	cl_int nPixelCount = static_cast< cl_int >( pData->m_nPixelCount );

	status = clSetKernelArg( pStatistic->GetKernel( ), 0, sizeof( cl_mem ), &clPixelBuffer );
	status = clSetKernelArg( pStatistic->GetKernel( ), 1, sizeof( cl_int ), &nPixelCount );
	status = clSetKernelArg( pStatistic->GetKernel( ), 2, sizeof( cl_mem ), &clResultBuffer );

	if( !CL_SUCCEEDED( status ) )
	{
		std::wcout << __FUNCTIONW__ L" failed to set kernel args " << std::hex << std::endl;
		return;
	}

	auto tmStatistics = Profiler::Measure< std::chrono::microseconds >( [ & ]( )
	{
		size_t global_work_size[ 1 ] = { global_worksize };
		size_t local_work_size[ 1 ] = { 32 };

		status = clEnqueueNDRangeKernel( clCommandQueue, pStatistic->GetKernel( ), 1, NULL, global_work_size, local_work_size, 0, NULL, NULL );
		if( !CL_SUCCEEDED( status ) )
		{
			std::cout << __FUNCTION__ << " Error: enqueuing kernel! " << std::hex << status << std::endl;
			return;
		}

		status = clFinish( clCommandQueue );
	} );

	status = clEnqueueReadBuffer( clCommandQueue, clResultBuffer, CL_TRUE, 0, sizeof( cl_int ) * 256, &arrHistogram[ 0 ], 0, NULL, NULL );

	if( !CL_SUCCEEDED( status ) )
	{
		std::cout << __FUNCTION__ << " failed to read buffer " << std::hex << status << std::endl;
		return;
	}

	clReleaseMemObject( clPixelBuffer );
	clReleaseMemObject( clResultBuffer );

	SafeDeleteArray( pResultBuffer );


	std::wcout << L"GPU calc done in " << tmStatistics << L" microseconds" << std::endl << std::endl;
}


int wmain( int argc, wchar_t* argv[ ], wchar_t* envp[ ] )
{
	std::wcout << L"Task 1.2 (Histogram)" << std::endl;


	shared_ptr< COpenCLKernel >	pStatistic;

	shared_ptr< SBitmapData > pBitmap;

	try
	{
		GetCLManager( )->LoadFile( L"Histogram.cl" );
		pStatistic = GetCLManager( )->CreateKernel( L"calcStatistic" );

#ifdef _DEBUG
		pBitmap = GetBitmapData( L"C:\\Users\\clemi\\Desktop\\1711533.png" );
#else
		pBitmap = GetBitmapData( ImageOpenDlg( ).c_str( ) );
#endif
	}
	catch( const std::exception& e )
	{
		std::cout << e.what( ) << std::endl;

		system( "pause" );
		return -1;
	}


	array< cl_int, 256 > arrHistogramGPU = { };
	CalcHistogram_GPU( pBitmap, pStatistic, arrHistogramGPU );


	std::wcout
		<< L"Dump:" << std::endl
		<< '\t' << L"GPU" << std::endl;

	for( size_t i = 0; i < 256; i++ )
	{
		std::wcout << i << L":\t" << arrHistogramGPU[ i ] << std::endl;
	}


	cl_int nGPUSum = 0;

	for( size_t i = 0; i < 256; i++ )
	{
		nGPUSum += arrHistogramGPU[ i ];
	}

	std::wcout << std::endl
		<< L"Sum" << std::endl
		<< L" GPU: " << nGPUSum
		<< std::endl
		;


	system( "pause" );
	return 0;
}

