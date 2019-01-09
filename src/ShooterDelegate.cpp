#include "stdafx.h"
#include "ClassHelpers.h"
#include "ShooterDelegate.h"
#include "ShooterWidget.h"


void ShooterDelegate::GameContentSize(int deviceWidth, int deviceHeight, int &width, int &height)
{
    auto& inst = InputParser::Instance();
    width = inst.Get(std::string("Width"));
    height = inst.Get(std::string("Height"));
}

void ShooterDelegate::ScreenMode(DeviceMode &mode)
{
    mode = DeviceMode::Windowed;
}

void ShooterDelegate::RegisterTypes()
{
    REGISTER_WIDGET_XML(ShooterWidget, "ShooterWidget");
}

void ShooterDelegate::LoadResources()
{
    Core::LuaExecuteStartupScript("start.lua");
}

void ShooterDelegate::OnResourceLoaded() {
    if (Core::appInstance->AllResourcesLoaded()) {
        Core::appInstance->SetLoaded();
    }
}

void ShooterDelegate::OnPostDraw() 
{
    if (!Render::isFontLoaded("arial"))
        return;

    Render::BindFont("arial");

    int dy = Render::getFontHeight();
    int x = Render::device.Width() - 5;
    int y = Render::device.Height() - 20;

    Render::PrintString(x, y, std::string("FPS: ") + utils::lexical_cast(Core::appInstance->GetCurrentFps()), 1.0f, RightAlign, BottomAlign);
    Render::PrintString(x, y -= dy, std::string("Video: ") + utils::lexical_cast(Render::device.GetVideoMemUsage() / 1024) + std::string("K"), 1.0f, RightAlign, BottomAlign);
    Render::PrintString(x, y -= dy, std::string("Audio: ") + utils::lexical_cast(Core::resourceManager.GetMemoryInUse<MM::AudioResource>() / 1024) + std::string("K"), 1.0f, RightAlign, BottomAlign);
    Render::PrintString(x, y -= dy, std::string("Animations: ") + utils::lexical_cast((Core::resourceManager.GetMemoryInUse<Render::StreamingAnimation>() + Core::resourceManager.GetMemoryInUse<Render::Animation>()) / 1024) + std::string("K"), 1.0f, RightAlign, BottomAlign);
    Render::PrintString(x, y -= dy, std::string("Textures: ") + utils::lexical_cast(Core::resourceManager.GetMemoryInUse<Render::Texture>() / 1024) + std::string("K"), 1.0f, RightAlign, BottomAlign);
    Render::PrintString(x, y -= dy, std::string("Particles: ") + utils::lexical_cast(Core::resourceManager.GetMemoryInUse<ParticleEffect>() / 1024) + std::string("K"), 1.0f, RightAlign, BottomAlign);
    Render::PrintString(x, y -= dy, std::string("Models: ") + utils::lexical_cast(Core::resourceManager.GetMemoryInUse<Render::ModelAnimation>() / 1024) + std::string("K"), 1.0f, RightAlign, BottomAlign);
}
