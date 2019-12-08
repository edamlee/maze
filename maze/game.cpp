#include "game.h"

#include <conio.h>

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <random>
#include <sstream>
#include <string>
#include <thread>

#include "util.h"

using namespace std;

Game *Game::Instance()
{
	static unique_ptr<Game> instance(new Game());
	return instance.get();
}

Game::Game()
{
	util_ = Util::Instance();
	for (int i = 0; i < kSize; ++i) scores_[i] = 0.0;
}

void Game::DrawTitle() const
{
	for (int i = 0; i < 3; ++i) cout << '\n';
	cout << "                __  __  ____ ___________ " << '\n';
	cout << "               |  |/  |/ _  /___  /  ___|" << '\n';
	cout << "               |      | |_| |  / /| |__  " << '\n';
	cout << "               | ||/| |  _  | / / |  __| " << '\n';
	cout << "               | |  | | | | |/ /__| |___ " << '\n';
	cout << "               |_|  |_|_| |_/_____|_____|" << '\n';
}

int Game::DrawMenu() const
{
	int x = 24;
	int y = 12;

	util_->MoveCursor(x - 2, y);
	cout << "> 게임 시작";

	util_->MoveCursor(x, y + 1);
	cout << "게임 기록";

	util_->MoveCursor(x + 2, y + 2);
	cout << "종 료";

	while (true) {
		switch (KeyState()) {
			case Key::kUp:
				if (y > 12) {
					util_->MoveCursor(x - 2, y);
					cout << ' ';
					util_->MoveCursor(x - 2, --y);
					cout << '>';
				}
				break;
			case Key::kDown:
				if (y < 14) {
					util_->MoveCursor(x - 2, y);
					cout << ' ';
					util_->MoveCursor(x - 2, ++y);
					cout << '>';
				}
				break;
			case Key::kEnter: return y - 12;
			default: continue;
		}
	}
}

Key Game::KeyState() const
{
	switch (static_cast<Key>(_getch())) {
		case Key::kFunction:
		case Key::kArrow:
			switch (static_cast<Key>(_getch())) {
				case Key::kUp: return Key::kUp;
				case Key::kLeft: return Key::kLeft;
				case Key::kRight: return Key::kRight;
				case Key::kDown: return Key::kDown;
				default: return Key::kError;
			}
		case Key::kEnter: return Key::kEnter;
		default: return Key::kError;
	}
}

void Game::CreateMap()
{
	util_->Amaze();
	system("amaze.cmd > maze.txt");
	remove("amaze.cmd");

	ostringstream oss;
	ifstream fin("maze.txt");

	while (fin >> map_) oss << map_;

	fin.close();
	remove("maze.txt");

	map_ = oss.str();
	map_[27] = '@';

	random_device rd;
	mt19937 mt(rd());
	uniform_int_distribution<> dist(56, 988);

	for (int i = 0; i < 5; ++i) {
		int index = dist(mt);
		if (map_[index] == '0') {
			map_[index] = '*';
		} else {
			--i;
		}
	}
}

void Game::DrawMap()
{
	system("CLS");

	for (int i = 0; i < map_.length(); ++i) {
		switch (map_[i]) {
			case '0':
				util_->TextColor(Color::kBlack, Color::kBlack);
				break;
			case '1':
				util_->TextColor(Color::kWhite, Color::kWhite);
				break;
			case '@':
				location_ = i;
				util_->TextColor(Color::kYellow, Color::kBlack);
				break;
			case '*':
				util_->TextColor(Color::kYellow, Color::kBlack);
				break;
			default:
				util_->TextColor(Color::kWhite, Color::kBlack);
				break;
		}
		cout << map_[i];
	}
}

void Game::Start()
{
	while (KeyState() != Key::kDown) continue;

	item_ = 0;
	playing_ = true;
	MovePlayer(0, 1);

	auto start = chrono::system_clock::now();

	while (playing_) {
		switch (KeyState()) {
			case Key::kEnter:
				playing_ = false;
				break;
			case Key::kUp:
				MovePlayer(0, -1);
				break;
			case Key::kLeft:
				MovePlayer(-1, 0);
				break;
			case Key::kRight:
				MovePlayer(1, 0);
				break;
			case Key::kDown:
				MovePlayer(0, 1);
				break;
			default: continue;
		}
	}

	auto end = chrono::system_clock::now();
	chrono::duration<double> elapse = end - start;

	util_->MoveCursor(23, 8);

	if (location_ == 1017) {
		cout << elapse.count() << "초";
		scores_[kSize - 1] = elapse.count();

		if (scores_[0] == 0.0) {
			scores_[0] = scores_[kSize - 1];
			sort(scores_, scores_ + kSize - 1);
		} else {
			sort(scores_, scores_ + kSize);
		}
	} else {
		cout << "Game Over";
	}

	this_thread::sleep_for(1.5s);
}

void Game::MovePlayer(int x, int y)
{
	int index = location_ + x + y * 55;

	if (map_[index] == '*') {
		++item_;
		map_[index] = '0';
	}

	if (map_[index] == '0') {
		util_->TextColor(Color::kWhite, Color::kBlack);
		util_->MoveCursor(location_ % 55, location_ / 55);
		cout << ' ';

		location_ = index;

		util_->TextColor(Color::kYellow, Color::kBlack);
		util_->MoveCursor(location_ % 55, location_ / 55);
		cout << '@';

		if (location_ == 1017) {
			if (item_ == 5) {
				playing_ = false;
			} else {
				MovePlayer(0, -1);
			}
		}
	}
}

void Game::DrawScore() const
{
	system("CLS");

	if (scores_[kSize - 1] == 0.0) {
		util_->MoveCursor(10, 1);
		cout << "기록이 없습니다.";
	} else {
		int rank = 1;
		for (int i = 0; i < kSize - 1; ++i) {
			if (scores_[i] > 0.0) {
				util_->MoveCursor(10, rank * 2 - 1);
				cout << rank << "등: " << scores_[i] << "초";
				++rank;
			}
		}
	}

	while (KeyState() != Key::kEnter) continue;
}