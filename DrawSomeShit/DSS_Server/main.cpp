#include "SDL.h"
#include "SDL_net.h"
#include <iostream>

int main(int argc, char * argv[])
{
	if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_EVENTS) != 0)
	{
		std::cout << "SDL_Init() failed!";
		exit(-1);
	}

	if (SDLNet_Init() == -1)
	{
		std::cout << "SDLNet_Init() failed!";
		exit(-1);
	}

	SDLNet_Quit();
	SDL_Quit();
	return 0;
}