#ifndef  __FT_RY3_HEADER_H
#define  __FT_RY3_HEADER_H

#include <Windows.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef void*  RY_HANDLE;

//������
#define   RY3_SUCCESS                       0x00000000          // �����ɹ�
#define   RY3_NOT_FOUND                     0xF0000001          // δ�ҵ�ָ�����豸
#define   RY3_INVALID_PARAMETER			    0xF0000002			// ��������
#define   RY3_COMM_ERROR					0xF0000003			// ͨѶ����
#define   RY3_INSUFFICIENT_BUFFER		    0xF0000004			// �������ռ䲻��
#define   RY3_NO_LIST					    0xF0000005			// û���ҵ��豸�б�
#define   RY3_DEVPIN_NOT_CHECK				0xF0000006			// �����̿���û����֤
#define   RY3_USERPIN_NOT_CHECK			    0xF0000007			// �û�����û����֤
#define	  RY3_RSA_FILE_FORMAT_ERROR	        0xF0000008			// RSA�ļ���ʽ����
#define   RY3_DIR_NOT_FOUND				    0xF0000009			// Ŀ¼û���ҵ�
#define   RY3_ACCESS_DENIED				    0xF000000A			// ���ʱ��ܾ�
#define   RY3_ALREADY_INITIALIZED		    0xF000000B		    // ��Ʒ�Ѿ���ʼ��
#define   RY3_INCORRECT_PIN				    0xF0000C00		    // ���벻��ȷ
#define   RY3_DF_SIZE						0xF000000D		    // ָ����Ŀ¼�ռ��С����
#define   RY3_FILE_EXIST					0xF000000E		    // �ļ��Ѵ���
#define   RY3_UNSUPPORTED					0xF000000F			// ���ܲ�֧�ֻ���δ�����ļ�ϵͳ
#define   RY3_FILE_NOT_FOUND				0xF0000010			// δ�ҵ�ָ�����ļ�
#define	  RY3_ALREADY_OPENED				0xF0000011		    // ���Ѿ�����
#define   RY3_DIRECTORY_EXIST				0xF0000012			// Ŀ¼�Ѵ���
#define   RY3_CODE_RANGE					0xF0000013			// ������ڴ��ַ���
#define   RY3_INVALID_POINTER				0xF0000014			// ����������ָ��
#define   RY3_GENERAL_FILESYSTEM			0xF0000015		    // �����ļ�ϵͳ���� 
#define   RY3_OFFSET_BEYOND				    0xF0000016		    // �ļ�ƫ���������ļ���С
#define   RY3_FILE_TYPE_MISMATCH			0xF0000017		    // �ļ����Ͳ�ƥ��
#define   RY3_PIN_BLOCKED					0xF0000018		    // PIN������
#define   RY3_INVALID_HANDLE				0xF0000019			// ��Ч�ľ��

#define	  RY3_NETLOCK_MAXCON				0xF000001A			// ����������
#define   RY3_NETLOCK_ERRORNETLOCK			0xF000001B			// ����������
#define	  RY3_NETLOCK_SUCCESS				0xF000001C			// �ɹ�����
#define	  RY3_NETLOCK_CONNECTED				0xF000001D			// �Ѿ�����������
#define   RY3_NETLOCK_GETMACERROR			0xF000001E			// ��ȡ��������MAC��ַ����
#define   RY3_NETLOCK_CTHREAD				0xF000001F			// ���������������߳�ʧ��
#define   RY3_NETLOCK_CLOSE					0xF0000020			// �ر�����ʧ��
#define   RY3_NETLOCK_BREATH				0xF0000021			// ʵʱ����ʧ��
#define   RY3_NETLOCK_DELETE				0xF0000022			// ɾ������ʧ��
#define   RY3_NETLOCK_PWDUSER				0xF0000023			// �û����������
#define   RY3_NETLOCK_ERRORATTR				0xF0000024			// ��ȡģ�����
#define   RY3_NETLOCK_SHUTDOWN				0xF0000025			// �жϽ����Ƿ��Ѿ���ǿ�ƶ˶Ͽ��򱻹ر�
#define   RY3_NETLOCK_MODUNUSEFUL			0xF0000026			// ģ�鲻����
#define	  RY3_NETLOCK_FAILD					0xF0000027			// ʧ�ܻ������Ĳ���֪����


#define   RY3_ERROR_UNKNOWN				    0xFFFFFFFF			// δ֪�Ĵ���



#define   RY3_C51_SUCCESS				    0x00000000			//	�ɹ�
#define   RY3_C51_UNKNOWN				    0x00000001			//	δ֪����
#define   RY3_C51_INVALID_PARAMETER	        0x00000002			//	��Ч�Ĳ���
#define   RY3_C51_INVALID_ADDRESS		    0x00000003			//	��Ч�ĵ�ַ,�������ַԽ��
#define   RY3_C51_INVALID_SIZE			    0x00000004			//	��Ч�ĳ���
#define   RY3_C51_FILE_NOT_FOUND		    0x00000005			//	�ļ�û�ҵ�
#define   RY3_C51_ACCESS_DENIED		        0x00000006			//	�����ļ�ʧ��
#define   RY3_C51_FILE_SELECT			    0x00000007			//	�ļ��򿪸����Ѵ�����
#define   RY3_C51_INVALID_HANDLE		    0x00000008			//	��Ч���ļ����
#define   RY3_C51_FILE_OUT_OF_RANGE	        0x00000009			//	�ļ���дԽ��
#define   RY3_C51_FILE_TYPE_MISMATCH	    0x0000000A			//	�ļ����ڵ����Ͳ�ƥ��
#define   RY3_C51_FILE_SIZE_MISMATCH	    0x0000000B			//	�ļ����ڵ����Ȳ�ƥ��
#define   RY3_C51_NO_SPACE				    0x0000000C			//	�ļ��пռ䲻��
#define   RY3_C51_FILE_EXIST			    0x0000000D			//	�ļ�����Ŀ¼�Ѵ���
#define   RY3_C51_INVALID_KEY_FORMAT	    0x0000000E			//	��Ч��RSA��Կ�ļ���ʽ
#define   RY3_C51_KEY_LEN_MISMATCH		    0x0000000F			//  �û��������Կ������ʵ�ʳ��Ȳ�ƥ��
#define   RY3_C51_RSA_INVALID_KEY_FILE	    0x00000010			//	�ļ����Ͳ�����Ҫ��
#define   RY3_C51_RSA_ENC_DEC_FAILED	    0x00000011			//	RSA���ܽ���ʧ��
#define   RY3_C51_RSA_SIGN_VERI_FAILED	    0x00000012			//	RSAǩ����֤ʧ��
#define   RY3_C51_SHA1					    0x00000013			//  SHA1�������
#define   RY3_C51_MD5					    0x00000014			//  MD5�������
#define   RY3_C51_INVALID_ADDERSS		    0x00000015			//  ��Ч���ڴ�ָ��
#define   RY3_C51_EEPROM				    0x00000016			//  дEEPROM����

#define   RY3_ERROR_NETERROR				0xF00000FF			// ���紫�����
#define   RY3_ERROR_PARAMETERS  		    0xF0000002; 		// �������Ϸ�

//===============================================================================
//����RY3 (����VendorID�ִ�, ����ҵ���RY3��Ŀ)
DWORD WINAPI RY3_Find(char* pVendorID, int* pCount);

//��ָ����Ŀ��RY3 (������Ŀ,����1,������)
DWORD WINAPI RY3_Open(RY_HANDLE* pHandle, int Item);

//�ر�RY3 (IsReset��ʾ�Ƿ�λ������,�����ȫ״̬)
DWORD WINAPI RY3_Close(RY_HANDLE handle, BOOL IsReset);

//��ȡӲ�����к�(���16�ֽڵ��ַ���)
DWORD WINAPI RY3_GetHardID(RY_HANDLE handle, char* pbuf);

//��ȡ�����(lenָ����������������)
DWORD WINAPI RY3_GenRandom(RY_HANDLE handle, int len_need, BYTE* pOutbuf);

//��ȡ���пռ��С
DWORD WINAPI RY3_GetFreeSize(RY_HANDLE handle, int* pSize);

//���洢��(0-8192�ֽ�)
DWORD WINAPI RY3_Read(RY_HANDLE handle, int offset, BYTE* pOutbuf, int len);

//д������
DWORD WINAPI RY3_Write(RY_HANDLE handle,int offset,BYTE*pInBuf,int len);

//���������ڴ���(0-32�ֽ�)
DWORD WINAPI RY3_ReadShare(RY_HANDLE handle, int offset, BYTE* pbuf, int len);

//ִ���ļ�
DWORD WINAPI RY3_ExecuteFile(RY_HANDLE handle, WORD FileID, BYTE* pInBuf, int InSize, BYTE* pOutBuf, int* pOutSize);

//��Կ�����㷨(kidȡֵ��Χ:1-8)
//�������ݳ��ȱ�����128��������
DWORD WINAPI RY3_PublicEncrypt(RY_HANDLE handle, int kid, BYTE* pBuf, int len);

//˽Կ�����㷨(kidȡֵ��Χ:1-8)
//�������ݳ��ȱ�����128��������
DWORD WINAPI RY3_PrivateDecrypt(RY_HANDLE handle, int kid, BYTE* pBuf, int len);

//MD5ǩ���㷨
DWORD WINAPI RY3_MD5(RY_HANDLE handle, BYTE* pBuf, int len, BYTE* pMD5);

//SHA1ǩ���㷨
DWORD WINAPI RY3_SHA1(RY_HANDLE handle, BYTE* pBuf, int len, BYTE* pSHA1);

#define FLAG_ENCODE  0 //����
#define FLAG_DECODE  1 //����

//3DES�ӽ����㷨(kidȡֵ��Χ:1-8)
//�������ݳ��ȱ�����8��������
DWORD WINAPI RY3_3DES(RY_HANDLE handle, int kid, int flag, BYTE* pBuf, int len);

//��������������
DWORD WINAPI RY3_OpenNetMod(RY_HANDLE handle,BYTE mod_num);		//��������ģ��
DWORD WINAPI RY3_CloseNetMod(RY_HANDLE &handle,BYTE mod_num);	//�õ�������ģ��
DWORD WINAPI RY3_GetModAttribute(RY_HANDLE handle,BYTE mod_num,BYTE *pOutBuf,int * pOutSize);	//��ȡ����ģ������
DWORD WINAPI RY3_QueryNetMod(RY_HANDLE handle,BYTE mod_num,BYTE* pOutBuf, int* pOutSize,int HostIndex);	//��ѯģ������
DWORD WINAPI RY3_QueryNetMod_Client(RY_HANDLE handle,BYTE mod_num,BYTE *pOutBuf,int *pOutSize,int HostIndex,int ClienIndex);
DWORD WINAPI RY3_DeleteNetMod(RY_HANDLE handle,BYTE mod_num,BYTE *pbyMACAddr,DWORD dwProcessID,BYTE *pUser,BYTE *pPwd);

DWORD WINAPI RY3_CheckProcUse(RY_HANDLE handle,BYTE mod_num);//����Ƿ�ǰ�Ľ��̿���


#ifdef __cplusplus 
}
#endif

#endif	//__FT_RY3_HEADER_H


