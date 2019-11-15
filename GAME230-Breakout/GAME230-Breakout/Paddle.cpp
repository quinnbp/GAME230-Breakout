#include "Paddle.h"

Paddle::Paddle() {
	this->position = Vector2f(0.0f, 0.0f);
	this->velocity_x = 0.0f;

	this->size = Vector2f(70.0f, 10.0f);
	this->baseVelocity = 0.4f;

	this->shape = RectangleShape(this->size);
}

void Paddle::update(int dt_ms, int windowWidth, int windowHeight) {
	// update position based on velocity
	this->position.x += this->velocity_x * dt_ms;
}

void Paddle::draw(RenderWindow* window) {
	this->shape.setPosition(Vector2f(this->position.x - this->size.x / 2.0f, this->position.y));
	window->draw(this->shape);
}

Vector2f Paddle::getSize() {
	return this->size;
}

void Paddle::setPosition(Vector2f newpos) {
	this->position = newpos;
}

void Paddle::setVelocity(float newvel) {
	this->velocity_x = newvel;
}

Vector2f Paddle::getPosition() {
	return this->position;
}

float Paddle::getVelocity() {
	return this->velocity_x;
}

