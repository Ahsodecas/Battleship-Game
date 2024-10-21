#pragma once
#include "app.h"
using namespace std;




std::wstring const app::main_class_name{ L"BATTLESHIPS - STATISTICS" };
std::wstring const app::board1_class_name{ L"BATTLESHIPS - MY" };
std::wstring const app::board2_class_name{ L"BATTLESHIPS - PC" };
static ULONGLONG start_time;


int index_from_row_column(int row, int column, int size)
{
	return row * size + column;
}



bool app::register_class()
{
	WNDCLASSEXW main_wind{};
	if (GetClassInfoExW(m_instance, (main_class_name).c_str(),
		&main_wind) != 0)
		return true;
	main_wind = {
   .cbSize = sizeof(WNDCLASSEXW),
   .lpfnWndProc = app::window_proc_static,
   .hInstance = m_instance,
   .hIcon = LoadIconW(m_instance, MAKEINTRESOURCE(IDI_ICON1)),
   .hCursor = LoadCursorW(nullptr, L"IDC_ARROW"),
   .hbrBackground = CreateSolidBrush(RGB(164,174,196)),
   .lpszMenuName = MAKEINTRESOURCE(IDR_MENU),
   .lpszClassName = main_class_name.c_str()
	};
	LoadAccelerators(m_instance, MAKEINTRESOURCE(IDR_ACCELERATOR));
	WNDCLASSEXW board1_wind{};
	if (GetClassInfoExW(m_instance, board1_class_name.c_str(),
		&board1_wind) != 0)
		return true;
	board1_wind = {
   .cbSize = sizeof(WNDCLASSEXW),
   .lpfnWndProc = window_board1_proc_static,
   .hInstance = m_instance,
   .hCursor = LoadCursorW(nullptr, L"IDC_ARROW"),
   .hbrBackground = CreateSolidBrush(RGB(164,174,196)),
   .lpszClassName = board1_class_name.c_str()
	};

	WNDCLASSEXW board2_wind{};
	if (GetClassInfoExW(m_instance, board2_class_name.c_str(),
		&board2_wind) != 0)
		return true;
	board2_wind = {
   .cbSize = sizeof(WNDCLASSEXW),
   .lpfnWndProc = window_board2_proc_static,
   .hInstance = m_instance,
   .hCursor = LoadCursorW(nullptr, L"IDC_ARROW"),
   .hbrBackground = CreateSolidBrush(RGB(164,174,196)),
   .lpszClassName = board2_class_name.c_str()
	};
	
	return RegisterClassExW(&main_wind) != 0 && RegisterClassExW(&board1_wind) != 0 && RegisterClassExW(&board2_wind) != 0;
}

LRESULT app::window_proc_static(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	app* ap = nullptr;
	if (message == WM_NCCREATE)
	{
		ap = static_cast<app*>(
			reinterpret_cast<LPCREATESTRUCTW>(lparam)->lpCreateParams);
		SetWindowLongPtrW(window, GWLP_USERDATA,
			reinterpret_cast<LONG_PTR>(ap));
	}
	else
		ap = reinterpret_cast<app*>(
			GetWindowLongPtrW(window, GWLP_USERDATA));
	LRESULT res = ap ?
		ap->window_main_proc(window, message, wparam, lparam) :
		DefWindowProcW(window, message, wparam, lparam);
	if (message == WM_NCDESTROY)
		SetWindowLongPtrW(window, GWLP_USERDATA, 0);
	return res;
}

bool app::ship_is_destroyed(const Ship& ship)
{
	return (ship.size <= ship.parts_destroyed);
}
void app::mark_destroyed_part_in_main(int clickedRow, int clickedCol, bool board1_typy)
{
	auto ships = board1_typy == true ? my_ships : pc_ships;
	auto& map = board1_typy == true ? board1_map : board2_map;
	int part_index = index_from_row_column(clickedRow, clickedCol, GRIT_SIZE);
	int ship_number = board1_typy==true? map.at(part_index): map.at(part_index) - 11;
	int part_number = clickedRow - ships[ship_number].row;
	ships[ship_number].parts_destroyed++;
	main_board[part_number][map.at(part_index)] = SHIP;

	if (ship_is_destroyed(ships[ship_number]) == true)
	{
		mark_destroyed_ship_neutral_area(ships[ship_number], board1_typy);
	}
	InvalidateRect(m_main, NULL, TRUE);
}


void app::mark_destroyed_ship_neutral_area(const Ship& ship, bool board1_typy)
{
	auto board = board1_typy == true ? board1 : board2;

	for (int i = ship.row; i < ship.row + ship.size; i++)
	{
		if (i + 1 < GRIT_SIZE && board[i + 1][ship.column] == EMPTY)
			board[i + 1][ship.column] = NEUTRAL;
		if (i - 1 >= 0 && board[i - 1][ship.column] == EMPTY)
			board[i - 1][ship.column] = NEUTRAL;
		if (ship.column + 1 < GRIT_SIZE && board[i][ship.column + 1] == EMPTY)
			board[i][ship.column + 1] = NEUTRAL;
		if (ship.column - 1 >= 0 && board[i][ship.column - 1] == EMPTY)
			board[i][ship.column - 1] = NEUTRAL;
	}
}

void app::HandleHit(HWND window, int clickedRow, int clickedCol, bool board1_typy)
{
	last_hit_ship = false;
	pc_turn = false;
	auto board = board1_typy == true ? board1 : board2;
	if (board[clickedRow][clickedCol] < EMPTY)
	{
		board[clickedRow][clickedCol] = SHIP;
		mark_destroyed_part_in_main(clickedRow, clickedCol, board1_typy);
		last_hit_ship = board1_typy == false ? true :false;
		pc_turn = board1_typy == true ? true : false;
	}
	else if (board[clickedRow][clickedCol] == EMPTY || board[clickedRow][clickedCol] == NEUTRAL)
	{
		last_hit_ship = false;
		pc_turn = false;
		board[clickedRow][clickedCol] = WATER;
	}
}

void app::select_random_cell_board1()
{
	srand(time(NULL));
	int randomRow = rand() % GRIT_SIZE;
	int randomColumn = rand() % GRIT_SIZE;
	
	while (board1[randomRow][randomColumn] > EMPTY)
	{
		randomRow = rand() % GRIT_SIZE;
		randomColumn = rand() % GRIT_SIZE;
	}
	HandleHit(m_board_my, randomRow, randomColumn, true);
	Sleep(100);
}

LRESULT  app::window_board1_proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message)
	{
	
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(window, &ps);
		COLORREF rectColorb = RGB(0, 0, 255);
		COLORREF rectColorr = RGB(255, 0, 0);
		COLORREF rectColory = RGB(255, 255, 0);
		COLORREF rectColorw = RGB(220, 220, 220);
		HBRUSH hBrush = (HBRUSH)::GetStockObject(DC_BRUSH);
		HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);



		int gridSize = GRIT_SIZE;
		int cellSize = CELL_SIZE;
		int margin = MARGIN;
		int marginBetweenCells = CELL_MARGIN;
		int totalMargin = 2 * margin + (gridSize - 1) * marginBetweenCells;
		int gridSizeWithMargins = gridSize * cellSize + totalMargin;

		HFONT font = CreateFontA(
			20,
			10,
			0,
			0,
			FW_DONTCARE,
			FALSE,
			FALSE,
			FALSE,
			ANSI_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY,
			DEFAULT_PITCH,
			NULL
		);
		HFONT font1 = (HFONT)SelectObject(hdc, font);
		SetBkMode(hdc, TRANSPARENT);
		SetTextColor(hdc, RGB(0, 0, 0));

		for (int i = 0; i < gridSize; ++i) {
			for (int j = 0; j < gridSize; ++j) {
				RECT cellRect = {
					margin + j * (cellSize + marginBetweenCells),
					margin + i * (cellSize + marginBetweenCells),
					margin + j * (cellSize + marginBetweenCells) + cellSize,
					margin + i * (cellSize + marginBetweenCells) + cellSize
				};
				switch (board1[i][j])
				{
				case EMPTY:
					SetDCBrushColor(hdc, rectColorw);
					RoundRect(hdc, cellRect.left, cellRect.top, cellRect.right, cellRect.bottom, 10, 10);
					break;
				case SHIP1:
					SetDCBrushColor(hdc, rectColorw);
					RoundRect(hdc, cellRect.left, cellRect.top, cellRect.right, cellRect.bottom, 10, 10);
					DrawTextW(hdc, TEXT("1"), 1, &cellRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
					break;
				case SHIP2:
					SetDCBrushColor(hdc, rectColorw);
					RoundRect(hdc, cellRect.left, cellRect.top, cellRect.right, cellRect.bottom, 10, 10);
					DrawTextW(hdc, TEXT("2"), 1, &cellRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
					break;
				case SHIP3:
					SetDCBrushColor(hdc, rectColorw);
					RoundRect(hdc, cellRect.left, cellRect.top, cellRect.right, cellRect.bottom, 10, 10);
					DrawTextW(hdc, TEXT("3"), 1, &cellRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
					break;
				case SHIP4:
					SetDCBrushColor(hdc, rectColorw);
					RoundRect(hdc, cellRect.left, cellRect.top, cellRect.right, cellRect.bottom, 10, 10);
					DrawTextW(hdc, TEXT("4"), 1, &cellRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
					break;
				case WATER:
					SetDCBrushColor(hdc, rectColorb);
					RoundRect(hdc, cellRect.left, cellRect.top, cellRect.right, cellRect.bottom, 10, 10); 
					DrawText(hdc, TEXT("."), 1, &cellRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
					break;
				case SHIP:
					SetDCBrushColor(hdc, rectColorr);
					RoundRect(hdc, cellRect.left, cellRect.top, cellRect.right, cellRect.bottom, 10, 10); 
					DrawText(hdc, TEXT("X"), 1, &cellRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
					break;
				case NEUTRAL:
					SetDCBrushColor(hdc, rectColory);
					RoundRect(hdc, cellRect.left, cellRect.top, cellRect.right, cellRect.bottom, 10, 10); 
					break;
				}
				
			}
		}
		EndPaint(window, &ps);
		SelectObject(hdc, hOldBrush);
		DeleteObject(hBrush);
		SelectObject(hdc, font);
		DeleteObject(font1);
		return 0;
	}
	case WM_ERASEBKGND:
	{
		return 1;
	}
	default:
		return DefWindowProcW(window, message, wparam, lparam);
	}
}

bool app::game_over(bool my_board_type)
{
	for (int i = 0; i < 10; i++)
	{
		if (my_board_type)
		{
			if (!ship_is_destroyed(my_ships[i]))
				return false;
		}
		else
		{
			if (!ship_is_destroyed(pc_ships[i]))
				return false;
		}
	}
	return true;
}



LRESULT  app::window_board2_proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message)
	{
	case WM_LBUTTONDOWN:
	{
		while (pc_turn == true)
		{
			select_random_cell_board1();
			InvalidateRect(m_board_my, nullptr, TRUE);
			Sleep(100);
		}
		int xPos = GET_X_LPARAM(lparam);
		int yPos = GET_Y_LPARAM(lparam);
		if (game_over(true) || game_over(false)) break;
		int clickedRow = (yPos - WINDOW_MARGIN) / (CELL_SIZE + CELL_MARGIN);
		int clickedCol = (xPos - WINDOW_MARGIN) / (CELL_SIZE + CELL_MARGIN);
		HandleHit(window, clickedRow, clickedCol, false);
		InvalidateRect(window, nullptr, TRUE);
		if (last_hit_ship == false)
		{
			select_random_cell_board1();
			InvalidateRect(m_board_my, nullptr, TRUE);
		}
		return 0;
	}
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(window, &ps);
		COLORREF rectColorb = RGB(0, 0, 255);
		COLORREF rectColorr = RGB(255, 0, 0);
		COLORREF rectColory = RGB(255, 255, 0);
		COLORREF rectColorw = RGB(220, 220, 220);
		HBRUSH hBrush = (HBRUSH)::GetStockObject(DC_BRUSH);
		HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);



		// Calculate grid size
		int gridSize = GRIT_SIZE;
		int cellSize = CELL_SIZE;
		int margin = MARGIN;
		int marginBetweenCells = CELL_MARGIN;
		int totalMargin = 2 * margin + (gridSize - 1) * marginBetweenCells;
		int gridSizeWithMargins = gridSize * cellSize + totalMargin;

		HFONT font = CreateFontA(
						20,
						10,
						0,
						0,
						FW_DONTCARE,
						FALSE,
						FALSE,
						FALSE,
						ANSI_CHARSET,
						OUT_DEFAULT_PRECIS,
						CLIP_DEFAULT_PRECIS,
						DEFAULT_QUALITY,
						DEFAULT_PITCH,
						NULL
		);
		HFONT font1 = (HFONT)SelectObject(hdc, font);
		SetBkMode(hdc, TRANSPARENT);
		SetTextColor(hdc, RGB(0, 0, 0));

		for (int i = 0; i < gridSize; ++i) {
			for (int j = 0; j < gridSize; ++j) {
				RECT cellRect = {
								margin + j * (cellSize + marginBetweenCells),
								margin + i * (cellSize + marginBetweenCells),
								margin + j * (cellSize + marginBetweenCells) + cellSize,
								margin + i * (cellSize + marginBetweenCells) + cellSize
				};
				switch (board2[i][j])
				{
					case SHIP1:
						SetDCBrushColor(hdc, rectColorw);
						RoundRect(hdc, cellRect.left, cellRect.top, cellRect.right, cellRect.bottom, 10, 10);
						break;
					case SHIP2:
						SetDCBrushColor(hdc, rectColorw);
						RoundRect(hdc, cellRect.left, cellRect.top, cellRect.right, cellRect.bottom, 10, 10);
						break;
					case SHIP3:
						SetDCBrushColor(hdc, rectColorw);
						RoundRect(hdc, cellRect.left, cellRect.top, cellRect.right, cellRect.bottom, 10, 10);
						break;
					case SHIP4:
						SetDCBrushColor(hdc, rectColorw);
						RoundRect(hdc, cellRect.left, cellRect.top, cellRect.right, cellRect.bottom, 10, 10);
						break;
					case EMPTY:
						SetDCBrushColor(hdc, rectColorw);
						RoundRect(hdc, cellRect.left, cellRect.top, cellRect.right, cellRect.bottom, 10, 10);
						break;
					case WATER:
						SetDCBrushColor(hdc, rectColorb);
						RoundRect(hdc, cellRect.left, cellRect.top, cellRect.right, cellRect.bottom, 10, 10);
						DrawText(hdc, TEXT("."), 1, &cellRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
						break;
					case SHIP:
						SetDCBrushColor(hdc, rectColorr);
						RoundRect(hdc, cellRect.left, cellRect.top, cellRect.right, cellRect.bottom, 10, 10);
						DrawText(hdc, TEXT("X"), 1, &cellRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
						break;
					case NEUTRAL:
						SetDCBrushColor(hdc, rectColory);
						RoundRect(hdc, cellRect.left, cellRect.top, cellRect.right, cellRect.bottom, 10, 10);
						break;
				}

			}
		}

		if(game_over(false))
			DisplayGameOver(window, hdc, true);
		if (game_over(true))
			DisplayGameOver(window, hdc, false);

		EndPaint(window, &ps);
		SelectObject(hdc, hOldBrush);
		DeleteObject(hBrush);
		SelectObject(hdc, font);
		DeleteObject(font1); 
		
		Sleep(10);

		return 0;
	}
	default:
		return DefWindowProcW(window, message, wparam, lparam);
}

}

void app::change_size(int old_size)
{
	int gridSize = GRIT_SIZE;
	int cellSize = CELL_SIZE;
	int margin = MARGIN;
	int marginBetweenCells = CELL_MARGIN;
	int totalMargin = 2 * margin + (gridSize - 1) * marginBetweenCells;
	int gridSizeWithMargins = gridSize * cellSize + totalMargin;

	RECT size{ 0, 0, gridSizeWithMargins, gridSizeWithMargins };
	AdjustWindowRect(&size, WS_CAPTION | WS_POPUP, FALSE);

	deleteBoards(old_size);
	initializeBoard();
	FillBoard(m_board_my, true);
	FillBoard(m_board_pc, false);
	SetWindowPos(m_board_my, NULL, WINDOW_POSITION_MARGIN, WINDOW_POSITION_MARGIN, size.right - size.left, size.bottom - size.top, NULL);
	SetWindowPos(m_board_pc, NULL, m_screen_size.x - WINDOW_POSITION_MARGIN - size.right, WINDOW_POSITION_MARGIN, size.right - size.left, size.bottom - size.top, NULL);
	InvalidateRect(m_board_my, nullptr, TRUE);
	InvalidateRect(m_board_pc, nullptr, TRUE);
}



LRESULT app::window_main_proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message)
	{
	case WM_CREATE:
		retrieveConfiguartions();
		SetTimer(window, 1, 1, NULL);
		start_time = GetTickCount64();
		break;
	case WM_CLOSE:
		DestroyWindow(m_main);
		return 0;
	case WM_DESTROY:
		saveConfiguartions();
		if (window == m_main)
			PostQuitMessage(EXIT_SUCCESS);
		WriteProfileSectionW(L"[BATTLESHIPS]", L"DIFFICULTY=10"
		);
		return 0;
	case WM_CTLCOLORSTATIC:
		return reinterpret_cast<INT_PTR>(m_field_brush);
	case WM_TIMER: {
		ULONGLONG new_time = GetTickCount64();
		print_timer(window, new_time);
		return 0;
	}
	case WM_COMMAND:
	{
		int old_size = GRIT_SIZE;
		switch (LOWORD(wparam))
		{
		case ID_DIFFICULTY_EASY:
			GRIT_SIZE = EASY;
			break;

		case ID_DIFFICULTY_MEDIUM:
			GRIT_SIZE = MEDIUM;
			break;

		case ID_DIFFICULTY_HARD:
			GRIT_SIZE = HARD;
			break;
		}
		change_size(old_size);
		return 0;
	}
	case WM_PAINT:
		{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(window, &ps);
		COLORREF rectColorb = RGB(0, 0, 255);
		COLORREF rectColorr = RGB(255, 0, 0);
		HBRUSH hBrush = (HBRUSH)::GetStockObject(DC_BRUSH);
		HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
		RECT rect;
		GetClientRect(window, &rect);

		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;


		// Calculate grid size
		int gridSizeX = 21;
		int gridSizeY = 4;
		int gridSizeWithMarginsX = width;
		int gridSizeWithMarginsY = height;
		int margin = 3;
		int marginBetweenCells = 1;
		int cellSizeMarginX = gridSizeWithMarginsX/(gridSizeX) - marginBetweenCells;
		int cellSizeMarginY = gridSizeWithMarginsY/ (gridSizeY) - marginBetweenCells;


		HFONT font = CreateFontA(
			20,
			10,
			0,
			0,
			FW_DONTCARE,
			FALSE,
			FALSE,
			FALSE,
			ANSI_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY,
			DEFAULT_PITCH,
			NULL
		);
		HFONT font1 = (HFONT)SelectObject(hdc, font);
		SetBkMode(hdc, TRANSPARENT);
		SetTextColor(hdc, RGB(0, 0, 0));

		for (int i = 0; i < gridSizeY; ++i) {
			for (int j = 0; j < gridSizeX; ++j) {
				RECT cellRect = {
					margin + j * (cellSizeMarginX + marginBetweenCells),
					margin + i * (cellSizeMarginY + marginBetweenCells),
					margin + j * (cellSizeMarginX + marginBetweenCells) + cellSizeMarginX,
					margin + i * (cellSizeMarginY + marginBetweenCells) + cellSizeMarginY
				};
				switch (main_board[i][j])
				{

				case WATER:
					SetDCBrushColor(hdc, rectColorb);
					RoundRect(hdc, cellRect.left, cellRect.top, cellRect.right, cellRect.bottom, 10, 10);
					//DrawText(hdc, TEXT("."), 1, &cellRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
					break;
				case SHIP:
					SetDCBrushColor(hdc, rectColorr);
					RoundRect(hdc, cellRect.left, cellRect.top, cellRect.right, cellRect.bottom, 10, 10);
					//DrawText(hdc, TEXT("X"), 1, &cellRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
					break;

				}

			}
		}
		EndPaint(window, &ps);
		SelectObject(hdc, hOldBrush);
		DeleteObject(hBrush);
		SelectObject(hdc, font);
		DeleteObject(font1);
		return 0;
	}
	}

	return DefWindowProcW(window, message, wparam, lparam);
}

LRESULT app::window_board1_proc_static(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	app* ap = nullptr;
	if (message == WM_NCCREATE)
	{
		ap = static_cast<app*>(
			reinterpret_cast<LPCREATESTRUCTW>(lparam)->lpCreateParams);
		SetWindowLongPtrW(window, GWLP_USERDATA,
			reinterpret_cast<LONG_PTR>(ap));
	}
	else
		ap = reinterpret_cast<app*>(
			GetWindowLongPtrW(window, GWLP_USERDATA));
	LRESULT res = ap ?
		ap->window_board1_proc(window, message, wparam, lparam) :
		DefWindowProcW(window, message, wparam, lparam);
	if (message == WM_NCDESTROY)
		SetWindowLongPtrW(window, GWLP_USERDATA, 0);
	return res;
}

LRESULT app::window_board2_proc_static(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	app* ap = nullptr;
	if (message == WM_NCCREATE)
	{
		ap = static_cast<app*>(
			reinterpret_cast<LPCREATESTRUCTW>(lparam)->lpCreateParams);
		SetWindowLongPtrW(window, GWLP_USERDATA,
			reinterpret_cast<LONG_PTR>(ap));
	}
	else
		ap = reinterpret_cast<app*>(
			GetWindowLongPtrW(window, GWLP_USERDATA));
	LRESULT res = ap ?
		ap->window_board2_proc(window, message, wparam, lparam) :
		DefWindowProcW(window, message, wparam, lparam);
	if (message == WM_NCDESTROY)
		SetWindowLongPtrW(window, GWLP_USERDATA, 0);
	return res;
}

void app::print_timer(HWND window, ULONGLONG new_time)
{
	ULONGLONG milliseconds = new_time - start_time;
	SetWindowTextA(window, (LPCSTR)("BATTLESHIPS: " + to_string(milliseconds / 1000) + "." + to_string(milliseconds % 1000)).c_str());
}


HWND app::create_board_window(DWORD style, HWND parent, DWORD ex_style, int my_pc)
{
	int gridSize = GRIT_SIZE;
	int cellSize = CELL_SIZE;
	int margin = MARGIN;
	int marginBetweenCells = CELL_MARGIN;
	int totalMargin = 2 * margin + (gridSize - 1) * marginBetweenCells;
	int gridSizeWithMargins = gridSize * cellSize + totalMargin;


	RECT size{ 0, 0, gridSizeWithMargins, gridSizeWithMargins };
	AdjustWindowRect(&size, style, FALSE);

	int x_position = (my_pc == 1) ? WINDOW_POSITION_MARGIN : m_screen_size.x - WINDOW_POSITION_MARGIN - size.right;

	auto window_name = (my_pc == 1) ? L"BATTLESHIPS - MY" : L"BATTLESHIPS - PC";

	LPCWSTR class_name = (my_pc == 1) ? board1_class_name.c_str() : board2_class_name.c_str();

	HWND window = CreateWindowExW(
		ex_style,
		class_name,
		window_name,
		style,
		x_position, WINDOW_POSITION_MARGIN,
		size.right - size.left,
		size.bottom - size.top,
		parent,
		nullptr,
		m_instance,
		this);

	return window;
}

HWND app::create_main_window(DWORD style, HWND parent, DWORD ex_style)
{
	RECT main_size{ 0, 0, 600, 250 };
	HWND window = CreateWindowExW(
		ex_style,
		main_class_name.c_str(),
		L"BATTLESHIPS - STATISTICS",
		style,
		m_screen_size.x / 2 - (main_size.right - main_size.left) / 2,
		m_screen_size.y * 3 / 4 - (main_size.bottom - main_size.top) / 2,
		main_size.right - main_size.left,
		main_size.bottom - main_size.top,
		parent,
		nullptr,
		m_instance,
		this);

	return window;
}




int app::run(int show_command)
{
	ShowWindow(m_main, show_command);
	ShowWindow(m_board_my, show_command);
	ShowWindow(m_board_pc, show_command);
	FillBoard(m_board_my, true);
	FillBoard(m_board_pc, false);
	MSG msg{};
	BOOL result = TRUE;
	while ((result = GetMessageW(&msg, nullptr, 0, 0)) != 0)
	{
		if (result == -1)
		{
			return EXIT_FAILURE;
		}
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
	return EXIT_SUCCESS;
}

app::app(HINSTANCE instance) : m_instance{ instance }, m_field_brush{ CreateSolidBrush(RGB(164, 174, 196)) }, m_screen_size{ GetSystemMetrics(SM_CXSCREEN),
	GetSystemMetrics(SM_CYSCREEN) }, m_main{  }, m_board_my{  },
		m_board_pc{  }, my_ships{}, board1_map{}, pc_ships{}, board2_map{}
{
	register_class();
	initializeBoard();
	last_hit_ship = false;
	pc_turn = false;
	DWORD main_style = WS_OVERLAPPED | WS_SYSMENU |
		WS_CAPTION | WS_MINIMIZEBOX;
	m_main = create_main_window(main_style, nullptr, WS_EX_LAYERED);
	m_board_my = create_board_window(WS_CAPTION | WS_POPUP, nullptr, 0, 1);
	m_board_pc = create_board_window(WS_CAPTION | WS_POPUP, nullptr, 0, 0);
	update_transparency();
	//game_over_transparent();
}

void app::update_transparency()
{
	RECT main_rc;
	GetWindowRect(m_main, &main_rc);
	BYTE alpha =
		255 * 0.7;
	SetLayeredWindowAttributes(m_main, 0, alpha, LWA_ALPHA);
}

bool app::canPlaceShip(int column, int row, int size, bool, bool board1_my) //assume only to the bottom
{
	auto board = board1_my == true ? board1 : board2;
	if (column + size > GRIT_SIZE) return 0;
	if (row + size > GRIT_SIZE) return 0;

		if ((column + 1 < GRIT_SIZE) && (row - 1 >= 0) && board[row - 1][column + 1] != EMPTY) {
			return false;
		}
		if ((column - 1 >= 0) && (row - 1 >= 0) && board[row - 1][column - 1] != EMPTY) {
			return false;
		}
		if ((column + 1 < GRIT_SIZE) && (row + size < GRIT_SIZE) && board[row + size][column + 1] != EMPTY) {
			return false;
		}
		if ((column - 1  >= 0) && (row + size < GRIT_SIZE) && board[row + size][column - 1] != EMPTY) {
			return false;
		}
		if ((row - 1 >= 0) && board[row - 1][column] != EMPTY) {
			return false;
		}
		if ((row + size < GRIT_SIZE) && board[row + size][column] != EMPTY) {
			return false;
		}
		for (int i = 0; i < size; ++i) {
			if (board[row + i][column] != EMPTY) {
				return false;
			}
			if ((column + 1 < GRIT_SIZE) && board[row + i][column + 1] != EMPTY) {
				return false;
			}
			if ((column - 1 >= 0) && board[row + i][column - 1] != EMPTY) {
				return false;
			}
		}
	return true;
}

void app::FillBoard(HWND window, bool board1_my)
{
	int size = GRIT_SIZE;
	auto board = board1_my == true ? board1 : board2;
	auto ships = board1_my == true ? my_ships : pc_ships;
	auto& map = board1_my == true ? board1_map : board2_map;
	int ships_number = 10;
	int ships_sizes[] = { 4, 3, 3, 2, 2, 2, 1, 1, 1, 1 };
	srand(time(NULL));
	for (int i = 0; i < ships_number; i++)
	{
		ships[i].size = ships_sizes[i];
		ships[i].my_board = true;
		ships[i].number = i;
		ships[i].parts_destroyed = 0;
		//ships[i].destroyed = false;
		int randomRow = rand() % size;
		int randomColumn = rand() % size;
		while (canPlaceShip(randomColumn, randomRow, ships_sizes[i], false, board1_my) == false)
		{
			randomRow = rand() % size;
			randomColumn = rand() % size;
		}
		ships[i].position_index = index_from_row_column(randomRow, randomColumn, GRIT_SIZE);
		ships[i].row = randomRow;
		ships[i].column = randomColumn;
		for (int j = randomRow; j < randomRow + ships_sizes[i]; j++)
		{
			board[j][randomColumn] = ships_sizes[i] * (-1);
			map[(index_from_row_column(j, randomColumn, GRIT_SIZE))] = (board1_my == true)? i :(11 + i);
		}
	}
	InvalidateRect(window, nullptr, TRUE);
}

void app::initializeBoard()
{
	my_ships = new Ship[10];
	pc_ships = new Ship[10];
	board1 = new int*[GRIT_SIZE];
	board2 = new int* [GRIT_SIZE];
	for (int i = 0; i < GRIT_SIZE; i++)
	{
		board1[i] = new int[GRIT_SIZE];
		board2[i] = new int[GRIT_SIZE];
	}
	for (int i = 0; i < GRIT_SIZE; i++)
	{
		for (int j = 0; j < GRIT_SIZE; j++)
		{
			board1[i][j] = EMPTY;
			board2[i][j] = EMPTY;
		}
	}
}

void app::deleteBoards(int old_size)
{
	for (int i = 0; i < old_size; i++)
	{
		delete board1[i];
		delete board2[i];
	}
	delete board1;
	delete board2;
	delete my_ships;
	delete pc_ships;
}

void app::saveConfiguartions()
{
	wstring message = to_wstring(GRIT_SIZE);
	WritePrivateProfileStringW(
		L"BATTLESHIPS",
		L"DIFFICULTY",
		message.c_str(),
		FileName
	);
}
void app::retrieveConfiguartions()
{
	WCHAR  difficulty[20];
	memset(difficulty, 0, 20);
	auto result = GetPrivateProfileStringW(
		L"BATTLESHIPS",
		L"DIFFICULTY",
		NULL,
		difficulty,
		20,
		FileName
	);
	if (result > 0)
	{
		GRIT_SIZE = _wtoi(difficulty);
		if (GRIT_SIZE != EASY)
			change_size(EASY);
	}
}



void app::DisplayGameOver(HWND hWnd, HDC hdc, bool green)
{
	RECT rcClient;
	GetClientRect(hWnd, &rcClient);

	HDC hdcMem = CreateCompatibleDC(hdc);
	HBITMAP hBitmap = CreateCompatibleBitmap(hdc, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);
	HBITMAP hBitmapOld = (HBITMAP)SelectObject(hdcMem, hBitmap);

	HFONT font = CreateFontA(40, 30, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, NULL);
	HFONT font1 = (HFONT)SelectObject(hdc, font);
	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, RGB(0, 0, 0));

	COLORREF color = green == true ? RGB(0, 255, 0) : RGB(255, 0, 0);
	HBRUSH hBrushGreen = CreateSolidBrush(color);
	RECT rcBitmap = { 0, 0, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top };
	FillRect(hdcMem, &rcBitmap, hBrushGreen);
	DeleteObject(hBrushGreen);
	
	BLENDFUNCTION blendFunction;
	blendFunction.BlendOp = AC_SRC_OVER;
	blendFunction.BlendFlags = 0;
	blendFunction.SourceConstantAlpha = 190;
	blendFunction.AlphaFormat = AC_SRC_ALPHA;

	AlphaBlend(hdc, rcClient.left, rcClient.top, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
		hdcMem, 0, 0, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, blendFunction);
	if (green)
		DrawText(hdc, TEXT("YOU WON"), 7, &rcClient, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);

	SelectObject(hdcMem, hBitmapOld);
	DeleteObject(hBitmap);
	SelectObject(hdc, font);
	DeleteObject(font1);
	DeleteDC(hdcMem);

}
