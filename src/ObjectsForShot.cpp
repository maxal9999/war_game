/**
 * \file
 * \brief Implementing classes to describe targets
 * \author Maksimovskiy A.S.
 */

#include "stdafx.h"

#include <cmath>

#include "ClassHelpers.h"
#include "ObjectsForShot.h"

// Time discrete
static float TIME_DELTA = 0.03f;

// Target interaction ratio
const float FORCE_K = 50;

IObjectForShot::IObjectForShot(FPoint&& init_point)
{
    mPoint = init_point;
}

std::unique_ptr<IObjectForShot> IObjectForShot::CreateObject(FPoint&& init_point, ObjectType type)
{
    switch (type)
    {
    case ObjectType::BOMB:
        return std::make_unique<Bomb>(std::move(init_point));
    case ObjectType::SUPER_BOMB:
        return std::make_unique<SuperBomb>(std::move(init_point));
    default:
        return nullptr;
    }
}

void IObjectForShot::InitVelocity(int min, int max)
{
    auto& inst = RandomGenerator::Instance();
    bool pos = inst.GenIntValue(0, 1);
    int16_t k = pos ? 1 : -1;
    mVelocity.mVx = k * inst.GetRealValue(min, max);
    mVelocity.mVy = k * inst.GetRealValue(min, max);
}

void IObjectForShot::InteractionWithOthers(const std::unique_ptr<IObjectForShot>& other)
{
    // Distance between targets
    float d = mPoint.GetDistanceTo(other->mPoint);
    float r2 = other->mRadius;

    // If the distance is greater than the sum of the size of the targets, then there is no interaction
    if (d >= mRadius + r2)
        return;

    float x1 = mPoint.x;
    float y1 = mPoint.y;
    float& vx1 = mVelocity.mVx;
    float& vy1 = mVelocity.mVy;

    float x2 = other->mPoint.x;
    float y2 = other->mPoint.y;
    float& vx2 = other->mVelocity.mVx;
    float& vy2 = other->mVelocity.mVy;

    // Calculation of changes in the projections of the speed of both targets.
    // F - the sum of the forces acting on the body. Accepted that this is the force of elasticity.
    // Formula: F = k * delta_x, where delta_x is the displacement during elastic impact.
    // a = F / m, where a is acceleration, m is body mass.
    // Suppose that the mass is linearly dependent on the size, and approximately m ~r
    // Substituting everything, a = k * delta_x / r
    // Projections: ax = a * (x1 - x2) / d, where d is the distance between the targets
    //              ay = a * (y1 - y2) / d
    float f = FORCE_K * (mRadius + r2 - d);
    vx1 += f * (x1 - x2) / d / mRadius * TIME_DELTA;
    vy1 += f * (y1 - y2) / d / mRadius * TIME_DELTA;
    vx2 -= f * (x1 - x2) / d / r2 * TIME_DELTA;
    vy2 -= f * (y1 - y2) / d / r2 * TIME_DELTA;
}

bool IObjectForShot::InteractionWithOthers(const FPoint& other, int size, float vx, float vy, int damage)
{
    // Distance between target and bullet
    float d = mPoint.GetDistanceTo(other);

    // If the distance is greater than the target size, then there is no interaction
    if (d >= mRadius)
        return false;

    // The equation of a line whose direction vector is (vx, vy) 
    // will be vy * x - vx * y + (vx * other.y + vy * other.x) = 0.
    // Therefore, it is necessary to check that the mPoint point lies approximately on this straight line.
    float c = vx * other.y + vy * other.x;
    float koeff = vx * mPoint.y / (vy * mPoint.x + c);
    if (abs(koeff) > size)
        return false;

    // Reduce the number of target lives depending on bullet damage
    mHP -= damage;
    return true;
}

void IObjectForShot::Move(float& x, float& y, float& vx, float& vy)
{
    x += vx * TIME_DELTA;
    y += vy * TIME_DELTA;
}

void IObjectForShot::MoveObject(int min_width, int max_width, int min_height, int max_height)
{
    float& x = mPoint.x;
    float& y = mPoint.y;
    float& vx = mVelocity.mVx;
    float& vy = mVelocity.mVy;

    Move(x, y, vx, vy);
    
    if (x < min_width + mDeltaX)
    {
        x = abs(x);
        vx *= -1;
    }
    if (y < min_height + mDeltaY)
    {
        y = abs(y);
        vy *= -1;
    }
    if (x + mRadius > max_width + mDeltaX)
    {
        x = max_width - abs(max_width - x);
        vx *= -1;
    }
    if (y + mRadius > max_height + mDeltaY)
    {
        y = max_height - abs(max_height - y);
        vy *= -1;
    }
}

void IObjectForShot::Draw()
{
    Render::device.PushMatrix();
    Render::device.MatrixTranslate(mPoint.x - mDeltaX, mPoint.y - mDeltaY, 0.0f);
    mTexture->Draw();
    Render::device.PopMatrix();
}

/*********************************************************************************************************/

Bomb::Bomb(FPoint&& init_point) : IObjectForShot(std::move(init_point))
{
    mTexture = object_params::GetText("Bomb");
    mHP = static_cast<int>(HitPoints::PEASANT);
    mObjectType = ObjectType::BOMB;
    mRadius = object_params::InitSize(mTexture, mDeltaX, mDeltaY);
    InitVelocity(static_cast<int>(VelocityType::FIRST), static_cast<int>(VelocityType::SECOND));
}

SuperBomb::SuperBomb(FPoint&& init_point) : IObjectForShot(std::move(init_point))
{
    mTexture = object_params::GetText("SuperBomb");
    mHP = static_cast<int>(HitPoints::WARRIOR);
    mObjectType = ObjectType::SUPER_BOMB;
    mRadius = object_params::InitSize(mTexture, mDeltaX, mDeltaY);
    InitVelocity(static_cast<int>(VelocityType::THIRD), static_cast<int>(VelocityType::FOURTH));
    mTimer = 0.0f;
}

void SuperBomb::Move(float& x, float& y, float& vx, float& vy)
{
    // Нелинейное движение
    x += vx * TIME_DELTA * cos(mTimer);
    y += vy * TIME_DELTA * sin(mTimer);
    mTimer += TIME_DELTA;
}

/*********************************************************************************************************/

void ObjectsPool::Init(int delta_width, int delta_height)
{
    mObjectTimer.Resume();

    auto& inst = InputParser::Instance();
    mWinWidth = inst.Get(std::string("Width"));
    mWinHeight = inst.Get(std::string("Height"));
    mDeltaWidth = delta_width;
    mDeltaHeight = delta_height * 2;
    int target_count = inst.Get(std::string("CountTarget"));
    mObjects.clear();
    mObjects.resize(static_cast<size_t>(target_count));
    for (int i = 0; i < target_count; i++)
    {
        ObjectType obj_type = i >= 10 ? ObjectType::BOMB : ObjectType::SUPER_BOMB;
        auto& inst = RandomGenerator::Instance();

        // Setting the initial position of the targets
        FPoint point(inst.GetRealValue(0, static_cast<int>(mWinWidth * 0.7)), 
                     inst.GetRealValue(mDeltaHeight, static_cast<int>(mWinHeight * 0.7)));
        // Filling the vector of pointers to the target with a new object
        mObjects[i] = IObjectForShot::CreateObject(std::move(point), obj_type);
    }

    mObjectTimer.Start();
    TIME_DELTA = 0.03f;
    mPrevTime = 0.0f;
}

void ObjectsPool::DeleteDeadObjects()
{
    auto delete_func = [](std::unique_ptr<IObjectForShot>& object) -> bool
    {
        bool is_dead = object->mHP <= 0;
        if (!is_dead)
            return false;

        object->mTexture = nullptr;
        return true;
    };

    mObjects.erase(std::remove_if(mObjects.begin(), mObjects.end(), delete_func), mObjects.end());
}

void ObjectsPool::CalcCoordinates()
{
    for (auto obj_it = mObjects.begin(); obj_it != mObjects.end(); obj_it++)
        for (auto sec_obj_it = mObjects.begin(); sec_obj_it != mObjects.end(); sec_obj_it++)
        {
            if (obj_it == sec_obj_it)
                continue;

            (*obj_it)->InteractionWithOthers(*sec_obj_it);
        }
}

void ObjectsPool::Draw()
{
    auto curr_time = mObjectTimer.getElapsedTime();
    TIME_DELTA = (curr_time - mPrevTime) * 2;
    mPrevTime = curr_time;
    CalcCoordinates();
    for (auto obj_it = mObjects.begin(); obj_it != mObjects.end(); obj_it++)
    {
        (*obj_it)->MoveObject(0, mWinWidth, mDeltaHeight, mWinHeight);
        (*obj_it)->Draw();
    }
}

bool ObjectsPool::CheckHitForObjects(const FPoint& other, int size, float vx, float vy, int damage)
{
    bool is_used = false;
    for (auto obj_it = mObjects.begin(); obj_it != mObjects.end(); obj_it++)
    {
        if (is_used)
            return is_used;

        is_used = (*obj_it)->InteractionWithOthers(other, size, vx, vy, damage);
    }

    return is_used;
}