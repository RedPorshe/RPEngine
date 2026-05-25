#include "Engine.h"
#include <iostream>

using namespace RPE;

Engine::Engine() 
	{
    std::cout << "Initializing RedPorshe Engine	version: " << version() << std::endl;
	}

Engine::~Engine() 
	{
    std::cout << "RedPorshe Engine destroyed!" << std::endl;
	}

void Engine::Run() {}
