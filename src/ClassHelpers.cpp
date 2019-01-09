/**
 * \file
 * \brief Implementation of helper classes
 * \author Maksimovskiy A.S.
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
    // Reading the file data
	IO::InputStreamPtr stream = Core::fileSystem.OpenRead("input.txt");
	if (!stream) {
		IO::FileStreamPtr filestream = new IO::FileStream("input.txt");
		if (!filestream->IsValid())
			throw std::runtime_error(std::string("Can't open file: input.txt"));
		stream = filestream;
	}

    // The stream is copied to the character vector
	std::vector<uint8_t> buffer;
	bool correct = stream->ReadAllBytes(buffer);
	if (!correct)
		throw std::runtime_error(std::string("Can't parse file: input.txt"));

    // Map is formed from the symbols vector.
    // Each line is read separately.
    // All characters before '=' are the name of the parameter, after - the value.
    // Only integer parameters are supported in this implementation.
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

	// These settings can be set from input.txt, but for now the default
	// Screen width
	if (mConfig.find("Width") == mConfig.end())
		mConfig.emplace("Width", 1024);
	// Screen height
	if (mConfig.find("Height") == mConfig.end())
		mConfig.emplace("Height", 768);

	// The bullets count in the gun
	if (mConfig.find("BulletCount") == mConfig.end())
	{
		int bullet_count = 15; // default
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
        // Getting a rectangle corresponding to the size of the texture
		IRect gun_rect = tex->getBitmapRect();
		int width = gun_rect.width;
		int height = gun_rect.height;

		delta_x = width * 0.5f;
		delta_y = height * 0.5f;

        // According to the Pythagorean theorem, the approximate radius  of a circle describing the texture is calculated
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