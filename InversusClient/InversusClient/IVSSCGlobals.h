#pragma once
#include "stdafx.h"

// ���� Ŭ���̾�Ʈ ��� ����

//#define SERVERIP "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE 3000
#define MAX_CLNT 256

typedef struct Clinfo {
	int ci;
}Clinfo;

typedef struct CData {//Ŭ���̾�Ʈ�κ��� ���� ������
	int ci;
	int dx = 0, dy = 0; //����
}CData;


// ���� ���� ����

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
