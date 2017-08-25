#include <bits/stdc++.h>
using namespace std;

class Board
{
 private:
  vector<vector<int>> board;
  int size;
  int player;
  int moves;
  bool is_evaluated;
  bool is_finished;

  void calc_score_line(int sum, int zeros)
  {
    int p1_sum = size;
    int p2_sum = 2 * size;
    if (zeros == 0)
    {
      if (sum == p1_sum)
        score[0] = INT_MAX;
      else if (sum == p2_sum)
        score[1] = INT_MAX;
    }
    else
    {
      int p2_moves = (size * size - moves) / 2;
      int p1_moves = p2_moves;
      if (moves % 2 == 0 && size % 2 == 1)
        p1_moves++;
      else if (moves % 2 == 1 && size % 2 == 0)
        p2_moves++;

      // cout << "p1_moves: " << p1_moves << " ; p2_moves: " << p2_moves <<
      // endl;
      if (p1_moves >= zeros && (sum + zeros == p1_sum))
        score[0] = max(score[0], score[0] + 1);
      if (p2_moves >= zeros && (sum + (2 * zeros) == p2_sum))
        score[1] = max(score[1], score[1] + 1);
    }
  }

 public:
  vector<int> score;
  Board(int n)
      : size(n),
        player(1),
        board(vector<vector<int>>(n, vector<int>(n, 0))),
        score(2, 0),
        moves(0),
        is_evaluated(false),
        is_finished(false)
  {
  }

  Board(const Board& b)
      : size(b.size),
        player(b.player),
        board(b.board),
        score(b.score),
        moves(b.moves),
        is_evaluated(b.is_evaluated),
        is_finished(b.is_finished)
  {
  }
  int cur_player() { return player; }
  string str() const
  {
    string answer = "";
    for (int i = 0; i < (size - 1); i++)
    {
      answer += to_string(board[i][0]);
      for (int j = 1; j < size; j++) answer += " | " + to_string(board[i][j]);
      answer += '\n';
      for (int j = 0; j < size; j++) answer += "---";
      answer += '\n';
    }

    answer += to_string(board[size - 1][0]);
    for (int j = 1; j < size; j++)
      answer += " | " + to_string(board[size - 1][j]);
    answer += '\n';
    answer += "Player: " + to_string(player) + "\n";

    return answer;
  }

  friend ostream& operator<<(ostream& output, const Board& B)
  {
    output << B.str();
    return output;
  }

  void calculate_score()
  {
    if (!is_evaluated)
    {
      vector<int> sum_hor(size, 0);
      vector<int> zero_hor(size, 0);
      vector<int> sum_ver(size, 0);
      vector<int> zero_ver(size, 0);
      vector<int> sum_diag(2, 0);
      vector<int> zero_diag(2, 0);
      score[0] = 0;
      score[1] = 0;
      for (int i = 0; i < size; i++)
      {
        for (int j = 0; j < size; j++)
        {
          sum_hor[i] += board[i][j];
          sum_ver[j] += board[i][j];
          if (i == j) sum_diag[0] += board[i][j];
          if (i == (size - 1 - j)) sum_diag[1] += board[i][j];
          if (board[i][j] == 0)
          {
            zero_hor[i]++;
            zero_ver[j]++;
            if (i == j) zero_diag[0]++;
            if (i == (size - 1 - j)) zero_diag[1]++;
          }
        }
      }

      for (int i = 0; i < size; i++)
      {
        calc_score_line(sum_hor[i], zero_hor[i]);
        calc_score_line(sum_ver[i], zero_ver[i]);
      }
      calc_score_line(sum_diag[0], zero_diag[0]);
      calc_score_line(sum_diag[1], zero_diag[1]);
      is_evaluated = true;
      is_finished = (score[0] == INT_MAX || score[1] == INT_MAX ||
                     (score[0] == 0 && score[1] == 0));
    }
  }

  void move(int i, int j)
  {
    calculate_score();
    if (is_finished) return;
    if (sanity_check(i, j))
    {
      board[i][j] = player;
      player = (player % 2) + 1;
      moves++;
      is_evaluated = false;
      calculate_score();
    }
  }

  int sanity_check(int i, int j)
  {
    if (i >= 0 && j >= 0 && i < (size) && j < size && board[i][j] == 0)
      return 1;
    return 0;
  }

  int winner()
  {
    calculate_score();
    if (score[0] == INT_MAX) return 1;
    if (score[1] == INT_MAX) return 2;
    if (score[0] != 0 || score[1] != 0) return -1;
    return 0;
  }
  vector<pair<int, int>> possible_moves()
  {
    vector<pair<int, int>> answer;
    if (is_end()) return answer;

    for (int i = 0; i < size; i++)
      for (int j = 0; j < size; j++)
        if (board[i][j] == 0) answer.push_back(make_pair(i, j));
  }

  bool is_end()
  {
    calculate_score();
    return is_finished;
  }
};

struct Node
{
  Board board;
  int score;
  vector<pair<pair<int, int>, Node*>> children;
  Node* parent;
  Node(Board& b) : board(b), score(0) {}
  ~Node()
  {
    for (int i = 0; i < children.size(); i++) delete children[i].second;
  }
};

Node* gen_min_max(Board& b, int AI_player, const int max_depth, int cur_depth)
{
  Node* node = new Node(b);
  int cur_player = b.cur_player();
  bool is_max = (cur_player == AI_player);
  if (b.is_end() || max_depth == cur_depth)
  {
    node->score = (b.score[0] == INT_MAX)
                      ? (INT_MAX)
                      : ((b.score[1] == INT_MAX) ? (-INT_MAX)
                                                 : (b.score[0] - b.score[1]));
    if (AI_player != 1) node->score *= -1;
    // cout << b << endl;
    // cout << "score: " << node->score << endl;
    return node;
  }

  vector<pair<int, int>> moves = b.possible_moves();

  Node* n;
  int score = (is_max) ? (INT_MIN) : (INT_MAX);
  for (int i = 0; i < moves.size(); i++)
  {
    Board board(b);
    board.move(moves[i].first, moves[i].second);
    n = gen_min_max(board, AI_player, max_depth, cur_depth + 1);
    n->parent = node;
    (node->children).push_back(make_pair(moves[i], n));
    // cout << n->score << endl;
    if (is_max)
      score = max(score, n->score);
    else
      score = min(score, n->score);
  }
  // cout << b << endl;
  // cout << "is_max: " << is_max << endl;
  node->score = score;
  // cout << "score: " << node->score << endl;
  return node;
}

int main()
{
  int n = 3;
  cout << "Board Size: ";
  cin >> n;
  Board b(n);
  int mode;

  int MAX_DEPTH = 10;
  while (true)
  {
    cout << "1) AI \n2) 2-player\nWhich mode do you want to play? :";
    cin >> mode;
    if (mode == 1 || mode == 2) break;
    cout << "Invalid option. Please try again.\n";
  }

  if (mode == 2)
  {
    while (!b.is_end())
    {
      cout << b << endl;
      // vector<pair<int, int>> moves = b.possible_moves();
      // cout << "Possible moves: " << endl;
      // for (int i = 0; i < moves.size(); i++)
      // cout << "(" << moves[i].first << "," << moves[i].second << ") ";
      cout << endl << "Move: ";
      int x, y;
      cin >> x >> y;
      b.move(x, y);
    }
  }
  else
  {
    int AI_player = 0;
    while (true)
    {
      cout << "Set max depth of minmax: ";
      cin >> MAX_DEPTH;
      if (MAX_DEPTH >= 1) break;
      cout << "Max depth should be >= 1" << endl;
    }
    while (true)
    {
      cout << "Who plays first? 1 => AI , 2 => You :";
      cin >> AI_player;
      if (AI_player == 1 || AI_player == 2) break;
      cout << "Invalid option. Please try again.\n";
    }
    if (AI_player == 2)
    {
      while (b.cur_player() != AI_player)
      {
        cout << b << endl;
        cout << endl << "Move: ";
        int x, y;
        cin >> x >> y;
        b.move(x, y);
      }
    }

    cout << b << endl;
    while (!b.is_end())
    {
      if (b.cur_player() == AI_player)
      {
        cout << "Moving AI" << endl;
        // cout << b << endl;
        Node* max_node = gen_min_max(b, AI_player, MAX_DEPTH, 0);
        int best_move = -1;
        int best_score = INT_MIN;
        vector<pair<pair<int, int>, Node*>> moves = max_node->children;
        for (int i = 0; i < moves.size(); i++)
        {
          cout << moves[i].first.first << "," << moves[i].first.second << "= > "
               << moves[i].second->score << endl;
          if (moves[i].second->score > best_score)
          {
            best_score = moves[i].second->score;
            best_move = i;
          }
        }
        b.move(moves[best_move].first.first, moves[best_move].first.second);
        delete max_node;
      }
      else
      {
        cout << endl << "Move: ";
        int x, y;
        cin >> x >> y;
        b.move(x, y);
        pair<int, int> move = make_pair(x, y);
      }
      cout << b << endl;
    }
  }
  if (b.score[0] == INT_MAX)
    cout << "Player 1 wins" << endl;
  else if (b.score[1] == INT_MAX)
    cout << "Player 2 wins" << endl;
  else
    cout << "Draw" << endl;
  return 0;
}
