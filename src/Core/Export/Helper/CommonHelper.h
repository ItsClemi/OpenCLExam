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


inline const std::wstring ImageOpenDlg( )
{
	OPENFILENAMEW ofn;       // common dialog box structure
	wchar_t szFile[ 260 ];       // buffer for file name

							// Initialize OPENFILENAME
	ZeroMemory( &ofn, sizeof( ofn ) );
	ofn.lStructSize = sizeof( ofn );
	ofn.lpstrFile = &szFile[ 0 ];
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
	// use the contents of szFile to initialize itself.
	ofn.lpstrFile[ 0 ] = '\0';
	ofn.nMaxFile = sizeof( szFile );
	ofn.lpstrFilter = L"All\0*.*\0jpg\0*.JPG\0jpeg\0*.JPEG\0Bitmap\0*.BMP\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// Display the Open dialog box. 

	if( !GetOpenFileName( &ofn ) == TRUE )
	{
		return L"";
	}

	WCHAR szPath[ MAX_PATH + 1 ] = { 0 };

	DWORD retval = GetFullPathNameW( ofn.lpstrFile, MAX_PATH, szPath, NULL );
	if( ( retval == 0 ) || ( retval > MAX_PATH ) )
		return L"";

	std::wstring wstrPath( szPath, retval );
	std::wcout << wstrPath << std::endl;
	return wstrPath;
}

inline const std::wstring ImageSaveDlg( )
{
	OPENFILENAMEW ofn;       // common dialog box structure
	wchar_t szFile[ 260 ];       // buffer for file name

								 // Initialize OPENFILENAME
	ZeroMemory( &ofn, sizeof( ofn ) );
	ofn.lStructSize = sizeof( ofn );
	ofn.lpstrFile = &szFile[ 0 ];
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
	// use the contents of szFile to initialize itself.
	ofn.lpstrFile[ 0 ] = '\0';
	ofn.nMaxFile = sizeof( szFile );
	ofn.lpstrFilter = L"png\0*.PNG\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// Display the Open dialog box. 

	if( !GetSaveFileNameW( &ofn ) == TRUE )
	{
		return L"";
	}

	WCHAR szPath[ MAX_PATH + 1 ] = { 0 };

	DWORD retval = GetFullPathNameW( ofn.lpstrFile, MAX_PATH, szPath, NULL );
	if( ( retval == 0 ) || ( retval > MAX_PATH ) )
		return L"";

	std::wstring wstrPath( szPath, retval );
	std::wcout << wstrPath << std::endl;
	return wstrPath;
}
