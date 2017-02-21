#include "stdafx.h"

#include <chrono>
#include <memory>

using namespace std;


void CalcHistogram_CPU( const shared_ptr< SBitmapData > pData, array< size_t, 256 >& arrHistogram )
{
	std::wcout << L"Performing CPU calc" << std::endl;

	const auto tm = Profiler::Measure< chrono::milliseconds >( [ & ]
	{
		for( size_t i = 0; i < pData->m_nPixelCount; i++ )
		{
			const auto pPixel = &pData->m_pBuffer[ i ];

			double I =
				0.299 * static_cast< double >( pPixel->red ) +
				0.587 * static_cast< double >( pPixel->green ) +
				0.114 * static_cast< double >( pPixel->blue )
				;

			arrHistogram[ static_cast< size_t >( I ) ]++;
		}
	} );

	std::wcout
		<< L"CPU done in "
		<< tm << L" ms"
		<< std::endl << std::endl;
}

void CalcHistogram_GPU(
	const shared_ptr< SBitmapData > pData,

	const shared_ptr< COpenCLKernel > pStatistic,
	const shared_ptr< COpenCLKernel > pReduce,

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
	cl_int* pResultBuffer = new cl_int[ nr_workgroups * 256 ];
	memset( pResultBuffer, 0, sizeof( cl_int ) * nr_workgroups * 256 );


	cl_int status = 0;
	cl_mem clPixelBuffer = clCreateBuffer( clContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, pData->m_nBuffLen, pData->m_pBuffer, &status );
	cl_mem clResultBuffer = clCreateBuffer( clContext, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof( cl_int ) * nr_workgroups * 256, pResultBuffer, &status );
	
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

	cl_int nClWorkGroups = static_cast< cl_int >( nr_workgroups );
	status = clSetKernelArg( pReduce->GetKernel( ), 0, sizeof( cl_mem ), &clResultBuffer );
	status = clSetKernelArg( pReduce->GetKernel( ), 1, sizeof( cl_int ), &nClWorkGroups );

	if( !CL_SUCCEEDED( status ) )
	{
		std::wcout << __FUNCTIONW__ L" failed to set kernel args(reduce) " << std::hex << std::endl;
		return;
	}

	auto tmReduce = Profiler::Measure< std::chrono::microseconds >( [ & ]( )
	{
		size_t global_work_size[ 1 ] = { 256 };
		size_t local_work_size[ 1 ] = { 256 };

		status = clEnqueueNDRangeKernel( clCommandQueue, pReduce->GetKernel( ), 1, NULL, global_work_size, local_work_size, 0, NULL, NULL );
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


	std::wcout << L"GPU calc done in " << tmStatistics + tmReduce << L" microseconds" << std::endl << std::endl;
}


int wmain( int argc, wchar_t* argv[ ], wchar_t* envp[ ] )
{
	std::wcout << L"Task 1.0 (Histogram)" << std::endl;


	shared_ptr< COpenCLKernel >	pStatistic;
	shared_ptr< COpenCLKernel >	pReduce;

	shared_ptr< SBitmapData > pBitmap;

	try
	{
		GetCLManager( )->LoadFile( L"Histogram.cl" );
		pStatistic = GetCLManager( )->CreateKernel( L"calcStatistic" );
		pReduce = GetCLManager( )->CreateKernel( L"reduceStatistic" );


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


	array< size_t, 256 > arrHistogramCPU = { };
	CalcHistogram_CPU( pBitmap, arrHistogramCPU );


	array< cl_int, 256 > arrHistogramGPU = { };
	CalcHistogram_GPU( pBitmap, pStatistic, pReduce, arrHistogramGPU );


	std::wcout
		<< L"Dump:" << std::endl
		<< L"CPU" << '\t' << '\t' << L"GPU" << std::endl;

	for( size_t i = 0; i < 256; i++ )
	{
		std::wcout << i << L":\t" << arrHistogramCPU[ i ] << '\t' << '\t' << arrHistogramGPU[ i ] << std::endl;
	}


	size_t nCPUSum = 0;
	cl_int nGPUSum = 0;

	for( size_t i = 0; i < 256; i++ )
	{
		nCPUSum += arrHistogramCPU[ i ];
		nGPUSum += arrHistogramGPU[ i ];
	}

	std::wcout << std::endl
		<< L"Sum" << std::endl
		<< L"CPU: " << nCPUSum << L" GPU: " << nGPUSum
		<< std::endl
		;


	system( "pause" );
	return 0;
}

