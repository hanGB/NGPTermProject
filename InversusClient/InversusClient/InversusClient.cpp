
#include "resource.h"
#include "InversusClient.h"
#include <atlImage.h>
#include <conio.h>

#define SERVERIP "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE 3000
#define MAX_CLNT 256


HINSTANCE g_hinst;
LPCTSTR lpszClass = "Window Class Name";
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM IParam);

DWORD WINAPI ClientMain(LPVOID arg);
DWORD WINAPI SendMsg(LPVOID arg);//������ �����Լ�
DWORD WINAPI RecvMsg(LPVOID arg);//������ �����Լ�

SOCKET sock;

player parray[MAX_CLNT];

typedef struct Clinfo {
	int ci;
}Clinfo;

typedef struct CData {//Ŭ���̾�Ʈ�κ��� ���� ������
	int ci;
	int dx = 0, dy = 0; //����
}CData;


CData clnt_data;
Clinfo clnt_info;

int clientcount = 5;

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

DWORD WINAPI ClientMain(LPVOID arg)
{
	WSADATA wsa;
	HANDLE sendThread, recvThread;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	sock = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));

	int GetSize;
	char suBuffer[BUFSIZE];
	GetSize = recv(sock, suBuffer, sizeof(suBuffer) - 1, 0);
	suBuffer[GetSize] = '\0';
	clnt_info = *(Clinfo*)suBuffer;
	clnt_data.ci = clnt_info.ci;

	sendThread = CreateThread(NULL, 0, SendMsg, NULL, 0, NULL);
	recvThread = CreateThread(NULL, 0, RecvMsg, NULL, 0, NULL);
	WaitForSingleObject(sendThread, INFINITE);//���ۿ� �����尡 �����ɶ����� ��ٸ���./
	WaitForSingleObject(recvThread, INFINITE);//���ſ� �����尡 �����ɶ����� ��ٸ���.

}

DWORD WINAPI SendMsg(LPVOID arg) {//���ۿ� �������Լ�
	while (1) {//�ݺ�

		Sleep(10);
		send(sock, (char*)&clnt_data, sizeof(CData), 0);
	}
	return 0;
}

DWORD WINAPI RecvMsg(LPVOID arg) {
	int len;
	while (1) {//�ݺ�

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

	static int block[20][20] = { 0 };//����
									 //�޼��� ó���ϱ�
	switch (iMessage) {
	case WM_CREATE:
		srand((unsigned)time(NULL));
		SetGame(hWnd, &rectView, &tect, &sx, &sy, block, bullet, ecolor, &cx, &cy, &seta, &reload
			, effect, reffect, enemy, &ecount, &etime, &death, &dcount, &life, &score, &combo
			, &ten, &gametime, level, sgun, &scount, multi);
		ZeroMemory(&lf, sizeof(lf));
		multireset(parray, rectView, sx, sy);
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
							Hshotbullet(parray[0].bullet, parray[0].regg, hMemDC, i, 0, ecolor, 0);
							//�Ѿ�-��� �浹
							Hcolblock(sx, sy, parray[0].regg, block, &score, &combo, parray[0].bullet, i, 0, 0);
							//�Ѿ�-�� �浹
							Hcolplayer(parray[1].cx, parray[1].cy, sx, sy, parray[0].regg, effect, i, hMemDC, rectView, 0, str, hWnd, &death);
						}
					}
				}
			}
			//�������Ʈ
			Hdeatheffect(effect, oldBrush, hBrush2, eBrush, hMemDC);

			SetBkColor(hMemDC, RGB(255, 255, 255));
			RECT aect = { rectView.left,rectView.bottom * 1 / 4,rectView.right,rectView.bottom * 3 / 4 };
			wsprintf(str, "Clint id = %d", clnt_data.ci);
			DrawText(hMemDC, str, -1, &aect, DT_CENTER);

			if (life == 0)
			{
				Hgameover(hMemDC, rectView, score, str, hWnd);
			}
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
		case 1:
			if (seta < 2 * 3.14)
			{
				seta += 0.3;
			}
			else
			{
				seta = 0;
			}

			{
				MoveBullet(rectView, parray[0].bullet, 50, &combo, multi, parray[0].regg, parray[1].regg);
				MoveBullet(rectView, parray[1].bullet, 50, &combo, multi, parray[1].regg, parray[0].regg);
				Tdetaheffect(effect);

				if (death == false)
				{
					MovePlayer(&clnt_data.dx, &clnt_data.dy);
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
				score++;
			}
			break;
		case 2:

			break;
		}
		InvalidateRect(hWnd, NULL, FALSE);
		break;

	case WM_CHAR:
		if (multi == true)
		{
			if (wParam == 'u' || wParam == 'U')
			{
				Kshotbullet(parray[0].bullet, parray[0].cx, parray[0].cy, 2);
			}
			else if (wParam == 'j' || wParam == 'J')
			{
				Kshotbullet(parray[0].bullet, parray[0].cx, parray[0].cy, 3);
			}
			else if (wParam == 'h' || wParam == 'H')
			{
				Kshotbullet(parray[0].bullet, parray[0].cx, parray[0].cy, 1);
			}
			else if (wParam == 'k' || wParam == 'K')
			{
				Kshotbullet(parray[0].bullet, parray[0].cx, parray[0].cy, 0);
			}
		}
		//InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_KEYDOWN:
		//MessageBox(NULL, "test", "test", MB_OK);

	{
		if (wParam == VK_NUMPAD5)
		{
			Kshotbullet(parray[1].bullet, parray[1].cx, parray[1].cy, 2);
		}
		else if (wParam == VK_NUMPAD2)
		{
			Kshotbullet(parray[1].bullet, parray[1].cx, parray[1].cy, 3);
		}
		else if (wParam == VK_NUMPAD1)
		{
			Kshotbullet(parray[1].bullet, parray[1].cx, parray[1].cy, 1);
		}
		else if (wParam == VK_NUMPAD3)
		{
			Kshotbullet(parray[1].bullet, parray[1].cx, parray[1].cy, 0);
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
	InvalidateRect(hWnd, NULL, FALSE);
	break;
	case WM_KEYUP:

		InvalidateRect(hWnd, NULL, FALSE);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case ID_40001://�ʱ�
			level = 0;
			SetGame(hWnd, &rectView, &tect, &sx, &sy, block, bullet, ecolor, &cx, &cy, &seta, &reload
				, effect, reffect, enemy, &ecount, &etime, &death, &dcount, &life, &score, &combo
				, &ten, &gametime, level, sgun, &scount, multi);
			multireset(parray, rectView, sx, sy);
			break;
		case ID_40002://�߱�
			level = 1;
			SetGame(hWnd, &rectView, &tect, &sx, &sy, block, bullet, ecolor, &cx, &cy, &seta, &reload
				, effect, reffect, enemy, &ecount, &etime, &death, &dcount, &life, &score, &combo
				, &ten, &gametime, level, sgun, &scount, multi);
			multireset(parray, rectView, sx, sy);
			break;
		case ID_40003://���
			level = 2;
			SetGame(hWnd, &rectView, &tect, &sx, &sy, block, bullet, ecolor, &cx, &cy, &seta, &reload
				, effect, reffect, enemy, &ecount, &etime, &death, &dcount, &life, &score, &combo
				, &ten, &gametime, level, sgun, &scount, multi);
			multireset(parray, rectView, sx, sy);
			break;
		case ID_40004:
			multi = false;
			SetGame(hWnd, &rectView, &tect, &sx, &sy, block, bullet, ecolor, &cx, &cy, &seta, &reload
				, effect, reffect, enemy, &ecount, &etime, &death, &dcount, &life, &score, &combo
				, &ten, &gametime, level, sgun, &scount, multi);
			multireset(parray, rectView, sx, sy);
			break;

		case ID_40005:
			multi = true;
			SetGame(hWnd, &rectView, &tect, &sx, &sy, block, bullet, ecolor, &cx, &cy, &seta, &reload
				, effect, reffect, enemy, &ecount, &etime, &death, &dcount, &life, &score, &combo
				, &ten, &gametime, level, sgun, &scount, multi);
			multireset(parray, rectView, sx, sy);
			break;
		}
		break;
	case WM_DESTROY:
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, IParam));//���� �� �޼��� ���� ������ �޼����� OS��
}