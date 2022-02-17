#!/usr/bin/env python3.8
# coding=utf-8

from matplotlib import pyplot as plt
import pandas as pd
import seaborn as sns
import numpy as np
import os

"""IZV project - 2nd part
    Author: Natália Holková
    Login: xholko02
"""

B_IN_MB: int = 1048576  # 1 MB = 1 048 576 B, constant for converting size to MB


def get_dataframe(filename: str, verbose: bool = False) -> pd.DataFrame:
    """
    Project task n. 1: Unpack compressed DataFrame and edit columns to lower final size
    :param filename: location of .pkl.gz file containing the DataFrame
    :param verbose: Print original and new size in MB
    :return: unpacked DataFrame
    """

    if os.path.isfile(filename):  # data file exists
        df = pd.read_pickle(filename, compression="gzip")

        # create date column in DataFrame
        df["date"] = pd.to_datetime(df["p2a"], format="%Y-%m-%d", errors="coerce")

        # get memory usage before changing columns
        old_size = df.memory_usage(deep=True, index=True).sum() / B_IN_MB

        # change data types of certain columns
        to_int32_cols = ["p1", "p36", "p37", "weekday(p2a)", "p6", "r", "s"]  # columns that should be int32
        df[to_int32_cols] = df[to_int32_cols].replace(r'^\s*$', "-1", regex=True)  # fill missing values
        df[to_int32_cols] = df[to_int32_cols].astype("int32")
        df["p2a"] = df["date"]  # date column - just copy "date" column
        df["p2b"] = df["p2b"].apply(lambda x: (str(int(x) // 100) if (int(x) // 100 > 9)
                                               else "0" + str(int(x) // 100)) + ":" + (str(int(x) % 100)
                                                                                       if (int(x) % 100 > 9)
                                                                                       else "0" + str(int(x) % 100)))
        df["p2b"] = pd.to_datetime(df["p2b"], format="%H:%M", errors="coerce")
        to_string_cols = ["h", "i", "j", "l", "n", "o", "t", ]  # columns that should be string
        df[to_string_cols] = df[to_string_cols].astype("string")
        to_categories_cols = ["p", "q", "k"]
        df[to_categories_cols] = df[to_categories_cols].astype('category')

        # get new size
        new_size = df.memory_usage(deep=True, index=True).sum() / B_IN_MB

        if verbose:
            print("old_size={} MB".format(round(old_size, 1)))
            print("new_size={} MB".format(round(new_size, 1)))

        return df

    else:  # data file does not exist
        raise FileNotFoundError("File " + filename + " containing data not found.")


def plot_conseq(df: pd.DataFrame, fig_location: str = None, show_figure: bool = False):
    """
    Project task n. 2: Plot the accident consequences in each region
    :param df: DataFrame containing accidents data
    :param fig_location: if not None, then location where to store figure
    :param show_figure: if True, will show the figure
    :return: doesn't return anything
    """
    if df is not None:
        # group by regions
        grouped_df = df.groupby('region').agg({'p1': 'count', 'p13a': 'sum', 'p13b': 'sum', 'p13c': 'sum'})
        grouped_df = grouped_df.reset_index()

        sns.set()
        fig, axs = plt.subplots(4, 1, squeeze=False, figsize=(10, 10))  # 4 plots, each in its own row

        # Prepare palettes for plots
        palette = np.array(sns.color_palette("rocket", len(grouped_df)))  # palette for last plot

        regs_p1 = list(grouped_df.sort_values('p1', ascending=False).region)  # regions ordered by column p1
        regs_p13a = list(grouped_df.sort_values('p13a', ascending=False).region)  # regions ordered by column p13a
        regs_p13b = list(grouped_df.sort_values('p13b', ascending=False).region)  # regions ordered by column p13b
        regs_p13c = list(grouped_df.sort_values('p13c', ascending=False).region)  # regions ordered by column p13c

        # prepare empty palettes for other 3 plots (p13a, p13b, p13c)
        palette_p13a = np.zeros((len(grouped_df), 3))
        palette_p13b = np.zeros((len(grouped_df), 3))
        palette_p13c = np.zeros((len(grouped_df), 3))

        # in loop go over regions sorted by p1 (last plot), check where that region should be by p13a/b/c column
        # and place that region on that place in palette for p13a/b/c
        for i in range(len(regs_p1)):
            palette_p13a[i] = palette[regs_p13a.index(regs_p1[i])]
            palette_p13b[i] = palette[regs_p13b.index(regs_p1[i])]
            palette_p13c[i] = palette[regs_p13c.index(regs_p1[i])]

        # plot by column p13a
        ax = sns.barplot(ax=axs[0, 0], x="region", y="p13a", data=grouped_df,
                         order=grouped_df.sort_values('p1', ascending=False).region, palette=palette_p13a)
        ax.set(xlabel="", ylabel="Počet")
        ax.set_title("Úmrtia")
        ax.set(xticklabels=[])

        # plot by column p13b
        ax = sns.barplot(ax=axs[1, 0], x="region", y="p13b", data=grouped_df,
                         order=grouped_df.sort_values('p1', ascending=False).region, palette=palette_p13b)
        ax.set(xlabel="", ylabel="Počet")
        ax.set_title("Ťažko ranení")
        ax.set(xticklabels=[])

        # plot by column p13c
        ax = sns.barplot(ax=axs[2, 0], x="region", y="p13c", data=grouped_df,
                         order=grouped_df.sort_values('p1', ascending=False).region, palette=palette_p13c)
        ax.set(xlabel="", ylabel="Počet")
        ax.set_title("Ľahko ranení")
        ax.set(xticklabels=[])

        # plot by column p1
        ax = sns.barplot(ax=axs[3, 0], x="region", y="p1", data=grouped_df,
                         order=grouped_df.sort_values('p1', ascending=False).region, palette=palette)
        ax.set(xlabel="", ylabel="Počet")
        ax.set_title("Celkom nehôd")

        plt.tight_layout()

        if fig_location:
            plt.savefig(fig_location)  # Store the figure

        if show_figure:
            plt.show()  # Show the figure

    else:  # df is None
        raise ValueError("No DataFrame provided.")


def plot_damage(df: pd.DataFrame, fig_location: str = None, show_figure: bool = False):
    """
    Project task n. 3: Plot the accident damage in chosen regions
    :param df: DataFrame containing accidents data
    :param fig_location: if not None, then location where to store figure
    :param show_figure: if True, will show the figure
    :return: doesn't return anything
    """
    if df is not None:
        regions = ['JHM', 'HKK', 'PLK', 'PHA']

        sns.set()
        fig, axs = plt.subplots(2, 2, squeeze=False, sharey="all", figsize=(15, 10))
        idx = 0  # subplot index

        for reg in regions:
            reg_df = df[df['region'] == reg]  # get df containing only entries for current region
            reg_df = reg_df[['region', 'p12', 'p53']]

            # cut df by damage cause into bins, set labels
            reg_df['damage_types_bins'] = pd.cut(x=reg_df['p12'], bins=[0, 200, 300, 400, 500, 600, 700],
                                                 labels=['nezavinená vodičom', 'neprimeraná rýchlost jazdy',
                                                         'nesprávne predbiehanie', 'nedanie prednosti v jazde',
                                                         'nesprávny spôsob jazdy', 'technická závada vozidla'])
            # cut df by damage size into bins, set labels
            # kind of cheat with 10001 since last bin is "> 1000" (left side not includes 10000),
            # intervals are (as understood from task description, ( = not includes, ] = includes:
            # (-inf, 500), <500, 2000>, (2000, 5000>, (5000, 10000>, (10000, inf)
            reg_df['damage_bins'] = pd.cut(x=reg_df['p53'], right=True,
                                           bins=[-1, 499, 2000, 5000, 10000, float("inf")],
                                           labels=['< 50', '50 - 200', '200 - 500', '500 - 1000', '> 1000'])

            ax = sns.countplot(ax=axs[idx // 2, idx % 2], x="damage_bins", hue="damage_types_bins", data=reg_df)
            ax.set_title(reg)
            ax.set_yscale('log')  # logarithmic scale
            ax.set(xlabel="Škoda [tis. Kč]", ylabel="Počet")
            idx += 1

            # sort out legend
            if idx < len(regions):  # not the last graph
                handles, labels = ax.get_legend_handles_labels()
                ax.legend(handles[:0], labels[:0], frameon=False)
            else:  # last graph - only it has legend
                plt.legend(bbox_to_anchor=(1.05, 1), loc=2, borderaxespad=0., title="Príčina nehody",
                           frameon=False)  # legend on the right
        plt.tight_layout()

        if fig_location:
            plt.savefig(fig_location)  # Store the figure

        if show_figure:
            plt.show()  # Show the figure

    else:  # df is None
        raise ValueError("No DataFrame provided.")


def plot_surface(df: pd.DataFrame, fig_location: str = None, show_figure: bool = False):
    """
    Project task n. 4: Plot the surface types during accidents in chosen regions
    :param df: DataFrame containing accidents data
    :param fig_location: if not None, then location where to store figure
    :param show_figure: if True, will show the figure
    :return: doesn't return anything
    """
    if df is not None:
        regions = ['JHM', 'HKK', 'PLK', 'PHA']

        sns.set()
        fig, axs = plt.subplots(2, 2, squeeze=False, sharey="all", figsize=(14, 10))
        idx = 0  # subplot index

        tmp_df = df[['region', 'date', 'p1', 'p16']]  # temporary df with only columns we need
        table = pd.crosstab(index=[tmp_df.region, tmp_df.date], columns=[tmp_df.p16], colnames=['p16'])
        table = table.reset_index()
        table = table.rename(columns={1: "suchý neznečistený", 2: "suchý znečistený", 3: "mokrý", 4: "blato",
                                      5: "námraza, ujazdený sneh - posypané", 6: "námraza, ujazdený sneh - neposypané",
                                      7: "rozliaty olej, nafta apod.", 8: "súvislý sneh", 9: "náhla zmena stavu",
                                      0: "iný stav"})

        for reg in regions:
            reg_df = table[table['region'] == reg]  # df with only entries for current region
            reg_df = reg_df.resample("M", on="date").sum()  # resample to go by months

            ax = reg_df.plot(ax=axs[idx // 2, idx % 2])
            ax.set_title(reg)
            ax.set(xlabel="Dátum vzniku nehody", ylabel="Počet nehôd")

            idx += 1

            if (idx - 1) // 2 == 0:  # top graph shares x labels with bottom
                ax.set(xticklabels=[])
                ax.set(xlabel="")

            if idx < len(regions):  # not last graph - doesn't have legend
                handles, labels = ax.get_legend_handles_labels()
                ax.legend(handles[:0], labels[:0], frameon=False)
            else:  # last graph - has legend
                plt.legend(bbox_to_anchor=(1.05, 1), loc=2, borderaxespad=0., title="Stav vozovky",
                           frameon=False)  # legend on the right

        plt.tight_layout()

        if fig_location:
            plt.savefig(fig_location)  # Store the figure

        if show_figure:
            plt.show()  # Show the figure

    else:  # df is None
        raise ValueError("No DataFrame provided.")


if __name__ == "__main__":
    pass
