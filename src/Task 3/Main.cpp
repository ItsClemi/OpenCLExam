#include "stdafx.h"

using namespace std;


void BlendX( shared_ptr< SBitmapData > pBitmap )
{
	const auto clContext = GetCLManager( )->GetContext( );
	const auto clCommandQueue = GetCLManager( )->GetCommandQueue( );

	const auto pKernel = GetCLManager( )->GetKernelByName( L"xblend" );


	size_t nAlignedPixels = ( ( ( pBitmap->m_nPixelCount ) + 255 ) / 256 ) * 256;

	cl_int status = 0;

	cl_mem clBuffA = clCreateBuffer( clContext, CL_MEM_READ_ONLY, nAlignedPixels * sizeof( cl_uchar4 ), nullptr, &status );
	cl_mem clBuffB = clCreateBuffer( clContext, CL_MEM_READ_WRITE, nAlignedPixels * sizeof( cl_uchar4 ), nullptr, &status );

	status = clEnqueueWriteBuffer(
		clCommandQueue,
		clBuffA,
		CL_TRUE,
		0,
		pBitmap->m_nBuffLen,
		pBitmap->m_pBuffer,
		0,
		nullptr,
		nullptr
	);

	cl_int nStride = static_cast< cl_int >( pBitmap->m_nStride / sizeof( SPixel ) );

	status = clSetKernelArg( pKernel->GetKernel( ), 0, sizeof( cl_mem ), &clBuffA );
	status = clSetKernelArg( pKernel->GetKernel( ), 1, sizeof( cl_mem ), &clBuffB );
	status = clSetKernelArg( pKernel->GetKernel( ), 2, sizeof( cl_int ), &nStride );


	size_t global_work_size[ 1 ] = { nAlignedPixels };
	size_t local_work_size[ 1 ] = { 256 };

	status = clEnqueueNDRangeKernel( clCommandQueue, pKernel->GetKernel( ), 1, NULL, global_work_size, local_work_size, 0, NULL, NULL );


	status = clEnqueueReadBuffer(
		clCommandQueue,
		clBuffB,
		CL_TRUE,
		0,
		pBitmap->m_nBuffLen,
		pBitmap->m_pBuffer,
		0,
		nullptr,
		nullptr
	);

	pBitmap->CopyInternalBufferToBitmap( );

	pBitmap->UnlockBitmapBits( );


	Gdiplus::Bitmap pImage( pBitmap->GetWidth( ), pBitmap->GetHeight( ) );
	{
		Gdiplus::Graphics graphics( &pImage );
		graphics.Clear( Gdiplus::Color::White );

		graphics.DrawImage( pBitmap.get( ), 0, 0, (INT)pBitmap->GetWidth( ), (INT)pBitmap->GetHeight( ) );
	}

	//graphics.Save( );


	CLSID pngClsid;
	GetEncoderClsid( L"image/png", &pngClsid );

	if( pImage.Save( L"C:\\Users\\clemi\\Desktop\\222.png", &pngClsid, nullptr ) != Gdiplus::Status::Ok )
	{
		__debugbreak( );
	}


}

int wmain( int argc, wchar_t* argv[ ], wchar_t* envp[ ] )
{
	std::wcout << L"Task 3.0 (Image Blend)" << std::endl;



	shared_ptr< SBitmapData > pBitmap;

	try
	{
		static const std::wstring szPath = L"C:\\Users\\clemi\\Desktop\\1711533.png";
		static const std::wstring szPath2 = L"C:\\Users\\clemi\\Desktop\\lul.png";
		static const std::wstring szPath3 = L"C:\\Users\\clemi\\Desktop\\small.png";
		static const std::wstring szPath4 = L"C:\\Users\\clemi\\Desktop\\Bg_Tree.jpg";


		GetCLManager( )->LoadFile( L"Blend.cl" );

		pBitmap = GetBitmapDataLocked( szPath2 );

		GetCLManager( )->InitializeKernel( L"xblend" );
	}
	catch( const std::exception& e )
	{
		std::cout << e.what( ) << std::endl;

		system( "pause" );
		return -1;
	}

	BlendX( pBitmap );










	return 0;
}

