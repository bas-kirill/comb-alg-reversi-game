#include <bits/stdc++.h>
using namespace std;

/*

& - black - true
* - white - false

*/

#define vvc vector<vector<char>>
// #define DEBUG 1

// За какой цвет играет человек: true -> Black, false -> White
bool human_color; 
bool AI_color;

// Ограничения для игрового поля
const int MAX_COLUMNS = 8;
const int MAX_ROWS = 8;

// Некоторое заведомое большое число, которое не может нигде встретиться
const int INF = 2e9;

vvc game_table;

// Сдвиги по осям в таблице
int dcol[4] = { 0, +1,  0, -1};
int drow[4] = {-1,  0, +1,  0};

// Количество свободных клеток
int free_cells = MAX_COLUMNS * MAX_ROWS - 4; 

// DEBUG: Счётчик для определения зависания программы
int debug_cnt = 1;

struct node {
	vvc game_table;
	vector<node*> childs;
	int score;
};


node* root = nullptr;


vvc init_game_table() {
	vvc new_game_table(8, vector<char>(8, '-'));
	new_game_table[3][3] = '*';
	new_game_table[3][4] = '&';
	new_game_table[4][3] = '&';
	new_game_table[4][4] = '*';
	return new_game_table;
}


void debug_print_game_table(const vvc game_table) {
	cerr << "   0 1 2 3 4 5 6 7" << endl;
	int step = 0;
	for (int row = 0; row < MAX_ROWS; ++row) {
		cerr << step++ << ':';
		for (int col = 0; col < MAX_COLUMNS; ++ col) {
			cerr  << ' ' << (char)game_table[row][col];
		}
		cerr << endl;
	}
}


void print_game_table(const vvc game_table) {
	cout << "   1 2 3 4 5 6 7 8" << endl;
	char letter = 'a';
	for (int row = 0; row < MAX_ROWS; ++row) {
		cout << (char)letter++ << ':';
		for (int col = 0; col < MAX_COLUMNS; ++ col) {
			cout << ' ' << (char)game_table[row][col];
		}
		cout << endl;
	}
}

char get_symbol(bool is_black) {
	return (is_black ? '&' : '*');
}

char get_opposite_symbol(bool is_black) {
	return (is_black ? '*' : '&');
}


bool is_row(vvc game_table, bool is_black, int cur_row, int cur_col) {
	/*
	В (cur_row, cur_col) лежит нужный нам элемент. Тогда если мы встретили до cur_col такую комбинацию:
		[own_symbol] [enemy_symbol], то гарантированно эта строка возможна для хода, потому что в 
		(cur_row, cur_col) лежит own_symbol.

	Аналогично при col > cur_col, где col \in {cur_col + 1, ..., MAX_COLUMNS - 1}.
	*/

	char own_symbol = get_symbol(is_black);
	char enemy_symbol = get_opposite_symbol(is_black);

	game_table[cur_row][cur_col] = own_symbol;

	// Принимает 2 значения: {1, 2}. Как только достигли значения 2, то 
	// красим все клетки от (cur_row, start_col) до (cur_row, end_col) в 
	// нужные символы.
	int cntOwnSymbol = 0;
	bool isEnemySymbol = false;

	// Двигаемся вперёд по колонкам
	for (int col = cur_col; col < MAX_COLUMNS; ++col) {
		if (game_table[cur_row][col] == '-') 
			break;

		if (cntOwnSymbol == 1 and game_table[cur_row][col] == enemy_symbol)
			isEnemySymbol = true;

		if (game_table[cur_row][col] == own_symbol) {
			++cntOwnSymbol;

			if (cntOwnSymbol == 2) {
				if (isEnemySymbol == true)
					return  true;
				else 
					break;
			}
		}
	}

	cntOwnSymbol = 0;
	isEnemySymbol = false;

	// Двигаемся назад по колонкам
	for (int col = cur_col; col >= 0; --col) {
		if (game_table[cur_row][col] == '-') 
			break;

		if (cntOwnSymbol == 1 and game_table[cur_row][col] == enemy_symbol)
			isEnemySymbol = true;

		if (game_table[cur_row][col] == own_symbol) {
			++cntOwnSymbol;

			if (cntOwnSymbol == 2) {
				if (isEnemySymbol == true)
					return true;
				else 
					break;
			}
		}
	}

	return false;
}


bool is_column(vvc game_table, bool is_black, int cur_row, int cur_col) {
	/*
	В (cur_row, cur_col) лежит нужный нам элемент. Тогда если мы встретили до cur_row такую комбинацию:
		[own_symbol]
		[enemy_symbol], то гранатированно этот столбец возможен для хода, потому что в (cur_row, cur_col) 
															лежит own_symbol

	Аналогично при row > cur_row, где row \in {cur_row + 1, ..., MAX_ROWS - 1}.

	При этом нужно проверить, что между комбинациями нет символа '-', который всё портит.
	*/

	char own_symbol = get_symbol(is_black);
	char enemy_symbol = get_opposite_symbol(is_black);

	game_table[cur_row][cur_col] = own_symbol;

	// Принимает 2 значения: {1, 2}. Как только достигли значения 2, то 
	// красим все клетки от (cur_row, start_col) до (cur_row, end_col) в 
	// нужные символы.
	int cntOwnSymbol = 0;
	bool isEnemySymbol = false;

	// Поднимаемся вверх по строкам
	for (int row = cur_row; row >= 0; --row) {
		if (game_table[row][cur_col] == '-') 
			break;

		if (cntOwnSymbol == 1 and game_table[row][cur_col] == enemy_symbol)
			isEnemySymbol = true;

		if (game_table[row][cur_col] == own_symbol) {
			++cntOwnSymbol;

			if (cntOwnSymbol == 2) {
				if (isEnemySymbol == true)
					return  true;
				else 
					break;
			}
		}
	}

	cntOwnSymbol = 0;
	isEnemySymbol = false;

	// Спускаемся вниз по строкам
	for (int row = cur_row; row < MAX_ROWS; ++row) {
		if (game_table[row][cur_col] == '-') 
			break;

		if (cntOwnSymbol == 1 and game_table[row][cur_col] == enemy_symbol)
			isEnemySymbol = true;

		if (game_table[row][cur_col] == own_symbol) {
			++cntOwnSymbol;

			if (cntOwnSymbol == 2) {
				if (isEnemySymbol == true)
					return true;
				else 
					break;
			}
		}
	}

	return false;
}


bool in_border(int row, int col) {
	return (row >= 0) and (row < MAX_ROWS) and (col >= 0) and (col < MAX_COLUMNS);
}


bool is_diagonals(vvc game_table, bool is_black, int cur_row, int cur_col) {
	char own_symbol = get_symbol(is_black);
	char enemy_symbol = get_opposite_symbol(is_black);

	game_table[cur_row][cur_col] = own_symbol;

	int cntOwnSymbol = 0;
	bool isEnemySymbol = false;

	int start_row = cur_row;
	int start_col = cur_col;

	// Check side diagonal
	// 1 2 4
	// 3 5 
	// 6
	// Сначала поднимаемся вверх по диагонали
	while (in_border(start_row, start_col)) {
		if (game_table[start_row][start_col] == '-') 
			break;

		if (cntOwnSymbol == 1 and game_table[start_row][start_col] == enemy_symbol)
			isEnemySymbol = true;

		if (game_table[start_row][start_col] == own_symbol) {
			++cntOwnSymbol;

			if (cntOwnSymbol == 2) {
				if (isEnemySymbol == true)
					return true;
				else 
					break;
			}
		}

		--start_row, ++start_col;
	}

	// Теперь идём вниз по диагонали
	cntOwnSymbol = 0;
	isEnemySymbol = false;
	start_row = cur_row;
	start_col = cur_col;
	while (in_border(start_row, start_col)) {
		if (game_table[start_row][start_col] == '-') 
			break;

		if (cntOwnSymbol == 1 and game_table[start_row][start_col] == enemy_symbol)
			isEnemySymbol = true;

		if (game_table[start_row][start_col] == own_symbol) {
			++cntOwnSymbol;

			if (cntOwnSymbol == 2) {
				if (isEnemySymbol == true)
					return true;
				else 
					break;
			}
		}

		++start_row, --start_col;
	}

	// Check main diagonal
	// 4 2 1
	//   5 3
	//     6
	cntOwnSymbol = 0;
	isEnemySymbol = false;
	start_row = cur_row;
	start_col = cur_col;

	// Сначала идём вверх
	while (in_border(start_row, start_col)) {
		if (game_table[start_row][start_col] == '-') 
			break;

		if (cntOwnSymbol == 1 and game_table[start_row][start_col] == enemy_symbol)
			isEnemySymbol = true;

		if (game_table[start_row][start_col] == own_symbol) {
			++cntOwnSymbol;

			if (cntOwnSymbol == 2) {
				if (isEnemySymbol == true)
					return true;
				else 
					break;
			}
		}

		--start_row, --start_col;
	}

	// Теперь идём вниз
	cntOwnSymbol = 0;
	isEnemySymbol = false;
	start_row = cur_row;
	start_col = cur_col;

	while (in_border(start_row, start_col)) {
		if (game_table[start_row][start_col] == '-') 
			break;

		if (cntOwnSymbol == 1 and game_table[start_row][start_col] == enemy_symbol)
			isEnemySymbol = true;

		if (game_table[start_row][start_col] == own_symbol) {
			++cntOwnSymbol;

			if (cntOwnSymbol == 2) {
				if (isEnemySymbol == true)
					return true;
				else 
					break;
			}
		}


		++start_row, ++start_col;
	}


	return false;
}


vector<pair<int, int>> get_available_spots(const vvc game_table, bool is_black) {
	char own_symbol = get_symbol(is_black);
	char enemy_symbol = get_opposite_symbol(is_black);

	vector<pair<int, int>> available_spots;  // {<row>, <column>}

	for (int row = 0; row < MAX_ROWS; ++row) {
		for (int col = 0; col < MAX_COLUMNS; ++col) {
			if (game_table[row][col] == enemy_symbol) {
				for (int k = 0; k < 4; ++k) {
					int new_row = row + drow[k];
					int new_col = col + dcol[k];

					if (!in_border(new_row, new_col))
						continue;

					if (game_table[new_row][new_col] == '-') {
						// isOk - подходит ли эта клетка как возможный ход
						bool isOk = is_row(game_table, is_black, new_row, new_col)
									|| is_column(game_table, is_black, new_row, new_col)
									|| is_diagonals(game_table, is_black, new_row, new_col);

						bool isRoww = is_row(game_table, is_black, new_row, new_col);
						bool isColumnn = is_column(game_table, is_black, new_row, new_col);
						bool isDiagonalss = is_diagonals(game_table, is_black, new_row, new_col);

						if (isOk) {
							available_spots.push_back({new_row, new_col});
						}
					}
				}
			}
		}
	}

	return available_spots;
}

void draw_cell(vvc &game_table, int row, int col, char symbol) {
	game_table[row][col] = symbol;
	if (game_table[row][col] == '-')
		--free_cells;
}


void draw_row(vvc &game_table, bool is_black, int cur_row, int cur_col) {
	char own_symbol = get_symbol(is_black);
	char enemy_symbol = get_opposite_symbol(is_black);

	// Принимает 2 значения: {1, 2}. Как только достигли значения 2, то 
	// красим все клетки от (cur_row, start_col) до (cur_row, end_col) в 
	// нужные символы.
	int cntOwnSymbol = 0;

	// С какой по какую колонку нужно покрасить
	int start_col = -1;
	int end_col = -1;

	for (int col = 0; col < MAX_COLUMNS; ++col) {
		if (game_table[cur_row][col] == '-') {
			cntOwnSymbol = 0;
			start_col = -1;
			end_col = -1;
		}

		if (game_table[cur_row][col] == own_symbol) {
			++cntOwnSymbol;

			if (cntOwnSymbol == 1)
				start_col = col;

			if (cntOwnSymbol == 2) {
				end_col = col;

				while (start_col <= end_col) {
					draw_cell(game_table, cur_row, start_col, own_symbol);
					++start_col;
				}

				// Так как вновь найденный символ может являться началом
				// следующей "подстроки", которую нужно покрасить.
				cntOwnSymbol = 1;
				start_col = col;
			}
		}
	}
}


void draw_column(vvc &game_table, bool is_black, int cur_row, int cur_col) {
	char own_symbol = get_symbol(is_black);
	char enemy_symbol = get_opposite_symbol(is_black);

	// Принимает 2 значения: {1, 2}. Как только достигли значения 2, то 
	// красим все клетки от (cur_row, start_col) до (cur_row, end_col) в 
	// нужные символы.
	int cntOwnSymbol = 0;
	// С какой по какую колонку нужно покрасить
	int start_row = -1;
	int end_row = -1;

	for (int row = 0; row < MAX_ROWS; ++row) {
		if (game_table[row][cur_col] == '-') {
			cntOwnSymbol = 0;
			start_row = -1;
			end_row = -1;
		}

		if (game_table[row][cur_col] == own_symbol) {
			++cntOwnSymbol;

			if (cntOwnSymbol == 1)
				start_row = row;

			if (cntOwnSymbol == 2) {
				end_row = row;

				while (start_row <= end_row) {
					draw_cell(game_table, start_row, cur_col, own_symbol);
					++start_row;
				}

				// Так как вновь найденный символ может являться началом
				// следующей "подстроки", которую нужно покрасить.
				cntOwnSymbol = 1;
				start_row = row;
			}
		}
	}
}


void draw_diagonals(vvc &game_table, bool is_black, int cur_row, int cur_col) {
	char own_symbol = get_symbol(is_black);
	char enemy_symbol = get_opposite_symbol(is_black);

	// С этих координат будет обходиться каждая диагональ
	int start_row;
	int start_col;

	// Принимает 2 значения: {1, 2}. Как только достигли значения 2, то 
	// красим все клетки от (occur_row_start , occur_col_start)  
	// до (occur_row_end, occur_col_end) в нужные символы.
	int cntOwnSymbol = 0;

	// Эти координаты хранят информацию о том, с какой по какую
	// координаты нужно покрасить диагональ в нужный символ
	int occur_row_start = -1;
	int occur_col_start = -1;
	int occur_row_end = -1;
	int occur_col_end = -1;

	// Check side diagonal
	// 1 2 4
	// 3 5 
	// 6
	int sum = cur_row + cur_col;
	start_row = sum <= 7 ? 0 : (sum - (MAX_ROWS - 1));
	start_col = sum <= 7 ? sum : (MAX_COLUMNS - 1);

	// DEBUG: Выводим начальные позиции для start_row и start_col
	#ifdef DEBUG
	cout << "Draw diagonals: " << start_row << ' ' << start_col << endl; 
	#endif

	while (in_border(start_row, start_col)) {
		if (game_table[start_row][start_col] == '-') {
			cntOwnSymbol = 0;
			occur_row_start = -1;
			occur_col_start = -1;
			occur_row_end = -1;
			occur_col_end = -1;
		}

		if (game_table[start_row][start_col] == own_symbol) {
			++cntOwnSymbol;

			if (cntOwnSymbol == 1) {
				occur_row_start = start_row;
				occur_col_start = start_col;
			}

			if (cntOwnSymbol == 2) {
				occur_row_end = start_row;
				occur_col_end = start_col;

				while (occur_row_start <= occur_row_end and occur_col_start >= occur_col_end) {
					draw_cell(game_table, occur_row_start, occur_col_start, own_symbol);
					// game_table[occur_row_start][occur_col_start] = own_symbol;

					++occur_row_start, --occur_col_start;
				}

				cntOwnSymbol = 1;
				occur_row_start = start_row;
				occur_col_start = start_col;
			}
		}

		++start_row, --start_col;
	}


	// Check main diagonal
	// 4 2 1
	//   5 3
	//     6
	cntOwnSymbol = 0;
	occur_row_start = -1;
	occur_col_start = -1;
	occur_row_end = -1;
	occur_col_end = -1;

	int diff = cur_row - cur_col;
	start_row = diff >= 0 ? diff : 0;
	start_col = diff >= 0 ? 0 : abs(diff);

	#ifdef DEBUG
	// DEBUG: Выводим начальные позиции для start_row и start_col
	cout << "Draw diagonals: " << start_row << ' ' << start_col << endl; 
	#endif

	while (in_border(start_row, start_col)) {
		if (game_table[start_row][start_col] == '-') {
			cntOwnSymbol = 0;
			occur_row_start = -1;
			occur_col_start = -1;
			occur_row_end = -1;
			occur_col_end = -1;
		}

		if (game_table[start_row][start_col] == own_symbol) {
			++cntOwnSymbol;

			if (cntOwnSymbol == 1) {
				occur_row_start = start_row;
				occur_col_start = start_col;
			}

			if (cntOwnSymbol == 2) {
				occur_row_end = start_row;
				occur_col_end = start_col;

				while (occur_row_start <= occur_row_end and occur_col_start <= occur_col_end) {
					draw_cell(game_table, occur_row_start, occur_col_start, own_symbol);

					++occur_row_start, ++occur_col_start;
				}

				cntOwnSymbol = 1;
				occur_row_start = start_row;
				occur_col_start = start_col;
			}
		}

		++start_row, ++start_col;
	}
}


void do_step(vvc &game_table, bool is_black, int row, int col) {
	char symbol = get_symbol(is_black);
	game_table[row][col] = symbol;

	// Делаем покраску согласно правилам игры
	draw_row(game_table, is_black, row, col);
	draw_column(game_table, is_black, row, col);
	draw_diagonals(game_table, is_black, row, col);
}


string make_uci(int row, int col) {
	string uci = "";
	char letter = 'a' + row;
	uci.push_back(letter);
	uci += to_string(col + 1);
	return uci;
}


pair<int, int> get_uci(string uci) {
	int row = uci[0] - 'a';
	int col = uci[1] - '0' - 1;

	return {row, col};
}

bool is_play_game(const vvc &game_table) {
	if (free_cells <= 0) 
		return false;

	vector<pair<int, int>> available_spots;
	available_spots = get_available_spots(game_table, human_color);

	if (available_spots.size() == 0) 
		return false;

	return true;
}


int game_version_random(vector<pair<int, int>> &available_spots) {
	srand(time(0));
	return rand() % available_spots.size();
}


void make_human_step() {
	vector<pair<int, int>> available_spots;
	// Получаем возможные позиции для нашего хода({<row>, <col>})
	available_spots = get_available_spots(game_table, human_color);

	// Выбираем случайные координаты для хода
	int own_step_idx = game_version_random(available_spots);

	// Наши координаты для хода({<row>, <col>})
	pair<int, int> own_step;
	own_step.first = available_spots[own_step_idx].first;
	own_step.second = available_spots[own_step_idx].second;

	// Делаем ход
	do_step(game_table, human_color, own_step.first, own_step.second);
	// Преобразовывам {<row>, <col>} в формат UCI
	string own_step_uci = make_uci(own_step.first, own_step.second);

	#ifdef DEBUG
	// DEBUG: Печатаем наш ход
	cerr << "Own step: " << own_step.first << ' ' << own_step.second << endl;
	// DEBUG: Возможными ходами становятся не те, поэтому будем отслеживать это
	int new_row = own_step.first;
	int new_col = own_step.second;
	bool isRoww = is_row(game_table, true, new_row, new_col);
	bool isColumnn = is_column(game_table, true, new_row, new_col);
	bool isDiagonalss = is_diagonals(game_table, true, new_row, new_col);
	cout << "New row: " << new_row << "; New col: " << new_col << "; " << isRoww << " " << isColumnn << " " << isDiagonalss << endl;
	#endif

	// Выводим наш шаг в общепринятом стандарте на экран
	cout << own_step_uci << endl;	// TODO: Перед отправкой исправить cerr -> cout

/*	#ifdef DEBUG
	// DEBUG: Печатаем таблицу после нашего хода
	print_game_table(game_table);
	#endif*/
}


void make_AI_step() {
	// Ход соперника
	string AI_step_uci;
	cin >> AI_step_uci;

	pair<int, int> AI_step;
	AI_step = get_uci(AI_step_uci);

	do_step(game_table, AI_color, AI_step.first, AI_step.second);	
}

void init_node(node* nd) {
	nd -> game_table = init_game_table();
	nd -> score = 0;
}

void copy_game_table(vvc &new_game_table, const vvc old_game_table) {
	for (int row = 0; row < MAX_ROWS; ++row) {
		for (int col = 0; col < MAX_COLUMNS; ++col) {
			new_game_table[row][col] = old_game_table[row][col];
		}
	}
}

void init_node(node* nd, vvc game_table) {
	nd -> game_table = init_game_table();
	copy_game_table(nd -> game_table, game_table);
	nd -> score = 0;
}

void make_tree(node* nd, bool is_black) {
	char own_symbol = get_symbol(is_black);
	char enemy_symbol = get_opposite_symbol(is_black);

	vector<pair<int, int>> available_spots;
	available_spots = get_available_spots(nd -> game_table, is_black);

	for (int spot_idx = 0; spot_idx < available_spots.size(); ++spot_idx) {
		int row = available_spots[spot_idx].first;
		int col = available_spots[spot_idx].second;

		vvc new_game_table = init_game_table();
		copy_game_table(new_game_table, nd->game_table);
		new_game_table[row][col] = enemy_symbol;

		node* new_node;
		init_node(new_node, new_game_table);

		nd->childs.push_back(new_node);
		make_tree(new_node, !is_black);
	}
}

int main() {
	// Считываем очередность хода
	int who_is;
	cin >> who_is;

	// First step of human relate to Black -> true, else -> false
	human_color = (who_is == 1 ? true : false);
	AI_color = !human_color;

	// Initialize the variables
	srand(time(0));
	game_table = init_game_table();
	vector<pair<int, int>> available_spots;

	// Делаем это, чтобы вывод cerr и cout не менялись местами
	std::mutex mtx;
	std::lock_guard<std::mutex> lock(mtx);
	

	// Start game
	bool play_game = is_play_game(game_table);
	while (play_game) {
		#ifdef DEBUG
		// DEBUG: Печатаем таблицу перед нашим ходом и ходом соперника
		debug_print_game_table(game_table);
		#endif

		if (who_is == 1) {
			make_human_step();
			#ifdef DEBUG
			// DEBUG: Печатаем таблицу после нашего хода и хода соперника
			print_game_table(game_table);
			#endif
			make_AI_step();
		} else {
			make_AI_step();
			#ifdef DEBUG
			// DEBUG: Печатаем таблицу после нашего хода и хода соперника
			print_game_table(game_table);
			#endif
			make_human_step();
		}


		play_game = is_play_game(game_table);
		if (!play_game)
			play_game = true;
	}

	#ifndef DEBUG
	// DEBUG
	cerr << available_spots.size() << '\n';
	for (int i = 0; i < available_spots.size(); ++i) {
		cerr << available_spots[i].first << ' ' << available_spots[i].second << endl;
	}
	#endif
}