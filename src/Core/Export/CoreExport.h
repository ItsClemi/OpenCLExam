#pragma once

//////////////////////////////////////////////////////////////////////////
//=> Includes

#include <CL/cl.h>
#include <objidl.h>
#include <gdiplus.h>


#include <memory>


//Own
#include <Core/Export/Helper/CommonHelper.h>

#include <Core/Export/OpenCL/OpenCLManager.h>
#include <Core/Export/OpenCL/OpenCLKernel.h>

#include <Core/Export/Gdiplus/GdiHelper.h>




//////////////////////////////////////////////////////////////////////////
//=> Lib
#pragma comment( lib, "OpenCL.lib" )
#pragma comment( lib, "Gdiplus.lib")