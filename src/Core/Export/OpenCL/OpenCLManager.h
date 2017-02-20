#pragma once

#ifndef CL_SUCCEEDED
#define CL_SUCCEEDED( x )  ( x == CL_SUCCESS )
#endif


class COpenCLKernel;
class COpenCLMangager
{
	COpenCLMangager( );

public:
	~COpenCLMangager( );

public:
	static COpenCLMangager* GetInstance( );

public:
	void Initialize( );
	void LoadFile( const std::wstring& szPath );

	const std::shared_ptr< COpenCLKernel > CreateKernel( const std::wstring& szKernel );

	void InitializeKernel( const std::wstring& szKernel );

private:
	void BuildKernel( const std::vector< char >& vecProgram );

	const std::string	GetBuildErrorMsg( );


public:
	const cl_context GetContext( ) const
	{
		return m_clContext;
	}

	const cl_command_queue GetCommandQueue( ) const
	{
		return m_clCommandQueue;
	}

	const cl_device_id GetTargetDevice( ) const
	{
		return reinterpret_cast< cl_device_id >( m_clDevices[ m_nDeviceNo ] );
	}

	const cl_device_id* GetTargetDevicesPtr( ) const
	{
		return reinterpret_cast< cl_device_id* >( m_clDevices + m_nDeviceNo ); // ?
	}

	inline cl_program GetProgram( ) const
	{
		return m_clProgram;
	}
	
	const bool IsGPUAvailable( ) const
	{
		return m_bUseGPU;
	}

public:
	std::shared_ptr< COpenCLKernel > GetKernelByName( std::wstring szKernelName ) const;


private:
	cl_uint				m_nDeviceNo = 1;
	cl_device_id*		m_clDevices = nullptr;

	cl_platform_id*		m_clPlatformBuffer = nullptr;

	cl_context			m_clContext = nullptr;
	cl_command_queue	m_clCommandQueue = nullptr;
	cl_program			m_clProgram = nullptr;


	bool				m_bUseGPU = false;

	std::vector< std::shared_ptr< COpenCLKernel > >	 m_vecKernels;

};




inline COpenCLMangager* GetCLManager( )
{
	return COpenCLMangager::GetInstance( );
}