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

void Update(float elapsedTimeInSec)
{
	move_player_object(elapsedTimeInSec);
	check_lauched_bullet(elapsedTimeInSec);
	move_bullet_object(elapsedTimeInSec);
	CollisionBetweenBulletAndBlock();
}


void ColRect(RECT rec, RECT& rec2, double* cx, double* cy)//������ �������� ���ϰ�
{
	RECT temp;
	if (IntersectRect(&temp, &rec, &rec2))
	{
		if ((temp.bottom - temp.top) <= (temp.right - temp.left))
		{
			if (temp.top == rec2.top)
			{
				*cy -= temp.bottom - temp.top;
			}
			else if (temp.bottom == rec2.bottom)
			{
				*cy += temp.bottom - temp.top;
			}
		}
		else
		{
			if (temp.left == rec2.left)
			{
				*cx -= temp.right - temp.left;
			}
			else if (temp.right == rec2.right)
			{
				*cx += temp.right - temp.left;
			}
		}
	}
}

void move_player_object(float elapsedTimeInSec)
{
	for (int id = 0; id < MAX_PLAYER; ++id) {
		if (clnt_data[id].ci != NON_PLAYER) {
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

			int player_BB_xidex = int((parray[id].cx - sx / 2) / (rectView.right / BOARD_SIZE));
			int player_BB_yidex = int((parray[id].cy - sy / 2) / (rectView.bottom / 14));

			if (g_GameObjects.blocks[player_AA_yidex][player_AA_xidex] != id)
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
			else if (g_GameObjects.blocks[player_BB_yidex][player_BB_xidex] != id)
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

			if (rectView.top+100 > parray[id].cy - sy / 2)
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
/*
bullet[i][0] : i��° �Ѿ� ���� (0�� �Ѿ� ���� 1�� �Ѿ� ���� 2�� Ư���Ѿ� 3�� �Ѿ� �߻���) 
bullet[i][1] : i��° �Ѿ� x��ǥ
bullet[i][2] : i��° �Ѿ� y��ǥ
bullet[i][3] : i��° ����
*/
void check_lauched_bullet(float elapsedTimeInSec)
{
	for (int id = 0; id < MAX_PLAYER; ++id) {
		if (clnt_data[id].ci != NON_PLAYER) {
			if (clnt_data[id].p_key.ARROW_UP)
			{
				if (clnt_data[id].coolTime <= 0) {
					Kshotbullet(id, 4);
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

void Kshotbullet(int id, int d)//id: �÷��̾� ��ȣ, d: ����
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
				else if (parray[id].bullet[i][3] == 2)// ��
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
				else if (parray[id].bullet[i][3] == 3)// �Ʒ�
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

void ReloadBullet(int* reload, double bullet[][4], int time)//�Ѿ� ����
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
	//�Ѿ�-��� �浹
	for (int id = 0; id < MAX_PLAYER; ++id) {
		for (int i = 0; i < 6; i++)
		{
			if (parray[id].bullet[i][0] == 3) {
				int bulletX = parray[id].bullet[i][1];
				int bulletY = parray[id].bullet[i][2];

				for (int y = 0; y < 20; y++)
				{
					for (int x = 0; x < 20; x++)
					{
						RECT rec = { 50 * x, 50 * y, 50 * (x + 1), 50 * (y + 1) };
						RECT temp;
						RECT regg;

						regg.left = bulletX - 5;
						regg.top = bulletY - 5;
						regg.right = bulletX + 5;
						regg.bottom = bulletY + 5;

						if (IntersectRect(&temp, &regg, &rec)) {
							g_GameObjects.blocks[y][x] = id;
						}
					}
				}
			}
		}
	}
}

void Hcolplayer(double cx, double cy, double dx, double dy, RECT* regg, double effect[][17], int i, HDC hMemDC, RECT rectView, int check, char* str, HWND hWnd, BOOL* death)//�ϴ��� �Ѿ� ����
{
	RECT rec = { cx - dx / 2, cy - dy / 2, cx + dx / 2, cy + dy / 2 };
	RECT temp;
	if (IntersectRect(&temp, &regg[i], &rec))
	{
		DeathEffect(effect, cx, cy, dx, i);
		Hgamewin(hMemDC, rectView, check, str, hWnd);
		*death = true;
	}
}

void Hsgun(double sgun[][3], double seta, double dx, double dy, HDC hMemDC)//������ ���� Ư���Ѿ�
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

void Hplayersgun(double sgun[][3], double dx, double dy, double bullet[][4], double cx, double cy, int* score)//�÷��̾ Ư���Ѿ� �Դ°�
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
{//�Ѿ� ���ư��°�
	rx[i] = dx / 3 * cos(seta + i) + cx;
	ry[i] = dy / 3 * sin(seta + i) + cy;
}


void DeathEffect(double effect[][17], double x, double y, int dx, int i)//������ ����Ʈ ���
{
	//0-�ð� 1-x1 2-y1 3-x2 4-y2 5-x3 6-y3 7-x4 8-y4
	//9- 1���� 10-2���� 11-3���� 12-4���� 13~16+/-
	for (int j = 1; j < 8; j += 2)
		effect[i][j] = x + rand() % 50 * cos(rand() % 3);
	for (int j = 2; j < 9; j += 2)
		effect[i][j] = y + rand() % 50 * sin(rand() % 3);
	for (int j = 9; j < 13; j++)
		effect[i][j] = rand() % (dx / 2) + 10;
	for (int j = 13; j < 17; j++)
		effect[i][j] = rand() % 2;
	effect[i][0] = 5;
}

void RespawnEffect(double reffect[][4], double x, double y, int nu)//������ �Ҷ� ����Ʈ ���
{
	reffect[0][0] = 30;
	reffect[0][1] = x;
	reffect[0][2] = y;
	reffect[0][3] = nu;
}


void Tdetaheffect(double effect[][17])//��������Ÿ��
{
	for (int i = 0; i < LIMIT_ENEMY + 1; i++)
	{
		if (effect[i][0] > 0)
		{
			for (int j = 13; j < 17; j++)
			{
				if (effect[i][j] == 0)
				{
					effect[i][j - 4] -= 5;
				}
				else
				{
					effect[i][j - 4] += 5;
				}
			}
			effect[i][0]--;
		}
	}
}

void Trespawneffect(double reffect[][4], int gametime, BOOL* death, double enemy[][5])//����������Ÿ�� 
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
				if (reffect[i][3] == -1)//���ΰ�
				{
					*death = false;
				}
				else//��
				{
					enemy[(int)reffect[i][3]][0] = 1;
				}
				reffect[i][0] = -1;
			}
		}
	}
}
