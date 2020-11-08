#include "stdafx.h"
#include "IVSSSGlobals.h"
#include "IVSSSUpdate.h"
#include "IVSSSNetwork.h"

extern int clientCount;
extern SOCKET clientSocks[MAX_CLNT];//클라이언트 소켓 보관용 배열
extern HANDLE hMutex;//뮤텍스

extern player parray[2];

extern CData clnt_data[MAX_CLNT];

int recvn(SOCKET s, char* buf, int len, int flags)
{
	int received;
	char* ptr = buf;
	int left = len;
	while (left > 0) {
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (received == 0)
			break;
		left -= received;
		ptr += received;
	}
	return (len - left);
}

DWORD WINAPI ProcessClient(LPVOID arg)
{
	SOCKET clientSock = (SOCKET)arg; //매개변수로받은 클라이언트 소켓을 전달
	int len;
	int ci = 0;//클라 아이디
	for (int i = 0; i < clientCount; i++) {//배열의 갯수만큼
		if (clientSock == clientSocks[i]) {
			ci = i;
			break;
		}
	}
	while (1)
	{
		int GetSize;
		char suBuffer[BUFSIZE];
		int playerid;
		Sleep(10);
		GetSize = recv(clientSock, suBuffer, sizeof(suBuffer) - 1, 0);
		WaitForSingleObject(hMutex, INFINITE);//뮤텍스 실행
		suBuffer[GetSize] = '\0';
		CData* tmp = (CData*)suBuffer;
		playerid = tmp->ci;

		clnt_data[playerid] = *tmp;

		move_player_object(playerid);

		player temp = parray[playerid];
		temp.nu = playerid;
		for (int i = 0; i < clientCount; i++)
		{
			send(clientSocks[i], (char*)&temp, sizeof(player), 0);

		}
		ReleaseMutex(hMutex);//뮤텍스 중지
		Sleep(10);
	}

	//이 줄을 실행한다는 것은 해당 클라이언트가 나갔다는 사실임 따라서 해당 클라이언트를 배열에서 제거해줘야함
	WaitForSingleObject(hMutex, INFINITE);//뮤텍스 실행
	for (int i = 0; i < clientCount; i++) {//배열의 갯수만큼
		if (clientSock == clientSocks[i]) {//만약 현재 clientSock값이 배열의 값과 같다면
			while (i++ < clientCount - 1)//클라이언트 개수 만큼
				clientSocks[i] = clientSocks[i + 1];//앞으로 땡긴다.
			break;
		}
	}
	clientCount--;//클라이언트 개수 하나 감소
	ReleaseMutex(hMutex);//뮤텍스 중지
	closesocket(clientSock);//소켓을 종료한다.
	return 0;
}
