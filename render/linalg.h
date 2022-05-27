#pragma once

#include <cmath>
using namespace std;

//R^3 벡터, R^(3x3) 행렬 모듈

const double PI = 3.141592653589793238462643;

struct v3 {
	double x, y, z;
	v3(void) {
		x = y = z = 0;
	}
	v3(double x, double y, double z) {
		this->x = x, this->y = y, this->z = z;
	}
	v3(double arr[3]) {
		x = arr[0], y = arr[1], z = arr[2];
	}
	double norm(void) {
		return sqrt(x * x + y * y + z * z);
	}
	v3 operator+(v3 op) {
		return { x + op.x,y + op.y,z + op.z };
	}
	v3 operator-(v3 op) {
		return { x - op.x,y - op.y,z - op.z };
	}
	v3 operator*(double op) {
		return { op * x,op * y,op * z };
	}
	v3 operator/(double op) {
		return { x / op,y / op,z / op };
	}
	void operator+=(v3 op) {
		*this = *this + op;
	}
	void operator*=(double op) {
		*this = *this * op;
	}
	void operator/=(double op) {
		*this *= (1 / op);
	}
	v3 unit(void) {
		return *this / this->norm();
	}
};

v3 zero(0,0,0),
		x_axis(1, 0, 0),
		y_axis(0, 1, 0), 
		z_axis(0, 0, 1);

v3 operator*(double op, v3 v) {
	return v * op;
}

double norm(v3 v) {
	return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}
double dot(v3 v1, v3 v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}
v3 cross(v3 v1, v3 v2) {
	return { v1.y * v2.z - v1.z * v2.y,v1.z * v2.x - v1.x * v2.z,v1.x * v2.y - v1.y * v2.x };
}
v3 proj(v3 b, v3 a) {
	return (dot(a, b) / dot(a, a)) * a;
}
double angle(v3 v1, v3 v2,v3 axis) { //signed angle (right hand) 
	axis = axis.unit();
	return atan2(dot(cross(v1, v2), axis), dot(v1, v2));
}


struct mat3 { //3x3 matrix
	double mat[3][3];

	mat3(double arr[3][3]) {
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				mat[i][j] = arr[i][j];
			}
		}
	}
	double det(void) {
		return mat[0][0] * mat[1][1] * mat[2][2] +
			mat[1][0] * mat[2][1] * mat[0][2] +
			mat[2][0] * mat[0][1] * mat[1][2] -
			mat[0][2] * mat[1][1] * mat[2][0] -
			mat[1][2] * mat[2][1] * mat[0][0] -
			mat[2][2] * mat[0][1] * mat[1][0];
	}
	mat3 transpose(void) {
		double arr[3][3];
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				arr[i][j] = mat[j][i];
			}
		}
		return mat3(arr);
	}
	v3 operator*(v3 op) {
		double arr[3];
		for (int i = 0; i < 3; i++) {
			arr[i] = mat[i][0] * op.x + mat[i][1] * op.y + mat[i][2] * op.z;
		}
		return v3(arr);
	}
	mat3 operator*(mat3 op) {
		double arr[3][3];
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				arr[i][j] = 0;
				for (int k = 0; k < 3; k++) {
					arr[i][j] += mat[i][k] * op.mat[k][j];
				}
			}
		}
		return mat3(arr);
	}
	mat3 operator*(double op) {
		double arr[3][3];
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				arr[i][j] = op * mat[i][j];
			}
		}
		return mat3(arr);
	}
	mat3 operator/(double op) {
		return *this * (1 / op);
	}
	void operator*=(mat3 op) {
		*this = *this * op;
	}
	void operator*=(double op) {
		*this = *this * op;
	}
	mat3 inverse(void) {
		mat3 temp = this->transpose();
		double arr[3][3];
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				arr[i][j] = temp.sub_det(i, j);
			}
		}
		return mat3(arr) / this->det();
	}
	double sub_det(int r, int c) {
		double temp[2][2];
		int now_r = 0, now_c = 0;
		for (int i = 0; i < 3; i++) {
			if (i == r) {
				now_r++;
				continue;
			}
			now_c = 0;
			for (int j = 0; j < 3; j++) {
				if (j == c) {
					now_c++;
					continue;
				}
				temp[now_r][now_c] = this->mat[i][j];
			}
		}
		return temp[0][0] * temp[1][1] - temp[0][1] * temp[1][0];
	}
};

mat3 operator*(double op, mat3 m) {
	return m * op;
}

mat3 identity(void){
	double arr[3][3] = { {1,0,0},{0,1,0},{0,0,1} };
	return mat3(arr);
}

v3 rotate_z(v3 v,double theta) {
	double arr[3][3] = { {cos(theta),-sin(theta),0},{sin(theta),cos(theta),0},{0,0,1} };
	v3 temp = mat3(arr) * v;
	return mat3(arr) * v;
}

v3 rotate_xy(v3 v, double theta) {
	v3 p_xy = v3(v.x, v.y, 0); //xy 평면 projection
	double phi = angle(x_axis,p_xy,v3(0,0,1)); //x축과 이루는 각 phi
	v3 temp = rotate_z(v, -phi); //x축에 나란하도록 -phi만큼 z축 회전
	double arr[3][3] = { {cos(theta),0,-sin(theta)},{0,1,0},{sin(theta),0,cos(theta)} }; 
	temp = mat3(arr) * temp; //theta만큼 y축 회전
	mat3 temp2 = mat3(arr);
	return rotate_z(temp, phi); //phi만큼 z축 회전
}


