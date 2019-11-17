#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Main.hpp>

#include "Ball.h"
#include "Paddle.h"
#include "Brick.h"

using namespace std;
using namespace sf;

const int WINDOW_WIDTH = 1024;
const int WINDOW_HEIGHT = 768;

/*
Calculates pythagorean distance between two points given as Vector2f
*/
float pythagDist(Vector2f p1, Vector2f p2) {
	return sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}

/*
Checks for circle-rectangle collisions for the ball and paddle
bp is the CENTER position of the circle, pp is the TOP LEFT of the paddle
br is the radius of the circle, ps is the (w, h) of the rect
*/
bool circleRectCollision(Vector2f bp, float br, Vector2f pp, Vector2f ps) {
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

	if (pythagDist(bp, Vector2f(testX, testY)) <= br) { // pythag collision
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

	bool leftKeyPressed = false;
	bool rightKeyPressed = false;

	Ball ball = Ball();
	ball.setPosition(Vector2f(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f));
	ball.setVelocity(Vector2f(0.0f, 0.5f));
	Paddle paddle = Paddle();
	paddle.setPosition(Vector2f(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT - 15.0f));

	sf::Font font;
	if (!font.loadFromFile("arial.ttf")) {
		exit(-1);
	}

	Text debugText;
	debugText.setPosition(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f);
	debugText.setFont(font);
	debugText.setFillColor(Color::White);
	debugText.setString("");

	while (window.isOpen()) {

		dt_ms = clock.getElapsedTime().asMilliseconds();
		clock.restart();

		Event event;
		while (window.pollEvent(event)) {
			if (event.type == Event::Closed) {
				window.close();
			}
			else if (event.type == Event::KeyPressed) {
				if (event.key.code == Keyboard::Left) {
					leftKeyPressed = true;
				}
				else if (event.key.code == Keyboard::Right) {
					rightKeyPressed = true;
				}
			}
			else if (event.type == Event::KeyReleased) {
				if (event.key.code == Keyboard::Left) {
					leftKeyPressed = false;
				}
				else if (event.key.code == Keyboard::Right) {
					rightKeyPressed = false;
				}
			}
		}

		if (circleRectCollision(ball.getPosition(), ball.getRadius(), paddle.getPosition(), paddle.getSize())) {
			// do bounce things
			ball.bouncePaddle(&paddle);
			ball.setPosition(Vector2f(ball.getPosition().x, paddle.getPosition().y - ball.getRadius()));
		}

		window.clear(Color(0, 0, 0));

		ball.update(dt_ms, WINDOW_WIDTH, WINDOW_HEIGHT);
		paddle.update(dt_ms, WINDOW_WIDTH, leftKeyPressed, rightKeyPressed);

		ball.draw(&window);
		paddle.draw(&window);

		// window.draw(debugText);

		window.display();
	}

	return 0;
}
