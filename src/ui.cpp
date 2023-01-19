#include "ui.h"
#include <format>
#include <type_traits>
#include <regex>
#include <cstring>
#include "magic_enum.hpp"

namespace gui = raylib::gui;

#define LABEL_CENTER(X) \
gui::GuiSetStyle(gui::GuiControl::LABEL, gui::GuiControlProperty::TEXT_ALIGNMENT, gui::GuiTextAlignment::TEXT_ALIGN_CENTER); \
(X); \
gui::GuiSetStyle(gui::GuiControl::LABEL, gui::GuiControlProperty::TEXT_ALIGNMENT, 0);


UI::UI(ResetCallback resetCallback, SettingsCallback settingsCallback, StepCallback stepCallback, PlayCallback playCallback) : resetCallback(resetCallback), settingsCallback(settingsCallback), stepCallback(stepCallback), playCallback(playCallback)
{
	gui::LoadDefaultStyle();
	LoadPreset(START_PRESET);
}

void UI::Update()
{
	RenderFPS();
	RenderControls();
	RenderSettings();
	RenderPresets();
}

void UI::RenderFPS()
{
	gui::GuiLabel(raylib::Rectangle { 0.0f, WINDOW_HEIGHT - UI_LINE_HEIGHT, 50.0f, UI_LINE_HEIGHT }, std::format("FPS = {0}", raylib::GetFPS()).c_str());
}

void UI::RenderControls()
{
	char playBtnText[32];
	std::strcpy(playBtnText, gui::GuiIconText(isPlaying ? gui::ICON_PLAYER_PAUSE : gui::ICON_PLAYER_PLAY, isPlaying ? "PAUSE" : "PLAY"));
	char stepBtnText[32];
	std::strcpy(stepBtnText, gui::GuiIconText(gui::ICON_PLAYER_NEXT, "STEP"));
	char resetBtnText[32];
	std::strcpy(resetBtnText, gui::GuiIconText(gui::ICON_CROSS, "RESET"));
	const char* btnTexts[3] = { resetBtnText, stepBtnText, playBtnText };
	float ctrlWidth = 0.0f;
	for(const char* btnText : btnTexts)
	{
		ctrlWidth += gui::CalcTextWidth(btnText) + UI_CTRL_MARGIN;
	}
	raylib::Rectangle ctrlRect = {};
	ctrlRect.x = WINDOW_WIDTH * 0.5f - ctrlWidth * 0.5f;
	ctrlRect.y = UI_CTRL_MARGIN_TOP;
	ctrlRect.height = UI_CTRL_HEIGHT;
	for(const char* btnText : btnTexts)
	{
		ctrlRect.width = gui::CalcTextWidth(btnText);
		if(gui::GuiButton(ctrlRect, btnText))
		{
			if(btnText == playBtnText)
			{
				TogglePlay();
			}
			else if(btnText == stepBtnText)
			{
				Step();
			}
			else if(btnText == resetBtnText)
			{
				Reset();
			}
		}
		ctrlRect.x += ctrlRect.width + UI_CTRL_MARGIN;
	}
}

void UI::RenderSettings()
{
	static bool settingsVisible = true;
	static const int highlightColor = 0xF5BF42FF;
	bool staticSettingsMismatch = false;

	gui::layout::VerticalLayout layout(0.0f, UI_WIDTH, UI_LINE_HEIGHT, UI_LINE_MARGIN);
	layout.Space(UI_CTRL_MARGIN_TOP);

	if(!settingsVisible)
	{
		char headerButtonText[32];
		std::strcpy(headerButtonText, gui::GuiIconText(gui::ICON_GEAR_BIG, ""));
		raylib::Rectangle rect = layout.GetNextLayoutRect(UI_CTRL_HEIGHT);
		rect.x = rect.y * 0.5f;
		rect.width = rect.height;
		if(gui::GuiButton(rect, headerButtonText))
		{
			settingsVisible = !settingsVisible;
		}

		return;
	}

	char headerButtonText[32];
	std::strcpy(headerButtonText, gui::GuiIconText(gui::ICON_ARROW_LEFT, "SETTINGS"));
	if(gui::GuiButton(layout.GetNextLayoutRect(UI_CTRL_HEIGHT), headerButtonText))
	{
		settingsVisible = !settingsVisible;
	}

	//Rendering
	layout.Space(UI_SETTING_SPACE);
	LABEL_CENTER(gui::GuiLabel(layout.GetNextLayoutRect(), "Rendering"));

	auto [lr, rr] = layout.SplitHorizontal(layout.GetNextLayoutRect(), UI_SETTING_LABEL_RATIO);
	gui::GuiLabel(lr, "Size");
	{
		bool highlight = currStaticSettings.dimSize != data.dimSize;
		staticSettingsMismatch |= highlight;
		gui::ScopedStyle style(highlight, gui::GuiControl::VALUEBOX, gui::GuiControlProperty::BORDER_COLOR_NORMAL, highlightColor);
		static bool dimEdit = false;
		static int tempDimSize = !dimEdit ? data.dimSize : tempDimSize;
		if(gui::GuiValueBox(rr, "", &tempDimSize, 5, SIM_MAX_DIM_SIZE, dimEdit))
		{
			dimEdit = !dimEdit;
			if(!dimEdit)
			{
				data.dimSize = tempDimSize;
			}
		}
	}

	std::tie(lr, rr) = layout.SplitHorizontal(layout.GetNextLayoutRect(), UI_SETTING_LABEL_RATIO);
	gui::GuiLabel(lr, "Render Mode");
	static bool renderModeEdit = false;
	if(EnumDropdown(layout, rr, data.renderMode, renderModeEdit))
	{
		SettingsChanged();
	}

	std::tie(lr, rr) = layout.SplitHorizontal(layout.GetNextLayoutRect(), UI_SETTING_LABEL_RATIO);
	gui::GuiLabel(lr, "Color Mode");
	static bool colorModeEdit = false;
	if(EnumDropdown(layout, rr, data.colorMode, colorModeEdit))
	{
		SettingsChanged();
	}

	if(data.colorMode != ColorMode::Xyz)
	{
		std::tie(lr, rr) = layout.SplitHorizontal(layout.GetNextLayoutRect(), UI_SETTING_LABEL_RATIO);
		gui::GuiLabel(lr, "Gradient");
		static bool gradientEdit = false;
		if(EnumDropdown(layout, rr, data.gradientPreset, gradientEdit))
		{
			SettingsChanged();
		}
	}

	//Initial Setup
	layout.Space(UI_SETTING_SPACE);
	LABEL_CENTER(gui::GuiLabel(layout.GetNextLayoutRect(), "Setup"));

	std::tie(lr, rr) = layout.SplitHorizontal(layout.GetNextLayoutRect(), UI_SETTING_LABEL_RATIO);
	gui::GuiLabel(lr, "Fill Shape");
	{
		bool highlight = currStaticSettings.fillShape != data.fillShape;
		staticSettingsMismatch |= highlight;
		gui::ScopedStyle style(highlight, gui::GuiControl::DROPDOWNBOX, gui::GuiControlProperty::BORDER_COLOR_NORMAL, highlightColor);
		static bool fillShapeEdit = false;
		EnumDropdown(layout, rr, data.fillShape, fillShapeEdit);
	}

	std::tie(lr, rr) = layout.SplitHorizontal(layout.GetNextLayoutRect(), UI_SETTING_LABEL_RATIO);
	gui::GuiLabel(lr, "Fill Diameter");
	{
		bool highlight = currStaticSettings.fillDiameter != data.fillDiameter;
		staticSettingsMismatch |= highlight;
		gui::ScopedStyle style(highlight, gui::GuiControl::SLIDER, gui::GuiControlProperty::BORDER_COLOR_NORMAL, highlightColor);
		data.fillDiameter = gui::GuiSliderBar(rr, std::format("{:.0f}", data.fillDiameter).c_str(), "", data.fillDiameter, 1, data.dimSize);
		data.fillDiameter = std::roundf(data.fillDiameter);
	}

	std::tie(lr, rr) = layout.SplitHorizontal(layout.GetNextLayoutRect(), UI_SETTING_LABEL_RATIO);
	gui::GuiLabel(lr, "Fill Prob");
	{
		bool highlight = currStaticSettings.fillProb != data.fillProb;
		staticSettingsMismatch |= highlight;
		gui::ScopedStyle style(highlight, gui::GuiControl::SLIDER, gui::GuiControlProperty::BORDER_COLOR_NORMAL, highlightColor);
		data.fillProb = gui::GuiSliderBar(rr, std::format("{:.0f}%", data.fillProb * 100.0f).c_str(), "", data.fillProb, 0.0f, 1.0f);
	}

	//Simulation
	layout.Space(UI_SETTING_SPACE);
	LABEL_CENTER(gui::GuiLabel(layout.GetNextLayoutRect(), "Simulation"));

	std::tie(lr, rr) = layout.SplitHorizontal(layout.GetNextLayoutRect(), UI_SETTING_LABEL_RATIO);
	gui::GuiLabel(lr, "Wrap Around");
	{
		bool highlight = currStaticSettings.wrapSide != data.wrapSide;
		staticSettingsMismatch |= highlight;
		gui::ScopedStyle style(highlight, gui::GuiControl::TOGGLE, gui::GuiControlProperty::BORDER_COLOR_NORMAL, highlightColor);
		data.wrapSide = gui::GuiToggleGroup(raylib::Rectangle { rr.x, rr.y, rr.width * 0.5f, rr.height }, "NO;YES", data.wrapSide ? 1 : 0) == 1;
	}

	std::tie(lr, rr) = layout.SplitHorizontal(layout.GetNextLayoutRect(), UI_SETTING_LABEL_RATIO);
	gui::GuiLabel(lr, "Steps/s");
	float oldStepsPerSecond = data.stepsPerSecond;
	data.stepsPerSecond = gui::GuiSliderBar(rr, std::format("{:.0f}", data.stepsPerSecond).c_str(), "", data.stepsPerSecond, 0.0f, 60.0f);
	data.stepsPerSecond = std::roundf(data.stepsPerSecond);
	if(data.stepsPerSecond != oldStepsPerSecond)
	{
		SettingsChanged();
	}

	//Rules
	layout.Space(UI_SETTING_SPACE);
	LABEL_CENTER(gui::GuiLabel(layout.GetNextLayoutRect(), "Rules"));

	std::tie(lr, rr) = layout.SplitHorizontal(layout.GetNextLayoutRect(), UI_SETTING_LABEL_RATIO);
	gui::GuiLabel(lr, "Neighbours");
	{
		bool highlight = currStaticSettings.neighbourMode != data.neighbourMode;
		staticSettingsMismatch |= highlight;
		gui::ScopedStyle style(highlight, gui::GuiControl::DROPDOWNBOX, gui::GuiControlProperty::BORDER_COLOR_NORMAL, highlightColor);
		static bool neighbourModeEdit = false;
		EnumDropdown(layout, rr, data.neighbourMode, neighbourModeEdit);
	}

	std::tie(lr, rr) = layout.SplitHorizontal(layout.GetNextLayoutRect(), UI_SETTING_LABEL_RATIO);
	gui::GuiLabel(lr, "States");
	{
		bool highlight = currStaticSettings.states != data.states;
		staticSettingsMismatch |= highlight;
		gui::ScopedStyle style(highlight, gui::GuiControl::VALUEBOX, gui::GuiControlProperty::BORDER_COLOR_NORMAL, highlightColor);
		static bool statesEditMode = false;
		if(gui::GuiValueBox(rr, "", &data.states, 2, SIM_MAX_STATES, statesEditMode))
		{
			statesEditMode = !statesEditMode;
		}
	}

	gui::GuiLabel(layout.GetNextLayoutRect(), "Survive Rule");
	{
		bool highlight = (uint64_t)currStaticSettings.surviveRule != (uint64_t)ParseRule(std::string(data.surviveRule));
		staticSettingsMismatch |= highlight;
		gui::ScopedStyle style(highlight, gui::GuiControl::TEXTBOX, gui::GuiControlProperty::BORDER_COLOR_NORMAL, highlightColor);
		static bool surviveRuleEdit = false;
		if(gui::GuiTextBox(layout.GetNextLayoutRect(), data.surviveRule, 32, surviveRuleEdit))
		{
			surviveRuleEdit = !surviveRuleEdit;
		}
	}

	gui::GuiLabel(layout.GetNextLayoutRect(), "Spawn Rule");
	{
		bool highlight = (uint64_t)currStaticSettings.spawnRule != (uint64_t)ParseRule(std::string(data.spawnRule));
		staticSettingsMismatch |= highlight;
		gui::ScopedStyle style(highlight, gui::GuiControl::TEXTBOX, gui::GuiControlProperty::BORDER_COLOR_NORMAL, highlightColor);
		static bool spawnRuleEdit = false;
		if(gui::GuiTextBox(layout.GetNextLayoutRect(), data.spawnRule, 32, spawnRuleEdit))
		{
			spawnRuleEdit = !spawnRuleEdit;
		}
	}

	if(staticSettingsMismatch)
	{
		gui::ScopedStyle style(true, gui::GuiControl::LABEL, gui::GuiControlProperty::TEXT_COLOR_NORMAL, highlightColor);
		static const char labelText[] = "Press RESET to apply highlighted settings";
		gui::GuiLabel(layout.GetNextLayoutRect(), labelText);
	}
}

void UI::RenderPresets()
{
	static bool presetsVisible = false;

	gui::layout::VerticalLayout layout(WINDOW_WIDTH - UI_WIDTH, UI_WIDTH, UI_LINE_HEIGHT, UI_LINE_MARGIN);
	layout.Space(UI_CTRL_MARGIN_TOP);

	if(!presetsVisible)
	{
		char headerButtonText[32];
		std::strcpy(headerButtonText, gui::GuiIconText(gui::ICON_NOTEBOOK, ""));
		raylib::Rectangle rect = layout.GetNextLayoutRect(UI_CTRL_HEIGHT);
		rect.width = rect.height;
		rect.x = WINDOW_WIDTH - rect.width - rect.y * 0.5f;
		if(gui::GuiButton(rect, headerButtonText))
		{
			presetsVisible = !presetsVisible;
		}

		return;
	}

	char headerButtonText[32];
	std::strcpy(headerButtonText, gui::GuiIconText(gui::ICON_ARROW_RIGHT, "PRESETS"));
	if(gui::GuiButton(layout.GetNextLayoutRect(UI_CTRL_HEIGHT), headerButtonText))
	{
		presetsVisible = !presetsVisible;
	}
	layout.Space(UI_SETTING_SPACE);

	raylib::Rectangle pageCtrlRect = layout.GetNextLayoutRect();
	static const float pItemHeight = UI_LINE_HEIGHT * 2.5f;
	static const int itemCount = sizeof(PRESETS) / sizeof(PRESETS[0]);
	static const float pageHeight = WINDOW_HEIGHT - (pageCtrlRect.y + UI_SETTING_SPACE) * 2.0f;
	static const int itemsPerPage = static_cast<int>(std::floor(pageHeight / pItemHeight));
	static const int pages = static_cast<int>(std::ceil(itemCount / static_cast<float>(itemsPerPage)));

	static int page = 1;
	auto [l, rr] = layout.SplitHorizontal(pageCtrlRect, 0.666f);
	auto [lr, mr] = layout.SplitHorizontal(l, 0.5f);
	if(gui::GuiButton(lr, "<"))
	{
		page--;
	}
	LABEL_CENTER(gui::GuiLabel(mr, std::format("{0} / {1}", page, pages).c_str()));
	if(gui::GuiButton(rr, ">"))
	{
		page++;
	}
	page = std::clamp(page, 1, pages);
	layout.Space(UI_SETTING_SPACE);

	int from = (page - 1) * itemsPerPage;
	int to = std::min(from + itemsPerPage, itemCount);
	for(int i = from; i < to; i++)
	{
		const Preset& preset = PRESETS[i];
		gui::GuiSetStyle(gui::GuiControl::BUTTON, gui::GuiControlProperty::TEXT_ALIGNMENT, gui::GuiTextAlignment::TEXT_ALIGN_LEFT);
		std::string content = std::format(" {0}\n {1}/{2}/{3}/{4}", preset.name, preset.surviveRule, preset.spawnRule, preset.states, preset.neighbourMode == NeighbourMode::Moore ? "M" : "VN");
		if(gui::GuiButton(layout.GetNextLayoutRect(pItemHeight), content.c_str()))
		{
			LoadPreset(preset);
		}
		gui::GuiSetStyle(gui::GuiControl::BUTTON, gui::GuiControlProperty::TEXT_ALIGNMENT, gui::GuiTextAlignment::TEXT_ALIGN_CENTER);
	}
}

void UI::LoadPreset(Preset preset)
{
	data.fillShape = preset.fillShape;
	data.fillDiameter = std::clamp(preset.fillDiameter, 0, data.dimSize);
	data.fillProb = preset.fillProb;
	data.neighbourMode = preset.neighbourMode;
	data.states = preset.states;
	std::snprintf(data.surviveRule, 128, "%s", preset.surviveRule.c_str());
	std::snprintf(data.spawnRule, 128, "%s", preset.spawnRule.c_str());
	Reset();
}

void UI::TogglePlay()
{
	isPlaying = !isPlaying;
	if(playCallback)
	{
		playCallback(isPlaying);
	}
}

void UI::Step()
{
	if(stepCallback)
	{
		stepCallback();
	}
}

void UI::SettingsChanged()
{
	if(settingsCallback)
	{
		settingsCallback(DynamicSimSettings
		{
			.renderMode = data.renderMode,
			.colorMode = data.colorMode,
			.gradientPreset = data.gradientPreset,
			.stepsPerSecond = data.stepsPerSecond
		});
	}
}

void UI::Reset()
{
	isPlaying = false;
	SettingsChanged();
	if(resetCallback)
	{
		currStaticSettings = StaticSimSettings
		{
			.dimSize = data.dimSize,
			.fillShape = data.fillShape,
			.fillDiameter = data.fillDiameter,
			.fillProb = data.fillProb,
			.wrapSide = data.wrapSide,
			.neighbourMode = data.neighbourMode,
			.states = data.states,
			.surviveRule = ParseRule(std::string(data.surviveRule)),
			.spawnRule = ParseRule(std::string(data.spawnRule)),
		};
		resetCallback(currStaticSettings);
	}
}

BitMask UI::ParseRule(std::string rule)
{
	rule = std::regex_replace(rule, std::regex(" "), "");
	BitMask mask;
	const std::regex reg(R"(((\d+)-(\d+))|(\d+))");
	std::smatch match;
	while(std::regex_search(rule, match, reg))
	{
		if(match[1].str().length())
		{
			//Range
			int from = std::stoi(match[2]);
			int to = std::stoi(match[3]);
			for(int i = from; i <= to; i++)
			{
				if(i >= 0 && i <= 26)
				{
					mask.Set(i, true);
				}
			}
		}
		else
		{
			//Single int
			int i = std::stoi(match[0]);
			if(i >= 0 && i <= 26)
			{
				mask.Set(i, true);
			}
		}
		rule = match.suffix();
	}
	return mask;
}

template<typename T>
bool UI::EnumDropdown(gui::layout::VerticalLayout& layout, raylib::Rectangle rect, T& value, bool& editMode)
{
	static_assert(std::is_enum<T>::value, "Must be an enum type");

	auto enumValues = magic_enum::enum_values<T>();
	std::string options = "";
	std::for_each(enumValues.begin(), enumValues.end(), [&](T n)
	{
		if(options.length() > 0)
		{
			options.append(";");
		}
		options.append(magic_enum::enum_name(n));
	});

	int val = static_cast<int>(value);
	int prevVal = val;
	if(gui::GuiDropdownBox(rect, options.c_str(), &val, editMode))
	{
		editMode = !editMode;
	}

	if(editMode)
	{
		layout.Space(enumValues.size() * (UI_LINE_HEIGHT + UI_LINE_MARGIN));
	}

	value = static_cast<T>(val);
	return val != prevVal;
}
