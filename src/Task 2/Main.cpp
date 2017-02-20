#include "stdafx.h"
#include "PrefixSum.h"

using namespace std;

void CalcPrefix_CPU( const int* pNumbers, int* pResult, size_t nLength )
{
	pResult[ 0 ] = 0;

	for( size_t i = 1; i < nLength; i++ )
	{
		pResult[ i ] = pNumbers[ i - 1 ] + pResult[ i - 1 ];
	}
}

bool CalcPrefix_Check( size_t nLength, bool bRandom = true )
{
	static const int arrNumbers[ ] = { 3, 2, 1, 2, 1, 4, 3, 2, 4, 3, };

	int* pNumbers = new int[ nLength ];
	int* pResultCPU = new int[ nLength ];
	int* pResultGPU = new int[ nLength ];


	std::wcout << L"Prep array" << std::endl;
	if( bRandom )
	{
		for( size_t i = 0; i < nLength; i++ )
		{
			pNumbers[ i ] = myRand( 1, 100 );
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

	std::wcout << L"Calc Prefix (" << nLength << L")" << std::endl;
	const auto tmCPU = Profiler::Measure< chrono::milliseconds >( [ & ]
	{
		CalcPrefix_CPU( pNumbers, pResultCPU, nLength );
	} );

	std::wcout << L"\tCPU Calc " << tmCPU << L"ms" << std::endl;
	const auto tmGPU = Profiler::Measure< chrono::milliseconds >( [ & ]
	{
		CalcPrefix_GPU( pNumbers, pResultGPU, nLength );
	} );

	std::wcout << L"\tGPU Calc " << tmGPU << L"ms" << std::endl;

	bool bResult = memcmp( pResultCPU, pResultGPU, nLength * sizeof( int ) ) == 0;
	{
		SafeDeleteArray( pNumbers );
		SafeDeleteArray( pResultCPU );
		SafeDeleteArray( pResultGPU );
	}
	return bResult;
}

int wmain( int argc, wchar_t* argv[ ], wchar_t* envp[ ] )
{
#ifdef _DEBUG
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF );
#endif

	std::wcout << L"Task 2.0 (Prefix sum)" << std::endl;

	srand( GetTickCount( ) );
	fast_srand( GetTickCount( ) );

	try
	{
		GetCLManager( )->LoadFile( L"PrefixSum.cl" );

		GetCLManager( )->InitializeKernel( L"calcPrefix256" );
		GetCLManager( )->InitializeKernel( L"finalize" );
	}
	catch( const std::exception& e )
	{
		std::cout << e.what( ) << std::endl;

		system( "pause" );
		return -1;
	}

#ifdef _DEBUG
	//> Tests
 	assert( CalcPrefix_Check( 10, false ) );
 	assert( CalcPrefix_Check( 256 ) );
 	assert( CalcPrefix_Check( 1024 ) );
 	assert( CalcPrefix_Check( 256 * 256 * 2 ) );
	assert( CalcPrefix_Check( 256 * 256 * 256 * 16 ) );

 	assert( CalcPrefix_Check( 117 ) );
 	assert( CalcPrefix_Check( 800 ) );
 	assert( CalcPrefix_Check( 1000 ) );
 	assert( CalcPrefix_Check( 5000 ) );
 	assert( CalcPrefix_Check( 25000 ) );
 
 	for( ;; )
 	{
 		assert( CalcPrefix_Check( myRand( 25, 256 * 256 * 256 * 8 ) ) );
 	}

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

