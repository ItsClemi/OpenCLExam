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
