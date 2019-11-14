#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Main.hpp>

using namespace std;
using namespace sf;

const int WINDOW_WIDTH = 1024;
const int WINDOW_HEIGHT = 768;

class Brick {
public:
	Brick();
	void draw(RenderWindow* window);
	void setPosition(Vector2f newpos);
	Vector2f getPosition();
	Vector2f getSize();
	bool isActive();
	void setActive(bool state);
private:
	Vector2f position;
	RectangleShape shape;
	Vector2f size;
	bool active;
};

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

class Paddle {
public:
	Paddle();
	void draw(RenderWindow* window);
	void update(int dt_ms);
	void setPosition(Vector2f newpos);
	Vector2f getPosition();
	void setVelocity(float newvel);
	float getVelocity();
	Vector2f getSize();
	Color color;
private:
	Vector2f position;
	float velocity_x;
	float baseVelocity;
	RectangleShape shape;
	Vector2f size;
};

Paddle::Paddle() {
	this->position = Vector2f(0.0f, 0.0f);
	this->velocity_x = 0.0f;
	
	this->size = Vector2f(70.0f, 10.0f);
	this->baseVelocity = 0.4f;

	this->shape = RectangleShape(this->size);

	this->color = Color(255, 255, 255);
}

void Paddle::update(int dt_ms) {
	// update position based on velocity
	this->position.x += this->velocity_x * dt_ms;
	this->shape.setFillColor(this->color);
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

class Ball {
public:
	Ball();
	void setPosition(Vector2f newpos);
	Vector2f getPosition();
	void setVelocity(Vector2f newvel);
	Vector2f getVelocity();
	float getRadius();
	void draw(RenderWindow* window);
	void update(int dt_ms);
private:
	Vector2f position;
	Vector2f velocity;
	CircleShape shape;
	float radius;
};

Ball::Ball() {
	this->position = Vector2f(0.0f, 0.0f);
	this->velocity = Vector2f(0.0f, 0.0f);

	this->radius = 5.0f;
	this->shape = CircleShape(this->radius);
}

void Ball::update(int dt_ms) {
	// update position based on velocity
	this->position.x += this->velocity.x * dt_ms;
	this->position.y += this->velocity.y * dt_ms;

	if (this->position.y + this->radius > WINDOW_HEIGHT) {
		this->position.y = WINDOW_HEIGHT - this->radius;
		this->velocity.y *= -1;
	}
	else if (this->position.y - this->radius < 0) {
		this->position.y = this->radius;
		this->velocity.y *= -1;
	}

	if (this->position.x + this->radius > WINDOW_WIDTH) {
		this->position.x = WINDOW_WIDTH - this->radius;
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

/*
Checks for circle-rectangle collisions for the ball and paddle
bp is the CENTER position of the circle, pp is the TOP LEFT of the paddle
*/
bool collisionRectangle(Ball* ball, Paddle* paddle) {
	Vector2f bp = ball->getPosition();
	float br = ball->getRadius();
	Vector2f pp = paddle->getPosition();
	Vector2f ps = paddle->getSize();

	float testX = bp.x;
	float testY = bp.y;
	if (bp.x < pp.x) {
		// ball center left from left edge
		testX = pp.x;
	}
	else if (bp.x > pp.x + ps.x) {
		// ball center right of right edge
		testX = pp.x + ps.x;
	}
	// else ball center within x range

	if (bp.y < pp.y) {
		// ball center up from top edge
		testY = pp.y;
	}
	else if (bp.y > pp.y + ps.y) {
		// ball center down from bottom edge
		testY = pp.y + ps.y;
	}
	// else ball center within y range

	// calculate pythagorean distance from ball to closest edge
	float distX = bp.x - testX;
	float distY = bp.y - testY;
	float pythagDist = sqrt((distX * distX) + (distY * distY));

	if (pythagDist <= br) { // pythag collision
		return true;
	}
	else {
		return false;
	}
}

int main() {
	RenderWindow window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Breakout");
	window.setVerticalSyncEnabled(true);
	window.setKeyRepeatEnabled(false);


	Clock clock = Clock();
	int dt_ms = 0;


	Ball ball = Ball();
	ball.setPosition(Vector2f(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f));
	ball.setVelocity(Vector2f(0.3f, 0.5f));
	Paddle paddle = Paddle();
	paddle.setPosition(Vector2f(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT - 15.0f));


	while (window.isOpen()) {

		dt_ms = clock.getElapsedTime().asMilliseconds();
		clock.restart();

		Event event;
		while (window.pollEvent(event)) {
			if (event.type == Event::Closed) {
				window.close();
			}
		}

		if (collisionRectangle(&ball, &paddle)) {
			paddle.color = Color(255, 0, 0);
		}

		window.clear(Color(0, 0, 0));

		ball.update(dt_ms);
		paddle.update(dt_ms);

		ball.draw(&window);
		paddle.draw(&window);

		window.display();
	}

	return 0;
}
