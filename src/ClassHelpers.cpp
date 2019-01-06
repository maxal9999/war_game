/**
 * \file
 * \brief Имплементация вспомогательных классов
 * \author Максимовский А.С.
 */

#include "stdafx.h"
#include <boost/algorithm/string.hpp>
#include <ctime>
#include <corecrt_math_defines.h>

#include "ClassHelpers.h"


RandomGenerator::RandomGenerator() :
	mGen(time(0))
{
}

RandomGenerator& RandomGenerator::Instance()
{
	static RandomGenerator rand_gen_instance;
	return rand_gen_instance;
}
	
int RandomGenerator::GenIntValue(int min, int max)
{
	std::uniform_int_distribution<> urd(min, max);
	return urd(mGen);
}
	
float RandomGenerator::GetRealValue(int min, int max)
{
	std::uniform_real_distribution<> urd(min, max);
	return static_cast<float>(urd(mGen));
}

/*********************************************************************************************************/

InputParser::InputParser()
{
    // Вычитываются данные файла
	IO::InputStreamPtr stream = Core::fileSystem.OpenRead("input.txt");
	if (!stream) {
		IO::FileStreamPtr filestream = new IO::FileStream("input.txt");
		if (!filestream->IsValid())
			throw std::runtime_error(std::string("Can't open file: input.txt"));
		stream = filestream;
	}

    // Для удобства поток копируется в вектор символов
	std::vector<uint8_t> buffer;
	bool correct = stream->ReadAllBytes(buffer);
	if (!correct)
		throw std::runtime_error(std::string("Can't parse file: input.txt"));

    // Формируем мапу из вектора символа.
    // Вычитываем отдельно каждую строку.
    // Все символы до '=' - имя параметра, после - значение.
    // В данной реализации поддерживаются только целочисленные параметры.
	auto begin_it = buffer.begin(), end_it = buffer.end();
	while (begin_it != end_it) 
	{
		auto mid_it = std::find(begin_it, end_it, '\n');
		std::vector<std::string> parse;
		std::string str(begin_it, mid_it);
		auto r_it = str.find('\r');
		if (r_it != std::string::npos)
			str.erase(r_it, 1);
		boost::split(parse, str, boost::is_any_of("="));
		begin_it = mid_it;
		if (mid_it != end_it)
			++begin_it;

		if (parse.size() != 2)
			continue;

		mConfig.emplace(parse[0], utils::lexical_cast<int>(parse[1]));
	}

	// Эти настройки можно задать из input.txt, но пока по умолчанию
	// Ширина экрана
	if (mConfig.find("Width") == mConfig.end())
		mConfig.emplace("Width", 1024);
	// Высота экрана
	if (mConfig.find("Height") == mConfig.end())
		mConfig.emplace("Height", 768);

	// Число пуль в магазине пушки
	if (mConfig.find("BulletCount") == mConfig.end())
	{
		int bullet_count = 15; // по умолчанию
		auto count_target = mConfig.find("CountTarget");
		if (count_target != mConfig.end())
			bullet_count = (*count_target).second;
		mConfig.emplace("BulletCount", bullet_count);
	}
}

InputParser& InputParser::Instance()
{
	static InputParser input_parser_instance;
	return input_parser_instance;
}

int InputParser::Get(const std::string& name)
{
	auto find_id = mConfig.find(name);
	if (find_id == mConfig.end())
		return 0;

	return (*find_id).second;
}

/*********************************************************************************************************/

CosSinCalc::CosSinCalc()
{
	for (int i = 0; i < 360; i++)
	{
		mCos.emplace(i, math::cos(i * M_PI / 180));
		mSin.emplace(i, math::sin(i * M_PI / 180));
	}
}

CosSinCalc& CosSinCalc::Instance()
{
	static CosSinCalc cos_sin_instance;
	return cos_sin_instance;
}

void CosSinCalc::CorrectAngle(int& angle)
{
	if (angle < 0)
		angle += 360;
	if (angle >= 360)
		angle -= 360;
}

float CosSinCalc::Cos(int angle)
{
	CorrectAngle(angle);
	auto find_id = mCos.find(angle);
	if (find_id == mCos.end())
		return 0;

	return (*find_id).second;
}

float CosSinCalc::Sin(int angle)
{
	CorrectAngle(angle);
	auto find_id = mSin.find(angle);
	if (find_id == mSin.end())
		return 0;

	return (*find_id).second;
}

/*********************************************************************************************************/

namespace object_params
{
	int InitSize(Render::Texture* tex, float& delta_x, float& delta_y)
	{
        // Получаем прямоугольник, соответствующий размерам текстуры
		IRect gun_rect = tex->getBitmapRect();
		int width = gun_rect.width;
		int height = gun_rect.height;

		delta_x = width * 0.5f;
		delta_y = height * 0.5f;

        // По теореме Пифагора рассчитываем приблизительный радиус окружности, описывающую текстуру
		return static_cast<int>(sqrt(delta_x * delta_x + delta_y * delta_y));
	}

	float VectorsAngle(float x1, float x2, float y1, float y2)
	{
		float num = x1 * x2 + y1 * y2;
		float denom = sqrt(static_cast<double>(x1 * x1 + y1 * y1) * sqrt(static_cast<double>(x2 * x2 + y2 * y2)));
		return acos(num / denom);
	}

	Render::Texture* GetText(const std::string& name)
	{
		return Core::resourceManager.Get<Render::Texture>(name);
	}
}