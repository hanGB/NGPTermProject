#pragma once
#include "stdafx.h"

// 서버 클라이언트 통신 관련
#define SERVERPORT 9000
#define BUFSIZE 3000
#define MAX_CLNT 256

typedef struct Clinfo {
	int ci;
}Clinfo;

typedef struct CData {//클라이언트로부터 받은 데이터
	int ci;
	int dx, dy; //방향
}CData;


// 게임 로직 관련
#define PLAYER_SPEED 1

#define LIMIT_ENEMY 20
#define LIMIT_SGUN 100

#define LIMIT_BGM 5
#define LIMIT_EFFECT 2

typedef struct player {
	int nu;
	BOOL enable = false;
	double rx[7], ry[7];
	double cx, cy;
	double bullet[6][4] = { 0 };
	RECT regg[6];
}player;

