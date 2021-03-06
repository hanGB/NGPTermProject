#pragma once
#include "stdafx.h"

// 서버 클라이언트 통신 관련

//#define SERVERIP "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE 10000
#define MAX_CLNT 256

#define MAX_PLAYER 4 //최대 플레이어

#define BOARD_SIZE 20 //보드 크기

typedef struct Clinfo {
	int ci;
}Clinfo;

typedef struct KeyInputs
{
	bool ARROW_UP = false;
	bool ARROW_DOWN = false;
	bool ARROW_LEFT = false;
	bool ARROW_RIGHT = false;

	bool KEY_W = false;
	bool KEY_A = false;
	bool KEY_S = false;
	bool KEY_D = false;

	bool KEY_SPACEBAR = false;
};

typedef struct CData {//서버로 보내는 데이터
	int ci;
	KeyInputs p_key;
	float coolTime = 0; //총알 연속 발사 쿨타임
}CData;

#define MAX_LIFE 2

typedef struct player {
	int nu;
	BOOL enable = false;
	float rx[7], ry[7];
	float cx, cy;
	float bullet[6][4] = { 0, };
	float d_effect[17] = { 0, };
	float reffect[3] = {0, };
	BOOL death = false;
	int life = MAX_LIFE;
	float coolTime = 0; //총알 연속 발사 쿨타임
	float respawnTime = 0; //리스폰까지 시간
	BOOL gameready = false;
}player;


typedef struct GameObjects {//서버에서 받는 데이터
	player players;
	int blocks[BOARD_SIZE][BOARD_SIZE];
	int GameState = 1;
	float time = 0;
	bool gameEnd = false;
	int winPlayer = -1;
	float timeAfterGameEnd = 6;	
}GameObjects;


// 게임 로직 관련

#define LIMIT_ENEMY 20
#define LIMIT_SGUN 100

#define LIMIT_BGM 5
#define LIMIT_EFFECT 2

#define KEYUP(vk_code) ((GetAsyncKeyState(vk_code)) ? 0 : 1)

#define DRAW 100
