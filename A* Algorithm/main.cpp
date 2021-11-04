#include <algorithm>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>

using std::sort;
using std::istringstream;
using std::ifstream;
using std::vector;
using std::cout;
using std::cin;
using std::string;
using std::endl;
using std::abs;

enum class State { kStart, kObstacle, kFinish, kPath, kEmpty, kClosed };

int delta[4][2] = {{-1, 0}, {0, -1}, {1, 0}, {0, 1}};

vector<State> ParseLine(string line) {
    istringstream sline(line);
    int n;
    char c;
    vector<State> row;
    while (sline >> n >> c && c == ',') {
      if (n == 0) {
        row.push_back(State::kEmpty);
      } else {
        row.push_back(State::kObstacle);
      }
    }
    return row;
}

vector<vector<State>> readBoardFile(string filename) {
    ifstream boardFile (filename);
    vector<vector<State>> board;

    if (boardFile) {
        string line;

        while (getline(boardFile, line)) {
            vector<State> row = ParseLine(line);
            board.push_back(row);            
        }
    }

    return board;
}

bool compare(vector<int> a, vector<int> b) {
    int f1 = a[2] + a[3];
    int f2 = b[2] + b[3];

    return f1 > f2;
}

void cellSort(vector<vector<int>> *v) {
  sort(v->begin(), v->end(), compare);
}

void addToOpen(
    int x, int y, int h, int g,
    vector<vector<int>> &open,
    vector<vector<State>> &grid
) {
    open.push_back({ x, y, g, h });
    grid[x][y] = State::kClosed;
}

bool checkValidCell(int x, int y, vector<vector<State>> &grid) {
    if (
        x >= 0 && x < grid.size() &&
        y >= 0 && y < grid[0].size()
    ) {
        return grid[x][y] == State::kEmpty;
    }

    return false;
}

int heuristic(int x1, int y1, int x2, int y2) {
    return abs(x2 - x1) + abs(y2 - y1);
}

void expandNeighbours(
    const vector<int> &current,
    int goal[2],
    vector<vector<int>> &openlist,
    vector<vector<State>> &grid
) {
    int x = current[0];
    int y = current[1];
    int g = current[2];
    
    for (int i = 0; i < 4; i++) {
        int x2 = x + delta[i][0];
        int y2 = y + delta[i][1];

        if (checkValidCell(x2, y2, grid)) {
            int g2 = g + 1;
            int h2 = heuristic(x2, y2, goal[0], goal[1]);
            addToOpen(x2, y2, g2, h2, openlist, grid);
        }
    }
}

vector<vector<State>> search(vector<vector<State>> &grid, int init[2], int goal[2]) {
    vector<vector<int>> open {};
    int x = init[0];
    int y = init[1];
    int g = 0;
    int h = heuristic(x, y, goal[0], goal[1]);

    addToOpen(x, y, h, g, open, grid);

    while (open.size() != 0) {
        cellSort(&open);
        auto current = open.back();
        open.pop_back();

        x = current[0];
        y = current[1];
        grid[x][y] = State::kPath;

        if (x == goal[0] && y == goal[1]) {
            grid[init[0]][init[1]] = State::kStart;
            grid[goal[0]][goal[1]] = State::kFinish;

            return grid;
        }

        expandNeighbours(current, goal, open, grid);
    }

    cout << "No path found!" << "\n";
    return vector<vector<State>> {};
}

string cellString(State cell) {
  switch(cell) {
    case State::kObstacle: return "⛰️   ";
    case State::kPath: return "🚗   ";
    case State::kStart: return "🚦   ";
    case State::kFinish: return "🏁   ";
    default: return "0   "; 
  }
}


void printBoard(const vector<vector<State>> board) {
  for (int i = 0; i < board.size(); i++) {
    for (int j = 0; j < board[i].size(); j++) {
      cout << cellString(board[i][j]);
    }
    cout << "\n";
  }
}


int main() {
    int init[2];
    int goal[2];
    string filename;

    cout << "Enter Initial x and y:";
    cin >> init[0] >> init[1];
    cout << endl;

    cout << "Enter the goal x and y:";
    cin >> goal[0] >> goal[1];
    cout << endl;

    cout << "Enter the board file name: ";
    cin >> filename;
    cout << endl;

    auto grid = readBoardFile(filename);
    search(grid, init, goal);
    printBoard(grid);

    return 1;
}