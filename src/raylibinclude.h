//To fix duplicate definition errors when including in multiple translation units
//Implement once with RAYGUI_IMPLEMENTATION defined and otherwise with RAYGUI_STATIC
//https://github.com/raysan5/raygui/issues/108
//https://github.com/raysan5/raygui/issues/62

#pragma once
//Required in global namespace for the raylib & raygui includes
#include <cstdlib>
#include <cstring>

//For layout extension
#include <utility>
#include <algorithm>

extern "C"
{
	namespace raylib
	{
		#include "raylib.h"
		namespace gui
		{			
			#include "raygui.h"
			#include "../styles/dark/dark.h"

			static void LoadDefaultStyle()
			{
				GuiLoadStyleDark();
			}

			static int CalcTextWidth(const char* text)
			{
				int fontSize = GuiGetStyle(DEFAULT, TEXT_SIZE);
				int size = MeasureText(text, fontSize);
				return size;
			}

			class ScopedStyle
			{
			public:
				ScopedStyle(bool enabled, GuiControl control, GuiControlProperty prop, int value) : control(control), prop(prop)
				{
					prevValue = GuiGetStyle(control, prop);
					if(enabled)
					{
						GuiSetStyle(control, prop, value);
					}
				}

				~ScopedStyle()
				{
					GuiSetStyle(control, prop, prevValue);
				}
				
			private:
				GuiControl control;
				GuiControlProperty prop;
				int prevValue;

				void* operator new(size_t) = delete;
				void* operator new(size_t, void*) = delete;
				void* operator new[](size_t) = delete;
				void* operator new[](size_t, void*) = delete;
			};
		}
	}
}

namespace raylib
{
	namespace gui
	{
		namespace layout
		{
			class VerticalLayout
			{
			private:
				float x = 0.0f;
				float y = 0.0f;
				float width = 0.0f;
				float height = 0.0f;
				float margin = 0.0f;

			public:
				VerticalLayout(float x, float width, float height, float margin) : x(x), y(0.0f), width(width), height(height), margin(margin)
				{

				}

				std::pair<Rectangle, Rectangle> SplitHorizontal(Rectangle rect, float percentage = 0.5f)
				{
					percentage = std::clamp(percentage, 0.0f, 1.0f);
					return std::pair<Rectangle, Rectangle>(
						Rectangle { rect.x, rect.y, rect.width * percentage, rect.height },
						Rectangle { rect.x + rect.width * percentage, rect.y, rect.width * (1.0f - percentage), rect.height }
					);
				}

				Rectangle GetNextLayoutRect(float height = 0.0f)
				{
					float h = height != 0 ? height : this->height;
					Rectangle r = { x, y, width, h };
					y += h + margin;
					return r;
				}

				void Space(float space = 5.0f)
				{
					y += space;
				}
			};
		}
	}
}