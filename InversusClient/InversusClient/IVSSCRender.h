#pragma once

//�׶��̼�
void DrawGr(HDC pDC, COLORREF start, COLORREF finish, RECT prect, BOOL direct); 

//������
void Hcreateboad(int block[][20], double dx, double dy, HBRUSH hBrush, HBRUSH hBrush2, HBRUSH hBrush3, 
	HBRUSH oldBrush, HDC hMemDC);

//������ ���� Ư���Ѿ�
void Hsgun(double sgun[][3], double seta, double dx, double dy, HDC hMemDC);

//�Ѿ� ȸ��
void Hrotategun(double* rx, double* ry, double cx, double cy, double dx, double dy, double seta, int i, BOOL death,
	double bullet[][4], HBRUSH oldBrush, HBRUSH hBrush, HBRUSH eBrush, HDC hMemDC);

//��Ȱ ����Ʈ
void Hrespwan(HPEN OldPen, HBRUSH oldBrush, HBRUSH unBrush, HBRUSH hBrush2, HBRUSH eBrush, 
	HPEN cPen, HPEN ePen, HDC hMemDC, double dx, double dy, double reffect[][4]);

//��������Ʈ
void Hdeatheffect(HBRUSH oldBrush, HBRUSH hBrush2, HBRUSH eBrush, HDC hMemDC);

//��� ���ھ� ������
void Hscorebord(HBRUSH oldBrush, HBRUSH hBrush, HBRUSH hBrush2, HBRUSH eBrush, HDC hMemDC, RECT rectView, 
	double dx, double dy, int life, HFONT Font, HFONT OldFont, 
	char* str, int score, int combo, RECT tect, LOGFONT* lf, BOOL multi);

//�ϴ��� ���� �޼���
void Hgamewin(HDC hMemDC, RECT rectView, int check, char* str, HWND hWnd);

//���� ���� �޼���
void Hgameover(HDC hMemDC, RECT rectView, int score, char* str, HWND hWnd);
