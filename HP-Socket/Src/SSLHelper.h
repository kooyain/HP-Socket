/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 3.5.4
 * Author	: Bruce Liang
 * Website	: http://www.jessma.org
 * Project	: https://github.com/ldcsaa
 * Blog		: http://www.cnblogs.com/ldcsaa
 * Wiki		: http://www.oschina.net/p/hp-socket
 * QQ Group	: 75375912
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "openssl/ssl.h"
#include "SocketHelper.h"
#include "SocketInterface-SSL.h"
#include "../../Common/Src/RingBuffer.h"

/************************************************************************
���ƣ�SSL ����״̬
��������ʶ��ǰ���ӵ� SSL ����״̬
************************************************************************/
enum EnSSLHandShakeStatus
{
	SSL_HSS_INIT	= 0,	// ��ʼ״̬
	SSL_HSS_PROC	= 1,	// ��������
	SSL_HSS_SUCC	= 2,	// ���ֳɹ�
};

/* SSL CRYPTO DYNLOCK �ṹ */
typedef struct CRYPTO_dynlock_value
{
	CSimpleRWLock cs;
} DynamicLock;

/************************************************************************
���ƣ�SSL Context
��������ʼ�������� SSL ȫ�����л���
************************************************************************/
class CSSLContext
{
public:
	/* ��ȡ CSSLContext �������� */
	static CSSLContext& getInstance()	{return sm_Instance;}

public:

	/*
	* ���ƣ���ʼ�� SSL ȫ�ֻ�������
	* ������SSL ȫ�ֻ������������� SSL ͨ���������ǰ��ɳ�ʼ������������ʧ��
	*		
	* ������		enSessionMode			-- SSL ����ģʽ���ο� EnSSLSessionMode��
	*			iVerifyMode				-- SSL ��֤ģʽ���ο� EnSSLVerifyMode��
	*			lpszPemCertFile			-- ֤���ļ����ͻ��˿�ѡ��
	*			lpszPemKeyFile			-- ˽Կ�ļ����ͻ��˿�ѡ��
	*			lpszKeyPasswod			-- ˽Կ���루û��������Ϊ�գ�
	*			lpszCAPemCertFileOrPath	-- CA ֤���ļ���Ŀ¼��������֤��ͻ��˿�ѡ��
	*
	* ����ֵ��	TRUE	-- �ɹ�
	*			FALSE	-- ʧ�ܣ���ͨ�� ::GetLastError() ��ȡʧ��ԭ��
	*/
	BOOL Initialize(EnSSLSessionMode enSessionMode, int iVerifyMode = SSL_VM_NONE, LPCTSTR lpszPemCertFile = nullptr, LPCTSTR lpszPemKeyFile = nullptr, LPCTSTR lpszKeyPasswod = nullptr, LPCTSTR lpszCAPemCertFileOrPath = nullptr);

	/*
	* ���ƣ����� SSL ȫ�����л���
	* ���������� SSL ȫ�����л��������� SSL ����ڴ�
	*		1��CSSLContext �������������Զ����ñ�����
	*		2����Ҫ�������� SSL ȫ�ֻ�������ʱ����Ҫ�ȵ��ñ���������ԭ�ȵĻ�������
	*		
	* ������	��
	* 
	* ����ֵ����
	*/
	void Cleanup();

	/*
	* ���ƣ������ֲ߳̾����� SSL ��Դ
	* �������κ�һ������ SSL ���̣߳���ͨ�Ž���ʱ����Ҫ�����ֲ߳̾����� SSL ��Դ
	*		1�����̺߳� HP-Socket �����߳���ͨ�Ž���ʱ���Զ������ֲ߳̾����� SSL ��Դ����ˣ�һ������²����ֹ����ñ�����
	*		2����������£����Զ����̲߳��� HP-Socket ͨ�Ų�������鵽 SSL �ڴ�й©ʱ������ÿ��ͨ�Ž���ʱ�Զ����̵߳��ñ�����
	*		
	* ������	��
	* 
	* ����ֵ����
	*/
	void RemoveThreadLocalState()	{CleanupThreadState();}

	/* ��� SSL ȫ�����л����Ƿ��ʼ����� */
	BOOL IsValid					() const	{return m_bValid;}
	/* ��ȡ SSL ȫ�����л��� SSL_CTX ���� */
	SSL_CTX* GetContext				() const	{return m_sslCtx;}
	/* ��ȡ SSL ȫ�����л���������ģʽ������ģʽ�ο���EnSSLSessionMode */
	EnSSLSessionMode GetSessionMode	() const	{return m_enSessionMode;}

private:

	CSSLContext()
	: m_bValid			(FALSE)
	, m_iLockNum		(0)
	, m_pcsLocks		(nullptr)
	, m_sslCtx			(nullptr)
	, m_enSessionMode	(SSL_SM_SERVER)
	{

	}

	~CSSLContext()
	{
		Cleanup();
	}

private:

	void CreateContext();
	BOOL LoadCertAndKey(int iVerifyMode, LPCTSTR lpszPemCertFile, LPCTSTR lpszPemKeyFile, LPCTSTR lpszKeyPasswod, LPCTSTR lpszCAPemCertFileOrPath);

private:

	static void CleanupGlobalState();
	static void CleanupThreadState();

	static void ssl_lock_callback(int mode, int n, const char *file, int line);
	static CRYPTO_dynlock_value* ssl_lock_dyn_create_callback(const char *file, int line);
	static void ssl_lock_dyn_callback(int mode, CRYPTO_dynlock_value* l, const char *file, int line);
	static void ssl_lock_dyn_destroy_callback(CRYPTO_dynlock_value* l, const char *file, int line);

private:

	BOOL				m_bValid;
	EnSSLSessionMode	m_enSessionMode;
	int					m_iLockNum;
	CSimpleRWLock*		m_pcsLocks;
	SSL_CTX*			m_sslCtx;

private:

	static		CSSLContext sm_Instance;
};

extern CSSLContext& g_SSL;

class CSSLSession
{
public:

	BOOL WriteRecvChannel(const BYTE* pData, int iLength);
	BOOL ReadRecvChannel();

	BOOL WriteSendChannel(const BYTE* pData, int iLength);
	BOOL WriteSendChannel(const WSABUF pBuffers[], int iCount);
	BOOL ReadSendChannel();

	const WSABUF& GetRecvBuffer()	const	{return m_bufRecv;}
	const WSABUF& GetSendBuffer()	const	{return m_bufSend;}

	CSSLSession*			Renew();
	BOOL					Reset();
	BOOL					IsValid()		const	{return GetStatus() != SSL_HSS_INIT;}
	BOOL					IsHandShaking()	const	{return GetStatus() == SSL_HSS_PROC;}
	BOOL					IsReady()		const	{return GetStatus() == SSL_HSS_SUCC;}
	EnSSLHandShakeStatus	GetStatus()		const	{return m_enStatus;}
	DWORD					GetFreeTime()	const	{return m_dwFreeTime;}
	CCriSec&				GetSendLock()			{return m_csSend;}

private:

	BOOL IsFatalError(int iBytes);

public:

	CSSLSession(CItemPool& itPool)
	: m_enStatus(SSL_HSS_INIT)
	, m_itPool	(itPool)
	, m_ssl		(nullptr)
	, m_bioSend	(nullptr)
	, m_bioRecv	(nullptr)
	, m_pitSend	(nullptr)
	, m_pitRecv	(nullptr)
	{

	}

	~CSSLSession()
	{
		Reset();
	}

private:
	CItemPool&				m_itPool;
	CCriSec					m_csSend;

	DWORD					m_dwFreeTime;
	EnSSLHandShakeStatus	m_enStatus;

	SSL* m_ssl;
	BIO* m_bioSend;
	BIO* m_bioRecv;

	TItem*		m_pitSend;
	TItem*		m_pitRecv;
	WSABUF		m_bufSend;
	WSABUF		m_bufRecv;
};

class CSSLSessionPool
{
	typedef CRingPool<CSSLSession>	TSSLSessionList;
	typedef CCASQueue<CSSLSession>	TSSLSessionQueue;

public:
	CSSLSession*	PickFreeSession	();
	void			PutFreeSession	(CSSLSession* pSession);

	void			Prepare			();
	void			Clear			();

private:
	void ReleaseGCSession	(BOOL bForce = FALSE);

public:
	void SetItemCapacity	(DWORD dwItemCapacity)		{m_itPool.SetItemCapacity(dwItemCapacity);}
	void SetItemPoolSize	(DWORD dwItemPoolSize)		{m_itPool.SetPoolSize(dwItemPoolSize);}
	void SetItemPoolHold	(DWORD dwItemPoolHold)		{m_itPool.SetPoolHold(dwItemPoolHold);}

	void SetSessionLockTime	(DWORD dwSessionLockTime)	{m_dwSessionLockTime = dwSessionLockTime;}
	void SetSessionPoolSize	(DWORD dwSessionPoolSize)	{m_dwSessionPoolSize = dwSessionPoolSize;}
	void SetSessionPoolHold	(DWORD dwSessionPoolHold)	{m_dwSessionPoolHold = dwSessionPoolHold;}

	DWORD GetItemCapacity	()	{return m_itPool.GetItemCapacity();}
	DWORD GetItemPoolSize	()	{return m_itPool.GetPoolSize();}
	DWORD GetItemPoolHold	()	{return m_itPool.GetPoolHold();}

	DWORD GetSessionLockTime()	{return m_dwSessionLockTime;}
	DWORD GetSessionPoolSize()	{return m_dwSessionPoolSize;}
	DWORD GetSessionPoolHold()	{return m_dwSessionPoolHold;}

public:
	CSSLSessionPool(DWORD dwPoolSize = DEFAULT_SESSION_POOL_SIZE,
					DWORD dwPoolHold = DEFAULT_SESSION_POOL_HOLD,
					DWORD dwLockTime = DEFAULT_SESSION_LOCK_TIME)
	: m_dwSessionPoolSize(dwPoolSize)
	, m_dwSessionPoolHold(dwPoolHold)
	, m_dwSessionLockTime(dwLockTime)
	{

	}

	~CSSLSessionPool()	{Clear();}

	DECLARE_NO_COPY_CLASS(CSSLSessionPool)

public:
	static const DWORD DEFAULT_ITEM_CAPACITY;
	static const DWORD DEFAULT_ITEM_POOL_SIZE;
	static const DWORD DEFAULT_ITEM_POOL_HOLD;
	static const DWORD DEFAULT_SESSION_LOCK_TIME;
	static const DWORD DEFAULT_SESSION_POOL_SIZE;
	static const DWORD DEFAULT_SESSION_POOL_HOLD;

private:
	CItemPool			m_itPool;

	DWORD				m_dwSessionLockTime;
	DWORD				m_dwSessionPoolSize;
	DWORD				m_dwSessionPoolHold;

	TSSLSessionList		m_lsFreeSession;
	TSSLSessionQueue	m_lsGCSession;
};

template<class T> EnHandleResult ProcessHandShake(T* pThis, TSocketObj* pSocketObj, CSSLSession* pSession)
{
	EnHandleResult result = HR_OK;

	CCriSecLock locallock(pSession->GetSendLock());

	while(TRUE)
	{
		VERIFY(pSession->ReadSendChannel());
		const WSABUF& buffer = pSession->GetSendBuffer();

		if(buffer.len == 0)
			break;

		if(!pThis->DoSendPackets(pSocketObj, &buffer, 1))
		{
			result = HR_ERROR;
			break;
		}
	}

	return result;
}

template<class T> EnHandleResult ProcessHandShake(T* pThis, CSSLSession* pSession)
{
	EnHandleResult result = HR_OK;

	CCriSecLock locallock(pSession->GetSendLock());

	while(TRUE)
	{
		VERIFY(pSession->ReadSendChannel());
		const WSABUF& buffer = pSession->GetSendBuffer();

		if(buffer.len == 0)
			break;

		if(!pThis->DoSendPackets(&buffer, 1))
		{
			result = HR_ERROR;
			break;
		}
	}

	return result;
}

template<class T, class S> EnHandleResult ProcessReceive(T* pThis, S* pSocketObj, CSSLSession* pSession, const BYTE* pData, int iLength)
{
	if(!pSession->WriteRecvChannel(pData, iLength))
		return HR_ERROR;

	EnHandleResult result			= HR_OK;
	EnSSLHandShakeStatus enStatus	= pSession->GetStatus();

	while(TRUE)
	{
		if(!pSession->ReadRecvChannel())
			return HR_ERROR;

		if(enStatus == SSL_HSS_PROC && pSession->IsReady())
		{
			result = ProcessHandShake(pThis, pSocketObj, pSession);

			if(result == HR_ERROR)
				break;

			enStatus = SSL_HSS_SUCC;
			result	 = pThis->DoFireHandShake(pSocketObj);

			if(result == HR_ERROR)
				break;
		}

		const WSABUF& buffer = pSession->GetRecvBuffer();

		if(buffer.len == 0)
			break;

		result = pThis->DoFireReceive(pSocketObj, (const BYTE*)buffer.buf, buffer.len);

		if(result == HR_ERROR)
			break;
	}

	if(result != HR_ERROR && pSession->IsHandShaking())
		result = ::ProcessHandShake(pThis, pSocketObj, pSession);

	return result;
}

template<class T> EnHandleResult ProcessReceive(T* pThis, CSSLSession* pSession, const BYTE* pData, int iLength)
{
	if(!pSession->WriteRecvChannel(pData, iLength))
		return HR_ERROR;

	EnHandleResult result			= HR_OK;
	EnSSLHandShakeStatus enStatus	= pSession->GetStatus();

	while(TRUE)
	{
		if(!pSession->ReadRecvChannel())
			return HR_ERROR;

		if(enStatus == SSL_HSS_PROC && pSession->IsReady())
		{
			result = ProcessHandShake(pThis, pSession);

			if(result == HR_ERROR)
				break;

			enStatus = SSL_HSS_SUCC;
			result	 = pThis->DoFireHandShake(pThis);

			if(result == HR_ERROR)
				break;
		}

		const WSABUF& buffer = pSession->GetRecvBuffer();

		if(buffer.len == 0)
			break;

		result = pThis->DoFireReceive(pThis, (const BYTE*)buffer.buf, buffer.len);

		if(result == HR_ERROR)
			break;
	}

	if(result != HR_ERROR && pSession->IsHandShaking())
		result = ::ProcessHandShake(pThis, pSession);

	return result;
}

template<class T> BOOL ProcessSend(T* pThis, TSocketObj* pSocketObj, CSSLSession* pSession, const WSABUF * pBuffers, int iCount)
{
	if(pSession == nullptr || !pSession->IsReady())
	{
		::SetLastError(ERROR_INVALID_STATE);
		return FALSE;
	}

	CCriSecLock locallock(pSession->GetSendLock());

	if(!pSession->IsReady())
	{
		::SetLastError(ERROR_INVALID_STATE);
		return FALSE;
	}

	VERIFY(pSession->WriteSendChannel(pBuffers, iCount));

	while(TRUE)
	{
		VERIFY(pSession->ReadSendChannel());
		const WSABUF& buffer = pSession->GetSendBuffer();

		if(buffer.len == 0)
			break;

		if(!pThis->DoSendPackets(pSocketObj, &buffer, 1))
			return FALSE;
	}

	return TRUE;
}

template<class T> BOOL ProcessSend(T* pThis, CSSLSession* pSession, const WSABUF * pBuffers, int iCount)
{
	if(pSession == nullptr || !pSession->IsReady())
	{
		::SetLastError(ERROR_INVALID_STATE);
		return FALSE;
	}

	CCriSecLock locallock(pSession->GetSendLock());

	if(!pSession->IsReady())
	{
		::SetLastError(ERROR_INVALID_STATE);
		return FALSE;
	}

	VERIFY(pSession->WriteSendChannel(pBuffers, iCount));

	while(TRUE)
	{
		VERIFY(pSession->ReadSendChannel());
		const WSABUF& buffer = pSession->GetSendBuffer();

		if(buffer.len == 0)
			break;

		if(!pThis->DoSendPackets(&buffer, 1))
			return FALSE;
	}

	return TRUE;
}
