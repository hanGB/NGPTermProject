#pragma once

int recvn(SOCKET s, char* buf, int len, int flags);

//���ۿ� ������ �Լ�
DWORD WINAPI SendMsg(LPVOID arg);

//���ſ� ������ �Լ�
DWORD WINAPI RecvMsg(LPVOID arg);
