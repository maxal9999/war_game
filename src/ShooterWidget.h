#pragma once

#include "ObjectsForShot.h"
#include "Weapons.h"

// Widget for drawing the battlefield.
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
    
    // Target management class object
    ObjectsPool mObjectsPool;
    // Weapons and bullet class object
    MachineGun mMachineGun;
    
    // Timer
    Core::Timer mTimer;
    
    // Objects for drawing effects
    EffectsContainer mEffCont;
    
    // Texture to display the clock
    Render::Texture* mClock;
    
    // Battle result
    boost::optional<bool> mWinLoseResult;
};
