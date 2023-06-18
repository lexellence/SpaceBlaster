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
		d2d::Window::ShowSimpleMessageBox(d2d::MessageBoxType::D2D_ERROR, "Fatal Exception"s, e.what());
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
