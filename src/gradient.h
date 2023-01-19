#pragma once
#include <map>
#include <vector>
#include <cmath>
#include <random>

#include "config.h"
#define RAYLIB_STATIC
#include "raylibinclude.h"

namespace Gradient
{
	static const std::map<GradientPreset, std::vector<uint32_t>> GRADIENT_PRESETS =
	{
		{ GradientPreset::Grayscale,			{ 0xFFFFFF, 0x000000 } },
		{ GradientPreset::Grayscale_Reverse,	{ 0x000000, 0xFFFFFF } },
		{ GradientPreset::Hue,					{ 0xFF0000, 0xFF00AA, 0xAA00FF, 0x0000FF, 0x00AAFF, 0x00FFAA, 0x00FF00, 0xAAFF00, 0xFFAA00, 0xFF0000 } },
		{ GradientPreset::Hue_Reverse,			{ 0xFF0000, 0xFFAA00, 0xAAFF00, 0x00FF00, 0x00FFAA, 0x00AAFF, 0x0000FF, 0xAA00FF, 0xFF00AA, 0xFF0000 } },
	};

	static raylib::Color Lerp(raylib::Color a, raylib::Color b, float t)
	{
		return raylib::Color
		{
			static_cast<unsigned char>(std::round(((1.0f - t) * (a.r / 255.0f) + (b.r / 255.0f) * t) * 255.0f)),
			static_cast<unsigned char>(std::round(((1.0f - t) * (a.g / 255.0f) + (b.g / 255.0f) * t) * 255.0f)),
			static_cast<unsigned char>(std::round(((1.0f - t) * (a.b / 255.0f) + (b.b / 255.0f) * t) * 255.0f)),
			static_cast<unsigned char>(std::round(((1.0f - t) * (a.a / 255.0f) + (b.a / 255.0f) * t) * 255.0f))
		};
	}

	static std::vector<raylib::Color> Generate(const std::vector<raylib::Color>& keys, int samples)
	{
		float step = 1.0f / (samples - 1);
		float keyStep = 1.0f / (keys.size() - 1);
		std::vector<raylib::Color> colors;

		for(int i = 0; i < samples; i++)
		{
			float t = step * i;
			float kt = 0.0f;
			int index = -1;
			for(int k = 0; k < keys.size(); k++)
			{
				if(t <= kt)
				{
					index = k;
					break;
				}
				kt += keyStep;
			}
			index = std::clamp(index - 1, 0, static_cast<int>(keys.size() - 2));
			float lt = (t - (index * keyStep)) / keyStep;

			raylib::Color c = Lerp(keys[index], keys[index + 1], lt);
			colors.push_back(c);
		}

		return colors;
	}

	static raylib::Color HSVToRGB(float h, float s, float v)
	{
		h *= 6;
		int i = static_cast<int>(std::floor(h));
		float f = h - i;
		float p = v * (1.0f - s);
		float q = v * (1.0f - s * f);
		float t = v * (1.0f - s * (1.0f - f));
		float r, g, b;
		switch(i % 6)
		{
			case 0: r = v; g = t, b = p;
				break;
			case 1: r = q; g = v, b = p;
				break;
			case 2: r = p; g = v, b = t;
				break;
			case 3: r = p; g = q, b = v;
				break;
			case 4: r = t; g = p, b = v;
				break;
			case 5: r = v; g = p, b = q;
				break;
		}
		return raylib::Color { static_cast<unsigned char>(std::floor(r * 255)), static_cast<unsigned char>(std::floor(g * 255)), static_cast<unsigned char>(std::floor(b * 255)), 255 };
	}

	static std::vector<raylib::Color> GetPreset(GradientPreset preset)
	{
		std::vector<raylib::Color> result = std::vector<raylib::Color>();
		if(static_cast<int>(preset) >= static_cast<int>(GradientPreset::Random) && static_cast<int>(preset) <= static_cast<int>(GradientPreset::Random_5))
		{
			static std::random_device rd;
			static std::default_random_engine randEngine(rd());
			static std::uniform_real_distribution<float> dist = std::uniform_real_distribution<float>(0.0f, 1.0f);
			if(preset == GradientPreset::Random)
			{
				preset = static_cast<GradientPreset>(static_cast<int>(GradientPreset::Random) + static_cast<int>(std::round(dist(randEngine) * static_cast<int>(GradientPreset::Random_5))));
			}
			int amount = static_cast<int>(preset) + 1;
			for(int i = 0; i < amount; i++)
			{
				result.push_back(HSVToRGB(dist(randEngine), 0.5f + dist(randEngine) * 0.5f, 0.5f + dist(randEngine) * 0.5f));
			}
		}
		else
		{
			const std::vector<uint32_t>& colors = GRADIENT_PRESETS.at(preset);
			for(uint32_t color : colors)
			{
				unsigned char r = (color >> 16) & 0xFF;
				unsigned char g = (color >> 8) & 0xFF;
				unsigned char b = (color >> 0) & 0xFF;
				result.push_back(raylib::Color { r, g, b, 255 });
			}
		}
		return result;
	}
}