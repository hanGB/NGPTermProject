#pragma once

//�׶��̼�
void DrawGr(HDC pDC, COLORREF start, COLORREF finish, RECT prect, BOOL direct); 

//������
void Hcreateboad(int block[][20], double dx, double dy, HDC hMemDC);

//������ ���� Ư���Ѿ�
void Hsgun(double sgun[][3], double seta, double dx, double dy, HDC hMemDC);

//��Ȱ ����Ʈ
void Hrespwan(HDC hMemDC, int* ecolor, double dx, double dy);

//��������Ʈ
void Hdeatheffect(HDC hMemDC, int* ecolor);

//��� ���ھ� ������
void Hscorebord(HDC hMemDC, RECT rectView, double dx, double dy, int life, 
	char* str, int score, int combo, RECT tect, LOGFONT* lf, BOOL multi);

//�Ѿ� �����°� ����
void Hshotbullet(double bullet[][4], RECT* regg, HDC hMemDC, int i, int check, int* ecolor, int ch);

//�ϴ��� ���� �޼���
void Hgamewin(HDC hMemDC, RECT rectView, int check, char* str, HWND hWnd);

//���� ���� �޼���
void Hgameover(HDC hMemDC, RECT rectView, int score, char* str, HWND hWnd);

void DrawGameWin(HDC hMemDC, RECT rectView);
