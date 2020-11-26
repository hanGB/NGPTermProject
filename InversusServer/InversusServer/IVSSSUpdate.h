#pragma once

//서버 로그파일
void log_msg(char* msg);

//보드 초기화 함수
void initboard();

//플레이어 좌표 초기화 함수
void initplayerpos(int id);

//플레이어 초기화 함수
void initplayer(int id);

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

//총알-블록 충돌
void CollisionBetweenBulletAndBlock();

//방향키 총알 발사
void Kshotbullet(int id, int d);

//죽을때 이펙트 계산
void DeathEffect(int id);

//데스이펙타임
void Tdetaheffect();

//게임 종료 체크 및 승리 플레이어 선정
void GameEndCheck();
