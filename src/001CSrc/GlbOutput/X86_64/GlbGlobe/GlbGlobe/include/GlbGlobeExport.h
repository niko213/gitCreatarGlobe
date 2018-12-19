#pragma once


#ifdef GLBGLOBE_EXPORTS

#ifndef GLB_DLLCLASS_EXPORT
#define GLB_DLLCLASS_EXPORT __declspec(dllexport) 
#endif
#else
#ifndef GLB_DLLCLASS_EXPORT
#define GLB_DLLCLASS_EXPORT __declspec(dllimport) 
#endif

#endif
