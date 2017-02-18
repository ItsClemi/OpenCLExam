#include "stdafx.h"

#include <gdiplus.h>
#include <chrono>
#include <memory>

using namespace std;
using namespace Gdiplus;


void CalcHistogram_CPU( const shared_ptr< SBitmapData > pData, array< size_t, 256 >& arrHistogram )
{
	std::wcout << L"Performing CPU calc" << std::endl;

	const auto tm = Profiler::Measure< chrono::milliseconds >( [ & ]
	{
		for( size_t i = 0; i < pData->m_nBuffLen; i++ )
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
		<< std::endl;
}

void CalcHistogram_GPU( 
	const shared_ptr< SBitmapData > pData,

	const shared_ptr< COpenCLKernel > pStatistic,
	const shared_ptr< COpenCLKernel > pReduce,

	array< size_t, 256 >& arrHistogram
	)
{
	std::wcout << L"Performing GPU calc" << std::endl;

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

		pBitmap = GetBitmapData( L"C:\\Users\\clemi\\Desktop\\1711533.png" );
	}
	catch( const std::exception& e )
	{
		std::cout << e.what( ) << std::endl;

		system( "pause" );
		return -1;
	}


	array< size_t, 256 > arrHistogramCPU = { };
	CalcHistogram_CPU( pBitmap, arrHistogramCPU );


	array< size_t, 256 > arrHistogramGPU = { };
	CalcHistogram_GPU( pBitmap, pStatistic, pReduce, arrHistogramGPU );



	for( const auto i : arrHistogramCPU )
	{
		printf( "%I64d\n", i );
	}


	system( "pause" );
	return 0;
}

