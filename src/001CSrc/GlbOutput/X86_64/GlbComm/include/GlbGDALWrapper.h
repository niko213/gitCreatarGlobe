#pragma once
class CGlbGDALWrapper
{
public:
	/**
		* @brief 构造函数
		*
		* 1.注册gdal驱动;
		* 2.注册org驱动
		* 3.支持中文名称GDAL_FILENAME_IS_UTF8=NO
		*		
		* @return 无
	*/
	CGlbGDALWrapper(void);
	~CGlbGDALWrapper(void);
};
CGlbGDALWrapper g_glbGDALWrapper;
