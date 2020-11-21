#include "stdafx.h"
#include "IVSSSGlobals.h"
#include "IVSSSUpdate.h"
#include "IVSSSNetwork.h"
#include <ctime>

extern int clientCount;
extern SOCKET clientSocks[MAX_CLNT];//Ŭ���̾�Ʈ ���� ������ �迭
extern HANDLE hMutex;//���ؽ�

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
	SOCKET clientSock = (SOCKET)arg; //�Ű������ι��� Ŭ���̾�Ʈ ������ ����
	int len;
	int ci = 0;//Ŭ�� ���̵�
	for (int i = 0; i < MAX_PLAYER; i++) {//�迭�� ������ŭ
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
		WaitForSingleObject(hMutex, INFINITE);//���ؽ� ����
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
		ReleaseMutex(hMutex);//���ؽ� ����
	}

	/*
	//�� ���� �����Ѵٴ� ���� �ش� Ŭ���̾�Ʈ�� �����ٴ� ����� ���� �ش� Ŭ���̾�Ʈ�� �迭���� �����������
	WaitForSingleObject(hMutex, INFINITE);//���ؽ� ����
	for (int i = 0; i < clientCount; i++) {//�迭�� ������ŭ
		if (clientSock == clientSocks[i]) {//���� ���� clientSock���� �迭�� ���� ���ٸ�
			while (i++ < clientCount - 1)//Ŭ���̾�Ʈ ���� ��ŭ
				clientSocks[i] = clientSocks[i + 1];//������ �����.
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

	clientCount--;//Ŭ���̾�Ʈ ���� �ϳ� ����
	ReleaseMutex(hMutex);//���ؽ� ����
	closesocket(clientSock);//������ �����Ѵ�.
	return 0;
}
