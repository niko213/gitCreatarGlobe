#pragma once
#include "GlbCommTypes.h"
#include "IGlbDataEngine.h"
class CGlbFSDSProvider : public IGlbDataSourceProvider
{
public:
	CGlbFSDSProvider(void);
	~CGlbFSDSProvider(void);
	const glbWChar*	GetName();
	IGlbDataSource*	CreateDataSource(const glbWChar* url, 
		                             const glbWChar* user,
		                             const glbWChar* password);
	IGlbDataSource*	OpenDataSource(  const glbWChar* url, 
		                             const glbWChar* user,
		                             const glbWChar* password);
	glbWChar*       GetLastError();
};
extern "C"__declspec(dllexport) IGlbDataSourceProvider* GetProvider();

