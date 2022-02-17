import copy
import logging
from dicewars.ai.utils import attack_succcess_probability, possible_attacks
from dicewars.client.ai_driver import BattleCommand, EndTurnCommand, TransferCommand
from dicewars.ai.xholko02.ml import *
from dicewars.ai.xholko02.utils import *

"""
Authors:
Filip Bali, xbalif00
Natalia Holkova, xholko02
Roland Zitny, xzitny01
Vit Bartak, xbarta47
"""

class FinalAI:
    """ Agent using depth one search strategy with machine-learning evaluation
    of whole board depending on the player.
    Transfers of dices are made from border to front before attacks and defending transfers from weakest areas
    before ending turn.
    """

    def __init__(self, player_name, board, players_order, max_transfers):
        # Load trained neural network
        self.model = Network()
        self.model.load_state_dict(torch.load("./dicewars/ai/xholko02/model.pt"))
        self.model.eval()  # Necessary when trying to predict new data
        # Initialize serializer necessary for NN
        self.serializer = Serializer(board, len(players_order))

        self.board = board
        self.player_name = player_name
        self.logger = logging.getLogger('AI')
        self.player_order = players_order
        self.max_transfers = max_transfers
        self.stage = 'attack'

        self.useNN = len(self.player_order) == 4  # Use neural net? Only if 4 players present

    def evaluate_attack(self, attack):
        """ Depth one search with evaluation of board.

        Simulate current player turn and than simulate single enemy attack
        using Strength Difference Checking (SDC) strategy. After whole cycle of player and enemies evaluate board
        score with neural-network depending on current player.

        Return:
            [attack, evaluation] -> pair attack and evaluation score
        """
        board_simulation = copy.deepcopy(self.board)
        board_simulation = attack_simulation(board_simulation, attack)

        for enemy in self.player_order:
            if enemy != self.player_name:
                enemy_attacks = []
                enemy_possible_attacks = list(possible_attacks(board_simulation, enemy))
                for en_attack in enemy_possible_attacks:
                    source, target = en_attack
                    area_dice = source.get_dice()
                    strength_difference = area_dice - target.get_dice()
                    enemy_attack = [en_attack, strength_difference]
                    enemy_attacks.append(enemy_attack)

                enemy_attacks = sorted(enemy_attacks, key=lambda enemy_attack: enemy_attack[1], reverse=True)
                if enemy_attacks and enemy_attacks[0][1] >= 0:
                    board_simulation = attack_simulation(board_simulation, enemy_attacks[0][0])

        source, target = attack
        player_attack_poss = attack_succcess_probability(source.get_dice(), target.get_dice())
        board_simulation_evaluation = evaluate_board_NN(self.model, self.serializer, board_simulation, self.player_name) \
            if self.useNN else evaluate_board(board_simulation, self.player_name)

        return [attack, board_simulation_evaluation * player_attack_poss]

    def choose_best_attack(self, attacks):
        """Choosing best attack.

        Evaluate every possible attack and return one with highest evaluation score.

        Return:
            best_attack -> attack with highest evaluation score
        """
        # Count evaluation for every possible attack.
        evaluated_attacks = []
        for attack in attacks:
            source, target = attack
            if source.get_dice() >= target.get_dice():
                eval_attack = self.evaluate_attack(attack)
                evaluated_attacks.append(eval_attack)

        # Choose attack with best evaluation value.
        best_attack = None
        best_attack_eval = 0
        for eval_attack in evaluated_attacks:
            if eval_attack[1] > best_attack_eval:
                best_attack = eval_attack[0]  # attack
                best_attack_eval = eval_attack[1]  # evaluation value

        return best_attack

    def ai_turn(self, board, nb_moves_this_turn, nb_transfers_this_turn, nb_turns_this_game, time_left):
        """ AI agent's turn
        Gets all possible attack and choose one, which leads to most optimal outcome.
        Transfers are made before attacks and before ending turn.
        """

        # Move dices in front
        while nb_transfers_this_turn < (self.max_transfers - round(self.max_transfers / 4)):
            transfer = get_transfer_to_border(board, self.player_name)
            if transfer:
                return TransferCommand(transfer[0], transfer[1])
            else:
                break

        # Attack
        if self.stage == "attack":
            attacks = list(possible_attacks(board, self.player_name))
            best_attack = self.choose_best_attack(attacks)
            if best_attack:
                source, target = best_attack
                return BattleCommand(source.get_name(), target.get_name())
            else:
                self.stage = "evacuation"

        # Evacuation
        if self.stage == 'evacuation':
            if nb_transfers_this_turn < self.max_transfers:
                transfer = get_transfer_from_endangered(board, self.player_name)
                if transfer:
                    return TransferCommand(transfer[0], transfer[1])

        # End turn
        self.stage = 'attack'
        return EndTurnCommand()
