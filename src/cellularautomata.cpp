#include <iostream>
#include <string>
#include <random>
#include <format>
#include <chrono>

#define RAYGUI_IMPLEMENTATION
#include "raylibinclude.h"
#include "grid3d.h"
#include "config.h"
#include "ui.h"
#include "renderer.h"
#include "intcell.h"
#include "gradient.h"
#include "gradientpresets.h"

Grid3d<IntCell> grid(0, false, NeighbourMode::Moore);
StaticSimSettings staticSettings;
DynamicSimSettings dynamicSettings;
bool simulate = false;
int steps = 0;
std::vector<raylib::Color> gradient;

float RandomF01();
void Simulate();
void Reset(StaticSimSettings settings);
void SettingsChanged(DynamicSimSettings settings);

int main()
{
	raylib::InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Cellular Automata");
	raylib::SetTargetFPS(RENDERER_FPS);

	Renderer<IntCell> renderer = Renderer<IntCell>();
	UI ui = UI(&Reset, &SettingsChanged, [](){ steps++; }, [](bool playing) { simulate = playing; });

	auto tPrev = std::chrono::high_resolution_clock::now();
	double simSync = 0.0;

	while(!raylib::WindowShouldClose())
	{
		//Update
		auto tCurr = std::chrono::high_resolution_clock::now();
		double dt = std::chrono::duration<double>(tCurr - tPrev).count();
		tPrev = tCurr;
		double targetSimSteps = 1.0 / dynamicSettings.stepsPerSecond;
		if((simulate && ((simSync += dt) > targetSimSteps)) || steps > 0)
		{
			if(simSync > targetSimSteps)
			{
				simSync -= targetSimSteps;
			}
			steps = std::clamp(steps - 1, 0, 100000);
			Simulate();
		}
		renderer.Update();

		//Rendering
		raylib::BeginDrawing();
		{
			raylib::ClearBackground(raylib::Color { 30, 30, 30, 255 });

			renderer.Render(grid, dynamicSettings, gradient);
			ui.Update();
		}
		raylib::EndDrawing();
	}
}

float RandomF01()
{
	static std::random_device rd;
	static std::default_random_engine randEngine(rd());
	static std::uniform_real_distribution<float> randDist = std::uniform_real_distribution<float>(0.0f, 1.0f);
	return randDist(randEngine);
}

void Simulate()
{
	grid.Transform([](const IntCell& cell, int neighbours)
	{
		if(cell.IsAlive())
		{
			if(staticSettings.surviveRule[neighbours] == false)
			{
				return static_cast<IntCell>(cell - 1);
			}
			return cell;
		}
		else if(cell.IsEmpty())
		{
			if(staticSettings.spawnRule[neighbours])
			{
				return IntCell(staticSettings.states - 1);
			}
			return cell;
		}
		return static_cast<IntCell>(cell - 1);
	});
}

void Reset(StaticSimSettings settings)
{
	staticSettings = settings;
	IntCell::statesMinusOne = settings.states - 1;
	grid = Grid3d<IntCell>(settings.dimSize, settings.wrapSide, staticSettings.neighbourMode);
	simulate = false;

	raylib::Vector3 center = { settings.dimSize * 0.5f - 0.01f, settings.dimSize * 0.5f - 0.01f, settings.dimSize * 0.5f - 0.01f };
	float d = settings.fillDiameter;

	bool (*selectFunc)(raylib::Vector3 p, raylib::Vector3 center, float radius) = nullptr;
	switch(settings.fillShape)
	{
		case FillShape::Cube:
			selectFunc = [](raylib::Vector3 p, raylib::Vector3 center, float diameter)
			{
				return (std::abs(p.x - center.x) < diameter * 0.5f && std::abs(p.y - center.y) < diameter * 0.5f && std::abs(p.z - center.z) < diameter * 0.5f);
			};
			break;
		case FillShape::Sphere:
			selectFunc = [](raylib::Vector3 p, raylib::Vector3 center, float diameter)
			{
				return (std::sqrtf((p.x - center.x) * (p.x - center.x)) + std::sqrtf((p.y - center.y) * (p.y - center.y)) + std::sqrtf((p.z - center.z) * (p.z - center.z))) < diameter * 0.5f;
			};
			break;
		default:
			throw std::exception("Missing switch label in Reset!");
	}

	for(int i = 0; i < settings.dimSize; i++)
	{
		for(int k = 0; k < settings.dimSize; k++)
		{
			for(int l = 0; l < settings.dimSize; l++)
			{
				if(selectFunc(raylib::Vector3 { static_cast<float>(i), static_cast<float>(k), static_cast<float>(l) }, center, d))
				{
					grid.SetCell(i, k, l, RandomF01() < settings.fillProb ? settings.states - 1 : 0);
				}
			}
		}
	}
	grid.UpdateNeighbours();
}

void SettingsChanged(DynamicSimSettings settings)
{
	dynamicSettings = settings;
	gradient = Gradient::Generate(Gradient::GetPreset(dynamicSettings.gradientPreset), GRADIENT_STEPS);
}
