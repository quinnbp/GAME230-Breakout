#include "Paddle.h"

enum {MOUSE, KEYBOARD, AI};

Paddle::Paddle(Texture* texture) {
	this->position = Vector2f(0.0f, 0.0f);
	this->velocity_x = 0.0f;
	this->size = Vector2f(70.0f, 10.0f);
	this->baseVelocity = 0.5f;
	this->shape = RectangleShape(this->size);
	this->shape.setTexture(texture);
	this->controls = KEYBOARD;
}

void Paddle::setSize(Vector2f newsize) {
	this->size = newsize;
}

int Paddle::getControls() {
	return this->controls;
}

void Paddle::checkBounds(int windowWidth) {
	if (this->position.x < 0) { // if off the left side of screen
		this->position.x = 0;
	}
	else if (this->position.x + this->size.x > windowWidth) { // if off the right of the screen
		this->position.x = windowWidth - this->size.x;
	}
}

void Paddle::updateDelegator(int dt_ms, int windowWidth, bool left, bool right, Vector2i mousePos, Vector2f ballPosition) {
	// delegate how control works based on chosen control scheme
	this->shape.setSize(this->size);
	if (this->controls == MOUSE) {
		this->updateMouse(windowWidth, mousePos);
	}
	else if (this->controls == AI) {
		this->updateAi(dt_ms, windowWidth, ballPosition);
	}
	else {
		this->updateKeyboard(dt_ms, windowWidth, left, right);
	}
}

void Paddle::updateAi(int dt_ms, int windowWidth, Vector2f ballPositon) {
	if (ballPositon.x > this->position.x + this->size.x) {
		this->velocity_x = this->baseVelocity;
	}
	else if (ballPositon.x < this->position.x) {
		this->velocity_x = -1.0f * this->baseVelocity;
	}
	else {
		this->velocity_x = 0.0f;
	}

	this->position.x += this->velocity_x * dt_ms;
	this->checkBounds(windowWidth);
}

void Paddle::updateMouse(int windowWidth, Vector2i mousePos) {
	this->position = Vector2f(mousePos.x - this->getSize().x / 2.0f, this->position.y);
	this->checkBounds(windowWidth);
}

void Paddle::updateKeyboard(int dt_ms, int windowWidth, bool left, bool right) {
	// update velocity based on input
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
	this->checkBounds(windowWidth);
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

void Paddle::setControls(int state) {
	this->controls = state;
}