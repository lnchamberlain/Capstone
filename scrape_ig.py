# UnderCover Recovery 
# Windows Desktop Application for scraping and storing keywords and flagged users from social media sites 
# Developed by Tyler and Logan 
# UAA CSCE Capstone Project Fall 2022
# Script to scrape Instagram data using region csv selection and keywords list
# Format for use: python3 scrape_ig.py REGION_SELECTION (int 1-6) OUTPUT FOLDER (CAN PUT DEFAULT)
# To use with test regions (much smaller): python3 scrape_ig.py 6 DEFAULT

from xml.sax.xmlreader import Locator
import requests
import sys
import datetime
import json
import time
import random
import csv



REGION_RESOLUTION_TABLE = {1:"./Program Data/Regions/ALL_ALASKA.csv", 2:"./Program Data/Regions/ANCHORAGE.csv", 3:"./Program Data/Regions/BETHEL.csv", 4:"./Program Data/Regions/FAIRBANKS.csv", 5:"./Program Data/Regions/JUNEAU.csv", 6:"./Program Data/Regions/TESTING.csv"}
LOCATION_URLS = {}
KEYWORDS = [] 
COOKIE = {}
FLAGGED_POSTS = []
FLAGGED_USERS = []
OUTPUT_DIR = ''
TOTAL_POSTS = 0
FOUND_FLAGGED = 0
HTML_CODE = []

#Fills global variable with urls from the last column of the .csv file
def get_urls():
    #Resolve region number into path
    region_file = REGION_RESOLUTION_TABLE[int(sys.argv[1])]
    with open(region_file, newline='') as csvfile:
        csv_data = csv.reader(csvfile, delimiter=',')
        for row in csv_data:
            LOCATION_URLS[row[1]] = row[-1]
    #First pair of values are header values
    LOCATION_URLS.pop("Location Name", None)


#Fills global variable with value from wordlist
def get_keywords():
    keywords_file = open("./Program Data/Wordlists/keywords.txt", "r+")
    global KEYWORDS
    KEYWORDS = keywords_file.read().split(",")
    

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
    global OUTPUT_DIR
    if(dir == "DEFAULT"):
        OUTPUT_DIR = "./Program Data/FoundPosts/FoundPostsIG"
    else:
        OUTPUT_DIR = dir

#Populates global list from flagged users list
def get_flagged_users():
    flagged_users_file = open("./Program Data/FlaggedUsers/IGFlaggedUsers/ig_flagged_users.txt", "r+")
    global FLAGGED_USERS
    FLAGGED_USERS = flagged_users_file.read().split(",")

 #Requests data from location url, formats the return, searches captions and comments for keywords, adds posts to flagged posts
def scrape_location(COUNTER, NUM_LOCATIONS, session, location):
    print("\n*****************************************************************\n")
    print("Scraping {}...".format(location))
    print("Location number {}/{}".format(COUNTER, NUM_LOCATIONS))
    response = session.get(LOCATION_URLS[location], cookies=COOKIE)
    print("Response Status is {}".format(response))
    post_list = response.content.split(b'"media":')
    print("Number of found posts: {}".format(len(post_list)))
    global TOTAL_POSTS
    global FLAGGED_POSTS
    TOTAL_POSTS += len(post_list)
    time.sleep(4)
    flagged = 0
    for post in post_list:
        for word in KEYWORDS:
            for author in FLAGGED_USERS:
                if (bytes(word, "utf-8")in post) or (bytes(author, "utf-8") in post):
                    FLAGGED_POSTS.append(post)
                    flagged +=1
    global FOUND_FLAGGED 
    FOUND_FLAGGED += flagged
    print("Found {} flagged posts at this location\nTotal Flagged Posts: {}".format(flagged, FOUND_FLAGGED))
    print("\n*****************************************************************\n")
    
     



#Requests the full info for a flagged post
def format_found_post(flagged_post):
    #GRAB THE VALUES WE WANT FROM THE FLAGGED POST, add to array of html code
    global HTML_CODE
    html_str = ''


#Iterate over array of lines of html code, write to scan output file
def write_html_to_file():
    for line in HTML_CODE:
        print(line)
    print("STUB")

#Writes full post info to selected output file

def main():
    get_urls()
    get_keywords()
    get_cookie()
    get_output_dir()
    get_flagged_users()
    COUNTER = 1
    NUM_LOCATIONS = len(LOCATION_URLS)

    session = requests.Session()
    #GENERAL PROCESS: scrape each location and flag posts, format post information and write to file
    for place in LOCATION_URLS:
        #ADD RANDOM LATENCY HERE
        time.sleep(random.randint(200,3000))
        scrape_location(COUNTER, NUM_LOCATIONS, session, place)
        COUNTER += 1
    for post in FLAGGED_POSTS:
        format_found_post(post)
    
    write_html_to_file()
    
    session.close()

if __name__ == "__main__":
    main()

