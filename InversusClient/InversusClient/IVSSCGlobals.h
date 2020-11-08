#pragma once
#include "stdafx.h"

// 서버 클라이언트 통신 관련

//#define SERVERIP "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE 3000
#define MAX_CLNT 256

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

};

typedef struct CData {//클라이언트로부터 받은 데이터
	int ci;
	int dx = 0, dy = 0; //방향
	KeyInputs p_key;
}CData;


// 게임 로직 관련

#define LIMIT_ENEMY 20
#define LIMIT_SGUN 100

#define LIMIT_BGM 5
#define LIMIT_EFFECT 2

#define KEYUP(vk_code) ((GetAsyncKeyState(vk_code)) ? 0 : 1)

typedef struct player {
	int nu;
	BOOL enable = false;
	double rx[7], ry[7];
	double cx, cy;
	double bullet[6][4] = { 0 };
	RECT regg[6];
}player;
