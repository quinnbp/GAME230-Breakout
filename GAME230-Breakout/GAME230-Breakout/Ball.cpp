#include "Ball.h"

Ball::Ball() {
	this->position = Vector2f(0.0f, 0.0f);
	this->velocity = Vector2f(0.0f, 0.0f);

	this->radius = 5.0f;
	this->shape = CircleShape(this->radius);
}

void Ball::update(int dt_ms, int windowWidth, int windowHeight) {
	// update position based on velocity
	this->position.x += this->velocity.x * dt_ms;
	this->position.y += this->velocity.y * dt_ms;

	if (this->position.y + this->radius > windowHeight) {
		this->position.y = windowHeight - this->radius;
		this->velocity.y *= -1;
	}
	else if (this->position.y - this->radius < 0) {
		this->position.y = this->radius;
		this->velocity.y *= -1;
	}

	if (this->position.x + this->radius > windowWidth) {
		this->position.x = windowWidth - this->radius;
		this->velocity.x *= -1;
	}
	else if (this->position.x - this->radius < 0) {
		this->position.x = this->radius;
		this->velocity.x *= -1;
	}
}

void Ball::draw(RenderWindow* window) {
	this->shape.setPosition(this->position);
	window->draw(this->shape);
}

float Ball::getRadius() {
	return this->radius;
}

void Ball::setPosition(Vector2f newpos) {
	this->position = newpos;
}

void Ball::setVelocity(Vector2f newvel) {
	this->velocity = newvel;
}

Vector2f Ball::getPosition() {
	return this->position;
}

Vector2f Ball::getVelocity() {
	return this->velocity;
}