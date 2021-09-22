#include "pch.h"
#include "PlayerSwitch.h"

void PlayerSwitch::SetImGuiContext(uintptr_t ctx) {
	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
}

std::string PlayerSwitch::GetPluginName() {
	return "PlayerSwitch";
}

/*
 * GUI Rendering
 */
void PlayerSwitch::RenderSettings() {
	EnableCheckbox();
    DrawTextNote();
    ImGui::Separator();

    StaticEnableCheckbox();
    StaticDifferenceSlider();
    DynamicEnableCheckbox();
    DynamicDifferenceSlider();
    ImGui::Separator();

    OtherSettingsText();
    SwitchBoostCheckbox();
    SwitchVelocityCheckbox();
    SwitchRotationCheckbox();
    ImGui::Separator();

    DrawTextDevNote();
}

/*
 * Enable-Checkbox
 */
void PlayerSwitch::EnableCheckbox() {
    CVarWrapper enableCvar = cvarManager->getCvar("playerswitch_enabled");
    if (!enableCvar) { return; }

    bool enabled = enableCvar.getBoolValue();
    if (ImGui::Checkbox("Enable", &enabled)) {
        enableCvar.setValue(enabled);
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Enable/Disable");
    }
}

/*
 * Draws the notes
 */
void PlayerSwitch::DrawTextNote() {
    ImGui::Text("Settings have to be applies before the match starts! At best configure the settings before creating a private game.");
    ImGui::Text("Either use static OR dynamic. Static meen every <x> seconds. Dynamic means a randomly generated number between <min> and <max> will be used.");
}

/*
 * Enable-Checkbox for static
 */
void PlayerSwitch::StaticEnableCheckbox() {
    CVarWrapper enableCvar = cvarManager->getCvar("playerswitch_static_enabled");
    if (!enableCvar) { return; }

    bool enabled = enableCvar.getBoolValue();
    if (ImGui::Checkbox("Enable for static time", &enabled)) {
        enableCvar.setValue(enabled);
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Enable/Disable static");
    }
}

/*
 * Time Difference-Slider for static
 */
void PlayerSwitch::StaticDifferenceSlider() {
    CVarWrapper staticDifferenceCvar = cvarManager->getCvar("playerswitch_time_difference_static");
    if (!staticDifferenceCvar) { return; }

    int difference = staticDifferenceCvar.getIntValue();
    if (ImGui::SliderInt("Static time in seconds", &difference, TIME_DIFFERENCE_MIN, TIME_DIFFERENCE_MAX, "%d")) {
        staticDifferenceCvar.setValue(difference);
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Time in seconds in which players will be switched");
    }
}

/*
 * Enable-Checkbox for dynamic
 */
void PlayerSwitch::DynamicEnableCheckbox() {
    CVarWrapper enableCvar = cvarManager->getCvar("playerswitch_dynamic_enabled");
    if (!enableCvar) { return; }

    bool enabled = enableCvar.getBoolValue();
    if (ImGui::Checkbox("Enable for dynamic time", &enabled)) {
        enableCvar.setValue(enabled);
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Enable/Disable dynamic");
    }
}

/*
 * Time Difference-Slider for dynamic
 * Also handels logic to prevent:
 *   min >= max
 *   max <= min
 */
void PlayerSwitch::DynamicDifferenceSlider() {
    CVarWrapper staticDifferenceMinCvar = cvarManager->getCvar("playerswitch_time_difference_dynamic_min");
    if (!staticDifferenceMinCvar) { return; }
    CVarWrapper staticDifferenceMaxCvar = cvarManager->getCvar("playerswitch_time_difference_dynamic_max");
    if (!staticDifferenceMaxCvar) { return; }

    int differenceMin = staticDifferenceMinCvar.getIntValue();
    int differenceMax = staticDifferenceMaxCvar.getIntValue();
    if (differenceMin >= differenceMax) { 
        if (differenceMax == TIME_DIFFERENCE_MIN) {
            differenceMax = TIME_DIFFERENCE_MIN + 1;
            differenceMin = TIME_DIFFERENCE_MIN;
        } else {
            differenceMin = differenceMax - 1;
        }
    }
    if (differenceMax <= differenceMin) {
        if (differenceMin == TIME_DIFFERENCE_MAX) {
            differenceMin = TIME_DIFFERENCE_MAX - 1;
            differenceMax = TIME_DIFFERENCE_MAX;
        } else {
            differenceMax = differenceMin + 1;
        }
    }

    int difference [2] = {differenceMin, differenceMax};
    if (ImGui::SliderInt2("Dynamic time (min-max) in seconds", difference, TIME_DIFFERENCE_MIN, TIME_DIFFERENCE_MAX, "%d")) {
        staticDifferenceMinCvar.setValue(difference[0]);
        staticDifferenceMaxCvar.setValue(difference[1]);
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Time window in seconds in which players will be switched");
    }
}

void PlayerSwitch::OtherSettingsText() {
    ImGui::Text("Other Settings");
}

/*
 * Checkbox for switing boost
 */
void PlayerSwitch::SwitchBoostCheckbox() {
    CVarWrapper enableCvar = cvarManager->getCvar("playerswitch_switch_boost");
    if (!enableCvar) { return; }

    bool enabled = enableCvar.getBoolValue();
    if (ImGui::Checkbox("Enable boost switching", &enabled)) {
        enableCvar.setValue(enabled);
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Enable/Disable boost switching");
    }
}

/*
 * Checkbox for switching velocity
 */
void PlayerSwitch::SwitchVelocityCheckbox() {
    CVarWrapper enableCvar = cvarManager->getCvar("playerswitch_switch_velocity");
    if (!enableCvar) { return; }

    bool enabled = enableCvar.getBoolValue();
    if (ImGui::Checkbox("Enable velocity switching", &enabled)) {
        enableCvar.setValue(enabled);
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Enable/Disable velocity switching");
    }
}

/*
 * Checkbox for switching rotation
 */
void PlayerSwitch::SwitchRotationCheckbox() {
    CVarWrapper enableCvar = cvarManager->getCvar("playerswitch_switch_rotation");
    if (!enableCvar) { return; }

    bool enabled = enableCvar.getBoolValue();
    if (ImGui::Checkbox("Enable rotation switching", &enabled)) {
        enableCvar.setValue(enabled);
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Enable/Disable rotation switching");
    }
}

/*
 * Draws the dev notes
 */
void PlayerSwitch::DrawTextDevNote() {
    ImGui::Text("Plugin made by xXxProKill (Oliver#1504)");
}
