#include "stdafx.h"
#include "resource.h"
#include "IVSSCGlobals.h"
#include "IVSSCNetwork.h"

extern SOCKET sock;

extern player parray[MAX_PLAYER];
extern int block[BOARD_SIZE][BOARD_SIZE];

extern GameObjects g_GameObjects;

extern CData clnt_data;
extern Clinfo clnt_info;

extern bool g_bReadyToSend;

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

DWORD WINAPI SendMsg(LPVOID arg) {//전송용 쓰레드함수
	while (1) {//반복
		if (g_bReadyToSend) {
			send(sock, (char*)&clnt_data, sizeof(CData), 0);
			g_bReadyToSend = false;
		}
	}
	return 0;
}

DWORD WINAPI RecvMsg(LPVOID arg) {
	int len;
	while (1) {//반복
		int retval;

		retval = recv(sock, (char*)&g_GameObjects, sizeof(GameObjects), 0);

		if (retval != SOCKET_ERROR) {
			memcpy(block, g_GameObjects.blocks, sizeof(int) * BOARD_SIZE * BOARD_SIZE);
			memcpy(parray, g_GameObjects.players, sizeof(player) * MAX_PLAYER);
			clnt_data.coolTime = g_GameObjects.players[clnt_data.ci].coolTime;
		}
		else
		{
			exit(0);
		}
	}
	return 0;
}
