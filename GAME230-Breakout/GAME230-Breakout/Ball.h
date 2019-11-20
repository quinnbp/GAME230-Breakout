#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Main.hpp>

#include "Paddle.h"
#include "Brick.h"

using namespace sf;

class Ball {
public:
	Ball();
	void setPosition(Vector2f newpos);
	Vector2f getPosition();
	void setVelocity(Vector2f newvel);
	Vector2f getVelocity();
	float getRadius();
	void draw(RenderWindow* window);
	void update(int dt_ms, int windowWidth, int windowHeight);
	void bouncePaddle(Paddle* paddle);
	void bounceBrick(Brick* brick);
	int getState();
	void setState(int state);
private:
	Vector2f position;
	Vector2f velocity;
	CircleShape shape;
	float radius;
	int state;
};