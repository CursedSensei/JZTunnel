#include "Frameworks.h"

int main() {

	SetConsoleTitleA("Checking Console Size");

	INPUT_RECORD out = { };

	while (out.EventType != KEY_EVENT) {
		COORD pos = { };
		pos.X = 0;
		pos.Y = 0;

		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);

		CONSOLE_SCREEN_BUFFER_INFO Consize = { };

		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &Consize);

		printf("Size:\nx = %6d\ny = %6d\n\n\n", Consize.dwSize.X, Consize.dwSize.Y);

		printf("Maximum:\nx = %6d\ny = %6d\n\n\n", Consize.dwMaximumWindowSize.X, Consize.dwMaximumWindowSize.Y);

		RECT winconSize = { };

		GetWindowRect(GetConsoleWindow(), &winconSize);

		printf("Window Size:\nx = %6d\ny = %6d\n\n\n\n", winconSize.right - winconSize.left, winconSize.bottom - winconSize.top);

		printf("All space:\nx = %6d\ny = %6d\n", Consize.dwSize.X, Consize.dwCursorPosition.Y);

		Sleep(20);

		DWORD trash;

		if (PeekConsoleInput(GetStdHandle(STD_INPUT_HANDLE), &out, 1, &trash) && trash > 0)
			ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), &out, 1, &trash);
	}

	SetConsoleTitleA("Size Checked!");

	return 0;
}