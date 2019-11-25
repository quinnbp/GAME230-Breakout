#include "Powerup.h"

enum { PADDLE, BALL };
enum { NORMAL, PENETRATING };

Powerup::Powerup(int ntype) {
	this->position = Vector2f(0.0f, 0.0f);
	this->velocity_y = 0.0f;

	this->radius = 10.0f;
	this->type = ntype;
	this->active = false;
	this->colorCycleCount = 0;

	this->shape = CircleShape(this->radius, 60);
	if (this->type == PADDLE) {
		this->baseColor = Color::Red;
	}
	else if (this->type == BALL) {
		this->baseColor = Color::Blue;
	}
}

Vector2f Powerup::getPosition() {
	return this->position;
}

float Powerup::getRadius() {
	return this->radius;
}

bool Powerup::isActive() {
	return this->active;
}

void Powerup::spawn(Vector2f newposition) {
	this->position = newposition;
	this->velocity_y = 0.6f;
	this->active = true;
}

void Powerup::paddleCollide(Paddle* paddle, Ball* ball) {
	// remove from screen
	this->velocity_y = 0;
	this->position = Vector2f(-10.0f, -10.0f);
	this->active = false;

	// update paddle or ball depending on type
	if (this->type == PADDLE) {
		paddle->setSize(Vector2f(paddle->getSize().x * 1.5f, paddle->getSize().y));
	}
	else if (this->type == BALL) {
		ball->setPower(PENETRATING);
	}
}

void Powerup::setActive(bool state) {
	this->active = state;
}

void Powerup::update(int dt_ms, int windowHeight) {
	// no x velocity needed (they only fall)
	this->position.y += this->velocity_y * dt_ms;
	if (this->position.y > windowHeight) {
		// we are offscreen
		this->velocity_y = 0;
		this->position = Vector2f(-10.0f, -10.0f);
		this->active = false;
	}
	else {
		// if still on screen, update colors
		this->colorCycleCount++;
		if (this->colorCycleCount == 1) {
			this->shape.setFillColor(this->baseColor);
		}
		else if (this->colorCycleCount == 6) {
			this->colorCycleCount = 0;
			this->shape.setFillColor(Color::White);
		}
	}
}

void Powerup::draw(RenderWindow* window) {
	if (this->active) {
		this->shape.setPosition(this->position.x - this->radius, this->position.y - this->radius);
		window->draw(this->shape);
	}
}