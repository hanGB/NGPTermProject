#pragma once

int recvn(SOCKET s, char* buf, int len, int flags);

//전송용 스레드 함수
DWORD WINAPI SendMsg(LPVOID arg);

//수신용 스레드 함수
DWORD WINAPI RecvMsg(LPVOID arg);
