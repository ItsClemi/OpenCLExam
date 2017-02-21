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
}

void BlendY( shared_ptr< SBitmapData > pBitmap )
{
	const auto clContext = GetCLManager( )->GetContext( );
	const auto clCommandQueue = GetCLManager( )->GetCommandQueue( );

	const auto pKernel = GetCLManager( )->GetKernelByName( L"yblend" );


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

	cl_int nWidth = static_cast< cl_int >( pBitmap->m_nWidth );
	cl_int nHeight = static_cast< cl_int >( pBitmap->m_nHeight );


	status = clSetKernelArg( pKernel->GetKernel( ), 0, sizeof( cl_mem ), &clBuffA );
	status = clSetKernelArg( pKernel->GetKernel( ), 1, sizeof( cl_mem ), &clBuffB );
	status = clSetKernelArg( pKernel->GetKernel( ), 2, sizeof( cl_int ), &nWidth );
	status = clSetKernelArg( pKernel->GetKernel( ), 3, sizeof( cl_int ), &nHeight );


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
}


int wmain( int argc, wchar_t* argv[ ], wchar_t* envp[ ] )
{
	std::wcout << L"Task 3.0 (Image Blend)" << std::endl;



	shared_ptr< SBitmapData > pBitmap;

	try
	{
		GetCLManager( )->LoadFile( L"Blend.cl" );

#ifdef _DEBUG
		static const std::wstring szPath = L"C:\\Users\\clemi\\Desktop\\1711533.png";
		static const std::wstring szPath2 = L"C:\\Users\\clemi\\Desktop\\lul.png";
		static const std::wstring szPath3 = L"C:\\Users\\clemi\\Desktop\\small.png";
		static const std::wstring szPath4 = L"C:\\Users\\clemi\\Desktop\\Bg_Tree.jpg";

		pBitmap = GetBitmapDataLocked( szPath.c_str( ) );

#else
		pBitmap = GetBitmapDataLocked( ImageOpenDlg( ).c_str( ) );
#endif


		GetCLManager( )->InitializeKernel( L"xblend" );
		GetCLManager( )->InitializeKernel( L"yblend" );

	}
	catch( const std::exception& e )
	{
		std::cout << e.what( ) << std::endl;

		system( "pause" );
		return -1;
	}


	BlendX( pBitmap );
	BlendY( pBitmap );



	pBitmap->CopyInternalBufferToBitmap( );

	pBitmap->UnlockBitmapBits( );

	Gdiplus::Bitmap pImage( pBitmap->GetWidth( ), pBitmap->GetHeight( ) );
	{
		Gdiplus::Graphics graphics( &pImage );
		graphics.Clear( Gdiplus::Color::White );

		graphics.DrawImage( pBitmap.get( ), 0, 0, ( INT )pBitmap->GetWidth( ), ( INT )pBitmap->GetHeight( ) );
	}

	CLSID pngClsid;
	GetEncoderClsid( L"image/png", &pngClsid );

	const auto szPath = ImageSaveDlg( );

	if( pImage.Save( szPath.c_str( ), &pngClsid, nullptr ) != Gdiplus::Status::Ok )
	{
		std::wcout << L"Failed to write image to " << szPath << std::endl;
	}
	else
	{
		std::wcout << L"Invalid path " << std::endl;
	}

	system( "pause" );

	return 0;
}

