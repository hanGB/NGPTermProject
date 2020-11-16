
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

DWORD WINAPI ClientMain(LPVOID arg);//Ŭ���̾�Ʈ �� ������

SOCKET sock;

player parray[MAX_PLAYER];
int block[BOARD_SIZE][BOARD_SIZE] = { 0 };//����

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

	CreateThread(NULL, 0, ClientMain, NULL, 0, NULL);
	//�̺�Ʈ ���� ó��
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
			GetDlgItemText(hDlg, IDC_IPADDRESS1, serverip, 100); // ��Ʈ�� ����
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
	WaitForSingleObject(sendThread, INFINITE);//���ۿ� �����尡 �����ɶ����� ��ٸ���.
	WaitForSingleObject(recvThread, INFINITE);//���ſ� �����尡 �����ɶ����� ��ٸ���.

}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM IParam)
{
	PAINTSTRUCT ps;
	static HDC hDC, hMemDC;
	HBRUSH hBrush, oldBrush, eBrush, eBrush2, ehBrush, unBrush;
	HBRUSH hBrush2, hBrush3, oldBrush2;
	HPEN MyPen, ePen, OldPen, cPen, unPen, iPen;
	HFONT Font, OldFont;
	RECT regg[6];// = { 400, 300, 500, 310 };//�Ѿ� ���󰡴°�
	RECT segg[18];//Ư���Ѿ�
	static RECT rectView;
	static RECT tect;
	static LOGFONT lf;
	static double sx, sy; //�簢�� ũ��
	static double rx[7], ry[7];//�Ѿ� ȸ����ġ
	static double cx = 400, cy = 300;//���ΰ� ù ��ġ
	static double bullet[6][4] = { 0 };//�Ѿ�
	static double seta = 0;//�Ѿ� ������ ���� ��Ÿ
	static int reload = 0;//�Ѿ� ����


	static double effect[LIMIT_ENEMY + 1][17];//��� ����Ʈ
	static double reffect[LIMIT_ENEMY][4] = { 0 };//������ ����Ʈ

	static double enemy[LIMIT_ENEMY][5] = { 0 };//��
	static int ecolor[3];//�� ����
	static int ecount = 0; //�� ī��Ʈ
	static int etime = 30; // �� �����ð�

	static BOOL death = false;//�÷��̾� ����
	static BOOL infi = false;
	static int dcount = 0;//��Ȱ ī��Ʈ
	static int life = 3;

	static double sgun[LIMIT_SGUN][3] = { 0 };//Ư���Ѿ�
	static int scount = 0;

	static int score = 0;
	static int combo = 0;

	static int ten = 0;
	static int gametime = 0;
	static char str[120];

	static int level = 1;

	static BOOL multi = true;
									 //�޼��� ó���ϱ�
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
		MyPen = CreatePen(PS_SOLID, 3, RGB(125, 125, 125));//����
		ePen = CreatePen(PS_SOLID, 3, RGB(ecolor[0], ecolor[1], ecolor[2]));//��
		cPen = CreatePen(PS_SOLID, 0, RGB(0, 0, 0));//���ΰ�
		iPen = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));//���� ���ΰ�
		unPen = (HPEN)GetStockObject(NULL_BRUSH);//����

		unBrush = (HBRUSH)GetStockObject(NULL_BRUSH);//����
		hBrush = CreateSolidBrush(RGB(255, 255, 255));//���
		hBrush2 = CreateSolidBrush(RGB(0, 0, 0));//����
		hBrush3 = CreateSolidBrush(RGB(125, 125, 125));//����
		eBrush = CreateSolidBrush(RGB(ecolor[0], ecolor[1], ecolor[2]));//��
		eBrush2 = CreateSolidBrush(RGB(200, 10, 10));
		ehBrush = CreateHatchBrush(HS_BDIAGONAL, RGB(ecolor[0], ecolor[1], ecolor[2]));//�� ����

		hMemDC = img.GetDC();
		{
			//-------------------------------------------------------
			//������
			Font = CreateFontIndirect(&lf);
			OldFont = (HFONT)SelectObject(hMemDC, Font);
			oldBrush = (HBRUSH)SelectObject(hMemDC, hBrush);
			Hscorebord(oldBrush, hBrush, hBrush2, eBrush, hMemDC, rectView, sx, sy, life, Font, OldFont, str, score, combo, tect, &lf, multi);

			OldPen = (HPEN)SelectObject(hMemDC, MyPen);
			Hcreateboad(block, sx, sy, hBrush, hBrush2, hBrush3, oldBrush, hMemDC);
			{
				OldPen = (HPEN)SelectObject(hMemDC, cPen);
				//���ΰ�
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

				//�Ѿ�
				for (int j = 0; j < clientcount; j++)
				{
					for (int i = 0; i < 6; i++)
					{
						//�Ѿ� ���°�
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
							//�Ѿ� ���
							Hshotbullet(parray[j].bullet, parray[j].regg, hMemDC, i, 0, ecolor, 0);
							//�Ѿ�-��� �浹
							//Hcolblock(sx, sy, parray[0].regg, block, &score, &combo, parray[0].bullet, i, 0, 0);
							//�Ѿ�-�� �浹
							//Hcolplayer(parray[1].cx, parray[1].cy, sx, sy, parray[0].regg, effect, i, hMemDC, rectView, 0, str, hWnd, &death);
						}
					}
				}
			}
			//�������Ʈ
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
	case WM_TIMER: // �ð��� ����ϸ� �޽��� �ڵ� ����
		switch (wParam) {
		case 0:
			//60fps�� ȭ�� ������
			InvalidateRect(hWnd, NULL, FALSE);
			UpdateWindow(hWnd);
			break;
		case 1:
			//30fps�� ������ ����
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
	return(DefWindowProc(hWnd, iMessage, wParam, IParam));//���� �� �޼��� ���� ������ �޼����� OS��
}