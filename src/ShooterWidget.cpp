#include "stdafx.h"

#include <windows.h>

#include "ClassHelpers.h"
#include "ShooterWidget.h"

ShooterWidget::ShooterWidget(const std::string& name, rapidxml::xml_node<>* elem)
	: Widget(name)
	, mMachineGun()
{
	Init();
}

void ShooterWidget::Init()
{
	mWinLoseResult = boost::none;
	mMachineGun.InitBullets(true);
	mObjectsPool.Init(mMachineGun.Width(), mMachineGun.Height());
	mClock = object_params::GetText("Clock");
	mTimer.Start();
#if defined(ENGINE_TARGET_WIN32)
	ShowCursor(FALSE);
#endif
}

void ShooterWidget::Draw()
{
	if (mWinLoseResult)
	{
		if (*mWinLoseResult)
		{
			Render::Texture* win = Core::resourceManager.Get<Render::Texture>("WinBackground");
			Render::device.PushMatrix();
			win->Draw();
			Render::device.PopMatrix();
			return;
		}
		else
		{
			Render::Texture* lose = Core::resourceManager.Get<Render::Texture>("LoseBackground");
			Render::device.PushMatrix();
			lose->Draw();
			Render::device.PopMatrix();
			return;
		}
	}

	MM::manager.PlayTrack("MainTheme");
	auto& inst = InputParser::Instance();
	int width = inst.Get(std::string("Width"));
	int height = inst.Get(std::string("Height"));
	auto time_limit = inst.Get(std::string("Time"));
	auto delta_time = time_limit - static_cast<int>(mTimer.getElapsedTime());
	
	if (mObjectsPool.Empty() && delta_time > 0)
	{
		mTimer.Resume();
		mWinLoseResult = true;
		MM::manager.ChangeTrack("WinTheme", 0.1f);
		return;
	}
	else if (delta_time <= 0)
	{
		mTimer.Resume();
		mObjectsPool.Clear();
		mWinLoseResult = false;
		MM::manager.ChangeTrack("LoseTheme", 0.1f);
		return;
	}

	// Отрисовка прямоугольников, в которых будут индикаторы часов и пуль.
	Render::device.SetTexturing(false);
	Render::BeginColor(Color("#FFFAFA"));
	Render::DrawRect(width - 200, 0, 200, 80);
	Render::DrawRect(0, 0, 200, 80);
	Render::EndColor();
	Render::device.SetTexturing(true);

	// Отрисовка всех мишеней
	mObjectsPool.Draw();

	// Отрисовка оружия
	mMachineGun.Draw();
	// Отрисовка пуль
	mMachineGun.BulletsDraw(mObjectsPool, mEffCont);

	// Удаление всех "убитых" мишеней
	mObjectsPool.DeleteDeadObjects();

	// Отрисовка числа оставшихся пуль в магазине
	mMachineGun.DrawOneBullet(width - 180, 25);
	Render::BindFont("arial");
	auto b_count = mMachineGun.BulletsCount();

	Render::BeginColor(Color("#8B0000"));
	Render::PrintString(width - mMachineGun.Width()/2, 
		                mMachineGun.Height()/2, 
		                utils::lexical_cast(b_count),
		                5.f, CenterAlign);

	Render::PrintString(mMachineGun.Width() / 2,
						mMachineGun.Height() / 2,
						utils::lexical_cast(delta_time),
						5.f, CenterAlign);
	Render::EndColor();

	Render::device.PushMatrix();
	Render::device.MatrixTranslate(mMachineGun.Width(), 0, 0);
	mClock->Draw();
	Render::device.PopMatrix();

	//
	// Рисуем все эффекты, которые добавили в контейнер (Update() для контейнера вызывать не нужно).
	//
	mEffCont.Draw();
}

void ShooterWidget::Update(float dt)
{
	//
	// Обновим контейнер с эффектами
	//
	mEffCont.Update(dt);
}

bool ShooterWidget::MouseDown(const IPoint &mouse_pos)
{
	if (Core::mainInput.GetMouseRightButton())
	{
	}
	else
	{
		mMachineGun.Shot();
	}
	return false;
}

void ShooterWidget::MouseMove(const IPoint &mouse_pos)
{
}

void ShooterWidget::MouseUp(const IPoint &mouse_pos)
{
}

void ShooterWidget::AcceptMessage(const Message& message)
{
}

void ShooterWidget::KeyPressed(int keyCode)
{
	// По клавише 'R' происходит перезарядка оружия
	if (keyCode == VK_R) 
		mMachineGun.InitBullets(false, true);

	// По клавише 'B' игра начнется заново
	if (keyCode == VK_B)
	{
		MM::manager.ChangeTrack("MainTheme", 0.1f);
		Init();
	}
}

void ShooterWidget::CharPressed(int unicodeChar)
{
}
