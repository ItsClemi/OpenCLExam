#pragma once

class COpenCLKernel
{
public:
	COpenCLKernel( );
	~COpenCLKernel( );

public:
	void CreateKernel( const std::string& szKernel );

public:
	const auto GetContext( ) const
	{
		return GetCLManager()->GetContext( );
	}

	const auto GetCommandQueue( ) const
	{
		return GetCLManager( )->GetCommandQueue( );
	}

	const auto GetKernel( ) const
	{
		return m_clKernel;
	}


private:

	cl_kernel		m_clKernel = nullptr;
};