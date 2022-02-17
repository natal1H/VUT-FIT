import pymongo
from argparse import ArgumentParser
from queries.A1 import A1_extract_csv, A1_plot_graph
from queries.A3 import A3_extract_csv, A3_plot_graph
from queries.B1 import B1_extract_csv, B1_plot_graph
from queries.C1 import c1
from queries.VL1 import VL1_extract_csv, VL1_plot_graph
from queries.VL2 import VL2_extract_csv, VL2_plot_graph

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


def main():
    args = parser.parse_args()
    # MongoDB connection
    mongo_client = pymongo.MongoClient(args.mongo)
    mongo_db = mongo_client[args.database]
    A1_extract_csv(mongo_db, "A1.csv")
    A1_plot_graph("A1.csv", "A1.png")
    A3_extract_csv(mongo_db, "A3.csv")
    A3_plot_graph("A3.csv", "A3.png")
    B1_extract_csv(mongo_db)
    B1_plot_graph("B1.png")
    c1(mongo_db, csv_before_location="C1_before.csv", csv_after_location="C1_after.csv")
    VL1_extract_csv(mongo_db, "VL1.csv")
    VL1_plot_graph("VL1.csv", "VL1.png")
    VL2_extract_csv(mongo_db, "VL2.csv")
    VL2_plot_graph("VL2.csv", "VL2.png")

    mongo_client.close()


if __name__ == "__main__":
    main()
