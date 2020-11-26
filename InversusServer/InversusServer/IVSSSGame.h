#pragma once

//게임 세팅
void SetGame(HWND hWnd, RECT* rectView, RECT* tect, double* dx, double* dy, 
	int block[][20], double bullet[][4], int* ecolor, double* cx, double* cy, 
	double* seta, int* reload, double effect[][17], double reffect[][4], double enemy[][5], 
	int* ecount, int* etime, BOOL* death, int* dcount, int* life, int* score, int* combo, int* ten, 
	int* gametime, int level, double sgun[][3], int* scount, BOOL multi);
