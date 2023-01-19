#pragma once
#include <type_traits>
#include <vector>
#include <cmath>

#include "config.h"
#include "grid3d.h"
#define RAYGUI_STATIC
#include "raylibinclude.h"

template<typename T>
class Renderer
{
	static_assert(std::is_base_of<Cell, T>::value, "T must derive from Cell");

public:
	Renderer()
	{
		cam = raylib::Camera();
		cam.position = raylib::Vector3 { 15.0f * 3, 15.0f * 3, 15.0f * 3 };
		cam.target = raylib::Vector3 { 0.0f, 0.0f, 0.0f };
		cam.up = raylib::Vector3 { 0.0f, 1.0f, 0.0f };
		cam.fovy = RENDERER_FOV;
		cam.projection = raylib::CameraProjection::CAMERA_PERSPECTIVE;
		raylib::SetCameraMode(cam, raylib::CameraMode::CAMERA_ORBITAL);
	}

	void Update()
	{
		raylib::UpdateCamera(&cam);
	}

	void Render(const Grid3d<T>& grid, const DynamicSimSettings& settings, const std::vector<raylib::Color>& gradient)
	{
		void (Renderer::*drawFunc)(float x, float y, float z, const raylib::Color& color);
		switch(settings.renderMode)
		{
			case RenderMode::Cube:
				drawFunc = &Renderer::RenderCube;
				break;
			case RenderMode::Quad:
				drawFunc = &Renderer::RenderQuad;
				break;
			case RenderMode::Point:
				drawFunc = &Renderer::RenderPoint;
				break;
			default:
				throw std::exception("Missing switch label in Renderer<T>::Render!");
				break;
		}

		raylib::Color (Renderer::*colorFunc)(int dimSize, int x, int y, int z, float t, const std::vector<raylib::Color>&gradient);
		switch(settings.colorMode)
		{
			case ColorMode::State:
				colorFunc = &Renderer::ColorState;
				break;
			case ColorMode::Xyz:
				colorFunc = &Renderer::ColorXYZ;
				break;
			case ColorMode::Radius:
				colorFunc = &Renderer::ColorRadius;
				break;
			default:
				throw std::exception("Missing switch label in Renderer<T>::Render!");
				break;
		}

		raylib::BeginMode3D(cam);

		int dimSize = grid.GetDimSize();
		float offset = -dimSize * 0.5f;

		raylib::DrawBoundingBox(raylib::BoundingBox { raylib::Vector3 { 0.0f + offset, 0.0f + offset, 0.0f + offset }, raylib::Vector3 { dimSize + offset, dimSize + offset, dimSize + offset } }, BOUNDS_COLOR);

		int len = grid.GetDimSize() * grid.GetDimSize() * grid.GetDimSize();
		for(int i = 0; i < len; i++)
		{
			const T& cell = grid[i];
			if(!cell.IsEmpty())
			{
				auto [x, y, z] = grid.GetCellPos(i);
				raylib::Color c = (this->*colorFunc)(dimSize, x, y, z, cell.RenderGradient(), gradient);
				(this->*drawFunc)(x + offset + 0.5f, y + offset + 0.5f, z + offset + 0.5f, c);
			}
		}

		raylib::EndMode3D();
	}

	void RenderCube(float x, float y, float z, const raylib::Color& color)
	{
		raylib::DrawCube(raylib::Vector3 { x, y, z }, 1.0f, 1.0f, 1.0f, color);
	}

	void RenderQuad(float x, float y, float z, const raylib::Color& color)
	{
		static uint8_t image2x2White[4] = { 255, 255, 255, 255 };
		static raylib::Texture2D tex = raylib::LoadTextureFromImage(raylib::Image { &image2x2White, 2, 2, 1, raylib::PixelFormat::PIXELFORMAT_UNCOMPRESSED_GRAYSCALE });

		raylib::DrawBillboard(cam, tex, raylib::Vector3 { x, y, z }, 1.0f, color);
	}

	void RenderPoint(float x, float y, float z, const raylib::Color& color)
	{
		raylib::DrawLine3D(raylib::Vector3 { x - 0.1f, y, z }, raylib::Vector3 { x + 0.1f, y, z }, color);
		raylib::DrawLine3D(raylib::Vector3 { x, y - 0.1f, z }, raylib::Vector3 { x, y + 0.1f, z }, color);
		raylib::DrawLine3D(raylib::Vector3 { x, y, z - 0.1f }, raylib::Vector3 { x, y, z + 0.1f }, color);
	}

	raylib::Color ColorState(int dimSize, int x, int y, int z, float t, const std::vector<raylib::Color>& gradient)
	{
		return gradient[static_cast<int>(std::floor(t * (gradient.size() - 1)))];
	}

	raylib::Color ColorXYZ(int dimSize, int x, int y, int z, float t, const std::vector<raylib::Color>& gradient)
	{
		float dimSizeMinusOne = dimSize - 1.0f;
		unsigned char r = static_cast<unsigned char>(std::floor(x / dimSizeMinusOne * 255.0f));
		unsigned char g = static_cast<unsigned char>(std::floor(y / dimSizeMinusOne * 255.0f));
		unsigned char b = static_cast<unsigned char>(std::floor(z / dimSizeMinusOne * 255.0f));
		return raylib::Color { r, g, b, 255 };
	}

	raylib::Color ColorRadius(int dimSize, int x, int y, int z, float t, const std::vector<raylib::Color>& gradient)
	{
		static const float sqrt3 = std::sqrtf(3.0f);
		float center = dimSize * 0.5f;
		t = std::clamp((std::abs(x - center) + std::abs(y - center) + std::abs(z - center)) / (sqrt3 * dimSize * 0.5f), 0.0f, 1.0f);
		return gradient[static_cast<int>(std::floor(t * (gradient.size() - 1)))];
	}

private:
	const raylib::Color BOUNDS_COLOR = { 245, 203, 66, 32 };

	raylib::Camera cam;
};