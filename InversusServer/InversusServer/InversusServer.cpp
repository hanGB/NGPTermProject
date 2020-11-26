
#include "stdafx.h"
#include "resource.h"
#include <atlImage.h>
#include "IVSSSGlobals.h"
#include "IVSSSNetwork.h"
#include "IVSSSUpdate.h"
#include "IVSSSGame.h"
#include <iostream>


#ifdef _DEBUG 
	#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console" ) 
#endif


HINSTANCE g_hinst;
LPCTSTR lpszClass = "Window Class Name";
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM IParam);

DWORD WINAPI ServerMain(LPVOID arg);
DWORD WINAPI ProcessClient(LPVOID arg);

SOCKET sock;
int clientCount = 0;
SOCKET clientSocks[MAX_PLAYER];//클라이언트 소켓 보관용 배열
HANDLE hMutex;//뮤텍스

player parray[MAX_PLAYER];
GameObjects g_GameObjects;


Clinfo clnt_info[MAX_PLAYER];
CData clnt_data[MAX_PLAYER];

RECT rectView;
double sx, sy;
double seta = 0;

int g_prevTimeInMillisecond = 0;
bool connect_index[MAX_PLAYER] = { false, };

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hPrevInstance, LPSTR IpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASSEX WndClass;
	g_hinst = hinstance;
	//윈도우 클래스 구조체 값 설정
	WndClass.cbSize = sizeof(WndClass);
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hinstance;
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = lpszClass;
	WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	//윈도우 클래스 등록
	RegisterClassEx(&WndClass);
	//윈도우 생성
	hWnd = CreateWindow(lpszClass, "window program", WS_OVERLAPPEDWINDOW, 0, 0, 1024, 768, NULL, (HMENU)NULL, hinstance, NULL);
	//윈도우 출력
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	CreateThread(NULL, 0, ServerMain, NULL, 0, NULL);
	//이벤트 루프 처리
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

	hMutex = CreateMutex(NULL, FALSE, NULL);//하나의 뮤텍스를 생성한다.

	sock = socket(AF_INET, SOCK_STREAM, 0);

	// Turn Off Nagle Algorithm 
	bool optval = TRUE;
	setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char*)&optval, sizeof(optval));

	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	bind(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	listen(sock, SOMAXCONN);

	char logstr[100];
	sprintf(logstr, "[Open]서버를 열었습니다.\n", inet_ntoa(serveraddr.sin_addr), ntohs(serveraddr.sin_port));
	log_msg(logstr);

	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	HANDLE hTread;

	//게임 오브젝트 초기화
	ZeroMemory(&g_GameObjects, sizeof(GameObjects));
	g_GameObjects.GameState = 0;
	for (int i = 0; i < MAX_PLAYER; ++i) {
		clnt_data[i].ci = NON_PLAYER;
	}

	initboard();

	while (1)
	{
		addrlen = sizeof(clientaddr);
		client_sock = accept(sock, (SOCKADDR*)&clientaddr, &addrlen);
		WaitForSingleObject(hMutex, INFINITE);//뮤텍스 실행

		int clinet_index = -1;
		for (int i = 0; i < MAX_PLAYER; i++)
		{
			if (connect_index[i] == false)
			{
				clinet_index = i;
				initplayerpos(i);
				parray[i].enable = true;

				clnt_info[i].ci = i;
				clientSocks[i] = client_sock;
				connect_index[i] = true;
				clientCount++;
				break;
			}
		}
		send(client_sock, (char*)&clnt_info[clinet_index], sizeof(Clinfo), 0);

		ReleaseMutex(hMutex);//뮤텍스 중지

		sprintf(logstr, "[접속]Player%d님이 접속하셨습니다.(ip: %s port: %d)\n", clinet_index, inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
		log_msg(logstr);
		hTread = CreateThread(NULL, 0, ProcessClient, (LPVOID)client_sock, 0, NULL);
	}
	closesocket(sock);
	WSACleanup();
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM IParam)
{
	static int ten = 0;
	static int gametime = 0;
	sx = rectView.right / 15;
	sy = rectView.bottom / 10;
	static int reload = 0;


	PAINTSTRUCT ps;
	static HDC hDC, hMemDC;
	static LOGFONT lf;

	switch (iMessage) {
	case WM_CREATE:
		GetClientRect(hWnd, &rectView);
		srand((unsigned)time(NULL));
		SetTimer(hWnd, 1, 100, NULL);

		ZeroMemory(&lf, sizeof(lf));

		return 0;
	case WM_PAINT:
	{
		hDC = BeginPaint(hWnd, &ps);

		CImage img;
		img.Create(rectView.right, rectView.bottom, 24);

		HBRUSH hBrush, oldBrush;
		HFONT Font, OldFont;
		HPEN hPen, OldPen;
		
		char str[100];
		hMemDC = img.GetDC();
		{
			hBrush = CreateSolidBrush(RGB(255, 255, 255));
			oldBrush = (HBRUSH)SelectObject(hMemDC, hBrush);
			Rectangle(hMemDC, 0, 0, rectView.right, rectView.bottom);
			SelectObject(hMemDC, oldBrush);
			DeleteObject(hBrush);

			lf.lfHeight = 50;
			Font = CreateFontIndirect(&lf);
			OldFont = (HFONT)SelectObject(hMemDC, Font);
			wsprintf(str, "Inversus Server");
			RECT aect = { rectView.right/2 - 500, 50, rectView.right / 2 + 500, 250 };
			SetTextColor(hMemDC, RGB(0, 0, 0));
			DrawText(hMemDC, str, -1, &aect, DT_CENTER);
			SelectObject(hMemDC, OldFont);
			DeleteObject(Font);

			if (g_GameObjects.GameState == 0)
			{
				for (int i = 0; i < MAX_PLAYER; i++)
				{
					if (parray[i].enable == true)
					{
						hBrush = CreateSolidBrush(RGB(200, 10, 10));
						oldBrush = (HBRUSH)SelectObject(hMemDC, hBrush);
						if (parray[i].gameready == true)
						{
							lf.lfHeight = 30;
							Font = CreateFontIndirect(&lf);
							OldFont = (HFONT)SelectObject(hMemDC, Font);
							wsprintf(str, "READY");
							aect = { 200 * i + 50, 350, 200 * (i + 1) - 50, 400 };
							SetTextColor(hMemDC, RGB(255, 0, 0));
							DrawText(hMemDC, str, -1, &aect, DT_CENTER);
							SelectObject(hMemDC, OldFont);
							DeleteObject(Font);
						}
					}
					else
					{
						hBrush = CreateSolidBrush(RGB(125, 125, 125));
						oldBrush = (HBRUSH)SelectObject(hMemDC, hBrush);
					}
					Rectangle(hMemDC, 200 * i + 50, 200, 200 * (i + 1) - 50, 300);
					SelectObject(hMemDC, oldBrush);
					DeleteObject(hBrush);
					lf.lfHeight = 30;
					Font = CreateFontIndirect(&lf);
					OldFont = (HFONT)SelectObject(hMemDC, Font);
					wsprintf(str, "Player%d", i);
					aect = { 200 * i + 50, 300, 200 * (i + 1) - 50, 350 };
					SetTextColor(hMemDC, RGB(0, 0, 0));
					DrawText(hMemDC, str, -1, &aect, DT_CENTER);
					SelectObject(hMemDC, OldFont);
					DeleteObject(Font);
				}
			}
			else
			{
				for (int i = 0; i < MAX_PLAYER; i++)
				{
					if (parray[i].enable == true)
					{
						hBrush = CreateSolidBrush(RGB(200, 10, 10));
						oldBrush = (HBRUSH)SelectObject(hMemDC, hBrush);
						if (parray[i].gameready == true)
						{
							lf.lfHeight = 30;
							Font = CreateFontIndirect(&lf);
							OldFont = (HFONT)SelectObject(hMemDC, Font);
							wsprintf(str, "LIFE: %d", parray[i].life);
							aect = { 200 * i + 50, 350, 200 * (i + 1) - 50, 400 };
							SetTextColor(hMemDC, RGB(255, 0, 0));
							DrawText(hMemDC, str, -1, &aect, DT_CENTER);
							SelectObject(hMemDC, OldFont);
							DeleteObject(Font);
						}
					}
					else
					{
						hBrush = CreateSolidBrush(RGB(125, 125, 125));
						oldBrush = (HBRUSH)SelectObject(hMemDC, hBrush);
					}
					Rectangle(hMemDC, 200 * i + 50, 200, 200 * (i + 1) - 50, 300);
					SelectObject(hMemDC, oldBrush);
					DeleteObject(hBrush);
					lf.lfHeight = 30;
					Font = CreateFontIndirect(&lf);
					OldFont = (HFONT)SelectObject(hMemDC, Font);
					wsprintf(str, "Player%d", i);
					aect = { 200 * i + 50, 300, 200 * (i + 1) - 50, 350 };
					SetTextColor(hMemDC, RGB(0, 0, 0));
					DrawText(hMemDC, str, -1, &aect, DT_CENTER);
					SelectObject(hMemDC, OldFont);
					DeleteObject(Font);
				}
			}

			img.Draw(hDC, 0, 0);
			img.ReleaseDC();
		}
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_TIMER: // 시간이 경과하면 메시지 자동 생성
		switch (wParam) {
		case 1:
			if (g_GameObjects.GameState == 1)
			{
				if (seta < 2 * 3.14)
				{
					seta += 0.3;
				}
				else
				{
					seta = 0;
				}

				if (reload < 30)
				{
					reload++;
				}
				else
				{
					for (int j = 0; j < MAX_PLAYER; j++)
					{
						if (parray[j].enable)
						{
							for (int i = 0; i < 6; i++)
							{
								if (parray[j].bullet[i][0] == 0)
								{
									parray[j].bullet[i][0] = 1;
									break;
								}
							}
						}
					}
					reload = 0;
				}

				Tdetaheffect();
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
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, IParam));//위의 세 메세지 외의 나머지 메세지는 OS로
}
