#pragma once

/**
 * \file
 * \brief Вспомогательные классы
 * \author Максимовский А.С.
 */

#include <random> 

// Синглтон для генерации случайных целых и вещественных числе
class RandomGenerator
{
public:
    // Инстанс
	static RandomGenerator& Instance();

    // Генерация целых чисел в пределах от min до max
	int GenIntValue(int min, int max);

    // Генерация вещественных чисел в пределах от min до max
	float GetRealValue(int min, int max);
private:
    // Параметр для функции распределения
	std::mt19937 mGen;

	RandomGenerator();
	RandomGenerator(const RandomGenerator&) = delete;
	RandomGenerator& operator=(RandomGenerator&) = delete;
};

// Синглтон для получения параметров конфигурации из файла input.txt
class InputParser
{
public:
    // Инстанс
	static InputParser& Instance();

    // Метод для получения параметров конфигурации.
	int Get(const std::string& name);
private:
	InputParser();
	InputParser(const InputParser&) = delete;
	InputParser& operator=(InputParser&) = delete;

    // Мапа для хранения распарсенных параметров
	std::map<std::string, int> mConfig;
};

// Синглтон для расчетов косинусов и синусов углов
class CosSinCalc
{
public:
	// Инстанс
	static CosSinCalc& Instance();

	// Метод для получения косинуса угла
	float Cos(int angle);
	// Метод для получения синуса угла
	float Sin(int angle);
private:
	CosSinCalc();
	CosSinCalc(const CosSinCalc&) = delete;
	CosSinCalc& operator=(CosSinCalc&) = delete;

	// Мапа для хранения косинусов
	std::map<int, float> mCos;

	// Мапа для хранения синусов
	std::map<int, float> mSin;

	void CorrectAngle(int& angle);
};

namespace object_params
{
    // Метод для нахождения размера объекта
	int InitSize(Render::Texture* tex, float& delta_x, float& delta_y);

	// Нахождение угла между векторами
	float VectorsAngle(float x1, float x2, float x3, float x4);

	Render::Texture* GetText(const std::string& name);
}