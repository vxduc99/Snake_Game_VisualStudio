﻿#include <Windows.h>
#include <iostream>
#include <time.h>
#include <stdio.h>
#include <conio.h>
#include <thread>
#include <string>
#include <fstream>
using namespace std;

#define MAX_SIZE_SNAKE 50
#define MAX_SIZE_FOOD 10
#define MAX_SPEED 3
//Biến toàn cục
POINT snake[50]; //Con rắn
POINT food[10]; // Thức ăn
int CHAR_LOCK;//Biến xác định hướng không thể di chuyển (Ở một thời điểm có một hướng rắn không thể di chuyển)
int MOVING;//Biến xác định hướng di chuyển của snake (Ở một thời điểm có ba hướng rắn có thể di chuyển)
int SPEED;// Có thể hiểu như level, level càng cao thì tốc độ càng nhanh
int HEIGH_CONSOLE, WIDTH_CONSOLE;// Độ rộng và độ cao của màn hình console
int FOOD_INDEX; // Chỉ số food hiện hành đang có trên màn hình
int SIZE_SNAKE; // Kích thước của snake, lúc đầu có 6 và tối đa lên tời 10
int STATE; // Trạng thái sống hay chết của rắn
int Score = 0;
void Drawsnake(string);
void MoveRight();
void Drawsnakehide(const char *);

string snakee = "1860005318600015";
// hàm fix màn hình console 
void FixConsoleWindow()
{
	HWND consoleWindow = GetConsoleWindow();
	LONG style = GetWindowLong(consoleWindow, GWL_STYLE);
	style = style & ~(WS_MAXIMIZEBOX) & ~(WS_THICKFRAME);
	SetWindowLong(consoleWindow, GWL_STYLE, style);
}
// hàm tọa độ
void GotoXY(int x, int y)
{
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}
// xay dung cong
void xaycong()
{
	int y = 10;
	GotoXY(40, y); cout << 'H';
	GotoXY(40, y + 2); cout << 'H';
	for (int i = 1; i < 4; i++)
	{
		GotoXY(41, y); cout << 'H';
		y++;
	}
}

void xoacong()
{
	int y = 10;
	GotoXY(40, y); cout << ' ';
	GotoXY(40, y + 2); cout << ' ';
	for (int i = 1; i < 4; i++)
	{
		GotoXY(41, y); cout << ' ';
		y++;
	}
}

//xu li vao cong
void xulyquacong(int tem) {
	int j = 0;
	while (j < tem)
	{
		Drawsnakehide(" ");
		for (int i = 0; i < SIZE_SNAKE - 1; i++)
		{
			snake[i].x = snake[i + 1].x;
			snake[i].y = snake[i + 1].y;
		}
		SIZE_SNAKE--;
		j++;

		Drawsnake(snakee);
		Sleep(300);
	}
}

// kiểm tra tọa độ đầu vào có trùng với bất kì một phần tử nào trong mảng snake hay không
bool IsValid(int x, int y)
{
	for (int i = 0; i < SIZE_SNAKE; i++) {
		if (snake[i].x == x && snake[i].y == y) {
			return false;
		}

	}
	return true;
}

// kiểm tra thức ăn không trùng với cổng
bool IsRoad(int x, int y)
{
	int w = 40, h = 10;
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (x == w && y == h)
			{
				return false;
			}
			h++;
		}
		h = 10;
		w++;
	}
	return true;
}

// hàm này giúp khởi tạo giá trị vị trí cho các thức ăn
void GenerateFood()
{
	int x, y;
	srand(time(NULL));
	for (int i = 0; i < MAX_SIZE_FOOD; i++) {
		do {
			x = rand() % (WIDTH_CONSOLE - 1) + 1;
			y = rand() % (HEIGH_CONSOLE - 1) + 1;
		} while (IsValid(x, y) == false && IsRoad(x, y) == false);
		food[i] = { x,y };
	}
}

// hàm cập nhật tọa độ rắn lại 1 vị trí khác khi qua cổng
void RessetSnake(int size)
{

	CHAR_LOCK = 'A', MOVING = 'D', FOOD_INDEX = 0;
	if (size == 0)
	{
		return;
	}
	SIZE_SNAKE = size;
	for (int i = 0; i < size; i++) {
		snake[i].x = i + 9;
		snake[i].y = 8;
	}
}

void ResetData()
{
	//Khởi tạo các giá trị toàn cục
	CHAR_LOCK = 'A', MOVING = 'D', SPEED = 1; FOOD_INDEX = 0, WIDTH_CONSOLE = 60, HEIGH_CONSOLE = 23, SIZE_SNAKE = 6;
	//Khởi tạo giá trị mặc định cho snake
	snake[0] = { 9, 8 };
	snake[1] = { 10, 8 };
	snake[2] = { 11, 8 };
	snake[3] = { 12, 8 };
	snake[4] = { 13, 8 };
	snake[5] = { 14, 8 };
	GenerateFood();//Tạo mảng thức ăn food
}

void DrawBoard(int x, int y, int width, int height, int curPosX = 0, int curPosY = 0)
{
	char box = 254;
	GotoXY(x, y); cout << box;
	for (int i = 1; i < width; i++)
		cout << box;
	cout << box;
	GotoXY(x, height + y);
	cout << box;
	for (int i = 1; i < width; i++)
		cout << box;
	cout << box;
	for (int i = y + 1; i < height + y; i++)
	{
		GotoXY(x, i);
		cout << box;
		GotoXY(x + width, i);
		cout << box;
	}
	GotoXY(curPosX, curPosY);
}

void StartGame()
{
	system("cls");
	ResetData(); // Khởi tạo dữ liệu gốc
	DrawBoard(0, 0, WIDTH_CONSOLE, HEIGH_CONSOLE); // Vẽ màn hình game
	STATE = 1;//Bắt đầu cho Thread chạy
}
//Hàm thoát game

void ExitGame(HANDLE t)
{
	system("cls");
	TerminateThread(t, 0);
}

//Hàm dừng game
void PauseGame(HANDLE t)
{
	SuspendThread(t);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Hàm rắn ăn foòd
void DrawFood(const char* str)
{
	GotoXY(food[FOOD_INDEX].x, food[FOOD_INDEX].y);
	printf(str);
}

void Drawsnakehide(const char *snakeh)
{
	for (int i = SIZE_SNAKE - 1; i >= 0; i--)
	{
		GotoXY(snake[i].x, snake[i].y);
		cout << snakeh;
	}
}

void Drawsnake(string snakee)
{
	int t = 0;
	//for (int i = SIZE_SNAKE - 1; i >= 0; i--)
	for (int i = 0; i < SIZE_SNAKE; i++)
	{
		int tem = t % 16;
		t++;
		GotoXY(snake[i].x, snake[i].y);
		cout << snakee[tem];
	}
}

//Hàm cập nhật dữ liệu toàn cục
void Eat()
{
	snake[SIZE_SNAKE] = snake[SIZE_SNAKE - 1];
	if (FOOD_INDEX == MAX_SIZE_FOOD - 1)
	{
		if (SPEED == MAX_SPEED)
		{
			GenerateFood();
		}
	}
	else
	{
		FOOD_INDEX++;
		SIZE_SNAKE++;
		Score++;
	}
}


void ProcessDead()
{
	STATE = 0;
	Score = 0;
	GotoXY(0, HEIGH_CONSOLE + 1);
	cout << (char)7;
	printf("May choi ngu vcl, type y to continue or anykey to exit");
}

// xxử lí đụng cổng chết
void diecong()
{
	int w = 41, h = 10;
	for (int i = 0; i < 3; i++)
	{
		if (snake[SIZE_SNAKE - 1].x == w && snake[SIZE_SNAKE - 1].y == h || snake[SIZE_SNAKE - 1].x == 40 && snake[SIZE_SNAKE - 1].y == 10 || snake[SIZE_SNAKE - 1].x == 40 && snake[SIZE_SNAKE - 1].y == 12)
		{
			ProcessDead();
		}
		h++;
	}
}

// rắn chạm thân
void xulidie()
{
	for (int j = 0; j < SIZE_SNAKE - 1; j++)
	{
		if (snake[SIZE_SNAKE - 1].x == snake[j].x && snake[SIZE_SNAKE - 1].y == snake[j].y)
		{
			ProcessDead();
		}
	}
}
// Tính điểm
void PrintScore(int score)
{
	GotoXY(62, 3);
	cout << "Score: " << score;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// các hàm di chuyển
void MoveRight()
{
	int temp = 0;
	if ((snake[SIZE_SNAKE - 1].x + 1 == WIDTH_CONSOLE)) {
		ProcessDead();
	}
	else {
		if (snake[SIZE_SNAKE - 1].x + 1 == food[FOOD_INDEX].x && snake[SIZE_SNAKE - 1].y == food[FOOD_INDEX].y)
		{
			Eat();
			temp = 1;
		}
		if (temp == 0)
		{
			for (int i = 0; i < SIZE_SNAKE - 1; i++)
			{
				snake[i].x = snake[i + 1].x;
				snake[i].y = snake[i + 1].y;
			}
			snake[SIZE_SNAKE - 1].x++;
		}
		else
		{
			snake[SIZE_SNAKE - 1].x++;
		}
	}

	xulidie();
}

void MoveLeft()
{
	int temp = 0;
	if ((snake[SIZE_SNAKE - 1].x - 1 == 0)) {
		ProcessDead();
	}
	else {
		if (snake[SIZE_SNAKE - 1].x - 1 == food[FOOD_INDEX].x && snake[SIZE_SNAKE - 1].y == food[FOOD_INDEX].y) {
			Eat();
			temp = 1;
		}
		if (temp == 0)
		{
			for (int i = 0; i < SIZE_SNAKE - 1; i++)
			{
				snake[i].x = snake[i + 1].x;
				snake[i].y = snake[i + 1].y;
			}
			snake[SIZE_SNAKE - 1].x--;
		}
		else
			snake[SIZE_SNAKE - 1].x--;

		xulidie();
	}
}

void MoveDown()
{
	int temp = 0;
	if (snake[SIZE_SNAKE - 1].y + 1 == HEIGH_CONSOLE)
	{
		ProcessDead();
	}
	else {
		if (snake[SIZE_SNAKE - 1].x == food[FOOD_INDEX].x && snake[SIZE_SNAKE - 1].y + 1 == food[FOOD_INDEX].y) {
			Eat();
			temp = 1;
		}
		if (temp == 0)
		{
			for (int i = 0; i < SIZE_SNAKE - 1; i++)
			{
				snake[i].x = snake[i + 1].x;
				snake[i].y = snake[i + 1].y;
			}
			snake[SIZE_SNAKE - 1].y++;
		}
		else
			snake[SIZE_SNAKE - 1].y++;

		xulidie();
	}
}

// hàm đi lên
void MoveUp()
{
	int temp = 0;
	if (snake[SIZE_SNAKE - 1].y - 1 == 0)
	{
		ProcessDead();
	}
	else {
		if (snake[SIZE_SNAKE - 1].x == food[FOOD_INDEX].x && snake[SIZE_SNAKE - 1].y - 1 == food[FOOD_INDEX].y)
		{
			Eat();
			temp = 1;
		}
		if (temp == 0)
		{
			for (int i = 0; i < SIZE_SNAKE - 1; i++)
			{
				snake[i].x = snake[i + 1].x;
				snake[i].y = snake[i + 1].y;
			}
			snake[SIZE_SNAKE - 1].y--;
		}
		else
			snake[SIZE_SNAKE - 1].y--;

		xulidie();
	}
}

void Nextt()
{
	xaycong();
	int tem = SIZE_SNAKE;
	if (snake[SIZE_SNAKE - 1].x == 40 && snake[SIZE_SNAKE - 1].y == 11)
	{
		xulyquacong(tem);
		RessetSnake(tem);
		xoacong();
		SPEED++;
		if (SPEED == MAX_SPEED)
		{
			ResetData();
		}
	}
	diecong();
}

void ThreadFunc()
{
	while (1) {
		if (STATE == 1) { //Nếu vẫn còn snake vẫn còn sống
			PrintScore(Score);
			Drawsnakehide(" ");
			switch (MOVING)
			{
			case 'A':
				MoveLeft();
				break;
			case 'D':
				MoveRight();
				break;
			case 'W':
				MoveUp();
				break;
			case 'S':
				MoveDown();
				break;
			}
			if (FOOD_INDEX == MAX_SIZE_FOOD - 1) {
				Nextt();
			}
			else
			{
				DrawFood(" ");
				DrawFood("0");
			}
			Drawsnake(snakee);
			Sleep(150 / SPEED);   //Hàm ngủ theo tốc độ SPEEED
		}
	}
}

void chuyencanh()
{
	DrawBoard(0, 0, WIDTH_CONSOLE, HEIGH_CONSOLE);
}

void Save(string Namefilesave)
{
	system("cls");
	cout << "Nhap ten file: ";
	cin >> Namefilesave;
	//save name file
	string name = Namefilesave + ".txt";
	ofstream fileout(name);

	// save score
	fileout << Score << endl;

	// save snake
	for (int i = 0; i < SIZE_SNAKE; i++)
	{
		fileout << snake[i].x << snake[i].y << endl;
	}

	// save food
	for (int j = 0; j < 10; j++)
	{
		fileout << food[j].x << food[j].y << endl;
	}
	system("cls");
	chuyencanh();
}

void Load()
{
	system("cls");
	string Namefileopen;

	cout << "Nhap ten file load: ";
	cin >> Namefileopen;

	//save name file
	string name = Namefileopen + ".txt";
	ifstream filein(name);

	// save score
	filein >> Score;

	// save snake
	for (int i = 0; i < SIZE_SNAKE; i++)
	{
		filein >> snake[i].x >> snake[i].y;
	}

	// save food
	for (int j = 0; j < 10; j++)
	{
		filein >> food[j].x >> food[j].y;
	}
	system("cls");
	chuyencanh();
}


//Hàm main
void main()
{
	int temp;
	FixConsoleWindow();
	StartGame();
	thread t1(ThreadFunc); //Tao thread cho snake
	HANDLE handle_t1 = t1.native_handle(); //Lay handle cua thread
	while (1)
	{
		temp = toupper(_getch());
		if (STATE == 1)
		{
			if (temp == 'P')
			{
				PauseGame(handle_t1);
			}
			else if (temp == 27)
			{
				ExitGame(handle_t1);
				return;
			}
			else if (temp == 'G')
			{
				PauseGame(handle_t1);
				string name;
				Save(name);
			}
			else if (temp == 'H')
			{
				PauseGame(handle_t1);
				Load();
			}
			else
			{
				ResumeThread(handle_t1);
				if ((temp != CHAR_LOCK) && (temp == 'D' || temp == 'A' || temp == 'W' || temp == 'S'))
				{
					if (temp == 'D') CHAR_LOCK = 'A';
					else if (temp == 'W') CHAR_LOCK = 'S';
					else if (temp == 'S') CHAR_LOCK = 'W';
					else CHAR_LOCK = 'D';
					MOVING = temp;
				}
			}
		}
		else
		{
			if (temp == 'Y')
				StartGame();
			else
			{
				ExitGame(handle_t1);
				return;
			}

		}
	}
}
