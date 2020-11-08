#include "stdafx.h"
#include "resource.h"
#include "IVSSCGlobals.h"
#include "IVSSCNetwork.h"

extern SOCKET sock;

extern player parray[MAX_CLNT];

extern CData clnt_data;
extern Clinfo clnt_info;

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

		Sleep(10);
		send(sock, (char*)&clnt_data, sizeof(CData), 0);
	}
	return 0;
}

DWORD WINAPI RecvMsg(LPVOID arg) {
	int len;
	while (1) {//반복

		Sleep(10);
		int GetSize;
		char suBuffer[BUFSIZE];
		GetSize = recv(sock, suBuffer, sizeof(suBuffer) - 1, 0);
		suBuffer[GetSize] = '\0';
		player temp = *(player*)suBuffer;
		parray[temp.nu] = temp;
	}
	return 0;
}
