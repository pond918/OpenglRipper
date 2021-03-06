// ClientTest.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "NetBaseHeader.h"
#include <iostream>
//#include "vld.h"

class MySession :public SessionBase
{
public:
	using SessionBase::SessionBase;
	virtual int HandlePacket(NetPacket &pck)
	{
		UINT32 a;
		pck >> a;
		NetPacket resp(1);
		resp << a;
		PushSendPacket(resp);
		int nCount = 100;
		for (int j = 0; j < nCount; ++j)
		{
			NetPacket resp2(2);
			resp2 << a;
			for (int i = 0; i < 362; ++i)
			{
				resp2 << rand();
			}
			PushSendPacket(resp2);
		}
		return 0;
	}
	virtual void OnPeerShutDown()
	{
		SessionBase::OnPeerShutDown();
	}
};

SessionManager *pSessionManager = NULL;
IocpReactor *pReactor = NULL;
void ClientTest()
{
	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);

	struct hostent * ci = gethostbyname("127.0.0.1");
	sockaddr_in addr_far;
	memset(&addr_far, 0, sizeof(struct sockaddr_in));
	addr_far.sin_family = AF_INET;
	memcpy(&addr_far.sin_addr.s_addr, ci->h_addr_list[0], ci->h_length);
	addr_far.sin_port = htons((u_short)8986);
	int error = connect(clientSocket, (struct sockaddr *)&addr_far, sizeof(struct sockaddr_in));
	if (error != 0)
	{
		printf("connect error\n");
		return;
	}
	else
	{
		printf("connect success\n");
	}
	int nSendBuf;
	int len = sizeof(nSendBuf);
	int iResult = getsockopt(clientSocket, SOL_SOCKET, SO_SNDBUF, (char *)&nSendBuf, &len);
	u_long arg = 1;
	ioctlsocket(clientSocket, FIONBIO, &arg);
	arg = 1;
	setsockopt(clientSocket, IPPROTO_TCP, TCP_NODELAY, (const char*)&arg, sizeof(arg));
	IoPeerIocpTcp *pIoPeer = new IoPeerIocpTcp();
	pIoPeer->SetFd(clientSocket);
	MySession *pSession = new MySession(pIoPeer, pSessionManager);
	pIoPeer->SetSession(pSession);
	pReactor->AddIoPeer(pIoPeer);
	NetPacket pck(1);
	pck << 1;
 	pSession->PushSendPacket(pck);

}
void TestClient()
{
	srand((int)time(NULL));
	WSADATA dwData;
	WSAStartup(0x0202, &dwData);
	pSessionManager = new SessionManager();
	pReactor = new IocpReactor();
	pReactor->Start(8);
	for (int i = 0; i < 10; ++i)
	{
		ClientTest();
	}
	while (true)
	{
		pSessionManager->Update(40);
		TCHAR buf[512];
		wsprintf(buf, _T("count:%d"), pSessionManager->SessionCount());
		SetConsoleTitle(buf);
		Sleep(40);
	}
	pReactor->Stop();
	pSessionManager->Stop();
	delete pReactor;
	delete pSessionManager;
}

void fun(std::string &s)
{
	printf("%s\n", s.c_str());
	s = "aa";
}

void TestBind()
{
	std::string s;
	auto f = std::bind(fun, s);
	f();
	printf("%s\n", s.c_str());
}

#include <math.h>
float TestShaderPositonPck(float a )
{
	float intp;
	float a1 = modf(a / 65025.f, &intp);
	a1 = (int(a1 * 255))/255.f;
	float a2 = modf(a / 255.f, &intp);
	a2 = (int(a2 * 255)) / 255.f;
	float a3 = modf(a / 1, &intp);
	a3 = (int(a3 * 255)) / 255.f;
	float a4 = modf(a * 255.f, &intp);
	a4 = (int(a4 * 255)) / 255.f;

	float r = a1 * 65025.f + a2 * 255.f + a3 * 1 + a4 / 255.f;
	return r;
}
#include "time.h"
void TestTime()
{
	extern long _timezone;
	time_t from = 1544368088;
	time_t to = 1544375288;
	from += _timezone;
	to += _timezone;
	unsigned int  d1 = from / (24 * 3600);
	unsigned int  d2 = to / (24 * 3600);
	int c = _timezone;
	int b = 0;
}

class T2
{
public:
	static constexpr bool HasIndex()
	{
		return true;
	}
};

template<typename T>
class TestC
{
public:
	//typedef decltype(GetType<T::HasIndex()>()) itTest;
	template<bool U>
	static auto GetType()
	{
		return int(0);
	}

	template<>
	static auto GetType<true>()
	{
		return bool(false);
	}

	template<>
	static auto GetType<false>()
	{
		return char(0);
	}


	decltype(GetType<T::HasIndex()>()) FunTest()
	{
		return decltype(GetType<T::HasIndex()>())();
	}
};
class A
{
public: 
	int a;
	A()
	{
		a = 1;
	}
};
class B
{
public:
	int b;
	B()
	{
		b = 2;
	}
};
class C:public A, public B
{
public:
	int c;
	C()
	{
		c = 3;
	}
};


#include "log4cplus/logger.h" 

#include "log4cplus/configurator.h" 
#include "log4cplus/loggingmacros.h"

using namespace log4cplus;
#include <iomanip> 
int main()
{
	log4cplus::initialize();
	BasicConfigurator config;
	config.configure();
	log4cplus::tstring m = L"main";
	Logger logger = Logger::getInstance(m);
	LOG4CPLUS_WARN(logger, L"Hello, World!");
	return 0;
}


