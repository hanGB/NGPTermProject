#pragma once

//�׶��̼�
void DrawGr(HDC pDC, COLORREF start, COLORREF finish, RECT prect, BOOL direct); 

//������
void Hcreateboad(int block[][20], float dx, float dy, HDC hMemDC);

//��Ȱ ����Ʈ
void Hrespwan(HDC hMemDC, int* ecolor, float dx, float dy);

//��������Ʈ
void Hdeatheffect(HDC hMemDC, int* ecolor);

//��� ���ھ� ������
void Hscorebord(HDC hMemDC, RECT rectView, float dx, float dy, int life, 
	char* str, int score, int combo, RECT tect, LOGFONT* lf, BOOL multi);

//�Ѿ� �����°� ����
void Hshotbullet(float bullet[][4], HDC hMemDC, int i, int check, int* ecolor, int ch);

void DrawGameWin(HDC hMemDC, RECT rectView);
