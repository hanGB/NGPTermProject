#pragma once

//주인공 이동
void MovePlayer(int* dx, int* dy);

//총알 이동
void MoveBullet(RECT rectView, double bullet[][4], int speed, int* combo, BOOL multi, RECT* regg, RECT* eegg);

//총알 나가는거 생성
void Hshotbullet(double bullet[][4], RECT* regg, HDC hMemDC, int i, int check, int* ecolor, int ch);

//플레이어가 특수총알 먹는거
void Hplayersgun(double sgun[][3], double dx, double dy, double bullet[][4], double cx, double cy, int* score);

//방향키 총알 발사
void Kshotbullet(double bullet[][4], double cx, double cy, int check);

//부활 이펙타임 
void Trespawneffect(double reffect[][4], int gametime, BOOL* death, double enemy[][5]);

//데스이펙타임
void Tdetaheffect(double effect[][17]);
