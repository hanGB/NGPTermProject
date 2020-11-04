
#include "resource.h"
#include "InversusServer.h"
#include <atlImage.h>

#ifdef _DEBUG 
	#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console" ) 
#endif


#define SERVERPORT 9000
#define BUFSIZE 3000
#define MAX_CLNT 256

#define PLAYER_SPEED 1

HINSTANCE g_hinst;
LPCTSTR lpszClass = "Window Class Name";
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM IParam);

DWORD WINAPI ServerMain(LPVOID arg);
DWORD WINAPI ProcessClient(LPVOID arg);

SOCKET sock;
int clientCount = 0;
SOCKET clientSocks[MAX_CLNT];//Ŭ���̾�Ʈ ���� ������ �迭
HANDLE hMutex;//���ؽ�

player parray[2];

typedef struct Clinfo {
	int ci;
}Clinfo;

typedef struct CData {//Ŭ���̾�Ʈ�κ��� ���� ������
	int ci;
	int dx, dy; //����
}CData;

Clinfo clnt_info[MAX_CLNT];
CData clnt_data[MAX_CLNT];

RECT rectView;
double sx, sy;
double seta = 0;

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hPrevInstance, LPSTR IpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASSEX WndClass;
	g_hinst = hinstance;
	//������ Ŭ���� ����ü �� ����
	WndClass.cbSize = sizeof(WndClass);
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hinstance;
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	WndClass.lpszClassName = lpszClass;
	WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	//������ Ŭ���� ���
	RegisterClassEx(&WndClass);
	//������ ����
	hWnd = CreateWindow(lpszClass, "window program", WS_OVERLAPPEDWINDOW, 0, 0, 1024, 768, NULL, (HMENU)NULL, hinstance, NULL);
	//������ ���
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	CreateThread(NULL, 0, ServerMain, NULL, 0, NULL);
	//�̺�Ʈ ���� ó��
	while (GetMessage(&Message, 0, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return Message.wParam;
}

DWORD WINAPI ServerMain(LPVOID arg)
{
	WSADATA wsa;
	HANDLE sendThread, recvThread;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	hMutex = CreateMutex(NULL, FALSE, NULL);//�ϳ��� ���ؽ��� �����Ѵ�.

	sock = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	bind(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	listen(sock, SOMAXCONN);

	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	HANDLE hTread;

	while (1)
	{
		addrlen = sizeof(clientaddr);
		client_sock = accept(sock, (SOCKADDR*)&clientaddr, &addrlen);
		WaitForSingleObject(hMutex, INFINITE);//���ؽ� ����

		if (clientCount == 0)
		{
			parray[clientCount].cx = 400;
			parray[clientCount].cy = 300;
			parray[clientCount].enable = true;
		}
		else
		{
			parray[clientCount].cx = rand() % 1000;
			parray[clientCount].cy = rand() % 700;
			parray[clientCount].enable = true;
		}
		clnt_info[clientCount].ci = clientCount;
		clientSocks[clientCount++] = client_sock;//Ŭ���̾�Ʈ ���Ϲ迭�� ��� ������ ���� �ּҸ� ����

		send(client_sock, (char*)&clnt_info[clientCount - 1], sizeof(Clinfo), 0);

		ReleaseMutex(hMutex);//���ؽ� ����

		hTread = CreateThread(NULL, 0, ProcessClient, (LPVOID)client_sock, 0, NULL);
	}
	closesocket(sock);
	WSACleanup();
}

void move_player_object(int playerid)
{
	parray[playerid].cx += (double)(clnt_data[playerid].dx) * double(PLAYER_SPEED);
	parray[playerid].cy += (double)(clnt_data[playerid].dy) * double(PLAYER_SPEED);
	for (int i = 0; i < 6; i++)
	{
		parray[playerid].rx[i] = sx / 4 * cos(seta + i) + parray[playerid].cx;
		parray[playerid].ry[i] = sy / 4 * sin(seta + i) + parray[playerid].cy;
	}
}

DWORD WINAPI ProcessClient(LPVOID arg)
{
	SOCKET clientSock = (SOCKET)arg; //�Ű������ι��� Ŭ���̾�Ʈ ������ ����
	int len;
	int ci = 0;//Ŭ�� ���̵�
	for (int i = 0; i < clientCount; i++) {//�迭�� ������ŭ
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
		WaitForSingleObject(hMutex, INFINITE);//���ؽ� ����
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
		ReleaseMutex(hMutex);//���ؽ� ����
		Sleep(10);
	}

	//�� ���� �����Ѵٴ� ���� �ش� Ŭ���̾�Ʈ�� �����ٴ� ����� ���� �ش� Ŭ���̾�Ʈ�� �迭���� �����������
	WaitForSingleObject(hMutex, INFINITE);//���ؽ� ����
	for (int i = 0; i < clientCount; i++) {//�迭�� ������ŭ
		if (clientSock == clientSocks[i]) {//���� ���� clientSock���� �迭�� ���� ���ٸ�
			while (i++ < clientCount - 1)//Ŭ���̾�Ʈ ���� ��ŭ
				clientSocks[i] = clientSocks[i + 1];//������ �����.
			break;
		}
	}
	clientCount--;//Ŭ���̾�Ʈ ���� �ϳ� ����
	ReleaseMutex(hMutex);//���ؽ� ����
	closesocket(clientSock);//������ �����Ѵ�.
	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM IParam)
{
	static int ten = 0;
	static int gametime = 0;
	sx = rectView.right / 15;
	sy = rectView.bottom / 10;
	static int reload = 0;

	switch (iMessage) {
	case WM_CREATE:
		GetClientRect(hWnd, &rectView);
		srand((unsigned)time(NULL));
		SetTimer(hWnd, 1, 100, NULL);
		return 0;
	case WM_PAINT:
	{

		break;
	}
	case WM_TIMER: // �ð��� ����ϸ� �޽��� �ڵ� ����
		switch (wParam) {
		case 1:
			if (seta < 2 * 3.14)
			{
				seta += 0.3;
			}
			else
			{
				seta = 0;
			}

			if (reload < 20)
			{
				(reload)++;
			}
			else
			{
				for (int j = 0; j < clientCount; j++)
				{
					for (int i = 0; i < 6; i++)
					{
						if (parray[j].bullet[i][0] == 0)
						{
							parray[j].bullet[i][0] = 1;
							reload = 0;
						}
					}
				}
			}

			if (ten < 10)//1��
			{
				ten++;
			}
			else
			{
				ten = 0;
				gametime++;
			}
			break;
		case 2:

			break;
		}
		InvalidateRect(hWnd, NULL, FALSE);
		break;

	case WM_CHAR:

		//InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_KEYDOWN:
		//MessageBox(NULL, "test", "test", MB_OK);


		InvalidateRect(hWnd, NULL, FALSE);
		break;
	case WM_KEYUP:

		InvalidateRect(hWnd, NULL, FALSE);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case ID_40001://�ʱ�
			break;
		case ID_40002://�߱�
			break;
		case ID_40003://���
			break;
		case ID_40004:
			break;

		case ID_40005:
			break;
		}
		break;
	case WM_DESTROY:
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, IParam));//���� �� �޼��� ���� ������ �޼����� OS��
}
