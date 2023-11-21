import requests

SERVER_URL = "http://localhost:8080/place" 

def place_stone(x, y):
  url = f"{SERVER_URL}/{x},{y}"
  response = requests.get(url)
  
  print(response.text)

# ゲームの進行を管理する
board = [[0] * 15 for _ in range(15)] 

x, y = 7, 7 # 打つ座標
board[x][y] = 1 # 盤面を更新
place_stone(x+1, y+1) 

x, y = 8, 8
board[x][y] = 2
place_stone(x+1, y+1)
