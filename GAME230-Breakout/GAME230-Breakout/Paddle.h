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
	Paddle();
	void draw(RenderWindow* window);

	void updateKeyboard(int dt_ms, int windowWidth, bool left, bool right);
	void updateMouse(int windowWidth, Vector2i mousePos);
	void updateDelegator(int dt_ms, int windowWidth, bool left, bool right, Vector2i mousePos);

	void setPosition(Vector2f newpos);
	Vector2f getPosition();
	void setVelocity(float newvel);
	float getVelocity();
	Vector2f getSize();
	void setMouseControl(bool state);
private:
	void checkBounds(int windowWidth);
	Vector2f position;
	float velocity_x;
	float baseVelocity;
	RectangleShape shape;
	Vector2f size;
	bool mouseControl;
};