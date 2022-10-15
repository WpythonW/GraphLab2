#include "screen.h"
#include <numeric>
#include <iostream>

using namespace std;

struct vec3
{
	bool operator!=(vec3& other) const
	{
		if (&this->x != &other.x && &this->y != &other.y && &this->z != &other.z) return true; //This is the pointer for 
		else return false;
	}
	float x, y, z;
};

struct connection
{
	int a, b;
};

void rotate(vec3& point, float x = 1, float y = 1, float z = 1)
{
	float rad = 0;

	rad = x;
	point.y = cos(rad) * point.y - sin(rad) * point.z;
	point.z = sin(rad) * point.y + cos(rad) * point.z;

	rad = y;
	point.x = cos(rad) * point.x + sin(rad) * point.z;
	point.z = -sin(rad) * point.x + cos(rad) * point.z;

	rad = z;
	point.x = cos(rad) * point.x - sin(rad) * point.y;
	point.y = sin(rad) * point.x + cos(rad) * point.y;
}

void line(Screen& screen, float x1, float y1, float x2, float y2)
{
	float dx = x2 - x1;
	float dy = y2 - y1;

	float length = sqrt(dx * dx + dy * dy);
	float angle = atan2(dy, dx);

	for (float i = 0; i < length; i++)
	{
		screen.pixel(x1 + cos(angle) * i, y1 + sin(angle) * i);
	}
}

class ShapeMatrix
{
	float m[4][10];
	vector<int> side;
	public:

	void plane(vec3 P, vec3 Q, vec3 R, int i)
	{
		float x1, x2, x3, y1, y2, y3, z1, z2, z3, A, B, C, D;
		x1 = P.x; x2 = Q.x; x3 = R.x;
		y1 = P.y; y2 = Q.y; y3 = R.y;
		z1 = P.z; z2 = Q.z; z3 = R.z;
		A = y1 * (z2 - z3) + y2 * (z3 - z1) + y3 * (z1 - z2);
		B = z1 * (x2 - x3) + z2 * (x3 - x1) + z3 * (x1 - x2);
		C = x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2);
		D = -(x1 * (y2 * z3 - y3 * z2) + x2 * (y3 * z1 - y1 * z3) + x3 * (y1 * z2 - y2 * z1));
		m[0][i] = A;
		m[1][i] = B;
		m[2][i] = C;
		m[3][i] = D;
	}

	void generator(vector<vec3> points)
	{
		/*vector<vec3> points
		{
			{0.5, 0.5, -0.5},
			{0.5, -0.5, -0.5},
			{-0.5, -0.5, -0.5},
			{-0.5, 0.5, -0.5},

			{0.5, 0.5, 0.5},
			{0.5, -0.5, 0.5},
			{-0.5, -0.5, 0.5},
			{-0.5, 0.5, 0.5},
		};*/

		/*vector<vec3> points
		{
			{R, -h, 0},
			{RCos, -h, RSin},
			{0, -h, R},
			{-RCos, -h, RSin},
			{-R, -h, 0},
			{-RCos, -h, -RSin},
			{0, -h, -R},
			{RCos, -h, -RSin},

			{R, h, 0},
			{RCos, h, RSin},
			{0, h, R},
			{-RCos, h, RSin},
			{-R, h, 0},
			{-RCos, h, -RSin},
			{0, h, -R},
			{RCos, h, -RSin},
		};*/


		for (int i = 1; i < 8; i++)
		{
			vec3 P = points[i - 1];
			vec3 Q = points[i];
			vec3 R = points[i + 7];
			plane(P, Q, R, i-1);
		}
		plane(points[7], points[0], points[15], 7);
		plane(points[0], points[1], points[2], 8);
		plane(points[8], points[9], points[10], 9);

	}

	
	//если точка не принадлежит видимой плоскости, попадает в чс
	//P = {R + x, -h + y, 0}
	void eye(vec3 P)
	{
		side.clear();
		//vector<float> P = { 1 / 4, 1 / 4, 1 / 4, 1 };
		float x, y, z;
		x = P.x;
		y = P.y;
		z = P.z;
		vector<float> S;
		for (int i = 0; i < 10; i++)
		{
			S.push_back(m[0][i] * x + m[1][i] * y + m[2][i] * z + m[3][i] * 1);
		}

		float m2[4][10];

		for (int i = 0; i < 10; i++)
		{
			if (S[i] < 0)
			{
				m2[0][i] = -m[0][i];
				m2[1][i] = -m[1][i];
				m2[2][i] = -m[2][i];
				m2[3][i] = -m[3][i];
			}
			else
			{
				m2[0][i] = m[0][i];
				m2[1][i] = m[1][i];
				m2[2][i] = m[2][i];
				m2[3][i] = m[3][i];
			}
		}

		for (int i = 0; i < 10; i++)
		{
			if (m2[2][i] > 0)
			{
				side.push_back(i);
			}
		}
		
	}

	bool check(vec3 P)
	{
		int c = 0;
		float A, B, C, D, x, y, z;
		bool in;
		x = P.x;
		y = P.y;
		z = P.z;
		for (auto& i : side)
		{
			A = m[0][i];
			B = m[1][i];
			C = m[2][i];
			D = m[3][i];
			
			if (A * x + B * y + C * z + D == 0)
			{
				c++;
			}
		}

		if (c == 0)
		{
			return true;	//не принадлежит ни одной видимой плоскости
		}
		return false;
	}
};

float leng(vec3 p, float R, float x, float y)
{
	float l = sqrt((p.x + x) * (p.x + x) + (p.y + y) * (p.y + y)  + (p.z + 2 * R) * (p.z + 2 * R));
	return l;
}

int main(int argc, char* argv[])
{
	Screen screen(1280, 960);
	//ShapeMatrix matrix;
	float longest;
	float R = (screen.W * 0.078) / (2 * sin(M_PI / 8));
	float RCos = R * cos(M_PI / 4);
	float RSin = R * sin(M_PI / 4);
	//float h = screen.W * 0.15;
	float h = screen.W * 0.15/2;

	float x = screen.W/4;
	float y = screen.H/4;
	// Ось Z смотрит на зрителя
	vector<vec3> points
	{
		{R, 0, -h},
		{RCos, RSin, -h},
		{0, R, -h}, 
		{-RCos, RSin, -h},
		{-R, 0, -h},
		{-RCos, -RSin, -h},
		{0, -R, -h},
		{RCos, -RSin, -h},

		{R, 0, h},
		{RCos, RSin, h},
		{0, R, h},
		{-RCos, RSin, h},
		{-R, 0, h},
		{-RCos, -RSin, h},
		{0, -R, h},
		{RCos, -RSin, h},

		//{x, y, -2*h},
		//{x, y, 3*h},
	};

	/*vector<vec3> points
	{
		{R + x, -h + y, 0},
		{RCos + x, -h + y, RSin},
		{x, -h + y, R},
		{-RCos + x, -h + y, RSin},
		{-R + x, -h + y, 0},
		{-RCos + x, -h + y, -RSin},
		{x, -h + y, -R},
		{RCos + x, -h + y, -RSin},

		{R + x, h + y, 0},
		{RCos + x, h + y, RSin},
		{x, h + y, R},
		{-RCos + x, h + y, RSin},
		{-R + x, h + y, 0},
		{-RCos + x, h + y, -RSin},
		{x, h + y, -R},
		{RCos + x, h + y, -RSin},
	};*/

	/*vector<vec3> points
	{
		{R, -h, 0},
		{RCos, -h, RSin},
		{0, -h, R},
		{-RCos, -h, RSin},
		{-R, -h, 0},
		{-RCos, -h, -RSin},
		{0, -h, -R},
		{RCos, -h, -RSin},

		{R, h, 0},
		{RCos, h, RSin},
		{0, h, R},
		{-RCos, h, RSin},
		{-R, h, 0},
		{-RCos, h, -RSin},
		{0, h, -R},
		{RCos, h, -RSin},
	};*/

	vector<connection> connections
	{
		{0, 1},
		{1, 2},
		{2, 3},
		{3, 4},
		{4, 5},
		{5, 6},
		{6, 7},
		{7, 0},

		{8, 9},
		{9, 10},
		{10, 11},
		{11, 12},
		{12, 13},
		{13, 14},
		{14, 15},
		{15, 8},

		{0, 8},
		{1, 9},
		{2, 10},
		{3, 11},
		{4, 12},
		{5, 13},
		{6, 14},
		{7, 15},
	};

	//matrix.generator(points);
	//matrix.eye({ 0, 0, 0 });
	

	vec3 c{0, 0, 0};
	for (auto& p : points)
	{
		p.x += x;
		p.y += y;
		c.x += p.x;
		c.y += p.y;
		c.z += p.z;
	}
	c.x /= points.size();
	c.y /= points.size();
	c.z /= points.size();


	int I, C, _C, C_;
	while(true)
	{
		vector<int> inds;

		for(auto& p : points)
		{
			if (screen.scaled)
			{
				p.x = ((p.x - x) / screen.prevScale) * screen.scale + x;
				p.y = ((p.y - y) / screen.prevScale) * screen.scale + y;
				p.z = p.z / screen.prevScale * screen.scale;
			}
			p.x -= c.x;
			p.y -= c.y;
			p.z -= c.z;
			rotate(p, screen.xR, screen.yR, 0.000);
			p.x += c.x;
			p.y += c.y;
			p.z += c.z;
			//screen.pixel(p.x, p.y);
		}
		//cout << points[2].x << " " << points[2].y << " " << points[2].z << "|" << points[14].x << " " << points[14].y << " " << points[14].z << "\n";
		float K = _MM_FIXUP_MAX_FLOAT;
		for (int i = 0; i < 16; i++)
		{
			if (K > points[i].z)
			{
				K = points[i].z;
				I = i;
			}
		}

		for (auto& conn : connections)
		{
			if (I == 7)
			{
				_C = 6;
				C = 7;
				C_ = 0;
			}
			else if (I == 0)
			{
				_C = 7;
				C = 0;
				C_ = 1;
			}
			else if (I == 8)
			{
				_C = 15;
				C = 8;
				C_ = 9;
			}
			else if (I == 15)
			{
				_C = 14;
				C = 15;
				C_ = 8;
			}
			else 
			{
				_C = I - 1;
				C = I;
				C_ = I + 1;
			}

			if (points[conn.a] != points[C] && points[conn.a] != points[_C] && points[conn.a] != points[C_] && points[conn.b] != points[C] && points[conn.b] != points[_C] && points[conn.b] != points[C_])
			{
				line(screen, points[conn.a].x,
					points[conn.a].y,
					points[conn.b].x,
					points[conn.b].y);
			}
		}

		screen.show();
		screen.clear();
		screen.input();
		SDL_Delay(30);
		//matrix.generator(points);
		//matrix.eye({R - 1, -h + 1, 0});
		//matrix.eye({ 0, 0, 0 });
	}
	
	return 0;
}