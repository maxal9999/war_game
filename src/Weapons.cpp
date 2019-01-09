/**
 * \file
 * \brief Implementing classes to describe the behavior of weapons and bullets
 * \author Maksimovskiy A.S.
 */

#include "stdafx.h"

#include <corecrt_math_defines.h>
#include <future>

#include "ClassHelpers.h"
#include "Weapons.h"

// 180 degree angle
const float PI_DEGREES = 180.0f;
// Time discrete
static float TIME_DELTA = 0.1f;
// Acceleration of gravity
const float G = 9.81f;
// Air resistance (kg / m ^ 3)
const float RHO = 1.23;

void Aim::Draw()
{
    // Current position of the mouse. The aim is attached to the mouse cursor.
    IPoint mouse_pos = Core::mainInput.GetMousePos();
    mPoint.x = static_cast<float>(mouse_pos.x);
    mPoint.y = static_cast<float>(mouse_pos.y);

    IRect aim_rect = mTexture->getBitmapRect();
    Render::device.PushMatrix();
    Render::device.MatrixTranslate(mPoint.x - aim_rect.width * 0.5f,
                                   mPoint.y - aim_rect.height * 0.5f, 0.0f);
    mTexture->Draw();
    Render::device.PopMatrix();
}

/**********************************************************************************/

Bullet::Bullet() : mIsUsed(false)
{
    mVelocity = InputParser::Instance().Get(std::string("Speed"));
    mSystemAngle = 0;
    mFirstDraw = false;
    mOneBulletTexture = object_params::GetText("OneBullet");
}

float* Bullet::RKFunc(float* xy_old)
{
    float* y = new float[N_DIM];

    y[0] = xy_old[1];
    // Vx change
    y[1] = -mCm * xy_old[1] - mKm * xy_old[3];
    y[2] = xy_old[3];
    // Vy change
    y[3] = -G - mCm * xy_old[3] + mKm * xy_old[1];

    return (y);
}

void Bullet::CalcAngles(float rotate_angle)
{
    float ang = rotate_angle * M_PI / PI_DEGREES;

    // Initial position, initial speed, shot angle
    // x0
    mXYold[0] = mCurrentPoint.x;
    // vx0
    mXYold[1] = mVelocity * cos(ang) * (mInvert ? -1 : 1);
    // y0
    mXYold[2] = mCurrentPoint.y;
    // vy0
    mXYold[3] = mVelocity * sin(ang) * (mInvert ? -1 : 1);
    // Shot angle
    mSystemAngle = rotate_angle;
}

void Bullet::Move()
{
    /**
    * In this method, the movement of the bullet is calculated as for an object launched at an angle to the horizon.
    * But in addition to the action of gravity, factors such as 
    * air resistance, angular velocity and wind action are also taken into account.
    * From the second law of Newton a = F / m, whence it follows that
    * d^2(x)/dt^2 = dvx/dt = (-(c/m) * vx - (k/m) * vy) * sqrt(vx * vx + vy * vy)
    * d^2(y)/dt^2 = dvy/dt = ((k/m) * vx - (c/m) * vy) * sqrt(vx * vx + vy * vy) - g ,
    * where с = (1/2) * Сd * A * ro, k = (1/2) * Cl * A * ro.
    * c is the drag coefficient due to air resistance,
    * k - offset factor due to angular velocity and wind
    * A - the cross-sectional area of ​​the bullet (or any other object)
    * ro - air resistance
    * Cd - deceleration parameter, Cl - offset parameter
    * Cd = 0.30 + (2.58 * 10^(-4)) * w
    * Cl = 0.319 * (1 - exp(-2.48 * 10^(-3) * w)), where w is the angular velocity in rad / s
    *
    * Further, the RK4 method is used - one of the Runge-Kutta family of numerical methods.
    * At each step n and at time iteration dt, it turns out
    * xy[n+1] = xy[n] + (1/6) * (k1 + 2*k2 + 2*k3 + k4), где
    * k1 = dt * RK4(xy[n])
    * k2 = dt * RK4(xy[n] + k1 / 2)
    * k3 = dt * RK4(xy[n] + k2 / 2)
    * k4 = dt * RK4(xy[n] + k3)
    *
    * xy - at each iteration n, this is an array of 4 elements:
    * current x and y coordinates and velocity projections vx and vy
    */

    // If the bullet did not hit one target,
    // it is considered used when it hits the ground.
    if (mXYold[2] < -0.001)
    {
        mIsUsed = true;
        return;
    }

    float y_new[N_DIM];
    float y1[N_DIM];
    float y2[N_DIM];
    float y3[N_DIM];

    // k1 = f(tn, yn)
    float* k1 = RKFunc(mXYold);
    for (int i = 0; i < N_DIM; i++)
        y1[i] = mXYold[i] + 0.5 * TIME_DELTA * k1[i];

    // k2 = f(tn + h/2, yn + k1/2)
    float* k2 = RKFunc(y1);
    for (int i = 0; i < N_DIM; i++)
        y2[i] = mXYold[i] + 0.5 * TIME_DELTA * k2[i];

    // k3 = f(tn + h/2, yn + k2/2)
    float* k3 = RKFunc(y2);
    for (int i = 0; i < N_DIM; i++)
        y3[i] = mXYold[i] + TIME_DELTA * k3[i];

    // k4 = f(tn + h/2, yn + k3)
    float* k4 = RKFunc(y3);

    for (int i = 0; i < N_DIM; i++)
        y_new[i] = mXYold[i] + TIME_DELTA * (k1[i] + 2.0 * k2[i] + 2.0 * k3[i] + k4[i]) / 6.0;

    mCurrentPoint.x = y_new[0];
    mCurrentPoint.y = y_new[2];
    for (int i = 0; i < N_DIM; i++)
        mXYold[i] = y_new[i];

    delete[] k1;
    delete[] k2;
    delete[] k3;
    delete[] k4;
}

void Bullet::SimpleDraw()
{
    Render::device.PushMatrix();
    Render::device.MatrixTranslate(mCurrentPoint.x - mDeltaX, mCurrentPoint.y - mDeltaY, 0);
    auto angle = acos(mXYold[3] / sqrt(mXYold[3] * mXYold[3] + mXYold[1] * mXYold[1]));
    float real_angle = mSystemAngle + angle * PI_DEGREES / M_PI * (mInvert ? 1 : -1);
    Render::device.MatrixRotate(math::Vector3(0, 0, 1), mInvert ? PI_DEGREES + real_angle : real_angle);
    mTexture->Draw();
    Render::device.PopMatrix();
}

void Bullet::Draw(ObjectsPool& shot_objects)
{
    // Move the bullet on the current iteration
    Move();

    // Check for hit on any of the objects.
    // If there was a hit, then we do not draw a bullet.
    bool used = shot_objects.CheckHitForObjects(mCurrentPoint, mSize, mXYold[1], mXYold[3], static_cast<int>(mDamage));
    if (used)
        mIsUsed = true;

    SimpleDraw();
}

void Bullet::DrawEffects(EffectsContainer& eff_cont)
{
    if (!mFlyEffect)
        mFlyEffect = eff_cont.AddEffect("FlyBullet");
    
    if (mFlyEffect)
    {
        mFlyEffect->posX = mCurrentPoint.x - mDeltaX;
        mFlyEffect->posY = mCurrentPoint.y - mDeltaY;

        if (mIsUsed)
            mFlyEffect->Finish();
    }

    if (mFirstDraw)
    {
        auto shot_effect = eff_cont.AddEffect("Shot");
        shot_effect->posX = mCurrentPoint.x - mDeltaX;
        shot_effect->posY = mCurrentPoint.y - mDeltaY;
        shot_effect->Reset();
        mFirstDraw = false;
    }

    if (!mIsUsed)
        return;

    if (!mHitEffect)
        mHitEffect = eff_cont.AddEffect("HitObject");
    if (!mHitEffect)
        return;

    mHitEffect->posX = mCurrentPoint.x - mDeltaX;
    mHitEffect->posY = mCurrentPoint.y - mDeltaY;
    mHitEffect->Reset();
}

void Bullet::DrawBulletIndicator()
{
    Render::device.PushMatrix();
    Render::device.MatrixTranslate(mCurrentPoint.x, mCurrentPoint.y, 0);
    mOneBulletTexture->Draw();
    Render::device.PopMatrix();
}

/**********************************************************************************/

PistolBullet::PistolBullet() : Bullet()
{
    mTexture = object_params::GetText("Bullet");
    float dx, dy;
    mSize = object_params::InitSize(mTexture, dx, dy);

    mDeltaX = dx;
    mDeltaY = dy;
    mDamage = Damage::SMALL;

    // Angular velocity
    float rpm = 5.0f;
    // Convert to rad/s
    float w = rpm * M_PI / 30.00;
    // Cross sectional area in m ^ 2
    float S = 0.0021;
    // Bullet mass in kg
    float m = 0.018;

    // Drag coefficient
    float CD = 0.30 + 2.58e-4 * w;
    mCm = 0.5 * CD * S * RHO / m;

    // Swift factor
    float CL = 0.3187 * (1.0 - exp(-2.483e-3 * w));
    mKm = 0.5 * CL * S * RHO / m; 
}

/**********************************************************************************/

MachineGun::MachineGun() : 
    mIsRecharged(false),
    mRotateAngle(0),
    mInvert(false)
{
    mTexture = object_params::GetText("LeftGun");
    mRechargeTexture = object_params::GetText("Recharge");
    IRect gun_rect = mTexture->getBitmapRect();
    mWidth = gun_rect.width;
    mHeight = gun_rect.height;
    mDiagonal = math::sqrt(mWidth * mWidth + mHeight * mHeight);
    mCorrectAngle = static_cast<int>(AngleCorrect::MIDDLE);

    mAim.mTexture = object_params::GetText("Aim");

    auto& inst = InputParser::Instance();
    mWinWidth = inst.Get(std::string("Width"));
    mX = static_cast<float>(mWinWidth / 2);

    size_t bullets_count = inst.Get(std::string("BulletCount"));
    mBulletPool.reserve(bullets_count);
    mUsedBulletPool.reserve(bullets_count);
}

void MachineGun::InitBullets(bool restart, bool recharge)
{
    mWeaponTimer.Resume();

    if (restart)
    {
        mBulletPool.clear();
        std::for_each(mUsedBulletPool.begin(), mUsedBulletPool.end(), [](bullet_ptr& bullet_obj)
        {
            if (bullet_obj->mFlyEffect)
                bullet_obj->mFlyEffect->Finish();
        });
        mUsedBulletPool.clear();
    }

    if (recharge)
    {
        MM::manager.PlaySample("RechargeSound");
        mRechargeTimer.Start();
        mIsRecharged = true;
    }

    auto& inst = InputParser::Instance();
    size_t bullets_count = inst.Get(std::string("BulletCount"));
    size_t current_size = mBulletPool.size();
    for (size_t i = 0; i < bullets_count - current_size; i++)
        mBulletPool.push_back(std::make_unique<PistolBullet>());
    mOneBullet = std::make_unique<PistolBullet>();
    
    mWeaponTimer.Start();
    mPrevTime = 0.0f;
}

void MachineGun::BulletsDraw(ObjectsPool& shot_objects, EffectsContainer& eff_cont)
{
    auto curr_time = mWeaponTimer.getElapsedTime();
    TIME_DELTA = (curr_time - mPrevTime) * 10;
    mPrevTime = curr_time;

    if (mBulletPool.size() == 0)
    {
        Render::device.PushMatrix();
        mRechargeTexture->Draw();
        Render::device.PopMatrix();
    }

    for (auto bullet_it = mUsedBulletPool.begin(); bullet_it != mUsedBulletPool.end(); bullet_it++)
    {
        (*bullet_it)->Draw(shot_objects);
        (*bullet_it)->DrawEffects(eff_cont);
    }

    auto delete_func = [](bullet_ptr& b_object) -> bool
    {
        bool is_used = b_object->mIsUsed;
        if (!is_used)
            return false;

        b_object->mFlyEffect = nullptr;
        b_object->mHitEffect = nullptr;
        b_object->mTexture = nullptr;
        return true;
    };

    mUsedBulletPool.erase(std::remove_if(mUsedBulletPool.begin(), mUsedBulletPool.end(), delete_func), mUsedBulletPool.end());
}

void MachineGun::DrawOneBullet(float x, float y)
{
    mOneBullet->mCurrentPoint = FPoint(x, y);
    mOneBullet->DrawBulletIndicator();
}

void MachineGun::RotateGun()
{
    float x1 = mAim.mPoint.x - mX;
    float x2 = 1.0f;
    float y1 = mAim.mPoint.y;
    float y2 = 0.0f;
    // Angle of the vector of the sight
    auto angle = object_params::VectorsAngle(x1, x2, y1, y2);
    mRotateAngle = angle * PI_DEGREES / M_PI;

    mInvert = false;
    // Inversion support: if the weapon is turned more than 90 degrees, 
    // then it is necessary to rotate the texture around Oy
    if (mRotateAngle > PI_DEGREES / 2)
    {
        mRotateAngle -= PI_DEGREES / 2;
        mInvert = true;
    }
    mRotateAngle -= mCorrectAngle;
    Render::device.MatrixRotate(math::Vector3(0, 0, 1), mRotateAngle);
    if (mInvert)
        Render::device.MatrixRotate(math::Vector3(0, 1, 0), PI_DEGREES);
}

void MachineGun::Draw()
{
    mAim.Draw();

    Render::device.PushMatrix();
    Render::device.MatrixTranslate(mX, 0, 0);
    RotateGun();

    IRect gun_rect = mTexture->getBitmapRect();
    FRect rect(gun_rect);
    FRect uv(0, 1, 0, 1);
    mTexture->TranslateUV(rect, uv);
    mTexture->Bind();
    Render::DrawQuad(rect, uv);
    Render::device.PopMatrix();
}

bool MachineGun::Shot()
{
    if (mBulletPool.size() == 0 || mShotTimer.getElapsedTime() < 0.5f || mIsRecharged)
        return false;

    MM::manager.PlaySample("ShotSound");
    mShotTimer.Resume();
    bullet_ptr bullet = std::move(mBulletPool[mBulletPool.size()-1]);
    mBulletPool.pop_back();
    bullet->mTargetPoint = mAim.mPoint;
    bullet->mInvert = mInvert;

    // The initial position of the bullet corresponds to the top of the texture describing the weapon, 
    // turned at an angle relative to the aim
    auto rotate_angle = mInvert ? mRotateAngle - PI_DEGREES / 2 : mRotateAngle;
    auto angle = rotate_angle * M_PI / PI_DEGREES;
    auto init_x = mWidth * math::cos(angle) - mHeight * math::sin(angle) + mX;
    auto init_y = mHeight * math::cos(angle) + mWidth * math::sin(angle);

    // Adjusting the initial position of the bullet
    auto init_point = FPoint(mInvert ? mWinWidth - init_x : init_x, abs(init_y));
    bullet->mCurrentPoint = init_point;
    bullet->CalcAngles(rotate_angle + mCorrectAngle);
    bullet->mFirstDraw = true;
    mUsedBulletPool.push_back(std::move(bullet));
    mShotTimer.Start();
    return true;
}

size_t MachineGun::BulletsCount() 
{ 
    // Recharging the gun takes time, so for 1 second we show that the magazine has 0 rounds
    if (mIsRecharged && mRechargeTimer.getElapsedTime() < 1.0f)
        return 0;

    if (mIsRecharged)
    {
        mRechargeTimer.Resume();
        mIsRecharged = false;
    }

    return mBulletPool.size(); 
}