#pragma once
#include "config.h"
#include "presets.h"
#define RAYGUI_STATIC
#include "raylibinclude.h"

class UI
{
public:
	typedef void (*ResetCallback)(StaticSimSettings);
	typedef void (*SettingsCallback)(DynamicSimSettings);
	typedef void (*StepCallback)();
	typedef void (*PlayCallback)(bool);
	UI(ResetCallback resetCallback, SettingsCallback settingsCallback, StepCallback stepCallback, PlayCallback playCallback);
	void Update();

private:
	struct UIData
	{
		int dimSize = 50;
		RenderMode renderMode = RenderMode::Quad;
		ColorMode colorMode = ColorMode::Radius;
		GradientPreset gradientPreset = GradientPreset::Random_3;

		FillShape fillShape = FillShape::Cube;
		float fillDiameter = 5;
		float fillProb = 0.25f;

		bool wrapSide = true;
		float stepsPerSecond = 30.0f;

		NeighbourMode neighbourMode = NeighbourMode::Moore;
		int states = 2;
		char surviveRule[128] = "";
		char spawnRule[128] = "";
	};

	ResetCallback resetCallback;
	SettingsCallback settingsCallback;
	StepCallback stepCallback;
	PlayCallback playCallback;
	UIData data;
	StaticSimSettings currStaticSettings;
	bool isPlaying = false;

	void RenderFPS();
	void RenderControls();
	void RenderSettings();
	void RenderPresets();
	void LoadPreset(Preset preset);
	void TogglePlay();
	void Step();
	void SettingsChanged();
	void Reset();

	BitMask ParseRule(std::string rule);

	template<typename T>
	bool EnumDropdown(raylib::gui::layout::VerticalLayout& layout, raylib::Rectangle rect, T& value, bool& editMode);
};