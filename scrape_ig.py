# UnderCover Recovery 
# Windows Desktop Application for scraping and storing keywords and flagged users from social media sites 
# Developed by Tyler and Logan 
# UAA CSCE Capstone Project Fall 2022
# Script to scrape Instagram data using region csv selection and keywords list
# Format for use: python3 scrape_ig.py REGION_SELECTION (int 1-5) OUTPUT FOLDER (CAN PUT DEFAULT)
# To use with test regions (much smaller): python3 scrape_ig.py 6 DEFAULT

import requests
import sys
import datetime
import json
import time
from bs4 import BeautifulSoup
import csv



REGION_RESOLUTION_TABLE = {1:"./Program Data/Regions/ALL_ALASKA.csv", 2:"./Program Data/Regions/ANCHORAGE.csv", 3:"./Program Data/Regions/BETHEL.csv", 4:"./Program Data/Regions/FAIRBANKS.csv", 5:"./Program Data/Regions/JUNEAU.csv", 6:"./Program Data/Regions/TESTING.csv"}
LOCATION_URLS = []
KEYWORDS = [] 
COOKIE = {}
FLAGGED_POSTS = []
OUTPUT_DIR = ''

#Fills global variable with urls from the last column of the .csv file
def get_urls():
    #Resolve region number into path
    region_file = REGION_RESOLUTION_TABLE[int(sys.argv[1])]
    with open(region_file, newline='') as csvfile:
        csv_data = csv.reader(csvfile, delimiter=',')
        for row in csv_data:
            LOCATION_URLS.append(row[-1])
    #First value is header 'URL'
    LOCATION_URLS.remove('URL')


#Fills global variable with value from wordlist
def get_keywords():
    keywords_file = open("./Program Data/Wordlists/keywords.txt", "r+")
    KEYWORDS = keywords_file.read()
    

#Fills global variable with value for IG_AUTH logs
def get_cookie():
    ig_auth_log_file = open("./Program Data/Logs/IG_AUTH_LOGS/log.txt")
    #Rebuild cookie dictionary from text file
    values = ig_auth_log_file.read().split("\n")
    for line in values:
        if(line != "SUCCESS"):
            line = line.split(":")
            COOKIE[line[0]] = line[1]


#Establishes the output directory
def get_output_dir():
    dir = sys.argv[2]
    if(dir == "DEFAULT"):
        OUTPUT_DIR = "./Program Data/FoundPosts/FoundPostsIG"
    else:
        OUTPUT_DIR = dir


 #Requests data from location url, formats the return, searches captions and comments for keywords, adds posts to flagged posts
def scrape_location(session, url):
    response = session.get(url, cookies=COOKIE)
    soup = BeautifulSoup(response.content, 'html.parser')
    #FIGURE OUT HOW TO GRAB THE POSTS HERE 
     



#Requests the full info for a flagged post
def get_full_info(flagged_post):
    print("This here is a stub")
    full_post_info = ''
    write_output(full_post_info)


#Writes output into.html file
def write_output(post_info):
    print("And here is another function stub")


#Writes full post info to selected output file

def main():
    get_urls()
    get_keywords()
    get_cookie()
    get_output_dir()

    session = requests.Session()
    #GENERAL PROCESS: scrape each location and flag posts, grab full info and write to file
    for url in LOCATION_URLS:
        #ADD RANDOM LATENCY HERE
        time.sleep(2)
        scrape_location(session, url)

    for post in FLAGGED_POSTS:
        #ADD RANDOM LATENCY HERE 
        time.sleep(2)
        get_full_info(post)
    
    session.close()

if __name__ == "__main__":
    main()

