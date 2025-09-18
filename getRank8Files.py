import datetime
from influxdb import InfluxDBClient

import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages
import matplotlib.dates as mdates
import matplotlib.colorbar as colorbar
from matplotlib import colormaps
import matplotlib.cm as cm
import pandas as pd
import json
import numpy as np
import datetime as datetime
import matplotlib.dates as mdates
from datetime import datetime, timedelta
import argparse
from scipy import stats
import csv




# to be able in the future to be able to use different bias voltages we will need to create a dictionary for different votage levels

# Create a dictionary to hold the bias voltage data for when the door is closed
# Bias voltages for door closed: SiPM current with door closed @ that bias voltage
dict_doorClosed = {44.0:3.6, 43.5:3.0, 43.0:3.0, 42.5:3.0, 42.0:3.0}


# Local host lines for access
host = 'localhost'
port = 8086
username = 'admin'
database = 'TDFiles'
password = 'Ttys@210'

# Initialize the InfluxDB client and write the points in batches
client = InfluxDBClient(host = host, port=port, username=username, password=password)

# Switch to the database
client.switch_database(database)

result = client.query(f'SELECT "Filename","ranking" FROM "RankingInfo"')

Filename = []
Ranking = []

for point in result.get_points():
    Filename.append(point['Filename'])
    Ranking.append(point['ranking'])

data = np.array([Filename, Ranking]).T
#print(data)

# Create a DataFrame from the array
dfRanking = pd.DataFrame(data, columns=['Filename', 'Ranking'])
#print(dfRanking)


result = client.query(f'SELECT "hvValues1", "hvcurrents1", "Filename" FROM "DataInfo"')


hvValues1 = []
hvcurrents1 = []
Filename = []

for point in result.get_points():
    hvValues1.append(point['hvValues1'])
    hvcurrents1.append(point['hvcurrents1'])
    Filename.append(point['Filename'])

data = np.array([hvValues1, hvcurrents1, Filename]).T
#print(data)
# Create a DataFrame from the array
dfData = pd.DataFrame(data, columns=['hvValues1', 'hvcurrents1', 'Filename'])
#print(dfData)

result = client.query(f'SELECT "sunAltitude", "moonAltitude", "Filename" FROM "CelestialInfo"')

sunAltitude = []
moonAltitude = []
Filename = []
for point in result.get_points():
    sunAltitude.append(point['sunAltitude'])
    moonAltitude.append(point['moonAltitude'])
    Filename.append(point['Filename'])

data = np.array([sunAltitude, moonAltitude, Filename]).T
#print(data)
# Create a DataFrame from the array
dfCelestial = pd.DataFrame(data, columns=['sunAltitude', 'moonAltitude', 'Filename'])
#print(dfCelestial)

# Merge the DataFrames on 'Filename'
dfMerged = pd.merge(dfRanking, dfData, on='Filename')
dfMerged = pd.merge(dfMerged, dfCelestial, on='Filename')

dfMerged['Ranking'] = dfMerged['Ranking'].astype(int)
dfMerged['hvValues1'] = dfMerged['hvValues1'].astype(float)
dfMerged['hvcurrents1'] = dfMerged['hvcurrents1'].astype(float)
dfMerged['sunAltitude'] = dfMerged['sunAltitude'].astype(float)
dfMerged['moonAltitude'] = dfMerged['moonAltitude'].astype(float)

# with pd.option_context('display.max_rows', None, 'display.max_columns', None, 'display.width', None, 'display.max_colwidth', None):
#     print(dfMerged)


# reduce the data by removing the rows where the ranking is not 8, and HVvalues are not 44
# check for data that is contains "44."
# in the future have it be that it loops through all the hvVlues and then uses that to get check for the dict value

dfFiltered = dfMerged[(dfMerged['Ranking'] == 8) & (dfMerged['hvValues1'] >= 43.95) & (dfMerged['hvcurrents1'] >= dict_doorClosed[44.0]) & (dfMerged['hvcurrents1'] <= 12.0)]

# with pd.option_context('display.max_rows', None, 'display.max_columns', None, 'display.width', None, 'display.max_colwidth', None):

# create a new column with the data from the filename column so that it is yyyy-mm-dd
dfFiltered['Date'] = (pd.to_datetime(dfFiltered['Filename'].str[12:22])).astype(str)
dfFiltered['Date'] = (dfFiltered['Date'].str.replace('-','')).astype(int)

print(dfFiltered)
# removed dates prior to 2024-10-01
dfFiltered = dfFiltered[dfFiltered['Date'] >= 20241001]
dfFiltered = dfFiltered[dfFiltered['Date'] <= 20250602]


""" 
create 1 csv file  start with 
total days = unique values in the Date column, 
total files = unique values in the Filename column, 
hours data = total  files *  97 seconds per file / 3600 seconds per hour

then the 2nd part will be the data for each day,
so  
night, files, hours data
"""

# create a txt file

with open('rank8Files.txt', mode='w', newline='') as file:
    
    # write the header
    file.write("Trinity Demonstrator Database Output - Rank 8 Files \n")
    file.write("")
    file.write(f"Period of time : {dfFiltered['Date'].min()} to {dfFiltered['Date'].max()} \n")
    file.write(f"Total Days: {len(dfFiltered['Date'].unique())} days \n")
    file.write(f"Total Files: {len(dfFiltered['Filename'].unique())} files \n")
    file.write(f"Hours Data: {(len(dfFiltered['Filename'].unique()) * 97) / 3600:.1f} hours \n")
    file.write("\n")
    file.write("Date, Night Files, Hours Data \n")
    # write the data for each day
    # get the unique values in the Date column
    unique_dates = dfFiltered['Date'].unique()
    total_days = len(unique_dates)
    
    # get the unique values in the Filename column
    unique_files = dfFiltered['Filename'].unique()
    total_files = len(unique_files)
    
    # calculate the hours data
    hours_data = (total_files * 97) / 3600
    for date in unique_dates:
        day_data = dfFiltered[dfFiltered['Date'] == date]
        night_files = len(day_data)
        night_hours_data = (night_files * 97) / 3600
        file.write(f"{date}, {night_files}, {night_hours_data:.2f}\n")

print("Rank 8 Files Data written to rank8Files.txt")
    

    # writer.writerow(['Date', 'Total Files', 'Total Days', 'Hours Data'])
    
    
    # # write the data
    # writer.writerow([unique_dates[0], total_files, total_days, hours_data])
    

