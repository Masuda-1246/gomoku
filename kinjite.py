SIZE = 15

def is_valid_move(board, row, col):
    return 0 <= row < SIZE and 0 <= col < SIZE and board[row][col] == 0

# Example usage
import numpy as np

board = np.array([
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0], # 0
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0], # 1
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0], # 2
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0], # 3
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0], # 4
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0], # 5
    [0, 0, 0, 0, 0, 0, 2, 0, 2, 0, 0, 0, 0, 0, 0], # 6
    [0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 0, 0, 0, 0, 0], # 7
    [0, 0, 0, 0, 0, 0, 1, 2, 1, 0, 0, 0, 0, 0, 0], # 8
    [0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0], # 9
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0], # 10
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0], # 11
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0], # 12
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0], # 13
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0], # 14
  #  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14
])
player = 1  # Assuming player 1 is starting


def is_count(board, player, start_row, start_col, delta_row, delta_col, amount):
    count = 0
    for i in range(amount+1):
        row = start_row + i * delta_row
        col = start_col + i * delta_col

        if 0 <= row < SIZE and 0 <= col < SIZE:
          if board[row][col] == player:
            count += 1
          elif board[row][col] != 0:
            if count == amount:
              return True
            return False
    if count == amount:
      return True
    return False

def count_amaount(board, player, amount):
  count = 0
  for i in range(SIZE):
    for j in range(SIZE):
      if board[i][j] == player:
        for dx, dy in [(0, 1), (1, 0), (1, 1), (1, -1)]:
          if is_count(board, player, i, j, dx, dy, amount):
            count += 1
  return count


while True:
    # Player's move
    row = int(input("Enter row: "))
    col = int(input("Enter column: "))

    pre_c3 = count_amaount(board, player, 3)
    pre_c4 = count_amaount(board, player, 4)
    if is_valid_move(board, row, col):
        board[row][col] = player
    else:
        print("Invalid move. Try again.")
        continue
    c3 = count_amaount(board, player, 3)
    c4 = count_amaount(board, player, 4)
    c5 = count_amaount(board, player, 5)
    c6 = count_amaount(board, player, 6)

    if c3 - pre_c3 >= 2:
      print(f"Player {player} is forbidden So Player {3 - player} is win")
      board[row][col] = 0
      break
    if c4 - pre_c4 >= 2:
      print(f"Player {player} is forbidden So Player {3 - player} is win")
      board[row][col] = 0
      break
    if c6 >= 1:
      print(f"Player {player} is forbidden So Player {3 - player} is win")
      board[row][col] = 0
      break
    if c5 >= 1:
      print(f"Player {player} is win")
      break
    print(board)

    # Switch players
    player = 3 - player
