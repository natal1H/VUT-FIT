#!/usr/bin/python3.8
# coding=utf-8

from matplotlib import pyplot as plt
import pandas as pd
from reportlab.pdfgen import canvas
from reportlab.lib.units import cm
from reportlab.lib.pagesizes import A4
from reportlab.platypus import Paragraph, Frame, Image, Table, Spacer
from reportlab.lib.styles import getSampleStyleSheet
import os

"""IZV project - Final part
    Author: Natália Holková
    Login: xholko02
"""


def create_graph(c, df):
    """
    Creates a graph about about vehicle type influence on driver injuries.
    Graph is normalized so that y-axis shows percentage.
    This graph is then displayed in report on predefined position.

    :param c: report canvas
    :param df: DataFrame containing accidents
    :return: nothing
    """

    # Get new DataFrame with only columns I need
    df_new = df[["p1", "p44", "p57"]]

    # only keep rows, where p44 (type of vehicle) is car or motorcycle (== 1/2/3/4)
    df_new = df_new[df_new["p44"].isin([1, 2, 3, 4])]
    df_new = df_new[df_new["p57"].isin([1, 6, 8])]

    stacked = df_new.pivot_table(index=["p44"], columns='p57', aggfunc='size', fill_value=0).stack()

    # get total cars and bike accidents to later transform data to %
    total_cars = stacked[3, 1] + stacked[3, 6] + stacked[3, 8] + stacked[4, 1] + stacked[4, 6] + stacked[4, 8]
    total_bikes = stacked[1, 1] + stacked[1, 6] + stacked[1, 8] + stacked[2, 1] + stacked[2, 6] + stacked[2, 8]

    # status, cars, bikes
    data = [["Good", (stacked[3, 1] + stacked[4, 1]) / total_cars * 100,
             (stacked[1, 1] + stacked[2, 1]) / total_bikes * 100],
            ["Injured", (stacked[3, 6] + stacked[4, 6]) / total_cars * 100,
             (stacked[1, 6] + stacked[2, 6]) / total_bikes * 100],
            ["Dead", (stacked[3, 8] + stacked[4, 8]) / total_cars * 100,
             (stacked[1, 8] + stacked[2, 8]) / total_bikes * 100],
            ]

    plt.style.use("fivethirtyeight")
    df2 = pd.DataFrame(data, columns=["status", "cars", "bikes"])
    ax = df2.plot(x="status", y=["cars", "bikes"], kind="bar", figsize=(10, 10), rot=0)
    ax.set_yscale('log')  # logarithmic scale
    plt.xlabel("Driver status after accident", fontsize=16)
    plt.ylabel("% from total accidents", fontsize=16)
    ax.legend(["car", "motorcycle"])
    plt.suptitle("Graph of vehicle type influence on driver status", fontsize=20, fontweight='bold')

    plt.savefig("fig.png")  # Store the figure

    Image('fig.png', width=10 * cm, height=10 * cm).drawOn(c, 2 * cm, 7 * cm)


def create_table(c, df):
    """
    Creates a table showing total numbers of car and motorcycle accidents for each driver status.
    Then displays this table in report on predefined position.

    :param c: report canvas
    :param df: DataFrame containing accidents
    :return: nothing
    """

    # Get new DataFrame with only columns I need
    df_new = df[["p1", "p44", "p57"]]

    # only keep rows, where p44 (type of vehicle) is car or motorcycle (== 1/2/3/4)
    df_new = df_new[df_new["p44"].isin([1, 2, 3, 4])]
    df_new = df_new[df_new["p57"].isin([1, 6, 8])]

    stacked = df_new.pivot_table(index=["p44"], columns='p57', aggfunc='size', fill_value=0).stack()

    # create the raw table
    data = [[None, 'Good', 'Injured', 'Dead'],
            ['Cars', stacked[3, 1] + stacked[4, 1], stacked[3, 6] + stacked[4, 6], stacked[3, 8] + stacked[4, 8]],
            ['Motorcycles', stacked[1, 1] + stacked[2, 1], stacked[1, 6] + stacked[2, 6],
             stacked[1, 8] + stacked[2, 8]],
            ['Total', stacked[3, 1] + stacked[4, 1] + stacked[1, 1] + stacked[2, 1],
             stacked[3, 6] + stacked[4, 6] + stacked[1, 6] + stacked[2, 6],
             stacked[3, 8] + stacked[4, 8] + stacked[1, 8] + stacked[2, 8]]]

    # print table to output - elements separated by tab
    for row in data:
        for col in row:
            print(f"{col}\t", end="")
        print()

    # format the table
    t = Table(data, style=[('FONTNAME', (0, 0), (-1, -1), 'Helvetica'), ('FONTSIZE', (0, 0), (-1, -1), 12),
                           ('LEADING', (0, 0), (-1, -1), 12),
                           ('TEXTCOLOR', (0, 1), (0, -2), "#ffffff"),
                           ('BACKGROUND', (0, 1), (0, 1), '#008fd5'),  # cars heading
                           ('BACKGROUND', (0, 2), (0, 2), '#fc4f30'),  # motorcycles heading
                           ('BACKGROUND', (0, 3), (0, 3), '#f0f0f0'),
                           ('BACKGROUND', (1, 0), (-1, 0), '#f0f0f0'),  # First row headings
                           ('GRID', (0, 0), (-1, -1), 0.5, '#f0f0f0'),
                           ('VALIGN', (0, 1), (0, -1), 'MIDDLE')])
    t.hAlign = 'LEFT'
    t.vAlign = 'TOP'

    t.wrapOn(c, 16 * cm, 4 * cm)
    t.drawOn(c, 2 * cm, 3 * cm)


def get_additional_data(df):
    """
    Get additional data that will be used in report,
    namely total number accidents, number of car accidents and motorcycle accidents.

    :param df: DataFrame containing accidents
    :return: additional data in a dictionary
    """
    res = {"total_accidents": len(df)}

    # Get new DataFrame with only columns I need
    df_new = df[["p1", "p44", "p57"]]

    # only keep rows, where p44 (type of vehicle) is car or motorcycle (== 1/2/3/4)
    df_new = df_new[df_new["p44"].isin([1, 2, 3, 4])]
    df_new = df_new[df_new["p57"].isin([1, 6, 8])]

    stacked = df_new.pivot_table(index=["p44"], columns='p57', aggfunc='size', fill_value=0).stack()

    total_cars = stacked[3, 1] + stacked[3, 6] + stacked[3, 8] + stacked[4, 1] + stacked[4, 6] + stacked[4, 8]
    res["total_car_accidents"] = total_cars
    total_bikes = stacked[1, 1] + stacked[1, 6] + stacked[1, 8] + stacked[2, 1] + stacked[2, 6] + stacked[2, 8]
    res["total_bike_accidents"] = total_bikes

    # print dictionary to output
    print("total accidents:", res["total_accidents"])
    print("total car accidents with driver ok/injured/dead:", res["total_car_accidents"])
    print("total motorcycle accidents with driver ok/injured/dead:", res["total_bike_accidents"])

    return res


def create_report(df):
    """
    Create pdf report about vehicle type influence on driver injuries.

    :param df: DataFrame containing accidents
    :return: nothing
    """

    # prepare styles
    styles = getSampleStyleSheet()
    style_normal = styles['Normal']
    style_normal.fontName = 'Helvetica'
    style_normal.fontSize = 12
    style_normal.textColor = "#424949"
    style_normal.leading = 1.2 * style_normal.fontSize
    style_heading = styles['Heading1']
    style_heading.fontName = 'Helvetica'
    style_heading.fontSize = 16
    style_heading.spaceAfter = 20

    # create canvas
    c = canvas.Canvas("doc.pdf", pagesize=A4)

    c.setFillColor('white')
    c.saveState()

    # Heading rectangle
    c.setFillColor('#21618c')
    c.rect(2 * cm, 24 * cm, 17 * cm, 3 * cm, stroke=0, fill=1)

    # Heading text
    c.restoreState()
    c.setFillColor('white')
    c.setFont('Helvetica', 24)
    c.drawString(3 * cm, 25 * cm, 'Influence of vehicle type on driver status')

    # Text
    c.setFillColor('#f0f0f0')  # color that blends well with graph
    c.rect(2 * cm, 18 * cm, 17 * cm, 5 * cm, stroke=0, fill=1)
    additional_data = get_additional_data(df)
    story = [Paragraph(
        "It is said that <b>motorcycles are deadlier than cars</b>. " +
        "In this report, we show real accident data from the Czech republic " +
        "how the vehicle type influences the severity of injuries sustained by the driver in the accident. ",
        style_normal
    ),
        Spacer(0, 0.4 * cm),
        Paragraph(
        "In total, there were <b>" + str(additional_data["total_accidents"]) + "</b> accidents in our dataset.\n" +
        "However, we only worked with those where the vehicle our either car or motorcycle, " +
        "and the driver afterward was good, injured, or dead. This resulted in <b>" +
        str(additional_data["total_car_accidents"]) + "</b> car and <b>" + str(additional_data["total_bike_accidents"])
        + "</b> motorcycle accidents.",
        style_normal
    )]

    f = Frame(2 * cm, 18 * cm, 17 * cm, 5 * cm, showBoundary=0)
    f.addFromList(story, c)

    # Graph
    create_graph(c, df)
    c.setFillColor('#f0f0f0')
    c.rect(13 * cm, 7 * cm, 6 * cm, 10 * cm, stroke=0, fill=1)

    # graph text
    story = [Paragraph(
        "The graph shows <b>driver status</b> after an accident - either the driver was <b>OK</b>, <b>injured</b>, " +
        "or <b>dead</b>.\nThe data are normalized because the number of car accidents is much higher.\n" +
        "While in the vast majority of cases the driver is OK, " +
        "the graph shows that motorcycle accidents <b>result in death</b> slightly <b>more</b> often " +
        "than car accidents.",
        style_normal)]

    f = Frame(13 * cm, 7 * cm, 6 * cm, 10 * cm, showBoundary=0)
    f.addFromList(story, c)

    # Table
    create_table(c, df)

    c.showPage()
    c.save()  # save the report


if __name__ == "__main__":
    if os.path.isfile("accidents.pkl.gz"):  # data file exists
        dataframe = pd.read_pickle("accidents.pkl.gz")
    else:  # data file does not exist
        raise FileNotFoundError("File accidents.pkl.gz containing data not found.")

    # create the report doc.pdf
    create_report(dataframe)
