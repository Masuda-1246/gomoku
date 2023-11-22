import numpy as np
import random
import signal
import socket
import sys

board = np.zeros((15, 15), dtype=int)
def flow(i, j):
  # 手番の判定
  turn = 2 if np.sum(board) % 2 == 0 else 1

  # 石を置く
  board[i-1][j-1] = turn

  # 禁手判定
  if is_forbidden(board, i, j, turn):
      return 'forbidden'

  # 勝敗判定
  elif is_win(board, i, j, turn):
      return 'win'

  # 引き分け判定
  elif is_draw(board):
      return 'draw'

  # 次の手を考える
  else:
      next_move = simulate(board, turn)
      print(board)
      return str(next_move)


# 禁じ手判定用の関数
def is_forbidden(board, i, j, turn):
  # 三三判定
  if has_3stones(board, i, j, turn):
    return True
  
  # 四四判定
  if has_4stones(board, i, j, turn):
    return True

  # 長連判定
  if is_long_line(board, i, j, turn):
    return True

  # 禁じ手なし
  return False

# 三を構成する判定
def has_3stones(board, i, j, turn):
  return False

# 四を構成する判定
def has_4stones(board, i, j, turn):
  return False

# 長連(6子以上)判定
def is_long_line(board, i, j, turn):
  return False

# 勝敗判定の関数
def is_win(board, i, j, turn):
    return False

# 引き分け判定の関数
def is_draw(board):
    # 判定処理
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

    while True:
        client, addr = server.accept()
        request = client.recv(1024).decode()
        print(request)
        i, j = request.split(",")
        result = flow(int(i), int(j))
        http_response = result
        client.sendall(http_response.encode())
        client.close()