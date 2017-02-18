#pragma once


class COpenCLKernel;
class COpenCLMangager
{
	COpenCLMangager( );

public:
	~COpenCLMangager( );

public:
	void Initialize( );
	void LoadFile( const std::wstring& szStr );

	const std::shared_ptr< COpenCLKernel > CreateKernel( const std::wstring& szKernel );


public:
	static COpenCLMangager* GetInstance( );


};


inline COpenCLMangager* GetCLManager( )
{
	return COpenCLMangager::GetInstance( );
}