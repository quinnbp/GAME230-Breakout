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

const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 750;

enum { FREE, OFFSCREEN, ONPADDLE };

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

	// timing setup
	Clock clock = Clock();
	int dt_ms = 0;

	// key bools
	bool leftKeyPressed = false;
	bool rightKeyPressed = false;

	// tracker vars
	int lives = 3;
	bool ballReleased = false; // used for on-paddle state to know if ball has been sent

	// construct gameplay objects
	Ball ball = Ball();
	ball.setPosition(Vector2f(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f));
	ball.setVelocity(Vector2f(0.0f, 0.5f));

	Paddle paddle = Paddle();
	paddle.setPosition(Vector2f(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT - 15.0f));
	paddle.setMouseControl(true);

	vector<Brick> bricks;
	int cols = 0;
	int rows = 0;
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 10; j++) {
			Brick brick = Brick();
			brick.setPosition(Vector2f(cols, rows));
			bricks.push_back(brick);
			cols += 100;
		}
		rows += 30;
		cols = 0;
	}

	// load fonts
	sf::Font font;
	if (!font.loadFromFile("arial.ttf")) {
		exit(-1);
	}
	sf::Font spaceFont;
	if (!spaceFont.loadFromFile("spacefont.otf")) {
		exit(-1);
	}

	// lives text
	Text livesText;
	livesText.setPosition(WINDOW_WIDTH - 50.0f, WINDOW_HEIGHT / 2.0f);
	livesText.setFont(font);
	livesText.setFillColor(Color::White);
	livesText.setString(to_string(lives));

	// debug text
	Text debugText;
	debugText.setPosition(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f);
	debugText.setFont(font);
	debugText.setFillColor(Color::White);
	debugText.setString("");

	while (window.isOpen()) {

		// frame timing
		dt_ms = clock.getElapsedTime().asMilliseconds();
		clock.restart();

		// poll for control and system events
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
				else if (event.key.code == Keyboard::Space) {
					if (ball.getState() == ONPADDLE) {
						ballReleased = true;
					}
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

		// update dynamic game objects
		ball.update(dt_ms, WINDOW_WIDTH, WINDOW_HEIGHT);
		paddle.updateDelegator(dt_ms, WINDOW_WIDTH, leftKeyPressed, rightKeyPressed, Mouse::getPosition(window));

		if (ball.getState() == OFFSCREEN) { // if ball is below screen
			// lose a life
			lives--;
			livesText.setString(to_string(lives));
			// TODO: Logic here to end the game 

			// remove velocity and set to paddle state
			ball.setVelocity(Vector2f(0.0f, 0.0f));
			ball.setState(ONPADDLE);

			debugText.setString("OFFSCREEN");
		}
		else if (ball.getState() == ONPADDLE) { // if ball on paddle
			// set ball to paddle midpoint
			ball.setPosition(Vector2f(paddle.getPosition().x + paddle.getSize().x / 2.0f, paddle.getPosition().y - ball.getRadius())); 
			if (ballReleased) {
				ballReleased = false;
				ball.setState(FREE);
				ball.setVelocity(Vector2f(0.0f, 0.5f));
			}
			debugText.setString("ONPADDLE");
		}
		else if (ball.getState() == FREE) { // we are in active state so check collisions
			// check ball-paddle collision
			if (circleRectCollision(ball.getPosition(), ball.getRadius(), paddle.getPosition(), paddle.getSize())) {
				// do bounce things
				ball.bouncePaddle(&paddle);
				ball.setPosition(Vector2f(ball.getPosition().x, paddle.getPosition().y - ball.getRadius()));
			}

			// check ball-brick collision
			for (auto& brick : bricks) {
				if (circleRectCollision(ball.getPosition(), ball.getRadius(), brick.getPosition(), brick.getSize())) {
					if (brick.isActive()) {
						ball.bounceBrick(&brick);
						ball.setPosition(Vector2f(ball.getPosition().x, brick.getPosition().y + brick.getSize().y + ball.getRadius()));
					}
				}
			}

			debugText.setString("FREE");

		}

		window.clear(Color(0, 0, 0));

		// draw objects
		ball.draw(&window);
		paddle.draw(&window);
		for (Brick &brick : bricks) {
			brick.draw(&window);
		}

		window.draw(debugText);
		window.draw(livesText);
		window.display();
	}

	return 0;
}
