#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <stdlib.h> 
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <math.h>
#include <random>
using namespace std;

#define FIXED_TIMESTEP 0.0166666f
#define MAX_TIMESTEPS 6
float timeLeftOver = 0.0f;

class Vector 
{
public:
	Vector(){};
	Vector(float x, float y, float z) :x(x), y(y), z(z){}
	float length();
	void normalize();
	float x;
	float y;
	float z;
};

class Matrix
{
public:
	union
	{ 
		float m[4][4];
		float ml[16]; 
	};  
	Matrix()
	{
		m[0][0] = 0.0f;
		m[0][1] = 0.0f;
		m[0][2] = 0.0f;
		m[0][3] = 0.0f;
		m[1][0] = 0.0f;
		m[1][1] = 0.0f;
		m[1][2] = 0.0f;
		m[1][3] = 0.0f;
		m[2][0] = 0.0f;
		m[2][1] = 0.0f;
		m[2][2] = 0.0f;
		m[2][3] = 0.0f;
		m[3][0] = 0.0f;
		m[3][1] = 0.0f;
		m[3][2] = 0.0f;
		m[3][3] = 0.0f;
	}
	Matrix(const Matrix& m2)
	{
		for (int i = 0; i <= 3; i++)
		{
			for (int p = 0; p <= 3; p++)
			{
				m[i][p] = m2.m[i][p];
			}
		}
	}
	void identity()
	{
		Matrix tmp;
		tmp.m[0][0] = 1.0f;
		tmp.m[1][1] = 1.0f;
		tmp.m[2][2] = 1.0f;
		tmp.m[3][3] = 1.0f;
		*this = *this*tmp;
	}
	Matrix inverse();
	Matrix operator * (const Matrix& m2);
	Vector operator * (const Vector& v2);
};

Matrix translate(float x, float y)
{
	Matrix tmp;
	tmp.m[0][0] = 1.0f;
	tmp.m[1][1] = 1.0f;
	tmp.m[2][2] = 1.0f;
	tmp.m[3][3] = 1.0f;
	tmp.m[3][0] = x;
	tmp.m[3][1] = y;
	tmp.m[3][2] = 1.0f;
	return tmp;
}

Matrix scale(float x, float y)
{
	Matrix tmp;
	tmp.m[0][0] = x;
	tmp.m[1][1] = y;
	tmp.m[2][2] = 1.0f;
	tmp.m[3][3] = 1.0f;
	return tmp;
}

Matrix rotate(float a)
{
	Matrix tmp;
	tmp.m[0][0] = cos(a);
	tmp.m[1][0] = -sin(a);
	tmp.m[0][1] = sin(a);
	tmp.m[1][1] = cos(a);
	tmp.m[2][2] = 1.0f;
	tmp.m[3][3] = 1.0f;
	return tmp;
}

Vector Matrix::operator *(const Vector& v2)
{
	return Vector(m[0][0] * v2.x + m[0][1] * v2.y + m[0][2] * v2.z, m[1][0] * v2.x + m[1][1] * v2.y + m[1][2] * v2.z, m[2][0] * v2.x + m[2][2] * v2.y + m[2][3] * v2.z);
}

Matrix Matrix::operator * (const Matrix& m2)
{
	float m00 = m2.m[0][0] * m[0][0] + m2.m[0][1] * m[1][0] + m2.m[0][2] * m[2][0] + m2.m[0][3] * m[3][0];
	float m01 = m2.m[0][0] * m[0][1] + m2.m[0][1] * m[1][1] + m2.m[0][2] * m[2][1] + m2.m[0][3] * m[3][1];
	float m02 = m2.m[0][0] * m[0][2] + m2.m[0][1] * m[1][2] + m2.m[0][2] * m[2][2] + m2.m[0][3] * m[3][2];
	float m03 = m2.m[0][0] * m[0][3] + m2.m[0][1] * m[1][3] + m2.m[0][2] * m[2][3] + m2.m[0][3] * m[3][3];
	float m10 = m2.m[1][0] * m[0][0] + m2.m[1][1] * m[1][0] + m2.m[1][2] * m[2][0] + m2.m[1][3] * m[3][0];
	float m11 = m2.m[1][0] * m[0][1] + m2.m[1][1] * m[1][1] + m2.m[1][2] * m[2][1] + m2.m[1][3] * m[3][1];
	float m12 = m2.m[1][0] * m[0][2] + m2.m[1][1] * m[1][2] + m2.m[1][2] * m[2][2] + m2.m[1][3] * m[3][2];
	float m13 = m2.m[1][0] * m[0][3] + m2.m[1][1] * m[1][3] + m2.m[1][2] * m[2][3] + m2.m[1][3] * m[3][3];
	float m20 = m2.m[2][0] * m[0][0] + m2.m[2][1] * m[1][0] + m2.m[2][2] * m[2][0] + m2.m[2][3] * m[3][0];
	float m21 = m2.m[2][0] * m[0][1] + m2.m[2][1] * m[1][1] + m2.m[2][2] * m[2][1] + m2.m[2][3] * m[3][1];
	float m22 = m2.m[2][0] * m[0][2] + m2.m[2][1] * m[1][2] + m2.m[2][2] * m[2][2] + m2.m[2][3] * m[3][2];
	float m23 = m2.m[2][0] * m[0][3] + m2.m[2][1] * m[1][3] + m2.m[2][2] * m[2][3] + m2.m[2][3] * m[3][3];
	float m30 = m2.m[3][0] * m[0][0] + m2.m[3][1] * m[1][0] + m2.m[3][2] * m[3][0] + m2.m[3][3] * m[3][0];
	float m31 = m2.m[3][0] * m[0][1] + m2.m[3][1] * m[1][1] + m2.m[3][2] * m[3][1] + m2.m[3][3] * m[3][1];
	float m32 = m2.m[3][0] * m[0][2] + m2.m[3][1] * m[1][2] + m2.m[3][2] * m[3][2] + m2.m[3][3] * m[3][2];
	float m33 = m2.m[3][0] * m[0][3] + m2.m[3][1] * m[1][3] + m2.m[3][2] * m[3][3] + m2.m[3][3] * m[3][3];
	Matrix result;
	result.m[0][0] = m00;
	result.m[0][1] = m01;
	result.m[0][2] = m02;
	result.m[0][3] = m03;
	result.m[1][0] = m10;
	result.m[1][1] = m11;
	result.m[1][2] = m12;
	result.m[1][3] = m13;
	result.m[2][0] = m20;
	result.m[2][1] = m21;
	result.m[2][2] = m22;
	result.m[2][3] = m23;
	result.m[3][0] = m30;
	result.m[3][1] = m31;
	result.m[3][2] = m32;
	result.m[3][3] = m33;
	return result;
}

Matrix Matrix::inverse() 
{
	float m00 = m[0][0], m01 = m[0][1], m02 = m[0][2], m03 = m[0][3];
	float m10 = m[1][0], m11 = m[1][1], m12 = m[1][2], m13 = m[1][3];
	float m20 = m[2][0], m21 = m[2][1], m22 = m[2][2], m23 = m[2][3];
	float m30 = m[3][0], m31 = m[3][1], m32 = m[3][2], m33 = m[3][3];

	float v0 = m20 * m31 - m21 * m30;
	float v1 = m20 * m32 - m22 * m30;
	float v2 = m20 * m33 - m23 * m30;
	float v3 = m21 * m32 - m22 * m31;
	float v4 = m21 * m33 - m23 * m31;
	float v5 = m22 * m33 - m23 * m32;

	float t00 = +(v5 * m11 - v4 * m12 + v3 * m13);
	float t10 = -(v5 * m10 - v2 * m12 + v1 * m13);
	float t20 = +(v4 * m10 - v2 * m11 + v0 * m13);
	float t30 = -(v3 * m10 - v1 * m11 + v0 * m12);

	float invDet = 1 / (t00 * m00 + t10 * m01 + t20 * m02 + t30 * m03);

	float d00 = t00 * invDet;
	float d10 = t10 * invDet;
	float d20 = t20 * invDet;
	float d30 = t30 * invDet;

	float d01 = -(v5 * m01 - v4 * m02 + v3 * m03) * invDet;
	float d11 = +(v5 * m00 - v2 * m02 + v1 * m03) * invDet;
	float d21 = -(v4 * m00 - v2 * m01 + v0 * m03) * invDet;
	float d31 = +(v3 * m00 - v1 * m01 + v0 * m02) * invDet;

	v0 = m10 * m31 - m11 * m30;
	v1 = m10 * m32 - m12 * m30;
	v2 = m10 * m33 - m13 * m30;
	v3 = m11 * m32 - m12 * m31;
	v4 = m11 * m33 - m13 * m31;
	v5 = m12 * m33 - m13 * m32;

	float d02 = +(v5 * m01 - v4 * m02 + v3 * m03) * invDet;
	float d12 = -(v5 * m00 - v2 * m02 + v1 * m03) * invDet;
	float d22 = +(v4 * m00 - v2 * m01 + v0 * m03) * invDet;
	float d32 = -(v3 * m00 - v1 * m01 + v0 * m02) * invDet;

	v0 = m21 * m10 - m20 * m11;
	v1 = m22 * m10 - m20 * m12;
	v2 = m23 * m10 - m20 * m13;
	v3 = m22 * m11 - m21 * m12;
	v4 = m23 * m11 - m21 * m13;
	v5 = m23 * m12 - m22 * m13;

	float d03 = -(v5 * m01 - v4 * m02 + v3 * m03) * invDet;
	float d13 = +(v5 * m00 - v2 * m02 + v1 * m03) * invDet;
	float d23 = -(v4 * m00 - v2 * m01 + v0 * m03) * invDet;
	float d33 = +(v3 * m00 - v1 * m01 + v0 * m02) * invDet;


	Matrix m2;

	m2.m[0][0] = d00;
	m2.m[0][1] = d01;
	m2.m[0][2] = d02;
	m2.m[0][3] = d03;
	m2.m[1][0] = d10;
	m2.m[1][1] = d11;
	m2.m[1][2] = d12;
	m2.m[1][3] = d13;
	m2.m[2][0] = d20;
	m2.m[2][1] = d21;
	m2.m[2][2] = d22;
	m2.m[2][3] = d23;
	m2.m[3][0] = d30;
	m2.m[3][1] = d31;
	m2.m[3][2] = d32;
	m2.m[3][3] = d33;

	return m2;
}



float lerp(float v0, float v1, float t)
{
	return (1.0 - t)*v0 + t*v1;
}

class Entity
{
public:
	Entity(float x, float y, float vx = 0.0, float vy = 0.0)
	{
		this->x = x;
		this->y = y;
		this->x_velocity = vx;
		this->y_velocity = vy;
		angle = 0.0f;
		scalex = 1.0f;
		scaley = 1.0f;
	}
	Entity() :x(0.0), y(0.0), y_velocity(0.0), x_velocity(0.0), top(false), bottom(false), right(false), left(false){}
	Matrix matrix;
	Vector vector;
	float x;
	float y;
	float scalex;
	float scaley;
	float angle;
	float width;
	float height;
	float y_velocity;
	float x_velocity;
	bool top;
	bool bottom;
	bool right;
	bool left;
	void build();
	void draw();
};

void Entity::build()
{
	matrix = scale(scalex,scaley)*rotate(angle)*translate(x, y);
}

void Entity::draw()
{
	build();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMultMatrixf(matrix.ml);
	GLfloat quad[] = { -0.10f, 0.10f, -0.10f, -0.10f, 0.10f, -0.10f, 0.10f, 0.10f };
	glVertexPointer(2, GL_FLOAT, 0, quad);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDrawArrays(GL_QUADS, 0, 4);
	glDisableClientState(GL_VERTEX_ARRAY);
	glPopMatrix();
}

class Game
{
private:
	bool done;
	SDL_Window* displayWindow;
	//vector<SheetSprite> Sprite;
	int vertexnumber;
	vector<Entity> entities;
	vector<unsigned int> font;
	float elapsed;
	float framesPerSecond;
	float lastFrameTicks;
	float gravity;
	float x_friction;
	float y_friction;
public:
	void clearup();
	void initialize();
	void render();
	void update();
	void Draw();
	void player_block_collide();
	void player_enemy_collide();
	void collision();
	bool entity_collide_tile(const std::pair<float, float>& entity1, const std::pair<float, float>&entity2);
	void entity_block_collide(Entity* entity);
	bool is_done();
	bool entity_collide(const Entity &entity1, const Entity &entity2);
	float y_penetration(const std::pair<float, float>& entity_1, const std::pair<float, float>& entity_2);
	float x_penetration(const std::pair<float, float>& entity_1, const std::pair<float, float>& entity_2);
};

bool Game::is_done()
{
	return done;
}

void Game::initialize()
{
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	glMatrixMode(GL_PROJECTION);
	glOrtho(-1.33, 1.33, -1.0, 1.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, 800, 600);
	framesPerSecond = 30.0f;
	elapsed = 0.0f;
	lastFrameTicks = 0.0f;
	for (int i = 0; i < 5; i++)
	{
		float x = float((rand() % 100 - 50) / 50.0);
		float y = float((rand() % 100 - 50) / 50.0);
		float xspeed = float((rand() % 100 - 50) / 50.0);
		float yspeed = float((rand() % 100 - 50) / 50.0);
		entities.push_back(Entity(x,y,xspeed,yspeed));
	}
}

void Game::update()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE)
		{
			done = true;
		}
	}
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	elapsed = ticks - lastFrameTicks;
	lastFrameTicks = ticks;
	float fixedElapsed = elapsed + timeLeftOver;
	if (fixedElapsed > FIXED_TIMESTEP * MAX_TIMESTEPS)
	{
		fixedElapsed = FIXED_TIMESTEP * MAX_TIMESTEPS;
	}
	while (fixedElapsed >= FIXED_TIMESTEP)
	{
		fixedElapsed -= FIXED_TIMESTEP;
		collision();
	}
	timeLeftOver = fixedElapsed;
}

bool Game::entity_collide(const Entity &entity1, const Entity &entity2)
{
	return (abs(float(entity1.x - entity2.x))*abs(float(entity1.x - entity2.x)) + abs(float(entity1.y - entity2.y))*abs(float(entity1.y - entity2.y))) < 0.2*0.2;
}

void Game::collision()
{
	for (int i = 0; i < entities.size(); i++)
	{
		entities[i].x += entities[i].x_velocity * FIXED_TIMESTEP;
		for (int p = 0; p < entities.size(); p++)
		{
			if (entity_collide(entities[i], entities[p]))
			{
				entities[i].x += 0.0001*entities[i].x_velocity;
				entities[i].x_velocity = -entities[i].x_velocity;
				entities[p].x += 0.0001*entities[p].x_velocity;
				entities[p].x_velocity = -entities[p].x_velocity;
			}
			if (entities[i].x>1.33 || entities[i].x < -1.33)
				entities[i].x_velocity = -entities[i].x_velocity;
		}
		entities[i].y += entities[i].y_velocity * FIXED_TIMESTEP;
		for (int p = 0; p < entities.size(); p++)
		{
			if (entity_collide(entities[i], entities[p]))
			{
				entities[i].y += 0.0001*entities[i].y_velocity;
				entities[i].y_velocity = -entities[i].y_velocity;
				entities[p].y += 0.0001*entities[p].y_velocity;
				entities[p].y_velocity = -entities[p].y_velocity;
			}
			if (entities[i].y>1.00 || entities[i].y < -1.00)
				entities[i].y_velocity = -entities[i].y_velocity;
		}
	}
}

void Game::render()
{	
	glClear(GL_COLOR_BUFFER_BIT);
	for (int i = 0; i < entities.size(); i++)
	{
		entities[i].draw();
	}
	SDL_GL_SwapWindow(displayWindow);
}



int main(int argc, char *argv[])
{
	Game Assignment_6;
	Assignment_6.initialize();
	while (!Assignment_6.is_done())
	{
		Assignment_6.render();
		Assignment_6.update();
	}
	return 0;
}