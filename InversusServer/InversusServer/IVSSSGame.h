#pragma once

//게임 세팅
void SetGame(HWND hWnd, RECT* rectView, RECT* tect, float* dx, float* dy, 
	int block[][20], float bullet[][4], int* ecolor, float* cx, float* cy, 
	float* seta, int* reload, float effect[][17], float reffect[][4], float enemy[][5], 
	int* ecount, int* etime, BOOL* death, int* dcount, int* life, int* score, int* combo, int* ten, 
	int* gametime, int level, float sgun[][3], int* scount, BOOL multi);
