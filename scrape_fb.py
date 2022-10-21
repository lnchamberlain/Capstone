# UnderCover Recovery 
# Windows Desktop Application for scraping and storing keywords and flagged users from social media sites 
# Developed by Tyler and Logan 
# UAA CSCE Capstone Project Fall 2022
# Script to scrape Facebook data using region csv selection and keywords list
# Format for use: python3 scrape_fb.py REGION_SELECTION (int 1-6) OUTPUT FOLDER (CAN PUT DEFAULT)
# To use with test regions (much smaller): python3 scrape_fb.py 6 DEFAULT

import datetime
from tempfile import tempdir
from tkinter import END
from urllib.request import urlretrieve
from bs4 import BeautifulSoup
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
import pickle
import selenium
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.common.by import By
from selenium.common.exceptions import TimeoutException
from selenium.webdriver.chrome.options import Options



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
    

#Grabs cookie value from AUTH logs, rebuilds dictionary and sets global variable
def get_cookie():
    ig_auth_log_file = open("./Program Data/Logs/FB_AUTH_LOGS/log.txt")
    global COOKIE
    cookies = pickle.load(open("./Program Data/Logs/FB_AUTH_LOGS/fb_cookies.pkl", "rb"))
    #cookies_dict = {}
    #for cookie in cookies:
    #    cookies_dict[cookie['name']] = cookie['value']
    #COOKIE = cookies_dict
    COOKIE = cookies


    


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
def scrape_location(driver, location, counter):
    #open temporary file to write to
    temp_file = open("./Program Data/Logs/FB_SCRAPE_LOGS/temp.txt", "w", encoding="utf-8")
    global TOTAL_POSTS
    SCROLL_PAUSE = 1
    SCROLL_COUNT = 0
    GET_CURRENT_SCROLL_HEIGHT = "return document.body.scrollHeight"
    SCROLL_DOWN_SCRIPT = "window.scrollTo(0, document.body.scrollHeight);"
    print("\n*****************************************************************\n")
    print("\nScraping Location {}...".format(location))
    print("Number {}/{}\n".format(counter, len(LOCATION_URLS)))
    l = []
    for word in KEYWORDS:
        url_base = LOCATION_URLS[location].split("word")
        #add keyword to crafted url
        url = url_base[0] + word[1:-1] + url_base[1]
        driver.get(url)
        try:
            WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.CLASS_NAME, 'x9f619 x1n2onr6 x1ja2u2z xdt5ytf x193iq5w xeuugli x1r8uery x1iyjqo2 xs83m0k x78zum5 x1t2pt76')))
        except TimeoutException:
            print("Advacing to scrolling")
        prev_height = driver.execute_script(GET_CURRENT_SCROLL_HEIGHT)
        #No results case
        if("We didn't find any results" in driver.page_source):
                print("FOUND POSTS: 0")
                print("TOTAL FOUND POSTS: {}\n".format(TOTAL_POSTS))
                print("\n*****************************************************************\n") 
                return
        #Keep scrolling and grabbing posts until bottom is reached
        while True:
            s = BeautifulSoup(driver.page_source, 'html.parser')
            l.append(s.find_all("div", {"class":"x1ja2u2z xh8yej3 x1n2onr6 x1yztbdb"}))
            print("Scroll count:{}\n".format(SCROLL_COUNT))
            driver.execute_script(SCROLL_DOWN_SCRIPT)
            SCROLL_COUNT +=1
            SCROLL_PAUSE = random.randint(500, 2000)
            SCROLL_PAUSE /= 1000
            time.sleep(SCROLL_PAUSE)
            curr_height = driver.execute_script(GET_CURRENT_SCROLL_HEIGHT)
            if curr_height == prev_height:
                #l.append(s.find_all("div", {"class":"x1ja2u2z xh8yej3 x1n2onr6 x1yztbdb"}))
                break 
            prev_height = curr_height
        
        print("FOUND POSTS: {}".format(len(l)))
        TOTAL_POSTS += len(l)
        print("TOTAL FOUND POSTS: {}\n".format(TOTAL_POSTS))
        for i in range(2):
            format_found_post(l[i])
       
    print("\n*****************************************************************\n")

   



#Requests the full info for a flagged post
def format_found_post(flagged_posts):
    #GRAB THE VALUES WE WANT FROM THE FLAGGED POST, add to array of html code
    global HTML_CODE
    HEADER_STR = "<html><head></head><body>"
    END_STR = "</body></html>"
    for post in flagged_posts:
        print(len(flagged_posts))
        closed_html = HEADER_STR + str(post) + END_STR
        s = BeautifulSoup(closed_html, "html.parser")
        f = open("POST_FB_AUTOSPY.txt", "w+", encoding='utf-8')
        f.write(s.prettify())
        f.write("\n\n")
 
    
    




#Iterate over array of lines of html code, write to scan output file
def write_html_to_file():
    output_file = open(OUTPUT_DIR + "/" + SCAN_NAME + ".html", 'w+', encoding="utf-8")
    #Fill in the table header and footer of the html document
    global HTML_CODE 
    #Clear out duplicate entries
    HTML_CODE = set(HTML_CODE)
    HTML_CODE = list(HTML_CODE)
    HTML_CODE.insert(0, "<html><head><link rel='stylesheet' href='../../../styles.css'></head><body><table>\n")
    HTML_CODE.insert(1, "<h1 style='text-align:center;'>" + SCAN_NAME + "</h1>")
    #HTML_CODE.insert(2, "<tr><th>Date/Time</th><th>Lat/Long</th><th>Username</th><th>Full Name</th><th>Profile</th><th>Caption/Comment</th><th>Link</th><th>Media</th></tr>")
    HTML_CODE.append("</table></body></html>\n")
    for line in HTML_CODE:
        output_file.write(line)
    output_file.close()


def main():
    get_urls()
    get_keywords()
    session = get_cookie()
    get_output_dir()
    get_flagged_users()
    COUNTER = 1
    global SCAN_NAME 
    currTime = datetime.datetime.now()
    date_and_time_formatted = MONTH_RESOLUTION_TABLE[currTime.month] + " "+str(currTime.day) +" "+ str(currTime.year) + " @ " + str(currTime.hour) + "." + str(currTime.minute) + "." + str(currTime.second) 
    SCAN_NAME = "FB SCAN REPORT " + date_and_time_formatted
    clear_log = open("./Program Data/Logs/FB_SCRAPE_LOGS/log.txt", "w", encoding="utf-8")
    clear_log.close()
    chrome_options = Options()
    #--headless makes the window not pop up
    chrome_options.add_argument("--headless")
    driver = selenium.webdriver.Chrome("./chromedriver", options=chrome_options)
    driver.get("https://facebook.com")
    for c in COOKIE:
        driver.add_cookie(c)
    #GENERAL PROCESS: scrape each location url using crafted urls looking for keywords, write relevant information to html strings
    for place in LOCATION_URLS:
        #Random Latency
        delay = random.randint(200, 3000)
        delay /= 1000
        print("delay is {}".format(delay))
        time.sleep(delay)
        scrape_location(driver, place, COUNTER)
        COUNTER += 1
        sys.exit()
   
    
    #write_html_to_file()
    #Clear log file, write final summary
    log_file = open("./Program Data/Logs/FB_SCRAPE_LOGS/log.txt", "w")
    log_file.write("TOTAL SCRAPED POSTS: {}\nTOTAL FLAGGED POSTS: {}\nSCAN COMPLETE".format(TOTAL_POSTS, FOUND_FLAGGED))
    log_file.close()
    session.close()

if __name__ == "__main__":
    main()


