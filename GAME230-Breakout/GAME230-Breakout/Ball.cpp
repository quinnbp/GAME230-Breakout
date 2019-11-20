#include "Ball.h"
#include "Paddle.h"


const double PI = 3.14159265358979323846264388;

enum { FREE, OFFSCREEN, ONPADDLE };

Ball::Ball() {
	this->position = Vector2f(0.0f, 0.0f);
	this->velocity = Vector2f(0.0f, 0.0f);

	this->radius = 5.0f;
	this->shape = CircleShape(this->radius);
	this->state = FREE;
}

void Ball::setState(int state) {
	this->state = state;
}

int Ball::getState() {
	return this->state;
}

void Ball::bounceBrick(Brick* brick) {
	brick->setActive(false);
	if (this->velocity.y < 0) {
		this->velocity.y *= -1.0f;
	}
}

void Ball::bouncePaddle(Paddle* paddle) {
	// get magnitude so as to keep consistent
	float magnitude = sqrt( this->velocity.x * this->velocity.x + this->velocity.y * this->velocity.y );

	// get midpoint of paddle, and distance from midpoint to ball x
	float paddleMidpoint = paddle->getPosition().x + (paddle->getSize().x / 2.0f);
	float paddleMidToBall = paddleMidpoint - this->getPosition().x;

	// which x direction to bounce (as in breakout)
	int xBounceMod = 1;
	if (paddleMidToBall > 0) { // left of middle
		xBounceMod = -1;
	}
	else { // right of middle
		paddleMidToBall *= -1;
	}

	// get ratio of ball distance along paddle to size of paddle, should be 0-1 but is constrained anyway
	float angleRatio = paddleMidToBall / (paddle->getSize().x / 2.0f);
	if (angleRatio > 1) {
		angleRatio = 1;
	}

	// what angle to bounce (as in breakout)
	float thetaDegrees = 90.0f - 60.0f * angleRatio; // max 90, min 30
	float theta = thetaDegrees * PI / 180.0f; // conv to radians
	float newXV = magnitude * cos(theta); // calc new components, always positive
	float newYV = magnitude * sin(theta);

	// set new vel comps
	this->velocity.x = newXV * xBounceMod;
	this->velocity.y = -1.0f * newYV;
}

void Ball::update(int dt_ms, int windowWidth, int windowHeight) {
	// update position based on velocity
	this->position.x += this->velocity.x * dt_ms;
	this->position.y += this->velocity.y * dt_ms;

	if (this->position.y + this->radius > windowHeight) {
		// pass as we are below board
		if (this->state == FREE) {
			this->state = OFFSCREEN;
		}
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