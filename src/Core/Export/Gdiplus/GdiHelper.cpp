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

	Bitmap* pGdiBitmap = new Bitmap( szPath );
	if( pGdiBitmap->GetLastStatus( ) != Status::Ok )
	{
		throw runtime_error( "failed to load bitmap" );
	}

	const auto pData = make_shared< SBitmapData >( );
	{
		pData->m_nWidth = static_cast< size_t >( pGdiBitmap->GetWidth( ) );
		pData->m_nHeight = static_cast< size_t >( pGdiBitmap->GetHeight( ) );

		pData->m_nPixelCount = pData->m_nWidth* pData->m_nHeight;
	}

	Rect rcLock( 
		0, 
		0, 
		static_cast< INT >( pData->m_nWidth ),
		static_cast< INT >( pData->m_nHeight )
	);

	BitmapData* pBitmapData = new BitmapData( );

	if( pGdiBitmap->LockBits( &rcLock, ImageLockModeRead, PixelFormat32bppRGB, pBitmapData ) != Status::Ok )
	{
		throw runtime_error( "failed to lock bitmap!" );
	}

	//> Copy Buffer
	{
		pData->m_nStride = static_cast< size_t >( pBitmapData->Stride );

		pData->m_nBuffLen = pData->m_nWidth * pData->m_nHeight * sizeof( SPixel );

		pData->m_pBuffer = new SPixel[ pData->m_nBuffLen ];
		memcpy_s( pData->m_pBuffer, pData->m_nBuffLen, pBitmapData->Scan0, pData->m_nBuffLen );
	}

	std::wcout
		<< L"Loaded Image " << szPath
		<< L" Stride " << pData->m_nStride
		<< L" Dim: " << pData->m_nWidth << L" x " << pData->m_nHeight
		<< std::endl;


	if( pGdiBitmap->UnlockBits( pBitmapData ) != Status::Ok )
	{
		throw runtime_error( "failed to unlock bitmap!" );
	}

	SafeDelete( pBitmapData );
	SafeDelete( pGdiBitmap );

	return pData;
}

