#ifndef MAZE_UTIL_H_
#define MAZE_UTIL_H_

#include <Windows.h>

enum class Color {
	kBlack,
	kBlue,
	kGreen,
	kCyan,
	kRed,
	kMagenta,
	kBrown,
	kLightGray,
	kDarkGray,
	kLightBlue,
	kLightGreen,
	kLightCyan,
	kLightRed,
	kLightMagenta,
	kYellow,
	kWhite
};

class Util {
  public:
	static Util *Instance();
	Util(const Util &) = delete;
	Util &operator=(const Util &) = delete;
	void HideCursor();
	void MoveCursor(int x, int y);
	void TextColor(Color foreground, Color background) const;
	void Amaze() const;

  private:
	Util();
	HANDLE hout_;
	CONSOLE_CURSOR_INFO cursor_;
	COORD position_;
};

#endif