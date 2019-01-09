#pragma once

/**
 * \file
 * \brief Classes to describe the behavior of weapons and bullets
 * \author Maksimovskiy A.S.
 */

#include <memory>
#include <list>

#include "ObjectsForShot.h"

// Dimension of arrays for the Runge - Kutta formula
const int N_DIM = 4;

// Bullet damage type
enum class Damage
{
	SMALL = 15,
	MIDDLE = 30,
	LARGE = 45
};

// Angle adjustment. Depends on the inclination of the cannon on the texture.
enum class AngleCorrect
{
	SMALL = 30,
	MIDDLE = 45,
	LARGE = 60
};

// Gun aim
struct Aim
{
	Render::Texture* mTexture;
	FPoint mPoint;

	void Draw();
};

using shared_tex = Render::Texture*;

// Base structure to describe the bullet
struct Bullet
{
	Bullet();

    // Calculation of the angle of rotation of the bullet and the initial coordinates
	void CalcAngles(float rotate_angle);

    // Method for simple drawing of a bullet
	void SimpleDraw();

    // Bullet drawing.
    // Accepts a link to the object vector object.
	void Draw(ObjectsPool& shot_objects);

    // Draw all effects
	void DrawEffects(EffectsContainer& eff_cont);

    // Draw ammo indicator
	void DrawBulletIndicator();

	// Bullet texture
	shared_tex mTexture;

    // Inversion flag.
    // Required to adjust the angle of the bullet and drawing.
	bool mInvert;

    // The flag is responsible for the use of bullet.
    // If a bullet hit the target, it is used.
	bool mIsUsed;

    // Bullet current position
	FPoint mCurrentPoint;
	FPoint mTargetPoint;

    // Tilt angle at which the bullet shot
	float mSystemAngle;

	// Bullet fly effect
	ParticleEffectPtr mFlyEffect;

	// Bullet hit effect
	ParticleEffectPtr mHitEffect;

    // Flag denoting the moment of a bullet shot
	bool mFirstDraw;
protected:
    // Bullet indicator texture
	shared_tex mOneBulletTexture;

    // Coordinate adjustment
	float mDeltaX;
	float mDeltaY;

    // Bullet velocity
    int mVelocity;

    // Approximate bullet size
	int mSize;

    // Bullet damage
	Damage mDamage;

    // Drag param
	float mCm;

    // Swift param
	float mKm;
private:
    // Array to store data about the previous position of the bullet
	float mXYold[N_DIM];

    // Function to calculate the coefficients by the method of Runge - Kutta
	float* RKFunc(float* xy_old);

    // Bullet movement method
	void Move();
};

// Kind of bullets used in pistols
struct PistolBullet : public Bullet
{
	PistolBullet();
};

// Weapon description class
class MachineGun
{
public:
	MachineGun();

    // Initialization of bullets in the store
	void InitBullets(bool restart = false, bool recharge = false);

    // Drawing weapons
	void Draw();

    // Drawing all bullets fired
	void BulletsDraw(ObjectsPool& shot_objects, EffectsContainer& eff_cont);
	void DrawOneBullet(float x, float y);

    // Gun shot method
	bool Shot();

    // Gun size
	int Width() noexcept { return mWidth; }
	int Height() noexcept { return mHeight; }

    // Bullets count in the gun store
	size_t BulletsCount();
private:
    // Gun texture
	Render::Texture* mTexture;
    // Recharge message texture
	Render::Texture* mRechargeTexture;
    // Gun aim
	Aim mAim;

    // Bullets array.
	using bullet_ptr = std::unique_ptr<Bullet>;
	std::vector<bullet_ptr> mBulletPool;
    // Used bullets
	std::vector<bullet_ptr> mUsedBulletPool;
    // One bullet for drawing indicator
	bullet_ptr mOneBullet;

    // Shot timer. The current gun shoots every 0.5 seconds.
	Core::Timer mShotTimer;

    // Recharge timer. Recharge will be completed in 2 seconds.
	Core::Timer mRechargeTimer;
	bool mIsRecharged;

    // Timer for calculating the flight time of the bullet
	Core::Timer mWeaponTimer;
    // Previous time to calculate bullet flight
	float mPrevTime;

    // Width of the main window
	int mWinWidth;

    // Gun size
	int mWidth;
	int mHeight;
	float mDiagonal;

    // Angle of the weapon
	float mRotateAngle;
    // Invert flag.
    // If it is true, then when drawing the texture is rotated relative to the axis Oy 180 degrees
	bool mInvert;
    // Angle adjustment. Depends on the inclination of the cannon on the texture.
	int mCorrectAngle;
    // Gun position on the axis Ox
	float mX;

    // Method to rotate the gun.The angle depends on the position of the aim.
	void RotateGun();
};