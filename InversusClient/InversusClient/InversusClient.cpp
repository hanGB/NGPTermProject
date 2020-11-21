
#include "stdafx.h"
#include "resource.h"
#include <atlImage.h>
#include <conio.h>
#include "IVSSCNetwork.h"
#include "IVSSCGlobals.h"
#include "IVSSCGame.h"
#include "IVSSCUpdate.h"
#include "IVSSCRender.h"

HINSTANCE g_hinst;
LPCTSTR lpszClass = "Window Class Name";

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM IParam);
BOOL CALLBACK DialogProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);

DWORD WINAPI ClientMain(LPVOID arg);//클라이언트 주 스레드

SOCKET sock;

player parray[MAX_PLAYER];
int block[BOARD_SIZE][BOARD_SIZE] = { 0 };//보드

GameObjects g_GameObjects;

CData clnt_data;
Clinfo clnt_info;

int clientcount = 5;
char serverip[32];

bool g_bReadyToSend = false;

HWND hWnd;
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hPrevInstance, LPSTR IpszCmdParam, int nCmdShow)
{
	//HWND hWnd;
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
	WndClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	WndClass.lpszClassName = lpszClass;
	WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	//윈도우 클래스 등록
	RegisterClassEx(&WndClass);
	//윈도우 생성
	hWnd = CreateWindow(lpszClass, "window program", WS_OVERLAPPEDWINDOW, 0, 0, 1024, 768, NULL, (HMENU)NULL, hinstance, NULL);
	//윈도우 출력
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	CreateThread(NULL, 0, ClientMain, NULL, 0, NULL);
	//이벤트 루프 처리
	while (GetMessage(&Message, 0, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}

	closesocket(sock);
	WSACleanup();
	return Message.wParam;
}

BOOL CALLBACK DialogProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg) {
	case WM_INITDIALOG:
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			GetDlgItemText(hDlg, IDC_IPADDRESS1, serverip, 100); // 스트링 복사
			EndDialog(hDlg, 0);
			break;
		case IDCANCEL:
			EndDialog(hDlg, 0);
			break;
		}
		break;
	}
	return 0;
}

DWORD WINAPI ClientMain(LPVOID arg)
{
	WSADATA wsa;
	HANDLE sendThread, recvThread;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	sock = socket(AF_INET, SOCK_STREAM, 0);

	// Turn Off Nagle Algorithm 
	bool optval = TRUE;
	setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char*)&optval, sizeof(optval));

	DialogBox(g_hinst, MAKEINTRESOURCE(IDC_IPADDRESS1), hWnd, (DLGPROC)&DialogProc);

	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(serverip);
	serveraddr.sin_port = htons(SERVERPORT);
	connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));

	ZeroMemory(&g_GameObjects, sizeof(GameObjects));

	int GetSize;
	char suBuffer[BUFSIZE];
	GetSize = recv(sock, suBuffer, sizeof(suBuffer) - 1, 0);
	suBuffer[GetSize] = '\0';
	clnt_info = *(Clinfo*)suBuffer;
	clnt_data.ci = clnt_info.ci;

	clnt_data.dx = 0, clnt_data.dy = 0;

	sendThread = CreateThread(NULL, 0, SendMsg, NULL, 0, NULL);
	recvThread = CreateThread(NULL, 0, RecvMsg, NULL, 0, NULL);
	WaitForSingleObject(sendThread, INFINITE);//전송용 쓰레드가 중지될때까지 기다린다.
	WaitForSingleObject(recvThread, INFINITE);//수신용 쓰레드가 중지될때까지 기다린다.

}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM IParam)
{
	PAINTSTRUCT ps;
	static HDC hDC, hMemDC;
	RECT regg[6];// = { 400, 300, 500, 310 };//총알 날라가는거
	RECT segg[18];//특수총알
	static RECT rectView;
	static RECT tect;
	static LOGFONT lf;
	static double sx, sy; //사각형 크기
	static double rx[7], ry[7];//총알 회전위치
	static double cx = 400, cy = 300;//주인공 첫 위치
	static double bullet[6][4] = { 0 };//총알
	static double seta = 0;//총알 돌리기 위한 세타
	static int reload = 0;//총알 장전


	static double effect[LIMIT_ENEMY + 1][17];//사망 이펙트
	static double reffect[LIMIT_ENEMY][4] = { 0 };//리스폰 이펙트

	static double enemy[LIMIT_ENEMY][5] = { 0 };//적
	static int ecolor[3];//적 색깔
	static int ecount = 0; //적 카운트
	static int etime = 30; // 적 생성시간

	static int dcount = 0;//부활 카운트
	static int life = 3;

	static double sgun[LIMIT_SGUN][3] = { 0 };//특수총알
	static int scount = 0;

	static int score = 0;
	static int combo = 0;

	static int ten = 0;
	static int gametime = 0;
	static char str[120];

	static int level = 1;
	static BOOL death = false;
	static BOOL multi = true;
									 //메세지 처리하기
	switch (iMessage) {
	case WM_CREATE:
		srand((unsigned)time(NULL));
		SetGame(hWnd, &rectView, &tect, &sx, &sy, block, bullet, ecolor, &cx, &cy, &seta, &reload
			, effect, reffect, enemy, &ecount, &etime, &death, &dcount, &life, &score, &combo
			, &ten, &gametime, level, sgun, &scount, multi);
		ZeroMemory(&lf, sizeof(lf));

		multireset(parray, rectView, sx, sy);
		SetTimer(hWnd, 0, 16, NULL);
		SetTimer(hWnd, 1, 33, NULL);

		return 0;
	case WM_PAINT:
	{
		hDC = BeginPaint(hWnd, &ps);

		CImage img;
		img.Create(rectView.right, rectView.bottom, 24);

		HBRUSH hBrush, oldBrush;
		HFONT Font, OldFont;
		HPEN hPen, OldPen;

		hMemDC = img.GetDC();
		{
			//-------------------------------------------------------
			//보드판
			if (g_GameObjects.GameState == 1)
			{
				Hscorebord(hMemDC, rectView, sx, sy, life, str, score, combo, tect, &lf, multi);
				Hcreateboad(block, sx, sy, hMemDC);
				{
					//주인공
					for (int i = 0; i < MAX_PLAYER; i++)
					{
						if (parray[i].enable == true)
						{
							if (i == clnt_info.ci) {
								hBrush = CreateSolidBrush(RGB(200, 10, 10));
								oldBrush = (HBRUSH)SelectObject(hMemDC, hBrush);
							}
							else {
								hBrush = CreateSolidBrush(RGB(125, 125, 125));//검정
								oldBrush = (HBRUSH)SelectObject(hMemDC, hBrush);
							}
							Rectangle(hMemDC, parray[i].cx - sx / 2, parray[i].cy - sy / 2, parray[i].cx + sx / 2, parray[i].cy + sy / 2);
							SelectObject(hMemDC, oldBrush);
							DeleteObject(hBrush);
						}
					}
					//총알
					for (int j = 0; j < MAX_PLAYER; j++)
					{
						for (int i = 0; i < 6; i++)
						{
							//총알 도는거
							if (parray[j].bullet[i][0] == 1 || parray[j].bullet[i][0] == 2)
							{
								if (parray[j].enable == true)
								{
									if (parray[j].bullet[i][0] == 1) {
										hBrush = CreateSolidBrush(RGB(255, 255, 255));//흰색
										oldBrush = (HBRUSH)SelectObject(hMemDC, hBrush);
									}
									else {
										hBrush = CreateSolidBrush(RGB(ecolor[0], ecolor[1], ecolor[2]));//적
										oldBrush = (HBRUSH)SelectObject(hMemDC, hBrush);
									}
									Ellipse(hMemDC, parray[j].rx[i] - sx / 10, parray[j].ry[i] - sx / 10, parray[j].rx[i] + sy / 10, parray[j].ry[i] + sy / 10);
									SelectObject(hMemDC, oldBrush);
									DeleteObject(hBrush);
								}
							}
							else if (parray[j].bullet[i][0] == 3)
							{
								//총알 쏜거
								Hshotbullet(parray[j].bullet, parray[j].regg, hMemDC, i, 0, ecolor, 0);
							}
						}
					}
				}
				//사망이펙트
				Hdeatheffect(hMemDC, ecolor);

				// 부활 이펙트
				Hrespwan(hMemDC, ecolor, sx, sy);

				Font = CreateFontIndirect(&lf);
				OldFont = (HFONT)SelectObject(hMemDC, Font);

				SetBkColor(hMemDC, RGB(255, 255, 255));
				RECT aect = { rectView.right * 3 / 32, rectView.bottom * 1 / 16,
					rectView.right * 15 / 32, rectView.bottom * 3 / 16 };

				wsprintf(str, "Clint id = %d", clnt_data.ci);
				DrawText(hMemDC, str, -1, &aect, DT_CENTER);

				aect = { rectView.right * 12 / 16, rectView.bottom * 1 / 32,
					rectView.right * 15 / 16, rectView.bottom * 8 / 32 };

				if (parray[clnt_data.ci].life >= 0) {
					SetTextColor(hMemDC, RGB(0, 0, 255));
					wsprintf(str, "LIFE: %d", parray[clnt_data.ci].life);
				}
				else {
					SetTextColor(hMemDC, RGB(255, 0, 0));
					wsprintf(str, "DEATH");
				}
				DrawText(hMemDC, str, -1, &aect, DT_CENTER);
				SetTextColor(hMemDC, RGB(0, 0, 0));

				SelectObject(hMemDC, OldFont);
				DeleteObject(Font);
				
			}
			else
			{
				
				hBrush = CreateSolidBrush(RGB(255, 255, 255));
				oldBrush = (HBRUSH)SelectObject(hMemDC, hBrush);
				Rectangle(hMemDC, 0, 0, rectView.right, rectView.bottom);
				SelectObject(hMemDC, oldBrush);
				DeleteObject(hBrush);

				lf.lfHeight = 50;
				Font = CreateFontIndirect(&lf);
				OldFont = (HFONT)SelectObject(hMemDC, Font);
				wsprintf(str, "Inversus Waiting Room");
				RECT aect = { rectView.right / 2 - 500, 50, rectView.right / 2 + 500, 250 };
				SetTextColor(hMemDC, RGB(0, 0, 0));
				DrawText(hMemDC, str, -1, &aect, DT_CENTER);
				SelectObject(hMemDC, OldFont);
				DeleteObject(Font);

				for (int i = 0; i < MAX_PLAYER; i++)
				{
					if (parray[i].enable == true)
					{
						hBrush = CreateSolidBrush(RGB(200, 10, 10));
						oldBrush = (HBRUSH)SelectObject(hMemDC, hBrush);
						if (parray[i].gameready == true)
						{
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

				lf.lfHeight = 50;
				Font = CreateFontIndirect(&lf);
				OldFont = (HFONT)SelectObject(hMemDC, Font);
				wsprintf(str, "Player%d", clnt_data.ci);
				aect = { rectView.right / 2 - 500, 500, rectView.right / 2 + 500, 600 };
				SetTextColor(hMemDC, RGB(0, 0, 0));
				DrawText(hMemDC, str, -1, &aect, DT_CENTER);
				SelectObject(hMemDC, OldFont);
				DeleteObject(Font);

				lf.lfHeight = 30;
				Font = CreateFontIndirect(&lf);
				OldFont = (HFONT)SelectObject(hMemDC, Font);
				wsprintf(str, "준비가 되면 스페이스바를 눌러주세요");
				aect = { rectView.right / 2 - 500, 600, rectView.right / 2 + 500, 700 };
				SetTextColor(hMemDC, RGB(0, 0, 0));
				DrawText(hMemDC, str, -1, &aect, DT_CENTER);
				SelectObject(hMemDC, OldFont);
				DeleteObject(Font);
				
			}

			img.Draw(hDC, 0, 0);
			img.ReleaseDC();
		}
		//-----------------------------------------------------------------------------------

		EndPaint(hWnd, &ps);
		break;
	}
	case WM_TIMER: // 시간이 경과하면 메시지 자동 생성
		switch (wParam) {
		case 0:
			//60fps로 화면 렌더링
			InvalidateRect(hWnd, NULL, FALSE);
			UpdateWindow(hWnd);
			break;
		case 1:
			//30fps로 데이터 전송
			if (!g_bReadyToSend)
				g_bReadyToSend = true;
		}
		break;

	case WM_CHAR:
		break;

	case WM_KEYDOWN:
	{
		if (wParam == 'w' || wParam == 'W')
		{
			clnt_data.p_key.KEY_W = true;
		}
		if (wParam == 's' || wParam == 'S')
		{
			clnt_data.p_key.KEY_S = true;
		}
		if (wParam == 'd' || wParam == 'D')
		{
			clnt_data.p_key.KEY_D = true;
		}
		if (wParam == 'a' || wParam == 'A')
		{
			clnt_data.p_key.KEY_A = true;
		}
		if (wParam == VK_UP)
		{
			clnt_data.p_key.ARROW_UP = true;
		}
		if (wParam == VK_DOWN)
		{
			clnt_data.p_key.ARROW_DOWN = true;
		}
		if (wParam == VK_LEFT)
		{
			clnt_data.p_key.ARROW_LEFT = true;
		}
		if (wParam == VK_RIGHT)
		{
			clnt_data.p_key.ARROW_RIGHT = true;
		}
		if (wParam == VK_SPACE)
		{
			clnt_data.p_key.KEY_SPACEBAR = true;
		}
	}
	break;

	case WM_KEYUP:

		if (wParam == 'w' || wParam == 'W')
		{
			clnt_data.p_key.KEY_W = false;
		}
		if (wParam == 's' || wParam == 'S')
		{
			clnt_data.p_key.KEY_S = false;
		}
		if (wParam == 'd' || wParam == 'D')
		{
			clnt_data.p_key.KEY_D = false;
		}
		if (wParam == 'a' || wParam == 'A')
		{
			clnt_data.p_key.KEY_A = false;
		}
		if (wParam == VK_UP)
		{
			clnt_data.p_key.ARROW_UP = false;
			clnt_data.coolTime = 0;
		}
		if (wParam == VK_DOWN)
		{
			clnt_data.p_key.ARROW_DOWN = false;
			clnt_data.coolTime = 0;
		}
		if (wParam == VK_LEFT)
		{
			clnt_data.p_key.ARROW_LEFT = false;
			clnt_data.coolTime = 0;
		}
		if (wParam == VK_RIGHT)
		{
			clnt_data.p_key.ARROW_RIGHT = false;
			clnt_data.coolTime = 0;
		}
		if (wParam == VK_SPACE)
		{
			clnt_data.p_key.KEY_SPACEBAR = false;
		}
		break;
	case WM_COMMAND:
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, IParam));//위의 세 메세지 외의 나머지 메세지는 OS로
}