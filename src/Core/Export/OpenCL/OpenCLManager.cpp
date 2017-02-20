#include "stdafx.h"
#include "OpenCLManager.h"
#include "OpenCLKernel.h"

#include <fstream>

using namespace std;

COpenCLMangager::COpenCLMangager( )
{
	Initialize( );
}

COpenCLMangager::~COpenCLMangager( )
{
	m_vecKernels.clear( );

	if( m_clProgram )
	{
		clReleaseProgram( m_clProgram );
	}

	if( m_clCommandQueue )
	{
		clReleaseCommandQueue( m_clCommandQueue );
	}

	if( m_clContext )
	{
		clReleaseContext( m_clContext );
	}

	SafeDeleteArray( m_clPlatformBuffer );
	SafeDeleteArray( m_clDevices );
}

COpenCLMangager* COpenCLMangager::GetInstance( )
{
	static COpenCLMangager mgr;

	return &mgr;
}

void COpenCLMangager::Initialize( )
{
	// Getting platforms and choose an available one.
	cl_uint numPlatforms;

	cl_int status = clGetPlatformIDs( 0, nullptr, &numPlatforms );

	if( !CL_SUCCEEDED( status ) )
	{
		ostringstream os;
		{
			os << __FUNCTION__ << " Error: Getting platforms!";
		}
		throw runtime_error( os.str( ) );
	}

	cl_platform_id platform = nullptr;
	if( numPlatforms > 0 )
	{
		m_clPlatformBuffer = new cl_platform_id[ numPlatforms ];

		status = clGetPlatformIDs( numPlatforms, m_clPlatformBuffer, nullptr );

		if( !CL_SUCCEEDED( status ) )
		{
			ostringstream os;
			{
				os << __FUNCTION__ << " Error: Getting platforms ids " << std::hex << status;
			}
			throw runtime_error( os.str( ) );
		}

		platform = m_clPlatformBuffer[ 0 ];
	}

	// Query devices and choose a GPU device if has one. Otherwise use the CPU as device.
	cl_uint	numDevices = 0;
	status = clGetDeviceIDs( platform, CL_DEVICE_TYPE_GPU, 0, nullptr, &numDevices );

	if( !CL_SUCCEEDED( status ) )
	{
		ostringstream os;
		{
			os << __FUNCTION__ << " Error: Getting device ids " << std::hex << status;
		}
		throw runtime_error( os.str( ) );
	}


	if( numDevices == 0 )
	{
		std::cout << "No GPU device available." << std::endl;
		std::cout << "Choose CPU as default device." << std::endl;

		status = clGetDeviceIDs( platform, CL_DEVICE_TYPE_CPU, 0, nullptr, &numDevices );
		if( !CL_SUCCEEDED( status ) )
		{
			ostringstream os;
			{
				os << __FUNCTION__ << " Error: Getting number of cpu devices " << std::hex << status;
			}
			throw runtime_error( os.str( ) );
		}

		m_clDevices = new cl_device_id[ numDevices ];

		status = clGetDeviceIDs( platform, CL_DEVICE_TYPE_CPU, numDevices, m_clDevices, nullptr );
		if( !CL_SUCCEEDED( status ) )
		{
			ostringstream os;
			{
				os << __FUNCTION__ << " Error: Getting cpu device id " << std::hex << status;
			}
			throw runtime_error( os.str( ) );
		}
	}
	else
	{
		m_clDevices = new cl_device_id[ numDevices ];

		status = clGetDeviceIDs( platform, CL_DEVICE_TYPE_GPU, numDevices, m_clDevices, nullptr );
		if( !CL_SUCCEEDED( status ) )
		{
			ostringstream os;
			{
				os << __FUNCTION__ << " Error: Getting gpu device id " << std::hex << status;
			}
			throw runtime_error( os.str( ) );
		}

		char dname[ 501 ] = { };

		clGetDeviceInfo( m_clDevices[ 0 ], CL_DEVICE_NAME, 500, dname, NULL );
		printf( "Device name = %s\n", dname );

		m_bUseGPU = true;
	}

	if( m_nDeviceNo >= numDevices )
	{
		m_nDeviceNo = 0;
	}

	// Create context
	m_clContext = clCreateContext( nullptr, 1, m_clDevices, nullptr, nullptr, nullptr );
	if( !CL_SUCCEEDED( status ) )
	{
		ostringstream os;
		{
			os << __FUNCTION__ << " Error: creating OpenCL context " << std::hex << status;
		}
		throw runtime_error( os.str( ) );
	}

	m_clCommandQueue = clCreateCommandQueue( m_clContext, m_clDevices[ m_nDeviceNo ], CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE | CL_QUEUE_PROFILING_ENABLE, &status );
	if( !CL_SUCCEEDED( status ) )
	{
		ostringstream os;
		{
			os << __FUNCTION__ << " Error: creating command queue " << std::hex << status;
		}
		throw runtime_error( os.str( ) );
	}

}

void COpenCLMangager::LoadFile( const std::wstring& szPath )
{
	const auto szFile = WstringToString( szPath );

	std::fstream f( szFile, std::fstream::in | std::fstream::binary );

	if( !f.is_open( ) )
	{
		ostringstream os;
		{
			os << __FUNCTION__ << " failed to open file " << szFile;
		}
		throw runtime_error( os.str( ) );
	}

	f.seekg( 0, std::fstream::end );
	auto size = f.tellg( );
	f.seekg( 0, std::fstream::beg );

	vector< char > vecBuffer( size.seekpos( ) + 1 );
	{
		f.read( &vecBuffer[ 0 ], size );
	}
	vecBuffer[ size ] = '\0';


	BuildKernel( vecBuffer );
}

const std::shared_ptr< COpenCLKernel > COpenCLMangager::CreateKernel( const std::wstring& szKernel )
{
	const auto pKernel = make_shared< COpenCLKernel >( );
	{
		pKernel->CreateKernel( szKernel );
	}
	m_vecKernels.push_back( pKernel );

	return pKernel;
}

void COpenCLMangager::InitializeKernel( const std::wstring& szKernel )
{
	const auto pKernel = make_shared< COpenCLKernel >( );
	{
		pKernel->CreateKernel( szKernel );
	}
	m_vecKernels.push_back( pKernel );
}

void COpenCLMangager::BuildKernel( const std::vector< char >& vecProgram )
{
	cl_int status;
	const char* szBuffer = &vecProgram[ 0 ];
	const size_t nBufferSize = { vecProgram.size( ) - 1 };

	m_clProgram = clCreateProgramWithSource( GetContext( ), 1, &szBuffer, &nBufferSize, &status );
	if( !CL_SUCCEEDED( status ) )
	{
		ostringstream os;
		{
			os << __FUNCTION__ << " Error: creating OpenCL program " << std::hex << status;
		}
		throw runtime_error( os.str( ) );
	}

	status = clBuildProgram( m_clProgram, 1, GetTargetDevicesPtr( ), nullptr, nullptr, nullptr );
	if( !CL_SUCCEEDED( status ) )
	{
		ostringstream os;
		{
			os << __FUNCTION__ << " === build failed ===" << std::endl
				<< " Status: " << std::hex << status << std::endl
				<< GetBuildErrorMsg( );
			;
		}
		throw runtime_error( os.str( ) );		
	}

}

const std::string COpenCLMangager::GetBuildErrorMsg( )
{
	cl_int status;

	std::string szError;

	size_t nStrLen;
	status = clGetProgramBuildInfo( GetProgram( ), const_cast< cl_device_id >( GetTargetDevice( ) ), CL_PROGRAM_BUILD_LOG, 0, nullptr, &nStrLen );
	if( !CL_SUCCEEDED( status ) )
	{
		std::cout << __FUNCTION__ << " failed to get clGetProgramBuildInfo length status: " << std::hex << status << std::endl;
		return std::string( );
	}

	szError.resize( nStrLen + 1 );

	status = clGetProgramBuildInfo( m_clProgram, GetTargetDevice( ), CL_PROGRAM_BUILD_LOG, szError.length( ), const_cast< char* >( szError.c_str( ) ), nullptr );
	if( !CL_SUCCEEDED( status ) )
	{
		std::cout << __FUNCTION__ << " failed to get clGetProgramBuildInfo " << std::hex << status << std::endl;
		return std::string( );
	}

	return szError;
}

std::shared_ptr< COpenCLKernel > COpenCLMangager::GetKernelByName( std::wstring szKernelName ) const
{
	for( const auto& i : m_vecKernels )
	{
		if( i->GetName( ).compare( szKernelName ) == 0 )
		{
			return i;
		}
	}

	return std::shared_ptr< COpenCLKernel >( nullptr );
}

