#include "stdafx.h"
#include "IVSSSGlobals.h"
#include "IVSSSUpdate.h"
#include "IVSSSNetwork.h"
#include <ctime>

extern int clientCount;
extern SOCKET clientSocks[MAX_CLNT];//클라이언트 소켓 보관용 배열
extern HANDLE hMutex;//뮤텍스

extern GameObjects g_GameObjects;
extern player parray[MAX_PLAYER];

extern CData clnt_data[MAX_CLNT];

extern int g_prevTimeInMillisecond;
extern bool connect_index[4];

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

void Waiting()
{
	for (int id = 0; id < MAX_PLAYER; ++id)
	{
		if (clnt_data[id].ci != NON_PLAYER)
		{
			if (parray[id].enable)
			{
				if (clnt_data[id].p_key.KEY_SPACEBAR)
				{
					parray[id].gameready = true;
				}
			}
		}
	}
}

DWORD WINAPI ProcessClient(LPVOID arg)
{
	SOCKET clientSock = (SOCKET)arg; //매개변수로받은 클라이언트 소켓을 전달
	int len;
	int ci = 0;//클라 아이디
	for (int i = 0; i < MAX_PLAYER; i++) {//배열의 갯수만큼
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
		GetSize = recv(clientSock, suBuffer, sizeof(suBuffer) - 1, 0);
		WaitForSingleObject(hMutex, INFINITE);//뮤텍스 실행
		if (GetSize >= 0 && GetSize < BUFSIZE) {
			suBuffer[GetSize] = '\0';
			CData* tmp = (CData*)suBuffer;
			playerid = tmp->ci;

			clnt_data[playerid] = *tmp;

			int currentTime = timeGetTime();

			int elapsedTime = currentTime - g_prevTimeInMillisecond;

			g_prevTimeInMillisecond = currentTime;
			float elapsedTimeInSec = (float)elapsedTime / 1000.f;
			
			if (g_GameObjects.GameState == 1)
				Update(elapsedTimeInSec);
			else
				Waiting();

			for (int i = 0; i < MAX_PLAYER; ++i) {
				player temp = parray[playerid];
				temp.nu = playerid;

				g_GameObjects.players[playerid] = temp;
			}
		}
		else
		{
			ReleaseMutex(hMutex);
			break;
		}
		for (int i = 0; i < MAX_PLAYER; i++)
		{
			if(connect_index[i] == true)
				send(clientSocks[i], (char*)&g_GameObjects, sizeof(GameObjects), 0);

		}
		ReleaseMutex(hMutex);//뮤텍스 중지
	}

	/*
	//이 줄을 실행한다는 것은 해당 클라이언트가 나갔다는 사실임 따라서 해당 클라이언트를 배열에서 제거해줘야함
	WaitForSingleObject(hMutex, INFINITE);//뮤텍스 실행
	for (int i = 0; i < clientCount; i++) {//배열의 갯수만큼
		if (clientSock == clientSocks[i]) {//만약 현재 clientSock값이 배열의 값과 같다면
			while (i++ < clientCount - 1)//클라이언트 개수 만큼
				clientSocks[i] = clientSocks[i + 1];//앞으로 땡긴다.
			break;
		}
	}
	*/

	WaitForSingleObject(hMutex, INFINITE);
	parray[ci].enable = false;
	for (int i = 0; i < MAX_PLAYER; i++)
	{
		if (connect_index[i] == true)
			send(clientSocks[i], (char*)&g_GameObjects, sizeof(GameObjects), 0);

	}
	connect_index[ci] = false;

	clientCount--;//클라이언트 개수 하나 감소
	ReleaseMutex(hMutex);//뮤텍스 중지
	closesocket(clientSock);//소켓을 종료한다.
	return 0;
}
