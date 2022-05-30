#pragma once

#include "linalg.h"

using namespace std;

struct PY {
	v3 p[3], dir;

	PY(void) {
		p[0] = p[1] = p[2] = dir = zero;
	}
	PY(v3 p0, v3 p1, v3 p2) {
		p[0] = p0, p[1] = p1, p[2] = p2;
		this->set_dir();
	}
	PY(v3 p[3]) {
		this->p[0] = p[0], this->p[1] = p[1], this->p[2] = p[2];
		this->set_dir();
	}

	void set_dir(void) {
		dir = cross(p[0] - p[1], p[2] - p[1]).unit(); //clockwise -> front
	}

	v3 mid(void) {
		return (p[0] + p[1] + p[2]) / 3;
	}

};

struct LINE {
	v3 p[2];

	LINE(v3 p0, v3 p1) {
		p[0] = p0, p[1] = p1;
	}
	LINE(v3 p[2]) {
		this->p[0] = p[0], this->p[1] = p[1];
	}

};

namespace COLL {

	v3 PYwLINE_COLLv3(PY P, LINE line) {
		double lambda = dot(P.p[0] - line.p[0], P.dir) / dot(line.p[1] - line.p[0], P.dir);
		return lambda * line.p[1] + (1 - lambda) * line.p[0];
	}

	BOOL PYwLINE(PY P, LINE line) {
		if (dot(P.dir, line.p[0] - P.p[0]) * dot(P.dir, line.p[1] - P.p[0]) > 0) return FALSE;
		double lambda = dot(P.p[0] - line.p[0], P.dir) / dot(line.p[1] - line.p[0], P.dir);
		v3 q = lambda * line.p[1] + (1 - lambda) * line.p[0];
		if (dot(cross(P.p[1] - P.p[0], q - P.p[0]), cross(q - P.p[0], P.p[2] - P.p[0])) > 0 &&
			dot(cross(P.p[0] - P.p[1], q - P.p[1]), cross(q - P.p[1], P.p[2] - P.p[1])) > 0) {
			return TRUE;
		}
		return FALSE;
	}

	BOOL LINEwPY(LINE line, PY P) {
		return PYwLINE(P, line);
	}

	BOOL PYwPY(PY P1, PY P2) {
		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 3; j++) {
				LINE line(P2.p[j], P2.p[(j + 1) % 3]);
				if (PYwLINE(P1, line)) return TRUE;
			}
			PY temp = P1;
			P1 = P2;
			P2 = temp;
		}
		return FALSE;
	}

}