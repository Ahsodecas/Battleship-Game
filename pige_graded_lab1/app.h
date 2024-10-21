#pragma once
#include "windows.h"
#include <string>
#include "time.h"
#include "WinBase.h"
#include <stdexcept>
#include "resource.h"
#include "WindowsX.h"
#include "wingdi.h"
using namespace std;


enum CELL_STATE
{
	EMPTY = 0,
	WATER = 1,
	SHIP = 2,
	NEUTRAL = 3,
	SHIP1 = -1,
	SHIP3 = -3,
	SHIP2 = -2,
	SHIP4 = -4
};

enum DIFFICULTY_LEVEL
{
	EASY = 10,
	MEDIUM = 15,
	HARD = 20
};

static int GRIT_SIZE = 10;

#define CELL_MARGIN 3
#define CELL_SIZE 30
#define WINDOW_MARGIN 1
#define MARGIN 5
#define WINDOW_POSITION_MARGIN 50
#define FileName L"./Wordle.ini"
#include <unordered_map>

static int main_board[4][21] = {    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ,0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
									{1, 1, 1, 1, 1, 1, 0, 0, 0, 0 ,0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0},
									{1, 1, 1, 0, 0, 0, 0, 0, 0, 0 ,0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0},
									{1, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};


struct Ship
{
	int size;
	int position_index;
	int number;
	int parts_destroyed;
	bool my_board;
	bool destroyed;
	int row;
	int column;
};

class app
{
private:
	static std::wstring const board1_class_name;
	static std::wstring const board2_class_name;
	static std::wstring const main_class_name;
	int** board1;
	int** board2;
	static LRESULT CALLBACK window_proc_static(
		HWND window,
		UINT message,
		WPARAM wparam,
		LPARAM lparam);
	LRESULT window_main_proc(
		HWND window,
		UINT message,
		WPARAM wparam,
		LPARAM lparam);
	LRESULT window_board1_proc(
		HWND window,
		UINT message,
		WPARAM wparam,
		LPARAM lparam);
	LRESULT window_board2_proc(
		HWND window,
		UINT message,
		WPARAM wparam,
		LPARAM lparam);
	bool register_class();
	bool last_hit_ship;
	bool pc_turn;
	HWND m_main;
	HWND m_board_my, m_board_pc;
	void update_transparency();
	HINSTANCE m_instance;
	POINT m_screen_size;
	HBRUSH m_field_brush;
	Ship* my_ships;
	Ship* pc_ships;
	unordered_map<int, int> board1_map;
	unordered_map<int, int> board2_map;
	
	void print_timer(HWND window, ULONGLONG new_time);
	static LRESULT CALLBACK window_board1_proc_static(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
	static LRESULT CALLBACK window_board2_proc_static(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
	HWND create_main_window(DWORD style, HWND parent, DWORD ex_style);
	HWND create_board_window(DWORD style, HWND parent = nullptr, DWORD ex_style = 0, int my_pc = 1);
	void select_random_cell_board1();
	void HandleHit(HWND window, int clickedRow, int clickedCol, bool board1_typy);
	void FillBoard(HWND window, bool board1_my);
	bool canPlaceShip(int column, int row, int size, bool horizontal,bool board1_my);
	void initializeBoard();
	void deleteBoards(int old_size);
	void change_size(int old_size);
	void saveConfiguartions();
	void retrieveConfiguartions();
	void mark_destroyed_part_in_main(int clickedRow, int clickedCol, bool board1_typy);
	void mark_destroyed_ship_neutral_area(const Ship& ship, bool board1_typy);
	bool ship_is_destroyed(const Ship& ship);
	bool game_over(bool my_board_type);
	void DisplayGameOver(HWND hWnd, HDC hdc, bool green);

; public:
	app(HINSTANCE instance);
	int run(int show_command);
};




