import pandas as pd
import pymongo
from bson.json_util import dumps
import json
import seaborn as sns
import matplotlib.pyplot as plt
from argparse import ArgumentParser

"""UPA - 2nd part
    Theme: Covid-19

    Operations with query A3:
    Vytvořte sérii sloupcových grafů, které zobrazí:
        1. graf: počty provedených očkování v jednotlivých krajích (celkový počet od začátku očkování).
        2. graf: počty provedených očkování jako v předchozím bodě navíc rozdělené podle pohlaví. 
                 Diagram může mít např. dvě části pro jednotlivá pohlaví.
        3. graf: Počty provedených očkování, ještě dále rozdělené dle věkové skupiny. 
                 Pro potřeby tohoto diagramu postačí 3 věkové skupiny (0-24 let, 25-59, nad 59).

    Authors: 
        - Filip Bali (xbalif00)
        - Natália Holková (xholko02)
        - Roland Žitný (xzitny01)
"""

parser = ArgumentParser(prog='UPA-data_loader')
parser.add_argument('-m', '--mongo', help="Mongo db location",
                    default="mongodb://127.0.0.1:27017/?directConnection=true&serverSelectionTimeoutMS=2000")
parser.add_argument('-d', '--database', help="Database name", default="UPA-db")


def A3_extract_csv(db, csv_location="A3.csv"):
    pipeline = [
        {"$match":
             {'pohlavi':
                  {"$in": ['Z', 'M']},
              }
         },

        {"$project": {
            "kraj_nuts_kod": 1,
            "pohlavi": 1,
            "vekova_skupina": 1,
        }},
        {"$group":
            {
                "_id": {'kraj_nuts_kod': '$kraj_nuts_kod', 'pohlavi': '$pohlavi', 'vekova_skupina': '$vekova_skupina'},
                "count": {"$sum": 1},
            }
        },
        {"$sort": {"kraj_nuts_kod": 1}}
    ]

    res_vaccinated = db.vaccinated_by_profession.aggregate(pipeline)
    res_vaccinated = dumps(list(res_vaccinated))
    res_vaccinated = json.loads(res_vaccinated)

    df_vaccinated = pd.json_normalize(res_vaccinated)
    df_vaccinated = df_vaccinated.rename(columns={"_id.kraj_nuts_kod": "cznuts"})

    df_vaccinated = df_vaccinated[df_vaccinated['cznuts'].notna()]

    # Get regions from mongodb
    pipeline = [
        {"$project": {
            "cznuts": 1,
            "region_shortcut": 1
        }}
    ]

    res_regions = db.region_enumerator.aggregate(pipeline)
    res_regions = dumps(list(res_regions))
    res_regions = json.loads(res_regions)
    df_regions = pd.json_normalize(res_regions)

    # Rename region codes in vaccinated dataframe to actual text names
    for region_code in df_regions['cznuts'].unique():
        shortcut = df_regions[df_regions["cznuts"] == region_code]["region_shortcut"].values[0]
        df_vaccinated['cznuts'] = [shortcut if code == region_code else code for code in df_vaccinated['cznuts']]

    # Rename columns in vaccinated dataframe
    df_vaccinated = df_vaccinated.rename(columns={"cznuts": "region", "_id.pohlavi": "gender",
                                                  "_id.vekova_skupina": "age_group"})
    df_vaccinated = df_vaccinated.set_index('region') # Ok?
    df_vaccinated.to_csv(csv_location, sep=';', encoding='utf-8')


def A3_plot_graph(csv_location="A3.csv", save_location="A3.png"):
    sns.set_style("darkgrid")

    # Load extracted data
    df = pd.read_csv(csv_location, sep=";", encoding="utf-8")

    fig, axes = plt.subplots(1, 3, figsize=(18, 8))
    fig.suptitle('Očkovanie v krajoch', fontsize=30)
    # Graph 1 - Total number of vaccinations in each region
    df_graph1 = df.groupby(['region'])['count'].sum().reset_index()
    # To make color gradient by order
    pal1 = sns.color_palette("ch:start=.2,rot=-.3_r", len(df_graph1))
    order = df_graph1.sort_values('count', ascending=False).region
    g = sns.barplot(ax=axes[0], x='region', y='count', data=df_graph1,
                    order=order,
                    palette=pal1)
    g.set_xlabel("Kraj", fontsize=20)
    g.set_ylabel("Počet očkovaných", fontsize=20)
    plt.setp(axes[0].xaxis.get_majorticklabels(), rotation=45)

    # Graph 2 - Total number of vaccinations, number of vaccinations by gender in each region
    df_gender = df.groupby(['region', 'gender'])['count'].sum().reset_index()  # Count vaccinations by gender
    # Prepare df used in plotting, will have total, male, and female vaccination counts
    df_graph2 = pd.DataFrame({'region': pd.Series(dtype='str'), 'total': pd.Series(dtype='int'),
                              'male': pd.Series(dtype='int'), 'female': pd.Series(dtype='int')})
    for region_name in df_gender['region'].unique():
        total = df_graph1[df_graph1['region'] == region_name]['count'].values[0]  # Total number of vaccinations
        male = df_gender[(df_gender['region'] == region_name) & (df_gender['gender'] == 'M')]['count'].values[0]
        female = df_gender[(df_gender['region'] == region_name) & (df_gender['gender'] == 'Z')]['count'].values[0]
        df_graph2 = df_graph2.append({"region": region_name, "total": total, "male": male,
                                      "female": female}, ignore_index=True)

    labels = ["Celkovo", "Muži", "Ženy"]
    pal2 = sns.color_palette("ch:start=.2,rot=-.3_r", 3)
    g = sns.barplot(x='region', y='value', hue='variable', data=pd.melt(df_graph2, ['region']), ax=axes[1],
                    order=order, palette=pal2)
    g.set_xlabel("Kraj", fontsize=20)
    g.set_ylabel("")
    h, l = axes[1].get_legend_handles_labels()
    axes[1].legend(h, labels, loc='upper left', title="", fontsize=15)
    plt.setp(axes[1].xaxis.get_majorticklabels(), rotation=45)

    # Graph 3 - Total number of vaccinations, number of vaccinations by age group in each region
    df_age = df.groupby(['region', 'age_group'])['count'].sum().reset_index()
    # Need to again count those in the same category (0-24, 25-59, 59+)
    group1 = ['0-11', '12-15', '16-17', '18-24']  # Kids & young
    group2 = ['25-29', '30-34', '35-39', '40-44', '45-49', '50-54', '55-59']  # Adults
    group3 = ['60-64', '65-69', '70-79', '80+']  # Elderly
    # Prepare dataset used in plotting
    df_graph3 = pd.DataFrame({'region': pd.Series(dtype='str'), 'total': pd.Series(dtype='int'),
                              'group1': pd.Series(dtype='int'), 'group2': pd.Series(dtype='int'),
                              'group3': pd.Series(dtype='int')})
    for region_name in df_age['region'].unique():
        total = df_graph1[df_graph1['region'] == region_name]['count'].values[0]  # Total number of vaccinations
        num_group1 = df_age[(df_age['region'] == region_name) & (df_age['age_group'].isin(group1))]["count"].sum()
        num_group2 = df_age[(df_age['region'] == region_name) & (df_age['age_group'].isin(group2))]["count"].sum()
        num_group3 = df_age[(df_age['region'] == region_name) & (df_age['age_group'].isin(group3))]["count"].sum()

        df_graph3 = df_graph3.append({"region": region_name, "total": total, "group1": num_group1, "group2": num_group2,
                                      "group3": num_group3}, ignore_index=True)

    labels = ["Celkovo", "Deti a mladí", "Dospelí", "Starší"]
    pal3 = sns.color_palette("ch:start=.2,rot=-.3_r", 4)
    g = sns.barplot(x='region', y='value', hue='variable', data=pd.melt(df_graph3, ['region']), ax=axes[2],
                    order=order, palette=pal3)
    g.set_xlabel("Kraj", fontsize=20)
    g.set_ylabel("")
    h, l = axes[2].get_legend_handles_labels()
    axes[2].legend(h, labels, loc='upper left', title="", fontsize=15)
    plt.setp(axes[2].xaxis.get_majorticklabels(), rotation=45)

    fig.tight_layout()
    if len(save_location) > 0:
        plt.savefig(save_location)
    plt.show()


if __name__ == "__main__":
    args = parser.parse_args()
    # MongoDB connection
    mongo_client = pymongo.MongoClient(args.mongo)
    mongo_db = mongo_client[args.database]

    A3_extract_csv(mongo_db, "A3.csv")
    A3_plot_graph("A3.csv", "A3.png")

    mongo_client.close()

