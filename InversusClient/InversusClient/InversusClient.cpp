
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
	HBRUSH hBrush, oldBrush, eBrush, eBrush2, ehBrush, unBrush;
	HBRUSH hBrush2, hBrush3, oldBrush2;
	HPEN MyPen, ePen, OldPen, cPen, unPen, iPen;
	HFONT Font, OldFont;
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

	static BOOL death = false;//플레이어 죽음
	static BOOL infi = false;
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
		CImage img;
		img.Create(rectView.right, rectView.bottom, 24);
		hDC = BeginPaint(hWnd, &ps);
		MyPen = CreatePen(PS_SOLID, 3, RGB(125, 125, 125));//보드
		ePen = CreatePen(PS_SOLID, 3, RGB(ecolor[0], ecolor[1], ecolor[2]));//적
		cPen = CreatePen(PS_SOLID, 0, RGB(0, 0, 0));//주인공
		iPen = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));//무적 주인공
		unPen = (HPEN)GetStockObject(NULL_BRUSH);//투명

		unBrush = (HBRUSH)GetStockObject(NULL_BRUSH);//투명
		hBrush = CreateSolidBrush(RGB(255, 255, 255));//흰색
		hBrush2 = CreateSolidBrush(RGB(0, 0, 0));//검정
		hBrush3 = CreateSolidBrush(RGB(125, 125, 125));//검정
		eBrush = CreateSolidBrush(RGB(ecolor[0], ecolor[1], ecolor[2]));//적
		eBrush2 = CreateSolidBrush(RGB(200, 10, 10));
		ehBrush = CreateHatchBrush(HS_BDIAGONAL, RGB(ecolor[0], ecolor[1], ecolor[2]));//적 빗금

		hMemDC = img.GetDC();
		{
			//-------------------------------------------------------
			//보드판
			Font = CreateFontIndirect(&lf);
			OldFont = (HFONT)SelectObject(hMemDC, Font);
			oldBrush = (HBRUSH)SelectObject(hMemDC, hBrush);
			Hscorebord(oldBrush, hBrush, hBrush2, eBrush, hMemDC, rectView, sx, sy, life, Font, OldFont, str, score, combo, tect, &lf, multi);

			OldPen = (HPEN)SelectObject(hMemDC, MyPen);
			Hcreateboad(block, sx, sy, hBrush, hBrush2, hBrush3, oldBrush, hMemDC);
			{
				OldPen = (HPEN)SelectObject(hMemDC, cPen);
				//주인공
				oldBrush = (HBRUSH)SelectObject(hMemDC, hBrush2);
				if (death == false)
				{
					for (int i = 0; i < clientcount; i++)
					{
						if (parray[i].enable == true)
						{
							oldBrush = (HBRUSH)SelectObject(hMemDC, hBrush3);
							if (i == clnt_info.ci)
								oldBrush = (HBRUSH)SelectObject(hMemDC, eBrush2);
							Rectangle(hMemDC, parray[i].cx - sx / 2, parray[i].cy - sy / 2, parray[i].cx + sx / 2, parray[i].cy + sy / 2);
						}
					}
				}
				OldPen = (HPEN)SelectObject(hMemDC, cPen);
				oldBrush = (HBRUSH)SelectObject(hMemDC, hBrush);

				//총알
				for (int j = 0; j < clientcount; j++)
				{
					for (int i = 0; i < 6; i++)
					{
						//총알 도는거
						if (parray[j].bullet[i][0] == 1 || parray[j].bullet[i][0] == 2)
						{
							if (death == false && parray[j].enable == true)
							{
								if (parray[j].bullet[i][0] == 1)
									oldBrush = (HBRUSH)SelectObject(hMemDC, hBrush);
								else
									oldBrush = (HBRUSH)SelectObject(hMemDC, eBrush);
								Ellipse(hMemDC, parray[j].rx[i] - sx / 10, parray[j].ry[i] - sx / 10, parray[j].rx[i] + sy / 10, parray[j].ry[i] + sy / 10);
							}
						}
						else if (parray[j].bullet[i][0] == 3)
						{
							//총알 쏜거
							Hshotbullet(parray[j].bullet, parray[j].regg, hMemDC, i, 0, ecolor, 0);
							//총알-블록 충돌
							//Hcolblock(sx, sy, parray[0].regg, block, &score, &combo, parray[0].bullet, i, 0, 0);
							//총알-적 충돌
							//Hcolplayer(parray[1].cx, parray[1].cy, sx, sy, parray[0].regg, effect, i, hMemDC, rectView, 0, str, hWnd, &death);
						}
					}
				}
			}
			//사망이펙트
			Hdeatheffect(oldBrush, hBrush2, eBrush, hMemDC);

			SetBkColor(hMemDC, RGB(255, 255, 255));
			RECT aect = { rectView.right * 3 / 32, rectView.bottom * 1 / 16,
				rectView.right * 15 / 32, rectView.bottom * 3 / 16};

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

			SelectObject(hMemDC, oldBrush);
			DeleteObject(hBrush);
			DeleteObject(eBrush);
			DeleteObject(eBrush2);
			DeleteObject(MyPen);
			DeleteObject(cPen);
			DeleteObject(ePen);
			DeleteObject(iPen);
			DeleteObject(hBrush2);
			DeleteObject(hBrush3);
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

		if (death == true)
		{
			if (wParam == VK_RETURN)
			{
				SetGame(hWnd, &rectView, &tect, &sx, &sy, block, bullet, ecolor, &cx, &cy, &seta, &reload
					, effect, reffect, enemy, &ecount, &etime, &death, &dcount, &life, &score, &combo
					, &ten, &gametime, level, sgun, &scount, multi);
				for (int i = 0; i < 2; i++)
				{
					for (int k = 0; k < 6; k++)
						parray[i].bullet[k][0] = 0;
				}

				parray[0].cx = sx * 2;
				parray[0].cy = sy * 4;

				parray[1].cx = rectView.right - sx * 2;
				parray[1].cy = rectView.bottom - sy * 2;
			}
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
		break;
	case WM_COMMAND:
		break;

	case WM_DESTROY:
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, IParam));//위의 세 메세지 외의 나머지 메세지는 OS로
}