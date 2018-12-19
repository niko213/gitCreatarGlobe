#pragma once
#include "curl/curl.h"
#include "GlbCommTypes.h"
#include "GlbString.h"
class GlbHttpClient
{
public:
	static glbBool InitGlobal();
	static void CleanupGlobal();
public:	
	GlbHttpClient(void);
	~GlbHttpClient(void);
	glbBool Get(const CGlbString& strUrl,CGlbString& rdStr);
private:
	static bool mpr_global_init;
};

