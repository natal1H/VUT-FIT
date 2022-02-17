import pandas as pd
import pymongo
from argparse import ArgumentParser
from bson.json_util import dumps
import json
from matplotlib import pyplot as plt
import seaborn as sns

"""UPA - 2nd part
    Theme: Covid-19

    Operations with query B1:
    Sestavte 4 žebříčky krajů "best in covid" za poslední 4 čtvrtletí (1 čtvrtletí = 1 žebříček).
    Jako kritérium volte počet nově nakažených přepočtený na jednoho obyvatele kraje.
    Pro jedno čtvrtletí zobrazte výsledky také graficky.
    Graf bude pro každý kraj zobrazovat
        - celkový počet nově nakažených           -----> (jeden sloupcový graf)
        - celkový počet obyvatel                  --^
        - počet nakažených na jednoho obyvatele.  -----> (jeden čárový graf)
    Graf můžete zhotovit kombinací dvou grafů do jednoho
        (jeden sloupcový graf zobrazí první dvě hodnoty a druhý, čárový graf, hodnotu třetí).

    Authors: 
        - Filip Bali (xbalif00)
        - Natália Holková (xholko02)
        - Roland Žitný (xzitny01)
"""

parser = ArgumentParser(prog='UPA-data_loader')
parser.add_argument('-m', '--mongo', help="Mongo db location",
                    default="mongodb://127.0.0.1:27017/?directConnection=true&serverSelectionTimeoutMS=2000")
parser.add_argument('-d', '--database', help="Database name", default="UPA-db")


def B1_extract_csv(db):
    pipeline = [
        {"$match":
             {'date':
                  { "$regex": "^2021"}}
         },
        {"$project": {
            "date": 1,
            "region": 1,
            "quarter": {
                "$switch": {
                    "branches": [
                        {"case": {"$lte": [{"$month": {"$toDate": "$date"}}, 3]}, "then": 1},
                        {"case": {"$lte": [{"$month": {"$toDate": "$date"}}, 6]}, "then": 2},
                        {"case": {"$lte": [{"$month": {"$toDate": "$date"}}, 9]}, "then": 3},
                        {"case": {"$lte": [{"$month": {"$toDate": "$date"}}, 12]}, "then": 4}]
                }
            }
        }},
        {"$group":
            {
                "_id": {"region": "$region", "quarter": "$quarter"},
                "count": {"$sum": 1},
            }
        },
        {"$sort": {"_id.region": 1, "_id.quarter": 1}}
    ]

    Q_infected_2021 = db.infected.aggregate(pipeline)
    json_Q_infected_2021 = dumps(list(Q_infected_2021))
    json_Q_infected_2021 = json.loads(json_Q_infected_2021)

    # pd_data_2020 = pd.DataFrame.from_dict(data_2020)
    df_Q_infected_2021 = pd.json_normalize(json_Q_infected_2021)

    # Delete rows when _id.region value is NaN
    df_Q_infected_2021 = df_Q_infected_2021[df_Q_infected_2021['_id.region'].notna()]

    # Jako kritérium volte počet nově nakažených přepočtený na jednoho obyvatele kraje.
    pipeline = [
            {"$project": {
                "value": 1,
                "territory_code": 1,
                "territory_txt": 1,
                "valid_date": 1,
                "gender_code": 1,
                "gender_txt": 1,
                "age_code": 1,
                "age_txt": 1,
                }
            }
    ]

    demo = db.demographic_data.aggregate(pipeline)
    demo = dumps(list(demo))
    demo = json.loads(demo)

    df_demo = pd.json_normalize(demo)
    df_demo = df_demo.drop(columns='_id.$oid')

    # Removes duplicate values in the dataset
    # Selects only sums of regions/districts
    # In the dataset, null means sum
    df_demo = df_demo[df_demo['gender_code'].isnull() &
                      df_demo['age_code'].isnull()]

    df_demo = df_demo[(df_demo['valid_date'] == '2020-12-31') &
                      ((df_demo['territory_code'] == '3018') |
                      (df_demo['territory_code'] == '3026') |
                      (df_demo['territory_code'] == '3034') |
                      (df_demo['territory_code'] == '3042') |
                      (df_demo['territory_code'] == '3051') |
                      (df_demo['territory_code'] == '3069') |
                      (df_demo['territory_code'] == '3077') |
                      (df_demo['territory_code'] == '3085') |
                      (df_demo['territory_code'] == '3093') |
                      (df_demo['territory_code'] == '3107') |
                      (df_demo['territory_code'] == '3115') |
                      (df_demo['territory_code'] == '3123') |
                      (df_demo['territory_code'] == '3131') |
                      (df_demo['territory_code'] == '3140'))
                      ]

    df_demo['value'] = df_demo['value'].astype(int)

    # df_demo_sum = df_demo.groupby('territory_code')['value'].sum()
    # df_demo_sum = df_demo_sum.reset_index()
    df_demo_sum = df_demo.rename(columns={"territory_code": "value", "value": "population"})
    df_demo_sum['value'] = df_demo_sum['value'].astype(int)


    # Divide by quarter
    df_Q1_infected_2021 = df_Q_infected_2021[df_Q_infected_2021['_id.quarter'] == 1]
    df_Q1_infected_2021= df_Q1_infected_2021.rename(columns={"count": "infected", "_id.region": "cznuts", "_id.quarter": "quarter"})

    df_Q2_infected_2021 = df_Q_infected_2021[df_Q_infected_2021['_id.quarter'] == 2]
    df_Q2_infected_2021 = df_Q2_infected_2021.rename(columns={"count": "infected", "_id.region": "cznuts", "_id.quarter": "quarter"})

    df_Q3_infected_2021 = df_Q_infected_2021[df_Q_infected_2021['_id.quarter'] == 3]
    df_Q3_infected_2021 = df_Q3_infected_2021.rename(columns={"count": "infected", "_id.region": "cznuts", "_id.quarter": "quarter"})

    df_Q4_infected_2021 = df_Q_infected_2021[df_Q_infected_2021['_id.quarter'] == 4]
    df_Q4_infected_2021 = df_Q4_infected_2021.rename(columns={"count": "infected", "_id.region": "cznuts", "_id.quarter": "quarter"})

    pipeline = [
            {"$project": {
                "cznuts": 1,
                "region_shortcut": 1,
                "value": 1,
                }
            }
    ]
    region_enum = db.region_enumerator.aggregate(pipeline)
    region_enum = dumps(list(region_enum))
    region_enum = json.loads(region_enum)

    df_region_enum = pd.json_normalize(region_enum)
    df_region_enum = df_region_enum.drop(columns='_id.$oid')

    df_Q1_infected_2021 = pd.merge(df_Q1_infected_2021, df_region_enum, on='cznuts')
    df_Q2_infected_2021 = pd.merge(df_Q2_infected_2021, df_region_enum, on='cznuts')
    df_Q3_infected_2021 = pd.merge(df_Q3_infected_2021, df_region_enum, on='cznuts')
    df_Q4_infected_2021 = pd.merge(df_Q4_infected_2021, df_region_enum, on='cznuts')

    df_Q1_infected_2021['value'] = df_Q1_infected_2021['value'].astype(int)
    df_Q2_infected_2021['value'] = df_Q2_infected_2021['value'].astype(int)
    df_Q3_infected_2021['value'] = df_Q3_infected_2021['value'].astype(int)
    df_Q4_infected_2021['value'] = df_Q4_infected_2021['value'].astype(int)

    df_Q1_infected_2021 = pd.merge(df_Q1_infected_2021, df_demo_sum, on='value')
    df_Q2_infected_2021 = pd.merge(df_Q2_infected_2021, df_demo_sum, on='value')
    df_Q3_infected_2021 = pd.merge(df_Q3_infected_2021, df_demo_sum, on='value')
    df_Q4_infected_2021 = pd.merge(df_Q4_infected_2021, df_demo_sum, on='value')

    df_Q1_infected_2021['infected_normalized'] = df_Q1_infected_2021['infected'] / df_Q1_infected_2021['population']
    df_Q2_infected_2021['infected_normalized'] = df_Q2_infected_2021['infected'] / df_Q2_infected_2021['population']
    df_Q3_infected_2021['infected_normalized'] = df_Q3_infected_2021['infected'] / df_Q3_infected_2021['population']
    df_Q4_infected_2021['infected_normalized'] = df_Q4_infected_2021['infected'] / df_Q4_infected_2021['population']

    # We don't need all the columns, keep only necessary ones
    df_Q1_infected_2021 = df_Q1_infected_2021[["region_shortcut", "quarter", "population", "infected", "infected_normalized"]]
    df_Q2_infected_2021 = df_Q2_infected_2021[["region_shortcut", "quarter", "population", "infected", "infected_normalized"]]
    df_Q3_infected_2021 = df_Q3_infected_2021[["region_shortcut", "quarter", "population", "infected", "infected_normalized"]]
    df_Q4_infected_2021 = df_Q4_infected_2021[["region_shortcut", "quarter", "population", "infected", "infected_normalized"]]

    df_Q1_infected_2021 = df_Q1_infected_2021.set_index('region_shortcut')
    df_Q2_infected_2021 = df_Q2_infected_2021.set_index('region_shortcut')
    df_Q3_infected_2021 = df_Q3_infected_2021.set_index('region_shortcut')
    df_Q4_infected_2021 = df_Q4_infected_2021.set_index('region_shortcut')

    # Sort each dataframe by normalized infected
    df_Q1_infected_2021 = df_Q1_infected_2021.sort_values('infected_normalized', ascending=False)
    df_Q2_infected_2021 = df_Q2_infected_2021.sort_values('infected_normalized', ascending=False)
    df_Q3_infected_2021 = df_Q3_infected_2021.sort_values('infected_normalized', ascending=False)
    df_Q4_infected_2021 = df_Q4_infected_2021.sort_values('infected_normalized', ascending=False)

    df_Q1_infected_2021.to_csv('B1_Q1.csv', sep=';', encoding='utf-8')
    df_Q2_infected_2021.to_csv('B1_Q2.csv', sep=';', encoding='utf-8')
    df_Q3_infected_2021.to_csv('B1_Q3.csv', sep=';', encoding='utf-8')
    df_Q4_infected_2021.to_csv('B1_Q4.csv', sep=';', encoding='utf-8')


def B1_plot_graph(save_location="B1.png", csv_loc="B1_Q4.csv", quarter="Q4"):
    sns.set_style("darkgrid")

    # Load csv first, rename columns for later mergin, drop unnecessary columns
    df_q = pd.read_csv(csv_loc, sep=";", encoding="utf-8")
    df_q = df_q.drop(['quarter'], axis=1)

    fig, axes = plt.subplots(2, 1, figsize=(18, 18))
    fig.suptitle("\"Best in covid\" za štvrťrok {} 2021".format(quarter), fontsize=40)
    # 1st graph - select necessary columns from merged dataframe (region population and newly infected
    df_graph1 = df_q[["region_shortcut", "population", "infected"]]

    # Graph designing
    order = df_q.sort_values('infected_normalized', ascending=False).region_shortcut
    g = sns.barplot(x='region_shortcut', y='value', hue='variable', data=pd.melt(df_graph1, ['region_shortcut']),
                    ax=axes[0], order=order)
    g.set_xlabel("Kraj", fontsize=20)
    g.set_ylabel("Počet", fontsize=20)
    g.set_title("Vývoj počtu novo nakazených vzhľadom na populáciu kraja", fontsize=30)
    g.set_yscale("log")
    labels = ["Populácia kraja", "Nakazení {}".format(quarter)]
    h, l = axes[0].get_legend_handles_labels()
    axes[0].legend(h, labels, loc='upper left', title="", fontsize=15)
    plt.setp(axes[0].xaxis.get_majorticklabels(), rotation=45)

    # 2nd graph - select necessary columns from merged dataframe (infected per 1 inhabitant)
    df_graph2 = df_q[["region_shortcut", "infected_normalized"]]

    # Graph designing
    g = sns.lineplot(x='region_shortcut', y='infected_normalized',
                     data=df_graph2.sort_values('infected_normalized', ascending=False), ax=axes[1])
    g.set_xlabel("Kraj", fontsize=20)
    g.set_ylabel("Počet nakazených na 1 obyvateľa", fontsize=20)
    g.set_title("Vývoj počtu nakazených na 1 obyvateľa v krajoch", fontsize=30)
    plt.setp(axes[1].xaxis.get_majorticklabels(), rotation=45)

    if len(save_location) > 0:
        plt.savefig(save_location)
    plt.show()


if __name__ == "__main__":
    args = parser.parse_args()
    # MongoDB connection
    mongo_client = pymongo.MongoClient(args.mongo)
    mongo_db = mongo_client[args.database]

    B1_extract_csv(mongo_db)
    B1_plot_graph("B1.png")

    mongo_client.close()
