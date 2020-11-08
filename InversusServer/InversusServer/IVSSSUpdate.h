#pragma once

//플레이어 오브젝트 이동
void move_player_object(int playerid);

//검은벽 움직이지 못하게
void ColRect(RECT rec, RECT& rec2, double* cx, double* cy);

//총알 장전
void ReloadBullet(int* reload, double bullet[][4], int time);

//총알 이동
void MoveBullet(RECT rectView, double bullet[][4], int speed, int* combo, BOOL multi, RECT* regg, RECT* eegg);

//총알-블록 충돌
void Hcolblock(double dx, double dy, RECT* regg, int block[][20], 
	int* score, int* combo, double bullet[][4], int i, int check, int ch);

//일대일 총알 맞음
void Hcolplayer(double cx, double cy, double dx, double dy, RECT* regg, double effect[][17], 
	int i, HDC hMemDC, RECT rectView, int check, char* str, HWND hWnd, BOOL* death);

//보드판 위에 특수총알
void Hsgun(double sgun[][3], double seta, double dx, double dy, HDC hMemDC);

//플레이어가 특수총알 먹는거
void Hplayersgun(double sgun[][3], double dx, double dy, double bullet[][4], double cx, double cy, int* score);

//총알 돌아가는거
void Hrotategun(double* rx, double* ry, double cx, double cy, double dx, double dy, double seta, int i, 
	BOOL death, double bullet[][4], HBRUSH oldBrush, HBRUSH hBrush, HBRUSH eBrush, HDC hMemDC);

//방향키 총알 발사
void Kshotbullet(double bullet[][4], double cx, double cy, int check);

//죽을때 이펙트 계산
void DeathEffect(double effect[][17], double x, double y, int dx, int i);

//데스이펙타임
void Tdetaheffect(double effect[][17]);

//리스폰 할때 이펙트 계산
void RespawnEffect(double reffect[][4], double x, double y, int nu);

//리스폰이펙타임 
void Trespawneffect(double reffect[][4], int gametime, BOOL* death, double enemy[][5]);
