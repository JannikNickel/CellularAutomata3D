#pragma once
#include <map>
#include "bitmask.h";

const float WINDOW_WIDTH = 1024;
const float WINDOW_HEIGHT = 720;

const float UI_WIDTH = 250.0f;
const float UI_LINE_HEIGHT = 25.0f;
const float UI_LINE_MARGIN = 2.5f;
const float UI_CTRL_MARGIN_TOP = 25.0f;
const float UI_CTRL_HEIGHT = 35.0f;
const float UI_CTRL_MARGIN = 5.0f;
const float UI_SETTING_LABEL_RATIO = 0.5f;
const float UI_SETTING_SPACE = 5.0f;

const float RENDERER_FOV = 60.0f;
const int RENDERER_FPS = 60;

const int SIM_MAX_STATES = 64;
const int SIM_MAX_DIM_SIZE = 100;
const int GRADIENT_STEPS = SIM_MAX_STATES;

enum class RenderMode
{
	Quad = 0,
	Cube = 1,
	Point = 2
};

enum class ColorMode
{
	State = 0,
	Xyz = 1,
	Radius = 2
};

enum class GradientPreset
{
	Random,
	Random_2,
	Random_3,
	Random_4,
	Random_5,
	Grayscale,
	Grayscale_Reverse,
	Hue,
	Hue_Reverse,
};

enum class FillShape
{
	Cube = 0,
	Sphere = 1
};

enum NeighbourMode
{
	Moore = 0,
	VonNeumann = 1
};

struct StaticSimSettings
{
	int dimSize;

	FillShape fillShape;
	float fillDiameter;
	float fillProb;

	bool wrapSide;

	NeighbourMode neighbourMode;
	int states;
	BitMask surviveRule;
	BitMask spawnRule;
};

struct DynamicSimSettings
{
	RenderMode renderMode;
	ColorMode colorMode;
	GradientPreset gradientPreset;

	float stepsPerSecond;
};
