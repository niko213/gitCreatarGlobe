/********************************************************************
 * Copyright (c) 2013 ³¬Î¬´´Ïë
 * All rights reserved.
 *
 * @file    
 * @brief   
 * @version 1.0
 * @author  GWB
 * @date    2014-03-07 11:28
 ********************************************************************
 */
// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the GLBDATAENGINE_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// GLBDATAENGINE_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef GLBDATAENGINE_EXPORTS
#define GLBDATAENGINE_API __declspec(dllexport)
#else
#define GLBDATAENGINE_API __declspec(dllimport)
#endif
