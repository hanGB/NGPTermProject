#pragma once

int recvn(SOCKET s, char* buf, int len, int flags);

//Ŭ���̾�Ʈ ������ ��� ó�� ������
DWORD WINAPI ProcessClient(LPVOID arg);
