#pragma once

#include "model.h"
#include "resource.h"
#include "linalg.h"
#include "object.h"
#include "framework.h"
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <vector>
#include <set>
#include <mutex>
#include <thread>

struct camera {

	v3 pos = zero, aim = x_axis;
	double x_fov = (double)48 * PI / 180, y_fov = (double)27 * PI / 180, view_distance = 1000;

	double speed = 50, rotate_speed = 1;

	PY view_space[6];

	camera(void) {
		update_view_space();
	}

	POINT v3_to_PT(v3 v, int WIDTH, int HEIGHT) {
		POINT ret;
		double phi_x = -angle(aim, v, rotate_xy(aim, PI / 2));
		double phi_y = -angle(aim, v, rotate_z(v3(aim.x, aim.y, 0), -PI / 2));
		ret.x = (int)((phi_x / x_fov + 1) * WIDTH / 2);
		ret.y = (int)((phi_y / y_fov + 1) * HEIGHT / 2);
		return ret;
	}

	void update_view_space(void) {
		v3 axis = aim.unit() * view_distance;
		v3 tl = rotate_z(rotate_xy(axis, y_fov), x_fov),
			tr = rotate_z(rotate_xy(axis, y_fov), -x_fov),
			bl = rotate_z(rotate_xy(axis, -y_fov), x_fov),
			br = rotate_z(rotate_xy(axis, -y_fov), -x_fov);
		view_space[0] = PY(pos, tl, bl);
		view_space[1] = PY(pos, bl, br);
		view_space[2] = PY(pos, br, tr);
		view_space[3] = PY(pos, tr, tl);
		view_space[4] = PY(tl, bl, br);
		view_space[5] = PY(tl, br, tr);
	}

	BOOL frustrum_culling(PY P) {
		if (norm(P.mid() - pos) > view_distance) return TRUE;
		for (auto& point : P.p) {
			v3 v = point - pos;
			double phi_x = -angle(aim, v, rotate_xy(aim, PI / 2));
			double phi_y = -angle(aim, v, rotate_z(v3(aim.x, aim.y, 0), -PI / 2));
			if (abs(phi_x) < x_fov && abs(phi_y) < y_fov) return FALSE;
		}
		for (auto& view_plane : view_space) {
			if (COLL::PYwPY(P, view_plane)) return FALSE;
		}
		return TRUE;
	}

	BOOL back_space_culling(PY P) {
		if (dot(P.dir, P.p[0] - pos) > 0) return TRUE;
		return FALSE;
	}

};

struct renderstruct {


	HWND hWnd;
	camera cam;
	physics* model;
	
	set<WPARAM> movement;
	int WIDTH, HEIGHT;

	BOOL available = TRUE;

	//±¤¿ø
	double ambient_light = 100;
	v3 directional_light = v3(1, -1, 1);
	double directional_light_intensity = 50;
	v3 point_light = v3(-200, 200, 200);
	double point_light_intensity=50;


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

	unsigned int intensity(PY P, v3 pos) { //polygon P À§ÀÇ Á¡ pos¿¡¼­ÀÇ ¹à±â
		double ret = ambient_light;
		ret += max(0, cos(P.dir, directional_light) * directional_light_intensity);
		ret += max(0, cos(P.dir, point_light - pos) * point_light_intensity);
		return (unsigned int)ret;
	}


	

	void render(void) {

		HDC hdc = GetDC(hWnd);
		
		double* Z = new double[WIDTH * HEIGHT]; //Z_value
		unsigned int* I = new unsigned int[WIDTH * HEIGHT]; //Intensity
		for (int i = 0; i < WIDTH * HEIGHT; i++) {
			*(Z + i) = 1;
			*(I + i) = 0xFF000000;
		}
		
		v3 axis = cam.aim.unit() * cam.view_distance;
		for (int i = 0; i < WIDTH; i++) {
			for (int j = 0; j < HEIGHT; j++) {
				v3 now_ray = rotate_xy(rotate_z(axis, ((double) - 2 * i / WIDTH + 1) * cam.x_fov), ((double)-2 * j / HEIGHT + 1) * cam.y_fov);
				LINE now_line = LINE(cam.pos, cam.pos + now_ray);
				for (auto &polygon : model->polygons) {
					if (cam.back_space_culling(polygon)) continue;
					if (!COLL::PYwLINE(polygon, now_line)) continue;
					v3 COLLv3 = COLL::PYwLINE_COLLv3(polygon, now_line);
					double now_Z = norm(COLLv3 - cam.pos) / cam.view_distance;
					if (now_Z < *(Z + i + j * WIDTH)) {
						*(I + i + j * WIDTH) = intensity(polygon, COLLv3) * 0x00010101 + 0xFF000000;
						*(Z + i + j * WIDTH) = now_Z;
					}
				}
			}
		}

		HDC hMemDC = CreateCompatibleDC(hdc);
		HBITMAP hBitmap = CreateBitmap(WIDTH, HEIGHT, 1, 32, I);
		HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);

		delete[] I;
		delete[] Z;
		BitBlt(hdc, 0, 0, WIDTH, HEIGHT, hMemDC, 0, 0, SRCCOPY);
		SelectObject(hMemDC, hOldBitmap);
		DeleteDC(hMemDC);
		DeleteObject(hBitmap);
		ReleaseDC(hWnd, hdc);

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
		for (auto &wParam : movement) {
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
		cam.update_view_space();
	}

	void quit() {
		available = FALSE;
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