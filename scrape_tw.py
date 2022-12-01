
# import requests

# payload = {
#     'inUserName': 'ExampleUsername',
#     'inUserPass': 'ExamplePassword'
# }

# with requests.Session() as s:
#     p = s.post('https://twitter.com/i/flow/login', data=payload)
#     #print(p.text)
    
#     r = s.get('https://twitter.com/home')
#     with open(r'/home/wisteria/CapstoneProject/Capstone/twitter_scrape_test.txt', mode='wb') as file:
#         file.write(r.content)
#         file.close()
        
# IF you look at the twitter_scrape_test.txt you'll see that the request worked,
# but that twitter hashes the content in line 6.
# Looks like we'll need to work around it with selenium.??
# More research required.
# coding=utf-8

# UnderCover Recovery 
# Windows Desktop Application for scraping and storing keywords and flagged users from social media sites 
# Developed by Tyler and Logan 
# UAA CSCE Capstone Project Fall 2022
# Script to scrape Twitter data using region csv selection and keywords list
# Format for use: python3 scrape_tw.py REGION_SELECTION (int 1-6) OUTPUT FOLDER (CAN PUT DEFAULT) //May change based on url type
# To use with test regions (much smaller): python3 scrape_tw.py 6 DEFAULT

import datetime
from email.errors import ObsoleteHeaderDefect
from operator import iadd
from re import I
from urllib.request import urlretrieve
from bs4 import BeautifulSoup
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
import sys



#REGION_RESOLUTION_TABLE = {1:"./Program Data/Regions/ALASKA_FB.csv", 2:"./Program Data/Regions/ANCHORAGE_FB.csv", 3:"./Program Data/Regions/BETHEL_FB.csv", 4:"./Program Data/Regions/FAIRBANKS_FB.csv", 5:"./Program Data/Regions/JUNEAU_FB.csv", 6:"./Program Data/Regions/TESTING_FB.csv"}
MONTH_RESOLUTION_TABLE = {1:"Jan", 2:"Feb", 3:"Mar", 4:"Apr", 5:"May", 6:"Jun", 7:"Jul", 8:"Aug", 9:"Sep", 10:"Oct", 11:"Nov", 12:"Dec"}
LOCATION_URLS = {}
KEYWORDS = [] 
COOKIE = {}
FLAGGED_POSTS = []
FLAGGED_USERS = []
OUTPUT_DIR = ''
TOTAL_POSTS = 0
FOUND_FLAGGED = 0
HTML_CODE_KEYWORDS = []
HTML_CODE_FLAGGED_USERS = []
SCAN_NAME = ''
NUM_LOCATIONS = 0
NOT_DEFAULT_DIR = False


#Fills global variable with urls from the last column of the .csv file
def get_urls():
    #Resolve region number into path
    #region_file = REGION_RESOLUTION_TABLE[int(sys.argv[1])] 
    #No regions split for csv, all alaska, no need for first arguement yet
    region_file = "./Program Data/Regions/ALASKA_TW.csv"
    with open(region_file, newline='', encoding="utf-8") as csvfile:
        csv_data = csv.reader(csvfile, delimiter=',')
        for row in csv_data:
            LOCATION_URLS[row[0]] = row[1]
    #First pair of values are header values
    global NUM_LOCATIONS
    NUM_LOCATIONS = len(LOCATION_URLS)




#Fills global variable with value from wordlist
def get_keywords():
    keywords_file = open("./Program Data/Wordlists/keywords.txt", "r+")
    global KEYWORDS
    lines = keywords_file.readlines()[7:]
    KEYWORDS = lines[0].split(",")
    # #Advanced search only support for IG
    for elem in KEYWORDS:
         if("BEFORE" in elem) or ("AFTER" in elem) or ("+" in elem):
             KEYWORDS.remove(elem)
    

#Grabs cookie value from AUTH logs, rebuilds dictionary and sets global variable
def get_cookie():
    global COOKIE
    cookies = pickle.load(open("./Program Data/Logs/TW_AUTH_LOGS/tw_cookies.pkl", "rb"))
    COOKIE = cookies

#Establishes the output directory
def get_output_dir():
    output_dir = sys.argv[2]
    global OUTPUT_DIR
    global NOT_DEFAULT_DIR 
    if(output_dir == "DEFAULT"):
        OUTPUT_DIR = "./Program Data/FoundPosts/FoundPostsTW"
    else:
        NOT_DEFAULT_DIR = True
        OUTPUT_DIR = output_dir
        shutil.copy("./styles.css", output_dir + "/styles.css")

#Populates global list from flagged users list
def get_flagged_users():
    flagged_users_file = open("./Program Data/FlaggedUsers/TWFlaggedUsers/tw_flagged_users.txt", "r+")
    global FLAGGED_USERS
    FLAGGED_USERS = flagged_users_file.read().split(",")

 #Requests data from location url, formats the return, searches captions and comments for keywords, adds posts to flagged posts
def scrape_location(driver, location, counter):
    SCROLL_LIMIT = 10
    #open temporary file to write to
    temp_file = open("./Program Data/Logs/TW_SCRAPE_LOGS/temp.txt", "w", encoding="utf-8")
    temp_file.write(location + "\n")
    temp_file.write("Location {}/{}\n".format(counter, NUM_LOCATIONS))
    global TOTAL_POSTS
    SCROLL_PAUSE = 1
    SCROLL_COUNT = 0
    GET_CURRENT_SCROLL_HEIGHT = "return document.body.scrollHeight"
    SCROLL_DOWN_SCRIPT = "window.scrollTo(0, document.body.scrollHeight);"
    print("\n*****************************************************************\n")
    print("\nScraping Location {}...".format(location))
    print("Number {}/{}\n".format(counter, len(LOCATION_URLS)))
    l = []
    posts = []
    global TOTAL_POSTS
    for word in KEYWORDS:
        url_base = LOCATION_URLS[location].split("word")
        #add keyword to crafted url
        url = url_base[0] + word + url_base[1]
        driver.get(url)
        time.sleep(0.5)
        if("We didn't find any results" in driver.page_source):
                temp_file.write("FOUND POSTS: 0\n")
                temp_file.write("TOTAL POSTS: {}\n".format(TOTAL_POSTS))
                print("FOUND POSTS: 0")
                print("TOTAL FOUND POSTS: {}\n".format(TOTAL_POSTS))
                print("\n*****************************************************************\n") 
                return
        try:
            WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.CLASS_NAME, 'x9f619.x1n2onr6.x1ja2u2z.xdt5ytf.x193iq5w.xeuugli.x1r8uery.x1iyjqo2.xs83m0k.x78zum5.x1t2pt76')))
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
            print("Scroll count:{}\n".format(SCROLL_COUNT))
            driver.execute_script(SCROLL_DOWN_SCRIPT)
            SCROLL_COUNT +=1
            SCROLL_PAUSE = random.randint(500, 2000)
            SCROLL_PAUSE /= 1000
            time.sleep(SCROLL_PAUSE)
            curr_height = driver.execute_script(GET_CURRENT_SCROLL_HEIGHT)
            if curr_height == prev_height or SCROLL_COUNT > SCROLL_LIMIT:
                break 
            prev_height = curr_height
    temp_file.write("Scrolled {} times\n".format(SCROLL_COUNT))
    s = BeautifulSoup(driver.page_source, 'html.parser')
    ####################################################################################################
    #Break up by tweets
    #######################################################################################################
    l.append(s.find_all("div", {"class": "css-1dbjc4n r-1iusvr4 r-16y2uox r-1777fci r-kzbkwu"}))
    for segment in l:
        for post in segment:
            posts.append(post)
    print("FOUND POSTS: {}".format(len(posts)))
    temp_file.write("Scraped Posts: {}\n".format(len(posts)))
    TOTAL_POSTS += len(posts)
    print("TOTAL FOUND POSTS: {}\n".format(TOTAL_POSTS))
    temp_file.write(("Total Posts: {}\n".format(TOTAL_POSTS)))
    temp_file.close()
    shutil.copy("./Program Data/Logs/TW_SCRAPE_LOGS/temp.txt", "./Program Data/Logs/TW_SCRAPE_LOGS/log.txt")
        #Get rid of temporary file
    os.remove("./Program Data/Logs/TW_SCRAPE_LOGS/temp.txt")
    #posts is an array of beautifulsoup objects, one per post 
    for post in posts:
       format_found_post(post, driver, "KEYWORDS")
       
    print("\n*****************************************************************\n")





     #Requests data from location url, formats the return, searches captions and comments for keywords, adds posts to flagged posts
def scrape_flagged_user(driver, username, counter):
    #open temporary file to write to
    temp_file = open("./Program Data/Logs/TW_SCRAPE_LOGS/temp.txt", "w", encoding="utf-8")
    temp_file.write(username + "\n")
    temp_file.write("user {}/{}\n".format(counter, len(FLAGGED_USERS)))
    global TOTAL_POSTS
    SCROLL_PAUSE = 1
    SCROLL_COUNT = 0
    SCROLL_LIMIT = 20
    GET_CURRENT_SCROLL_HEIGHT = "return document.body.scrollHeight"
    SCROLL_DOWN_SCRIPT = "window.scrollTo(0, document.body.scrollHeight);"
    print("\n*****************************************************************\n")
    print("\nScraping User {}...".format(username))
    print("Number {}/{}\n".format(counter, len(FLAGGED_USERS)))
    l = []
    posts = []
    global TOTAL_POSTS
    url = "https://www.twitter.com/" + username.strip()
    print(url)
    time.sleep(1)
    driver.get(url)
    try:
        ##############################################################################################################################
        #Wait for page content to load
        ##############################################################################################################################
            WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.CLASS_NAME, 'x9f619 x1n2onr6 x1ja2u2z xdt5ytf x193iq5w xeuugli x1r8uery x1iyjqo2 xs83m0k x78zum5 x1t2pt76')))
    except TimeoutException:
            print("Advacing to scrolling")
    prev_height = driver.execute_script(GET_CURRENT_SCROLL_HEIGHT)
    #No results case
    if("Try searching for another." in driver.page_source):
        print("FOUND POSTS: 0")
        print("TOTAL FOUND POSTS: {}\n".format(TOTAL_POSTS))
        print("\n*****************************************************************\n") 
        return
    #Keep scrolling and grabbing posts until bottom is reached
    while True:
        print("Scroll count:{}\n".format(SCROLL_COUNT))
        driver.execute_script(SCROLL_DOWN_SCRIPT)
        SCROLL_COUNT +=1
        SCROLL_PAUSE = random.randint(500, 2000)
        SCROLL_PAUSE /= 1000
        time.sleep(SCROLL_PAUSE)
        curr_height = driver.execute_script(GET_CURRENT_SCROLL_HEIGHT)
        if(curr_height == prev_height) or (SCROLL_COUNT >= SCROLL_LIMIT):
                break 
        prev_height = curr_height

    temp_file.write("Scrolled {} times\n".format(SCROLL_COUNT))
    s = BeautifulSoup(driver.page_source, 'html.parser')
    ####################################################################################################
    #Break up by tweets
    #######################################################################################################
    l.append(s.find_all("div", {"class": "css-1dbjc4n r-1iusvr4 r-16y2uox r-1777fci r-kzbkwu"}))
    for segment in l:
        for post in segment:
            posts.append(post)
    print("FOUND POSTS: {}".format(len(posts)))
    temp_file.write("Scraped Posts: {}\n".format(len(posts)))
    TOTAL_POSTS += len(posts)
    print("TOTAL FOUND POSTS: {}\n".format(TOTAL_POSTS))
    temp_file.write(("Total Posts: {}\n".format(TOTAL_POSTS)))
    temp_file.close()
    shutil.copy("./Program Data/Logs/TW_SCRAPE_LOGS/temp.txt", "./Program Data/Logs/TW_SCRAPE_LOGS/log.txt")
        #Get rid of temporary file
    os.remove("./Program Data/Logs/TW_SCRAPE_LOGS/temp.txt")
    #posts is an array of beautifulsoup objects, one per post 
    for post in posts:
       format_found_post(post, driver, "FLAGGED_USERS")
       
    print("\n*****************************************************************\n")




#Requests the full info for a flagged post
def format_found_post(flagged_post, driver, mode):
    #GRAB THE VALUES WE WANT FROM THE FLAGGED POST, add to array of html code
    global HTML_CODE
    AUTHOR = ''
    TIMESTAMP = ''
    CAPTION = ''
    POST_LINK = ''
    AUTHOR_LINK = ''
    IMG_PATH_HTML = ''
    img_hash = ''
    html_str = ''
    #############################################################################################################
    #Grab values of interest
    #####################################################################################################################
    l = flagged_post.find_all("span", {"class": "css-901oao css-16my406 css-1hf3ou5 r-poiln3 r-bcqeeo r-qvutc0"})
    if len(l) > 0: 
        AUTHOR = l[0].get_text()
    else:
        AUTHOR = ''
       
    for i in flagged_post.findAll('time'):
        if(i.has_attr('datetime')):
            TIMESTAMP = i['datetime']

        else:
            TIMESTAMP = ''
            POST_LINK = ''

    l = flagged_post.find_all("div", {"class": "css-901oao css-1hf3ou5 r-14j79pv r-18u37iz r-37j5jr r-a023e6 r-16dba41 r-rjixqe r-bcqeeo r-qvutc0"})
    if(len(l)>0):
        AUTHOR_LINK = "https://twitter.com/"+ l[0].get_text()[1:]
    else:
        AUTHOR_LINK = ''

    l = flagged_post.find_all("div", {"class": "css-901oao r-18jsvk2 r-37j5jr r-a023e6 r-16dba41 r-rjixqe r-bcqeeo r-bnwqim r-qvutc0"})
    if(len(l)>0):
        CAPTION = l[0].get_text()
    else:
        CAPTION = ''

    l = flagged_post.find_all("a", {"class": "css-4rbku5 css-18t94o4 css-901oao r-14j79pv r-1loqt21 r-xoduu5 r-1q142lx r-1w6e6rj r-37j5jr r-a023e6 r-16dba41 r-9aw3ui r-rjixqe r-bcqeeo r-3s2u2q r-qvutc0"})
    if(len(l)>0):
        link_str = str(l[0])
        href_index = link_str.find(" href=")
        href_end = link_str.find("role", href_index)
        POST_LINK = "https://twitter.com" + link_str[(href_index + 7):href_end - 2]

    else:
        POST_LINK = ''

    img = flagged_post.find_all("img", {"alt": "Image"})
    if(len(img)>0):
       img_str = str(img[0])
       href_index = link_str.find(" src=")
       href_end = link_str.find("class", href_index)
       img_url = img_str[(href_index + 59): href_end - 2]
       img_url = img_url.replace("amp;", "")
       driver.get(img_url)
       time.sleep(0.25)
       
       #     IMG_PATH_HTML = ''
       driver.save_screenshot("./Program Data/temp_img.png")
       img_hash = hashlib.md5(Image.open("./Program Data/temp_img.png").tobytes())
       hash_str = img_hash.hexdigest()
       img_path = "./Program Data/Images/ImagesTW/" + hash_str + ".png"
       IMG_PATH_HTML = str("../../../Program Data/Images/ImagesTW/"+ hash_str + ".png")
       if(not os.path.exists(img_path)):
                shutil.copy("./Program Data/temp_img.png", img_path)
            #Get rid of temporary image
       os.remove("./Program Data/temp_img.png")
      
       #IMG_PATH_HTML = img_url
       print(IMG_PATH_HTML)
       print("AUTHOR: {}\nTIMESTAMP: {}\nCAPTION: {}\nLINK: {}\nACCOUNT_LINK: {}\nMEDIA_LINK: {}\n".format(AUTHOR, TIMESTAMP, CAPTION, POST_LINK, AUTHOR_LINK, IMG_PATH_HTML))
       html_str += "<td>" + AUTHOR + "</td><td>" + TIMESTAMP + "</td><td>" + CAPTION + "</td><td><a href=" + POST_LINK + "> Tweet Link</a><td><a href=" + AUTHOR_LINK + ">Account Link</td><td><img style='max-width:200px;' src='" + IMG_PATH_HTML + "'></td></tr>"
       if(mode == "KEYWORDS"):
             HTML_CODE_KEYWORDS.append(html_str)
       if(mode == "FLAGGED_USERS"):
             HTML_CODE_FLAGGED_USERS.append(html_str)

    else:
       IMG_PATH_HTML = "No Image Media"
       print("AUTHOR: {}\nTIMESTAMP: {}\nCAPTION: {}\nLINK: {}\nACCOUNT_LINK: {}\nMEDIA_LINK: {}\n".format(AUTHOR, TIMESTAMP, CAPTION, POST_LINK, AUTHOR_LINK, IMG_PATH_HTML))
       html_str += "<td>" + AUTHOR + "</td><td>" + TIMESTAMP + "</td><td>" + CAPTION + "</td><td><a href=" + POST_LINK + ">Tweet Link</a><td><a href=" + AUTHOR_LINK + ">Account Link</td><td></td></tr>"
       if(mode == "KEYWORDS"):
             HTML_CODE_KEYWORDS.append(html_str)
       if(mode == "FLAGGED_USERS"):
             HTML_CODE_FLAGGED_USERS.append(html_str)
        
   



#Iterate over array of lines of html code, write to scan output file
def write_html_to_file(mode):
    
    #Fill in the table header and footer of the html document
    if(mode == "KEYWORDS"):
        output_file = open(OUTPUT_DIR + "/" + SCAN_NAME_KEYWORDS + ".html", 'w+', encoding="utf-8")
        global HTML_CODE_KEYWORDS
    
        #Clear out duplicate entries
        HTML_CODE_KEYWORDS = set(HTML_CODE_KEYWORDS)
        HTML_CODE_KEYWORDS = list(HTML_CODE_KEYWORDS)
        HTML_CODE_KEYWORDS.insert(0, "<html><head><link rel='stylesheet' href='../../../styles.css'></head><body><table>\n")
        HTML_CODE_KEYWORDS.insert(1, "<h1 style='text-align:center;'>" + SCAN_NAME_KEYWORDS + "</h1>")
        HTML_CODE_KEYWORDS.insert(2, "<tr><th>Post Author</th><th>Timestamp</th><th>Caption</th><th>Post Link</th><th>Account Link</th><th>Image</th></tr>")
        HTML_CODE_KEYWORDS.append("</table></body></html>\n")
        for line in HTML_CODE_KEYWORDS:
            output_file.write(line)
        output_file.close()

    if(mode == "FLAGGED_USERS"):
        output_file = open(OUTPUT_DIR + "/" + SCAN_NAME_FLAGGED + ".html", 'w+', encoding="utf-8")
        global HTML_CODE_FLAGGED_USERS
    
        #Clear out duplicate entries
        HTML_CODE_FLAGGED_USERS = set(HTML_CODE_FLAGGED_USERS)
        HTML_CODE_FLAGGED_USERS = list(HTML_CODE_FLAGGED_USERS)
        HTML_CODE_FLAGGED_USERS.insert(0, "<html><head><link rel='stylesheet' href='../../../styles.css'></head><body><table>\n")
        HTML_CODE_FLAGGED_USERS.insert(1, "<h1 style='text-align:center;'>" + SCAN_NAME_FLAGGED + "</h1>")
        HTML_CODE_FLAGGED_USERS.insert(2, "<tr><th>Post Author</th><th>Timestamp</th><th>Caption</th><th>Post Link</th><th>Account Link</th><th>Image</th></tr>")
        HTML_CODE_FLAGGED_USERS.append("</table></body></html>\n")
        for line in HTML_CODE_FLAGGED_USERS:
            output_file.write(line)
        output_file.close()



def main():
    get_urls()
    get_keywords()
    session = get_cookie()
    get_output_dir()
    get_flagged_users()
    COUNTER = 1
    global SCAN_NAME_KEYWORDS
    global SCAN_NAME_FLAGGED 
    currTime = datetime.datetime.now()
    date_and_time_formatted = MONTH_RESOLUTION_TABLE[currTime.month] + " "+str(currTime.day) +" "+ str(currTime.year) + " @ " + str(currTime.hour) + "." + str(currTime.minute) + "." + str(currTime.second) 
    SCAN_NAME_KEYWORDS = "TW KEYWORDS REPORT " + date_and_time_formatted
    SCAN_NAME_FLAGGED = "TW FLAGGED USERS REPORT " + date_and_time_formatted
    clear_log = open("./Program Data/Logs/TW_SCRAPE_LOGS/log.txt", "w", encoding="utf-8")
    clear_log.close()
    chrome_options = Options()
    #--headless makes the window not pop up
    chrome_options.add_argument("--headless")
    driver = selenium.webdriver.Chrome("./chromedriver", options=chrome_options)
    driver.get("https://twitter.com")
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

    COUNTER = 1
    for user in FLAGGED_USERS:
        delay = random.randint(200, 3000)
        delay /= 1000
        print("delay is {}".format(delay))
        time.sleep(delay)
        scrape_flagged_user(driver, user, COUNTER)
        COUNTER += 1
    
    
    write_html_to_file("KEYWORDS")
    write_html_to_file("FLAGGED_USERS")

    #Clear log file, write final summary
    log_file = open("./Program Data/Logs/TW_SCRAPE_LOGS/log.txt", "w")
    log_file.write("TOTAL FLAGGED POSTS: {}\nSCAN COMPLETE".format(TOTAL_POSTS))
    log_file.close()

if __name__ == "__main__":
    main()


