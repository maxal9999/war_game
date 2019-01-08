#pragma once

/**
 * \file
 * \brief ������ ��� �������� ��������� ������ � ����
 * \author ������������ �.�.
 */

#include <memory>
#include <list>

#include "ObjectsForShot.h"

// ����������� �������� ��� ������� �����-�����
const int N_DIM = 4;

// ���� �� ����
enum class Damage
{
	SMALL = 15,
	MIDDLE = 30,
	LARGE = 45
};

// ���� �������������. ������� �� ������� ����� �� ��������
enum class AngleCorrect
{
	SMALL = 30,
	MIDDLE = 45,
	LARGE = 60
};

// ���������, ����������� ������ ������
struct Aim
{
	Render::Texture* mTexture;
	FPoint mPoint;

	void Draw();
};

using shared_tex = Render::Texture*;

// ������� ��������� �������� ����
struct Bullet
{
	Bullet();

    // ������ ���� �������� ���� � ��������� ���������
	void CalcAngles(float rotate_angle);

	// ����� ��� ������� ��������� ����
	void SimpleDraw();

    // ��������� ����.
    // ��������� ������ �� ������ ������� ��������.
    // ���������� ��� �������� ��������� ���� �� �������
	void Draw(ObjectsPool& shot_objects);

	// ���������� ��� �������
	void DrawEffects(EffectsContainer& eff_cont);

	// ���������� ��������� ��������
	void DrawBulletIndicator();

	// ��������
	shared_tex mTexture;

    // ���� �����������������.
    // ��������� ��� ������������� ���� ������� ���� � ���������.
	bool mInvert;

    // ���� ���������� �� ���������������� ����.
    // ���� ����� ������ �� ������, �� ��� ������������
	bool mIsUsed;

    // ������� ��������� ����
	FPoint mCurrentPoint;
	FPoint mTargetPoint;

    // ���� �������, ��� ������� ��������� ������� �����
	float mSystemAngle;

	// ������ ��� ������ ����
	ParticleEffectPtr mFlyEffect;

	// ������ ��� ������ ����
	ParticleEffectPtr mHitEffect;

	// ����, ������������ ������ �������� ����
	bool mFirstDraw;
protected:
	// �������� ���������� ����
	shared_tex mOneBulletTexture;

	// ������������� ���������
	float mDeltaX;
	float mDeltaY;

    // �������� ����
    int mVelocity;

    // ��������������� ������ ����
	int mSize;

    // ���� �� ����
	Damage mDamage;

    // �������� ����������
	float mCm;

	// �������� ������
	float mKm;
private:
    // ������ ��� �������� ������ � ���������� ��������� ����
	float mXYold[N_DIM];

	// ������� ��� ������� ������������� ������� �����-�����
	float* RKFunc(float* xy_old);

	// ����� �������� ����
	void Move();
};

// ��� ����, ������������ � ����������
struct PistolBullet : public Bullet
{
	PistolBullet();
};

// ����� �������� ������
class MachineGun
{
public:
	MachineGun();

	// ������������� ���� � ��������
	void InitBullets(bool restart = false, bool recharge = false);

    // ��������� ������
	void Draw();

    // ��������� ���� ����, ���������� �������
	void BulletsDraw(ObjectsPool& shot_objects, EffectsContainer& eff_cont);
	void DrawOneBullet(float x, float y);

    // ����� �������� �� ������
	bool Shot();

    // ������� ������
	int Width() noexcept { return mWidth; }
	int Height() noexcept { return mHeight; }

    // ����� ���� � �������� ������
	size_t BulletsCount();
private:
    // ��������
	Render::Texture* mTexture;
	// �������� ��������� � �����������
	Render::Texture* mRechargeTexture;
    // ������
	Aim mAim;

    // ������ ����.
	// ������� ����� ����� ��������� ��� ����������� ����������� ���������� ������������ ����� ����
	using bullet_ptr = std::unique_ptr<Bullet>;
	std::vector<bullet_ptr> mBulletPool;
	// �������������� ����
	std::vector<bullet_ptr> mUsedBulletPool;
	// ���� ���� ��� ���������
	bullet_ptr mOneBullet;

	// ������ �� �������. ������� ����� �������� ��� � 0.5 ������
	Core::Timer mShotTimer;

	// ������ �� �����������. ����������� ���������� �� 2 �������
	Core::Timer mRechargeTimer;
	bool mIsRecharged;

	// ������ ��� ������� ������� ������ ����
	Core::Timer mWeaponTimer;
	// ���������� �������� ������� ��� ������� ������ ����
	float mPrevTime;

    // ������ �������� ����
	int mWinWidth;

    // ������� ������
	int mWidth;
	int mHeight;
	float mDiagonal;

    // ���� �������� ������
	float mRotateAngle;
    // ���� �����������������.
    // ���� �� true, �� ��� ��������� �������� �������������� ������������ ��� Oy �� 180 ��������
	bool mInvert;
    // ���� �������������. ������� �� ������� ����� �� ��������
	int mCorrectAngle;
    // ��������� ����� �� ��� Ox
	float mX;

    // ����� ��� �������� �����. ���� ������� �� ��������� �������.
	void RotateGun();
};