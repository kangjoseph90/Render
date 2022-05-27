#pragma once

#include "linalg.h"
#include "object.h"
#include <thread>
#include <vector>
#include <mutex>

using std::thread;
using namespace std;



struct physics {


	vector<PY> polygons;

	BOOL available = true;

	void quit(void) {
		available = false;
	}

	void run() {

	}

};


void model_loop(physics& model, double fps,mutex& m) {

	while (model.available) {

		m.lock();
		//run dt
		m.unlock();

		this_thread::sleep_for(chrono::milliseconds((long long)(1e3 / fps)));
	}
}


