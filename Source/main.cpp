#include "pch.h"
#include "App.h"

int main(int argc, char *argv[])
{
	try
	{
		Space::App spaceBlaster;
		spaceBlaster.Run();
	}
	catch(const std::exception& e)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Fatal Exception", e.what(), nullptr);
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
