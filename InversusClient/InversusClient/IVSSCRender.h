#pragma once

//그라데이션
void DrawGr(HDC pDC, COLORREF start, COLORREF finish, RECT prect, BOOL direct); 

//보드판
void Hcreateboad(int block[][20], float dx, float dy, HDC hMemDC);

//부활 이펙트
void Hrespwan(HDC hMemDC, int* ecolor, float dx, float dy);

//데스이펙트
void Hdeatheffect(HDC hMemDC, int* ecolor);

//상단 스코어 보드판
void Hscorebord(HDC hMemDC, RECT rectView, float dx, float dy, int life, 
	char* str, int score, int combo, RECT tect, LOGFONT* lf, BOOL multi);

//총알 나가는거 생성
void Hshotbullet(float bullet[][4], HDC hMemDC, int i, int check, int* ecolor, int ch);

void DrawGameWin(HDC hMemDC, RECT rectView);
