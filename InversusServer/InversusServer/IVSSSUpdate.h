#pragma once

//���� ������Ʈ ������Ʈ �Լ�
void Update(float elapsedTimeInSec);

//�÷��̾� ������Ʈ �̵�
void move_player_object(float elapsedTimeInSec);

//�Ѿ� ������Ʈ �̵�
void move_bullet_object(float elapsedTimeInSec);

//�Ѿ� �߻� üũ �� �Ѿ� �߻� ��Ÿ�� ����
void check_lauched_bullet(float elapsedTimeInSec);

//���� �÷��̾� ������ Ÿ�� ��� �� ��Ȱ
void HandleDeathPlayer(float elapsedTimeInSec);

//������ �������� ���ϰ�
void ColRect(RECT rec, RECT& rec2, double* cx, double* cy);

//�Ѿ� ����
void ReloadBullet(int* reload, double bullet[][4], int time);

//�Ѿ�-��� �浹
void CollisionBetweenBulletAndBlock();

//������ ���� Ư���Ѿ�
void Hsgun(double sgun[][3], double seta, double dx, double dy, HDC hMemDC);

//�÷��̾ Ư���Ѿ� �Դ°�
void Hplayersgun(double sgun[][3], double dx, double dy, double bullet[][4], double cx, double cy, int* score);

//�Ѿ� ���ư��°�
void Hrotategun(double* rx, double* ry, double cx, double cy, double dx, double dy, double seta, int i, 
	BOOL death, double bullet[][4], HBRUSH oldBrush, HBRUSH hBrush, HBRUSH eBrush, HDC hMemDC);

//����Ű �Ѿ� �߻�
void Kshotbullet(int id, int d);

//������ ����Ʈ ���
void DeathEffect(int id);

//��������Ÿ��
void Tdetaheffect();

//������ �Ҷ� ����Ʈ ���
void RespawnEffect(double reffect[][4], double x, double y, int nu);

//����������Ÿ�� 
void Trespawneffect(double reffect[][4], int gametime, BOOL* death, double enemy[][5]);
