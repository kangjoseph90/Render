#pragma once

#include "linalg.h"

using namespace std;

struct PY {
	v3 p[3], dir;
	double kappa; //���

	void set_dir(void) {
		dir = cross(p[0] - p[1], p[2] - p[1]).unit(); //clockwise -> front
	}
};