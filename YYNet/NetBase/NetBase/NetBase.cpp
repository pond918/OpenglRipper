// NetBase.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"


class MySession :public SessionBase
{
public:
	using SessionBase::SessionBase;
	virtual int HandlePacket(NetPacket &pck)
	{
		UINT32 a;
		pck >> a;
		printf("%d\n", a);
		if (m_pIoPeer)
		{
			m_pIoPeer->CloseFd();
		}
		return 0;
	}
	virtual void OnPeerShutDown()
	{
		SessionBase::OnPeerShutDown();
	}
};

class MyAcceptSession :public SessionBase
{
public:
	MyAcceptSession(IoPeerBase* pIoPeer, ReactorBase* pReactor, SessionManager* pManager):
		SessionBase(pIoPeer, pManager),
		m_pReactor(pReactor)
	{

	}

	void PushNewConnectSocket(SOCKET sock)
	{
		m_newSocketList.push(sock);
	}

	virtual void Update(UINT32 dt)
	{
		SessionBase::Update(dt);
		UpdateNewConnectSocket();
	}

	void UpdateNewConnectSocket()
	{
		SOCKET fd;
		while (m_newSocketList.try_pop(fd))
		{
			IoPeerIocpTcp *pIoPeer = new IoPeerIocpTcp();
			pIoPeer->SetFd(fd);
			MySession *pSession = new MySession(pIoPeer, m_sessionManager);
			pIoPeer->SetSession(pSession);
			m_pReactor->AddIoPeer(pIoPeer);;
		}
	}
	~MyAcceptSession()
	{
		SOCKET s;
		while (m_newSocketList.try_pop(s))
		{
			closesocket(s);
		}
	}
private:
	SafeQueue<SOCKET> m_newSocketList;
	ReactorBase*	  m_pReactor;
};
class MyIoAccept :public IoPeerIocpAccept
{
public:
	MyIoAccept(ReactorBase* pReactor, SessionManager *pSessionManager) :
		m_pReactor(pReactor),
		m_pSessionManager(pSessionManager)
	{

	}
	//todo thread safe for m_pSessionManager Add
	virtual void OnNewClientConnect(SOCKET fd)
	{
		((MyAcceptSession *)m_pSession)->PushNewConnectSocket(fd);
	}

private:
	ReactorBase * m_pReactor;
	SessionManager *m_pSessionManager;
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
	int a = 12345;
	pck << 12345;
	pSession->PushSendPacket(pck);
	
}
// int main()
// {
// 	WSADATA dwData;
// 	WSAStartup(0x0202, &dwData);
// 	pSessionManager = new SessionManager();
// 	pReactor = new IocpReactor();
// 	pReactor->Start(8);
// 	MyIoAccept *pIoPeerAccept = new MyIoAccept(pReactor, pSessionManager);
// 	MyAcceptSession *pAcceptSession = new MyAcceptSession(pIoPeerAccept, pReactor, pSessionManager);
// 	pIoPeerAccept->SetSession(pAcceptSession);
// 	pIoPeerAccept->Init("0.0.0.0", 8986);
// 	pReactor->AddIoPeer(pIoPeerAccept);
// 	//std::thread clientThread(ClientTest);
// 	ClientTest();
// 	while (true)
// 	{
// 		pSessionManager->Update(40);
// 		Sleep(40);
// 	}
//     return 0;
// }

