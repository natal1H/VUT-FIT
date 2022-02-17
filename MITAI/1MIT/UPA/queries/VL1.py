import pymongo
import pandas as pd
import matplotlib.pyplot as plt
from bson.json_util import dumps
import json
import seaborn as sns
from argparse import ArgumentParser

"""UPA - 2nd part
    Theme: Covid-19

    Operations with query VL1:
    --
    
    Authors: 
        - Filip Bali (xbalif00)
        - Natália Holková (xholko02)
        - Roland Žitný (xzitny01)
"""

parser = ArgumentParser(prog='UPA-data_loader')
parser.add_argument('-m', '--mongo', help="Mongo db location",
                    default="mongodb://127.0.0.1:27017/?directConnection=true&serverSelectionTimeoutMS=2000")
parser.add_argument('-d', '--database', help="Database name", default="UPA-db")


def VL1_extract_csv(db, csv_location="VL1.csv"):
    """
    Smrť po krajoch
    """

    pipeline = [
        {"$match":
             {'date':
                  {"$regex": "^202"},
             }
        },
        {"$project": {
            "date": 1,
            "region": 1,
            }
        },
        {"$group":
            {
                "_id": {"cznuts": "$region"},
                "dead_count": {"$sum": 1},
            }
        },

    ]

    df_dead = db.dead.aggregate(pipeline)
    df_dead = dumps(list(df_dead))
    df_dead = json.loads(df_dead)

    df_dead = pd.json_normalize(df_dead).reset_index(drop=True)
    df_dead = df_dead.rename(
        columns={"_id.cznuts": "cznuts",
                })
    df_dead = df_dead[df_dead['cznuts'].notna()]

    pipeline = [
        {"$project": {
            "region_shortcut": 1,
            "cznuts": 1,
            }
        }
    ]

    region_enum = db.region_enumerator.aggregate(pipeline)
    region_enum = dumps(list(region_enum))
    region_enum = json.loads(region_enum)

    region_enum = pd.json_normalize(region_enum)
    region_enum = region_enum.drop(columns='_id.$oid')
    region_enum = region_enum[region_enum['cznuts'] !='CZZZZ']

    df_dead = pd.merge(df_dead, region_enum, on='cznuts')
    df_dead = df_dead.drop(columns='cznuts')

    df_dead = df_dead.set_index('region_shortcut')
    df_dead.to_csv(csv_location, sep=';', encoding='utf-8')


def VL1_plot_graph(csv_location="VL1.csv", save_location="VL1.png"):
    sns.set_style("darkgrid")

    df = pd.read_csv(csv_location, sep=";", encoding="utf-8")
    fig, ax = plt.subplots(figsize=(12, 12))

    # To make color gradient by order
    pal = sns.color_palette("ch:start=.2,rot=-.3_r", len(df))

    # Graph designing
    g = sns.barplot(ax=ax, x='region_shortcut', y='dead_count', data=df,
                    order=df.sort_values('dead_count', ascending=False).region_shortcut,
                    palette=pal)
    g.set_xlabel("Kraj", fontsize=25)
    g.set_ylabel("Počet úmrtí", fontsize=25)
    plt.title("Počet úmrtí v krajoch počas pandémie Covid-19", fontsize=35)
    plt.xticks(rotation=45)

    if len(save_location) > 0:
        plt.savefig(save_location)
    plt.show()


if __name__ == "__main__":
    args = parser.parse_args()
    # MongoDB connection
    mongo_client = pymongo.MongoClient(args.mongo)
    mongo_db = mongo_client[args.database]

    VL1_extract_csv(mongo_db, "VL1.csv")
    VL1_plot_graph("VL1.csv", "VL1.png")

    mongo_client.close()
