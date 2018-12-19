#ifndef  __FT_RY3_HEADER_H
#define  __FT_RY3_HEADER_H

#include <Windows.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef void*  RY_HANDLE;

//错误码
#define   RY3_SUCCESS                       0x00000000          // 操作成功
#define   RY3_NOT_FOUND                     0xF0000001          // 未找到指定的设备
#define   RY3_INVALID_PARAMETER			    0xF0000002			// 参数错误
#define   RY3_COMM_ERROR					0xF0000003			// 通讯错误
#define   RY3_INSUFFICIENT_BUFFER		    0xF0000004			// 缓冲区空间不足
#define   RY3_NO_LIST					    0xF0000005			// 没有找到设备列表
#define   RY3_DEVPIN_NOT_CHECK				0xF0000006			// 开发商口令没有验证
#define   RY3_USERPIN_NOT_CHECK			    0xF0000007			// 用户口令没有验证
#define	  RY3_RSA_FILE_FORMAT_ERROR	        0xF0000008			// RSA文件格式错误
#define   RY3_DIR_NOT_FOUND				    0xF0000009			// 目录没有找到
#define   RY3_ACCESS_DENIED				    0xF000000A			// 访问被拒绝
#define   RY3_ALREADY_INITIALIZED		    0xF000000B		    // 产品已经初始化
#define   RY3_INCORRECT_PIN				    0xF0000C00		    // 密码不正确
#define   RY3_DF_SIZE						0xF000000D		    // 指定的目录空间大小不够
#define   RY3_FILE_EXIST					0xF000000E		    // 文件已存在
#define   RY3_UNSUPPORTED					0xF000000F			// 功能不支持或尚未建立文件系统
#define   RY3_FILE_NOT_FOUND				0xF0000010			// 未找到指定的文件
#define	  RY3_ALREADY_OPENED				0xF0000011		    // 卡已经被打开
#define   RY3_DIRECTORY_EXIST				0xF0000012			// 目录已存在
#define   RY3_CODE_RANGE					0xF0000013			// 虚拟机内存地址溢出
#define   RY3_INVALID_POINTER				0xF0000014			// 虚拟机错误的指针
#define   RY3_GENERAL_FILESYSTEM			0xF0000015		    // 常规文件系统错误 
#define   RY3_OFFSET_BEYOND				    0xF0000016		    // 文件偏移量超出文件大小
#define   RY3_FILE_TYPE_MISMATCH			0xF0000017		    // 文件类型不匹配
#define   RY3_PIN_BLOCKED					0xF0000018		    // PIN码锁死
#define   RY3_INVALID_HANDLE				0xF0000019			// 无效的句柄

#define	  RY3_NETLOCK_MAXCON				0xF000001A			// 连接数已满
#define   RY3_NETLOCK_ERRORNETLOCK			0xF000001B			// 不是网络锁
#define	  RY3_NETLOCK_SUCCESS				0xF000001C			// 成功返回
#define	  RY3_NETLOCK_CONNECTED				0xF000001D			// 已经建立了连接
#define   RY3_NETLOCK_GETMACERROR			0xF000001E			// 获取本地主机MAC地址错误
#define   RY3_NETLOCK_CTHREAD				0xF000001F			// 创建网络锁呼吸线程失败
#define   RY3_NETLOCK_CLOSE					0xF0000020			// 关闭连接失败
#define   RY3_NETLOCK_BREATH				0xF0000021			// 实时更新失败
#define   RY3_NETLOCK_DELETE				0xF0000022			// 删除数据失败
#define   RY3_NETLOCK_PWDUSER				0xF0000023			// 用户名密码错误
#define   RY3_NETLOCK_ERRORATTR				0xF0000024			// 获取模块错误
#define   RY3_NETLOCK_SHUTDOWN				0xF0000025			// 判断进程是否已经被强制端断开或被关闭
#define   RY3_NETLOCK_MODUNUSEFUL			0xF0000026			// 模块不可用
#define	  RY3_NETLOCK_FAILD					0xF0000027			// 失败或其他的不可知错误


#define   RY3_ERROR_UNKNOWN				    0xFFFFFFFF			// 未知的错误



#define   RY3_C51_SUCCESS				    0x00000000			//	成功
#define   RY3_C51_UNKNOWN				    0x00000001			//	未知错误
#define   RY3_C51_INVALID_PARAMETER	        0x00000002			//	无效的参数
#define   RY3_C51_INVALID_ADDRESS		    0x00000003			//	无效的地址,虚拟机地址越界
#define   RY3_C51_INVALID_SIZE			    0x00000004			//	无效的长度
#define   RY3_C51_FILE_NOT_FOUND		    0x00000005			//	文件没找到
#define   RY3_C51_ACCESS_DENIED		        0x00000006			//	访问文件失败
#define   RY3_C51_FILE_SELECT			    0x00000007			//	文件打开个数已达上限
#define   RY3_C51_INVALID_HANDLE		    0x00000008			//	无效的文件句柄
#define   RY3_C51_FILE_OUT_OF_RANGE	        0x00000009			//	文件读写越界
#define   RY3_C51_FILE_TYPE_MISMATCH	    0x0000000A			//	文件存在但类型不匹配
#define   RY3_C51_FILE_SIZE_MISMATCH	    0x0000000B			//	文件存在但长度不匹配
#define   RY3_C51_NO_SPACE				    0x0000000C			//	文件夹空间不足
#define   RY3_C51_FILE_EXIST			    0x0000000D			//	文件或子目录已存在
#define   RY3_C51_INVALID_KEY_FORMAT	    0x0000000E			//	无效的RSA密钥文件格式
#define   RY3_C51_KEY_LEN_MISMATCH		    0x0000000F			//  用户传入的密钥长度与实际长度不匹配
#define   RY3_C51_RSA_INVALID_KEY_FILE	    0x00000010			//	文件类型不符合要求
#define   RY3_C51_RSA_ENC_DEC_FAILED	    0x00000011			//	RSA加密解密失败
#define   RY3_C51_RSA_SIGN_VERI_FAILED	    0x00000012			//	RSA签名验证失败
#define   RY3_C51_SHA1					    0x00000013			//  SHA1计算错误
#define   RY3_C51_MD5					    0x00000014			//  MD5计算错误
#define   RY3_C51_INVALID_ADDERSS		    0x00000015			//  无效的内存指针
#define   RY3_C51_EEPROM				    0x00000016			//  写EEPROM错误

#define   RY3_ERROR_NETERROR				0xF00000FF			// 网络传输出错
#define   RY3_ERROR_PARAMETERS  		    0xF0000002; 		// 参数不合法

//===============================================================================
//查找RY3 (输入VendorID字串, 输出找到的RY3数目)
DWORD WINAPI RY3_Find(char* pVendorID, int* pCount);

//打开指定条目的RY3 (输入条目,基于1,输出句柄)
DWORD WINAPI RY3_Open(RY_HANDLE* pHandle, int Item);

//关闭RY3 (IsReset表示是否复位加密锁,清除安全状态)
DWORD WINAPI RY3_Close(RY_HANDLE handle, BOOL IsReset);

//获取硬件序列号(输出16字节的字符串)
DWORD WINAPI RY3_GetHardID(RY_HANDLE handle, char* pbuf);

//获取随机数(len指定所需的随机数长度)
DWORD WINAPI RY3_GenRandom(RY_HANDLE handle, int len_need, BYTE* pOutbuf);

//获取空闲空间大小
DWORD WINAPI RY3_GetFreeSize(RY_HANDLE handle, int* pSize);

//读存储区(0-8192字节)
DWORD WINAPI RY3_Read(RY_HANDLE handle, int offset, BYTE* pOutbuf, int len);

//写入数据
DWORD WINAPI RY3_Write(RY_HANDLE handle,int offset,BYTE*pInBuf,int len);

//读锁共享内存区(0-32字节)
DWORD WINAPI RY3_ReadShare(RY_HANDLE handle, int offset, BYTE* pbuf, int len);

//执行文件
DWORD WINAPI RY3_ExecuteFile(RY_HANDLE handle, WORD FileID, BYTE* pInBuf, int InSize, BYTE* pOutBuf, int* pOutSize);

//公钥加密算法(kid取值范围:1-8)
//输入数据长度必须是128的整数倍
DWORD WINAPI RY3_PublicEncrypt(RY_HANDLE handle, int kid, BYTE* pBuf, int len);

//私钥解密算法(kid取值范围:1-8)
//输入数据长度必须是128的整数倍
DWORD WINAPI RY3_PrivateDecrypt(RY_HANDLE handle, int kid, BYTE* pBuf, int len);

//MD5签名算法
DWORD WINAPI RY3_MD5(RY_HANDLE handle, BYTE* pBuf, int len, BYTE* pMD5);

//SHA1签名算法
DWORD WINAPI RY3_SHA1(RY_HANDLE handle, BYTE* pBuf, int len, BYTE* pSHA1);

#define FLAG_ENCODE  0 //加密
#define FLAG_DECODE  1 //解密

//3DES加解密算法(kid取值范围:1-8)
//输入数据长度必须是8的整数倍
DWORD WINAPI RY3_3DES(RY_HANDLE handle, int kid, int flag, BYTE* pBuf, int len);

//网络锁计数功能
DWORD WINAPI RY3_OpenNetMod(RY_HANDLE handle,BYTE mod_num);		//打开网络锁模块
DWORD WINAPI RY3_CloseNetMod(RY_HANDLE &handle,BYTE mod_num);	//得到网络锁模块
DWORD WINAPI RY3_GetModAttribute(RY_HANDLE handle,BYTE mod_num,BYTE *pOutBuf,int * pOutSize);	//获取所有模块属性
DWORD WINAPI RY3_QueryNetMod(RY_HANDLE handle,BYTE mod_num,BYTE* pOutBuf, int* pOutSize,int HostIndex);	//查询模块数据
DWORD WINAPI RY3_QueryNetMod_Client(RY_HANDLE handle,BYTE mod_num,BYTE *pOutBuf,int *pOutSize,int HostIndex,int ClienIndex);
DWORD WINAPI RY3_DeleteNetMod(RY_HANDLE handle,BYTE mod_num,BYTE *pbyMACAddr,DWORD dwProcessID,BYTE *pUser,BYTE *pPwd);

DWORD WINAPI RY3_CheckProcUse(RY_HANDLE handle,BYTE mod_num);//检查是否当前的进程可用


#ifdef __cplusplus 
}
#endif

#endif	//__FT_RY3_HEADER_H


