#include "stdafx.h"
#include "OpenCLManager.h"
#include "OpenCLKernel.h"


COpenCLMangager::COpenCLMangager( )
{

}

COpenCLMangager::~COpenCLMangager( )
{

}

void COpenCLMangager::Initialize( )
{

}

void COpenCLMangager::LoadFile( const std::wstring& szStr )
{

}

const std::shared_ptr< COpenCLKernel > COpenCLMangager::CreateKernel( const std::wstring& szKernel )
{
	return nullptr;
}

COpenCLMangager* COpenCLMangager::GetInstance( )
{
	static COpenCLMangager mgr;

	return &mgr;
}

