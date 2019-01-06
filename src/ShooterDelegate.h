#ifndef __SHOOTERDELEGATE_H__
#define __SHOOTERDELEGATE_H__

#pragma once

class ShooterDelegate : public Core::EngineAppDelegate {
public:
	ShooterDelegate() = default;

	virtual void GameContentSize(int deviceWidth, int deviceHeight, int &width, int &height) override;
	virtual void ScreenMode(DeviceMode &mode) override;

	virtual void RegisterTypes() override;

	virtual void LoadResources() override;
	virtual void OnResourceLoaded() override;

	virtual void OnPostDraw() override;
};

#endif // __TESTAPPDELEGATE_H__
