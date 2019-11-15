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

	Color collisionColor = Color(0, 0, 0);

	while (window.isOpen()) {

		dt_ms = clock.getElapsedTime().asMilliseconds();
		clock.restart();

		Event event;
		while (window.pollEvent(event)) {
			if (event.type == Event::Closed) {
				window.close();
			}
		}

		if (circleRectCollision(ball.getPosition(), ball.getRadius(), paddle.getPosition(), paddle.getSize())) {
			// do bounce things
			collisionColor = Color(255, 0, 0);
		}

		window.clear(collisionColor);

		ball.update(dt_ms, WINDOW_WIDTH, WINDOW_HEIGHT);
		paddle.update(dt_ms, WINDOW_WIDTH, WINDOW_HEIGHT);

		ball.draw(&window);
		paddle.draw(&window);

		window.display();
	}

	return 0;
}
