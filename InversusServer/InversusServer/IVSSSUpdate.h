#pragma once

//게임 오브젝트 업데이트 함수
void Update(float elapsedTimeInSec);

//플레이어 오브젝트 이동
void move_player_object(float elapsedTimeInSec);

//총알 오브젝트 이동
void move_bullet_object(float elapsedTimeInSec);

//총알 발사 체크 및 총알 발사 쿨타임 저장
void check_lauched_bullet(float elapsedTimeInSec);

//죽은 플레이어 리스폰 타임 계산 및 부활
void HandleDeathPlayer(float elapsedTimeInSec);

//검은벽 움직이지 못하게
void ColRect(RECT rec, RECT& rec2, double* cx, double* cy);

//총알 장전
void ReloadBullet(int* reload, double bullet[][4], int time);

//총알-블록 충돌
void CollisionBetweenBulletAndBlock();

//보드판 위에 특수총알
void Hsgun(double sgun[][3], double seta, double dx, double dy, HDC hMemDC);

//플레이어가 특수총알 먹는거
void Hplayersgun(double sgun[][3], double dx, double dy, double bullet[][4], double cx, double cy, int* score);

//총알 돌아가는거
void Hrotategun(double* rx, double* ry, double cx, double cy, double dx, double dy, double seta, int i, 
	BOOL death, double bullet[][4], HBRUSH oldBrush, HBRUSH hBrush, HBRUSH eBrush, HDC hMemDC);

//방향키 총알 발사
void Kshotbullet(int id, int d);

//죽을때 이펙트 계산
void DeathEffect(int id);

//데스이펙타임
void Tdetaheffect();

//리스폰 할때 이펙트 계산
void RespawnEffect(double reffect[][4], double x, double y, int nu);

//리스폰이펙타임 
void Trespawneffect(double reffect[][4], int gametime, BOOL* death, double enemy[][5]);
