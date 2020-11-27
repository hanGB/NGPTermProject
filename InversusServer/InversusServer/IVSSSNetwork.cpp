#include "stdafx.h"
#include "IVSSSGlobals.h"
#include "IVSSSUpdate.h"
#include "IVSSSNetwork.h"
#include <ctime>
#include <iostream>

extern int clientCount;
extern SOCKET clientSocks[MAX_PLAYER];//클라이언트 소켓 보관용 배열
extern HANDLE hMutex;//뮤텍스

extern GameObjects g_GameObjects;
extern player parray[MAX_PLAYER];

extern CData clnt_data[MAX_PLAYER];

extern int g_prevTimeInMillisecond;
extern bool connect_index[MAX_PLAYER];

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
	int pready_count = 0;
	for (int id = 0; id < MAX_PLAYER; id++)
	{
		if (clnt_data[id].ci != NON_PLAYER)
		{
			if (parray[id].enable)
			{
				if (clnt_data[id].p_key.KEY_SPACEBAR)
				{
					parray[id].gameready = true;
				}

				if (parray[id].gameready)
					pready_count++;
			}
		}
	}
	if (pready_count == clientCount)
	{
		if (clientCount > 1)
			g_GameObjects.GameState = 1;
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

			player temp = parray[playerid];
			g_GameObjects.players = temp;
			g_GameObjects.players.nu = playerid;
			/*
			for (int i = 0; i < MAX_PLAYER; ++i) {
				player temp = parray[playerid];
				temp.nu = playerid;

				g_GameObjects.players[playerid] = temp;
			}
			*/
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

	WaitForSingleObject(hMutex, INFINITE);
	parray[ci].enable = false;
	parray[ci].gameready = false;

	g_GameObjects.players = parray[ci];
	g_GameObjects.players.nu = ci;

	char logstr[100];
	sprintf(logstr, "[퇴장]Player%d님이 접속을 종료했습니다.\n", ci);
	log_msg(logstr);

	/*
	for (int i = 0; i < MAX_PLAYER; ++i) {
		player temp = parray[i];
		g_GameObjects.players[i] = temp;
	}
	*/

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
