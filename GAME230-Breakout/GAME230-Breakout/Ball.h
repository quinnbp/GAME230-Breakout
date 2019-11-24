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
	void setPosition(Vector2f newpos); // position of the CENTER of the circle
	Vector2f getPosition();
	void setVelocity(Vector2f newvel);
	Vector2f getVelocity();
	float getRadius();
	void draw(RenderWindow* window);
	bool update(int dt_ms, int windowWidth, int windowHeight);
	void bouncePaddle(Paddle* paddle);
	void bounceBrick(Brick* brick);
	void paddleRelease(int windowWidth, int windowHeight, Vector2f paddlePos);
	int getState();
	void setState(int state);
	float getSpeed();
	void setSpeed(float newspeed);
private:
	Vector2f position;
	Vector2f velocity;
	CircleShape shape;
	float radius;
	int state;
	int colorCycleCount;
	float speed;
};