import torch
import torch.nn as nn
import torch.nn.functional as F
import numpy as np
from dicewars.client.game.board import Board

"""
Authors:
Filip Bali, xbalif00
Natalia Holkova, xholko02
Roland Zitny, xzitny01
Vit Bartak, xbarta47
"""

def relu(x):
    return x * F.relu(x)


class Network(nn.Module):

    def __init__(self):
        super().__init__()

        self.fc1 = nn.Linear(633, 256)
        self.b1 = nn.BatchNorm1d(256)
        self.fc2 = nn.Linear(256, 64)
        self.b2 = nn.BatchNorm1d(64)
        self.fc4 = nn.Linear(64, 4)

    def forward(self, x):

        x = relu(self.fc1(x))
        x = self.b1(x)
        x = relu(self.fc2(x))
        x = self.b2(x)
        x = self.fc4(x)

        return x


class Serializer:
    numAreas = 34  # Number of areas on board (names from 1 to 34 (including)

    def __init__(self, board: Board, players):
        """
        At start it determines the matrix of neighbouring areas, because that doesn't change
        """
        self.numPlayers = players
        neighbours_matrix = self.get_neighbours_matrix(board)
        self.neighbours_triangle = get_matrix_upper_triangle(neighbours_matrix)

    def serialize_board_state(self, board: Board):
        # each round we need new info about areas and biggest regions
        areas_info = self.get_array_of_area_info(board)
        biggest_regions = self.get_array_of_biggest_regions(board)
        # concatenate arrays into one
        game_state = np.concatenate((self.neighbours_triangle, areas_info, biggest_regions), dtype=int)
        return torch.from_numpy(game_state).type(torch.float32)

    def get_neighbours_matrix(self, board: Board):
        matrix = []

        for i in range(1, self.numAreas + 1):
            row = []
            area = board.get_area(i)
            neighbours = area.get_adjacent_areas_names()
            for j in range(1, self.numAreas + 1):
                if j in neighbours:
                    row.append(1)
                else:
                    row.append(0)
            matrix.append(row)
        return matrix

    def get_array_of_area_info(self, board: Board):
        areas_info = []
        for i in range(1, self.numAreas + 1):
            area = board.get_area(i)
            dice = area.get_dice()
            owner = area.get_owner_name()
            areas_info.append(dice)
            areas_info.append(owner)
        return areas_info

    def get_array_of_biggest_regions(self, board: Board):
        biggest_regions = []
        for i in range(1, self.numPlayers + 1):
            player_regions = board.get_players_regions(i)
            biggest_regions.append(max(len(elem) for elem in player_regions))
        return biggest_regions


def get_matrix_upper_triangle(matrix):
    """
    N = 3
    matrix:
    [[00,01,02],
     [10,11,12],
     [20,21,22]]
    upper_triangle:
     --,01,02
     --,--,12
     --,--,--
    as single array: [01,02,12]
    """
    triangle = []
    N = len(matrix)
    for row in range(0, N):
        for col in range(row + 1, N):
            triangle.append(matrix[row][col])
    return triangle


def get_player_win_prob(probs, player):
    """
    Get probabylity for player [1/2/3/4] winning
    """
    return probs.numpy()[0][player - 1]


def evaluate_board_NN(model, serializer, board, player):
    board_state = serializer.serialize_board_state(board)
    with torch.no_grad():
        probabilities = model(board_state[None, ...])  # Has to be this way for some reason
        probabilities_normalized = torch.softmax(probabilities, dim=1)
    evaluation = get_player_win_prob(probabilities_normalized, player)
    return evaluation


