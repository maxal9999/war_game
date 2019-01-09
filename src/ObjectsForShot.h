#pragma once

/**
 * \file
 * \brief Classes for describing targets
 * \author Maksimovskiy A.S.
 */

#include <memory>
#include <atomic>

// Target type
enum class ObjectType
{
    BOMB,
    SUPER_BOMB
};

// Velocity type
enum class VelocityType
{
    FIRST = 10,
    SECOND = 30,
    THIRD = 50,
    FOURTH = 70
};

// Target hit points
enum class HitPoints
{
    PEASANT = 20,
    WARRIOR = 25,
    KNIGHT = 30
};

// Velocity. Two params - projections on the axes of Ox and Oy
struct Velocity
{
    float mVx;
    float mVy;
};

// Base class for create targets
struct IObjectForShot
{
    IObjectForShot(FPoint&& init_point);
    virtual ~IObjectForShot() = default;
    
    // Factory
    static std::unique_ptr<IObjectForShot> CreateObject(FPoint&& init_point, ObjectType type);
    
    // Methods of the interaction with another objects.
    // The first implements the logic of interaction with another targets.
    // The second - the logic of interaction with bullets.
    void InteractionWithOthers(const std::unique_ptr<IObjectForShot>& other);
    bool InteractionWithOthers(const FPoint& other, int size, float vx, float vy, int damage);
    
    // Method to implements the movement of targets
    void MoveObject(int min_width, int max_width, int min_height, int max_height);
    
    // Method to draw the targets
    void Draw();
    
    // Texture
    Render::Texture* mTexture;
    
    // Target position
    FPoint mPoint;
    
    // Velocity
    Velocity mVelocity;
    
    // Size
    int mRadius;
    
    // Hit points
    int mHP;
protected:
    // Coordinate adjustment
    float mDeltaX;
    float mDeltaY;
    
    // Aim type
    ObjectType mObjectType;
    
    // Method to generate the original target velocity
    void InitVelocity(int min, int max);
    
    // Method to describe the logic of movement
    virtual void Move(float& x, float& y, float& vx, float& vy);
};

// Structure describing an object of type Bomb
struct Bomb : public IObjectForShot
{
    Bomb(FPoint&& init_point);
    virtual ~Bomb() = default;
};

// Structure describing an object of type SuperBomb
struct SuperBomb : public IObjectForShot
{
    SuperBomb(FPoint&& init_point);
    virtual ~SuperBomb() = default;
    
    virtual void Move(float& x, float& y, float& vx, float& vy) override;
private:
    float mTimer;
};

using objects = std::vector<std::unique_ptr<IObjectForShot>>;

// Class responsible for the management, storage and coordination of targets
class ObjectsPool
{
public:
    ObjectsPool() = default;
    
    // Method for initial setting of params
    void Init(int delta_width, int delta_height);
    
    // Method to remove all dead targets
    void DeleteDeadObjects();
    
    // Method to draw all targets
    void Draw();
    
    // Checking the hit of a bullet with a position at the point of other for any of the targets
    bool CheckHitForObjects(const FPoint& other, int size, float vx, float vy, int damage);
    
    bool Empty() { return mObjects.size() == 0; }
    
    void Clear() { mObjects.clear(); }
private:
    // Method for calculating current target coordinates
    void CalcCoordinates();
    
    // Base vector storage of pointers to target objects
    objects mObjects;
    
    // Width and height of the main window
    int mWinWidth;
    int mWinHeight;
    
    // Width and height of the offset relative to the walls of the main window.
    // It is necessary to determine the limits of movement of the targets.
    int mDeltaWidth;
    int mDeltaHeight;
    
    // Timer to calculate the time of movement of the targets
    Core::Timer mObjectTimer;
    // Previous time
    float mPrevTime;
};