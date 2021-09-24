#pragma once

#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"

#include "version.h"
constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);


class PlayerSwitch: public BakkesMod::Plugin::BakkesModPlugin, public BakkesMod::Plugin::PluginSettingsWindow {

public:
	virtual void onLoad();
	virtual void onUnload();

	void GameStart(std::string eventName);
	void EpochNow(std::string eventName);
	
	void performSwitchStatic();
	void performSwitchDynamic();
	void switchPlayers();

	PriWrapper getPlayer1();
	PriWrapper getPlayer2(std::string id_string_p1, std::string name_p1);
	int createRandomInt(int player_count);
	float createNextRandomSwitch();
	int getStaticSwitchCvarValue();
	bool isSamePlayer(std::string id_string_p1, std::string id_string_p2, std::string name_p1, std::string name_p2);

	void switchLocation(CarWrapper car_p1, CarWrapper car_p2);
	void switchVelocity(CarWrapper car_p1, CarWrapper car_p2);
	void switchRotation(CarWrapper car_p1, CarWrapper car_p2);
	void switchBoost(CarWrapper car_p1, CarWrapper car_p2);

	// Constans
	const int TIME_DIFFERENCE_MIN = 1;
	const int TIME_DIFFERENCE_MAX = 300;

	const int TEAM_NUM_BLUE = 0;
	const int TEAM_NUM_ORANGE = 1;

	// Dynamic variables
	float last_switch = 0.0f;
	float next_random_dynamic_switch = 0.0f;

	// Gui functions
	void RenderSettings() override;
	std::string GetPluginName() override;
	void SetImGuiContext(uintptr_t ctx) override;

	// Render functions
	void EnableCheckbox();
	void DrawTextNote();
	void StaticEnableCheckbox();
	void StaticDifferenceSlider();
	void DynamicEnableCheckbox();
	void DynamicDifferenceSlider();
	void OtherSettingsText();
	void SwitchVelocityCheckbox();
	void SwitchRotationCheckbox();
	void SwitchBoostCheckbox();
	void DrawTextDevNote();
};
