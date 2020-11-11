#include "stdafx.h"
#include "IVSSSGlobals.h"
#include "IVSSSGame.h"
#include "IVSSSUpdate.h"

extern player parray[MAX_PLAYER];

extern CData clnt_data[MAX_CLNT];
extern GameObjects g_GameObjects;
extern RECT rectView;

extern double sx, sy;
extern double seta;

void ColRect(RECT rec, RECT& rec2, double* cx, double* cy)//검은벽 움직이지 못하게
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

			if (rectView.top > parray[id].cy - sy / 2)
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


void MoveBullet(RECT rectView, double bullet[][4], int speed, int* combo, BOOL multi, RECT* regg, RECT* eegg)//총알 이동
{
	for (int i = 0; i < 6; i++)
	{
		if (bullet[i][0] == 3 || bullet[i][0] == 4)
		{
			if (bullet[i][3] == 0) // ->
			{
				if (bullet[i][1] - 50 < rectView.right)
				{
					bullet[i][1] += speed;
				}
				else
				{
					bullet[i][0] = 0;
					*combo = 0;
				}
			}
			else if (bullet[i][3] == 1)// <-
			{
				if (bullet[i][1] + 50 > rectView.left)
				{
					bullet[i][1] -= speed;
				}
				else
				{
					bullet[i][0] = 0;
					*combo = 0;
				}
			}
			else if (bullet[i][3] == 2)// 위
			{
				if (bullet[i][2] - 50 > rectView.top)
				{
					bullet[i][2] -= speed;
				}
				else
				{
					bullet[i][0] = 0;
					*combo = 0;
				}
			}
			else if (bullet[i][3] == 3)// 아래
			{
				if (bullet[i][2] + 50 < rectView.bottom)
				{
					bullet[i][2] += speed;
				}
				else
				{
					bullet[i][0] = 0;
					*combo = 0;
				}
			}

			if (multi == true)
			{
				for (int j = 0; j < 6; j++)
				{
					if (bullet[j][0] == 3 || bullet[j][0] == 4)
					{
						RECT temp;
						if (IntersectRect(&temp, &eegg[j], &regg[i]))
						{
							bullet[i][0] = 0;
							bullet[j][0] = 0;
							*combo = 0;
						}
					}
				}
			}
		}
	}
}

void Hcolblock(double dx, double dy, RECT* regg, int block[][20], int* score, int* combo, double bullet[][4], int i, int check, int ch)
{
	//총알-블록 충돌
	for (int y = 0; y < 20; y++)
	{
		for (int x = 0; x < 20; x++)
		{
			RECT rec = { dx * x, dy * y, dx * (x + 1), dy * (y + 1) };
			RECT temp;
			if (check == 0)
			{
				if (IntersectRect(&temp, &regg[i], &rec))
				{
					if (ch == 0)
					{
						if (block[y][x] == 1)
						{
							block[y][x] = 0;
							*score += 5;
						}
						else if (block[y][x] == 2)
						{
							bullet[i][0] = 0;
							*combo = 0;
						}
					}
					else
					{
						if (block[y][x] == 0)
						{
							block[y][x] = 1;
							*score += 5;
						}
						else if (block[y][x] == 2)
						{
							bullet[i][0] = 0;
							*combo = 0;
						}
					}
				}
			}
			else
			{
				if (IntersectRect(&temp, &regg[i * 3], &rec) || IntersectRect(&temp, &regg[i * 3 + 1], &rec) || IntersectRect(&temp, &regg[i * 3 + 2], &rec))
				{
					if (block[y][x] == 1)
					{
						block[y][x] = 0;
						*score += 5;
					}
				}
			}
		}
	}
}

void Hcolplayer(double cx, double cy, double dx, double dy, RECT* regg, double effect[][17], int i, HDC hMemDC, RECT rectView, int check, char* str, HWND hWnd, BOOL* death)//일대일 총알 맞음
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

void Kshotbullet(double bullet[][4], double cx, double cy, int check)//방향키 총알 발사
{
	for (int i = 0; i < 6; i++)
	{
		if (bullet[i][0] == 1 || bullet[i][0] == 2)
		{
			if (bullet[i][0] == 1)
				bullet[i][0] = 3;
			else
				bullet[i][0] = 4;

			if (check == 0)
			{
				bullet[i][1] = cx + 50;
				bullet[i][2] = cy;
			}
			else if (check == 1)
			{
				bullet[i][1] = cx - 50;
				bullet[i][2] = cy;
			}
			else if (check == 2)
			{
				bullet[i][1] = cx;
				bullet[i][2] = cy - 50;
			}
			else if (check == 3)
			{
				bullet[i][1] = cx;
				bullet[i][2] = cy + 50;
			}
			bullet[i][3] = check;
			break;
		}
	}
}

void DeathEffect(double effect[][17], double x, double y, int dx, int i)//죽을때 이펙트 계산
{
	//0-시간 1-x1 2-y1 3-x2 4-y2 5-x3 6-y3 7-x4 8-y4
	//9- 1지름 10-2지름 11-3지름 12-4지름 13~16+/-
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

void RespawnEffect(double reffect[][4], double x, double y, int nu)//리스폰 할때 이펙트 계산
{
	reffect[0][0] = 30;
	reffect[0][1] = x;
	reffect[0][2] = y;
	reffect[0][3] = nu;
}


void Tdetaheffect(double effect[][17])//데스이펙타임
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
