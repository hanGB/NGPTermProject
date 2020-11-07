#include "stdafx.h"
#include "IVSSCGlobals.h"
#include "IVSSCRender.h"
#include "IVSSCUpdate.h"

void MovePlayer(int* dx, int* dy)//주인공 이동
{

	if (GetAsyncKeyState('w') & 0x8000 || GetAsyncKeyState('W') & 0x8000)
	{
		*dy = -1;
	}

	if (GetAsyncKeyState('s') & 0x8000 || GetAsyncKeyState('S') & 0x8000)
	{
		*dy = 1;
	}

	if (GetAsyncKeyState('a') & 0x8000 || GetAsyncKeyState('A') & 0x8000)
	{
		*dx = -1;
	}

	if (GetAsyncKeyState('d') & 0x8000 || GetAsyncKeyState('D') & 0x8000)
	{
		*dx = 1;
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

void Hshotbullet(double bullet[][4], RECT* regg, HDC hMemDC, int i, int check, int* ecolor, int ch)
{//총알 나가는거 생성
	if (bullet[i][3] == 0 || bullet[i][3] == 1)
	{
		if (check == 0)
		{
			regg[i].left = bullet[i][1] - 50;
			regg[i].top = bullet[i][2] - 5;
			regg[i].right = bullet[i][1] + 50;
			regg[i].bottom = bullet[i][2] + 5;
			if (ch == 0)
			{
				if (bullet[i][3] == 0)
					DrawGr(hMemDC, RGB(250, 250, 250), RGB(0, 0, 0), regg[i], TRUE);
				else
					DrawGr(hMemDC, RGB(0, 0, 0), RGB(250, 250, 250), regg[i], TRUE);
			}
			else
			{
				if (bullet[i][3] == 0)
					DrawGr(hMemDC, RGB(20, 20, 20), RGB(255, 255, 255), regg[i], TRUE);
				else
					DrawGr(hMemDC, RGB(255, 255, 255), RGB(20, 20, 20), regg[i], TRUE);
			}
		}
		else
		{
			regg[3 * i].left = bullet[i][1] - 50;
			regg[3 * i].top = bullet[i][2] - 5;
			regg[3 * i].right = bullet[i][1] + 50;
			regg[3 * i].bottom = bullet[i][2] + 5;

			regg[3 * i + 1].left = bullet[i][1] - 50;
			regg[3 * i + 1].top = bullet[i][2] + 45;
			regg[3 * i + 1].right = bullet[i][1] + 50;
			regg[3 * i + 1].bottom = bullet[i][2] + 55;

			regg[3 * i + 2].left = bullet[i][1] - 50;
			regg[3 * i + 2].top = bullet[i][2] - 55;
			regg[3 * i + 2].right = bullet[i][1] + 50;
			regg[3 * i + 2].bottom = bullet[i][2] - 45;
			if (bullet[i][3] == 0)
			{
				DrawGr(hMemDC, RGB(255, 255, 255), RGB(ecolor[0], ecolor[1], ecolor[2]), regg[3 * i], TRUE);
				DrawGr(hMemDC, RGB(255, 255, 255), RGB(ecolor[0], ecolor[1], ecolor[2]), regg[3 * i + 1], TRUE);
				DrawGr(hMemDC, RGB(255, 255, 255), RGB(ecolor[0], ecolor[1], ecolor[2]), regg[3 * i + 2], TRUE);
			}
			else
			{
				DrawGr(hMemDC, RGB(ecolor[0], ecolor[1], ecolor[2]), RGB(255, 255, 255), regg[3 * i], TRUE);
				DrawGr(hMemDC, RGB(ecolor[0], ecolor[1], ecolor[2]), RGB(255, 255, 255), regg[3 * i + 1], TRUE);
				DrawGr(hMemDC, RGB(ecolor[0], ecolor[1], ecolor[2]), RGB(255, 255, 255), regg[3 * i + 2], TRUE);
			}
		}
	}
	else
	{
		if (check == 0)
		{
			regg[i].left = bullet[i][1] - 5;
			regg[i].top = bullet[i][2] - 50;
			regg[i].right = bullet[i][1] + 5;
			regg[i].bottom = bullet[i][2] + 50;
			if (ch == 0)
			{
				if (bullet[i][3] == 2)
					DrawGr(hMemDC, RGB(0, 0, 0), RGB(250, 250, 250), regg[i], FALSE);
				else
					DrawGr(hMemDC, RGB(250, 250, 250), RGB(0, 0, 0), regg[i], FALSE);
			}
			else
			{
				if (bullet[i][3] == 2)
					DrawGr(hMemDC, RGB(255, 255, 255), RGB(20, 20, 20), regg[i], FALSE);
				else
					DrawGr(hMemDC, RGB(20, 20, 20), RGB(255, 255, 255), regg[i], FALSE);
			}
		}
		else
		{
			regg[3 * i].left = bullet[i][1] - 5;
			regg[3 * i].top = bullet[i][2] - 50;
			regg[3 * i].right = bullet[i][1] + 5;
			regg[3 * i].bottom = bullet[i][2] + 50;

			regg[3 * i + 1].left = bullet[i][1] + 45;
			regg[3 * i + 1].top = bullet[i][2] - 50;
			regg[3 * i + 1].right = bullet[i][1] + 55;
			regg[3 * i + 1].bottom = bullet[i][2] + 50;

			regg[3 * i + 2].left = bullet[i][1] - 55;
			regg[3 * i + 2].top = bullet[i][2] - 50;
			regg[3 * i + 2].right = bullet[i][1] - 45;
			regg[3 * i + 2].bottom = bullet[i][2] + 50;
			if (bullet[i][3] == 2)
			{
				DrawGr(hMemDC, RGB(ecolor[0], ecolor[1], ecolor[2]), RGB(255, 255, 255), regg[3 * i], FALSE);
				DrawGr(hMemDC, RGB(ecolor[0], ecolor[1], ecolor[2]), RGB(255, 255, 255), regg[3 * i + 1], FALSE);
				DrawGr(hMemDC, RGB(ecolor[0], ecolor[1], ecolor[2]), RGB(255, 255, 255), regg[3 * i + 2], FALSE);
			}
			else
			{
				DrawGr(hMemDC, RGB(255, 255, 255), RGB(ecolor[0], ecolor[1], ecolor[2]), regg[3 * i], FALSE);
				DrawGr(hMemDC, RGB(255, 255, 255), RGB(ecolor[0], ecolor[1], ecolor[2]), regg[3 * i + 1], FALSE);
				DrawGr(hMemDC, RGB(255, 255, 255), RGB(ecolor[0], ecolor[1], ecolor[2]), regg[3 * i + 2], FALSE);
			}
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
