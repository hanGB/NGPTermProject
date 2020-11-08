#pragma once

int recvn(SOCKET s, char* buf, int len, int flags);

//클라이언트 데이터 통신 처리 스레드
DWORD WINAPI ProcessClient(LPVOID arg);
