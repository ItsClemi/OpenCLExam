#include "stdafx.h"
#include "OpenCLKernel.h"

using namespace std;

COpenCLKernel::COpenCLKernel( )
{

}

COpenCLKernel::~COpenCLKernel( )
{
	if( m_clKernel )
	{
		clReleaseKernel( m_clKernel );
	}

}

void COpenCLKernel::CreateKernel( const std::wstring& szKernelName )
{
	const auto szKernel = WstringToString( szKernelName );
	const auto clProgram = GetCLManager( )->GetProgram( );

	m_szKernelName = szKernelName;

	cl_int status;
	m_clKernel = clCreateKernel( clProgram, szKernel.c_str( ), &status );
	if( !CL_SUCCEEDED( status ) )
	{
		ostringstream os;
		{
			os << __FUNCTION__ << " Error: creating kernel " << szKernel << " " << std::hex << status << std::endl;
		}
		throw runtime_error( os.str( ) );
	}


// 	size_t bin_sz;
// 	status = clGetProgramInfo( clProgram, CL_PROGRAM_BINARY_SIZES, sizeof( size_t ), &bin_sz, NULL );
// 
// 	unsigned char *bin = ( unsigned char * )malloc( bin_sz );
// 	status = clGetProgramInfo( clProgram, CL_PROGRAM_BINARIES, sizeof( unsigned char * ), &bin, NULL );
// 
// 
// 	__debugbreak( );

}
