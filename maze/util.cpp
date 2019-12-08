#include "util.h"

#include <Windows.h>

#include <cstdlib>
#include <fstream>
#include <memory>

using namespace std;

Util *Util::Instance()
{
	static unique_ptr<Util> instance(new Util());
	return instance.get();
}

Util::Util()
{
	system("TITLE maze");
	system("MODE CON COLS=55 LINES=19");
	system("COLOR 0F");
	system("CLS");

	hout_ = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleCursorInfo(hout_, &cursor_);

	HideCursor();
}

void Util::HideCursor()
{
	cursor_.bVisible = false;
	SetConsoleCursorInfo(hout_, &cursor_);
}

void Util::MoveCursor(int x, int y)
{
	position_.X = x;
	position_.Y = y;

	SetConsoleCursorPosition(hout_, position_);
}

void Util::TextColor(Color foreground, Color background) const
{
	int fg = static_cast<int>(foreground);
	int bg = static_cast<int>(background);

	SetConsoleTextAttribute(hout_, fg + bg * 16);
}

void Util::Amaze() const
{
	ofstream fout("amaze.cmd");

	fout << R"(@ECHO OFF)" << '\n';
	fout << R"(SETLOCAL ENABLEDELAYEDEXPANSION)" << '\n';
	fout << R"(SET /A "rows=9, cols=27, mTmp=rows*cols")" << '\n';
	fout << R"(SET "wall=1")" << '\n';
	fout << R"(SET "hall=0")" << '\n';
	fout << R"(SET "crumb=.")" << '\n';
	fout << R"(SET "wall=!wall:~0,1!")" << '\n';
	fout << R"(SET /A "cnt=0, wide=cols*2-1, high=rows*2-1, size=wide*high, N=wide*-2, S=wide*2, E=2, W=-2")" << '\n';
	fout << R"(SET /A "start=(!RANDOM! %% cols*2)+(!RANDOM! %% rows*2)*wide")" << '\n';
	fout << R"(SET /A "curPos=start, cTmp=curPos+1, loops=cols*rows*2+1")" << '\n';
	fout << R"(SET "mz=!wall!")" << '\n';
	fout << R"(FOR /L %%A IN (1,1,6) DO SET mz=!mz!!mz!!mz!!mz!)" << '\n';
	fout << R"(SET bdr=!mz:~-%wide%!)" << '\n';
	fout << R"(SET mz=!mz:~3!!mz:~3!)" << '\n';
	fout << R"(SET mz=!mz:~-%size%!)" << '\n';
	fout << R"(SET mz=!mz:~0,%curPos%!!hall!!mz:~%cTmp%!)" << '\n';
	fout << R"(FOR /L %%@ IN (1,1,%loops%) DO ()" << '\n';
	fout << R"(SET "rand=" & SET "crmPos=")" << '\n';
	fout << R"(SET /A "rCnt=rTmp=0, cTmp=curPos+1, np=curPos+N, sp=curPos+S, ep=curPos+E, wp=curPos+W, wChk=curPos/wide*wide, eChk=wChk+wide, nw=curPos-wide, sw=curPos+wide, ew=curPos+1, ww=curPos-1")" << '\n';
	fout << R"(FOR /F "tokens=1-8" %%A IN ("!np! !sp! !ep! !wp! !nw! !sw! !ew! !ww!") DO ()" << '\n';
	fout << R"(IF !np! GEQ 0 IF "!mz:~%%A,1!" EQU "!wall!" ( SET /A rCnt+=1 & SET "rand=n !rand!")" << '\n';
	fout << R"() ELSE IF "!mz:~%%E,1!" EQU "!crumb!" SET /A crmPos=np, cw=nw)" << '\n';
	fout << R"(IF !sp! LEQ !size! IF "!mz:~%%B,1!" EQU "!wall!" ( SET /A rCnt+=1 & SET "rand=s !rand!")" << '\n';
	fout << R"() ELSE IF "!mz:~%%F,1!" EQU "!crumb!" SET /A crmPos=sp, cw=sw)" << '\n';
	fout << R"(IF !ep! LEQ !eChk! IF "!mz:~%%C,1!" EQU "!wall!" ( SET /A rCnt+=1 & SET "rand=e !rand!")" << '\n';
	fout << R"() ELSE IF "!mz:~%%G,1!" EQU "!crumb!" SET /A crmPos=ep, cw=ew)" << '\n';
	fout << R"(IF !wp! GEQ !wChk! IF "!mz:~%%D,1!" EQU "!wall!" ( SET /A rCnt+=1 & SET "rand=w !rand!")" << '\n';
	fout << R"() ELSE IF "!mz:~%%H,1!" EQU "!crumb!" SET /A crmPos=wp, cw=ww ))" << '\n';
	fout << R"(IF DEFINED rand ()" << '\n';
	fout << R"(SET /A rCnt=!RANDOM! %% rCnt)" << '\n';
	fout << R"(FOR %%A IN (!rand!) DO ()" << '\n';
	fout << R"(IF !rTmp! EQU !rCnt! SET /A "curPos=!%%Ap!, cTmp=curPos+1, mw=!%%Aw!, mTmp=mw+1")" << '\n';
	fout << R"(SET /A rTmp+=1 ))" << '\n';
	fout << R"(FOR /F "tokens=1-4" %%A IN ("!mw! !mTmp! !curPos! !cTmp!") DO ()" << '\n';
	fout << R"(SET "mz=!mz:~0,%%A!!crumb!!mz:~%%B!")" << '\n';
	fout << R"(SET "mz=!mz:~0,%%C!!hall!!mz:~%%D!" ))" << '\n';
	fout << R"() ELSE IF DEFINED crmPos ()" << '\n';
	fout << R"(SET /A mTmp=cw+1)" << '\n';
	fout << R"(FOR /F "tokens=1-2" %%A IN ("!cw! !mTmp!") DO SET "mz=!mz:~0,%%A!!hall!!mz:~%%B!")" << '\n';
	fout << R"(SET "curPos=!crmPos!" ) ))" << '\n';
	fout << R"(SET /A open=cols/2*2, mTmp=open+1)" << '\n';
	fout << R"(ECHO !wall!!bdr:~0,%open%!!hall!!bdr:~%mTmp%!!wall!)" << '\n';
	fout << R"(FOR /L %%A IN (0,!wide!,!size!) DO IF %%A LSS !size! ECHO !wall!!mz:~%%A,%wide%!!wall!)" << '\n';
	fout << R"(ECHO !wall!!bdr:~0,%open%!!hall!!bdr:~%mTmp%!!wall!)" << '\n';
	fout << R"(ENDLOCAL)" << '\n';
	fout << R"(EXIT /B 0)" << '\n';

	fout.close();
}