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

#include "SocketHelper.h"
#include "../../Common/Src/Event.h"
#include "../../Common/Src/bufferptr.h"
#include "../../Common/Src/bufferpool.h"
#include "../../Common/Src/CriticalSection.h"

class CTcpClient : public ITcpClient
{
public:
	virtual BOOL Start	(LPCTSTR lpszRemoteAddress, USHORT usPort, BOOL bAsyncConnect = TRUE, LPCTSTR lpszBindAddress = nullptr);
	virtual BOOL Stop	();
	virtual BOOL Send	(const BYTE* pBuffer, int iLength, int iOffset = 0);
	virtual BOOL SendSmallFile	(LPCTSTR lpszFileName, const LPWSABUF pHead = nullptr, const LPWSABUF pTail = nullptr);
	virtual BOOL SendPackets	(const WSABUF pBuffers[], int iCount)	{return DoSendPackets(pBuffers, iCount);}
	virtual BOOL			HasStarted			()	{return m_enState == SS_STARTED || m_enState == SS_STARTING;}
	virtual EnServiceState	GetState			()	{return m_enState;}
	virtual CONNID			GetConnectionID		()	{return m_dwConnID;};
	virtual BOOL			GetLocalAddress		(TCHAR lpszAddress[], int& iAddressLen, USHORT& usPort);
	virtual BOOL GetPendingDataLength			(int& iPending) {iPending = m_iPending; return HasStarted();}
	virtual EnSocketError	GetLastError		()	{return m_enLastError;}
	virtual LPCTSTR			GetLastErrorDesc	()	{return ::GetSocketErrorDesc(m_enLastError);}

public:
	virtual void SetSocketBufferSize	(DWORD dwSocketBufferSize)		{m_dwSocketBufferSize	= dwSocketBufferSize;}
	virtual void SetKeepAliveTime		(DWORD dwKeepAliveTime)			{m_dwKeepAliveTime		= dwKeepAliveTime;}
	virtual void SetKeepAliveInterval	(DWORD dwKeepAliveInterval)		{m_dwKeepAliveInterval	= dwKeepAliveInterval;}
	virtual void SetFreeBufferPoolSize	(DWORD dwFreeBufferPoolSize)	{m_dwFreeBufferPoolSize = dwFreeBufferPoolSize;}
	virtual void SetFreeBufferPoolHold	(DWORD dwFreeBufferPoolHold)	{m_dwFreeBufferPoolHold = dwFreeBufferPoolHold;}
	virtual void SetExtra				(PVOID pExtra)					{m_pExtra				= pExtra;}						

	virtual DWORD GetSocketBufferSize	()	{return m_dwSocketBufferSize;}
	virtual DWORD GetKeepAliveTime		()	{return m_dwKeepAliveTime;}
	virtual DWORD GetKeepAliveInterval	()	{return m_dwKeepAliveInterval;}
	virtual DWORD GetFreeBufferPoolSize	()	{return m_dwFreeBufferPoolSize;}
	virtual DWORD GetFreeBufferPoolHold	()	{return m_dwFreeBufferPoolHold;}
	virtual PVOID GetExtra				()	{return m_pExtra;}

protected:
	virtual EnHandleResult FirePrepareConnect(IClient* pClient, SOCKET socket)
		{return DoFirePrepareConnect(pClient, socket);}
	virtual EnHandleResult FireConnect(IClient* pClient)
		{return DoFireConnect(pClient);}
	virtual EnHandleResult FireHandShake(IClient* pClient)
		{return DoFireHandShake(pClient);}
	virtual EnHandleResult FireSend(IClient* pClient, const BYTE* pData, int iLength)
		{return DoFireSend(pClient, pData, iLength);}
	virtual EnHandleResult FireReceive(IClient* pClient, const BYTE* pData, int iLength)
		{return DoFireReceive(pClient, pData, iLength);}
	virtual EnHandleResult FireReceive(IClient* pClient, int iLength)
		{return DoFireReceive(pClient, iLength);}
	virtual EnHandleResult FireClose(IClient* pClient, EnSocketOperation enOperation, int iErrorCode)
		{return DoFireClose(pClient, enOperation, iErrorCode);}

	virtual EnHandleResult DoFirePrepareConnect(IClient* pClient, SOCKET socket)
		{return m_psoListener->OnPrepareConnect(pClient, socket);}
	virtual EnHandleResult DoFireConnect(IClient* pClient)
		{return m_psoListener->OnConnect(pClient);}
	virtual EnHandleResult DoFireHandShake(IClient* pClient)
		{return m_psoListener->OnHandShake(pClient);}
	virtual EnHandleResult DoFireSend(IClient* pClient, const BYTE* pData, int iLength)
		{return m_psoListener->OnSend(pClient, pData, iLength);}
	virtual EnHandleResult DoFireReceive(IClient* pClient, const BYTE* pData, int iLength)
		{return m_psoListener->OnReceive(pClient, pData, iLength);}
	virtual EnHandleResult DoFireReceive(IClient* pClient, int iLength)
		{return m_psoListener->OnReceive(pClient, iLength);}
	virtual EnHandleResult DoFireClose(IClient* pClient, EnSocketOperation enOperation, int iErrorCode)
		{return m_psoListener->OnClose(pClient, enOperation, iErrorCode);}

	void SetLastError(EnSocketError code, LPCSTR func, int ec);
	virtual BOOL CheckParams();
	virtual void PrepareStart();
	virtual void Reset();

	virtual void OnWorkerThreadEnd(DWORD dwThreadID) {}

	BOOL DoSendPackets(const WSABUF pBuffers[], int iCount);

protected:
	void SetReserved	(PVOID pReserved)	{m_pReserved = pReserved;}						
	PVOID GetReserved	()					{return m_pReserved;}

private:
	BOOL CheckStarting();
	BOOL CheckStoping(DWORD dwCurrentThreadID);
	BOOL CreateClientSocket();
	BOOL BindClientSocket(LPCTSTR lpszBindAddress);
	BOOL ConnectToServer(LPCTSTR lpszRemoteAddress, USHORT usPort);
	BOOL CreateWorkerThread();
	BOOL ProcessNetworkEvent();
	BOOL ReadData();
	BOOL SendData();
	BOOL DoSendData(TItem* pItem);
	TItem* GetSendBuffer();
	BOOL SendInternal(const WSABUF pBuffers[], int iCount);
	void WaitForWorkerThreadEnd(DWORD dwCurrentThreadID);

	BOOL HandleError	(WSANETWORKEVENTS& events);
	BOOL HandleRead		(WSANETWORKEVENTS& events);
	BOOL HandleWrite	(WSANETWORKEVENTS& events);
	BOOL HandleConnect	(WSANETWORKEVENTS& events);
	BOOL HandleClose	(WSANETWORKEVENTS& events);

	static UINT WINAPI WorkerThreadProc(LPVOID pv);

public:
	CTcpClient(ITcpClientListener* psoListener)
	: m_psoListener			(psoListener)
	, m_lsSend				(m_itPool)
	, m_soClient			(INVALID_SOCKET)
	, m_evSocket			(nullptr)
	, m_dwConnID			(0)
	, m_hWorker				(nullptr)
	, m_dwWorkerID			(0)
	, m_bAsyncConnect		(FALSE)
	, m_iPending			(0)
	, m_enState				(SS_STOPPED)
	, m_enLastError			(SE_OK)
	, m_pExtra				(nullptr)
	, m_pReserved			(nullptr)
	, m_dwSocketBufferSize	(DEFAULT_TCP_SOCKET_BUFFER_SIZE)
	, m_dwFreeBufferPoolSize(DEFAULT_CLIENT_FREE_BUFFER_POOL_SIZE)
	, m_dwFreeBufferPoolHold(DEFAULT_CLIENT_FREE_BUFFER_POOL_HOLD)
	, m_dwKeepAliveTime		(DEFALUT_TCP_KEEPALIVE_TIME)
	, m_dwKeepAliveInterval	(DEFALUT_TCP_KEEPALIVE_INTERVAL)
	{
		ASSERT(m_psoListener);
	}

	virtual ~CTcpClient()
	{
		Stop();
	}

private:
	CInitSocket			m_wsSocket;

private:
	ITcpClientListener*	m_psoListener;
	TClientCloseContext m_ccContext;

	BOOL				m_bAsyncConnect;
	SOCKET				m_soClient;
	HANDLE				m_evSocket;
	CONNID				m_dwConnID;

	DWORD				m_dwSocketBufferSize;
	DWORD				m_dwFreeBufferPoolSize;
	DWORD				m_dwFreeBufferPoolHold;
	DWORD				m_dwKeepAliveTime;
	DWORD				m_dwKeepAliveInterval;

	HANDLE				m_hWorker;
	UINT				m_dwWorkerID;

	volatile EnServiceState	m_enState;
	EnSocketError		m_enLastError;

	PVOID				m_pExtra;
	PVOID				m_pReserved;

	CBufferPtr			m_rcBuffer;

protected:
	CItemPool			m_itPool;

private:
	CSpinGuard			m_csState;

	CCriSec				m_csSend;
	TItemList			m_lsSend;

	CEvt				m_evBuffer;
	CEvt				m_evWorker;

	volatile int		m_iPending;
};
