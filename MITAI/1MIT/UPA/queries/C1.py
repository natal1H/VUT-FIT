import pandas as pd
import pymongo
from bson.json_util import dumps
import json
import numpy as np
from argparse import ArgumentParser

"""UPA - 2nd part
    Theme: Covid-19

    Authors: 
        - Filip Bali (xbalif00)
        - Natália Holková (xholko02)
        - Roland Žitný (xzitny01)
"""

parser = ArgumentParser(prog='UPA-data_loader')
parser.add_argument('-m', '--mongo', help="Mongo db location",
                    default="mongodb://127.0.0.1:27017/?directConnection=true&serverSelectionTimeoutMS=2000")
parser.add_argument('-d', '--database', help="Database name", default="UPA-db")


def c1(db, csv_before_location="C1_before.csv", csv_after_location="C1_after.csv"):
    """
    Hledání skupin podobných měst z hlediska vývoje covidu a věkového složení obyvatel.

    Atributy: počet nakažených za poslední 4 čtvrtletí,
              počet očkovaných za poslední 4 čtvrtletí,
              počet obyvatel ve věkové skupině 0..14 let,
              počet obyvatel ve věkové skupině 15 - 59,
              počet obyvatel nad 59 let.
    Pro potřeby projektu vyberte libovolně 50 měst,
        pro které najdete potřebné hodnoty (můžete např. využít nějaký žebříček 50 nejlidnatějších měst v ČR).

    """

    # Count infected by district
    pipeline = [
        {"$match":
             {'date':
                  {"$regex": "^2021"}}
         },
        {"$project": {
            "date": 1,
            "district": 1,
        }},
        {"$group":
            {
                "_id": {"district": "$district"},
                "count": {"$sum": 1},
            }
        },
    ]

    cursor = db.infected.aggregate(pipeline)
    data = dumps(list(cursor))
    df_infected_district = json.loads(data)

    df_infected_district = pd.json_normalize(df_infected_district)
    df_infected_district = df_infected_district[df_infected_district['_id.district'].notna()]

    ######################################################################################################

    pipeline = [
        {"$project": {
            "text": 1,
            "cznuts": 1,
            "okres_lau": 1,
            "region_shortcut": 1,
        }
        }
    ]

    district_enum = db.district_enumerator.aggregate(pipeline)
    district_enum = dumps(list(district_enum))
    district_enum = json.loads(district_enum)

    district_enum = pd.json_normalize(district_enum)
    district_enum = district_enum.drop(columns='_id.$oid')
    district_enum = district_enum[district_enum['cznuts'] !='CZZZZZ']

    district_enum = district_enum.rename(
        columns={"text": "district_name",
                })
    df_infected_district = df_infected_district.rename(
        columns={"count": "infected_count",
                 "_id.district": "okres_lau",
                })
    df_infected_merged = pd.merge(district_enum, df_infected_district, on='okres_lau')

    # TODO nastavit vsade dobre kvartaly
    pipeline = [
        {"$match":
             {'date':
                  {"$regex": "^2021"},
              '$or':[
                      {'shot_order': 2, 'vaccine_code': 'CO01'},
                      {'shot_order': 2, 'vaccine_code': 'CO02'},
                      {'shot_order': 2, 'vaccine_code': 'CO03'},
                      {'shot_order': 1, 'vaccine_code': 'CO04'}
                      ],
              }
         },
        {"$project": {
            "date": 1,
            "district_name": 1,
            "orp": 1,
            "shot_count": 1,
            }
        },
        {"$group":
            {
                "_id": {"district_name": "$district_name"},
                'vaccinated_count': {
                        '$sum': '$shot_count'
                }
            }
        },

    ]

    df_vaccinated_geography = db.vaccinated_geography.aggregate(pipeline)
    df_vaccinated_geography = dumps(list(df_vaccinated_geography))
    df_vaccinated_geography = json.loads(df_vaccinated_geography)

    df_vaccinated_geography = pd.json_normalize(df_vaccinated_geography).reset_index(drop=True)

    df_vaccinated_geography = df_vaccinated_geography.rename(
        columns={"_id.district_name": "district_name",
                })

    df_merged = pd.merge(df_infected_merged, df_vaccinated_geography, on='district_name')

    pipeline = [
        {"$match":
             {
                 # TODO teraz ich je 51
                 # 'territory_txt': {'$in': ['Benešov', 'Beroun', 'Kladno', 'Kolín', 'Mělník', 'Nymburk', 'Příbram', 'Rakovník', 'Písek', 'Prachatice', 'Strakonice', 'Tábor', 'Domažlice', 'Klatovy', 'Rokycany', 'Tachov', 'Cheb', 'Sokolov', 'Děčín', 'Chomutov', 'Litoměřice', 'Louny', 'Most', 'Teplice', 'Liberec', 'Semily', 'Jičín', 'Náchod', 'Trutnov', 'Chrudim', 'Pardubice', 'Svitavy', 'Jihlava', 'Pelhřimov', 'Třebíč', 'Blansko', 'Břeclav', 'Hodonín', 'Vyškov', 'Znojmo', 'Jeseník', 'Olomouc', 'Prostějov', 'Přerov', 'Šumperk', 'Kroměříž', 'Vsetín', 'Zlín', 'Bruntál', 'Frýdek-Místek', 'Karviná', 'Opava']},
                 'territory_txt': {'$in': ['Kladno', 'Kolín', 'Mělník', 'Nymburk', 'Příbram', 'Rakovník', 'Písek', 'Prachatice', 'Strakonice', 'Tábor', 'Domažlice', 'Klatovy', 'Rokycany', 'Tachov', 'Cheb', 'Sokolov', 'Děčín', 'Chomutov', 'Litoměřice', 'Louny', 'Most', 'Teplice', 'Liberec', 'Semily', 'Jičín', 'Náchod', 'Trutnov', 'Chrudim', 'Pardubice', 'Svitavy', 'Jihlava', 'Pelhřimov', 'Třebíč', 'Blansko', 'Břeclav', 'Hodonín', 'Vyškov', 'Znojmo', 'Jeseník', 'Olomouc', 'Prostějov', 'Přerov', 'Šumperk', 'Kroměříž', 'Vsetín', 'Zlín', 'Bruntál', 'Frýdek-Místek', 'Karviná', 'Opava']},
                 'valid_date': '2020-12-31'
             }
         },
        {"$project": {
            "value": 1,
            "territory_txt": 1,
            "territory_code": 1,
            "age_txt": 1,
            "age_code": 1,
            "gender_txt": 1,
            "gender_code": 1,
        }
        },
    ]

    df_demo = db.demographic_data.aggregate(pipeline)

    df_demo = dumps(list(df_demo))
    df_demo = json.loads(df_demo)

    df_demo = pd.json_normalize(df_demo).reset_index(drop=True)
    df_demo = df_demo.drop(columns='_id.$oid')

    df_demo_0_14 = df_demo[(df_demo['age_code'] == '400000600005000') |   # 0-5
                           (df_demo['age_code'] == '400005610010000') |  # 5-10
                           (df_demo['age_code'] == '410010610015000')    # 10-15
    ]

    df_demo_15_59 = df_demo[(df_demo['age_code'] == '410015610020000') |  # 15-20
                            (df_demo['age_code'] == '410020610025000') |  # 20-25
                            (df_demo['age_code'] == '410025610030000') |  # 25-30
                            (df_demo['age_code'] == '410030610035000') |  # 30-35
                            (df_demo['age_code'] == '410035610040000') |  # 35-40
                            (df_demo['age_code'] == '410040610045000') |  # 40-45
                            (df_demo['age_code'] == '410045610050000') |  # 45-50
                            (df_demo['age_code'] == '410050610055000') |  # 50-55
                            (df_demo['age_code'] == '410055610060000')  # 55-60
    ]

    df_demo_59_above = df_demo[(df_demo['age_code'] == '410060610065000') |  # 60-65
                               (df_demo['age_code'] == '410065610070000') |  # 65-70
                               (df_demo['age_code'] == '410070610075000') |  # 70-75
                               (df_demo['age_code'] == '410075610080000') |  # 75-80
                               (df_demo['age_code'] == '410080610085000') |  # 80-85
                               (df_demo['age_code'] == '410085610090000') |  # 85-90
                               (df_demo['age_code'] == '410090610095000') |  # 90-95
                               (df_demo['age_code'] == '410095799999000')  # 95-above
    ]

    df_demo_0_14 = df_demo_0_14[df_demo_0_14['gender_code'].isnull()]
    df_demo_15_59 = df_demo_15_59[df_demo_15_59['gender_code'].isnull()]
    df_demo_59_above = df_demo_59_above[df_demo_59_above['gender_code'].isnull()]

    df_demo_0_14 = df_demo_0_14.drop(columns=['gender_code',
                                      'age_code',
                                      'territory_code',
                                      'age_txt',
                                      'gender_txt'
                                      ])

    df_demo_15_59 = df_demo_15_59.drop(columns=['gender_code',
                                        'age_code',
                                        'territory_code',
                                        'age_txt',
                                        'gender_txt'
                                        ])

    df_demo_59_above = df_demo_59_above.drop(columns=['gender_code',
                                              'age_code',
                                              'territory_code',
                                              'age_txt',
                                              'gender_txt'
                                              ])

    df_demo_0_14 = df_demo_0_14.astype({"value": int})
    df_demo_15_59 = df_demo_15_59.astype({"value": int})
    df_demo_59_above = df_demo_59_above.astype({"value": int})


    df_demo_0_14 = df_demo_0_14.groupby(['territory_txt']).sum().reset_index()
    df_demo_15_59 = df_demo_15_59.groupby(['territory_txt']).sum().reset_index()
    df_demo_59_above = df_demo_59_above.groupby(['territory_txt']).sum().reset_index()

    df_demo_0_14 = df_demo_0_14.rename(
        columns={"value": "population_count(age_0-14)",
                 'territory_txt': 'district_name'
                })

    df_demo_15_59 = df_demo_15_59.rename(
        columns={"value": "population_count(age_15-59)",
                 'territory_txt': 'district_name'
                })

    df_demo_59_above = df_demo_59_above.rename(
        columns={"value": "population_count(age_59-above)",
                 'territory_txt': 'district_name'
                })

    df_merged = pd.merge(df_merged, df_demo_0_14, on='district_name')
    df_merged = pd.merge(df_merged, df_demo_15_59, on='district_name')
    df_merged = pd.merge(df_merged, df_demo_59_above, on='district_name')

    df_merged = df_merged.drop(columns=['cznuts',
                                  'okres_lau',
                                  'region_shortcut',
                              ])

    df_merged = df_merged.astype({"vaccinated_count": int})
    df_merged = df_merged.set_index("district_name")  # Set index to town name (unique value)
    # Write to file df before applying normalization, discretization and dealing with outliers
    df_merged.to_csv(csv_before_location, sep=';', encoding='utf-8')

    # Prepare csv after aplying normalization, discretization and dealing with outliers
    df_merged['population_count(age_0-14)'] = df_merged['population_count(age_0-14)'] / \
                                              (df_merged['population_count(age_0-14)'] +
                                               df_merged['population_count(age_15-59)'] +
                                               df_merged['population_count(age_59-above)'])  # Normalization
    # Replace outliers from vaccinate count and replace with median
    median_vaccinated = df_merged["vaccinated_count"].median()
    std_vaccinated = df_merged["vaccinated_count"].std()
    df_merged.loc[(df_merged.vaccinated_count - median_vaccinated).abs() > std_vaccinated, 'vaccinated_count'] = np.nan
    df_merged.fillna(median_vaccinated, inplace=True)

    # Write new df to csv
    df_merged.to_csv(csv_after_location, sep=';', encoding='utf-8')


if __name__ == "__main__":
    args = parser.parse_args()
    # MongoDB connection
    mongo_client = pymongo.MongoClient(args.mongo)
    mongo_db = mongo_client[args.database]

    c1(mongo_db, csv_before_location="C1_before.csv", csv_after_location="C1_after.csv")

    mongo_client.close()
