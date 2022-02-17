import csv
import io
import numpy as np
import os
import sys
import pickle
import re
import requests
import zipfile
from bs4 import BeautifulSoup
from datetime import datetime

"""IZV project - 1st part
    Author: Natália Holková
    Login: xholko02
"""


class DataDownloader:
    csv_headers = [
        "id", "p36", "p37", "date", "weekday", "time", "type", "type_driving_vehicles",
        "type_fixed_obstacle", "accident_character", "accident_fault", "alcohol_in_culprit", "main_cause",
        "dead", "seriously_injured", "lightly_injured", "total_material_damage", "road_surface", "road_during_accident",
        "road_state", "weather_conditions", "visibility", "visual_conditions", "road_division", "accident_placement",
        "traffic management", "local_driving_change", "specific_places_objects", "directional_conditions",
        "number_vehicles", "traffic_accident_location", "crossing_type", "vehicle_type", "vehicle_manufacturer",
        "vehicle_year", "vehicle_characteristic", "skid", "vehicle_after", "leakage", "rescue_from_vehicle",
        "driving_direction", "vehicle_damage", "driver_category", "drive_state", "driver_external_influence", "a", "b",
        "x_coord", "y_coord", "f", "g", "h", "i", "j", "k", "l",
        "n", "o", "p", "q", "r", "s", "t", "accident_location"
    ]

    region_filename = {
        "PHA": "00.csv", "STC": "01.csv", "JHC": "02.csv", "PLK": "03.csv", 
        "KVK": "19.csv", "ULK": "04.csv", "LBK": "18.csv", "HKK": "05.csv",
        "PAK": "17.csv", "OLK": "14.csv", "MSK": "07.csv", "JHM": "06.csv",
        "ZLK": "15.csv", "VYS": "16.csv"
    }

    region_cache = {
        "PHA": None, "STC": None, "JHC": None, "PLK": None,
        "KVK": None, "ULK": None, "LBK": None, "HKK": None,
        "PAK": None, "OLK": None, "MSK": None, "JHM": None,
        "ZLK": None, "VYS": None
    }

    years = ['2016', '2017', '2018', '2019', '2020']

    def __init__(self, url="https://ehw.fit.vutbr.cz/izv/", folder="data", cache_filename="data_{}.pkl.gz"):
        self.url = url
        self.folder = folder
        self.cache_filename = cache_filename

        # Folder syntax structure check - remove "/" at end
        if self.folder[-1] == "/":
            self.folder = self.folder[:len(self.folder)-1]

        # create folder if it doesn't exist
        if not os.path.exists(self.folder):
            os.makedirs(self.folder)

    def download_data(self):
        """Function to download compressed data from url to folder, both specified by object attributes.
        For each year will download only single zip file.
        """

        headers = {
            'Accept-Encoding': 'gzip, deflate, sdch',
            'Accept-Language': 'en-US,en;q=0.8',
            'User-Agent': 'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_10_1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/39.0.2171.95 Safari/537.36',
            'Accept': 'text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8',
            'Referer': 'http://www.wikipedia.org/',
            'Connection': 'keep-alive',
        }

        resp = requests.get(self.url, headers=headers)
        if resp.status_code == 200:
            soup = BeautifulSoup(resp.text, 'html.parser')
            table = soup.find('table', class_='table table-fluid')
            
            if table is None:
                raise Exception("Could not find table within html.")

            # Create output folder if doesn't exist
            if not os.path.exists(self.folder):
                os.makedirs(self.folder)

            links = []
            for link in table.find_all('a', class_='btn btn-sm btn-primary'):
                links.append(link["href"])
            for year in self.years:
                # find if exists file with statistics for whole year
                year_archive = None
                year_archive = next((s for s in [s for s in links if "rok" in s] if year in s), None)
                if year_archive is None:
                    # 2016 has format "datagis2016.zip" - check for that
                    result = list(filter(lambda v: re.match(r"data/data[-]?gis[-]?{0}.zip".format(year), v), links))
                    if len(result) != 0:
                        year_archive = result[0]
                if year_archive is None:  # File with whole year stats is not present, need to find latest month file
                    for month in range(12, 0, -1):
                        month_str = str(month) if month > 9 else "0" + str(month)
                        result = list(
                            filter(lambda v: re.match(r"data/data[-]?gis[-]?{0}[-]?{1}.zip".format(month_str, year), v), links))
                        if len(result) != 0:
                            year_archive = result[0]
                            break

                if year_archive is not None:
                    download_path = self.url + year_archive
                    save_path = self.folder + "/" + os.path.basename(year_archive)

                    # Check if file not already downloaded
                    if os.path.basename(year_archive) not in [f for f in os.listdir(self.folder) if os.path.isfile(os.path.join(self.folder, f))]:
                        with requests.get(download_path, stream=True) as r:
                            with open(save_path, 'wb') as fd:
                                for chunk in r.iter_content(chunk_size=128):
                                    fd.write(chunk)
        else:
            raise Exception("Bad response code.")

    def parse_region_data(self, region):
        """Function to parse data from compressed archive for specified region.
        In case archives are not yet downloaded, it call function download().
        Returns data in form of list containing list of csv headers
        and actual data in numpy arrays by columns.

        Keyword arguments:
        region -- region code (e.g. "PHA")
        Return: tuple(list[str], list[np.ndarray])
        """
        # Get list of files in folder
        all_files = [f for f in os.listdir(self.folder) if os.path.isfile(os.path.join(self.folder, f))]
        all_found = True
        for year in self.years:
            if year not in [f for f in all_files if year in f]:
                all_found = False
        if not all_found:  # not archives for all years present - try download again
            self.download_data()
            all_files = [f for f in os.listdir(self.folder) if os.path.isfile(os.path.join(self.folder, f))]

        if region not in self.region_filename.keys():
            raise Exception("Invalid region code.")

        # prepare the tuple
        np_list = (len(self.csv_headers) + 1) * [np.array([])]
        column_list = (len(self.csv_headers) + 1) * [None]
        for i in range(len(column_list)):
            column_list[i] = []

        for year in self.years:
            # get all files with 'year' in name
            files = [s for s in all_files if year in s]

            # find if exists file with statistics for whole year
            year_file = next((s for s in files if "rok" in s), None)
            if year_file is None:
                # 2016 has format "datagis2016.zip" - check for that
                result = list(filter(lambda v: re.match(r"data[-]?gis[-]?{0}".format(year), v), files))
                if len(result) != 0:
                    year_file = result[0]

            if year_file is None:  # File with whole year stats is not present, need to find latest month file
                for month in range(12, 0, -1):
                    month_str = str(month) if month > 9 else "0" + str(month)
                    result = list(filter(lambda v: re.match(r"data[-]?gis[-]?{0}[-]?{1}".format(month_str, year), v), files))
                    if len(result) != 0:
                        year_file = result[0]
                        break

            if year_file is None:
                print("Warning! No data from year {} for region {}.".format(year, region), file=sys.stderr)
                continue

            # read one csv file from zip archive
            with zipfile.ZipFile(self.folder + "/" + year_file) as zf:
                with zf.open(self.region_filename[region], 'r') as infile:
                    reader = csv.reader(io.TextIOWrapper(infile, 'windows-1250'), delimiter=";")
                    num_lines = 0
                    
                    for row in reader:
                        num_lines += 1
                        for i in range(len(self.csv_headers)):
                            if i == self.csv_headers.index("time"):
                                try:
                                    time = int(row[i])
                                    hours = time // 100
                                    minutes = time % 100
                                    if hours > 24 or minutes > 59:  # invalid time
                                        column_list[i].append(None)
                                    else:  # valid time
                                        time_obj = datetime.strptime("{}:{}".format(hours, minutes), "%H:%M")
                                        column_list[i].append(time_obj)
                                except ValueError:
                                    column_list[i].append(None)
                            elif i == self.csv_headers.index("date"):
                                try:
                                    date_obj = datetime.strptime(row[i], "%Y-%m-%d")
                                    column_list[i].append(date_obj)
                                except ValueError:
                                    column_list[i].append(None)
                            else:
                                # try to convert to int
                                try:
                                    # if re.match(r"^[-+]?\d+$", lines[i]):  # is integer
                                    if re.match(r"^[-+]?([1-9]\d*|0)$", row[i]):  # is integer
                                        column_list[i].append(int(row[i]))
                                    elif re.match(r"^[-+]?\d*\.\d+|\d+$", row[i]):  # is float
                                        column_list[i].append(float(row[i]))
                                    elif re.match(r"^[-+]?\d*,\d+|\d+$", row[i]):  # is float but needs to replace ","
                                        corrected_float = row[i].replace(',', '.')
                                        column_list[i].append(float(corrected_float))
                                    elif len(row[i]) == 0:  # empty string
                                        column_list[i].append(None)
                                    else:  # is just normal string
                                        column_list[i].append(row[i])
                                except ValueError:
                                    column_list[i].append(row[i])

                    column_list[-1] = column_list[-1] + (num_lines * [region])

        for i in range(len(np_list)):
            if i == self.csv_headers.index("date") or i == self.csv_headers.index("time"):
                date_array = np.array(np.array(column_list[i], dtype='datetime64'))
                np_list[i] = np.asarray(column_list[i])
            else:
                np_list[i] = np.asarray(column_list[i])
        return self.csv_headers + ["region"], np_list

    def get_list(self, regions=None):
        """Function to get list of accident entries for all specified regions.
        In case data for region are not cached in object variable, it looks to cache file with location
        specified by object attribute.
        In case there is no cache file, calls function parse_region_data().

        Keyword arguments:
        region -- region code (e.g. "PHA")
        Return: tuple(list[str], list[np.ndarray])
        """
        if regions is None:
            regions = self.region_filename.keys()
        else:  # Check if all region names are correct
            for region in regions:
                if region not in self.region_filename.keys():
                    print("Error! Invalid region code ({}), skipping this region.".format(region), file=sys.stderr)
                    regions.remove(region)

        # prepare the tuple
        np_list = (len(self.csv_headers) + 1) * [np.array([])]
        for region in regions:
            # check if region data already loaded in class attribute
            if self.region_cache[region] is not None:
                data = self.region_cache[region]
            else:  # check if region data already stored as cache file
                region_cache_filename = self.cache_filename.replace("{}", region)
                if os.path.isfile(self.folder + "/" + region_cache_filename):  # cache file exists
                    with open(self.folder + "/" + region_cache_filename, 'rb') as f:
                        data = pickle.load(f)
                        self.region_cache[region] = data
                else:  # region data not yet cached
                    data = self.parse_region_data(region)
                    self.region_cache[region] = data  # cache data in attribute
                    with open(self.folder + "/" + region_cache_filename, 'wb') as f:  # Create cache file
                        pickle.dump(data, f)

            # Append acquired data to np array
            for i in range(len(np_list)):
                if np_list[i].size == 0:
                    np_list[i] = data[1][i]
                else:
                    np_list[i] = np.concatenate((np_list[i], data[1][i]))

        return self.csv_headers + ["region"], np_list
        

if __name__ == "__main__":
    dataDownloader = DataDownloader()
    data = dataDownloader.get_list(["JHM", "PAK", "OLK"])
    #data = dataDownloader.get_list()

    # print out column names
    print("Columns:\n\t", end="")
    print(*data[0], sep=' ')

    # print which regions are in dataset
    regions = []
    for i in range(data[1][0].size):
        # Check accident region
        region = data[1][data[0].index("region")][i]
        if region not in regions:
            regions.append(region)
    print("Regions:\n\t", end="")
    print(*sorted(regions), sep=' ')

    # print years
    years = []
    for i in range(data[1][0].size):
        # Check accident region
        date = data[1][data[0].index("date")][i]
        if date is None:
            continue
        year = date.year
        if year not in years:
            years.append(year)
    print("Years:\n\t", end="")
    print(*sorted(years), sep=' ')

    # print number of entries
    print("Number of entries:\n\t{}".format(data[1][0].size))
