#pragma once

#include "ObjectsForShot.h"
#include "Weapons.h"

///
/// Виджет для отрисоки поля битвы.
///
class ShooterWidget : public GUI::Widget
{
public:
	ShooterWidget(const std::string& name, rapidxml::xml_node<>* elem);

	void Draw() override;
	void Update(float dt) override;
	
	void AcceptMessage(const Message& message) override;
	
	bool MouseDown(const IPoint& mouse_pos) override;
	void MouseMove(const IPoint& mouse_pos) override;
	void MouseUp(const IPoint& mouse_pos) override;

	void KeyPressed(int keyCode) override;
	void CharPressed(int unicodeChar) override;

private:
	void Init();

	// Объекта класса по управлению мишенями
	ObjectsPool mObjectsPool;
	// Объекта класса по управлению оружием и пулями
	MachineGun mMachineGun;

	// Таймер
	Core::Timer mTimer;

	// Объекты для отрисовки эффектов
	EffectsContainer mEffCont;

	// Текстура для отображения часов
	Render::Texture* mClock;

	// Результат
	boost::optional<bool> mWinLoseResult;
};
