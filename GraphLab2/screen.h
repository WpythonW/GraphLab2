#include <SDL.h>
#include <vector>

using namespace std;

class Screen
{
	SDL_Window* window;
	vector<SDL_FPoint> points;	
	SDL_Renderer* renderer;
	SDL_Event e;
	public:
	float xR, yR, prevScale, scale, totalYR, totalXR;
	int H, W;
	bool scaled;
	Screen(int w, int h)
	{
		W = w;
		H = h;
		scale = 1;
		totalXR = 0;
		totalYR = 0;
		xR = 0;
		yR = 0;
		SDL_Init(SDL_INIT_VIDEO);
		window = SDL_CreateWindow("Primitives", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, W, H, SDL_WINDOW_SHOWN);
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
		SDL_RenderSetScale(renderer, 2, 2);

	}

	void pixel(float x, float y)
	{
		SDL_FPoint point;
		point.x = x;
		point.y = y;
		points.emplace_back(point);
	}

	void clear()
	{
		points.clear();
	}

	void show()
	{
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		for (auto& point : points)
		{
			SDL_RenderDrawPointF(renderer, point.x, point.y);
		}

		SDL_RenderPresent(renderer);
	}

	void input()
	{
		scaled = false;
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
			{
				SDL_Quit();
				exit(0);
			}
			else if (e.type == SDL_KEYDOWN)
			{
				switch (e.key.keysym.sym)
				{
					case SDLK_LEFT:  yR = -0.02; break;
					case SDLK_RIGHT: yR = 0.02; break;
					case SDLK_UP:    xR = 0.02; break;
					case SDLK_DOWN:  xR = -0.02; break;
				}

				totalXR += xR;
				totalYR += yR;
			}

			else if (e.type == SDL_KEYUP)
			{
				xR = 0;
				yR = 0;
			}
			else if (e.type == SDL_MOUSEWHEEL)
			{
				if (e.wheel.y > 0) // scroll up
				{
					prevScale = scale;
					scale += 0.02;
				}
				else if (e.wheel.y < 0) // scroll down
				{
					prevScale = scale;
					scale -= 0.02;
				}
				scaled = true;
			}
		}
	}
};