from ..utils import probability_of_holding_area

"""
Authors:
Filip Bali, xbalif00
Natalia Holkova, xholko02
Roland Zitny, xzitny01
Vit Bartak, xbarta47
"""

def attack_simulation(board, attack):
    """
    Simulate attack on board and do changes.

    Return:
        board -> board with changes
    """
    if attack is None:
        return board

    source, target = attack
    source_area = board.get_area(source.get_name())
    target_area = board.get_area(target.get_name())
    target_area.set_owner(source.get_owner_name())
    target_area.set_dice(source.get_dice() - 1)
    source_area.set_dice(1)
    return board


def evaluate_board(board, player):
    """
    Evaluate board without machine learning
    """

    # Count number of dices on all fields of player
    dices_number = board.get_player_dice(player)

    # Get size of biggest region of player
    biggest_region_size = 0
    player_regions = board.get_players_regions(player)
    for region in player_regions:
        if len(region) > biggest_region_size:
            biggest_region_size = len(region)

    # Return some evaluation of the board
    evaluation = dices_number + biggest_region_size

    return evaluation


def get_transfer_to_border(board, player_name):
    """
    Transfers from other AI
    """
    border_names = [a.name for a in board.get_player_border(player_name)]
    all_areas = board.get_player_areas(player_name)
    inner = [a for a in all_areas if a.name not in border_names]

    for area in inner:
        if area.get_dice() < 2:
            continue

        for neigh in area.get_adjacent_areas_names():
            if neigh in border_names and board.get_area(neigh).get_dice() < 8:
                return area.get_name(), neigh

    return None


def areas_expected_loss(board, player_name, areas):
    """
    Transfers from other AI
    """
    hold_ps = [probability_of_holding_area(board, a.get_name(), a.get_dice(), player_name) for a in areas]
    return sum((1-p) * a.get_dice() for p, a in zip(hold_ps, areas))


def get_transfer_from_endangered(board, player_name):
    """
    Transfers from other AI
    """
    border_names = [a.name for a in board.get_player_border(player_name)]
    all_areas_names = [a.name for a in board.get_player_areas(player_name)]

    retreats = []

    for area in border_names:
        area = board.get_area(area)
        if area.get_dice() < 2:
            continue

        for neigh in area.get_adjacent_areas_names():
            if neigh not in all_areas_names:
                continue
            neigh_area = board.get_area(neigh)

            expected_loss_no_evac = areas_expected_loss(board, player_name, [area, neigh_area])

            src_dice = area.get_dice()
            dst_dice = neigh_area.get_dice()

            dice_moved = min(8-dst_dice, src_dice - 1)

            area.dice -= dice_moved
            neigh_area.dice += dice_moved

            expected_loss_evac = areas_expected_loss(board, player_name, [area, neigh_area])

            area.set_dice(src_dice)
            neigh_area.set_dice(dst_dice)

            retreats.append(((area, neigh_area), expected_loss_no_evac - expected_loss_evac))

    retreats = sorted(retreats, key=lambda x: x[1], reverse=True)

    if retreats:
        retreat = retreats[0]
        if retreat[1] > 0.0:
            return retreat[0][0].get_name(), retreat[0][1].get_name()

    return None
