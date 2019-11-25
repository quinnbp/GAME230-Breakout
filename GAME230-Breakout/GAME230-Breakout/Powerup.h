#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Main.hpp>

#include "Paddle.h"
#include "Ball.h"

using namespace sf;

class Powerup {
public:
	Powerup(int type);
	void update(int dt_ms, int windowHeight);
	void draw(RenderWindow* window);
	void paddleCollide(Paddle* paddle, Ball* ball);
	void spawn(Vector2f newposition);
	bool isActive();
	float getRadius();
	Vector2f getPosition();
	void setActive(bool state);
private:
	CircleShape shape;
	Vector2f position;
	float velocity_y;
	float radius;
	int type;
	bool active;
	int colorCycleCount;
	Color baseColor;
};