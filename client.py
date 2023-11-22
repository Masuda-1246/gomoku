import numpy as np
import random
import signal
import socket
import sys

SIZE = 15

board = np.zeros((SIZE, SIZE), dtype=int)
def flow(i, j):
  # 手番の判定
  turn = 2 if np.sum(board) % 2 == 0 else 1

  # 禁手判定
  if is_forbidden(board, i, j, turn):
      return 'forbidden'

  # 石を置く
  board[i-1][j-1] = turn

  # 引き分け判定
  if is_draw(board):
      return 'draw'

  # 次の手を考える
  next_move = simulate(board, turn)
  print(board)
  # 勝敗判定
  if is_win(board, turn):
      return 'win'
  return str(next_move)


# 禁じ手判定用の関数
def is_forbidden(board, i, j, turn):
  pre_c3 = count_amaount(board, turn, 3)
  pre_c4 = count_amaount(board, turn, 4)
  # tmpにboardの値をnumpy.copy()でコピー
  tmp = np.copy(board)
  tmp[i][j] = turn
  c3 = count_amaount(tmp, turn, 3)
  c4 = count_amaount(tmp, turn, 4)
  c6 = count_amaount(tmp, turn, 6)
  if c3 - pre_c3 >= 2 or c4 - pre_c4 >= 2 or c6 >= 1:
    return True
  return False

def is_valid_move(board, row, col):
    return 0 <= row < SIZE and 0 <= col < SIZE and board[row][col] == 0

def is_count(board, turn, start_row, start_col, delta_row, delta_col, amount):
    count = 0
    for i in range(amount+1):
        row = start_row + i * delta_row
        col = start_col + i * delta_col

        if 0 <= row < SIZE and 0 <= col < SIZE:
          if board[row][col] == turn:
            count += 1
          elif board[row][col] != 0:
            if count == amount:
              return True
            return False
    if count == amount:
      return True
    return False

def count_amaount(board, turn, amount):
  count = 0
  for i in range(SIZE):
    for j in range(SIZE):
      if board[i][j] == turn:
        for dx, dy in [(0, 1), (1, 0), (1, 1), (1, -1)]:
          if is_count(board, turn, i, j, dx, dy, amount):
            count += 1
  return count

# 勝敗判定の関数
def is_win(board, turn):
    c5 = count_amaount(board, turn, 5)
    if c5 >= 1:
        return True
    return False

# 引き分け判定の関数
def is_draw(board):
    # 判定処理
    if np.count_nonzero(board) == SIZE**2:
        return True
    return False

# 次の手を考える関数
def simulate(board, turn):
    i, j = 0, 0
    while True:
        i = random.randint(0, 14)
        j = random.randint(0, 14)
        if board[i][j] == 0:
            board[i][j] = turn%2 + 1
            break
    result = f"{i+1},{j+1}"
    return result

def signal_handler(sig, frame):
    print("プログラムを終了します。")
    sys.exit(0)

if __name__ == "__main__":
    signal.signal(signal.SIGINT, signal_handler)
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.bind(('127.0.0.1', 8080))
    server.listen(5)

    client, addr = server.accept()
    while True:
        request = client.recv(1024).decode()
        print(request)
        i, j = request.split(",")
        result = flow(int(i)-1, int(j)-1)
        http_response = result
        client.sendall(http_response.encode())
        client.close()
