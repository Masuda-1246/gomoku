#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <winsock2.h>

// 現在の最強の評価値
const int bestEvalValues[] = {10, 10, 10, 10, 10, 10};

// 評価値配列の中、評価特徴の場所
#define CLOSED_FOUR_VALUE 0  // 1番目の値
#define OPEN_THREE_VALUE 1   // 2番目の値
#define CLOSED_THREE_VALUE 2 // 3番目の値
#define OPEN_TWO_VALUE 3     // 4番目の値
#define CLOSED_TWO_VALUE 4   // 5番目の値
#define INITIATIVE 5         // 6番目の値

// 探索の最大深さ
#define MAXDEPTH 3

// マスの情報
#define EMPTY_SQUARE 0 // 空きマス
#define BLACK_STONE 1  // 黒い石
#define WHITE_STONE 2  // 白い石

// 手番
#define BLACK 1 // 黒の手番
#define WHITE 2 // 白の手番

// 五目並べの盤の大きさは15
#define BOARDSIZE 15

// COMと人間はプレイヤー
#define HUMAN 1   // 人間
#define BEST_AI 2 // 現在の一番強い五目AI

char namesAI[][18] = {"", "", "BestAI", "TestAI"};

// ゲームの結果
#define UNFINISHED 0 // まだ終わっていない
#define BLACK_WIN 1  // 先手の勝ち
#define WHITE_WIN 2  // 後手の勝ち
#define DRAW 3       // 引き分け

// 特別な評価値
#define INFINITYVAL 32000 // 無限大
#define WINNING 30000     // 勝ち

// 最大の乱数でつける値
#define MAXRANDBONUS 5

// 先手と後手はCOMか人間かを管理する変数
int blackPlayer;
int whitePlayer;

int gomokuBoard[BOARDSIZE + 1][BOARDSIZE + 1]; // 五目並べの盤
int nextMoveX;                                 // 次の手のx軸
int nextMoveY;                                 // 次の手のy軸

int numberOfGames; // 自己対戦の場合、対局の数

int rootToMove; // 探索の初期局面の手番

int moveCount; // 対局の現在局面まで指した手の数

// 対局の手を保存するための配列
int gameRecord[(BOARDSIZE * BOARDSIZE) + 1][2];

//
// 盤のマスの評価
//
// 中央に近い石を高く評価するための配列．
// 数字の決め方：マスから可能な五目並べの数
// 過度は最低3、端から離れたマスは最大8ので評価のボーナス値は0から5まで．
int potentialEvaluation[BOARDSIZE + 1][BOARDSIZE + 1] =
    {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0},
        {0, 2, 2, 2, 2, 5, 5, 5, 5, 5, 5, 5, 2, 2, 2, 2},
        {0, 2, 2, 2, 2, 5, 5, 5, 5, 5, 5, 5, 2, 2, 2, 2},
        {0, 2, 2, 2, 2, 5, 5, 5, 5, 5, 5, 5, 2, 2, 2, 2},
        {0, 2, 2, 2, 2, 5, 5, 5, 5, 5, 5, 5, 2, 2, 2, 2},
        {0, 2, 2, 2, 2, 5, 5, 5, 5, 5, 5, 5, 2, 2, 2, 2},
        {0, 2, 2, 2, 2, 5, 5, 5, 5, 5, 5, 5, 2, 2, 2, 2},
        {0, 2, 2, 2, 2, 5, 5, 5, 5, 5, 5, 5, 2, 2, 2, 2},
        {0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0},
};

// 局面に同じ色の石の連結を管理するための偏す
int blackOpenFour;    // 黒い石を四つ並びの数、ブロックなし： * B B B B *
int blackClosedFour;  // 黒い石を四つ並びの数、白い石のブロック (W B B B B *) か盤の端にブロック (E B B B B *)
int blackOpenThree;   // 黒い石を三つ並びの数、ブロックなし： * B B B *
int blackClosedThree; // 黒い石を三つ並びの数、白い石のブロック (W B B B *) か盤の端にブロック (E B B B *)
int blackOpenTwo;     // 黒い石を二つ並びの数、ブロックなし： * B B *
int blackClosedTwo;   // 黒い石を二つ並びの数、白い石のブロック (W B B *) か盤の端にブロック (E B B *)
int whiteOpenFour;    // 白い石を四つ並びの数、黒と同様
int whiteClosedFour;  // 白い石を四つ並びの数、黒と同様
int whiteOpenThree;   // 白い石を三つ並びの数、黒と同様
int whiteClosedThree; // 白い石を三つ並びの数、黒と同様
int whiteOpenTwo;     // 白い石を二つ並びの数、黒と同様
int whiteClosedTwo;   // 白い石を二つ並びの数、黒と同様

// 関数のプロトタイプ
void makeBoardEmpty();
void showBoard();
int playGomokuGame(SOCKET, int);
int fiveInRowCheck(int, int, int);
void getNextHumanMove();
int decideComputerMove(int);
int alphaBetaSearch(int, int, int, int);
int flip(int);
int evaluate(int, int, int);
void blackConnectionAdmin(int, int, int);
void whiteConnectionAdmin(int, int, int);

int main()
{
  char c, userSelection, i;
  int gameResult;
  int firstAI, secondAI;
  int firstAIWins, secondAIWins, drawNumber;
  int tempPlayer;
  char dataFileName[100];
  struct sockaddr_in dest1;
  char input_addr[20];
  char input_port[20];

  memset(&dest1, 0, sizeof(dest1));
  // printf("input address: \n");
  // scanf("%s", input_addr);
  // printf("address: %s \n", input_addr);
  sprintf(input_addr, "127.0.0.1");

  printf("input port: \n");
  scanf("%s", input_port);
  int port = atoi(input_port);
  printf("port: %d \n", port);

  dest1.sin_port = htons(port);
  dest1.sin_family = AF_INET;
  dest1.sin_addr.s_addr = inet_addr(input_addr);

  WSADATA data;
  WSAStartup(MAKEWORD(2, 0), &data);

  SOCKET s1 = socket(AF_INET, SOCK_STREAM, 0);
  if (connect(s1, (struct sockaddr *)&dest1, sizeof(dest1)))
  {
    printf("%s connection refused\n", input_addr);
    return -1;
  }

  printf("%s connected! \n", input_addr);

  char buffer[1024];
  char name[20];
  char response[1024];
  int bytes_received = 0;
  int isFirst = 0;
  char msg[20];
  recv(s1, buffer, 1024, 0);
  printf("%s\n", buffer);
  sprintf(name, "DENDEN");
  send(s1, name, strlen(name), 0);

  memset(&buffer, 0, sizeof(buffer));
  recv(s1, buffer, 1024, 0);

  if (port == 12345)
  {
    isFirst = 1;
    gomokuBoard[8][8] = BLACK_STONE;
    sprintf(msg, "8,8");
    send(s1, msg, strlen(msg), 0);
  }
  else if (port == 12346)
  {
    // bufferをint x, yに分割
    char *token = strtok(buffer, ",");
    int x = atoi(token);
    token = strtok(NULL, ",");
    int y = atoi(token);
    gomokuBoard[x][y] = BLACK_STONE;
    x %= BOARDSIZE;
    x++;
    y %= BOARDSIZE;
    y++;
    sprintf(msg, "%d,%d", x, y);
    send(s1, msg, strlen(msg), 0);
    gomokuBoard[x][y] = WHITE_STONE;
  }
  else
  {
    printf("Illegal input: Please select one of the menu options.\n");
  }

  // 乱数の初期化
  srand(time(NULL));

  switch (isFirst)
  {
  case 0:
    // 後攻の場合
    whitePlayer = BEST_AI;
    // ゲームを最初から最後までやる
    if (whitePlayer)
    {
      blackPlayer = HUMAN;
      int toMove = BLACK;
      gameResult = playGomokuGame(s1, toMove);
      // 結果を伝える
      if (gameResult == BLACK_WIN)
        printf("\nI'm Loser.\n\n");
      else if (gameResult == WHITE_WIN)
        printf("\nI'm Winner.\n\n");
      else if (gameResult == DRAW)
        printf("\nThis game ended in a draw.\n\n");
      else
        printf("\nGame was not finished.\n\n");
    }
    break;

  case 1:
    // 先行の場合
    blackPlayer = BEST_AI;
    if (blackPlayer)
    {
      whitePlayer = HUMAN;
      // ゲームを最初から最後までやる
      int toMove = WHITE;
      gameResult = playGomokuGame(s1, toMove);
      // 結果をユーザの伝える
      if (gameResult == BLACK_WIN)
        printf("\nI'm Winner.\n\n");
      else if (gameResult == WHITE_WIN)
        printf("\nI'm Loser.\n\n");
      else if (gameResult == DRAW)
        printf("\nThis game ended in a draw.\n\n");
      else
        printf("\nGame was not finished.\n\n");
    }
    break;

  default:
    printf("Illegal input: Please select one of the menu options.\n");
    break;
  }
  return 0;
}

void showBoard()
{
  int x, y;

  printf("  1 2 3 4 5 6 7 8 9 101112131415\n");
  for (y = 1; y <= BOARDSIZE; y++)
  {
    if (y < 10)
      printf(" %d", y);
    else
      printf("%d", y);
    for (x = 1; x <= BOARDSIZE; x++)
    {
      if (gomokuBoard[x][y] == BLACK_STONE)
        printf(" o");
      else if (gomokuBoard[x][y] == WHITE_STONE)
        printf(" x");
      else
        printf(" -");
    }
    printf("\n");
  }
}

// 盤の初期化：すべてのマスを空きマスにする
void makeBoardEmpty()
{
  int x, y;

  for (x = 1; x <= BOARDSIZE; x++)
    for (y = 1; y <= BOARDSIZE; y++)
      gomokuBoard[x][y] = EMPTY_SQUARE;
}

// 対局終わるまで自分の手、相手の手を繰り返す；
int playGomokuGame(SOCKET s, int toMove)
{
  int gameResult = UNFINISHED;
  char msg[20];
  char buffer[1024];
  int isComTurn = 0;

  moveCount = 1;

  nextMoveX = 0;
  nextMoveY = 0;
  // ゲームが終わるまでにプレイを続く
  while (gameResult == UNFINISHED)
  {
    if ((toMove == BLACK && blackPlayer != HUMAN) ||
        (toMove == WHITE && whitePlayer != HUMAN))
    {
      // COMの手番
      printf("Thinking about my move...\n");
      isComTurn = 1;
      if (decideComputerMove(toMove))
      {
        printf("Computer move: %d) (%d,%d)\n", moveCount, nextMoveX, nextMoveY);
      }
      else //	COMの手は決められない
      {
        printf("Computer move could not be decided.\n");
        if (blackPlayer != HUMAN)
          gameResult = WHITE_WIN;
        else
          gameResult = BLACK_WIN;
        break;
      }
    }
    else
    { // 相手の手番
      memset(&buffer, 0, sizeof(buffer));
      recv(s, buffer, 1024, 0);
      printf("from server ---> %s\n", buffer);
      // bufferをカンマで分割してx, yにintとして格納
      char *token = strtok(buffer, ",");
      int x = atoi(token);
      token = strtok(NULL, ",");
      int y = atoi(token);
      nextMoveX = x;
      nextMoveY = y;
    }

    // 石を盤に置く
    if (toMove == BLACK)
      gomokuBoard[nextMoveX][nextMoveY] = BLACK_STONE;
    else
      gomokuBoard[nextMoveX][nextMoveY] = WHITE_STONE;

    // 対局の手を保存
    gameRecord[moveCount][0] = nextMoveX;
    gameRecord[moveCount][1] = nextMoveY;

    // 手がさされたので手数を更新
    moveCount++;

    // 盤を表示
    showBoard();

    // 終了条件が満たしているかどうか
    if (fiveInRowCheck(nextMoveX, nextMoveY, toMove))
    {
      sprintf(msg, "%d,%d,win", nextMoveX, nextMoveY);
      send(s, msg, strlen(msg), 0);
      if (toMove == BLACK)
        gameResult = BLACK_WIN;
      else
        gameResult = WHITE_WIN;
      break;
    }
    // 盤がいっぱいになったかどうかのチェック
    else if (moveCount == (BOARDSIZE * BOARDSIZE))
    {
      sprintf(msg, "draw");
      send(s, msg, strlen(msg), 0);
      gameResult = DRAW;
      break;
    }
    else
    {
      if (isComTurn)
      {
        memset(&msg, 0, sizeof(msg));
        sprintf(msg, "%d,%d", nextMoveX, nextMoveY);
        send(s, msg, strlen(msg), 0);
        isComTurn = 0;
      }
    }

    // 手番を更新
    if (toMove == BLACK)
      toMove = WHITE;
    else
      toMove = BLACK;

    nextMoveX = 0;
    nextMoveY = 0;
  }

  return gameResult;
}

// 最後の手の情報を利用し、五目並べになったかどうかをチェック
int fiveInRowCheck(int lastX, int lastY, int color)
{
  int x, y;
  int connectNo = 1;

  // 横の五目並べチェック:(lastx, lasty)座標からcolorと同じ色の石の連結を右と左にあわせて数える
  if (lastX > 1)
  {
    for (x = lastX - 1; gomokuBoard[x][lastY] == color; x--)
    {
      connectNo++;
      if (x == 1)
        break;
    }
  }
  if (lastX < BOARDSIZE)
  {
    for (x = lastX + 1; gomokuBoard[x][lastY] == color; x++)
    {
      connectNo++;
      if (x == BOARDSIZE)
        break;
    }
  }
  if (connectNo >= 5)
    return 1; // 横の五目並べ見つけた

  // 縦の五目並べチェック:(lastx, lasty)座標からcolorと同じ色の石の連結を上と下にあわせて数える
  connectNo = 1;
  if (lastY > 1)
  {
    for (y = lastY - 1; gomokuBoard[lastX][y] == color; y--)
    {
      connectNo++;
      if (y == 1)
        break;
    }
  }
  if (lastY < BOARDSIZE)
  {
    for (y = lastY + 1; gomokuBoard[lastX][y] == color; y++)
    {
      connectNo++;
      if (y == BOARDSIZE)
        break;
    }
  }
  if (connectNo >= 5)
    return 1; // 縦の五目並べ見つけた

  // 斜め（北西－南東）の五目並べチェック:(lastx, lasty)座標からcolorと同じ色の石の連結を北西と南東にあわせて数える
  connectNo = 1;
  if (lastX > 1 && lastY > 1)
  {
    for (x = lastX - 1, y = lastY - 1; gomokuBoard[x][y] == color; x--, y--)
    {
      connectNo++;
      if (x == 1 || y == 1)
        break;
    }
  }
  if (lastX < BOARDSIZE && lastY < BOARDSIZE)
  {
    for (x = lastX + 1, y = lastY + 1; gomokuBoard[x][y] == color; x++, y++)
    {
      connectNo++;
      if (x == BOARDSIZE || y == BOARDSIZE)
        break;
    }
  }
  if (connectNo >= 5)
    return 1; // 斜め（北西－南東）の五目並べ見つけた

  // 斜め（北東－南西）の五目並べチェック:(lastx, lasty)座標からcolorと同じ色の石の連結を北東と南西にあわせて数える
  connectNo = 1;
  if (lastX < BOARDSIZE && lastY > 1)
  {
    for (x = lastX + 1, y = lastY - 1; gomokuBoard[x][y] == color; x++, y--)
    {
      connectNo++;
      if (x == BOARDSIZE || y == 1)
        break;
    }
  }
  if (lastX > 1 && lastY < BOARDSIZE)
  {
    for (x = lastX - 1, y = lastY + 1; gomokuBoard[x][y] == color; x--, y++)
    {
      connectNo++;
      if (x == 1 || y == BOARDSIZE)
        break;
    }
  }
  if (connectNo >= 5)
    return 1; // 斜め（北東－南西）の五目並べ見つけた

  return 0;
}

// 手を決める。これは五目並べプログラムの探索の部分．
int decideComputerMove(int toMove)
{
  int score;

  rootToMove = toMove;

  score = alphaBetaSearch(0, rootToMove, -INFINITYVAL, INFINITYVAL);
  printf("Best move evaluation: %d\n", score);

  return 1;
}

//
// Minmax探索とalpha-beta枝刈り
//
int alphaBetaSearch(int depth, int toMove, int alpha, int beta)
{
  int score, eval;
  int x, y;

  // 最大深さになったら局面を評価する
  if (depth == MAXDEPTH)
    return evaluate(rootToMove, toMove, depth);

  // MaxプレイヤーとMinプレイヤーの最高評価を初期化
  if (toMove == rootToMove)
    score = -INFINITYVAL;
  else
    score = INFINITYVAL;

  // 空いているマスに正しい色の石を置いて、すべての手を作成
  for (x = 1; x <= BOARDSIZE; x++)
  {
    for (y = 1; y <= BOARDSIZE; y++)
    {
      if (gomokuBoard[x][y] == EMPTY_SQUARE)
      {
        gomokuBoard[x][y] = toMove;
        // この手で五目並べになったかどうかをチェック
        if (fiveInRowCheck(x, y, toMove))
        {
          if (rootToMove == toMove)
          {
            // Maxプレイヤーの五目並べ
            // 手を戻す
            gomokuBoard[x][y] = EMPTY_SQUARE;
            // 勝ちになった手は探索の初期局面にあったので手を保存する
            if (depth == 0)
            {
              nextMoveX = x;
              nextMoveY = y;
            }
            return WINNING - depth; // 浅い探索の勝ちは深い探索の勝ちより良い
          }
          else
          {
            // Minプレイヤーの五目並べ
            // 手を戻す
            gomokuBoard[x][y] = EMPTY_SQUARE;
            return -(WINNING - depth); // 浅い探索の勝ちは深い探索の勝ちより良い
          }
        }
        else
        {
          // alpha-beta探索を再帰的に呼ぶ
          eval = alphaBetaSearch(depth + 1, flip(toMove), alpha, beta);

          // 手を戻す
          gomokuBoard[x][y] = EMPTY_SQUARE;

          if (rootToMove == toMove)
          {
            // この局面はMaxプレイヤーの手番．探索の結果は現在の最大評価より高いならば最大評価を更新
            if (eval > score)
            {
              score = eval;
              // 最善手は探索の初期局面にあったので手を保存する
              if (depth == 0)
              {
                nextMoveX = x;
                nextMoveY = y;
              }
            }

            // Beta枝刈り
            if (score >= beta)
              return score;
            // alphaを更新
            if (score > alpha)
              alpha = score;
          }
          else
          {
            // この局面はMinプレイヤーの手番．探索の結果は現在の最低評価より低いならば最低評価を更新
            if (eval < score)
              score = eval;

            // Alpha枝刈り
            if (score <= alpha)
              return score;
            // Betaを更新
            if (beta < score)
              beta = score;
          }
        }
      }
    }
  }
  return score;
}

// 手番を逆にする
int flip(int toMove)
{
  if (toMove == BLACK)
    return WHITE;
  else if (toMove == WHITE)
    return BLACK;
  return 0;
}

// 現在の局面を評価
// 正の値は黒い石のパターンに対しての評価なので白の評価であれば評価値を正負逆にする必要がある．
// 勝ち判断のために現在の局面の手番（nextToMove）も必要．
int evaluate(int side, int nextToMove, int depth)
{
  int eval = 0;
  int x, y;
  int connectNo;  // 連続の同じ色の石の数
  int emptySqCon; // 連結はどこまで伸ばせる
  int blocked;    // 石の連結はブロックされている
  int countX, countY;

  blackOpenFour = 0;
  blackClosedFour = 0;
  blackOpenThree = 0;
  blackClosedThree = 0;
  blackOpenTwo = 0;
  blackClosedTwo = 0;
  whiteOpenFour = 0;
  whiteClosedFour = 0;
  whiteOpenThree = 0;
  whiteClosedThree = 0;
  whiteOpenTwo = 0;
  whiteClosedTwo = 0;

  for (x = 1; x <= BOARDSIZE; x++)
  {
    for (y = 1; y <= BOARDSIZE; y++)
    {

      if (gomokuBoard[x][y] == BLACK_STONE)
      {
        // このマスから黒い石を数える

        // 北東方向
        connectNo = 1;
        emptySqCon = 0;

        // 石の連結は盤の端か相手の石にブロックされている
        if (x == 1 || y == BOARDSIZE || gomokuBoard[x - 1][y + 1] == WHITE_STONE)
          blocked = 1;
        else
        {
          blocked = 0;
          emptySqCon++;
        }

        // 石の連結を数える
        for (countX = x + 1, countY = y - 1; countX <= BOARDSIZE && countY >= 1; countX++, countY--)
        {
          if (gomokuBoard[countX][countY] == BLACK_STONE)
          {
            connectNo++;
          }
          else if (gomokuBoard[countX][countY] == WHITE_STONE)
          {
            blocked++;
            break;
          }
          else
          {
            break;
          }
        }
        // どこまで伸ばせるか
        if (blocked <= 1 && connectNo > 1)
        {
          for (; countX <= BOARDSIZE && countY >= 1 && gomokuBoard[countX][countY] == EMPTY_SQUARE; countX++, countY--)
            emptySqCon++;
        }
        // 石の連結の種類の数を更新
        blackConnectionAdmin(connectNo, blocked, emptySqCon);

        // 東方向
        connectNo = 1;
        emptySqCon = 0;

        // 石の連結は盤の端か相手の石にブロックされている
        if (x == 1 || gomokuBoard[x - 1][y] == WHITE_STONE)
          blocked = 1;
        else
        {
          blocked = 0;
          emptySqCon++;
        }

        // 石の連結を数える
        countY = y;
        for (countX = x + 1; countX <= BOARDSIZE; countX++)
        {
          if (gomokuBoard[countX][countY] == BLACK_STONE)
          {
            connectNo++;
          }
          else if (gomokuBoard[countX][countY] == WHITE_STONE)
          {
            blocked++;
            break;
          }
          else
          {
            break;
          }
        }
        // どこまで伸ばせるか
        if (blocked <= 1 && connectNo > 1)
        {
          for (; countX <= BOARDSIZE && gomokuBoard[countX][countY] == EMPTY_SQUARE; countX++)
            emptySqCon++;
        }
        // 石の連結の種類の数を更新
        blackConnectionAdmin(connectNo, blocked, emptySqCon);

        // 南東方向
        connectNo = 1;
        emptySqCon = 0;

        // 石の連結は盤の端か相手の石にブロックされている
        if (x == 1 || y == 1 || gomokuBoard[x - 1][y - 1] == WHITE_STONE)
          blocked = 1;
        else
        {
          blocked = 0;
          emptySqCon++;
        }

        // 石の連結を数える
        for (countX = x + 1, countY = y + 1; countX <= BOARDSIZE && countY <= BOARDSIZE; countX++, countY++)
        {
          if (gomokuBoard[countX][countY] == BLACK_STONE)
          {
            connectNo++;
          }
          else if (gomokuBoard[countX][countY] == WHITE_STONE)
          {
            blocked++;
            break;
          }
          else
          {
            break;
          }
        }
        // どこまで伸ばせるか
        if (blocked <= 1 && connectNo > 1)
        {
          for (; countX <= BOARDSIZE && countY <= BOARDSIZE && gomokuBoard[countX][countY] == EMPTY_SQUARE; countX++, countY++)
            emptySqCon++;
        }
        // 石の連結の種類の数を更新
        blackConnectionAdmin(connectNo, blocked, emptySqCon);

        // 南方向
        connectNo = 1;
        emptySqCon = 0;

        // 石の連結は盤の端か相手の石にブロックされている
        if (y == 1 || gomokuBoard[x][y - 1] == WHITE_STONE)
          blocked = 1;
        else
        {
          blocked = 0;
          emptySqCon++;
        }

        // 石の連結を数える
        countX = x;
        for (countY = y + 1; countY <= BOARDSIZE; countY++)
        {
          if (gomokuBoard[countX][countY] == BLACK_STONE)
          {
            connectNo++;
          }
          else if (gomokuBoard[countX][countY] == WHITE_STONE)
          {
            blocked++;
            break;
          }
          else
          {
            break;
          }
        }
        // どこまで伸ばせるか
        if (blocked <= 1 && connectNo > 1)
        {
          for (; countY <= BOARDSIZE && gomokuBoard[countX][countY] == EMPTY_SQUARE; countY++)
            emptySqCon++;
        }
        // 石の連結の種類の数を更新
        blackConnectionAdmin(connectNo, blocked, emptySqCon);
      }
      else if (gomokuBoard[x][y] == WHITE_STONE)
      {
        // このマスから黒い石を数える
        // 北東方向
        connectNo = 1;
        emptySqCon = 0;

        // 石の連結は盤の端か相手の石にブロックされている
        if (x == 1 || y == BOARDSIZE || gomokuBoard[x - 1][y + 1] == BLACK_STONE)
          blocked = 1;
        else
        {
          blocked = 0;
          emptySqCon++;
        }

        // 石の連結を数える
        for (countX = x + 1, countY = y - 1; countX <= BOARDSIZE && countY >= 1; countX++, countY--)
        {
          if (gomokuBoard[countX][countY] == WHITE_STONE)
          {
            connectNo++;
          }
          else if (gomokuBoard[countX][countY] == BLACK_STONE)
          {
            blocked++;
            break;
          }
          else
          {
            break;
          }
        }
        // どこまで伸ばせるか
        if (blocked <= 1 && connectNo > 1)
        {
          for (; countX <= BOARDSIZE && countY >= 1 && gomokuBoard[countX][countY] == EMPTY_SQUARE; countX++, countY--)
            emptySqCon++;
        }
        // 石の連結の種類の数を更新
        whiteConnectionAdmin(connectNo, blocked, emptySqCon);

        // 東方向
        connectNo = 1;
        emptySqCon = 0;

        // 石の連結は盤の端か相手の石にブロックされている
        if (x == 1 || gomokuBoard[x - 1][y] == BLACK_STONE)
          blocked = 1;
        else
        {
          blocked = 0;
          emptySqCon++;
        }

        // 石の連結を数える
        countY = y;
        for (countX = x + 1; countX <= BOARDSIZE; countX++)
        {
          if (gomokuBoard[countX][countY] == WHITE_STONE)
          {
            connectNo++;
          }
          else if (gomokuBoard[countX][countY] == BLACK_STONE)
          {
            blocked++;
            break;
          }
          else
          {
            break;
          }
        }
        // どこまで伸ばせるか
        if (blocked <= 1 && connectNo > 1)
        {
          for (; countX <= BOARDSIZE && gomokuBoard[countX][countY] == EMPTY_SQUARE; countX++)
            emptySqCon++;
        }
        // 石の連結の種類の数を更新
        whiteConnectionAdmin(connectNo, blocked, emptySqCon);

        // 南東方向
        connectNo = 1;
        emptySqCon = 0;

        // 石の連結は盤の端か相手の石にブロックされている
        if (x == 1 || y == 1 || gomokuBoard[x - 1][y - 1] == BLACK_STONE)
          blocked = 1;
        else
        {
          blocked = 0;
          emptySqCon++;
        }

        // 石の連結を数える
        for (countX = x + 1, countY = y + 1; countX <= BOARDSIZE && countY <= BOARDSIZE; countX++, countY++)
        {
          if (gomokuBoard[countX][countY] == WHITE_STONE)
          {
            connectNo++;
          }
          else if (gomokuBoard[countX][countY] == BLACK_STONE)
          {
            blocked++;
            break;
          }
          else
          {
            break;
          }
        }
        // どこまで伸ばせるか
        if (blocked <= 1 && connectNo > 1)
        {
          for (; countX <= BOARDSIZE && countY <= BOARDSIZE && gomokuBoard[countX][countY] == EMPTY_SQUARE; countX++, countY++)
            emptySqCon++;
        }
        // 石の連結の種類の数を更新
        whiteConnectionAdmin(connectNo, blocked, emptySqCon);

        // 南方向（この場合にダブルカウントはないので確認しなくてよい）
        connectNo = 1;
        emptySqCon = 0;

        // 石の連結は盤の端か相手の石にブロックされている
        if (y == 1 || gomokuBoard[x][y - 1] == BLACK_STONE)
          blocked = 1;
        else
        {
          blocked = 0;
          emptySqCon++;
        }

        // 石の連結を数える
        countX = x;
        for (countY = y + 1; countY <= BOARDSIZE; countY++)
        {
          if (gomokuBoard[countX][countY] == WHITE_STONE)
          {
            connectNo++;
          }
          else if (gomokuBoard[countX][countY] == BLACK_STONE)
          {
            blocked++;
            break;
          }
          else
          {
            break;
          }
        }
        // どこまで伸ばせるか
        if (blocked <= 1 && connectNo > 1)
        {
          for (; countY <= BOARDSIZE && gomokuBoard[countX][countY] == EMPTY_SQUARE; countY++)
            emptySqCon++;
        }
        // 石の連結の種類の数を更新
        whiteConnectionAdmin(connectNo, blocked, emptySqCon);
      }

      // 盤の中央に近い石の評価を高くする
      if (side == BLACK && gomokuBoard[x][y] == BLACK_STONE)
        eval += potentialEvaluation[x][y];
      else if (side == WHITE && gomokuBoard[x][y] == WHITE_STONE)
        eval -= potentialEvaluation[x][y];
    }
  }

  // 連結に値を付ける
  if ((blackOpenFour || blackClosedFour) && nextToMove == BLACK)
    eval = WINNING - (depth + 1); // 黒の手番と長さ４の連結あり、浅い勝ちは深い勝ちより良い
  else if ((whiteOpenFour || whiteClosedFour) && nextToMove == WHITE)
    eval = -(WINNING - (depth + 1)); // 白の手番と長さ４の連結あり、浅い勝ちは深い勝ちより良い
  else if (blackOpenFour >= 1)
    eval = WINNING - (depth + 2); // 黒のOpen Four、浅い勝ちは深い勝ちより良い
  else if (whiteOpenFour >= 1)
    eval = -(WINNING - (depth + 2)); // 白のOpen Four、浅い勝ちは深い勝ちより良い
  else if (blackClosedFour >= 2)
    eval = WINNING - (depth + 2); // 黒は連結４を二つ以上がある、浅い勝ちは深い勝ちより良い
  else if (whiteClosedFour >= 2)
    eval = -(WINNING - (depth + 2)); // 白は連結４を二つ以上がある、浅い勝ちは深い勝ちより良い
  else
  {
    // 最強のAIかテストAIの評価値を使うか
    if ((rootToMove == BLACK && blackPlayer == BEST_AI) ||
        (rootToMove == WHITE && whitePlayer == BEST_AI))
    {
      // 勝ち以外の連結の評価（黒）
      eval += blackClosedFour * bestEvalValues[CLOSED_FOUR_VALUE];
      eval += blackOpenThree * bestEvalValues[OPEN_THREE_VALUE];
      eval += blackClosedThree * bestEvalValues[CLOSED_THREE_VALUE];
      eval += blackOpenTwo * bestEvalValues[OPEN_TWO_VALUE];
      eval += blackClosedTwo * bestEvalValues[CLOSED_TWO_VALUE];

      // 勝ち以外の連結の評価（白）
      eval -= whiteClosedFour * bestEvalValues[CLOSED_FOUR_VALUE];
      eval -= whiteOpenThree * bestEvalValues[OPEN_THREE_VALUE];
      eval -= whiteClosedThree * bestEvalValues[CLOSED_THREE_VALUE];
      eval -= whiteOpenTwo * bestEvalValues[OPEN_TWO_VALUE];
      eval -= whiteClosedTwo * bestEvalValues[CLOSED_TWO_VALUE];

      // 手番にボーナス
      if (nextToMove == BLACK)
      {
        eval += bestEvalValues[INITIATIVE];
      }
      else
      {
        eval -= bestEvalValues[INITIATIVE];
      }
    }
    else
    {
      printf("erorr in evaluate");
    }
  }

  // 手を変えるために乱数を追加
  eval += (rand() % MAXRANDBONUS);

  // 白の評価だったら正負逆
  if (side == WHITE)
    return -eval;

  return eval;
}

// 石の連結の種類の数を更新（黒の場合）
void blackConnectionAdmin(int connectNo, int blocked, int openSquare)
{
  if (blocked >= 2)
  {
    // 連結は完全にブロックされている
  }
  else if (connectNo + openSquare < 5)
  {
    // 連結は５まで伸ばせない
  }
  else if (connectNo == 4)
  {
    // 長さ４の連結を発見した
    if (blocked == 0)
      blackOpenFour++;
    else if (blocked == 1)
      blackClosedFour++;
  }
  else if (connectNo == 3)
  {
    // 長さ３の連結を発見した
    if (blocked == 0)
      blackOpenThree++;
    else if (blocked == 1)
      blackClosedThree++;
  }
  else if (connectNo == 2)
  {
    // 長さ２の連結を発見した
    if (blocked == 0)
      blackOpenTwo++;
    else if (blocked == 1)
      blackClosedTwo++;
  }
}

// 石の連結の種類の数を更新（白の場合）
void whiteConnectionAdmin(int connectNo, int blocked, int openSquare)
{
  if (blocked >= 2)
  {
    // 連結は完全にブロックされている
  }
  else if (connectNo + openSquare < 4)
  {
    // 連結は５まで伸ばせない
  }
  else if (connectNo == 4)
  {
    // 長さ４の連結を発見した
    if (blocked == 0)
      whiteOpenFour++;
    else if (blocked == 1)
      whiteClosedFour++;
  }
  else if (connectNo == 3)
  {
    // 長さ３の連結を発見した
    if (blocked == 0)
      whiteOpenThree++;
    else if (blocked == 1)
      whiteClosedThree++;
  }
  else if (connectNo == 2)
  {
    // 長さ２の連結を発見した
    if (blocked == 0)
      whiteOpenTwo++;
    else if (blocked == 1)
      whiteClosedTwo++;
  }
}