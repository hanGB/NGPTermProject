#include "stdafx.h"
#include "IVSSSGlobals.h"
#include "IVSSSGame.h"
#include "IVSSSUpdate.h"
#include <iostream>

extern player parray[MAX_PLAYER];

extern CData clnt_data[MAX_CLNT];
extern GameObjects g_GameObjects;
extern RECT rectView;

extern double sx, sy;
extern double seta;

extern bool connect_index[4];

void log_msg(char* msg)
{
	time_t curTime = time(NULL);
	struct tm* pLocal;
	pLocal = localtime(&curTime);

	char timestr[100];
	sprintf(timestr, "%04d-%02d-%02dT%02d:%02d:%02d",
		pLocal->tm_year + 1900, pLocal->tm_mon + 1, pLocal->tm_mday,
		pLocal->tm_hour, pLocal->tm_min, pLocal->tm_sec);
	printf("[%s] %s\n", timestr, msg);

	FILE* fp;

	fp = fopen("logfile.log", "a");
	fprintf(fp, "[%s] %s\n", timestr, msg);
	fclose(fp);
}

void initboard()
{
	for (int y = 0; y < BOARD_SIZE; y++)
	{
		for (int x = 0; x < BOARD_SIZE; x++)
		{
			if (x < BOARD_SIZE / 2 && y < 8)
			{
				g_GameObjects.blocks[y][x] = 0;
			}

			if (x >= BOARD_SIZE / 2 && y < 8)
			{
				g_GameObjects.blocks[y][x] = 1;
			}

			if (x < BOARD_SIZE / 2 && y >= 8)
			{
				g_GameObjects.blocks[y][x] = 2;
			}

			if (x >= BOARD_SIZE / 2 && y >= 8)
			{
				g_GameObjects.blocks[y][x] = 3;
			}
		}
	}
}

void initplayerpos(int id)
{
	if (id == 0)
	{
		parray[id].cx = 100;
		parray[id].cy = 200;
	}
	else if (id == 1)
	{
		parray[id].cx = 900;
		parray[id].cy = 200;
	}
	else if (id == 2)
	{
		parray[id].cx = 100;
		parray[id].cy = 600;
	}
	else if (id == 3)
	{
		parray[id].cx = 900;
		parray[id].cy = 600;
	}
}

void initplayer(int id)
{
	initplayerpos(id);

	if (connect_index[id])
		parray[id].enable = true;
	else
		parray[id].enable = false;
	parray[id].gameready = false;
	parray[id].death = false;
	parray[id].life = MAX_LIFE;
	parray[id].d_effect[0] = 0;
	parray[id].reffect[0] = 0;
	parray[id].coolTime = 0;
	parray[id].respawnTime = 0;

	for(int i=0; i<6; i++)
		parray[id].bullet[i][0] = 0;

}

void Update(float elapsedTimeInSec)
{
	move_player_object(elapsedTimeInSec);
	check_lauched_bullet(elapsedTimeInSec);
	move_bullet_object(elapsedTimeInSec);
	HandleDeathPlayer(elapsedTimeInSec);
	CollisionBetweenBulletAndBlock();
	GameEndCheck();

	if (!g_GameObjects.gameEnd)
		g_GameObjects.time += elapsedTimeInSec;
	else {
		g_GameObjects.timeAfterGameEnd -= elapsedTimeInSec;
		
		if (g_GameObjects.timeAfterGameEnd <= 0) {
			for (int id = 0; id < MAX_PLAYER; id++)
			{
				initplayer(id);
			}
			initboard();

			g_GameObjects.time = 0;
			g_GameObjects.gameEnd = false;
			g_GameObjects.winPlayer = -1;
			g_GameObjects.timeAfterGameEnd = 6;

			g_GameObjects.GameState = 0;
		}
	}
}

void ColRect(RECT rec, RECT& rec2,int id)//검은벽 움직이지 못하게
{
	RECT temp;
	if (IntersectRect(&temp, &rec, &rec2))
	{
		if ((temp.bottom - temp.top) <= (temp.right - temp.left))
		{
			if (temp.top == rec2.top)
			{
				parray[id].cy -= temp.bottom - temp.top;
			}
			else if (temp.bottom == rec2.bottom)
			{
				parray[id].cy += temp.bottom - temp.top;
			}
		}
		else
		{
			if (temp.left == rec2.left)
			{
				parray[id].cx -= temp.right - temp.left;
			}
			else if (temp.right == rec2.right)
			{
				parray[id].cx += temp.right - temp.left;
			}
		}
	}
}

void back_move_player(int id, float elapsedTimeInSec)
{
	if (clnt_data[id].p_key.KEY_W)
	{
		parray[id].cy += double(PLAYER_SPEED) * elapsedTimeInSec;
	}

	if (clnt_data[id].p_key.KEY_A)
	{
		parray[id].cx += double(PLAYER_SPEED) * elapsedTimeInSec;
	}

	if (clnt_data[id].p_key.KEY_S)
	{
		parray[id].cy -= double(PLAYER_SPEED) * elapsedTimeInSec;
	}

	if (clnt_data[id].p_key.KEY_D)
	{
		parray[id].cx -= double(PLAYER_SPEED) * elapsedTimeInSec;
	}
}

void move_player_object(float elapsedTimeInSec)
{
	for (int id = 0; id < MAX_PLAYER; ++id) 
	{
		if (clnt_data[id].ci != NON_PLAYER) 
		{
			if (parray[id].enable) 
			{
				if (clnt_data[id].p_key.KEY_W)
				{
					parray[id].cy -= double(PLAYER_SPEED) * elapsedTimeInSec;
				}

				if (clnt_data[id].p_key.KEY_A)
				{
					parray[id].cx -= double(PLAYER_SPEED) * elapsedTimeInSec;
				}

				if (clnt_data[id].p_key.KEY_S)
				{
					parray[id].cy += double(PLAYER_SPEED) * elapsedTimeInSec;
				}

				if (clnt_data[id].p_key.KEY_D)
				{
					parray[id].cx += double(PLAYER_SPEED) * elapsedTimeInSec;
				}
				
				int player_AA_xidex = int((parray[id].cx + sx / 2) / (rectView.right / BOARD_SIZE));
				int player_AA_yidex = int((parray[id].cy + sy / 2) / (rectView.bottom / 14));

				int player_AB_xidex = int((parray[id].cx + sx / 2) / (rectView.right / BOARD_SIZE));
				int player_AB_yidex = int((parray[id].cy - sy / 2) / (rectView.bottom / 14));

				int player_BB_xidex = int((parray[id].cx - sx / 2) / (rectView.right / BOARD_SIZE));
				int player_BB_yidex = int((parray[id].cy - sy / 2) / (rectView.bottom / 14));

				int player_BA_xidex = int((parray[id].cx - sx / 2) / (rectView.right / BOARD_SIZE));
				int player_BA_yidex = int((parray[id].cy + sy / 2) / (rectView.bottom / 14));

				if (g_GameObjects.blocks[player_AA_yidex][player_AA_xidex] != id)
				{
					back_move_player(id, elapsedTimeInSec);
				}

				if (g_GameObjects.blocks[player_AB_yidex][player_AB_xidex] != id)
				{
					back_move_player(id, elapsedTimeInSec);
				}

				if (g_GameObjects.blocks[player_BA_yidex][player_BA_xidex] != id)
				{
					back_move_player(id, elapsedTimeInSec);
				}

				if (g_GameObjects.blocks[player_BB_yidex][player_BB_xidex] != id)
				{
					back_move_player(id, elapsedTimeInSec);
				}
				

				if (rectView.top + 100 > parray[id].cy - sy / 2)
					parray[id].cy += double(PLAYER_SPEED) * elapsedTimeInSec;

				if (rectView.bottom < parray[id].cy + sy / 2)
					parray[id].cy -= double(PLAYER_SPEED) * elapsedTimeInSec;

				if (rectView.left > parray[id].cx - sx / 2)
					parray[id].cx += double(PLAYER_SPEED) * elapsedTimeInSec;

				if (rectView.right < parray[id].cx + sx / 2)
					parray[id].cx -= double(PLAYER_SPEED) * elapsedTimeInSec;

				for (int i = 0; i < 6; i++)
				{
					parray[id].rx[i] = sx / 4 * cos(seta + i) + parray[id].cx;
					parray[id].ry[i] = sy / 4 * sin(seta + i) + parray[id].cy;
				}
			}
		}
	}
}
/*
bullet[i][0] : i번째 총알 상태 (0은 총알 없음 1은 총알 있음 2은 특수총알 3은 총알 발사중) 
bullet[i][1] : i번째 총알 x좌표
bullet[i][2] : i번째 총알 y좌표
bullet[i][3] : i번째 방향
*/
void check_lauched_bullet(float elapsedTimeInSec)
{
	for (int id = 0; id < MAX_PLAYER; ++id) 
	{
		if (clnt_data[id].ci != NON_PLAYER) 
		{
			if (parray[id].enable) 
			{
				if (clnt_data[id].p_key.ARROW_UP)
				{
					if (clnt_data[id].coolTime <= 0) {
						Kshotbullet(id, 2);
						clnt_data[id].coolTime = BULLET_COOL_TIME;
					}
					else
						clnt_data[id].coolTime -= elapsedTimeInSec;
				}

				if (clnt_data[id].p_key.ARROW_DOWN)
				{
					if (clnt_data[id].coolTime <= 0) {
						Kshotbullet(id, 3);
						clnt_data[id].coolTime = BULLET_COOL_TIME;
					}
					else
						clnt_data[id].coolTime -= elapsedTimeInSec;
				}

				if (clnt_data[id].p_key.ARROW_LEFT)
				{
					if (clnt_data[id].coolTime <= 0) {
						Kshotbullet(id, 1);
						clnt_data[id].coolTime = BULLET_COOL_TIME;
					}
					else
						clnt_data[id].coolTime -= elapsedTimeInSec;
				}

				if (clnt_data[id].p_key.ARROW_RIGHT)
				{
					if (clnt_data[id].coolTime <= 0) {
						Kshotbullet(id, 0);
						clnt_data[id].coolTime = BULLET_COOL_TIME;
					}
					else
						clnt_data[id].coolTime -= elapsedTimeInSec;
				}
				parray[id].coolTime = clnt_data[id].coolTime;
			}
		}
	}
}

void HandleDeathPlayer(float elapsedTimeInSec)
{
	for (int i = 0; i < MAX_PLAYER; ++i) 
	{
		if (parray[i].death) 
		{
			if (parray[i].life >= 0) 
			{
				parray[i].respawnTime -= elapsedTimeInSec;
				parray[i].reffect[0] -= elapsedTimeInSec;
				if (parray[i].respawnTime <= 0)
				{
					parray[i].death = false;
					parray[i].enable = true;
					parray[i].reffect[0] = 0;

					char logstr[100];
					sprintf(logstr, "[Spawn]Player%d님이 리스폰했습니다.\n", i);
					log_msg(logstr);

					RECT SpawnBlocks;

					if (i == 0) {
						g_GameObjects.blocks[3][1] = i;
						g_GameObjects.blocks[3][2] = i;
						g_GameObjects.blocks[4][1] = i;
						g_GameObjects.blocks[4][2] = i;
						SpawnBlocks = {BLOCK_SIZE * 1, BLOCK_SIZE * 3, BLOCK_SIZE * 3, BLOCK_SIZE * 5};
					}
					else if (i == 1) {
						g_GameObjects.blocks[3][17] = i;
						g_GameObjects.blocks[3][18] = i;
						g_GameObjects.blocks[4][17] = i;
						g_GameObjects.blocks[4][18] = i;
						SpawnBlocks = { BLOCK_SIZE * 17, BLOCK_SIZE * 3, BLOCK_SIZE * 19, BLOCK_SIZE * 5 };
					}
					else if (i == 2) {
						g_GameObjects.blocks[11][1] = i;
						g_GameObjects.blocks[11][2] = i;
						g_GameObjects.blocks[12][1] = i;
						g_GameObjects.blocks[12][2] = i;
						SpawnBlocks = { BLOCK_SIZE * 1, BLOCK_SIZE * 11, BLOCK_SIZE * 3, BLOCK_SIZE * 13 };
					}
					else if (i == 3) {
						g_GameObjects.blocks[11][17] = i;
						g_GameObjects.blocks[11][18] = i;
						g_GameObjects.blocks[12][17] = i;
						g_GameObjects.blocks[12][18] = i;
						SpawnBlocks = { BLOCK_SIZE * 17, BLOCK_SIZE * 11, BLOCK_SIZE * 19, BLOCK_SIZE * 13 };
					}

					for (int deathid = 0; deathid < MAX_PLAYER; ++deathid) {
						if (parray[deathid].enable == true && deathid != i)
						{
							RECT rec = { parray[deathid].cx - sx / 2, parray[deathid].cy - sy / 2, parray[deathid].cx + sx / 2, parray[deathid].cy + sy / 2 };
							RECT temp;

							if (IntersectRect(&temp, &SpawnBlocks, &rec)) {
								parray[deathid].enable = false;
								parray[deathid].death = true;
								parray[deathid].life--;
								parray[deathid].respawnTime = RESPAWN_TIME;
								parray[deathid].reffect[0] = RESPAWN_EFFECT_TIME;
								DeathEffect(deathid);

								sprintf(logstr, "[Kill]Player%d님이 Player%d님(life: %d)을 폭발로 살해했습니다.\n", i, deathid, parray[deathid].life);
								log_msg(logstr);

								if (deathid == 0) {
									parray[deathid].cx = parray[deathid].reffect[1] = 100;
									parray[deathid].cy = parray[deathid].reffect[2] = 200;
								}
								else if (deathid == 1) {
									parray[deathid].cx = parray[deathid].reffect[1] = 900;
									parray[deathid].cy = parray[deathid].reffect[2] = 200;
								}
								else if (deathid == 2) {
									parray[deathid].cx = parray[deathid].reffect[1] = 100;
									parray[deathid].cy = parray[deathid].reffect[2] = 600;
								}
								else if (deathid == 3) {
									parray[deathid].cx = parray[deathid].reffect[1] = 900;
									parray[deathid].cy = parray[deathid].reffect[2] = 600;
								}
							}
						}
					}
				}
			}
		}
	}
}

void Kshotbullet(int id, int d)//id: 플레이어 번호, d: 방향
{
	for (int i = 0; i < 6; i++)
	{
		if (parray[id].bullet[i][0] == 1 || parray[id].bullet[i][0] == 2)
		{
			if (parray[id].bullet[i][0] == 1)
				parray[id].bullet[i][0] = 3;
			else
				parray[id].bullet[i][0] = 4;

			if (d == 0)
			{
				parray[id].bullet[i][1] = parray[id].cx + 50;
				parray[id].bullet[i][2] = parray[id].cy;
			}
			else if (d == 1)
			{
				parray[id].bullet[i][1] = parray[id].cx - 50;
				parray[id].bullet[i][2] = parray[id].cy;
			}
			else if (d == 2)
			{
				parray[id].bullet[i][1] = parray[id].cx;
				parray[id].bullet[i][2] = parray[id].cy - 50;
			}
			else if (d == 3)
			{
				parray[id].bullet[i][1] = parray[id].cx;
				parray[id].bullet[i][2] = parray[id].cy + 50;
			}
			parray[id].bullet[i][3] = d;
			break;
		}
	}
}

void move_bullet_object(float elapsedTimeInSec)
{
	for (int id = 0; id < MAX_PLAYER; ++id) {
		for (int i = 0; i < 6; i++)
		{
			if (parray[id].bullet[i][0] == 3 || parray[id].bullet[i][0] == 4)
			{
				if (parray[id].bullet[i][3] == 0) // ->
				{
					if (parray[id].bullet[i][1] - 50 < rectView.right)
					{
						parray[id].bullet[i][1] += double(BULLET_SPEED) * elapsedTimeInSec;
					}
					else
					{
						parray[id].bullet[i][0] = 0;
					}
				}
				else if (parray[id].bullet[i][3] == 1)// <-
				{
					if (parray[id].bullet[i][1] + 50 > rectView.left)
					{
						parray[id].bullet[i][1] -= double(BULLET_SPEED) * elapsedTimeInSec;
					}
					else
					{
						parray[id].bullet[i][0] = 0;
					}
				}
				else if (parray[id].bullet[i][3] == 2)// 위
				{
					if (parray[id].bullet[i][2] - 50 > rectView.top)
					{
						parray[id].bullet[i][2] -= double(BULLET_SPEED) * elapsedTimeInSec;
					}
					else
					{
						parray[id].bullet[i][0] = 0;
					}
				}
				else if (parray[id].bullet[i][3] == 3)// 아래
				{
					if (parray[id].bullet[i][2] + 50 < rectView.bottom)
					{
						parray[id].bullet[i][2] += double(BULLET_SPEED) * elapsedTimeInSec;
					}
					else
					{
						parray[id].bullet[i][0] = 0;
					}
				}
			}
		}
	}
}

void ReloadBullet(int* reload, double bullet[][4], int time)//총알 장전
{
	if (*reload < time)
	{
		(*reload)++;
	}
	else
	{
		for (int i = 0; i < 6; i++)
		{
			if (bullet[i][0] == 0)
			{
				bullet[i][0] = 1;
				*reload = 0;
				return;
			}
		}
	}
}

void CollisionBetweenBulletAndBlock()
{
	for (int id = 0; id < MAX_PLAYER; ++id) {
		for (int i = 0; i < 6; i++)
		{
			if (parray[id].bullet[i][0] == 3) {
				int bulletX = parray[id].bullet[i][1];
				int bulletY = parray[id].bullet[i][2];

				RECT regg;
				regg.left = bulletX - 5;
				regg.top = bulletY - 5;
				regg.right = bulletX + 5;
				regg.bottom = bulletY + 5;

				//총알-블록 충돌
				for (int y = 0; y < 20; y++)
				{
					for (int x = 0; x < 20; x++)
					{
						RECT rec = { 50 * x, 50 * y, 50 * (x + 1), 50 * (y + 1) };
						RECT temp;

						if (IntersectRect(&temp, &regg, &rec)) {
							g_GameObjects.blocks[y][x] = id;
						}
					}
				}

				//총알-플레이어 충돌
				for (int deathid = 0; deathid < MAX_PLAYER; ++deathid)
				{
					if (parray[deathid].enable == true && deathid != id)
					{
						RECT rec = { parray[deathid].cx - sx/2, parray[deathid].cy - sy / 2, parray[deathid].cx + sx / 2, parray[deathid].cy + sy / 2 };
						RECT temp;

						if (IntersectRect(&temp, &regg, &rec)) {
							parray[deathid].enable = false;
							parray[deathid].death = true;
							parray[deathid].life--;
							parray[deathid].respawnTime = RESPAWN_TIME;
							DeathEffect(deathid);

							char logstr[100];
							sprintf(logstr, "[Kill]Player%d님이 Player%d님(life: %d)을 총으로 살해했습니다.\n", i, deathid, parray[deathid].life);
							log_msg(logstr);

							if (deathid == 0) {
								parray[deathid].cx = parray[deathid].reffect[1] = 100;
								parray[deathid].cy = parray[deathid].reffect[2] = 200;
							}
							else if (deathid == 1) {
								parray[deathid].cx = parray[deathid].reffect[1] = 900;
								parray[deathid].cy = parray[deathid].reffect[2] = 200;
							}
							else if (deathid == 2) {
								parray[deathid].cx = parray[deathid].reffect[1] = 100;
								parray[deathid].cy = parray[deathid].reffect[2] = 600;
							}
							else if (deathid == 3) {
								parray[deathid].cx = parray[deathid].reffect[1] = 900;
								parray[deathid].cy = parray[deathid].reffect[2] = 600;
							}

							if (parray[deathid].life >= 0)
								parray[deathid].reffect[0] = RESPAWN_TIME;
						}
					}
				}
			}
		}
	}
}

void DeathEffect(int id)//죽을때 이펙트 계산
{
	//0-시간 1-x1 2-y1 3-x2 4-y2 5-x3 6-y3 7-x4 8-y4
	//9- 1지름 10-2지름 11-3지름 12-4지름 13~16+/-
	for (int j = 1; j < 8; j += 2)
		parray[id].d_effect[j] = parray[id].cx + rand() % 50 * cos(rand() % 3);
	for (int j = 2; j < 9; j += 2)
		parray[id].d_effect[j] = parray[id].cy + rand() % 50 * sin(rand() % 3);
	for (int j = 9; j < 13; j++)
		parray[id].d_effect[j] = rand() % (int)(sx / 2) + 10;
	for (int j = 13; j < 17; j++)
		parray[id].d_effect[j] = rand() % 2;
	parray[id].d_effect[0] = 5;
}

void Tdetaheffect()//데스이펙타임
{
	for (int id = 0; id < MAX_PLAYER + 1; id++)
	{
		if (parray[id].d_effect[0] > 0)
		{
			for (int j = 13; j < 17; j++)
			{
				if (parray[id].d_effect[j] == 0)
				{
					parray[id].d_effect[j - 4] -= 5;
				}
				else
				{
					parray[id].d_effect[j - 4] += 5;
				}
			}
			parray[id].d_effect[0]--;
		}
	}
}

void Hsgun(double sgun[][3], double seta, double dx, double dy, HDC hMemDC)//보드판 위에 특수총알
{
	for (int i = 0; i < LIMIT_SGUN; i++)
	{
		if (sgun[i][0] == 1)
		{
			int gx, gy;
			gx = dx / 3 * cos(seta + i) + sgun[i][1];
			gy = dy / 3 * sin(seta + i) + sgun[i][2];
			Ellipse(hMemDC, gx - dx / 10, gy - dx / 10, gx + dx / 10, gy + dx / 10);
		}
	}
}

void Hplayersgun(double sgun[][3], double dx, double dy, double bullet[][4], double cx, double cy, int* score)//플레이어가 특수총알 먹는거
{
	RECT cec = { cx - dx / 2, cy - dy / 2, cx + dx / 2, cy + dy / 2 };
	for (int i = 0; i < LIMIT_SGUN; i++)
	{
		if (sgun[i][0] == 1)
		{
			RECT sec = { sgun[i][1] - dx / 2 , sgun[i][2] - dy / 2, sgun[i][1] + dx / 2 , sgun[i][2] + dy / 2 };
			RECT temp;
			if (IntersectRect(&temp, &sec, &cec))
			{
				for (int i = 0; i < 6; i++)
				{
					if (bullet[i][0] == 0)
					{
						bullet[i][0] = 2;
						break;
					}

					if (i == 5)
					{
						if (bullet[i][0] == 0)
						{
							bullet[i][0] = 2;
							break;
						}
						else
						{
							for (int i = 0; i < 6; i++)
							{
								if (bullet[i][0] == 1)
								{
									bullet[i][0] = 2;
									break;
								}
							}
						}
					}
				}
				sgun[i][0] = 0;
				*score += 5;
			}
		}
	}
}

void Hrotategun(double* rx, double* ry, double cx, double cy, double dx, double dy, double seta, int i, BOOL death, double bullet[][4], HBRUSH oldBrush, HBRUSH hBrush, HBRUSH eBrush, HDC hMemDC)
{//총알 돌아가는거
	rx[i] = dx / 3 * cos(seta + i) + cx;
	ry[i] = dy / 3 * sin(seta + i) + cy;
}

void RespawnEffect(double reffect[][4], double x, double y, int nu)//리스폰 할때 이펙트 계산
{
	reffect[0][0] = 30;
	reffect[0][1] = x;
	reffect[0][2] = y;
	reffect[0][3] = nu;
}

void Trespawneffect(double reffect[][4], int gametime, BOOL* death, double enemy[][5])//리스폰이펙타임 
{
	for (int i = 0; i < LIMIT_ENEMY; i++)
	{
		if (reffect[i][0] > 0)
		{
			if (gametime >= 60)
				reffect[i][0] -= 5;
			else
				reffect[i][0] -= 3;
			if (reffect[i][0] == 0)
			{
				if (reffect[i][3] == -1)//주인공
				{
					*death = false;
				}
				else//적
				{
					enemy[(int)reffect[i][3]][0] = 1;
				}
				reffect[i][0] = -1;
			}
		}
	}
}

void GameEndCheck()
{
	int check[] = {-2, -2, -2, -2}; // -2: 없음, -1: 죽음, 0이상: 남은 목숨 수

	int enablePlayerNum = 0;
	int maxLife = -1;
	int winPlayerNum = 0;

	int maxBlockNum = -1;

	char logstr[100];

	if (!g_GameObjects.gameEnd) {
		if (g_GameObjects.time > 180) {
			g_GameObjects.gameEnd = true;
			for (int i = 0; i < MAX_PLAYER; ++i) {
				if (parray[i].enable || parray[i].death) {
					if (parray[i].life > maxLife)
						maxLife = parray[i].life;
				}
			}
			for (int i = 0; i < MAX_PLAYER; ++i) {
				if (parray[i].enable || parray[i].death) {
					if (parray[i].life == maxLife) {
						g_GameObjects.winPlayer = i;
						g_GameObjects.timeAfterGameEnd = DLAY_TIME_BACK_TO_LOBBY;
						winPlayerNum++;
					}
				}
				if (winPlayerNum >= 2) {
					if (parray[i].enable || parray[i].death) {
						if (parray[i].life == maxLife) {
							int blockNum = 0;

							for (int x = 0; x < BOARD_SIZE; ++i) {
								for (int y = 0; y < BOARD_SIZE; ++i) {
									if (g_GameObjects.blocks[y][x] == i)
										blockNum++;
								}
							}

							if (blockNum > maxBlockNum) {
								maxBlockNum = blockNum;
								g_GameObjects.winPlayer = i;
								g_GameObjects.timeAfterGameEnd = DLAY_TIME_BACK_TO_LOBBY;
							}
							else if (blockNum == maxBlockNum) {
								g_GameObjects.winPlayer = DRAW;
								g_GameObjects.timeAfterGameEnd = DLAY_TIME_BACK_TO_LOBBY;
							}
						}
					}
				}
			}

		}
		else {
			for (int i = 0; i < MAX_PLAYER; ++i) {

				if (parray[i].enable || parray[i].death) {
					if (parray[i].life >= 0) {
						enablePlayerNum++;
					}
				}
			}
			if (enablePlayerNum == 1) {
				for (int i = 0; i < MAX_PLAYER; ++i) {
					if (parray[i].enable) {
						g_GameObjects.gameEnd = true;
						g_GameObjects.winPlayer = i;
						g_GameObjects.timeAfterGameEnd = DLAY_TIME_BACK_TO_LOBBY;
						sprintf(logstr, "[Win]Player%d님이 승리하셨습니다!!(최후 1인)\n", g_GameObjects.winPlayer);
						log_msg(logstr);
						break;
					}
				}
			}
		}
	}
}
