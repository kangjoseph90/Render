#pragma once

#include "linalg.h"
#include <thread>
#include <Windows.h>

using std::thread;
using namespace std;



struct physics {

	BOOL available = true;


};


int model_loop(physics model, int fps) {
	while (TRUE) { //while(!quit message)



		this_thread::sleep_for(chrono::milliseconds((long long)1e3 / fps));

		//run dt

	}
}


