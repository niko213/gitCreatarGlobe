#pragma once
#include <string>
struct GlbLicResult
{
	std::string code;    //ok,err
	std::string message;
};
class GlbLic
{
public:
	GlbLic();
	~GlbLic(void);
public:
	void          init(std::string ip,int port);
	bool          connect();
	void          close();
	bool          isconnected();
	GlbLicResult* verylic(std::string mdname);	
private:	
	GlbLicResult* handle_rt (char* buf,int len);
	GlbLicResult* handle_err(char* msg);
private:
	void*       mpr_tcphandler;
	std::string mpr_ip;
	int         mpr_port;
	static char tcperr[512];	
};

