#pragma once
class CGlbGDALWrapper
{
public:
	/**
		* @brief ���캯��
		*
		* 1.ע��gdal����;
		* 2.ע��org����
		* 3.֧����������GDAL_FILENAME_IS_UTF8=NO
		*		
		* @return ��
	*/
	CGlbGDALWrapper(void);
	~CGlbGDALWrapper(void);
};
CGlbGDALWrapper g_glbGDALWrapper;
