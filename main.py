import numpy as np
from http.server import BaseHTTPRequestHandler, HTTPServer
import random

board = np.zeros((15, 15), dtype=int)

class MyServer(BaseHTTPRequestHandler):

    def do_GET(self):
        if self.path.startswith('/place/'):
            i, j = self.path[7:].split(',')
            i, j = int(i), int(j)

            # 手番の判定
            turn = 2 if np.sum(board) % 2 == 0 else 1

            # 石を置く
            board[i-1][j-1] = turn
            # レスポンスコードを設定
            self.send_response(200)

            # レスポンスヘッダーを設定
            self.send_header('Content-type', 'text/plain')
            self.end_headers()

            # 禁手判定
            if is_forbidden(board, i, j, turn):
                self.wfile.write(b'forbidden')

            # 勝敗判定
            elif is_win(board, i, j, turn):
                self.wfile.write(b'win')

            # 引き分け判定
            elif is_draw(board):
                self.wfile.write(b'draw')

            # 次の手を考える
            else:
                next_move = simulate(board, turn)
                self.wfile.write(str(next_move).encode())

            # 盤面を表示
            print(board)

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
    return (i+1, j+1)

if __name__ == "__main__":
    server = HTTPServer(('localhost', 8080), MyServer)
    print('Starting server, use <Ctrl-C> to stop')
    server.serve_forever()