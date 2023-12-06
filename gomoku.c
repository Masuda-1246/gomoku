#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// 現在の最強の評価値
const int bestEvalValues[] = { 10, 10, 10, 10, 10, 10 };
// テストしたい評価値
const int testValues[] = { 10, 10, 10, 10, 10, 10 };

// 評価値配列の中、評価特徴の場所
#define CLOSED_FOUR_VALUE	0		// 1番目の値
#define OPEN_THREE_VALUE	1		// 2番目の値
#define CLOSED_THREE_VALUE	2		// 3番目の値
#define OPEN_TWO_VALUE		3		// 4番目の値
#define CLOSED_TWO_VALUE	4		// 5番目の値
#define INITIATIVE			5		// 6番目の値

// 探索の最大深さ
#define MAXDEPTH		3

// マスの情報
#define EMPTY_SQUARE	0		// 空きマス
#define BLACK_STONE		1		// 黒い石
#define WHITE_STONE		2		// 白い石

//　手番
#define BLACK			1		// 黒の手番
#define WHITE			2		// 白の手番

// 五目並べの盤の大きさは15
#define BOARDSIZE		15

// COMと人間はプレイヤー
#define HUMAN				1		// 人間
#define BEST_AI				2		// 現在の一番強い五目AI
#define TEST_AI				3		// テストしたいAI

char namesAI[][18] = { "", "", "BestAI", "TestAI" };

// ゲームの結果
#define UNFINISHED		0		// まだ終わっていない
#define BLACK_WIN		1		// 先手の勝ち
#define WHITE_WIN		2		// 後手の勝ち
#define DRAW			3		// 引き分け

// 特別な評価値
#define INFINITYVAL		32000	// 無限大
#define WINNING			30000	// 勝ち

// 最大の乱数でつける値
#define MAXRANDBONUS	5

// 先手と後手はCOMか人間かを管理する変数
int blackPlayer;
int whitePlayer;

int gomokuBoard[BOARDSIZE + 1][BOARDSIZE + 1];  // 五目並べの盤
int nextMoveX;										// 次の手のx軸
int nextMoveY;										// 次の手のy軸

int numberOfGames;			// 自己対戦の場合、対局の数

int rootToMove;		// 探索の初期局面の手番

int moveCount;		// 対局の現在局面まで指した手の数

// 対局の手を保存するための配列
int gameRecord[(BOARDSIZE * BOARDSIZE) + 1][2];

// データを保存するための変数
int saveData;
FILE *dataFile;

//
// 盤のマスの評価
//
// 中央に近い石を高く評価するための配列．
// 数字の決め方：マスから可能な五目並べの数
// 過度は最低3、端から離れたマスは最大8ので評価のボーナス値は0から5まで．
int potentialEvaluation[BOARDSIZE + 1][BOARDSIZE + 1] =
{
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0 },
	{ 0, 2, 2, 2, 2, 5, 5, 5, 5, 5, 5, 5, 2, 2, 2, 2 },
	{ 0, 2, 2, 2, 2, 5, 5, 5, 5, 5, 5, 5, 2, 2, 2, 2 },
	{ 0, 2, 2, 2, 2, 5, 5, 5, 5, 5, 5, 5, 2, 2, 2, 2 },
	{ 0, 2, 2, 2, 2, 5, 5, 5, 5, 5, 5, 5, 2, 2, 2, 2 },
	{ 0, 2, 2, 2, 2, 5, 5, 5, 5, 5, 5, 5, 2, 2, 2, 2 },
	{ 0, 2, 2, 2, 2, 5, 5, 5, 5, 5, 5, 5, 2, 2, 2, 2 },
	{ 0, 2, 2, 2, 2, 5, 5, 5, 5, 5, 5, 5, 2, 2, 2, 2 },
	{ 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0 },
};

// 局面に同じ色の石の連結を管理するための偏す
int blackOpenFour;		// 黒い石を四つ並びの数、ブロックなし： * B B B B *
int blackClosedFour;	// 黒い石を四つ並びの数、白い石のブロック (W B B B B *) か盤の端にブロック (E B B B B *)
int blackOpenThree;		// 黒い石を三つ並びの数、ブロックなし： * B B B *
int blackClosedThree;	// 黒い石を三つ並びの数、白い石のブロック (W B B B *) か盤の端にブロック (E B B B *)
int blackOpenTwo;		// 黒い石を二つ並びの数、ブロックなし： * B B *
int blackClosedTwo;		// 黒い石を二つ並びの数、白い石のブロック (W B B *) か盤の端にブロック (E B B *)
int whiteOpenFour;		// 白い石を四つ並びの数、黒と同様
int whiteClosedFour;	// 白い石を四つ並びの数、黒と同様
int whiteOpenThree;		// 白い石を三つ並びの数、黒と同様
int whiteClosedThree;	// 白い石を三つ並びの数、黒と同様
int whiteOpenTwo;		// 白い石を二つ並びの数、黒と同様
int whiteClosedTwo;		// 白い石を二つ並びの数、黒と同様

// 関数のプロトタイプ
void makeBoardEmpty();
void showBoard();
void saveBoardToFile(FILE *);
int playGomokuGame();
int fiveInRowCheck(int, int, int);
int takeBackAfterGame(int);
int askForHumanInput(int);
void getNextHumanMove();
int getNumberOfGames();
int decideComputerMove(int);
int firstFreeSquareAI(int);
int randomFreeSquareAI(int);
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

	while (1)
	{
		// 乱数の初期化
		srand(time(NULL));

		// とりあえず、データは保存しない
		saveData = 0;

		// 盤を初期化
		makeBoardEmpty();

		// 初期局面を表示
		showBoard();

		// メーンメニュを表示
		printf("=============================================================\n");
		printf("This is a very weak Gomoku program made by Reijer Grimbergen.\n");
		printf("=============================================================\n");
		printf("Main menu:\n");
		printf("1) Play human against human\n");
		printf("2) Play with black against computer\n");
		printf("3) Play with white against computer\n");
		printf("4) Play computer against computer\n");
		printf("Q) Quit program\n");
		printf("Please enter your selection: ");

		c = getchar();
		userSelection = c;
		while ((c = getchar()) != '\n')
			;

		switch (userSelection)
		{
		case '1':
			// COMは白の場合にプレイヤーを初期化
			blackPlayer = HUMAN;
			whitePlayer = HUMAN;
			// ゲームを最初から最後までやる
			gameResult = playGomokuGame();
			// 結果をユーザの伝える
			if (gameResult == BLACK_WIN)
				printf("\nBlack player wins.\n\n");
			else if (gameResult == WHITE_WIN)
				printf("\nWhite player wins.\n\n");
			else if (gameResult == DRAW)
				printf("\nThis game ended in a draw.\n\n");
			else
				printf("\nGame was not finished.\n\n");

			printf("Press any key to continue.\n");
			while ((c = getchar()) != '\n')
				;
			break;

		case '2':
			// COMは白の場合にプレイヤーを初期化
			whitePlayer = BEST_AI;
			// ゲームを最初から最後までやる
			if (whitePlayer)
			{
				blackPlayer = HUMAN;
				gameResult = playGomokuGame();
				// 結果をユーザの伝える
				if (gameResult == BLACK_WIN)
					printf("\nBlack player wins.\n\n");
				else if (gameResult == WHITE_WIN)
					printf("\nWhite player wins.\n\n");
				else if (gameResult == DRAW)
					printf("\nThis game ended in a draw.\n\n");
				else
					printf("\nGame was not finished.\n\n");

				printf("Press any key to continue.\n");
				while ((c = getchar()) != '\n')
					;
			}
			break;

		case '3':
			// COMは黒の場合にプレイヤーを初期化
			blackPlayer = BEST_AI;
			if (blackPlayer)
			{
				whitePlayer = HUMAN;
				// ゲームを最初から最後までやる
				gameResult = playGomokuGame();
				// 結果をユーザの伝える
				if (gameResult == BLACK_WIN)
					printf("\nBlack player wins.\n\n");
				else if (gameResult == WHITE_WIN)
					printf("\nWhite player wins.\n\n");
				else if (gameResult == DRAW)
					printf("\nThis game ended in a draw.\n\n");
				else
					printf("\nGame was not finished.\n\n");

				printf("Press any key to continue.\n");
				while ((c = getchar()) != '\n')
					;
			}
			break;

		case '4':
			// COMvsCOM
			blackPlayer = BEST_AI;
			whitePlayer = TEST_AI;

			// ゲームを最初から最後までやる
			if (blackPlayer && whitePlayer)
			{

				firstAI = blackPlayer;
				secondAI = whitePlayer;

				numberOfGames = getNumberOfGames();

				// データを保存する準備
				printf("Enter filename to save data: ");
				scanf("%s", dataFileName);
				scanf("%*c");
				dataFile = fopen(dataFileName, "w");
				if (dataFile == NULL)
					printf("Unable to open data file\n");
				else
					saveData = 1;

				firstAIWins = secondAIWins = drawNumber = 0;

				for (i = 1; i <= numberOfGames; i++)
				{
					printf("Starting game number %d\n", i);
					if (saveData)
						fprintf(dataFile, "Starting game number %d\n", i);

					gameResult = playGomokuGame();

					// 結果を表示
					if (gameResult == BLACK_WIN)
					{
						printf("\nBlack player wins.\n");
						if (saveData)
							fprintf(dataFile, "\nBlack player wins.\n");
					}
					else if (gameResult == WHITE_WIN)
					{
						printf("\nWhite player wins.\n");
						if (saveData)
							fprintf(dataFile, "\nWhite player wins.\n");
					}
					else if (gameResult == DRAW)
					{
						printf("\nThis game ended in a draw.\n");
						if (saveData)
							fprintf(dataFile, "\nThis game ended in a draw.\n");
					}

					// 途中結果を更新
					if ((gameResult == BLACK_WIN && blackPlayer == firstAI) ||
						(gameResult == WHITE_WIN && whitePlayer == firstAI))
						firstAIWins++;
					else if ((gameResult == BLACK_WIN && blackPlayer == secondAI) ||
						(gameResult == WHITE_WIN && whitePlayer == secondAI))
						secondAIWins++;
					else if (gameResult == DRAW)
						drawNumber++;
					// 黒と白を入れ替えて、次の対局をやる
					if (i != numberOfGames)
					{
						printf("\nCurrent score: %s - %s %d-%d(%d)\n\n", namesAI[firstAI], namesAI[secondAI], firstAIWins, secondAIWins, drawNumber);
						if (saveData)
							fprintf(dataFile, "\nCurrent Score: %s - %s %d-%d(%d)\n\n", namesAI[firstAI], namesAI[secondAI], firstAIWins, secondAIWins, drawNumber);

						tempPlayer = blackPlayer;
						blackPlayer = whitePlayer;
						whitePlayer = tempPlayer;
						makeBoardEmpty();
						showBoard();
					}

				}

				// 最終結果を表示
				printf("\nFinal result: %s - %s %d-%d(%d)\n\n", namesAI[firstAI], namesAI[secondAI], firstAIWins, secondAIWins, drawNumber);
				if (saveData) {
					fprintf(dataFile, "\nFinal result: %s - %s %d-%d(%d)\n\n", namesAI[firstAI], namesAI[secondAI], firstAIWins, secondAIWins, drawNumber);
					fclose(dataFile);
				}

				printf("Press any key to continue.\n");
				while ((c = getchar()) != '\n')
					;
			}
			break;

		case 'Q':
			return 0;

		default:
			printf("Illegal input: Please select one of the menu options.\n");
			break;
		}
	}
	return 0;	
}

// 盤の初期化：すべてのマスを空きマスにする
void makeBoardEmpty()
{
	int x, y;

	for (x = 1; x <= BOARDSIZE; x++)
	for (y = 1; y <= BOARDSIZE; y++)
		gomokuBoard[x][y] = EMPTY_SQUARE;
}

// 現在の局面を表示
void showBoard()
{
	int x, y;

	printf("   １ ２ ３ ４ ５ ６ ７ ８ ９ 10 11 12 13 14 15\n");
	for (y = 1; y <= BOARDSIZE; y++)
	{
		if (y < 10)
			printf(" %d", y);
		else
			printf("%d", y);
		for (x = 1; x <= BOARDSIZE; x++)
		{
			if (gomokuBoard[x][y] == BLACK_STONE)
				printf(" ●");
			else if (gomokuBoard[x][y] == WHITE_STONE)
				printf(" ○");
			else
				printf(" ＊");
		}
		printf("\n");
	}
}

// 現在の局面をファイルに保存
void saveBoardToFile(FILE *fp)
{
	int x, y;

	fprintf(fp, "   １ ２ ３ ４ ５ ６ ７ ８ ９ 10 11 12 13 14 15\n");
	for (y = 1; y <= BOARDSIZE; y++)
	{
		if (y < 10)
			fprintf(fp, " %d", y);
		else
			fprintf(fp, "%d", y);
		for (x = 1; x <= BOARDSIZE; x++)
		{
			if (gomokuBoard[x][y] == BLACK_STONE)
				fprintf(fp, " ○");
			else if (gomokuBoard[x][y] == WHITE_STONE)
				fprintf(fp, " ●");
			else
				fprintf(fp, " ＊");
		}
		fprintf(fp, "\n");
	}
}

// 対局の数をユーザに入力してもらう
int getNumberOfGames()
{
	int game_number = 1;

	printf("\nEnter number of games to play: ");
	scanf("%d", &game_number);
	scanf("%*c");

	return game_number;
}

// 人間対COMの対局．ゲームが終わるまでに以下のことを繰り返す；
// 1) COMの手番にCOMの手を決める
// 2) 人間の手番の場合に人間の手を獲得
int playGomokuGame()
{
	int gameResult = UNFINISHED;
	int toMove = BLACK;

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
			if (saveData)
				fprintf(dataFile, "Thinking about my move...\n");
			if (decideComputerMove(toMove))
			{
				printf("Computer move: %d) (%d,%d)\n", moveCount, nextMoveX, nextMoveY);
				if (saveData)
					fprintf(dataFile, "Computer move: %d) (%d,%d)\n", moveCount, nextMoveX, nextMoveY);
			}
			else //	COMの手は決められない
			{
				printf("Computer move could not be decided.\n");
				if (saveData)
					fprintf(dataFile, "Computer move could not be decided.\n");
				if (blackPlayer != HUMAN)
					gameResult = WHITE_WIN;
				else
					gameResult = BLACK_WIN;
				break;
			}
		}
		else
		{
			// プレイヤーの手番
			if (askForHumanInput(toMove))
			{
				printf("You played: %d) (%d,%d)\n", moveCount, nextMoveX, nextMoveY);
				if (saveData)
					fprintf(dataFile, "You played: %d) (%d,%d)\n", moveCount, nextMoveX, nextMoveY);
			}
			else
			{
				printf("User aborted game.\n");
				break;
			}
		}

		// 石を盤に置く
		if (toMove == BLACK)
			gomokuBoard[nextMoveX][nextMoveY] = BLACK_STONE;
		else
			gomokuBoard[nextMoveX][nextMoveY] = WHITE_STONE;

		// 更新した局面を表示
		showBoard();

		// 対局の手を保存
		gameRecord[moveCount][0] = nextMoveX;
		gameRecord[moveCount][1] = nextMoveY;

		if (saveData)
			saveBoardToFile(dataFile);

		// 手がさされたので手数を更新
		moveCount++;

		// 終了条件が満たしているかどうか
		if (fiveInRowCheck(nextMoveX, nextMoveY, toMove))
		{
			// プレイヤーに手を戻すチャンスあげる（最後！）
			if (!takeBackAfterGame(toMove))
			{
				if (toMove == BLACK)
					gameResult = BLACK_WIN;
				else
					gameResult = WHITE_WIN;
				break;
			}
		}
		// 盤がいっぱいになったかどうかのチェック
		else if (moveCount == (BOARDSIZE * BOARDSIZE))
		{
			if (!takeBackAfterGame(toMove))
			{
				gameResult = DRAW;
				break;
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
		return 1;			// 横の五目並べ見つけた

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
		return 1;			// 縦の五目並べ見つけた

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
		return 1;			// 斜め（北西－南東）の五目並べ見つけた

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
		return 1;			// 斜め（北東－南西）の五目並べ見つけた

	return 0;
}

// 対局が終わってしまったが、入力ミスがあったので手を戻すことを可能する関数
int takeBackAfterGame(int toMove)
{
	char c, userSelection;

	// 手を戻すは人間プレイヤーだけ
	if ((toMove == BLACK && blackPlayer == HUMAN) ||
		(toMove == WHITE && whitePlayer == HUMAN))
	{
		// プレイヤーにオプションの選択を与える.　手を戻したいかどうかを確認
		while (1)
		{
			printf("\nGame over. Do you want to take back moves? (y/n): ");

			c = getchar();
			userSelection = c;
			while ((c = getchar()) != '\n')
				;

			switch (userSelection)
			{
			case 'y':
				// プレイヤーは手を戻したい（待った機能）
				if (moveCount <= 2)
					printf("Cannot take back first move of the game.\n");
				else
				{
					moveCount--;
					gomokuBoard[gameRecord[moveCount][0]][gameRecord[moveCount][1]] = EMPTY_SQUARE;
					moveCount--;
					gomokuBoard[gameRecord[moveCount][0]][gameRecord[moveCount][1]] = EMPTY_SQUARE;
				}
				showBoard();
				return 1;

			case 'n':
				return 0;

			default:
				printf("Illegal input: Please select one of the menu options.\n");
				break;
			}
		}
	}

	return 0;
}

// プレイヤーに新しい手を入力するか、以前指した手を戻すかに聞く
int askForHumanInput(int toMove)
{

	char c, userSelection;

	// プレイヤーにオプションの選択を与える．正しい手が入力されたか、プレイヤーはやめるかまでのループ
	while (1)
	{
		// 選択メニュ
		printf("1) Enter next move.\n");
		printf("2) Take back moves.\n");
		printf("Q) Quit game.\n");
		printf("Please enter your selection: ");

		c = getchar();
		userSelection = c;
		while ((c = getchar()) != '\n')
			;

		switch (userSelection)
		{
		case '1':
			// プレイヤーは次の手を入力したい
			getNextHumanMove();
			return 1;

		case '2':
			// プレイヤーは手を戻したい（待った機能）
			if (moveCount <= 2)
				printf("Cannot take back first move of the game.\n");
			else
			{
				moveCount--;
				gomokuBoard[gameRecord[moveCount][0]][gameRecord[moveCount][1]] = EMPTY_SQUARE;
				moveCount--;
				gomokuBoard[gameRecord[moveCount][0]][gameRecord[moveCount][1]] = EMPTY_SQUARE;
			}
			showBoard();
			break;

		case 'Q':
			return 0;

		default:
			printf("Illegal input: Please select one of the menu options.\n");
			break;
		}
	}

	return 0;
}

// プレイヤーから新しいてを獲得
void getNextHumanMove()
{
	int legalX, legalY, legalMove;
	int i;
	char c;
	char buf[256];

	legalX = legalY = legalMove = 0;
	// 正しい手を獲得できるまでに繰り返す
	while (!legalMove)
	{
		// 正しいx軸の獲得まで繰り返す
		while (!legalX)
		{
			printf("Enter x coordinate of move (1-15): ");
			c = getchar();
			for (i = 0; c != '\n'; i++)
			{
				buf[i] = c;
				c = getchar();
			}
			buf[i] = '\0';
			sscanf(buf, "%d", &nextMoveX);
			if (nextMoveX >= 1 && nextMoveX <= BOARDSIZE)
				legalX = 1;
			else
			{
				printf("The x-coordinate should be a number between 1 and 15.\n");
				nextMoveX = 0;
			}
		}
		// 正しいy軸の獲得まで繰り返す
		while (!legalY)
		{
			printf("Enter y coordinate of move (1-15): ");
			c = getchar();
			for (i = 0; c != '\n'; i++)
			{
				buf[i] = c;
				c = getchar();
			}
			buf[i] = '\0';
			sscanf(buf, "%d", &nextMoveY);
			if (nextMoveY >= 1 && nextMoveY <= BOARDSIZE)
				legalY = 1;
			else
			{
				printf("The y-coordinate should be a number between 1 and 15.\n");
				nextMoveY = 0;
			}
		}
		// 選んだマスは空きマスかどうかを確認
		if (gomokuBoard[nextMoveX][nextMoveY] == EMPTY_SQUARE)
			legalMove = 1;
		else
		{
			printf("The selected square is not empty.\n");
			nextMoveX = 0;
			nextMoveY = 0;
			legalX = 0;
			legalY = 0;
		}
	}

}

// COMの手を決める．これは五目並べプログラムのAIの部分．
int decideComputerMove(int toMove)
{
	int score;

	rootToMove = toMove;

	score = alphaBetaSearch(0, rootToMove, -INFINITYVAL, INFINITYVAL);
	printf("Best move evaluation: %d\n", score);
	if (saveData)
		fprintf(dataFile, "Best move evaluation: %d\n", score);

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

	// Generate all the moves by putting stones of the right color on the empty squares
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
						return WINNING - depth;			// 浅い探索の勝ちは深い探索の勝ちより良い
					}
					else
					{
						// Minプレイヤーの五目並べ
						// 手を戻す
						gomokuBoard[x][y] = EMPTY_SQUARE;
						return -(WINNING - depth);	// 浅い探索の勝ちは深い探索の勝ちより良い
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
// 北、北西、西、南西方向の連結確認はしなくてよい．（盤の調べる方向ですでに発見されました）
// この評価関数は効率が悪いし、値が調整していないので改善余地が十分ある．
int evaluate(int side, int nextToMove, int depth)
{
	int eval = 0;
	int x, y;
	int connectNo;			// 連続の同じ色の石の数
	int emptySqCon;			// 連結はどこまで伸ばせる
	int blocked;			// 石の連結はブロックされている
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
				else {
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
				if( blocked <= 1 && connectNo > 1 ) {
					for ( ; countX <= BOARDSIZE && countY >= 1 && gomokuBoard[countX][countY] == EMPTY_SQUARE ; countX++, countY--)
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
				else {
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
				if( blocked <= 1 && connectNo > 1 ) {
					for ( ; countX <= BOARDSIZE && gomokuBoard[countX][countY] == EMPTY_SQUARE ; countX++ )
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
				else {
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
				if( blocked <= 1 && connectNo > 1 ) {
					for ( ; countX <= BOARDSIZE && countY <= BOARDSIZE && gomokuBoard[countX][countY] == EMPTY_SQUARE ; countX++, countY++ )
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
				else {
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
				if( blocked <= 1 && connectNo > 1 ) {
					for ( ; countY <= BOARDSIZE && gomokuBoard[countX][countY] == EMPTY_SQUARE ; countY++ )
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
				else {
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
				if( blocked <= 1 && connectNo > 1 ) {
					for ( ; countX <= BOARDSIZE && countY >= 1 && gomokuBoard[countX][countY] == EMPTY_SQUARE ; countX++, countY-- )
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
				else {
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
				if( blocked <= 1 && connectNo > 1 ) {
					for ( ; countX <= BOARDSIZE && gomokuBoard[countX][countY] == EMPTY_SQUARE ; countX++ )
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
				else {
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
				if( blocked <= 1 && connectNo > 1 ) {
					for ( ; countX <= BOARDSIZE && countY <= BOARDSIZE && gomokuBoard[countX][countY] == EMPTY_SQUARE ; countX++, countY++ )
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
				else {
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
				if( blocked <= 1 && connectNo > 1 ) {
					for ( ; countY <= BOARDSIZE && gomokuBoard[countX][countY] == EMPTY_SQUARE ; countY++ )
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
		eval = WINNING - (depth + 1);				// 黒の手番と長さ４の連結あり、浅い勝ちは深い勝ちより良い
	else if ((whiteOpenFour || whiteClosedFour) && nextToMove == WHITE)
		eval = -(WINNING - (depth + 1));		// 白の手番と長さ４の連結あり、浅い勝ちは深い勝ちより良い
	else if (blackOpenFour >= 1)
		eval = WINNING - (depth + 2);				// 黒のOpen Four、浅い勝ちは深い勝ちより良い
	else if (whiteOpenFour >= 1)
		eval = -(WINNING - (depth + 2));		// 白のOpen Four、浅い勝ちは深い勝ちより良い
	else if (blackClosedFour >= 2)
		eval = WINNING - (depth + 2);				// 黒は連結４を二つ以上がある、浅い勝ちは深い勝ちより良い
	else if (whiteClosedFour >= 2)
		eval = -(WINNING - (depth + 2));		// 白は連結４を二つ以上がある、浅い勝ちは深い勝ちより良い
	else {
		// 最強のAIかテストAIの評価値を使うか
		if ((rootToMove == BLACK && blackPlayer == BEST_AI) ||
			(rootToMove == WHITE && whitePlayer == BEST_AI)) {
			//　勝ち以外の連結の評価（黒）
			eval += blackClosedFour * bestEvalValues[CLOSED_FOUR_VALUE];
			eval += blackOpenThree * bestEvalValues[OPEN_THREE_VALUE];
			eval += blackClosedThree * bestEvalValues[CLOSED_THREE_VALUE];
			eval += blackOpenTwo * bestEvalValues[OPEN_TWO_VALUE];
			eval += blackClosedTwo * bestEvalValues[CLOSED_TWO_VALUE];

			//　勝ち以外の連結の評価（白）
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
		else {
			//　勝ち以外の連結の評価（黒）
			eval += blackClosedFour * testValues[CLOSED_FOUR_VALUE];
			eval += blackOpenThree * testValues[OPEN_THREE_VALUE];
			eval += blackClosedThree * testValues[CLOSED_THREE_VALUE];
			eval += blackOpenTwo * testValues[OPEN_TWO_VALUE];
			eval += blackClosedTwo * testValues[CLOSED_TWO_VALUE];

			//　勝ち以外の連結の評価（白）
			eval -= whiteClosedFour * testValues[CLOSED_FOUR_VALUE];
			eval -= whiteOpenThree * testValues[OPEN_THREE_VALUE];
			eval -= whiteClosedThree * testValues[CLOSED_THREE_VALUE];
			eval -= whiteOpenTwo * testValues[OPEN_TWO_VALUE];
			eval -= whiteClosedTwo * testValues[CLOSED_TWO_VALUE];

			// 手番にボーナス
			if (nextToMove == BLACK)
			{
				eval += testValues[INITIATIVE];
			}
			else
			{
				eval -= testValues[INITIATIVE];
			}

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
void blackConnectionAdmin( int connectNo, int blocked, int openSquare )
{
	if ( blocked >= 2 ) {
		// 連結は完全にブラックされている
	}
	else if ( connectNo + openSquare < 5 )
	{
		// 連結は５まで伸ばせない
	}
	else if ( connectNo == 4 )
	{
		// 長さ４の連結を発見した
		if ( blocked == 0 )
			blackOpenFour++;
		else if ( blocked == 1 )
			blackClosedFour++;
	}
	else if ( connectNo == 3 )
	{
		// 長さ３の連結を発見した
		if ( blocked == 0 )
			blackOpenThree++;
		else if ( blocked == 1 )
			blackClosedThree++;
	}
	else if ( connectNo == 2 )
	{
		// 長さ２の連結を発見した
		if ( blocked == 0 )
			blackOpenTwo++;
		else if ( blocked == 1 )
			blackClosedTwo++;
	}
}

// 石の連結の種類の数を更新（白の場合）
void whiteConnectionAdmin( int connectNo, int blocked, int openSquare )
{
	if ( blocked >= 2 ) {
		// 連結は完全にブラックされている
	}
	else if ( connectNo + openSquare < 4 )
	{
		// 連結は５まで伸ばせない
	}
	else if ( connectNo == 4 )
	{
		// 長さ４の連結を発見した
		if ( blocked == 0 )
			whiteOpenFour++;
		else if ( blocked == 1 )
			whiteClosedFour++;
	}
	else if ( connectNo == 3 )
	{
		// 長さ３の連結を発見した
		if ( blocked == 0 )
			whiteOpenThree++;
		else if ( blocked == 1 )
			whiteClosedThree++;
	}
	else if ( connectNo == 2 )
	{
		// 長さ２の連結を発見した
		if ( blocked == 0 )
			whiteOpenTwo++;
		else if ( blocked == 1 )
			whiteClosedTwo++;
	}
}