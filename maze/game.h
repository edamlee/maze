#ifndef MAZE_GAME_H_
#define MAZE_GAME_H_

#include <string>

enum class Key {
	kError = -1,
	kFunction = 0,
	kEnter = 13,
	kUp = 72,
	kLeft = 75,
	kRight = 77,
	kDown = 80,
	kArrow = 224
};

class Util;

class Game {
  public:
	static constexpr int kSize = 10;
	static Game *Instance();
	Game(const Game &) = delete;
	Game &operator=(const Game &) = delete;
	void DrawTitle() const;
	int DrawMenu() const;
	Key KeyState() const;
	void CreateMap();
	void DrawMap();
	void Start();
	void MovePlayer(int x, int y);
	void DrawScore() const;

  private:
	Game();
	Util *util_;
	std::string map_;
	int location_;
	int item_;
	bool playing_;
	double scores_[kSize];
};

#endif