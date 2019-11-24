#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Main.hpp>

using namespace sf;

class Paddle {
public:
	Paddle(Texture* texture);
	void draw(RenderWindow* window);

	void updateDelegator(int dt_ms, int windowWidth, bool left, bool right, Vector2i mousePos, Vector2f ballPosition);
	void updateKeyboard(int dt_ms, int windowWidth, bool left, bool right);
	void updateMouse(int windowWidth, Vector2i mousePos);
	void updateAi(int dt_ms, int windowWidth, Vector2f ballPositon);

	void setPosition(Vector2f newpos);
	Vector2f getPosition();
	void setVelocity(float newvel);
	float getVelocity();
	void setControls(int state);
	int getControls();
	Vector2f getSize();
private:
	void checkBounds(int windowWidth);
	Vector2f position;
	float velocity_x;
	float baseVelocity;
	RectangleShape shape;
	Vector2f size;
	int controls;
};