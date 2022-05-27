#pragma once

#include "model.h"
#include "resource.h"
#include "linalg.h"
#include "object.h"
#include "framework.h"
#include <vector>
#include <set>
#include <mutex>
#include <thread>


struct camera {

	v3 pos = zero, aim = x_axis;
	double x_fov = (double)48 * PI / 180, y_fov = (double)27 * PI / 180;
	
	double speed = 50, rotate_speed = 1;
	
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
	physics* model;
	
	set<WPARAM> movement;
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

	void assign_model(physics* model) {
		this->model = model;
	}

	void render(void) {

		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		
		HGDIOBJ original = NULL;
		original = SelectObject(hdc, GetStockObject(DC_PEN));

		SelectObject(hdc, GetStockObject(BLACK_PEN));
		SelectObject(hdc, GetStockObject(GRAY_BRUSH));

		for (auto polygon: model->polygons) {

			//culling
			if (dot(polygon.dir, polygon.p[0] - cam.pos) > 0) continue; //back face culling
			
			
			POINT points[3];
			for (int i = 0; i < 3; i++) {
				points[i] = cam.v3_to_PT(polygon.p[i] - cam.pos, WIDTH, HEIGHT);
			}
			Polygon(hdc, points, 3);
		}

		SelectObject(hdc, original);
		EndPaint(hWnd, &ps);
	}

	void update_movement(WPARAM wParam,bool key_state) {

#ifdef _DEBUG
		printf_s("%c\n", wParam);
#endif

		if (key_state) { //keydown
			movement.insert(wParam);
		}
		else { //keyup
			movement.erase(wParam);
		}
	}

	void move_camera(double fps) {
		for (auto wParam : movement) {
			switch (wParam)
			{
				case VK_UP: cam.aim = rotate_xy(cam.aim, cam.rotate_speed / fps);
					break;
				case VK_DOWN: cam.aim = rotate_xy(cam.aim, -cam.rotate_speed / fps);
					break;
				case VK_LEFT: cam.aim = rotate_z(cam.aim, cam.rotate_speed / fps);
					break;
				case VK_RIGHT: cam.aim = rotate_z(cam.aim, -cam.rotate_speed / fps);
					break;
				case VK_SPACE: cam.pos = cam.pos + z_axis * cam.speed / fps;
					break;
				case VK_SHIFT: cam.pos = cam.pos - z_axis * cam.speed / fps;
					break;
				case 'W': cam.pos = cam.pos + cam.aim.unit() * cam.speed / fps;
					break;
				case 'A': cam.pos = cam.pos - cross(cam.aim, rotate_xy(cam.aim, PI / 2)).unit() * cam.speed / fps;
					break;
				case 'S': cam.pos = cam.pos - cam.aim.unit() * cam.speed / fps;
					break;
				case 'D': cam.pos = cam.pos + cross(cam.aim, rotate_xy(cam.aim, PI / 2)).unit() * cam.speed / fps;
					break;
			}
		}
	}

	void quit() {
		available = false;
	}

};

void render_loop(renderstruct& rs, double fps,mutex& m) {

	while (rs.available) {

		InvalidateRect(rs.hWnd, NULL, TRUE);

		rs.move_camera(fps);

		m.lock();
		rs.render();
		m.unlock();

		this_thread::sleep_for(chrono::milliseconds((long long)(1e3 / fps)));
	
#ifdef _DEBUG
		printf_s("aim  (%.1f, %.1f, %.1f)\n", rs.cam.aim.x, rs.cam.aim.y, rs.cam.aim.z);
#endif
	
	}
}