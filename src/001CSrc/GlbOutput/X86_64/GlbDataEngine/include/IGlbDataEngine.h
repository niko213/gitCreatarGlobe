#pragma once
#include "GlbFields.h"
#include "GlbIndexes.h"
#include "GlbPixelBlock.h"
#include "GlbQueryFilter.h"
#include "IGlbProgress.h"
#include "GlbCell.h"

#pragma warning(disable:4091)

/**
* @brief 基接口
* 
*提供查询其他接口的方法
*
* @version 1.0
* @date    2014-03-10 11:21
* @author  GWB
*/
class GLBDATAENGINE_API IGlbUnknown:virtual public CGlbReference
{
public:
	/**
	* @brief 查询其他接口
	*
	* @param riid 要查询的接口类型
	* @param ppvoid 接收查询到的接口指针
	* @return 成功:true，失败:false
	*/
	virtual glbBool QueryInterface(const glbWChar *riid,void **ppvoid)=0;
};
class IGlbDataSource;
class IGlbDataset;
class IGlbObjectCursor;
class IGlbObjectUpdateCursor;
class IGlbObjectInsertCursor;
class IGlbFeatureCursor;
class IGlbFeatureUpdateCursor;
class IGlbFeatureInsertCursor;
class IGlbRasterDataset;
class IGlbTerrainDataset;
class IGlbSGridDataset;
class IGlbRasterBand;
class CGlbObject;
class CGlbFeature;
class CGlbPixelBlock;
class CGlbDataSourceFactory;
struct GlbSpatialIndexInfo;
struct GlbGridTile;
struct GlbGridTileSurface;
/**
* @brief 数据源提供者接口
*
* @version 1.0
* @date    2014-03-10 11:21
* @author  GWB
*/
class GLBDATAENGINE_API IGlbDataSourceProvider:public CGlbReference
{
public:
	/**
	* @brief 获取标识提供者类型的名称
	*
	* @return 对于文件系统，是L"file";对于db2，是L"db2";对于oracle，是L"oracle".
	*/
	virtual const glbWChar *	GetName()=0;
	/**
	  * @brief 创建一个数据源
	  *
	  * @param url 数据源创建的url
	  * @param user 用户名
	  * @param password 密码
	  * @return 成功 数据源对象，失败 NULL
	  */
	virtual IGlbDataSource*	CreateDataSource(const glbWChar* url, const glbWChar* user,
		const glbWChar* password)=0;
	/**
	* @brief 根据数据源的名字打开数据源
	*
	* @param url 数据源创建的url
	* @param user 用户名
	* @param password 密码
	* @return 成功:返回数据源接口指针，失败:NULL
	*/
	virtual IGlbDataSource*	OpenDataSource(const glbWChar* url, const glbWChar* user,
		const glbWChar* password)=0;
	/**
	* @brief 获取上次执行的错误信息
	*
	* @return 错误信息
	*/
	virtual glbWChar* GetLastError() = 0;
};
/**
* @brief 数据源接口
*
* @version 1.0
* @date    2014-03-10 11:21
* @author  GWB
*/
class  GLBDATAENGINE_API IGlbDataSource:public IGlbUnknown
{
public:
	/**
	* @brief  获取数据集数目
	*
	* @return 数据集数目
	*/
	virtual glbInt32 GetCount() = 0;
	/**
	* @brief 获取数据集
	*
	* @param datasetIndex 数据集索引
	* @return             数据集接口指针，失败:NULL
	*/
	virtual IGlbDataset* GetDataset(glbInt32 datasetIndex) =0;
	/**
	* @brief 获取数据集
	*
	* @param datasetName 数据集名字
	* @return            数据集接口指针，失败:NULL
	*/
	virtual IGlbDataset* GetDatasetByName(const glbWChar* datasetName) = 0;
	/**
	* @brief 删除数据集
	*
	* @param datasetName 数据集的名字
	* @return            成功:true，失败:false
	*/
	virtual glbBool DeleteDataset (const glbWChar * datasetName) = 0;
	/**
	* @brief 获取数据源的别名
	*
	* @return 成功:返回数据源别名，失败:NULL
	*/
	virtual const glbWChar*	GetAlias() = 0;
	/**
	* @brief 获取数据源别名
	*
	* @param alias 数据源别名
	* @return      成功:true失败:false
	*/
	virtual glbBool SetAlias(const glbWChar* alias) = 0;
	/**
	* @brief  设置数据源别名
	*
	* @return 数据源别名，失败:NULL
	*/
	virtual const glbWChar*	GetProviderName()=0;
	/**
	* @brief  获取数据集id
	*
	* @return 数据集id
	*/
	virtual glbInt32 GetID() = 0 ;
	/**
	* @brief 设置数据集id
	*
	* @param  id 数据源id
	* @return    成功:true，失败:false
	*/
	virtual glbBool SetID(glbInt32 id) = 0 ;
	/**
	* @brief 获取最近一次执行的错误信息
	*
	* @return 错误信息
	*/
	virtual glbWChar* GetLastError() = 0;
};
/**
* @brief 要素数据源接口
*
* @version 1.0
* @date    2014-03-10 11:21
* @author  GWB
*/
class GLBDATAENGINE_API IGlbFeatureDataSource:virtual public IGlbDataSource
{
public:
	/**
	* @brief 创建一个数据集
	*
	* @param name 数据集名字
	* @param alias 数据集别名
	* @param type 数据集类型
	* @param srs 数据集空间参考坐标系
	* @param hasZ 数据集是否包含Z值
	* @param hasM 数据集是否包含测量值
	* @return 返回数据集
	*/
	virtual IGlbDataset* CreateDataset(const glbWChar* name ,
		const glbWChar* alias,
		GlbDatasetTypeEnum type,
		const glbWChar* srs,
		glbBool hasZ, 
		glbBool hasM) = 0;
	/**
	* @brief 执行一个sql语句
	*
	* @param sqlstr sql字符串
	* @return 成功:true失败:false
	*/
	virtual glbBool ExecuteSQL(const glbWChar* sqlstr ) = 0;
	/**
	* @brief 根据查询语句获取查询结果集游标
	*
	* @param querystr 查询语句字符串
	* @return 返回查询结果集游标
	*/
	virtual IGlbObjectCursor* Query(const glbWChar* querystr) = 0;
};
/**
* @brief 数据集的基类接口
*
* @version 1.0
* @date    2014-03-10 11:21
* @author  GWB
*/
class GLBDATAENGINE_API IGlbDataset:public IGlbUnknown
{
public:
	/**
	* @brief 获取数据集的类型
	*
	* @return 返回数据集类型
	*/
	virtual GlbDatasetTypeEnum GetType() = 0;
	/**
	* @brief 获取数据集的名字
	*
	* @return 返回数据集的名字
	*/
	virtual const glbWChar* GetName() = 0;
	/**
	* @brief 获取数据集的别名
	*
	* @return 返回数据集的别名
	*/
	virtual const glbWChar* GetAlias() = 0;
	/**
	* @brief 获取数据集所属的数据源
	*
	* @return 返回所属数据源接口指针
	*/
	virtual const IGlbDataSource* GetDataSource() = 0;
	/**
	* @brief 获取数据集的父数据集
	*
	* @return 有父数据集，返回父数据集接口指针。没有，返回NULL
	*/
	virtual const IGlbDataset* GetParentDataset() = 0;
	/**
	* @brief 获取子角色
	*/
	virtual const GlbSubDatasetRoleEnum GetSubRole() = 0;
	/**
	* @brief 获取最近一次执行的错误信息
	*
	* @return 错误信息
	*/
	virtual glbWChar* GetLastError() = 0;
};
/**
* @brief 地理空间数据集抽象接口
*
* @version 1.0
* @date    2014-03-10 11:21
* @author  GWB
*/
class GLBDATAENGINE_API IGlbGeoDataset:public IGlbDataset
{
public:
	/**
	* @brief  获取坐标系,WKT字符串
	*
	* @return 坐标系，失败:NULL
	*/
	virtual const glbWChar*	GetSRS() = 0;
	/**
	* @brief 设置坐标系
	*        
	* @param srs 坐标系
	             可以是: Proj4字符串
				         WKT字符串
						 Esri坐标系文件
						 广为人知的简称,如WGS84
						 EPSG代码
	* @return 成功:true，失败:false
	*/
	virtual glbBool AlterSRS(const glbWChar* srs) = 0;
	/**
	* @brief 获取数据集外包
	*
	* @return 返回外包
	*/
	virtual const CGlbExtent* GetExtent() = 0;
	/**
	* @brief 是否有Z。
	*/
	virtual glbBool HasZ() = 0;
	/**
	* @brief 是否有M。
	*/
	virtual glbBool HasM() = 0;
};
/**
* @brief 事务操作接口
*
* @version 1.0
* @date    2014-03-10 11:21
* @author  GWB
*/
class GLBDATAENGINE_API IGlbTransactions:public IGlbUnknown
{
public:
	/**
	* @brief 开始一个事务
	*
	* @return 成功:true，失败:false
	*/
	virtual glbBool StartTransaction() = 0;
	/**
	* @brief 提交一个事务
	*
	* @return 成功:true，失败:false
	*/
	virtual glbBool CommitTransaction() = 0;
	/**
	* @brief 回滚一个事务
	*
	* @return 成功:true，失败:false
	*/
	virtual glbBool RollbackTransaction() = 0;
	/**
	* @brief 判断是否有开启的事务
	*
	* @return 成功:true，失败:false
	*/
	virtual glbBool IsInTransaction() = 0;
	/**
	* @brief 获取最近一次执行的错误信息
	*
	* @return 成功:true，失败:false
	*/
	virtual glbWChar*	GetLastError() = 0;
};
/**
* @brief 普通数据集接口
*
* @version 1.0
* @date    2014-03-10 11:21
* @author  GWB
*/
class GLBDATAENGINE_API IGlbObjectClass: public IGlbDataset
{
public:
	/**
	* @brief 添加字段
	*
	* @param field 字段
	* @return      成功:true，失败:false
	*/
	virtual glbBool AddField(CGlbField* field) = 0;
	/**
	* @brief 删除字段
	*
	* @param fname 字段名
	* @return      成功:true，失败:false
	*/
	virtual glbBool DeleteField(const glbWChar * fname) = 0;
	/**
	* @brief  获取字段信息
	*
	* @return 字段信息,失败:NULL
	*/
	virtual CGlbFields* GetFields() = 0;
	/**
	* @brief  获取索引信息
	*
	* @return 索引信息,失败:NULL
	*/
	virtual CGlbIndexes* GetIndexes() = 0;
	/**
	* @brief 添加索引
	*
	* @param index 索引
	* @return      成功:true，失败:false
	*/
	virtual glbBool AddIndex(CGlbIndex* index) = 0;
	/**
	* @brief 删除索引
	*
	* @param iname 索引名
	* @return      成功:true，失败:false
	*/
	virtual glbBool DeleteIndex(const glbWChar* iname) = 0;
	/**
	* @brief  获取对象数目
	*
	* @return 对象数目
	*/
	virtual glbInt32 GetObjectCount() = 0;
	/**
	* @brief 获取数对象
	*
	* @param oid 对象id
	* @return    对象,失败:NULL
	*/
	virtual CGlbObject* GetObject(glbInt32 oid) = 0;
	/**
	* @brief 更新对象
	*
	* @param obj 对象
	* @return    成功:true，失败:false
	*/
	virtual glbBool UpdateObject(CGlbObject* obj) = 0;
	/**
	* @brief 删除对象
	*
	* @param oid 对象id
	* @return    成功:true，失败:false
	*/
	virtual glbBool DeleteObject(glbInt32 oid) = 0;
	/**
	* @brief 删除一批对象
	*
	* @param queryfilter 过滤器
	* @return            成功:true，失败:false
	*/
	virtual glbBool DeleteObjects(CGlbQueryFilter* queryfilter) = 0;
	/**
	* @brief 获取查询游标
	*
	* @param queryfilter 过滤器
	* @return            游标,失败:NULL
	*/
	virtual IGlbObjectCursor* Query(CGlbQueryFilter* queryfilter) = 0;
	/**
	* @brief 获取更新游标
	*
	* @param queryfilter 过滤器
	* @return            游标,失败NULL
	*/
	virtual IGlbObjectUpdateCursor*	Update(CGlbQueryFilter* queryfilter) = 0;
	/**
	* @brief 获取插入游标
	*
	* @return 游标,失败NULL
	*/
	virtual IGlbObjectInsertCursor*	Insert() = 0;
};
/**
* @brief 要素数据集接口
*
* @version 1.0
* @date    2014-03-10 11:21
* @author  GWB
*/
class GLBDATAENGINE_API IGlbFeatureClass:public IGlbGeoDataset
{
public:
	/**
	* @brief 添加字段
	*
	* @param field 字段
	* @return      成功:true，失败:false
	*/
	virtual glbBool            AddField(CGlbField* field) = 0;
	/**
	* @brief 删除字段
	*
	* @param fname 字段名
	* @return      成功:true，失败:false
	*/
	virtual glbBool            DeleteField(const glbWChar * fname) = 0;
	/**
	* @brief  获取字段信息
	*
	* @return 字段信息,失败:NULL
	*/
	virtual const CGlbFields*  GetFields() = 0;
	/**
	* @brief   获取索引信息
	*
	* @return  索引信息
	*/
	virtual const CGlbIndexes* GetIndexes() = 0;
	/**
	* @brief 添加索引
	*
	* @param index 索引
	* @return      成功:true，失败:false
	*/
	virtual glbBool            AddIndex(CGlbIndex* index) = 0;
	/**
	* @brief 删除索引
	*
	* @param iname 索引名
	* @return      成功:true，失败:false
	*/
	virtual glbBool            DeleteIndex(const glbWChar* iname) = 0;
	/**
	* @brief  获取空间索引信息
	*
	* @return 空间索引信息
	*/
	virtual GlbSpatialIndexInfo* GetSpatialIndexInfo()=0;
	/**
	* @brief 创建空间索引
	*
	* @param sidxInfo 索引信息
	* @return         成功:true，失败:false
	*/
	virtual glbBool              BuildSpatialIndex(GlbSpatialIndexInfo * sidxInfo)=0;
	/**
	* @brief 删除空间索引信息
	*
	* @return 成功:true，失败:false
	*/
	virtual glbBool              DeleteSpatialIndex()=0;
	/**
	* @brief  获取要素数目
	*
	* @return 要素数目
	*/
	virtual glbInt32             GetFeatureCount()=0;
	/**
	* @brief 获取要素
	*
	* @param oid 要素id
	* @return    要素,失败:NULL
	*/
	virtual CGlbFeature*         GetFeature(glbInt32 oid) = 0;
	/**
	* @brief 更新要素
	*
	* @param feature 要素
	* @return        成功:true，失败:false
	*/
	virtual glbBool	             UpdateFeature(CGlbFeature* feature) = 0;
	/**
	* @brief 删除要素
	*
	* @param oid 要素id
	* @return    成功:true，失败:false
	*/
	virtual glbBool              DeleteFeature(glbInt32 oid) = 0;
	/**
	* @brief 删除一批要素
	*
	* @param queryfilter 过滤器
	* @return            成功:true，失败:false
	*/
	virtual glbBool              DeleteFeatures(CGlbQueryFilter* queryfilter) = 0;
	/**
	* @brief 获取查询游标
	*
	* @param queryfilter 过滤器
	* @return            游标,失败NULL
	*/
	virtual IGlbFeatureCursor*       Query(CGlbQueryFilter* queryfilter) = 0;
	/**
	* @brief 获取更新游标
	*
	* @param queryfilter 过滤器
	* @return            游标,失败NULL
	*/
	virtual IGlbFeatureUpdateCursor* Update(CGlbQueryFilter* queryfilter)=0;
	/**
	* @brief   获取插入游标
	*
	* @return  游标,失败NULL
	*/
	virtual IGlbFeatureInsertCursor* Insert() = 0;
	/**
	* @brief  获取几何类型
	*
	* @return 几何类型
	*/
	virtual GlbGeometryTypeEnum	     GetGeometryType() = 0;
	/**
	* @brief 添加几何类字段
	*
	* @param geocls  几何类
	* @param field   字段
	* @return        成功:true，失败:false
	*/
	virtual glbBool                  AddGeoField(GlbGeometryClassEnum geocls, CGlbField* field) = 0;
	/**
	* @brief 删除几何类字段
	*
	* @param geocls  几何类
	* @param fname   字段名
	* @return        成功:true，失败:false
	*/
	virtual glbBool                  DeleteGeoField(GlbGeometryClassEnum geocls, const glbWChar* fname) = 0;
	/**
	* @brief 获取几何类
	*
	* @param geocls  几何类
	* @return        几何类对象
	*/
	virtual const CGlbGeometryClass* GetGeoClass(GlbGeometryClassEnum subgeocls) = 0;
	/**
	* @brief 数据集中,根据对象的oid,获取其主几何字段指定级别的数据
	*
	* @param oid  对象的oid
	* @param geo  获取的主几何字段数据
	* @param level  几何数据的级别
	* @return 成功:true，失败:false
	*/
	virtual glbBool                  GetGeometry(glbInt32 oid,IGlbGeometry** geo,glbInt32 level = 0) = 0;
};
/**
* @brief 普通数据集对象只读游标接口
*
* @version 1.0
* @date    2014-03-10 11:21
* @author  GWB 
*/
class GLBDATAENGINE_API IGlbObjectCursor:public CGlbReference
{
public:	
	/**
	* @brief  获取字段信息
	*
	* @return 字段信息,失败:NULL
	*/
	virtual CGlbFields*      GetFields() = 0;
	/**
	* @brief  获取类
	*
	* @return 对象类,失败:NULL
	*/
	virtual IGlbObjectClass* GetClass() = 0;
	/**
	* @brief 重查询
	*
	* @return 成功:true，失败:false
	*/
	virtual glbBool          Requery() = 0;
	/**
	* @brief 游标移动到下一个
	*
	* @return 成功:true，失败:false
	*/
	virtual glbBool          MoveNext() = 0;
	/**
	* @brief 获取对象总数
	*
	* @return 返回对象总个数
	*/
	virtual glbInt32         GetCount()=0;
	/**
	* @brief 获取当前对象
	*
	* @return 返回该对象
	*/
	virtual CGlbObject*      GetObject() = 0;
	/**
	* @brief 获取最近一次执行的错误信息
	*
	* @return 返回错误信息
	*/
	virtual glbWChar*        GetLastError() = 0;
};
/**
* @brief 普通对象更新游标接口
*
* @version 1.0
* @date    2014-03-10 11:21
* @author  GWB
*/
class GLBDATAENGINE_API IGlbObjectUpdateCursor:public IGlbObjectCursor
{
public:	
	/**
	* @brief 更新对象
	*
	* @param obj  对象
	* @return     成功:true，失败:false
	*/
	virtual glbBool Update(CGlbObject* obj) = 0;
	/**
	* @brief 删除游标所指的对象,删除后指针前移
	*
	* @return 成功:true，失败:false
	*/
	virtual glbBool Delete() = 0;
	/**
	* @brief 开始批量更新
	*
	* @return 成功:true，失败:false
	*/
	virtual glbBool BeginBatch()   = 0;
	/**
	* @brief 取消批量更新
	*
	* @return 成功:true，失败:false
	*/
	virtual glbBool CancelBatch()  = 0;
	/**
	* @brief 提交批量更新
	*
	* @return 成功:true，失败:false
	*/
	virtual glbBool CommitBatch()  = 0;
};
/**
* @brief 普通对象插入游标接口
*
* @version 1.0
* @date    2014-03-10 11:21
* @author  GWB
*/
class GLBDATAENGINE_API IGlbObjectInsertCursor:public CGlbReference
{
public:
	/**
	* @brief  获取字段信息
	*
	* @return 字段信息,失败:NULL
	*/
	virtual CGlbFields*      GetFields()             = 0;
	/**
	* @brief 获取类
	*
	* @return 对象类,失败NULL
	*/
	virtual IGlbObjectClass* GetClass()              = 0;
	/**
	* @brief      插入一个对象
	*
	* @param obj  对象
	* @return     成功:true，失败:false
	*/
	virtual glbBool	         Insert(CGlbObject* obj) = 0;
	/**
	* @brief 开始批量更新
	*
	* @return 成功:true，失败:false
	*/
	virtual glbBool          BeginBatch()   = 0;
	/**
	* @brief 取消批量更新
	*
	* @return 成功:true，失败:false
	*/
	virtual glbBool          CancelBatch()  = 0;
	/**
	* @brief 提交批量更新
	*
	* @return 成功:true，失败:false
	*/
	virtual glbBool	         CommitBatch()  = 0;
	/**
	* @brief 获取最近一次执行的错误信息
	*
	* @return 成功:true，失败:false
	*/
	virtual glbWChar*        GetLastError() = 0;
};
/**
* @brief 要素对象只读游标接口
*
* @version 1.0
* @date    2014-03-10 11:21
* @author  GWB
*/
class GLBDATAENGINE_API IGlbFeatureCursor : public CGlbReference
{
public:
	/**
	* @brief  获取要素数目
	*
	* @return 要素数目
	*/
	virtual glbInt32                GetCount()=0;
	/**
	* @brief  获取字段信息
	*
	* @return 字段信息,失败:NULL
	*/
	virtual const CGlbFields*       GetFields() = 0;
	/**
	* @brief  获取类
	*
	* @return 要素类,失败:NULL
	*/
	virtual const IGlbFeatureClass* GetClass() = 0;
	/**
	* @brief 重查询
	*
	* @return 成功:true，失败:false
	*/
	virtual glbBool	                Requery() = 0;
	/**
	* @brief 游标移动到下一个
	*
	* @return 成功:true，失败:false
	*/
	virtual glbBool	                MoveNext() = 0;
	/**
	* @brief 获取游标当前所指对象
	*
	* @return 返回该对象
	*/
	virtual CGlbFeature*            GetFeature() = 0;
	/**
	* @brief 获取最近一次执行的错误信息
	*
	* @return 返回错误信息
	*/
	virtual glbWChar*               GetLastError() = 0;
};
/**
* @brief 要素对象更新游标接口
*
* @version 1.0
* @date    2014-03-10 11:21
* @author  GWB
*/
class GLBDATAENGINE_API IGlbFeatureUpdateCursor : public IGlbFeatureCursor
{
public:	
	/**
	* @brief  删除要素,同时游标下移
	*
	* @return 成功:true，失败:false
	*/
	virtual glbBool   Delete() = 0;
	/**
	* @brief     更新要素
	*
	* @param ft  要素
	* @return    成功:true，失败:false
	*/
	virtual glbBool   Update(CGlbFeature* ft) = 0;
	/**
	* @brief 开始批量更新
	*
	* @return 成功:true，失败:false
	*/
	virtual glbBool   BeginBatch()   = 0;
	/**
	* @brief 取消批量更新
	*
	* @return 成功:true，失败:false
	*/
	virtual glbBool   CancelBatch()  = 0;
	/**
	* @brief 提交批量更新
	*
	* @return 成功:true，失败:false
	*/
	virtual glbBool   CommitBatch()  = 0;
	/**
	* @brief 设置是否为只更新模式
	*
	* @param isloadOnly  是：true不更新空间索引,否:false
	* @return            成功:true，失败:false
	*/
	virtual glbBool	  SetUpdateOnly(glbBool isloadOnly)=0;
};
/**
* @brief 要素对象插入游标接口
*
* @version 1.0
* @date    2014-03-10 11:21
* @author  GWB
*/
class GLBDATAENGINE_API IGlbFeatureInsertCursor:public CGlbReference
{
public:
	/**
	* @brief  获取字段信息
	*
	* @return 字段信息,失败:NULL
	*/
	virtual CGlbFields*	      GetFields() = 0;
	/**
	* @brief  获取类
	*
	* @return 要素类,失败:NULL
	*/
	virtual IGlbFeatureClass* GetClass() = 0;
	/**
	* @brief     插入要素
	*
	* @param ft  要素
	* @return    成功:true，失败:false
	*/
	virtual glbBool           Insert(CGlbFeature* ft) = 0;
	/**
	* @brief 开始批量更新
	*
	* @return 成功:true，失败:false
	*/
	virtual glbBool           BeginBatch()   = 0;
	/**
	* @brief 取消批量更新
	*
	* @return 成功:true，失败:false
	*/
	virtual glbBool           CancelBatch()  = 0;
	/**
	* @brief 提交批量更新
	*
	* @return 成功:true，失败:false
	*/
	virtual glbBool           CommitBatch()  = 0;
	/**
	* @brief 设置为只插入模式
	*
	* @param isloadOnly  是：true不更新空间索引,否:false
	* @return 成功:true，失败:false
	*/
	virtual glbBool           SetLoadOnly(glbBool isloadOnly) = 0;
	/**
	* @brief 获取最近一次执行的错误信息
	*
	* @return 成功:true，失败:false
	*/
	virtual glbWChar*         GetLastError() = 0;	
};
/**
* @brief 剖面数据集接口
*
* @version 1.0
* @date    2014-03-10 11:21
* @author  GWB
*/
class GLBDATAENGINE_API IGlbSectionDataset:public IGlbFeatureClass
{
public:
	/**
	  * @brief  取控制点数据集
	  *
	  * @return 控制点子数据集,,失败NULL
	*/
	virtual IGlbObjectClass* GetCtrlPtClass() = 0;
	/**
	  * @brief  取节点数据集
	  *
	  * @return 节点子数据集,失败NULL
	  */
	virtual IGlbFeatureClass* GetNodeClass() = 0;
	/**
	  * @brief  获取子弧段子数据集
	  *
	  * @return 弧段子数据集,失败NULL
	  */
	virtual IGlbFeatureClass* GetArcClass()=0;
	/**
	  * @brief  获取多边形子数据集
	  *
	  * @return 多边形子数据集,失败NULL
	  */
	virtual IGlbFeatureClass* GetPolygonClass()=0;
};
/**
* @brief 网络数据集接口
         弧段ID GLBEDGEID 起始节点ID GLBFNODEID 终止节点ID GLBTNODEID
*        交通网络
*        {
             边、节点、障碍边、障碍点、转向表
*            弧段权值: 通过该弧段的花费，分为正向权值(F-->T)，反向权值(T-->F)
*            交通规则：道路为正向单行线、逆向单行线、双向通行线或禁行线。
*		 }
*        
*        设施网络：
*        {
              弧、节点、源(Source)、汇(Sink)、流向、权值、障碍
			  流向：0 与弧段数字化方向相同
			        1 与弧段数字化方向相反
					2 流向不确定或无效
					3 不连通 弧段与源、汇的节点不连通
			  节点类型：
			        0 普通点 1 源 2 汇
			  等级
*        }
* @version 1.0
* @date    2014-03-10 11:21
* @author  GWB
*/
class GLBDATAENGINE_API IGlbNetworkDataset:public IGlbFeatureClass
{
public:
	/**
	* @brief  获取节点子数据集
	*         节点ID GLBNODEID
	*         点到弧段的距离允限：决定孤立点是否产于网络分析
	* @return 节点子数据集,失败NULL
	*/
	virtual const IGlbFeatureClass* GetNodeClass()   =0;
	/**
	* @brief  获取转向子数据集
	*         转向节点ID GLBNODEID 起始弧段ID GLBFEDGEID 终止弧段ID GLBTEDGEID
	*         转向权值：通过该转向节点的花费
	* @return 转弯子数据集,失败NULL
	*/
	virtual const IGlbObjectClass*  GetTurnClass()   =0;
	/**
	* @brief  创建转弯子数据集
	*
	* @return 转弯子数据集,失败NULL
	*/
	virtual const IGlbObjectClass*  CreateTurnClass()=0;
	/**
	* @brief 删除转弯子数据集
	*
	* @return 成功:true，失败:false
	*/
	virtual glbBool                 DeleteTurnClass()=0;	
};
/**
* @brief 钻孔数据集接口
*
* @version 1.0
* @date    2014-03-10 11:21
* @author  GWB
*/
class GLBDATAENGINE_API IGlbDrillDataset:public IGlbFeatureClass
{
public:
	/**
	* @brief  获取测斜子数据集
	*
	* @return 测斜子数据集,失败:NULL
	*/
	virtual IGlbObjectClass* GetSkewClass()=0;
	/**
	* @brief  获取分层子数据集
	*
	* @return 分层子数据集,失败:NULL
	*/
	virtual IGlbFeatureClass* GetStratumClass()=0;
	/**
	* @brief  获取样品子数据集数目
	*
	* @return 样品子数据集总数
	*/
	virtual glbInt32 GetSampleClassCount()=0;
	/**
	* @brief      获取样品子数据集
	*
	* @param idx  样品表索引
	* @return     样品子数据集,失败:NULL
	*/
	virtual IGlbObjectClass* GetSampleClass(glbInt32 idx)=0;
	/**
	* @brief       获取样品子数据集
	*
	* @param name  样品表名字
	* @return      样品子数据集,失败:NULL
	*/
	virtual IGlbObjectClass* GetSampleClassByName(const glbWChar* name)=0;
	/**
	* @brief        创建样品子数据集
	*
	* @param name   样品表名字
	* @param alias  样品表别名
	* @return       样品子数据集,失败NULL
	*/
	virtual IGlbObjectClass* CreateSampleClass(const glbWChar* name, const glbWChar* alias)=0;
	/**
	* @brief        删除样品子数据集
	*
	* @param name   样品表名字
	* @return       成功:true，失败:false
	*/
	virtual glbBool DeleteSampleClass(const glbWChar* name)=0;
};
/**
* @brief 栅格数据集存储方式的定义
*
*
* @version 1.0
* @date    2014-03-10 11:21
* @author  GWB
*/
struct GlbRasterStorageDef
{
	glbInt32               blockSizeX;	///< x方向存储分块大小
	glbInt32               blockSizeY;	///< y方向存储分块大小
	GlbPixelLayoutTypeEnum pixelLayout;	///< 像素排序格式
	GlbCompressTypeEnum    compress;    ///< 压缩方式
	glbBool                hasNodata;   ///< 是否有nodata
	glbDouble              nodata;      ///< nodata值
};
/**
* @brief 空间索引信息
*
* @version 1.0
* @date    2014-03-10 11:21
* @author  GWB
*/
struct GlbSpatialIndexInfo
{
	GlbSpatialIndexTypeEnum		m_idxType;		///<
	glbDouble					m_grid1XYSize;	///<
	glbDouble					m_grid1ZSize;	///<
	glbDouble					m_grid2XYSize;	///<
	glbDouble					m_grid2ZSize;	///<
	glbDouble					m_grid3XYSize;	///<
	glbDouble					m_grid3ZSize;	///<
};

/**
* @brief 删格数据源接口
*
* @version 1.0
* @date    2014-03-10 11:21
* @author  GWB
*/
class GLBDATAENGINE_API IGlbRasterDataSource:virtual public IGlbDataSource
{
public:
	/**
	* @brief 获取栅格数据集
	*
	* @param datasetName 数据集名字
	* @return            栅格数据集,失败返回NULL
	*/
	virtual IGlbRasterDataset* GetRasterDataset(const glbWChar * datasetName) =0;
	/**
	  * @brief 获取地形数据集
	  *
	  * @param terrainDatasetName 数据集名字
	  * @return                  地形数据集,失败返回NULL
	*/
	virtual IGlbTerrainDataset* GetTerrainDataset( const glbWChar* terrainDatasetName)=0;
	/**
	  * @brief 获取格网数据集
	  *
	  * @param terrainDatasetName 数据集名字
	  * @return                  地形数据集,失败返回NULL
	*/
	virtual IGlbSGridDataset* GetSGridDataset( const glbWChar* datasetName)=0;
	/**
	* @brief 创建栅格数据集
	*
	* @param name      数据集名字
	* @param alias     数据集别名
	* @param nBands    波段数目,必须大于0
	* @param pixelType 像素类型
	* @param columns   列数,列的方向为x方向
	* @param rows      行数,行的方向为y方向
	* @param orgX      原点的x坐标
	* @param orgY      原点的y坐标
	* @param rsX       x方向分辨率
	* @param rsY       y方向分辨率
	* @param format    数据集文件格式
	* @param SRS       空间参考坐标系
	* @param storeDef  存储格式的定义
	* @return          栅格数据集,失败返回NULL
	*/
	virtual IGlbRasterDataset* CreateRasterDataset
		   (const glbWChar * name,
		    const glbWChar * alias,
		    glbInt32         nBands,
		    GlbPixelTypeEnum pixelType,
		    glbInt32         columns,
		    glbInt32        rows,
		    glbDouble       orgX   =0,
		    glbDouble       orgY   =0,
		    glbDouble       rsX    =1,
		    glbDouble       rsY    =1,
		    const glbWChar* format =L"GTiff",
		    const glbWChar* SRS    =NULL,
		    const GlbRasterStorageDef *storeDef=NULL)=0;
	/**
	* @brief 创建地形数据集
	*
	* @param name        数据集名字
	* @param alias       数据集别名
	* @param isGlobe     是否球面
	* @param lzts        0级分块范围
	* @param SRS         空间参考坐标系
	* @param domCompress 影像压缩格式
	* @param demCompress 高程压缩格式
	* @return            地形数据集,失败返回NULL
	*/
	virtual IGlbTerrainDataset* CreateTerrainDataset(
		const glbWChar*     name,
		const glbWChar*     alias,
		bool                isGlobe    =true,
		glbDouble           lzts       =36,
		const glbWChar*     SRS        =NULL,
		GlbCompressTypeEnum domCompress=GLB_COMPRESSTYPE_JPEG,
		GlbCompressTypeEnum demCompress=GLB_COMPRESSTYPE_ZLIB)=0;
};

/**
* @brief 删格数据集接口
*
* @version 1.0
* @date    2014-03-10 11:21
* @author  GWB
*/
class GLBDATAENGINE_API IGlbRasterDataset:public IGlbGeoDataset
{
public:
	/**
	* @brief 获取数据集原点坐标
	*        原点是左上角,originX = 范围.minX
	*                    originY = 范围.maxY
	*
	* @param originX 原点x坐标
	* @param originY 原点y坐标
	* @return 成功:true，失败:false
	*/
	virtual glbBool GetOrigin(glbDouble* originX, glbDouble* originY)=0;
	/**
	* @brief 获取数据集的行列数
	*
	* @param columns 列数
	* @param rows    行数
	* @return 成功:true，失败:false
	*/
	virtual glbBool GetSize(glbInt32* columns, glbInt32* rows)= 0;
	/**
	* @brief 获取数据集的分辨率
	*
	* @param rsX x方向分辨率
	* @param rsY y方向分辨率
	* @return 成功:true，失败:false
	*/
	virtual glbBool GetRS(glbDouble* rsX, glbDouble* rsY)= 0;
	/**
	* @brief 获取数据集像素类型
	*
	* @return 返回像素类型
	*/
	virtual GlbPixelTypeEnum GetPixelType()=0;
	/**
	* @brief 获取数据集像素布局
	*
	* @return 返回像素布局
	*/
	virtual GlbPixelLayoutTypeEnum GetPixelLayout()=0;
	/**
	* @brief 获取数据集压缩类型
	*
	* @return 返回压缩类型
	*/
	virtual const glbWChar* GetCompressType()=0;
	/**
	* @brief 获取文件格式
	*
	* @return 返回编码格式
	*/
	virtual const glbWChar*	GetFormat()=0 ;
	/**
	* @brief 获取数据集的无效值
	*        不支持不同波段有不同的无效值
	*
	* @param noDataVal 无效值
	* @return 成功:有无效值，失败:没有无效值
	*/
	virtual glbBool GetNoData(glbDouble* noDataVal)=0 ;
	/**
	* @brief 设置数据集的无效值
	*        不支持不同波段有不同的无效值
	*
	* @param noDataVal 无效值
	* @return 成功:true，失败:false
	*/
	virtual glbBool AlterNoData(glbDouble noDataVal)=0 ;
	/**
	* @brief 获取数据集波段数
	*
	* @return 返回波段数
	*/
	virtual glbInt32 GetBandCount() = 0;
	/**
	* @brief 根据索引获取数据集波段
	*
	* @return 返回波段
	*/
	virtual const IGlbRasterBand* GetBand(glbInt32 bandIndex) =0;
	/**
	* @brief 检索数据集的分块大小
	*
	* @param blockSizeX x方向像素个数
	* @param blockSizeY y方向像素个数
	* @return 成功:true，失败:false
	*/
	virtual glbBool GetBlockSize(glbInt32* blockSizeX,glbInt32* blockSizeY)=0;
	/**
	* @brief 按指定的行列数创建一个像素块
	*
	* @param columns 列数
	* @param rows 行数
	* @return 返回创建的像素块
	*/
	virtual CGlbPixelBlock* CreatePixelBlock(glbInt32 columns, glbInt32 rows) = 0;
	/**
	* @brief 读取一个像素块
	*        如果是分块存储的,按GetBlockSize的大小读取效率最高
	*        没有数据的部分像素值初始化为无效值,对应的掩码值为0.
	*
	* @param startColum  起始列>=0
	* @param startRow    起始行>=0
	* @param pPixelBlock 读取的像素块
	* @return 成功:true，失败:false
	*/
	virtual glbBool Read(glbInt32 startColum, glbInt32 startRow, CGlbPixelBlock *pPixelBlock)=0;
	/**
	* @brief 读取一个像素块
	*        如果写入数据分辨率与数据集分辨率不一致,重采样.
	*
	* @param destExtent  范围
	* @param pPixelBlock 读取的像素块
	* @param rspType     采样方式
	* @return 成功:true，失败:false
	*/
	virtual glbBool ReadEx(const CGlbExtent *destExtent,
		                   CGlbPixelBlock   *pPixelBlock,
						   GlbRasterResampleTypeEnum rspType)=0;
	/**
	* @brief 写入一个像素块
	*
	* @param  startColum  起始列
	* @param  startRow    起始行
	* @param  pPixelBlock 写入的像素块
	* @return 成功:true，失败:false
	*/
	virtual glbBool Write(glbInt32 startColum, glbInt32 startRow, CGlbPixelBlock *pPixelBlock)=0;
	/**
	* @brief 判断数据集是否有金字塔数据
	*
	* @return 有:true，没有:false
	*/
	virtual glbBool HasPyramid() =0;
	/**
	* @brief 创建金字塔
	*        如果isShareEdge,是创建DEM金字塔；否则是创建DOM金字塔.
	*        DEM金字塔：
	*                 只支持1波段数据集
	*                 金字塔像素值类型是Float32
	*        DOM金字塔：
	*                 只支持3波段或4波段数据集
	*                 金字塔像素类型是BYTE.
	*
	* @param srcCol      原始数据起始列
	* @param srcRow      原始数据起始行        
	* @param srcColumns  原始数据列数
	* @param srcRows     原始数据行数
	* @param rspType     采样方式
	* @param tileSizeX   分片X方向像素个数
	* @param tileSizeY   分片Y方向像素个数
	* @param lzts        0级分片范围
	* @param isGlobe     是否球面模式
	* @param isShareEdge 是否共享边界,高程数据必须共享边
	* @param isReplace   是否替换value(只针对高程)
	* @param rpValue     替换值(只针对高程)
	* @param progress    金字塔创建的进度信息获取接口
	* @return 成功:true，失败:false
	*/
	virtual glbBool BuildPyramid(
		glbInt32  srcCol,
		glbInt32  srcRow,
		glbInt32  srcColumns,
		glbInt32  srcRows,
		GlbRasterResampleTypeEnum rspType,
		glbInt32  tileSizeX,
		glbInt32  tileSizeY,
		glbDouble lzts        = 36,
		glbBool   isGlobe     = true, 
		glbBool   isShareEdge = false,
		glbBool   isReplace   =false,
		glbDouble rpValue     =0.0,
		IGlbProgress* progress=0)=0;
	/**
	* @brief 删除数据集金字塔
	*
	* @return 成功:true，失败:false
	*/
	virtual glbBool DeletePyramid() = 0;
	/**
	* @brief 获取金字塔像素布局方式
	*
	* @return 返回金字塔排列类型
	*/
	virtual GlbPixelLayoutTypeEnum GetPyramidPixelLayout()=0;
	/**
	* @brief 获取金字塔像素类型
	*
	* @return 返回金字塔像素类型
	*/
	virtual GlbPixelTypeEnum GetPyramidPixelType()=0;
	/**
	* @brief  获取金子塔0级瓦片范围
	*
	* @return 瓦片范围
	*/
	virtual glbDouble GetPyramidLZTS()=0;
	/**
	* @brief 获取金字塔是否共享边界
	*
	* @return 共享:true，不共享:false
	*/
	virtual glbBool IsPyramidShareEdge() =0;
	/**
	* @brief 获取金字塔级数
	*
	* @param minLevel 获取最小级别
	* @param maxLevel 获取最大级别
	* @return 成功:true，失败:false
	*/
	virtual glbBool	GetPyramidLevels(glbInt32* minLevel, glbInt32* maxLevel) =0;
	/**
	* @brief 获取某一级别金字塔的分辨率
	*
	* @param level 级别数
	* @param rsX 获取x方向分辨率
	* @param rsY 获取y方向分辨率
	* @return 成功:true，失败:false
	*/
	virtual glbBool GetPyramidLevelRS(glbInt32 level, glbDouble* rsX, glbDouble* rsY)=0;
	/**
	  * @brief 获取金字塔瓦片行列编号范围
	  *
	  * @param iLevel            级别
	  * @param startTileColumn   返回开始列编号
	  * @param startTileRow      返回开始行编号
	  * @param endTileColumn     返回结束列编号
	  * @param endTileRow        返回结束行编号
	  * @return 成功:true，失败:false
	  */
	virtual glbBool GetPyramidTileIndexes(glbInt32 iLevel,glbInt32& startTileColumn,glbInt32& startTileRow,
		glbInt32& endTileColumn,glbInt32& endTileRow)=0;
	/**
	  * @brief 获取空间范围内金子塔瓦片行列编号范围
	  *
	  * @param extent            空间范围            
	  * @param iLevel            级别
	  * @param startTileColumn   返回开始列编号
	  * @param startTileRow      返回开始行编号
	  * @param endTileColumn     返回结束列编号
	  * @param endTileRow        返回结束行编号
	  * @return 成功:true，失败:false
	  */
	virtual glbBool GetExtentCoverTiles(const CGlbExtent* extent, glbInt32 iLevel,glbInt32& startTileColumn, glbInt32& startTileRow,
		glbInt32& endTileColumn, glbInt32& endTileRow)=0;
	/**
	  * @brief 获取一组金子塔瓦片的空间范围
	  *      
	  * @param iLevel            级别
	  * @param startTileColumn   开始列编号
	  * @param startTileRow      开始行编号
	  * @param endTileColumn     结束列编号
	  * @param endTileRow        结束行编号
	  * @param extent            空间范围 
	  * @return 成功:true，失败:false
	  */
	virtual glbBool GetTilesCoverExtent(glbInt32 iLevel,glbInt32 startTileColumn, glbInt32 startTileRow,glbInt32 endTileColumn, 
		glbInt32 endTileRow,CGlbExtent *extent)=0;
	/**
	* @brief 获取金字塔的范围
	*
	* @return 返回范围，失败返回NULL
	*/
	virtual glbBool GetLevelExtent(glbInt32 ilevel, CGlbExtent* ext) = 0;

	/**
	* @brief 获取金字塔的瓦片大小
	*
	* @param blockSizeX 块在x方向像素个数
	* @param blockSizeY 块在y方向像素个数
	* @return 成功:true，失败:false
	*/
	virtual glbBool GetPyramidBlockSize(glbInt32* blockSizeX, glbInt32* blockSizeY) =0;

	/**
	  * @brief 创建一个金字塔像素块
	  *
	  * @return 返回创建的像素块，失败返回NULL
	  */
	virtual CGlbPixelBlock* CreatePyramidPixelBlock() = 0;

	/**
	  * @brief 读取金子塔瓦片
	  *
	  * @param level        级别
	  * @param tileColumn   列
	  * @param tileRow      行
	  * @param pPixelBlock  像素块
	  * @return 成功:true，失败:false
	  */
	virtual glbBool ReadPyramidByTile(glbInt32 level,
		                              glbInt32 tileColumn,
		                              glbInt32 tileRow,
		                              CGlbPixelBlock* pPixelBlock)=0;
	/**
	* @brief 写入金子塔瓦片
	*
	* @param level      级别
	* @param tileColumn 列
	* @param tileRow    行
	* @param pPixelBlock像素块
	* @return 成功:true，失败:false
	*/
	virtual glbBool WritePyramidByTile(glbInt32 level,
		                               glbInt32 tileColumn,
		                               glbInt32 tileRow,
		                               CGlbPixelBlock* pPixelBlock)=0;
	/**
	* @brief 设置自定义掏空指定区域
	*
	* @param extent   指定区域
	* @param isMove  是否掏空
	*/
	virtual void SetSelfDealMoveExtent(CGlbExtent *extent,glbBool isMove);
	/**
	* @brief 设置自定义压平指定区域
	*
	* @param extent   指定区域
	* @param value     修改结果值
	* @param isChange 是否修改
	*/
	virtual void SetSelfDealChangeExtent(CGlbExtent *extent,glbDouble value,glbBool isChange);
};

/**
* @brief SGrid网格数据集接口
*
* @version 1.0
* @date    2018-01-29 11:21
* @author  ML
*/
class GLBDATAENGINE_API IGlbSGridDataset:public IGlbGeoDataset
{
public:
	virtual glbInt32 get_NI() = 0;
	virtual glbInt32 get_NJ() = 0;
	virtual glbInt32 get_NK() = 0;
	virtual glbDouble get_StepI() = 0;
	virtual glbDouble get_StepJ() = 0;
	virtual glbVector get_StepVectorI() = 0;
	virtual glbVector get_StepVectorJ() = 0;
	virtual glbBool GetOrigin(glbDouble* x, glbDouble* y) = 0;
	virtual glbBool GetCell(glbInt32 I, glbInt32 J, glbInt32 K, cwSGridCell * pSGridCell, glbBool * isValidCell) = 0;
	virtual glbBool GetCellColor(glbInt32 I, glbInt32 J, glbInt32 K,glbInt32* clr , glbBool * isValidColor) = 0;
	// 获取外包单元cell集合
	virtual glbBool GetBoundCells(cwSGridFacet** index, glbInt32 * count) = 0;
	// 根据索引获取cell
	virtual glbBool GetCellByIndex(glbInt32 index, cwSGridCell * pSGridCell, glbBool * isValidCell) = 0;
	// 根据索引获取cell颜色值
	virtual glbBool GetCellColorByIndex(glbInt32 index, glbInt32* clr , glbBool * isValidColor) = 0;
	// 设置颜色方案文件
	virtual glbBool SetColorScheme(glbWChar* schemeLocate) = 0;

	// 跟索引获取Cell属性值
	//virtual glbBool GetCellPropertyByIndex(glbInt32 index, glbDouble* value) = 0;
	// 获取最大最小等级
	virtual glbBool GetLevels(glbInt32& minLevel, glbInt32& maxLevel) = 0;
	// 获取原点XY坐标
	virtual glbBool GetOrigin(glbDouble& originX,glbDouble& originY) = 0;
	// 获取某级的起始和终止行列号
	virtual glbBool GetCovertTiles(glbInt32 level,	glbInt32& startTileCol, glbInt32& startTileRow,	glbInt32& endTileCol,glbInt32& endTileRow, glbInt32& startTileLayer,glbInt32& endTileLayer) = 0;
	// 读取一个tile
	virtual glbBool ReadTile(glbInt32 level, glbInt32 row, glbInt32 column, glbInt32 layer = 0, GlbGridTile* pTile=NULL) = 0;
	// 读取一个tile的外表面数据
	virtual glbBool ReadTileSurface(glbInt32 level, glbInt32 row, glbInt32 column, glbInt32 layer = 0, GlbGridTileSurface* pTileSurface=NULL) = 0;
};
/**
* @brief 波段的接口
*
* @version 1.0
* @date    2014-03-10 11:21
* @author  GWB
*/
class GLBDATAENGINE_API IGlbRasterBand:public CGlbReference
{
public:
	/**
	* @brief 获取波段id
	*
	* @return 返回id
	*/
	virtual glbInt32 GetID()=0;
	/**
	* @brief 获取波段名
	*
	* @return 返回波段名
	*/
	virtual const glbWChar* GetName()=0;
};
struct GlbTileNeighbor
{
	glbBool hasLeft;   ///<瓦片是否有左邻居
	glbBool hasRight;  ///<瓦片是否有右邻居
	glbBool hasUp;     ///<瓦片是否有上邻居
	glbBool hasDown;   ///<瓦片是否有下邻居
	glbBool hasLeftUp; ///<瓦片是否有左上邻居
	glbBool hasRightUp;///<瓦片是否有右上邻居
	glbBool hasLeftDown;///<瓦片是否有左下邻居
	glbBool hasRightDown;///<瓦片是否有右下邻居
};
/**
* @brief 地形数据集接口
*
* @version 1.0
* @date    2014-03-10 11:21
* @author  GWB
*/
class  GLBDATAENGINE_API IGlbTerrainDataset:public IGlbGeoDataset
{
public:
	/**
	* @brief      获取0级瓦片范围大小
	*
	* @param lzts 瓦片大小
	* @return     成功:true，失败:false
	*/
	virtual glbBool GetLZTS(glbDouble& lzts)=0;
	/**
	* @brief 是否球面模式
	*
	* @return true是球面模式，false是平面模式
	*/
	virtual glbBool IsGlobe()=0;
	/**
	* @brief 获取影像像素类型
	*
	* @return 返回影像像素类型
	*/
	virtual GlbPixelTypeEnum GetDomPixelType()=0;
	/**
	* @brief 获取高程像素类型
	*
	* @return 返回高程像素类型
	*/
	virtual GlbPixelTypeEnum GetDemPixelType()=0;
	/**
	* @brief 获取影像像素排列类型
	*
	* @return 返回影像像素排列类型
	*/
	virtual GlbPixelLayoutTypeEnum GetDomPixelLayout()=0;
	/**
	* @brief 获取高程像素排列类型
	*
	* @return 返回高程像素排列类型
	*/
	virtual GlbPixelLayoutTypeEnum GetDemPixelLayout()=0;
	/**
	* @brief 检索影像的分块大小
	*
	* @param blockSizeX x方向的像素个数
	* @param blockSizeY y方向的像素个数
	* @return 成功:true，失败:false
	*/
	virtual glbBool GetDomBlockSize(glbInt32& blockSizeX, glbInt32& blockSizeY)=0;
	/**
	* @brief 检索高程的分块大小
	*
	* @param blockSizeX x方向的像素个数
	* @param blockSizeY y方向的像素个数
	* @return 成功:true，失败:false
	*/
	virtual glbBool GetDemBlockSize(glbInt32& blockSizeX, glbInt32& blockSizeY)=0;
	/**
	* @brief 获取影像波段数
	*
	* @return 返回影像波段数
	*/
	virtual glbInt32 GetDomBandCount() = 0;
	/**
	* @brief 获取高程波段数
	*
	* @return 返回高程波段数
	*/
	virtual glbInt32 GetDemBandCount() = 0;
	/**
	* @brief 获取影像级别数
	*
	* @param maxLevel 最大级别
	* @param minLevel 最小级别
	* @return 成功:true，失败:false
	*/
	virtual glbBool GetDomLevels(glbInt32& maxLevel,glbInt32& minLevel)=0;
	/**
	* @brief 获取高程级别数
	*
	* @param maxLevel 最大级别
	* @param minLevel 最小级别
	* @return 成功:true，失败:false
	*/
	virtual glbBool GetDemLevels(glbInt32& maxLevel,glbInt32& minLevel)=0;	
	/**
	* @brief 获取影像范围
	*
	* @param iLevel 级别
	* @param ext    范围
	* @return 成功返回true，失败返回false
	*/
	virtual glbBool GetDomLevelExtent(glbInt32 iLevel, CGlbExtent* ext)=0;
	/**
	* @brief 获取高程范围
	*
	* @param iLevel 级别
	* @param ext    范围
	* @return 成功返回true，失败返回false
	*/
	virtual glbBool GetDemLevelExtent(glbInt32 iLevel, CGlbExtent* ext)=0;
	/** 
	* @brief 获取影像分块索引
	* @brief 球面模式:
	*                原点是(-180,90),开始列从最小X开始,结束列在最大X
	*                                开始行从最大Y开始,结束行在最小Y
	*        平面模式:
	*                原点(0,0)，     开始列从最小X开始,结束列在最大X
	*                                开始行从最小Y开始,结束行在最大Y
	*
	* @param level           级别
	* @param startTileCol    开始列值
	* @param startTileRow    开始行值
	* @param endTileCol      结束列值
	* @param endTileRow      结束行值
	* @return 成功返回true，失败返回false
	*/
	virtual glbBool GetDomTileIndexes(glbInt32 level, 
		                              glbInt32& startTileCol,  
									  glbInt32& startTileRow, 
		                              glbInt32& endTileCol,  
									  glbInt32& endTileRow)=0;
	/** 
	* @brief 获取高程分开索引
	* @brief 球面模式:
	*                原点是(-180,90),开始列从最小X开始,结束列在最大X
	*                                开始行从最大Y开始,结束行在最小Y
	*        平面模式:
	*                原点(0,0)，     开始列从最小X开始,结束列在最大X
	*                                开始行从最小Y开始,结束行在最大Y
	*
	* @param level           级别
	* @param startTileCol    开始列值
	* @param startTileRow    开始行值
	* @param endTileCol      结束列值
	* @param endTileRow      结束行值
	* @return 成功返回true，失败返回false
	*/
	virtual glbBool GetDemTileIndexes(glbInt32 level, 
		                              glbInt32& startTileCol,
									  glbInt32& startTileRow, 
		                              glbInt32& endTileCol,  
									  glbInt32& endTileRow)=0;
	/** 
	* @brief 计算影像范围的分块索引
	* @brief 球面模式:
	*                原点是(-180,90),开始列从最小X开始,结束列在最大X
	*                                开始行从最大Y开始,结束行在最小Y
	*        平面模式:
	*                原点(0,0)，     开始列从最小X开始,结束列在最大X
	*                                开始行从最小Y开始,结束行在最大Y
	*
	* @param extent          空间范围
	* @param level           级别
	* @param startTileCol    开始列值
	* @param startTileRow    开始行值
	* @param endTileCol      结束列值
	* @param endTileRow      结束行值
	* @return 成功返回true，失败返回false
	*/
	virtual glbBool GetDomExtentCoverTiles(const CGlbExtent* extent, 
		                                   glbInt32  level,
										   glbInt32& startTileCol,
										   glbInt32& startTileRow,
		                                   glbInt32& endTileCol,
										   glbInt32& endTileRow)=0;
	/** 
	* @brief 计算高程范围的分块索引
	* @brief 球面模式:
	*                原点是(-180,90),开始列从最小X开始,结束列在最大X
	*                                开始行从最大Y开始,结束行在最小Y
	*        平面模式:
	*                原点(0,0)，     开始列从最小X开始,结束列在最大X
	*                                开始行从最小Y开始,结束行在最大Y
	*
	* @param extent          空间范围
	* @param level           级别
	* @param startTileCol    开始列值
	* @param startTileRow    开始行值
	* @param endTileCol      结束列值
	* @param endTileRow      结束行值
	* @return 成功返回true，失败返回false
	*/
	virtual glbBool GetDemExtentCoverTiles(const CGlbExtent* extent, 
		                                  glbInt32  level,
										  glbInt32& startTileCol, 
										  glbInt32& startTileRow,
		                                  glbInt32& endTileCol, 
										  glbInt32& endTileRow)=0;
	/** 
	* @brief 获取影像一组分块的范围
	*
	* @param level           级别
	* @param startTileCol    开始列值
	* @param startTileRow    开始行值
	* @param endTileCol      结束列值
	* @param endTileRow      结束行值
	* @param extent          空间范围
	* @return 成功返回true，失败返回false
	*/
	virtual glbBool GetDomTilesCoverExtent(glbInt32 level,
		                                   glbInt32 startTileCol, 
										   glbInt32 startTileRow,
		                                   glbInt32 endTileCol, 
										   glbInt32 endTileRow, 
										   CGlbExtent* extent)=0;
	/** 
	* @brief 获取高程一组分块的范围
	*
	
	* @param level           级别
	* @param startTileCol    开始列值
	* @param startTileRow    开始行值
	* @param endTileCol      结束列值
	* @param endTileRow      结束行值
	* @param extent          空间范围
	* @return 成功返回true，失败返回false
	*/
	virtual glbBool GetDemTilesCoverExtent(glbInt32 level,
		                                   glbInt32 startTileCol, 
										   glbInt32 startTileRow,
		                                   glbInt32 endTileCol, 
										   glbInt32 endTileRow, 
										   CGlbExtent* extent)=0;
	/**
	* @brief 创建一个影像像素块
	*
	* @return 返回一个影像像素块
	*/
	virtual CGlbPixelBlock* CreateDemPixelBlock()=0;
	/**
	* @brief 创建一个高程像素块
	*
	* @return 返回一个高程像素块
	*/
	virtual CGlbPixelBlock* CreateDomPixelBlock()=0;
	/**
	* @brief 读取一个影像像素块
	*
	* @param level 影像级别
	* @param startColumn 开始读取的列数
	* @param startRow 开始读取的行数
	* @param pixelBlock 读入的像素块
	* @return 成功:true，失败:false
	*/
	virtual glbBool ReadDom(glbInt32 level, glbInt32 tileCol,glbInt32 tileRow, CGlbPixelBlock* pixelBlock)=0;
	/**
	* @brief 读取一个高程像素块
	*
	* @param level 高程级别
	* @param startColumn 开始读取的列数
	* @param startRow 开始读取的行数
	* @param pixelBlock 读入的像素块
	* @return 成功:true，失败:false
	*/
	virtual glbBool ReadDem(glbInt32 level, glbInt32 tileCol,glbInt32 tileRow, CGlbPixelBlock* pixelBlock)=0;
	/**
	* @brief 读取最高分辨率地形DEM
	* 
	* @param lonOrX  地形块涵盖的经度
	* @param latOrY  地形块涵盖的纬度
	* @param pixelBlock 读入的像素块
	* @return 成功:true，失败:false
	*/
	virtual glbBool ReadDem(glbDouble lonOrX,glbDouble latOrY, CGlbPixelBlock* pixelBlock) = 0;
	/**
	* @brief 写入一个影像像素块
	*
	* @param level      影像级别
	* @param tileCol    列号
	* @param tileRow    行号
	* @param neighbors  邻居是否存在Left,Right,Up,Down
	* @param pixelBlock 像素块
	* @return 成功:true，失败:false
	*/	
	virtual glbBool WriteDom(glbInt32 level, 
		                     glbInt32 tileCol, 
							 glbInt32 tileRow, 
							 GlbTileNeighbor& neighbors, 
							 CGlbPixelBlock* pixelBlock)=0;
	/**
	* @brief 写入一个高程块
	*
	* @param level       高程级别
	* @param tileCol     列号
	* @param tileRow     行号
	* @param neighbors   邻居是否存在Left,Right,Up,Down,LeftUp,RightUp,LeftDown,RightDown
	* @param pixelBlock  像素块
	* @return 成功:true，失败:false
	*/										 
	virtual glbBool WriteDem(glbInt32 level, 
		                     glbInt32 tileCol,
							 glbInt32 tileRow, 
							 GlbTileNeighbor& neighbors,
							 CGlbPixelBlock*  pixelBlock)=0;	
	/**
	* @brief 缝合球面DEM的边界（-180和180）
	*
	* @return 成功:true，失败:false
	*/
	virtual glbBool MergerGlobeDem()=0;
	/**
	* @brief 导入影像数据集
	*
	* @param dataset  影像数据集
	* @param minLevel 最小级数
	* @param maxLevel 最大级数
	* @param progress 进度信息
	* @return 成功:true，失败:false
	*/
	virtual glbBool ImportDomDataset(IGlbRasterDataset* dataset,
		                             glbInt32 minLevel,
								     glbInt32 maxLevel,
		                             IGlbProgress* progress=NULL)=0;
	/**
	* @brief 导入高程数据集
	*
	* @param dataset  栅格数据集
	* @param minLevel 最小级数
	* @param maxLevel 最大级数
	* @param progress 进度信息
	* @return 成功:true，失败:false
	*/
	virtual glbBool ImportDemDataset(IGlbRasterDataset* dataset,
		                             glbInt32 minLevel,
								     glbInt32 maxLevel,
		                             IGlbProgress* progress=NULL)=0;
};
/**
* @brief 网络瓦片数据集接口
*
* @version 1.0
* @date    2016-12-02 11:21
* @author  ML
*/
class GLBDATAENGINE_API IGlbWebMapTileDataset:public IGlbTerrainDataset
{
public:
	virtual glbWChar* GetUrl() = 0;
	virtual glbWChar* GetTileMatrixSetID() = 0;
	virtual glbWChar* GetImageFormat() = 0;
	virtual glbWChar* GetStyle() = 0;
	//virtual glbWChar* GetRSR() = 0;
	//virtual CGlbExtent* GetExtent() = 0;
	virtual glbWChar* GetTileUrl(glbInt32 tileMatrix, glbInt32 tileCol,glbInt32 tileRow)=0; 
	virtual glbBool		SetTileLevels(glbInt32 minLevel, glbInt32 maxLevel)=0;
	virtual glbBool		SetExtent(CGlbExtent* ext)=0;

	// 设置最低等级的dom的起始和终止行列号
	virtual glbBool SetDomMinLevelConverTiles(	glbInt32 startTileCol,
												glbInt32 startTileRow,
												glbInt32 endTileCol,
												glbInt32 endTileRow) = 0;

	virtual glbBool	SetTerrianUrl(const glbWChar* url) = 0;
	virtual const glbWChar* GetTerrainUrl() = 0;
	virtual const glbWChar* GetTerrainTileUrl(glbInt32 tileMatrix, glbInt32 tileCol,glbInt32 tileRow)=0;
	virtual glbBool  SetTerrainTileLevels(glbInt32 minLevel, glbInt32 maxLevel)=0;
};
/**
* @brief 网络图像数据源接口
*
* @version 1.0
* @date    2016-12-02 11:21
* @author  ML
*/
class GLBDATAENGINE_API IGlbWebMapDataSource:virtual public IGlbDataSource
{
public:
	/**
	* @brief 获取地形数据集
	*
	* @param terrainDatasetName 数据集名字
	* @return                  地形数据集,失败返回NULL
	*/
	virtual IGlbWebMapTileDataset* GetMapTileDataset(const glbWChar* mapTileDatasetName)=0;
	/**
	* @brief 创建一个webMap数据集
	*
	* @param url 网址
	* @param style 样式
	* @param format 图像格式 例如：image/png
	* @param mapWidth 图像宽度
	* @param mapHeight 图像高度
	* @param extent 范围
	* @return 返回数据集
	*/
	virtual IGlbDataset* CreateWebMapDataset(glbWChar* url, 
											glbWChar* style, 
											glbWChar* format, 
											glbInt32 mapWidth, 
											glbInt32 mapHeight, 
											CGlbExtent* extent)=0;
	/**
	* @brief 创建一个webMapTile数据集
	*
	* @param url 网址
	* @param style 样式
	* @param format 图像格式 例如：image/png
	* @param tileMatrixSetID 瓦片集合
	* @param tileWidth 瓦片宽度
	* @param tileHeight 瓦片高度
	* @param extent 范围
	* @param minimumLevel 最小级数
	* @param maximumLevel 最大级数
	* @param ellipsoid	是否wgs84坐标系
	* @return 返回数据集
	*/
	virtual IGlbDataset* CreateWebMapTileDataset(glbWChar* url,
												glbWChar* style,
												glbWChar* format,
												glbWChar* tileMatrixSetID,
												glbInt32 tileWidth,
												glbInt32 tileHeight,
												CGlbExtent* extent, 
												glbInt32 minimumLevel,
												glbInt32 maximumLevel,
												glbBool ellipsoid)=0;
	/**
	* @brief 创建天地图WGS84网络瓦片数据集
	*
	* @param name 数据集名称
	* @return 返回数据集
	*/
	virtual IGlbTerrainDataset* CreateTianDiTuWGS84WebMapTileDataset(glbWChar* name,glbWChar* alias)=0;
};
/**
* @brief 网络图像数据集接口
*
* @version 1.0
* @date    2016-12-02 11:21
* @author  ML
*/
class GLBDATAENGINE_API IGlbWebMapDataset:public IGlbDataset
{
public:
	 virtual glbWChar* GetUrl() = 0;
	 virtual glbBool ReadImage(CGlbPixelBlock* pixelBlock) = 0;
	 virtual glbWChar* GetImageFormat() = 0;
	 virtual glbInt32 GetWidth() = 0;
	 virtual glbInt32 GetHeight() = 0;
	 //virtual CGlbExtent* GetExtent() = 0;
	 virtual glbBool GetTransparent() = 0;
	 virtual glbDouble GetElevation() = 0;
	 virtual glbWChar* GetRSR() = 0;
	 virtual glbWChar* GetLayers() = 0;
	 virtual glbBool IsEllipsoid() = 0;
	 
};

GLBDATAENGINE_API extern glbInt32	 getPixelSize(GlbPixelTypeEnum pixelType);
GLBDATAENGINE_API extern GLBVARIANT* defautToVariant(CGlbField* field);

//<!--tile文件命名规范：sgrid名称_level_row_col.sgr-->
struct GlbGridTile
{
	GlbGridTile()
	{
		level = row = col = layer = -1;
		cnt = 0;
		z = NULL;
		isValid = NULL;
	}
	int level;
	int row;
	int col;
	int layer;  // layer=0
	CGlbExtent extent;
	int cnt;
	double* z; // (NI*NJ*(NK+1)
	BYTE * isValid;
};
typedef GlbGridTile tileStruct;


struct GlbExtent
{
	double minX;
	double minY;
	double minZ;
	double maxX;
	double maxY;
	double maxZ;
};
//<!--tile文件命名规范：sgrid名称_level_row_col.sgrface-->
struct GLBDATAENGINE_API GlbGridTileSurface
{
	GlbGridTileSurface()
	{
		level = row = col = layer = -1;
		iStep = jStep = 0.0;			
		quatCount = 0;
		index = NULL;
		quatVertex = NULL;
		color = NULL;
	}	
public:
	//////////////////////////////////////////////////////////////////////////
	int level;
	int row;
	int col;
	int layer;  // layer=0
	//int tmStamp; // 时间戳
	GlbExtent extent;
	double iStep;
	double jStep;

	long quatCount;//表面数量
	long* index;//表面所在格子的索引[quatCount]
	double* quatVertex;//表面的顶点坐标[quatCount*4*3]
	double* color;//表面的颜色[quatCount*3]
public:
	void write(std::string filePath);
	void read(std::string filePath);
};
//typedef GlbGridTileSurface tileSurfaceStruct;