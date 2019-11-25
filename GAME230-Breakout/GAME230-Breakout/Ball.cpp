#include "Ball.h"
#include "Paddle.h"

using namespace std;

const double PI = 3.14159265358979323846264388;

enum { FREE, OFFSCREEN, ONPADDLE };
enum { NORMAL, PENETRATING };

Ball::Ball() {
	this->position = Vector2f(0.0f, 0.0f);
	this->velocity = Vector2f(0.0f, 0.0f);

	// set up vector for trail fx
	this->pastPositions = {};
	// push 5 dummy positions to maintain size
	for (int i = 0; i < 9; i++) {
		this->pastPositions.push_back(Vector2f(-10, -10));
	}

	this->radius = 7.0f;

	this->shape = CircleShape(this->radius, 60);
	this->shape.setFillColor(Color::White);
	this->shape.setOutlineColor(Color::Black);
	this->shape.setOutlineThickness(2.0f);

	this->shape2 = CircleShape(this->radius, 60);
	this->shape2.setFillColor(Color::White);
	this->shape2.setOutlineColor(Color::Black);
	this->shape2.setOutlineThickness(1.0f);
	
	this->colorCycleCount = 0;

	this->state = ONPADDLE;
	this->power = NORMAL;
	this->speed = 0.5;
}

int Ball::getPower(){
	return this->power;
}

void Ball::setPower(int newpower) {
	this->power = newpower;
}

void Ball::setSpeed(float newspeed) {
	this->speed = newspeed;
}

float Ball::getSpeed() {
	return this->speed;
}

void Ball::setState(int state) {
	this->state = state;
}

int Ball::getState() {
	return this->state;
}

void Ball::bounceBrick(Brick* brick) {
	if (this->power != PENETRATING) { // if ball is penetrating, take no bounce action
		// get min max X/Y coords for brick
		float brickTopY = brick->getPosition().y;
		float brickBottomY = brick->getPosition().y + brick->getSize().y;
		float brickLeftX = brick->getPosition().x;
		float brickRightX = brick->getPosition().x + brick->getSize().x;

		if (this->position.x < brickLeftX || this->position.x > brickRightX) {
			// we are coming from left or right
			this->velocity.x *= -1.0f;
		}
		else if (this->position.y < brickTopY || this->position.y > brickBottomY) {
			// we are coming from top or bottom
			this->velocity.y *= -1.0f;
		}
		else {
			// catch weird edge cases by forcing ball DOWN
			if (this->velocity.y < 0) {
				this->velocity.y *= -1.0f;
			}
		}
	}
}

void Ball::bouncePaddle(Paddle* paddle) {
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
	float newXV = this->speed * cos(theta); // calc new components, always positive
	float newYV = this->speed * sin(theta);

	// set new vel comps
	this->velocity.x = newXV * xBounceMod;
	this->velocity.y = -1.0f * newYV;
}

bool Ball::update(int dt_ms, int windowWidth, int windowHeight) {
	// colors
	if (this->power == PENETRATING) {
		if (this->colorCycleCount > 3) {
			this->shape.setFillColor(Color(rand(), rand(), rand()));
			this->colorCycleCount = 0;
		}
		else {
			this->colorCycleCount++;
		}
	}
	else {
		this->shape.setFillColor(Color::White);
	}

	// push current pos to vector for trail fx
	this->pastPositions.push_back(this->position);
	// remove oldest pos in list to maintain size
	this->pastPositions.erase(this->pastPositions.begin());

	// update position based on velocity
	this->position.x += this->velocity.x * dt_ms;
	this->position.y += this->velocity.y * dt_ms;

	bool wallBounce = false;

	if (this->position.y + this->radius > windowHeight) {
		// pass as we are below board
		if (this->state == FREE) {
			this->state = OFFSCREEN;
		}
	}
	else if (this->position.y - this->radius < 0) {
		this->position.y = this->radius;
		this->velocity.y *= -1;
		wallBounce = true;
	}

	if (this->position.x + this->radius > windowWidth) {
		this->position.x = windowWidth - this->radius;
		this->velocity.x *= -1;
		wallBounce = true;
	}
	else if (this->position.x - this->radius < 0) {
		this->position.x = this->radius;
		this->velocity.x *= -1;
		wallBounce = true;
	}

	return wallBounce;
}

void Ball::paddleRelease(int windowWidth, int windowHeight, Vector2f paddlePos) {
	Vector2f midpoint = Vector2f(windowWidth / 2.0f, windowHeight / 2.0f);
	Vector2f displacement = Vector2f(midpoint.x - paddlePos.x, midpoint.y - paddlePos.y);
	float magnitude = sqrt(displacement.x * displacement.x + displacement.y * displacement.y);
	Vector2f unit_displacement = Vector2f(displacement.x / magnitude, displacement.y / magnitude);

	this->setVelocity(Vector2f(unit_displacement.x * this->speed, unit_displacement.y * this->speed));
}

void Ball::draw(RenderWindow* window) {
	if (this->state == FREE) {
		for (int i = 0; i < 9; i++) {
			// get most oldest-newest positions to draw effect
			Vector2f posToDraw = this->pastPositions.at(i);
			this->shape2.setRadius(this->radius / 9 * i);
			this->shape2.setFillColor(Color(255, 255, 255, 255 * (i + 1) / 10));
			this->shape2.setPosition(posToDraw.x - shape2.getRadius(), posToDraw.y - shape2.getRadius());
			window->draw(this->shape2);
		}
	}
	this->shape.setPosition(Vector2f(this->position.x - this->radius, this->position.y - this->radius));
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