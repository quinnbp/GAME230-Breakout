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

vector<Brick> brickLevelSetup(int level, Texture* brickTexture) {
	vector<Brick> bricks; // there are 30 bricks
	int cols = 0;
	int rows = 60;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 10; j++) {
			Brick brick = Brick(brickTexture);
			brick.setPosition(Vector2f(cols, rows));
			cols += 100;

			// level 1: 1, 1, 1
			// level 2: 2, 1, 1
			// level 3: 3, 2, 1
			if (level == 2) { 
				if (i == 0) {
					brick.setHits(2);
				}
			}
			else if (level == 3) { 
				if (i == 0) {
					brick.setHits(3);
				}
				else if (i == 1) {
					brick.setHits(2);
				}
			}

			bricks.push_back(brick);
		}
		rows += 30;
		cols = 0;
	}
	return bricks;
}

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
	// load fonts
	sf::Font normalFont;
	if (!normalFont.loadFromFile("arial.ttf")) {
		exit(-1);
	}
	sf::Font spaceFont;
	if (!spaceFont.loadFromFile("spacefont.otf")) {
		exit(-1);
	}

	// load sounds
	sf::SoundBuffer buffer_launch;
	if (!buffer_launch.loadFromFile("launch1.wav")) {
		return -1;
	}
	sf::Sound sfx_launch;
	sfx_launch.setBuffer(buffer_launch);

	sf::SoundBuffer buffer_start;
	if (!buffer_start.loadFromFile("gameStart1.wav")) {
		return -1;
	}
	sf::Sound sfx_start;
	sfx_start.setBuffer(buffer_start);

	sf::SoundBuffer buffer_impact;
	if (!buffer_impact.loadFromFile("impact1.wav")) {
		return -1;
	}
	sf::Sound sfx_paddleImpact;
	sfx_paddleImpact.setBuffer(buffer_impact);

	sf::SoundBuffer buffer_brick;
	if (!buffer_brick.loadFromFile("impact1.wav")) {
		return -1;
	}
	sf::Sound sfx_brickCollide;
	sfx_brickCollide.setBuffer(buffer_brick);
	sfx_brickCollide.setPitch(0.5);

	sf::SoundBuffer buffer_wall;
	if (!buffer_wall.loadFromFile("impact1.wav")) {
		return -1;
	}
	sf::Sound sfx_wallCollide;
	sfx_wallCollide.setBuffer(buffer_wall);
	sfx_wallCollide.setPitch(0.75);

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

	/*Music music;
	if (!music.openFromFile("pongdraft02.wav")) {
		exit(-1);
	}
	music.setLoop(true);
	music.setVolume(50);
	music.play();*/

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

	// create window
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
	int gameState = MENU;
	int level = 1;
	int lives = 3;
	int score = 0;
	bool ballReleased = false; // used to know if ball has been released from paddle
	bool deadToNext = false;
	int leftMenu = 0;

	// construct gameplay objects
	Ball ball = Ball();
	ball.setPosition(Vector2f(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f));
	ball.setVelocity(Vector2f(0.0f, 0.5f));

	Paddle paddle = Paddle(&paddleTexture);
	paddle.setPosition(Vector2f(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT - 50.0f));
	paddle.setControls(MOUSE);

	vector<Brick> bricks = brickLevelSetup(1, &brickTexture);

	// initialize text objects
	Text livesNumber;
	Text livesText;
	Text scoreNumber;
	Text scoreText;
	Text deathText;
	Text deathTextShadow;
	Text titleText;
	Text titleTextShadow;
	Text menuText;

	menuText.setFont(normalFont);
	titleText.setFont(spaceFont);
	titleTextShadow.setFont(spaceFont);
	deathText.setFont(spaceFont);
	deathTextShadow.setFont(spaceFont);
	livesNumber.setFont(normalFont);
	livesText.setFont(normalFont);
	scoreNumber.setFont(normalFont);
	scoreText.setFont(normalFont);

	titleText.setCharacterSize(50);
	titleTextShadow.setCharacterSize(50);
	deathText.setCharacterSize(40);
	deathTextShadow.setCharacterSize(40);

	titleText.setRotation(-5.0f);
	titleTextShadow.setRotation(-5.0f);
	deathText.setRotation(-5.0f);
	deathTextShadow.setRotation(-5.0f);

	menuText.setFillColor(Color::White);
	titleText.setFillColor(Color::White);
	titleTextShadow.setFillColor(Color::Red);
	deathText.setFillColor(Color::White);
	deathTextShadow.setFillColor(Color::Red);
	livesNumber.setFillColor(Color::White);
	livesText.setFillColor(Color::White);
	scoreNumber.setFillColor(Color::White);
	scoreText.setFillColor(Color::White);

	menuText.setPosition(WINDOW_WIDTH / 2.0f - 150.0f, WINDOW_HEIGHT / 2.0f - 100.0f);
	titleText.setPosition(WINDOW_WIDTH / 2.0f - 300.0f, WINDOW_HEIGHT / 2.0f - 200.0f);
	titleTextShadow.setPosition(WINDOW_WIDTH / 2.0f - 303.0f, WINDOW_HEIGHT / 2.0f - 205.0f);
	deathText.setPosition(WINDOW_WIDTH / 2.0f - 100.0f, WINDOW_HEIGHT / 2.0f);
	deathTextShadow.setPosition(WINDOW_WIDTH / 2.0f - 103.0f, WINDOW_HEIGHT / 2.0f - 3.0f);
	livesNumber.setPosition(WINDOW_WIDTH - 30.0f, WINDOW_HEIGHT - 40.0f);
	livesText.setPosition(WINDOW_WIDTH - 120.0f, WINDOW_HEIGHT - 40.0f);
	scoreNumber.setPosition(100.0f, WINDOW_HEIGHT - 40.0f);
	scoreText.setPosition(10.0f, WINDOW_HEIGHT - 40.0f);

	menuText.setString("[1] Play with mouse control\n[2] Play with keyboard control\n[3] Demo mode\n[4] Quit");
	titleText.setString("space breakout");
	titleTextShadow.setString("space breakout");
	deathText.setString("you died");
	deathTextShadow.setString("you died");
	livesNumber.setString(to_string(lives));
	livesText.setString("Lives:");
	scoreNumber.setString(to_string(score));
	scoreText.setString("Score:");

	// initialize background
	RectangleShape bg;
	bg.setSize(Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
	bg.setTexture(&backgroundTexture);
	bg.setPosition(0.0f, 0.0f);

	// initialize debug objects
	Text debugText;
	debugText.setPosition(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f);
	debugText.setFont(normalFont);
	debugText.setFillColor(Color::White);
	debugText.setString("");

	RectangleShape midline;
	midline.setSize(Vector2f(2.0f, WINDOW_HEIGHT));
	midline.setPosition(WINDOW_WIDTH / 2.0f - 1.0f, 0.0f);

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
				else if (event.key.code == Keyboard::Num1) {
					if (gameState == MENU) {
						leftMenu = 1;
					}
				}
				else if (event.key.code == Keyboard::Num2) {
					if (gameState == MENU) {
						leftMenu = 2;
					}
				}
				else if (event.key.code == Keyboard::Num3) {
					if (gameState == MENU) {
						leftMenu = 3;
					}
				}
				else if (event.key.code == Keyboard::Num4) {
					if (gameState == MENU) {
						window.close();
					}
				}
				else if (event.key.code == Keyboard::Space) {
					if (paddle.getControls() == AI) {
						gameState = DEAD;
					}
					if (ball.getState() == ONPADDLE) {
						ballReleased = true;
					}
					if (gameState == DEAD) {
						deadToNext = true;
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

		window.clear(Color(0, 0, 0)); // window clear and bg in any state
		window.draw(bg);

		if (gameState == MENU) {
			window.draw(titleTextShadow);
			window.draw(titleText);
			window.draw(menuText);

			if (leftMenu > 0) {
				if (leftMenu == 1) { // 1p mouse
					paddle.setControls(MOUSE);
				}
				else if (leftMenu == 2) {
					paddle.setControls(KEYBOARD);
				}
				else if (leftMenu == 3) {
					paddle.setControls(AI);
				}

				leftMenu = 0;
				gameState = GAMEPLAY;
				sfx_start.play();
			}
		}
		else if (gameState == DEAD) {
			window.draw(bg);
			window.draw(deathTextShadow);
			window.draw(deathText);
			window.draw(scoreNumber);
			window.draw(scoreText);

			if (deadToNext) { // return to menu on [space]
				gameState = MENU;
				// put score text back in corner
				scoreNumber.setPosition(100.0f, WINDOW_HEIGHT - 40.0f);
				scoreText.setPosition(10.0f, WINDOW_HEIGHT - 40.0f);
				// reset score/lives/level and their text objects
				lives = 3;
				score = 0;
				level = 1;
				scoreNumber.setString(to_string(score));
				livesNumber.setString(to_string(lives));
				// reset bricks for level 1
				bricks = brickLevelSetup(1, &brickTexture);
				// reset ball/paddle
				ballReleased = false;
				ball.setState(ONPADDLE);
				ball.setSpeed(0.4); // base speed reset
			}
		}
		else if (gameState == NEXTLEVEL) {
			// reset bricks and increase ball speed
			level++;
			if (level > 3) { // reset after all levels
				level = 1;
				ball.setSpeed(ball.getSpeed() * 1.1f); // additional speed increase
			}
			bricks = brickLevelSetup(level, &brickTexture);
			ball.setSpeed(ball.getSpeed() * 1.1f);
			gameState = GAMEPLAY;

		} 
		else if (gameState == GAMEPLAY) {

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
					deadToNext = false; // set in order to wait on death screen
					scoreNumber.setPosition(WINDOW_WIDTH / 2.0f + 45.0f, WINDOW_HEIGHT / 2.0f + 50.0f); // move score to center
					scoreText.setPosition(WINDOW_WIDTH / 2.0f - 45.0f, WINDOW_HEIGHT /2.0f + 50.0f);
				}

				// remove velocity and set to paddle state
				ball.setVelocity(Vector2f(0.0f, 0.0f));
				ball.setState(ONPADDLE);

				debugText.setString("OFFSCREEN");
			}
			else if (ball.getState() == ONPADDLE) { // if ball on paddle
				// set ball to paddle midpoint
				ball.setPosition(Vector2f(paddle.getPosition().x + paddle.getSize().x / 2.0f, paddle.getPosition().y - ball.getRadius()));
				if (paddle.getControls() == AI) { // play if AI control
					ballReleased = true;
				}
				if (ballReleased) {
					ballReleased = false;
					ball.setState(FREE);
					ball.paddleRelease(WINDOW_WIDTH, WINDOW_HEIGHT,
						Vector2f(paddle.getPosition().x + paddle.getSize().x / 2.0f, paddle.getPosition().y));
					sfx_launch.play();
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
							ball.bounceBrick(&brick);
							brick.resolveHit();
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

			// draw objects
			ball.draw(&window);
			paddle.draw(&window);
			for (Brick& brick : bricks) {
				brick.draw(&window);
			}

			//window.draw(debugText);

			window.draw(livesNumber);
			window.draw(livesText);
			window.draw(scoreNumber);
			window.draw(scoreText);
		}

		window.display();
	}

	return 0;
}
