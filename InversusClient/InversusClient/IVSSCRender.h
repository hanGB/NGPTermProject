#pragma once

//그라데이션
void DrawGr(HDC pDC, COLORREF start, COLORREF finish, RECT prect, BOOL direct); 

//보드판
void Hcreateboad(int block[][20], double dx, double dy, HDC hMemDC);

//보드판 위에 특수총알
void Hsgun(double sgun[][3], double seta, double dx, double dy, HDC hMemDC);

//부활 이펙트
void Hrespwan(HDC hMemDC, int* ecolor, double dx, double dy);

//데스이펙트
void Hdeatheffect(HDC hMemDC, int* ecolor);

//상단 스코어 보드판
void Hscorebord(HDC hMemDC, RECT rectView, double dx, double dy, int life, 
	char* str, int score, int combo, RECT tect, LOGFONT* lf, BOOL multi);

//총알 나가는거 생성
void Hshotbullet(double bullet[][4], RECT* regg, HDC hMemDC, int i, int check, int* ecolor, int ch);

//일대일 승패 메세지
void Hgamewin(HDC hMemDC, RECT rectView, int check, char* str, HWND hWnd);

//게임 오버 메세지
void Hgameover(HDC hMemDC, RECT rectView, int score, char* str, HWND hWnd);

void DrawGameWin(HDC hMemDC, RECT rectView);
