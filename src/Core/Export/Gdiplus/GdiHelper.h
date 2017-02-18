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

struct __declspec( novtable ) SBitmapData
{
	~SBitmapData( )
	{
		SafeDeleteArray( m_pBuffer );
	}


	size_t		m_nWidth;
	size_t		m_nHeight;
	size_t		m_nStride;

	size_t		m_nPixelCount;
	size_t		m_nBuffLen;
	SPixel*		m_pBuffer;
};

void StartupGdiplus( );


const std::shared_ptr< SBitmapData > GetBitmapData( const wchar_t* szPath );