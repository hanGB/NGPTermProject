#pragma once
#include "stdafx.h"

// ���� Ŭ���̾�Ʈ ��� ����
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

typedef struct CData {//Ŭ���̾�Ʈ�κ��� ���� ������
	int ci;
	int dx, dy; //����
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
	double reffect[3] = { 0, };
	BOOL death = false;
	int life = MAX_LIFE;
	float coolTime = 0; //�Ѿ� ���� �߻� ��Ÿ��
	float respawnTime = 0; //���������� �ð�
	BOOL gameready = false;
}player;

typedef struct GameObjects {//Ŭ���̾�Ʈ�� ���� ������
	player players[MAX_PLAYER];
	int blocks[BOARD_SIZE][BOARD_SIZE];
	int GameState = 1; //0�� �κ� 1�� ����
}GameObjects;


// ���� ���� ����
#define PLAYER_SPEED 100
#define BULLET_SPEED 500

#define LIMIT_ENEMY 20
#define LIMIT_SGUN 100

#define LIMIT_BGM 5
#define LIMIT_EFFECT 2

#define NON_PLAYER -1

#define BULLET_COOL_TIME 0.3

#define BLOCK_SIZE 50

#define RESPAWN_TIME 3
