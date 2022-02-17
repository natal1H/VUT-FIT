from .area import Area

from typing import List, Optional


class Board:
    """Object representing the game board
    """
    def __init__(self, board):
        """
        Parameters
        ----------
        board : dict of int: list of int
            Dictionary listing adjacent areas for each area

        Attributes
        ----------
        areas : dict of int: Area
            Dictionary of Area instances
        """
        self.board = board
        self.areas = {}
        for area in board:
            self.areas[area] = Area(area, board[area]['neighbours'])
        for a in self.areas:
            self.areas[a].add_adjacent_areas(self)

    def get_area_by_name(self, name):
        """Get instance of Area by its name

        Parameters
        ----------
        name : int
            Area's name

        Returns
        -------
        Area
            Instance of an area
        """
        for a in self.areas:
            if name == self.areas[a].get_name():
                return self.areas[a]

    def get_board(self):
        """Get dictionary listing adjacent areas for each area

        Returns
        -------
        dictionary of int: list of int
        """
        return self.board

    def get_number_of_areas(self):
        """Get number of areas in the game

        Returns
        -------
        int
        """
        return len(self.areas)

    def get_player_areas(self, player_name: int) -> List[Area]:
        """Get all Areas belonging to a player
        """
        return [area for area in self.areas.values() if area.get_owner_name() == player_name]

    def get_players_regions(self, player_name: int, skip_area: Optional[int] = None) -> List[List[int]]:
        """Get all unbroken regions belonging to a player.

        Returns them as a list of regions, where every region a list of names of area in the region.
        If skip_area is given, it is treated as not belonging to the player.
        """
        area_names_to_test = [area.get_name() for area in self.get_player_areas(player_name) if area.get_name() != skip_area]

        if not area_names_to_test:
            return [[]]

        regions = []
        while area_names_to_test:
            area_names_in_current_region = self.get_areas_region(area_names_to_test[0], area_names_to_test)
            assert(len(area_names_in_current_region) > 0)
            regions.append(area_names_in_current_region)

            for area in area_names_in_current_region:
                area_names_to_test.remove(area)

        return regions

    def get_areas_region(self, area_name: int, available_areas: List[int]) -> List[int]:
        """Get all areas from available_areas which are in the same region as the given one.

        Returns them as a list of regions, where every region a list of names of area in the region.
        """
        to_test = {area_name}
        current_region = []

        while to_test:
            current_area = to_test.pop()

            if current_area in current_region:
                continue

            current_region.append(current_area)

            for neighbour_name in self.get_area_by_name(current_area).get_adjacent_areas_names():
                if neighbour_name in current_region:
                    continue
                if neighbour_name in current_region:
                    continue

                if neighbour_name in available_areas:
                    to_test.add(neighbour_name)

        return current_region

