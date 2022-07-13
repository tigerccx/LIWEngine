#include "TextConsole.h"

void LIW::App::TextConsole::Print(std::string str)
{
	std::cout << str;
}

void LIW::App::TextConsole::PrintLine(std::string str)
{
	std::cout << str << std::endl;
}

