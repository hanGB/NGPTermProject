#pragma once
#include "stdafx.h"

// ���� Ŭ���̾�Ʈ ��� ����

//#define SERVERIP "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE 10000
#define MAX_CLNT 256

#define MAX_PLAYER 5 //�ִ� �÷��̾�

#define BOARD_SIZE 20 //���� ũ��

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

typedef struct CData {//������ ������ ������
	int ci;
	int dx = 0, dy = 0; //����
	KeyInputs p_key;
	float coolTime = 0; //�Ѿ� ���� �߻� ��Ÿ��
}CData;

#define MAX_LIFE 2

typedef struct player {
	int nu;
	BOOL enable = false;
	double rx[7], ry[7];
	double cx, cy;
	double bullet[6][4] = { 0, };
	RECT regg[6];
	double d_effect[17] = { 0, };
	double reffect[3] = {0, };
	BOOL death = false;
	int life = MAX_LIFE;
	float coolTime = 0; //�Ѿ� ���� �߻� ��Ÿ��
	float respawnTime = 0; //���������� �ð�
	BOOL gameready = false;
}player;


typedef struct GameObjects {//�������� �޴� ������
	player players[MAX_PLAYER];
	int blocks[BOARD_SIZE][BOARD_SIZE];
	int GameState = 1;
}GameObjects;


// ���� ���� ����

#define LIMIT_ENEMY 20
#define LIMIT_SGUN 100

#define LIMIT_BGM 5
#define LIMIT_EFFECT 2

#define KEYUP(vk_code) ((GetAsyncKeyState(vk_code)) ? 0 : 1)
