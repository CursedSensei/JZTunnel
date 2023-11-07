#pragma once

int initializeWinMutex() {
	LPCSTR winMutexText = "JZTunnel Application";

	HWND winMutexWindow = FindWindowA(winMutexText, "Client");

	if (winMutexText) {
		return 1;
	}

	WNDCLASSA winMutex;
	ZeroMemory(&winMutex, sizeof(WNDCLASSA));

	winMutex.hInstance = (HINSTANCE)GetModuleHandleA(NULL);
	winMutex.lpszClassName = winMutexText;

	RegisterClassA(&winMutex);
	//Create window

	winMutexWindow = CreateWindowA(
		winMutexText,
		"Client",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		(HINSTANCE)GetModuleHandleA(NULL),
		NULL);

	if (!winMutexWindow) return 1;

	CloseHandle(winMutexWindow);

	return 0;
} 

p_Sockets_Data initializeSocketDataVar(short int clientport) {
	p_Sockets_Data sockdata = (p_Sockets_Data)malloc(sizeof(Sockets_Data));
	sockdata->client_port = clientport;

	return sockdata;
}