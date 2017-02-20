#include "stdafx.h"
#include "PrefixSum.h"

using namespace std;

void CalcPrefix_CPU( const int* pNumbers, int* pResult, size_t nLength )
{
	std::wcout << L"Calc Prefix CPU (" << nLength << L")" << std::endl;

	const auto tm = Profiler::Measure< chrono::milliseconds >( [ & ]
	{
		pResult[ 0 ] = 0;
		for( size_t i = 1; i < nLength; i++ )
		{
			pResult[ i ] = pNumbers[ i - 1 ] + pResult[ i - 1 ];
		}
	} );

	std::wcout << L"CPU done in " << tm << L"ms" << std::endl;
}

bool CalcPrefix_Check( size_t nLength, bool bRandom = true )
{
	static const int arrNumbers[ ] = { 3, 2, 1, 2, 1, 4, 3, 2, 4, 3, };

	int* pNumbers = new int[ nLength ];
	int* pResultCPU = new int[ nLength ];
	int* pResultGPU = new int[ nLength ];

	if( bRandom )
	{
		for( size_t i = 0; i < nLength; i++ )
		{
			pNumbers[ i ] = 1;//myRand( 1, 100 );
		}
	}
 	else
 	{
 		memcpy_s( 
 			&pNumbers[ 0 ], 
 			ARRAYSIZE( arrNumbers ) * sizeof( int ), 
 			&arrNumbers[ 0 ], 
 			ARRAYSIZE( arrNumbers ) * sizeof( int ) 
 		);
 	}

	CalcPrefix_CPU( pNumbers, pResultCPU, nLength );
	CalcPrefix_GPU( pNumbers, pResultGPU, nLength );

	for( size_t i = 0; i < nLength; i++ )
	{
		int nGPU = pResultGPU[ i ];
		int nCPU = pResultCPU[ i ];

		if( nGPU != nCPU )
		{
			__debugbreak( );
		}
	}

	//bool bResult = memcmp( pResultCPU, pResultGPU, nLength * sizeof( int ) ) == 0;
	{
		SafeDeleteArray( pNumbers );
		SafeDeleteArray( pResultCPU );
		SafeDeleteArray( pResultGPU );
	}
	//return bResult;

	return true;
}



int wmain( int argc, wchar_t* argv[ ], wchar_t* envp[ ] )
{
#ifdef _DEBUG
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF );
#endif

	std::wcout << L"Task 2.0 (Prefix sum)" << std::endl;

	srand( GetTickCount( ) );

	try
	{
		GetCLManager( )->LoadFile( L"PrefixSum.cl" );

		GetCLManager( )->InitializeKernel( L"calcPrefix256" );
		GetCLManager( )->InitializeKernel( L"finalize" );

		//GetCLManager( )->InitializeKernel( L"calcPrefix2562" );
	}
	catch( const std::exception& e )
	{
		std::cout << e.what( ) << std::endl;

		system( "pause" );
		return -1;
	}

#ifdef _DEBUG
	//> Tests
//  	assert( CalcPrefix_Check( 10, false ) );
//  	assert( CalcPrefix_Check( 256 ) );
//  	

//   	for( ;; )
//   	{
//   		assert( CalcPrefix_Check( myRand( 25, 256 * 200 ) ) );
//   	}

//	__debugbreak( );
//
//	assert( CalcPrefix_Check( 1024 ) );
	assert( CalcPrefix_Check( 256 * 256 * 2 ) );
	assert( CalcPrefix_Check( 256 * 256 * 256 * 32 ) );

	assert( CalcPrefix_Check( 117 ) );
	assert( CalcPrefix_Check( 800 ) );
	assert( CalcPrefix_Check( 1000 ) );
	assert( CalcPrefix_Check( 5000 ) );
	assert( CalcPrefix_Check( 25000 ) );

	assert( CalcPrefix_Check( 256 * 256 * 256 * 90 ) );
#else
	size_t nLength = 2048;
	std::cout << L"Enter Field length " << std::endl;
	std::cin >> nLength;

	bool bResult = CalcPrefix_Check( nLength );

	std::cout << bResult ? "true" : "false" << std::endl;

#endif
	system( "pause" );

	return 0;
}

