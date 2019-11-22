#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Main.hpp>

using namespace sf;

class Brick {
public:
	Brick(const Texture* texture);
	void draw(RenderWindow* window);
	void update();
	void setPosition(Vector2f newpos);
	Vector2f getPosition();
	Vector2f getSize();
	bool isActive();
	void setActive(bool state);
	bool active;
private:
	Vector2f position;
	RectangleShape shape;
	Vector2f size;
	
};
