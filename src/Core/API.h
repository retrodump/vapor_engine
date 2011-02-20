/************************************************************************
*
* vapor3D Engine � (2008-2010)
*
*	<http://www.vapor3d.org>
*
************************************************************************/

#pragma once

//---------------------------------------------------------------------//
// Configuration
//---------------------------------------------------------------------//

#define VAPOR_SERIALIZATION_JSON

//---------------------------------------------------------------------//
// Platform headers
//---------------------------------------------------------------------//

#include "Platform.h"

//---------------------------------------------------------------------//
// Language features
//---------------------------------------------------------------------//

#include "Language.h"

//---------------------------------------------------------------------//
// API Exports
//---------------------------------------------------------------------//

#if defined(CORE_API_DLL) && defined(CORE_API_DLL_EXPORT)
	#ifdef VAPOR_PLATFORM_WINDOWS
		#define CORE_API __declspec( dllexport )
	#else
		#define CORE_API __attribute__ ((visibility("default")))
	#endif
#elif defined(CORE_API_DLL) && defined(VAPOR_PLATFORM_WINDOWS)
	#define CORE_API __declspec( dllimport )
#else
	#define CORE_API
#endif

//---------------------------------------------------------------------//
// External Templates
//---------------------------------------------------------------------//

#ifdef INSTANTIATE_TEMPLATES
template class std::basic_string<char>;
#else
extern template class std::basic_string<char>;
#endif