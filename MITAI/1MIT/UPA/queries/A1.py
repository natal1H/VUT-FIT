import pymongo
from argparse import ArgumentParser
import pandas as pd
import matplotlib.pyplot as plt
from bson.json_util import dumps
import re
import json
import seaborn as sns

"""UPA - 2nd part
    Theme: Covid-19
    
    Operations with query A1:
    Vytvořte čárový (spojnicový) graf zobrazující vývoj covidové situace po měsících pomocí následujících hodnot: 
    počet nově nakažených za měsíc, počet nově vyléčených za měsíc, počet nově hospitalizovaných osob za měsíc, 
    počet provedených testů za měsíc. Pokud nebude výsledný graf dobře čitelný, zvažte logaritmické měřítko, 
    nebo rozdělte hodnoty do více grafů. 
    
    Authors: 
        - Filip Bali (xbalif00)
        - Natália Holková (xholko02)
        - Roland Žitný (xzitny01)
"""

parser = ArgumentParser(prog='UPA-data_loader')
parser.add_argument('-m', '--mongo', help="Mongo db location",
                    default="mongodb://127.0.0.1:27017/?directConnection=true&serverSelectionTimeoutMS=2000")
parser.add_argument('-d', '--database', help="Database name", default="UPA-db")


def A1_extract_csv(db, csv_location="A1.csv"):
    # aggregation pipeline for infected and cured
    pipeline = [
        {"$group": {
            "_id": {
                "year": {"$year": {
                    "$dateFromString": {"dateString": "$date", "format": "%Y-%m-%d"}
                }},
                "month": {"$month": {
                    "$dateFromString": {"dateString": "$date", "format": "%Y-%m-%d"}
                }},
            },
            "count": {"$sum": 1}
        }},
        {'$project': {
            "month": {
                "$cond": {
                    "if": {
                        "$gte": ["$_id.month", 10]
                    },
                    "then": {
                        "$concat": [
                            {"$toString": "$_id.year"},
                            "-",
                            {"$toString": "$_id.month"}
                        ]
                    },
                    "else": {
                        "$concat": [
                            {"$toString": "$_id.year"},
                            "-0",
                            {"$toString": "$_id.month"}
                        ]
                    }
                }
            },
            "count": "$count"
        }}
    ]

    # aggregate infected - group by month and year and count total number in that month
    res_infected = db.infected.aggregate(pipeline)
    json_data = dumps(list(res_infected))
    json_data = re.sub('"_id": {"year": [\d]+, "month": [\d]+}, ', '', json_data)
    df_infected = pd.DataFrame.from_dict(json.loads(json_data))
    df_infected = df_infected.rename(columns={"count": "infected"})

    # aggregate cured - group by month and year and count total number in that month
    res_cured = db.cured.aggregate(pipeline)
    json_data = dumps(list(res_cured))
    json_data = re.sub('"_id": {"year": [\d]+, "month": [\d]+}, ', '', json_data)
    df_cured = pd.DataFrame.from_dict(json.loads(json_data))
    df_cured = df_cured.rename(columns={"count": "cured"})

    # get hospitalized by months - already by months
    res_hospitalized = db.hospitalized.find({})
    df_hospitalized = pd.DataFrame(list(res_hospitalized))  # transform to pandas DataFrame
    df_hospitalized.pop("_id")
    df_hospitalized = df_hospitalized.rename(columns={"patients": "hospitalized"})

    # get tests by months - already by months
    res_tests = db.tests.find({})
    df_tests = pd.DataFrame(list(res_tests))  # transform to pandas DataFrame
    df_tests.pop("_id")

    # merge dataframes
    merged = pd.merge(df_infected, df_cured, how='outer', on='month')
    merged = pd.merge(merged, df_hospitalized, how='outer', on='month')
    merged = pd.merge(merged, df_tests, how='outer', on='month')
    merged = merged.sort_values(by=['month'], ascending=True)
    merged = merged.set_index('month')
    merged.to_csv(csv_location, sep=';', encoding='utf-8')


def A1_plot_graph(csv_location="A1.csv", save_location=""):
    sns.set_style("darkgrid")

    df = pd.read_csv(csv_location, sep=";", encoding="utf-8")
    fig, ax = plt.subplots(figsize=(12, 8))
    pal = sns.color_palette("mako", 4)
    g = sns.lineplot(x='month', y='value', hue='variable', data=pd.melt(df, ['month']), ax=ax, palette=pal)
    g.set_yscale("log")
    g.set_xlabel("Mesiac", fontsize=20)
    g.set_ylabel("Počet", fontsize=20)
    plt.legend(title='', loc='upper left', labels=['Nakazení', 'Vyliečení', 'Hospitalizovaní', 'Testy'], fontsize=15)
    plt.title("Vývoj covidovej situácie v Česku po mesiacoch", fontsize=30)
    plt.xticks(rotation=45)

    if len(save_location) > 0:
        plt.savefig(save_location)
    plt.show()


if __name__ == "__main__":
    args = parser.parse_args()
    # MongoDB connection
    mongo_client = pymongo.MongoClient(args.mongo)
    mongo_db = mongo_client[args.database]

    A1_extract_csv(mongo_db, "A1.csv")
    A1_plot_graph("A1.csv", "A1.png")

    mongo_client.close()
