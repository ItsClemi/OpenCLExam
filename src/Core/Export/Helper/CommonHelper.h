#pragma once


template< typename T >
void SafeDelete( T& t )
{
	if( t )
	{
		delete( t );
		t = nullptr;
	}
}

template< typename T >
void SafeDeleteArray( T& t )
{
	if( t )
	{
		delete[ ] t;
		t = nullptr;
	}
}

template< typename T >
inline void SafeRelease( T** pPtr )
{
	if( *pPtr )
	{
		( *pPtr )->Release( );
		( *pPtr ) = nullptr;
	}
}

inline std::wstring StringToWstring( const std::string& str )
{
	std::wstring_convert< std::codecvt_utf8< wchar_t >, wchar_t > conv;

	return conv.from_bytes( str );
}

inline std::string WstringToString( const std::wstring& str )
{
	std::wstring_convert< std::codecvt_utf8< wchar_t >, wchar_t > conv;

	return conv.to_bytes( str );
}

namespace Profiler
{
	//std::chrono::microseconds
	template< typename TM, typename _Fn, typename... _Args >
	inline const auto Measure( _Fn&& _Fx, _Args&&... _Ax )
	{
		auto begin = std::chrono::high_resolution_clock::now( );
		{
			_Fx( std::forward< _Args >( _Ax )... );
		}
		auto end = std::chrono::high_resolution_clock::now( );

		return std::chrono::duration_cast< TM >( end - begin ).count( );
	}
};


//////////////////////////////////////////////////////////////////////////
//> see: https://software.intel.com/en-us/articles/fast-random-number-generator-on-the-intel-pentiumr-4-processor

static unsigned int g_seed;


//Used to seed the generator.

inline void fast_srand( int seed )
{
	g_seed = seed;
}

//fastrand routine returns one integer, similar output value range as C lib.

inline int fastrand( )
{
	g_seed = ( 214013 * g_seed + 2531011 );

	return ( g_seed >> 12 );
}


inline int myRand( int low, int high )
{
	return fastrand( ) % ( high - low + 1 ) + low;
}