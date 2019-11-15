#include "Brick.h"

Brick::Brick() {
	this->position = Vector2f(0.0f, 0.0f);
	this->size = Vector2f(5.0f, 3.0f);

	this->shape = RectangleShape(this->size);
}

void Brick::draw(RenderWindow* window) {
	window->draw(this->shape);
}

bool Brick::isActive() {
	return this->active;
}

void Brick::setActive(bool state) {
	this->active = state;
}

Vector2f Brick::getSize() {
	return this->size;
}

void Brick::setPosition(Vector2f newpos) {
	this->position = newpos;
}

Vector2f Brick::getPosition() {
	return this->position;
}

