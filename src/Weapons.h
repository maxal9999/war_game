#pragma once

/**
 * \file
 * \brief Классы для описания поведения оружия и пуль
 * \author Максимовский А.С.
 */

#include <memory>
#include <list>

#include "ObjectsForShot.h"

// размерность массивов для формулы Рунге-Кутта
const int N_DIM = 4;

// Урон от пули
enum class Damage
{
	SMALL = 15,
	MIDDLE = 30,
	LARGE = 45
};

// Угол корректировки. Зависит от наклона пушки на текстуре
enum class AngleCorrect
{
	SMALL = 30,
	MIDDLE = 45,
	LARGE = 60
};

// Структура, описывающая прицел орудия
struct Aim
{
	Render::Texture* mTexture;
	FPoint mPoint;

	void Draw();
};

using shared_tex = Render::Texture*;

// Базовая структура описания пули
struct Bullet
{
	Bullet();

    // Расчет угла поворота пули и начальных координат
	void CalcAngles(float rotate_angle);

	// Метод для простой отрисовки пули
	void SimpleDraw();

    // Отрисовка пули.
    // Принимает ссылку на объект вектора объектов.
    // Необходимо для проверки попадания пули по мишеням
	void Draw(ObjectsPool& shot_objects);

	// Отрисовать все эффекты
	void DrawEffects(EffectsContainer& eff_cont);

	// Отрисовать индикатор патронов
	void DrawBulletIndicator();

	// Текстура
	shared_tex mTexture;

    // Флаг инвертированности.
    // Необходим для корректировки угла наклона пули и отрисовки.
	bool mInvert;

    // Флаг отвечающий за использованность пули.
    // Если пулей попали по мишени, то она использована
	bool mIsUsed;

    // Текущее положение пули
	FPoint mCurrentPoint;
	FPoint mTargetPoint;

    // Угол наклона, под которым произошел выстрел пулей
	float mSystemAngle;

	// Эффект при полете пули
	ParticleEffectPtr mFlyEffect;

	// Эффект при полете пули
	ParticleEffectPtr mHitEffect;

	// Флаг, обозначающий момент выстрела пули
	bool mFirstDraw;
protected:
	// Текстура индикатора пули
	shared_tex mOneBulletTexture;

	// Корректировка координат
	float mDeltaX;
	float mDeltaY;

    // Скорость пули
    int mVelocity;

    // Приблизительный размер пули
	int mSize;

    // Урон от пули
	Damage mDamage;

    // Параметр торможения
	float mCm;

	// Параметр сдвига
	float mKm;
private:
    // Массив для хранения данных о предыдущем положении пули
	float mXYold[N_DIM];

	// Функция для рассчет коэффициентов методом Рунге-Кутта
	float* RKFunc(float* xy_old);

	// Метод движения пули
	void Move();
};

// Вид пуль, используемая в пистолетах
struct PistolBullet : public Bullet
{
	PistolBullet();
};

// Класс описания оружия
class MachineGun
{
public:
	MachineGun();

	// Инициализация пуль в магазине
	void InitBullets(bool restart = false, bool recharge = false);

    // Отрисовка оружия
	void Draw();

    // Отрисовка всех пуль, выпущенных оружием
	void BulletsDraw(ObjectsPool& shot_objects, EffectsContainer& eff_cont);
	void DrawOneBullet(float x, float y);

    // Метод выстрела из оружия
	bool Shot();

    // Размеры оружия
	int Width() noexcept { return mWidth; }
	int Height() noexcept { return mHeight; }

    // Число пуль в магазине оружия
	size_t BulletsCount();
private:
    // Текстура
	Render::Texture* mTexture;
	// Текстура сообщения о перезарядке
	Render::Texture* mRechargeTexture;
    // Прицел
	Aim mAim;

    // Массив пуль.
	// Сделано через умные указатели для возможности дальнейшего добавления разнообразия видов пуль
	using bullet_ptr = std::unique_ptr<Bullet>;
	std::vector<bullet_ptr> mBulletPool;
	// Использованные пули
	std::vector<bullet_ptr> mUsedBulletPool;
	// Одна пуля для отрисовки
	bullet_ptr mOneBullet;

	// Таймер на выстрел. Текущая пушка стреляет раз в 0.5 секунд
	Core::Timer mShotTimer;

	// Таймер на перезарядку. Перезарядка выполнятся за 2 секунды
	Core::Timer mRechargeTimer;
	bool mIsRecharged;

	// Таймер для расчета времени полета пули
	Core::Timer mWeaponTimer;
	// Предыдущее значение времени для расчета полета пули
	float mPrevTime;

    // Ширина главного окна
	int mWinWidth;

    // Размеры оружия
	int mWidth;
	int mHeight;
	float mDiagonal;

    // Угол поворота оружия
	float mRotateAngle;
    // Флаг инвертированности.
    // Если он true, то при отрисовке текстура поворачивается относительно оси Oy на 180 градусов
	bool mInvert;
    // Угол корректировки. Зависит от наклона пушки на текстуре
	int mCorrectAngle;
    // Положение пушки на оси Ox
	float mX;

    // Метод для поворота пушки. Угол зависит от положения прицела.
	void RotateGun();
};