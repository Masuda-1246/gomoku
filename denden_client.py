from __future__ import print_function
import pickle
from game import Board, Game
from mcts_pure import MCTSPlayer as MCTS_Pure
from mcts_alphaZero import MCTSPlayer
from policy_value_net_numpy import PolicyValueNetNumpy
# from policy_value_net import PolicyValueNet  # Theano and Lasagne
from policy_value_net_pytorch import PolicyValueNet  # Pytorch
# from policy_value_net_tensorflow import PolicyValueNet # Tensorflow
# from policy_value_net_keras import PolicyValueNet  # Keras
import numpy as np
import random
import signal
import socket
import sys


SIZE = 15

board_client = np.zeros((SIZE, SIZE), dtype=int)
def flow(i, j):
  # 手番の判定
  turn = 0 if np.sum(board_client) % 2 == 0 else 1

  # 禁手判定
  if is_forbidden(board_client, i, j, turn):
      return 'forbidden'

  # 石を置く
  board_client[i-1][j-1] = turn

  # 引き分け判定
  if is_draw(board_client):
      return 'draw'

  # 次の手を考える
  if flag==0:
    flag=1
    for element in board_client:
      if element != 0:
        next_move = simulate(board_client, turn)
        break
    next_move = "7,7"
    
  next_move = simulate(board_client, turn) 
  print(board_client)
  # 勝敗判定
  if is_win(board_client, turn):
      return 'win'
  return str(next_move)


# 禁じ手判定用の関数
def is_forbidden(board_client, i, j, turn):
  pre_c3 = count_amaount(board_client, turn, 3)
  pre_c4 = count_amaount(board_client, turn, 4)
  # tmpにboard_clientの値をnumpy.copy()でコピー
  tmp = np.copy(board_client)
  tmp[i][j] = turn
  c3 = count_amaount(tmp, turn, 3)
  c4 = count_amaount(tmp, turn, 4)
  c6 = count_amaount(tmp, turn, 6)
  if c3 - pre_c3 >= 2 or c4 - pre_c4 >= 2 or c6 >= 1:
    return True
  return False

def is_valid_move(board_client, row, col):
    return 0 <= row < SIZE and 0 <= col < SIZE and board_client[row][col] == 0

def is_count(board_client, turn, start_row, start_col, delta_row, delta_col, amount):
    count = 0
    for i in range(amount+1):
        row = start_row + i * delta_row
        col = start_col + i * delta_col

        if 0 <= row < SIZE and 0 <= col < SIZE:
          if board_client[row][col] == turn:
            count += 1
          elif board_client[row][col] != 0:
            if count == amount:
              return True
            return False
    if count == amount:
      return True
    return False

def count_amaount(board_client, turn, amount):
  count = 0
  for i in range(SIZE):
    for j in range(SIZE):
      if board_client[i][j] == turn:
        for dx, dy in [(0, 1), (1, 0), (1, 1), (1, -1)]:
          if is_count(board_client, turn, i, j, dx, dy, amount):
            count += 1
  return count

# 勝敗判定の関数
def is_win(board_client, turn):
    c5 = count_amaount(board_client, turn, 5)
    if c5 >= 1:
        return True
    return False

# 引き分け判定の関数
def is_draw(board_client):
    # 判定処理
    if np.count_nonzero(board_client) == SIZE**2:
        return True
    return False

# # 次の手を考える関数
# def simulate(board_client2, turn):
#     current_player = board_client.get_current_player()
#     player_in_turn = players[current_player]
#     opp_move = player_in_turn.get_action(board_client, i, j)
#     board_client.do_move(opp_move)
    
#     current_player = board_client.get_current_player()
#     player_in_turn = players[current_player]
#     move = player_in_turn.get_action(board_client)
#     board_client.do_move(move)
#     return move
           
        
    i, j = 0, 0
    while True:
        i = random.randint(0, 14)
        j = random.randint(0, 14)
        if board_client2[i][j] == 0:
            board_client2[i][j] = turn%2 + 1
            break
    result = f"{i+1},{j+1}"
    return result

def signal_handler(sig, frame):
    print("プログラムを終了します。")
    sys.exit(0)

import socket

class Human(object):
    """
    human player that connects to server
    """

    def __init__(self):
        self.player = None
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.connect(('localhost', 8080))

    def set_player_ind(self, p):
        self.player = p

    def get_action(self, board):
        # receive input coord from server
        data = self.socket.recv(1024)
        location = data.decode()
        
        if isinstance(location, str):
            location = [int(n) for n in location.split(",")]
        move = board.location_to_move(location) 

        if move not in board.availables:
            print("invalid move")
            move = self.get_action(board)
        return move

    def __str__(self):
        return "Human {}".format(self.player)

def run():
    n = 5
    width, height = 15, 15
    model_file = 'current_policy_15_15_5_gen2.model'
    try:
        board = Board(width=width, height=height, n_in_row=n)
        game = Game(board)

        # ############### human VS AI ###################
        # load the trained policy_value_net in either Theano/Lasagne, PyTorch or TensorFlow

        best_policy = PolicyValueNet(width, height, model_file = model_file)
        mcts_player = MCTSPlayer(best_policy.policy_value_fn, c_puct=5, n_playout=400)

        # load the provided model (trained in Theano/Lasagne) into a MCTS player written in pure numpy

        
        # try:
        #     policy_param = pickle.load(open(model_file, 'rb'))
        # except:
        #     policy_param = pickle.load(open(model_file, 'rb'),
        #                                encoding='bytes')  # To support python3
        # best_policy = PolicyValueNetNumpy(width, height, policy_param)
        # mcts_player = MCTSPlayer(best_policy.policy_value_fn,
        #                          c_puct=5,
        #                          n_playout=400)  # set larger n_playout for better performance

        # uncomment the following line to play with pure MCTS (it's much weaker even with a larger n_playout)
        # mcts_player = MCTS_Pure(c_puct=5, n_playout=1000)

        # human player, input your move in the format: 2,3
        human = Human()

        # set start_player=0 for human first
        game.start_play(human, mcts_player, start_player=1, is_shown=1)
    except KeyboardInterrupt:
        print('\n\rquit')


if __name__ == '__main__':
    run()
