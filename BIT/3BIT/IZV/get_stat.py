import matplotlib.pyplot as plt
import numpy as np
import os
import argparse
from download import DataDownloader

"""IZV project - 1st part
    Author: Natália Holková
    Login: xholko02
"""

def plot_stat(data_source, fig_location=None, show_figure=False):
    """Method to create graph of yearly accidents in regions of Czech republic.

    Keyword arguments:
        data_source  -- accident data from module download.py
        fig_location -- location for storing graph
        show_figure  -- True to show graph
    """
    if data_source is None:
        raise Exception("Error! No data source.")

    # Create dictionary for accidents by year(will have format: region name - number of accidents)
    accidents = {}

    # Iterate over all accident records
    for i in range(data_source[1][0].size):
        # Check accident region
        region = data_source[1][data_source[0].index("region")][i]
        date = data_source[1][data_source[0].index("date")][i]
        if date is None:
            continue
        try:
            year = date.year
        except ValueError:
            # invalid year format - skip this row
            continue

        # Increase accident count
        if year not in accidents.keys():
            accidents[year] = {region: 1}
        elif region not in accidents[year].keys():
            accidents[year][region] = 1
        else:
            accidents[year][region] += 1

    # find max number of accident to set y limit on graph (slightly bigger than it)
    accidents_by_years = [[num for region, num in stats.items()] for stats in accidents.values()]
    all_accidents = [inner for outer in accidents_by_years for inner in outer]  # join all nested lists into one list
    max_accidents = max(all_accidents)

    # prepare figure
    dpi = 96  # typical monitor dpi in order to get size in pixels we want
    plt.figure(figsize=(1000/dpi, 1600/dpi), dpi=dpi)
    plt.suptitle("Počty nehôd v krajoch Českej republiky", weight="bold", size=20)
    num_rows = len(accidents.keys())
    plot_index = 1

    for year, stats in accidents.items():
        ax = plt.subplot(num_rows, 1, plot_index)
        ax.spines['right'].set_visible(False)  # set top border invisible
        ax.spines['top'].set_visible(False)  # set right border invisible
        regions = list(stats.keys())
        regions.sort()
        num_accidents = [stats[region] for region in regions]

        # get region order
        order = [sorted(num_accidents, reverse=True).index(num) + 1 for num in num_accidents]

        # making the graph
        plt.title(str(year), size=14)  # Subplot title
        plt.grid(axis="y", zorder=0)  # Horizontal grid
        plt.ylabel("$\\it{Počet\ nehôd}$")  # Y label
        plt.ylim(top=max_accidents + 4000)
        plt.yticks(np.arange(0, max_accidents + 4000, 4000))
        barlist = plt.bar(regions, num_accidents, zorder=3, color="blue")  # zorder to get grid behind bars
        xlocs, xlabs = plt.xticks()
        plt.subplots_adjust(hspace=0.8)

        for i, v in enumerate(num_accidents):  # show exact number of accidents
            plt.text(xlocs[i], num_accidents[i] - 2000, num_accidents[i], fontsize=8, color="white", ha="center")

        for i, v in enumerate(order):  # show region order in list of accidents sorted from max to min
            plt.text(xlocs[i], num_accidents[i] + 500, str(v), ha='center')

        # set bar with most accidents to a different color
        barlist[order.index(1)].set_color('darkblue')

        plot_index += 1

    # show or save figure
    if fig_location is not None:
        # check if folder where to save it exists
        folder = os.path.dirname(fig_location)
        if not os.path.exists(folder):
            os.makedirs(folder)
        plt.savefig(fig_location, dpi=dpi)
    if show_figure:  # figure location not set -> only show graph
        plt.show()

    plt.close()


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Visualize graph of accidents.')
    parser.add_argument("--fig_location", type=str, help="figure location for saving (path + file new with extension)")
    parser.add_argument("--show_figure", help="show figure", default=False, action="store_true")
    args = parser.parse_args()

    #data = DataDownloader().get_list()
    data = DataDownloader().get_list(["JHM", "PAK", "OLK"])
    plot_stat(data, fig_location=args.fig_location, show_figure=args.show_figure)
