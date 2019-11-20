#include "Brick.h"

Brick::Brick() {
	this->position = Vector2f(0.0f, 0.0f);
	this->size = Vector2f(100.0f, 30.0f);

	this->active = true;

	this->shape = RectangleShape(this->size);
	this->shape.setPosition(this->position);
	this->shape.setFillColor(Color::White);
	this->shape.setOutlineColor(Color::Black);
	this->shape.setOutlineThickness(-2);
}

void Brick::update() {

}

void Brick::draw(RenderWindow* window) {
	if (this->active) {
		this->shape.setPosition(this->position);
		window->draw(this->shape);
	}
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

