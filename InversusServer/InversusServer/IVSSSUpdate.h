#pragma once

//���� �α�����
void log_msg(char* msg);

//���� �ʱ�ȭ �Լ�
void initboard();

//�÷��̾� ��ǥ �ʱ�ȭ �Լ�
void initplayerpos(int id);

//�÷��̾� �ʱ�ȭ �Լ�
void initplayer(int id);

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

//�Ѿ�-��� �浹
void CollisionBetweenBulletAndBlock();

//����Ű �Ѿ� �߻�
void Kshotbullet(int id, int d);

//������ ����Ʈ ���
void DeathEffect(int id);

//��������Ÿ��
void Tdetaheffect();

//���� ���� üũ �� �¸� �÷��̾� ����
void GameEndCheck();
