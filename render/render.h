#pragma once

#include "model.h"
#include "resource.h"
#include "linalg.h"
#include "framework.h"
#include <vector>


struct camera {

	v3 pos = v3(0, 0, 0), aim = v3(1, 0, 0);
	double x_fov = (double)48 * PI / 180, y_fov = (double)27 * PI / 180;
	
	double speed = 20, rotate_speed = 1;
	
	POINT v3_to_PT(v3 v,int WIDTH,int HEIGHT) { 
		POINT ret;
		double phi_x = -angle(aim, v, rotate_xy(aim, PI / 2));
		double phi_y = -angle(aim, v, rotate_z(v3(aim.x, aim.y, 0), -PI / 2));
		ret.x = (int)((phi_x / x_fov + 1) * WIDTH / 2);
		ret.y = (int)((phi_y / y_fov + 1) * HEIGHT / 2);
		return ret;
	}
};

struct renderstruct {


	HWND hWnd;
	camera cam;
	vector<polygon> p;
	vector<v3> light;
	
	int WIDTH, HEIGHT;

	BOOL available = true;

	renderstruct(int WIDTH, int HEIGHT) {
		this->WIDTH = WIDTH;
		this->HEIGHT = HEIGHT;
	}

	void on_resize(int WIDTH, int HEIGHT) {
		this->WIDTH = WIDTH;
		this->HEIGHT = HEIGHT;
	}

	void assign_window(HWND hWnd) {
		this->hWnd = hWnd;
	}

	void render(void) {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		HGDIOBJ original = NULL;
		original = SelectObject(hdc, GetStockObject(DC_PEN));

		SelectObject(hdc, GetStockObject(BLACK_PEN));
		SelectObject(hdc, GetStockObject(GRAY_BRUSH));

		for (auto poly : p) {
			if (dot(poly.dir, poly.p[0] - cam.pos) > 0) continue; //back face culling
			POINT points[3];
			points[0] = cam.v3_to_PT(poly.p[0] - cam.pos, WIDTH, HEIGHT);
			points[1] = cam.v3_to_PT(poly.p[1] - cam.pos, WIDTH, HEIGHT);
			points[2] = cam.v3_to_PT(poly.p[2] - cam.pos, WIDTH, HEIGHT);
			Polygon(hdc, points, 3);
		}



		SelectObject(hdc, original);
		EndPaint(hWnd, &ps);
	}

	void move_camera(WPARAM wParam) {
		printf_s("%c\n", wParam);
		int c = 10;
		switch (wParam)
		{	
			case VK_UP: cam.aim = rotate_xy(cam.aim, cam.rotate_speed / c);
				break;
			case VK_DOWN: cam.aim = rotate_xy(cam.aim, -cam.rotate_speed / c);
				break;
			case VK_LEFT: cam.aim = rotate_z(cam.aim, cam.rotate_speed / c);
				break;
			case VK_RIGHT: cam.aim = rotate_z(cam.aim, -cam.rotate_speed / c);
				break;	
			case VK_SPACE: cam.pos = cam.pos + v3(0, 0, 1) * cam.speed / c;
				break;
			case VK_SHIFT: cam.pos = cam.pos - v3(0, 0, 1) * cam.speed / c;
				break;
			case 'W': cam.pos = cam.pos + cam.aim.unit() * cam.speed/c;
				break;
			case 'A': cam.pos = cam.pos - cross(cam.aim, rotate_xy(cam.aim, PI / 2)).unit() * cam.speed/c;
				break;
			case 'S': cam.pos = cam.pos - cam.aim.unit() * cam.speed/c;
				break;
			case 'D': cam.pos = cam.pos + cross(cam.aim, rotate_xy(cam.aim, PI / 2)).unit() * cam.speed/c;
				break;
		}
	}

	void quit() {
		available = false;
	}

};

void render_loop(renderstruct *rs, int fps) {
	
	while (rs->available) {
		InvalidateRect(rs->hWnd, NULL,TRUE);
		this_thread::sleep_for(chrono::milliseconds((long long)1e3 / fps));
		printf_s("aim : (%.2f, %.2f, %.2f)\n", rs->cam.aim.x, rs->cam.aim.y, rs->cam.aim.z);
	}
}