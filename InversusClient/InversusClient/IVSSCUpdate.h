#pragma once

//���ΰ� �̵�
void MovePlayer(int* dx, int* dy);

//�Ѿ� �̵�
void MoveBullet(RECT rectView, double bullet[][4], int speed, int* combo, BOOL multi, RECT* regg, RECT* eegg);

//�Ѿ� �����°� ����
void Hshotbullet(double bullet[][4], RECT* regg, HDC hMemDC, int i, int check, int* ecolor, int ch);

//�÷��̾ Ư���Ѿ� �Դ°�
void Hplayersgun(double sgun[][3], double dx, double dy, double bullet[][4], double cx, double cy, int* score);

//����Ű �Ѿ� �߻�
void Kshotbullet(double bullet[][4], double cx, double cy, int check);

//��Ȱ ����Ÿ�� 
void Trespawneffect(double reffect[][4], int gametime, BOOL* death, double enemy[][5]);

//��������Ÿ��
void Tdetaheffect(double effect[][17]);
