#pragma once

/**
 * \file
 * \brief Классы для описания мишеней
 * \author Максимовский А.С.
 */

#include <memory>
#include <atomic>

// Тип мишени.
enum class ObjectType
{
	BOMB,
	SUPER_BOMB
};

// Квант скорости
enum class VelocityType
{
	FIRST = 10,
	SECOND = 30,
	THIRD = 50,
	FOURTH = 70
};

// Количество жизней мишеней
enum class HitPoints
{
	PEASANT = 20,
	WARRIOR = 25,
	KNIGHT = 30
};

// Скорость. Содержит 2 параметра - проекции на оси Ox и Oy
struct Velocity
{
	float mVx;
	float mVy;
};

// Базовый класс для создания мишеней
struct IObjectForShot
{
	IObjectForShot(FPoint&& init_point);
	virtual ~IObjectForShot() = default;

	// Фабрика
	static std::unique_ptr<IObjectForShot> CreateObject(FPoint&& init_point, ObjectType type);

	// Методы взаимодействия с другими объектами
    // Первый имплементирует логику взаимодействия с другими мишенями
    // Второй - логику взаимодействия с пулями
	void InteractionWithOthers(const std::unique_ptr<IObjectForShot>& other);
	bool InteractionWithOthers(const FPoint& other, int size, float vx, float vy, int damage);

	// Метод, реализующий передвижение мишеней
	void MoveObject(int min_width, int max_width, int min_height, int max_height);

	// Метод для отрисовки объектов
	void Draw();

	// Текстура
	Render::Texture* mTexture;

	// Положение мишени
	FPoint mPoint;

	// Скорость
	Velocity mVelocity;

	// Размер
	int mRadius;

	// Количество жизней.
	// Атомарный параметр, так как несколько пуль одновременно могут попасть в мишень
	std::atomic<int> mHP;
protected:
	// Корректировка координат
	float mDeltaX;
	float mDeltaY;

    // Тип мишени
	ObjectType mObjectType;

    // Метод для генерации первоначальной скорости мишени
	void InitVelocity(int min, int max);

	// Метод, описывающий логику перемещения
	virtual void Move(float& x, float& y, float& vx, float& vy);
};

// Структура, описывающая объект типа Bomb
struct Bomb : public IObjectForShot
{
	Bomb(FPoint&& init_point);
	virtual ~Bomb() = default;
};

// Структура, описывающая объект типа SuperBomb
struct SuperBomb : public IObjectForShot
{
	SuperBomb(FPoint&& init_point);
	virtual ~SuperBomb() = default;

	virtual void Move(float& x, float& y, float& vx, float& vy) override;
private:
	float mTimer;
};

using objects = std::vector<std::unique_ptr<IObjectForShot>>;

// Класс, отвечающий за управление, хранение и координацию мишеней
class ObjectsPool
{
public:
	ObjectsPool() = default;

    // Метод для первоначального задания параметров.
    // Инициализации реализована не в констукторе,
    // потому что входные параметры известны не сразу
	void Init(int delta_width, int delta_height);

	// Метод для удаления всех "мертвых" мишеней
	void DeleteDeadObjects();

    // Метод для отрисовки всех мишеней
	void Draw();

    // Проверка попадания пули с положением в точке other по какой либо из мишеней
	bool CheckHitForObjects(const FPoint& other, int size, float vx, float vy, int damage);

	bool Empty() { return mObjects.size() == 0; }

	void Clear() { mObjects.clear(); }
private:
    // Метод для расчета текущих координат мишеней
	void CalcCoordinates();

    // Базовый вектор хранения указателей на объекты мишеней
	objects mObjects;

    // Ширина и высота главного окна
	int mWinWidth;
	int mWinHeight;

    // Ширина и высота смещения относительно стенок главного окна.
    // Необходимы для определения пределов перемещения мишеней.
	int mDeltaWidth;
	int mDeltaHeight;
};