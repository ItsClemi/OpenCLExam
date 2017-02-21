#pragma once

//////////////////////////////////////////////////////////////////////////
//=> Includes

#include <CL/cl.h>

#include <objidl.h>
#include <gdiplus.h>

#include <Shlwapi.h>
#include <Commdlg.h>

#include <memory>
#include <codecvt>
#include <iostream>
#include <string>
#include <vector>
#include <array>


//Own
#include <Core/Export/Helper/CommonHelper.h>

#include <Core/Export/OpenCL/OpenCLManager.h>
#include <Core/Export/OpenCL/OpenCLKernel.h>

#include <Core/Export/Gdiplus/GdiHelper.h>




//////////////////////////////////////////////////////////////////////////
//=> Lib
#pragma comment( lib, "OpenCL.lib" )
#pragma comment( lib, "Gdiplus.lib" )
#pragma comment( lib, "Shlwapi.lib" )