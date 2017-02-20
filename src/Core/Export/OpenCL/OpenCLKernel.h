#pragma once

class COpenCLKernel
{
public:
	COpenCLKernel( );
	~COpenCLKernel( );

public:
	void CreateKernel( const std::wstring& szKernelName );

public:
	const auto GetKernel( ) const
	{
		return m_clKernel;
	}

	const auto& GetName( ) const
	{
		return m_szKernelName;
	}

private:
	cl_kernel		m_clKernel = nullptr;

	std::wstring	m_szKernelName;

};