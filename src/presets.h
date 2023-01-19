#pragma once
#include <string>
#include "config.h"

struct Preset
{
	std::string name;

	FillShape fillShape;
	int fillDiameter;
	float fillProb;

	NeighbourMode neighbourMode;
	int states;
	std::string surviveRule;
	std::string spawnRule;

	Preset(std::string name, FillShape fillShape, float fillDiameter, float fillProb, NeighbourMode neighbourMode, int states, std::string surviveRule, std::string spawnRule)
		: name(name), fillShape(fillShape), fillDiameter(fillDiameter), fillProb(fillProb), neighbourMode(neighbourMode), states(states), surviveRule(surviveRule), spawnRule(spawnRule)
	{
		
	}
};

const Preset PRESETS[] =
{
	Preset("Rhombus", FillShape::Cube, 2, 1.0f, NeighbourMode::Moore, 4, "", "4"),
	Preset("Cubeception", FillShape::Cube, 2, 1.0f, NeighbourMode::Moore, 5, "", "1"),
	Preset("445", FillShape::Cube, 25, 0.09f, NeighbourMode::Moore, 5, "4", "4"),
	Preset("Amoeba 1", FillShape::Cube, 10, 0.3f, NeighbourMode::Moore, 16, "9-26", "5-7,12-13,15"),
	Preset("Amoeba 2", FillShape::Cube, 10, 0.3f, NeighbourMode::Moore, 5, "9-26", "5-7,12-13,15"),
	Preset("Crystal Growth 1", FillShape::Cube, 3, 1.0f, NeighbourMode::VonNeumann, 2, "0-6", "1,3"),
	Preset("Crystal Growth 2", FillShape::Cube, 3, 1.0f, NeighbourMode::VonNeumann, 5, "1-3", "1-3"),
	Preset("Crystal Growth 3", FillShape::Cube, 10, 0.3f, NeighbourMode::VonNeumann, 5, "1,2", "1,3"),
	Preset("3D Brain", FillShape::Cube, 14, 0.33f, NeighbourMode::Moore, 2, "", "4"),
	Preset("Builder", FillShape::Cube, 13, 0.47f, NeighbourMode::Moore, 10, "2,6,9", "4,6,8,9"),
	Preset("Clouds 1", FillShape::Cube, 100, 0.5f, NeighbourMode::Moore, 2, "13-26", "13,14,17-19"),
	Preset("Clouds 2", FillShape::Cube, 100, 0.5f, NeighbourMode::Moore, 2, "12-26", "13,14"),
	Preset("Construction", FillShape::Cube, 10, 0.3f, NeighbourMode::Moore, 2, "0-2,4,6-11,13-17,21-26", "9-10,16,23-24"),
	Preset("Coral", FillShape::Cube, 6, 0.26f, NeighbourMode::Moore, 4, "5-8", "6,7,9,12"),
	Preset("More Structures", FillShape::Cube, 19, 0.42f, NeighbourMode::Moore, 4, "7-26", "4"),
	Preset("Pyroclastic", FillShape::Cube, 10, 0.3f, NeighbourMode::Moore, 10, "4-7", "6-8"),
	Preset("Spiky Growth", FillShape::Cube, 14, 0.32f, NeighbourMode::Moore, 10, "7-26", "4,12-13,15"),
	Preset("678", FillShape::Cube, 8, 0.32f, NeighbourMode::Moore, 3, "6-8", "6-8"),
	Preset("Slow Decay 1", FillShape::Cube, 100, 0.45f, NeighbourMode::Moore, 5, "1,4,8,11,13-26", "13-26"),
	Preset("Slow Decay 2", FillShape::Cube, 100, 0.4f, NeighbourMode::Moore, 3, "13-26", "10-26"),
	Preset("Ripple Cube", FillShape::Cube, 28, 0.35f, NeighbourMode::Moore, 10, "8-26", "4,12-13,5"),
};

const Preset START_PRESET = PRESETS[3];
