#pragma once

#include <gdiplus.h>

//> ARGB -> BGRA (Endianness)
struct SPixel
{
	uint8_t blue;
	uint8_t green;
	uint8_t red;
	uint8_t alpha;
};


class CBitmapData : public Gdiplus::Bitmap
{
public:
	CBitmapData( const std::wstring& szPath )
		: Gdiplus::Bitmap( szPath.c_str( ) )
	{
		if( GetLastStatus( ) != Gdiplus::Status::Ok )
		{
			throw std::runtime_error( "failed to load bitmap" );
		}
	}

	~CBitmapData( )
	{
		SafeDeleteArray( m_pBuffer );
	}

public:
	inline void InitializeBitmap( ) noexcept
	{
		m_nWidth = static_cast< size_t >( GetWidth( ) );
		m_nHeight = static_cast< size_t >( GetHeight( ) );

		m_nPixelCount = m_nWidth * m_nHeight;

		LockBitmapBits( );
	}

	inline void LockBitmapBits( )
	{
		Gdiplus::Rect rcLock(
			0,
			0,
			static_cast< INT >( m_nWidth ),
			static_cast< INT >( m_nHeight )
		);

		if( LockBits( &rcLock, Gdiplus::ImageLockModeRead, PixelFormat32bppRGB, m_pBitmapData.get( ) ) != Gdiplus::Status::Ok )
		{
			throw std::runtime_error( "failed to lock bitmap!" );
		}

		m_nStride = static_cast< size_t >( m_pBitmapData->Stride );
		m_nBuffLen = m_nWidth * m_nHeight * sizeof( SPixel );

		m_pBuffer = new SPixel[ m_nBuffLen ];
		memcpy_s( m_pBuffer, m_nBuffLen, m_pBitmapData->Scan0, m_nBuffLen );
	}

	void UnlockBitmapBits( )
	{
		if( UnlockBits( m_pBitmapData.get( ) ) != Gdiplus::Status::Ok )
		{
			throw std::runtime_error( "failed to unlock bitmap!" );
		}

		//m_pBitmapData.reset( nullptr );
	}

 	void CopyInternalBufferToBitmap( )
 	{
		memcpy_s( m_pBitmapData->Scan0, m_nBuffLen, m_pBuffer, m_nBuffLen );
	}

public:
	size_t		m_nWidth;
	size_t		m_nHeight;
	size_t		m_nStride;

	size_t		m_nPixelCount;
	size_t		m_nBuffLen;
	SPixel*		m_pBuffer;



	std::unique_ptr< Gdiplus::BitmapData >	m_pBitmapData = std::make_unique< Gdiplus::BitmapData >( );
};

typedef CBitmapData SBitmapData;



void StartupGdiplus( );


const std::shared_ptr< SBitmapData > GetBitmapData( const wchar_t* szPath );

const std::shared_ptr< SBitmapData > GetBitmapDataLocked( const std::wstring& szPath );

void UnlockBitmap( const std::shared_ptr< SBitmapData > pBitmap );


//> MSDN: https://msdn.microsoft.com/en-us/library/windows/desktop/ms533843(v=vs.85).aspx
inline int GetEncoderClsid( const WCHAR* format, CLSID* pClsid )
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

	Gdiplus::GetImageEncodersSize( &num, &size );
	if( size == 0 )
		return -1;  // Failure

	pImageCodecInfo = ( Gdiplus::ImageCodecInfo* )( malloc( size ) );
	if( pImageCodecInfo == NULL )
		return -1;  // Failure

	Gdiplus::GetImageEncoders( num, size, pImageCodecInfo );

	for( UINT j = 0; j < num; ++j )
	{
		if( wcscmp( pImageCodecInfo[ j ].MimeType, format ) == 0 )
		{
			*pClsid = pImageCodecInfo[ j ].Clsid;
			free( pImageCodecInfo );
			return j;  // Success
		}
	}

	free( pImageCodecInfo );
	return -1;  // Failure
}
