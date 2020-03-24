#include <SFML/Graphics.hpp>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <vector>
#include <math.h>

using namespace sf;
class Figure;
class Player;
class Game;
class Finish;

enum class Gdirection
{
	UP = 1,
	DOWN = 2,
	LEFT = 3,
	RIGHT = 4
};

class Object
{
private:


public:

	/////////////

	virtual void update(RenderWindow &, float, Event &, int) = 0;

	virtual void Draw(RenderWindow&) = 0;

	virtual RectangleShape getModel() = 0;

};

class Player: public Object
{

private:

	// model ref
	RectangleShape model;

	//Step size == size
	int size;	

public:

	Player() {};
	// size, r, g, b, window
	Player(int s, int r, int g, int b, RenderWindow &window)
	{
		
		this->size = s;
		model.setSize(Vector2f(size, size));
		
		model.move(size, size);
		model.setFillColor(Color(r, g, b));	

	}

	virtual RectangleShape getModel() override
	{
		return model;
	}
	
	void move(Gdirection way, RenderWindow &window, float deltatime)
	{
		
		switch (way)
		{
		case Gdirection::UP:     
			if (model.getPosition().y >= size) model.move(0, -size); 

			else model.setPosition(model.getPosition().x, window.getSize().y - size); break;

		case Gdirection::DOWN:

			// For smooth small distance movement
			// if (model.getPosition().y<=window.getSize().y) model.move(0, size*deltatime); break;

			if (model.getPosition().y <= window.getSize().y-size*2) model.move(0, size);

			else model.setPosition(model.getPosition().x, 0); break;

		case Gdirection::LEFT:

			if (model.getPosition().x>=size ) model.move(-size, 0); 

			else model.setPosition(window.getSize().x - size, model.getPosition().y); break;

		case Gdirection::RIGHT:

			if (model.getPosition().x<=window.getSize().x-size*2) model.move(size, 0); 

			else model.setPosition(0, model.getPosition().y); break;

		}	
	}

	// collider == 1 -> death; collider == 1 -> win 
	virtual void update(RenderWindow &window, float deltatime, Event &event, int collider) override
	{

		// handling live key actions
		// for smooth movement
	/*	if (Keyboard::isKeyPressed(Keyboard::W))
		{
			move(Gdirection::UP, window, deltatime);
		}
		if (Keyboard::isKeyPressed(Keyboard::S))
		{
			move(Gdirection::DOWN, window, deltatime);
		}
		if (Keyboard::isKeyPressed(Keyboard::A))
		{
			move(Gdirection::LEFT, window, deltatime);
		}
		if (Keyboard::isKeyPressed(Keyboard::D))
		{
			move(Gdirection::RIGHT, window, deltatime);
		}*/

		if (event.type == Event::KeyPressed)
		{
			if (event.key.code == Keyboard::W)
			{
				move(Gdirection::UP, window, deltatime);
			}
			if (event.key.code == Keyboard::S)
			{
				move(Gdirection::DOWN, window, deltatime);
			}
			if (event.key.code == Keyboard::A)
			{
				move(Gdirection::LEFT, window, deltatime);
			}
			if (event.key.code == Keyboard::D)
			{
				move(Gdirection::RIGHT, window, deltatime);
			}
		}

		if (collider == 1) std::cout << "Wasted" << std::endl;
		if (collider == 2) std::cout << "You win dat game" << std::endl;
		

	}

	void Draw(RenderWindow &window) override
	{
		window.draw(model);
	}


};


class Hole: public Object
{

private:

	RectangleShape model;

	//Step size == size
	int size;

public:

	//Need default constructor to create an object via vector
	Hole(){};

	Hole(int s, int r, int g, int b, RenderWindow &window)
	{

		this->size = s;

		model.setSize(Vector2f(size, size));

		model.setFillColor(Color(r, g, b));

	}

	void place(int x, int y)
	{
		model.setPosition(x, y);
	}

	virtual RectangleShape getModel() override
	{
		return model;
	}

	void Draw(RenderWindow &window) override
	{
		window.draw(model);
	}

	virtual void update(RenderWindow &window, float deltatime, Event &event, int) override
	{

	}

};

class Finish: public Object
{
private:
	RectangleShape model;

	//Step size == size
	int size;

public:

	//Need default constructor to create an object via vector
	Finish() {};

	Finish(int s, int r, int g, int b, RenderWindow &window)
	{

		this->size = s;

		model.setSize(Vector2f(size, size));

		model.setFillColor(Color(r, g, b));

	}

	void place(int x, int y)
	{
		model.setPosition(x, y);
	}

	virtual RectangleShape getModel() override
	{
		return model;
	}

	void Draw(RenderWindow &window) override
	{
		window.draw(model);
	}

	virtual void update(RenderWindow &window, float deltatime, Event &event, int) override
	{

	}

};

class Game
{
protected :

	std::vector<Hole> generate_Holes(int difficulty, int playerSize, RenderWindow &window)
	{

		srand(time(NULL));

		int holes_number = 0;

		if (difficulty == 1)
		{
			// up to 30 holes
			holes_number = 30 + rand() % 75;
		}
		if (difficulty == 2)
		{
			// up to 200 holes
			holes_number = 150 + rand() % 200;
		}

		std::vector<std::pair<int, int> > coordinates(holes_number);

		// Holes parameters here!!! //
		std::vector<Hole> holes(holes_number, Hole(playerSize, 255, 0, 0, window));

		for (int i = 0; i < holes_number; i++)
		{
			int rand_x = playerSize + rand() % (window.getSize().x - playerSize + 1);
			int rand_y = playerSize + rand() % (window.getSize().y - playerSize + 1);

			holes[i].place(rand_x, rand_y);
		}

		return holes;

	}

	// returns object not reference because object generated by function will get deleted as soon as function complites and return value is assigned
	Finish generate_Finish(int playerSize, std::vector<Hole> &holes, RenderWindow &window)
	{
		srand(time(NULL));

		int rand_x = 0;
		int rand_y = 0;

		int try_counter = 0;

		while (true)
		{
			bool ck_holes_collision = 0;
			//dictance between finish block and hole block centres
			float distance = 0;

			rand_x = playerSize + rand() % (window.getSize().x - playerSize + 1);
			rand_y = playerSize + rand() % (window.getSize().y - playerSize + 1);

			for (int i = 0; i < holes.size(); i++)
			{
				
				distance = sqrt(pow((rand_x + (playerSize / 2) - holes[i].getModel().getPosition().x + (playerSize / 2)), 2)
					+ pow((rand_y + (playerSize / 2) - holes[i].getModel().getPosition().y + (playerSize / 2)), 2));
				//collision is true
				if (distance < playerSize)
				{
					ck_holes_collision = 1;
					try_counter++;
					break;
				}
			}

			if (!ck_holes_collision) 
				break;
			

		}

		Finish generated_finish(playerSize, 0, 255, 0, window);

		generated_finish.place(rand_x, rand_y);

		return generated_finish;

	}

	void draw_holes(std::vector<Hole> & holes, RenderWindow &window)
	{
		for (int i = 0; i < holes.size(); i++)
		{
			holes[i].Draw(window);
		}
	}

	template<class T1, class T2>
	bool ck_collision(T1 object1, T2 object2, RenderWindow &window)
	{
		if (object1.getModel().getGlobalBounds().intersects(object2.getModel().getGlobalBounds()))
		{
			//std::cout << "Intersection" << std::endl;
			return 1;
		}
		else
		{
			return 0;
		}
	}

	Sprite create_endgame_sprite(char * filename, Texture &texture, RenderWindow & window)
	{
		if (!texture.loadFromFile(filename))
		{
			std::cout << "Win sprite texture not loaded" << std::endl;
		}

		Sprite sprite(texture);
		return sprite;
	}

	void show_endgame_window(Sprite &sprite, RenderWindow &window)
	{
		window.draw(sprite);
	}


public:

	Game()
	{
		draw();
	}

	virtual void draw()
	{
		// Anti-aliasing
		ContextSettings settings;
		settings.antialiasingLevel = 8;

		// Main window
		RenderWindow window(VideoMode(500, 500), "2d DESTRUCTOR!!!", Style::Default, settings);
		window.setFramerateLimit(60); // Frame limit to less cpu load

		int playerSize = 10;

		Texture game_start_texture;
		Sprite game_start_sprite = create_endgame_sprite("Game start sprite.png", game_start_texture, window);

				
		// Win/Lose sprites and textures //
		Texture win_texture, lose_texture;
		Sprite win_sprite = create_endgame_sprite("You win sprite.png", win_texture, window);
		Sprite lose_sprite = create_endgame_sprite("You lose sprite.png", lose_texture, window);

		int game_start = 0;

		while (true)
		{
			// 0 -> default; 1 -> restart; 2-> exit
			int game_status = 0;

			int player_collision = 0;

			Player p1(playerSize, 14, 222, 50, window);

			// My danger pits -> red color
			std::vector<Hole> pits = generate_Holes(2, playerSize, window);

			//My finish -> green color
			Finish finish = generate_Finish(playerSize, pits, window);

			float deltatime = 0.f;

			Clock clock;
			Time time = clock.getElapsedTime();

			while (window.isOpen())
			{
				deltatime = clock.restart().asSeconds();
				Event event;

				if (game_start == 0)
				{
					while (window.pollEvent(event))
					{
						//window.clear(Color(250, 220, 100, 0));
						if (!Keyboard::isKeyPressed(Keyboard::Enter))
						{
							show_endgame_window(game_start_sprite, window);

						}
						else
						{
							game_start = 1;
							break;
						}
					}
				}
				else
				{
					//If gamelpay is running
					if (!player_collision)
					{
						while (window.pollEvent(event))
						{

							if (event.type == Event::Closed)
							{
								window.close();
								return;
							}
								

							for (int i = 0; i < pits.size(); i++)
								if (ck_collision(p1, pits[i], window) == 1)
								{
									player_collision = 1;
									break;
								}

							if (ck_collision(p1, finish, window) == 1) player_collision = 2;

							// Alive condition
							if (player_collision == 0) p1.update(window, deltatime, event, 0);

							// Lose condition
							else if (player_collision == 1)	p1.update(window, deltatime, event, 1);

							// Win condition
							else if (player_collision == 2)	p1.update(window, deltatime, event, 2);

						}

						window.clear(Color(250, 220, 100, 0));

						draw_holes(pits, window);
						finish.Draw(window);
						p1.Draw(window);
					}

					// Lose condition
					if (player_collision == 1)
					{
						window.clear(Color(250, 220, 100, 0));

						show_endgame_window(lose_sprite, window);
						while (window.pollEvent(event))
						{
							if (Keyboard::isKeyPressed(Keyboard::F2))
							{
								game_status = 1;
							}
							if (Keyboard::isKeyPressed(Keyboard::Escape))
							{
								game_status = 2;
								window.close();
								return;
							}
						}
					}
					// Win condition
					else if (player_collision == 2)
					{
						window.clear(Color(250, 220, 100, 0));

						show_endgame_window(win_sprite, window);
						while (window.pollEvent(event))
						{
							if (Keyboard::isKeyPressed(Keyboard::F2))
							{
								game_status = 1;
							}
							if (Keyboard::isKeyPressed(Keyboard::Escape))
							{
								game_status = 2;
								window.close();
								return;
							}
						}
					}

				}
				
				
				window.display();

				if (game_status == 1) break;
			}
		}

	}

};

int main()
{
	
	Game G1;
	
	return 0;
}