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

#ifndef _SSL_SUPPORT
	#define _SSL_SUPPORT
#endif

#include "SocketInterface.h"

/************************************************************************
���ƣ�SSL ����ģʽ
��������ʶ SSL �Ĺ���ģʽ���ͻ���ģʽ������ģʽ
************************************************************************/
enum EnSSLSessionMode
{
	SSL_SM_CLIENT	= 0,	// �ͻ���ģʽ
	SSL_SM_SERVER	= 1,	// �����ģʽ
};

/************************************************************************
���ƣ�SSL ��֤ģʽ
������SSL ��֤ģʽѡ�SSL_VM_PEER ���Ժͺ�������ѡ�����һ��
************************************************************************/
enum EnSSLVerifyMode
{
	SSL_VM_NONE					= 0x00,	// SSL_VERIFY_NONE
	SSL_VM_PEER					= 0x01,	// SSL_VERIFY_PEER
	SSL_VM_FAIL_IF_NO_PEER_CERT	= 0x02,	// SSL_VERIFY_FAIL_IF_NO_PEER_CERT
	SSL_VM_CLIENT_ONCE			= 0x04,	// SSL_VERIFY_CLIENT_ONCE
};
