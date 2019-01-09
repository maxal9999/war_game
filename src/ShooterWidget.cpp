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

    // Drawing rectangles in which there will be indicators of hours and bullets
	Render::device.SetTexturing(false);
	Render::BeginColor(Color("#FFFAFA"));
	Render::DrawRect(width - 200, 0, 200, 80);
	Render::DrawRect(0, 0, 200, 80);
	Render::EndColor();
	Render::device.SetTexturing(true);

    // Drawing all targets
	mObjectsPool.Draw();

	// Drawing the weapon
	mMachineGun.Draw();
	// Drawing the bullets
	mMachineGun.BulletsDraw(mObjectsPool, mEffCont);

    // Remove all dead targets
	mObjectsPool.DeleteDeadObjects();

    // Drawing the number of remaining bullets in the gun
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

    // Draw all the effects that are added to the container
	mEffCont.Draw();
}

void ShooterWidget::Update(float dt)
{
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
	// Press on the key 'R' to reload weapons
	if (keyCode == VK_R) 
		mMachineGun.InitBullets(false, true);

    // Press on the 'B' key, the game will start again
	if (keyCode == VK_B)
	{
		MM::manager.ChangeTrack("MainTheme", 0.1f);
		Init();
	}
}

void ShooterWidget::CharPressed(int unicodeChar)
{
}
