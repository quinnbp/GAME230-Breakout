#include "Paddle.h"

Paddle::Paddle() {
	this->position = Vector2f(0.0f, 0.0f);
	this->velocity_x = 0.0f;

	this->size = Vector2f(70.0f, 10.0f);
	this->baseVelocity = 0.4f;

	this->shape = RectangleShape(this->size);
}

void Paddle::update(int dt_ms, int windowWidth, bool left, bool right) {
	if (left && right || !left && !right) { // neither or both keys
		this->velocity_x = 0.0f;
	}
	else if (left) {
		this->velocity_x = -1.0f * this->baseVelocity;
	}
	else if (right) {
		this->velocity_x = this->baseVelocity;
	}
	
	// update position based on velocity
	this->position.x += this->velocity_x * dt_ms;

	if (this->position.x < 0) { // if off the left side of screen
		this->position.x = 0;
	}
	else if (this->position.x + this->size.x > windowWidth) { // if off the right of the screen
		this->position.x = windowWidth - this->size.x;
	}
}

void Paddle::draw(RenderWindow* window) {
	this->shape.setPosition(this->position);
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

