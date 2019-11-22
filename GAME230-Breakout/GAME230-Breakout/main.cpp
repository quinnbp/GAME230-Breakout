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
enum { MOUSE, KEYBOARD, AI };
enum { MENU, GAMEPLAY, DEAD, NEXTLEVEL};

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
	int gameState = GAMEPLAY;
	int lives = 3;
	int score = 0;
	bool ballReleased = false; // used to know if ball has been released from paddle

	// load fonts
	sf::Font font;
	if (!font.loadFromFile("arial.ttf")) {
		exit(-1);
	}
	sf::Font spaceFont;
	if (!spaceFont.loadFromFile("spacefont.otf")) {
		exit(-1);
	}

	// load sounds
	sf::SoundBuffer buffer_impact;
	if (!buffer_impact.loadFromFile("knock.wav")) {
		return -1;
	}
	sf::Sound sfx_paddleImpact;
	sfx_paddleImpact.setBuffer(buffer_impact);

	sf::SoundBuffer buffer_brick;
	if (!buffer_brick.loadFromFile("brick collide.flac")) {
		return -1;
	}
	sf::Sound sfx_brickCollide;
	sfx_brickCollide.setBuffer(buffer_brick);

	sf::SoundBuffer buffer_wall;
	if (!buffer_wall.loadFromFile("knock.wav")) {
		return -1;
	}
	sf::Sound sfx_wallCollide;
	sfx_wallCollide.setBuffer(buffer_wall);

	sf::SoundBuffer buffer_life;
	if (!buffer_life.loadFromFile("lifelost2.wav")) {
		return -1;
	}
	sf::Sound sfx_lifeLost;
	sfx_lifeLost.setBuffer(buffer_life);

	sf::SoundBuffer buffer_win;
	if (!buffer_win.loadFromFile("sweet sweet victory yeah.wav")) {
		return -1;
	}
	sf::Sound sfx_win;
	sfx_win.setBuffer(buffer_win);

	// load textures
	Texture brickTexture;
	if (!brickTexture.loadFromFile("brick.png", IntRect(0, 0, 100, 30))) {
		return -1;
	}
	Texture backgroundTexture;
	if (!backgroundTexture.loadFromFile("spacebg2.png")) {
		return -1;
	}
	Texture paddleTexture;
	if (!paddleTexture.loadFromFile("flagpiece.png")) {
		return -1;
	}

	// construct gameplay objects
	Ball ball = Ball();
	ball.setPosition(Vector2f(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f));
	ball.setVelocity(Vector2f(0.0f, 0.5f));

	Paddle paddle = Paddle(&paddleTexture);
	paddle.setPosition(Vector2f(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT - 30.0f));
	paddle.setControls(MOUSE);

	vector<Brick> bricks; // there are 30 bricks
	int cols = 0;
	int rows = 60;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 10; j++) {
			Brick brick = Brick(&brickTexture);
			brick.setPosition(Vector2f(cols, rows));
			bricks.push_back(brick);
			cols += 100;
		}
		rows += 30;
		cols = 0;
	}

	// lives text
	Text livesNumber;
	Text livesText;
	Text scoreNumber;
	Text scoreText;
	livesNumber.setFont(font);
	livesNumber.setFillColor(Color::White);
	livesText.setFont(font);
	livesText.setFillColor(Color::White);
	scoreNumber.setFont(font);
	scoreNumber.setFillColor(Color::White);
	scoreText.setFont(font);
	scoreText.setFillColor(Color::White);
	livesNumber.setPosition(100.0f, WINDOW_HEIGHT - 80.0f);
	livesText.setPosition(10.0f, WINDOW_HEIGHT - 80.0f);
	livesNumber.setString(to_string(lives));
	livesText.setString("Lives:");
	scoreNumber.setPosition(100.0f, WINDOW_HEIGHT - 40.0f);
	scoreText.setPosition(10.0f, WINDOW_HEIGHT - 40.0f);
	scoreNumber.setString(to_string(score));
	scoreText.setString("Score:");

	//background
	RectangleShape bg;
	bg.setSize(Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
	bg.setTexture(&backgroundTexture);
	bg.setPosition(0.0f, 0.0f);

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
		paddle.updateDelegator(dt_ms, WINDOW_WIDTH, leftKeyPressed, rightKeyPressed, Mouse::getPosition(window), ball.getPosition());
		if (ball.update(dt_ms, WINDOW_WIDTH, WINDOW_HEIGHT)) {
			sfx_wallCollide.play();
		}

		if (ball.getState() == OFFSCREEN) { // if ball is below screen
			// lose a life
			lives--;
			livesNumber.setString(to_string(lives));
			sfx_lifeLost.play();
			if (lives <= 0) {
				gameState = DEAD;
			}

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
				ball.paddleRelease(WINDOW_WIDTH, WINDOW_HEIGHT, 
					Vector2f(paddle.getPosition().x + paddle.getSize().x / 2.0f, paddle.getPosition().y));
			}
			debugText.setString("ONPADDLE");
		}
		else if (ball.getState() == FREE) { // we are in active state so check collisions
			// check ball-paddle collision
			if (circleRectCollision(ball.getPosition(), ball.getRadius(), paddle.getPosition(), paddle.getSize())) {
				// do paddle bounce things
				ball.bouncePaddle(&paddle);
				ball.setPosition(Vector2f(ball.getPosition().x, paddle.getPosition().y - ball.getRadius()));
				sfx_paddleImpact.play();
			}

			// check ball-brick collision
			bool brickActive = false;
			for (auto& brick : bricks) {
				if (brick.isActive()) {
					brickActive = true;
					if (circleRectCollision(ball.getPosition(), ball.getRadius(), brick.getPosition(), brick.getSize())) {
						// TODO: put better logic here for if hit side of brick
						ball.bounceBrick(&brick);
						score += rand() % 900 + 100;
						scoreNumber.setString(to_string(score));
						sfx_brickCollide.play();
					}
				}
			}

			if (!brickActive) {
				ball.setState(ONPADDLE);
				gameState = NEXTLEVEL;
				sfx_win.play();
			}

			debugText.setString("FREE");

		}

		window.clear(Color(0, 0, 0));
		window.draw(bg);

		// draw objects
		ball.draw(&window);
		paddle.draw(&window);
		for (Brick &brick : bricks) {
			brick.draw(&window);
		}

		//window.draw(debugText);
		window.draw(livesNumber);
		window.draw(livesText);
		window.draw(scoreNumber);
		window.draw(scoreText);
		window.display();
	}

	return 0;
}
