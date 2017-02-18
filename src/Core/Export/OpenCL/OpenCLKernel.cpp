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

void COpenCLKernel::CreateKernel( const std::string& szKernel )
{
	const auto clProgram = GetCLManager( )->GetProgram( );

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

}
