#pragma once

/**
 * \file
 * \brief ������ ��� �������� �������
 * \author ������������ �.�.
 */

#include <memory>
#include <atomic>

// ��� ������.
enum class ObjectType
{
	BOMB,
	SUPER_BOMB
};

// ����� ��������
enum class VelocityType
{
	FIRST = 10,
	SECOND = 30,
	THIRD = 50,
	FOURTH = 70
};

// ���������� ������ �������
enum class HitPoints
{
	PEASANT = 20,
	WARRIOR = 25,
	KNIGHT = 30
};

// ��������. �������� 2 ��������� - �������� �� ��� Ox � Oy
struct Velocity
{
	float mVx;
	float mVy;
};

// ������� ����� ��� �������� �������
struct IObjectForShot
{
	IObjectForShot(FPoint&& init_point);
	virtual ~IObjectForShot() = default;

	// �������
	static std::unique_ptr<IObjectForShot> CreateObject(FPoint&& init_point, ObjectType type);

	// ������ �������������� � ������� ���������
    // ������ �������������� ������ �������������� � ������� ��������
    // ������ - ������ �������������� � ������
	void InteractionWithOthers(const std::unique_ptr<IObjectForShot>& other);
	bool InteractionWithOthers(const FPoint& other, int size, float vx, float vy, int damage);

	// �����, ����������� ������������ �������
	void MoveObject(int min_width, int max_width, int min_height, int max_height);

	// ����� ��� ��������� ��������
	void Draw();

	// ��������
	Render::Texture* mTexture;

	// ��������� ������
	FPoint mPoint;

	// ��������
	Velocity mVelocity;

	// ������
	int mRadius;

	// ���������� ������.
	// ��������� ��������, ��� ��� ��������� ���� ������������ ����� ������� � ������
	std::atomic<int> mHP;
protected:
	// ������������� ���������
	float mDeltaX;
	float mDeltaY;

    // ��� ������
	ObjectType mObjectType;

    // ����� ��� ��������� �������������� �������� ������
	void InitVelocity(int min, int max);

	// �����, ����������� ������ �����������
	virtual void Move(float& x, float& y, float& vx, float& vy);
};

// ���������, ����������� ������ ���� Bomb
struct Bomb : public IObjectForShot
{
	Bomb(FPoint&& init_point);
	virtual ~Bomb() = default;
};

// ���������, ����������� ������ ���� SuperBomb
struct SuperBomb : public IObjectForShot
{
	SuperBomb(FPoint&& init_point);
	virtual ~SuperBomb() = default;

	virtual void Move(float& x, float& y, float& vx, float& vy) override;
private:
	float mTimer;
};

using objects = std::vector<std::unique_ptr<IObjectForShot>>;

// �����, ���������� �� ����������, �������� � ����������� �������
class ObjectsPool
{
public:
	ObjectsPool() = default;

    // ����� ��� ��������������� ������� ����������.
    // ������������� ����������� �� � �����������,
    // ������ ��� ������� ��������� �������� �� �����
	void Init(int delta_width, int delta_height);

	// ����� ��� �������� ���� "�������" �������
	void DeleteDeadObjects();

    // ����� ��� ��������� ���� �������
	void Draw();

    // �������� ��������� ���� � ���������� � ����� other �� ����� ���� �� �������
	bool CheckHitForObjects(const FPoint& other, int size, float vx, float vy, int damage);

	bool Empty() { return mObjects.size() == 0; }

	void Clear() { mObjects.clear(); }
private:
    // ����� ��� ������� ������� ��������� �������
	void CalcCoordinates();

    // ������� ������ �������� ���������� �� ������� �������
	objects mObjects;

    // ������ � ������ �������� ����
	int mWinWidth;
	int mWinHeight;

    // ������ � ������ �������� ������������ ������ �������� ����.
    // ���������� ��� ����������� �������� ����������� �������.
	int mDeltaWidth;
	int mDeltaHeight;
};