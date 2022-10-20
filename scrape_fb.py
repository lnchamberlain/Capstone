# UnderCover Recovery 
# Windows Desktop Application for scraping and storing keywords and flagged users from social media sites 
# Developed by Tyler and Logan 
# UAA CSCE Capstone Project Fall 2022
# Script to scrape Facebook data using region csv selection and keywords list
# Format for use: python3 scrape_fb.py REGION_SELECTION (int 1-6) OUTPUT FOLDER (CAN PUT DEFAULT)
# To use with test regions (much smaller): python3 scrape_fb.py 6 DEFAULT

import datetime
from urllib.request import urlretrieve
import requests
import sys
import datetime
import json
import time
import random
import csv
import urllib
import hashlib
from PIL import Image
import shutil
import os




REGION_RESOLUTION_TABLE = {1:"./Program Data/Regions/ALASKA_FB.csv", 2:"./Program Data/Regions/ANCHORAGE_FB.csv", 3:"./Program Data/Regions/BETHEL_FB.csv", 4:"./Program Data/Regions/FAIRBANKS_FB.csv", 5:"./Program Data/Regions/JUNEAU_FB.csv", 6:"./Program Data/Regions/TESTING_FB.csv"}
MONTH_RESOLUTION_TABLE = {1:"Jan", 2:"Feb", 3:"Mar", 4:"Apr", 5:"May", 6:"Jun", 7:"Jul", 8:"Aug", 9:"Sep", 10:"Oct", 11:"Nov", 12:"Dec"}
LOCATION_URLS = {}
KEYWORDS = [] 
COOKIE = {}
FLAGGED_POSTS = []
FLAGGED_USERS = []
OUTPUT_DIR = ''
TOTAL_POSTS = 0
FOUND_FLAGGED = 0
HTML_CODE = []
SCAN_NAME = ''


#Fills global variable with urls from the last column of the .csv file
def get_urls():
    #Resolve region number into path
    region_file = REGION_RESOLUTION_TABLE[int(sys.argv[1])]
    with open(region_file, newline='', encoding="utf-8") as csvfile:
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
    ig_auth_log_file = open("./Program Data/Logs/FB_AUTH_LOGS/log.txt")
    values = ig_auth_log_file.read().split("\n")
    #Maybe need to cast?
    global COOKIE
    val = values[0][1:-1]
    cookie_dict = {}
    val = (values[0][1:-1]).split(":")
    for i in range(len(val) - 1):
        cookie_dict[val[i]] = val[i+1]
        i += 1
    
    print(cookie_dict)
    sys.exit()
    #COOKIE = json.loads(val)
    COOKIE = cookie_dict
    


#Establishes the output directory
def get_output_dir():
    dir = sys.argv[2]
    global OUTPUT_DIR
    if(dir == "DEFAULT"):
        OUTPUT_DIR = "./Program Data/FoundPosts/FoundPostsFB"
    else:
        OUTPUT_DIR = dir

#Populates global list from flagged users list
def get_flagged_users():
    flagged_users_file = open("./Program Data/FlaggedUsers/FBFlaggedUsers/fb_flagged_users.txt", "r+")
    global FLAGGED_USERS
    FLAGGED_USERS = flagged_users_file.read().split(",")

 #Requests data from location url, formats the return, searches captions and comments for keywords, adds posts to flagged posts
def scrape_location(session, location):
    #open temporary file to write to
    temp_file = open("./Program Data/Logs/FB_SCRAPE_LOGS/temp.txt", "w", encoding="utf-8")
    print("\n*****************************************************************\n")
    print("Cookie is {}\nLocation is {}\n".format(COOKIE, location))
    response = session.get(LOCATION_URLS[location], cookies=COOKIE)
    print("Response is {}".format(response))
    print("\n*****************************************************************\n")
   



#Requests the full info for a flagged post
def format_found_post(flagged_post):
    #GRAB THE VALUES WE WANT FROM THE FLAGGED POST, add to array of html code
    global HTML_CODE
    
    




#Iterate over array of lines of html code, write to scan output file
def write_html_to_file():
    output_file = open(OUTPUT_DIR + "/" + SCAN_NAME + ".html", 'w+', encoding="utf-8")
    #Fill in the table header and footer of the html document
    global HTML_CODE 
    #Clear out duplicate entries
    HTML_CODE = set(HTML_CODE)
    HTML_CODE = list(HTML_CODE)
    HTML_CODE.insert(0, "<html><body><table><head><link rel='stylesheet' href='../../../styles.css'></head>\n")
    HTML_CODE.insert(1, "<h1 style='text-align:center;'>" + SCAN_NAME + "</h1>")
    #HTML_CODE.insert(2, "<tr><th>Date/Time</th><th>Lat/Long</th><th>Username</th><th>Full Name</th><th>Profile</th><th>Caption/Comment</th><th>Link</th><th>Media</th></tr>")
    HTML_CODE.append("</table></body></html>\n")
    for line in HTML_CODE:
        output_file.write(line)
    output_file.close()


def main():
    get_urls()
    get_keywords()
    get_cookie()
    get_output_dir()
    get_flagged_users()
    COUNTER = 1
    NUM_LOCATIONS = len(LOCATION_URLS)
    global SCAN_NAME 
    currTime = datetime.datetime.now()
    date_and_time_formatted = MONTH_RESOLUTION_TABLE[currTime.month] + " "+str(currTime.day) +" "+ str(currTime.year) + " @ " + str(currTime.hour) + "." + str(currTime.minute) + "." + str(currTime.second) 
    SCAN_NAME = "FB SCAN REPORT " + date_and_time_formatted
    clear_log = open("./Program Data/Logs/FB_SCRAPE_LOGS/log.txt", "w", encoding="utf-8")
    clear_log.close()
    session = requests.Session()
    #GENERAL PROCESS: scrape each location and flag posts, format post information and write to file
    for place in LOCATION_URLS:
        #Random Latency
        delay = random.randint(200, 3000)
        delay /= 1000
        print("delay is {}".format(delay))
        time.sleep(delay)
        scrape_location(session, place)
        COUNTER += 1
   
    
    #write_html_to_file()
    #Clear log file, write final summary
    log_file = open("./Program Data/Logs/FB_SCRAPE_LOGS/log.txt", "w")
    log_file.write("TOTAL SCRAPED POSTS: {}\nTOTAL FLAGGED POSTS: {}\nSCAN COMPLETE".format(TOTAL_POSTS, FOUND_FLAGGED))
    log_file.close()
    session.close()

if __name__ == "__main__":
    main()


