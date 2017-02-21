#include "stdafx.h"
#include "GdiHelper.h"

using namespace std;
using namespace Gdiplus;

ULONG_PTR s_gdiplusToken = 0;


void StartupGdiplus( )
{
	GdiplusStartupInput gdiplusStartupInput;
	if( GdiplusStartup( &s_gdiplusToken, &gdiplusStartupInput, NULL ) != Status::Ok )
	{
		throw std::runtime_error( "failed to initialize gdi+" );
	}

	atexit( [ ]
	{
		GdiplusShutdown( s_gdiplusToken );
	} );
}

const std::shared_ptr< SBitmapData > GetBitmapData( const wchar_t* szPath )
{
	if( s_gdiplusToken == 0 )
	{
		StartupGdiplus( );
	}

	std::wcout << L"Loading Image " << szPath << std::endl;

	const auto pData = make_shared< SBitmapData >( szPath );
	{
		pData->InitializeBitmap( );
		pData->UnlockBitmapBits( );
	}

	std::wcout
		<< L"Loaded Image " << szPath
		<< L" Stride " << pData->m_nStride
		<< L" Dim: " << pData->m_nWidth << L" x " << pData->m_nHeight
		<< std::endl;

	return pData;
}

const std::shared_ptr< SBitmapData > GetBitmapDataLocked( const std::wstring& szPath )
{
	if( s_gdiplusToken == 0 )
	{
		StartupGdiplus( );
	}

	std::wcout << L"Loading Image " << szPath << std::endl;

	const auto pData = make_shared< SBitmapData >( szPath );
	{
		pData->InitializeBitmap( );
	}

	std::wcout
		<< L"Loaded Image " << szPath
		<< L" Stride " << pData->m_nStride
		<< L" Dim: " << pData->m_nWidth << L" x " << pData->m_nHeight
		<< std::endl;

	return pData;
}

void UnlockBitmap( const std::shared_ptr< SBitmapData > pBitmap )
{
	pBitmap->UnlockBitmapBits( );
}