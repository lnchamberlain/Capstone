# coding=utf-8

# UnderCover Recovery 
# Windows Desktop Application for scraping and storing keywords and flagged users from social media sites 
# Developed by Tyler and Logan 
# UAA CSCE Capstone Project Fall 2022
# Script to scrape Facebook data using region csv selection and keywords list
# Format for use: python3 scrape_fb.py REGION_SELECTION (int 1-6) OUTPUT FOLDER (CAN PUT DEFAULT)
# To use with test regions (much smaller): python3 scrape_fb.py 6 DEFAULT

import datetime
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
import authenticator



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
HTML_CODE_KEYWORDS = []
HTML_CODE_FLAGGED_USERS = []
SCAN_NAME = ''
NUM_LOCATIONS = 0
NOT_DEFAULT_DIR = False


#Fills global variable with urls from the last column of the .csv file
def get_urls():
    #Resolve region number into path
    region_file = REGION_RESOLUTION_TABLE[int(sys.argv[1])]
    with open(region_file, newline='', encoding="utf-8") as csvfile:
        csv_data = csv.reader(csvfile, delimiter=',')
        for row in csv_data:
            LOCATION_URLS[row[1]] = row[-1]
    #First pair of values are header values
    global NUM_LOCATIONS
    NUM_LOCATIONS = len(LOCATION_URLS)
    LOCATION_URLS.pop("Location Name", None)



#Fills global variable with value from wordlist
def get_keywords():
    keywords_file = open("./Program Data/Wordlists/keywords.txt", "r+")
    global KEYWORDS
    lines = keywords_file.readlines()[7:]
    KEYWORDS = lines[0].split(",")
    #Advanced search only support for IG
    for elem in KEYWORDS:
        if("BEFORE" in elem) or ("AFTER" in elem) or ("+" in elem):
            KEYWORDS.remove(elem)
 
    

#Grabs cookie value from AUTH logs, rebuilds dictionary and sets global variable
def get_cookie():
    fb_auth_log_file = open("./Program Data/Logs/FB_AUTH_LOGS/log.txt")
    global COOKIE
    cookies = pickle.load(open("./Program Data/Logs/FB_AUTH_LOGS/fb_cookies.pkl", "rb"))
    COOKIE = cookies

#Establishes the output directory
def get_output_dir():
    output_dir = sys.argv[2]
    global OUTPUT_DIR
    global NOT_DEFAULT_DIR 
    if(output_dir == "DEFAULT"):
        OUTPUT_DIR = "./Program Data/FoundPosts/FoundPostsFB"
    else:
        NOT_DEFAULT_DIR = True
        OUTPUT_DIR = output_dir
        shutil.copy("./styles.css", output_dir + "/styles.css")

#Populates global list from flagged users list
def get_flagged_users():
    flagged_users_file = open("./Program Data/FlaggedUsers/FBFlaggedUsers/fb_flagged_users.txt", "r+")
    global FLAGGED_USERS
    FLAGGED_USERS = flagged_users_file.read().split(",")



def reauth():
    '''For when a 401 error is hit, use authenticator program to get new cookie, returns True on success'''
    config_file = open("./Program Data/Configuration/user_config.txt").read()
    l = config_file.split("\n")
    config_file.close()
    email_enc = l[1]
    pass_enc = l[2]
    email = ''
    password = ''
    for c in email_enc:
        email += chr(ord(c) - 10)
    for c in pass_enc:
        password += chr(ord(c) - 10)
    auth = authenticator.FB_AUTH(email, password)
    return auth.attempt_login()

 #Requests data from location url, formats the return, searches captions and comments for keywords, adds posts to flagged posts
def scrape_location(driver, location, counter):
    #open temporary file to write to
    temp_file = open("./Program Data/Logs/FB_SCRAPE_LOGS/temp.txt", "w", encoding="utf-8")
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

    for word in KEYWORDS:
        url_base = LOCATION_URLS[location].split("word")
        #add keyword to crafted url
        url = url_base[0] + word + url_base[1]
        driver.get(url)
        time.sleep(0.2)
        if("We didn't find any results" in driver.page_source):
                print("FOUND POSTS: 0")
                print("TOTAL FOUND POSTS: {}\n".format(TOTAL_POSTS))
                print("\n*****************************************************************\n") 
                return
        if("log in" in driver.title):
            temp_file.write("Permissions Error\nAttempting reauthenticating...\n")
            success = reauth()
            if(success):
                temp_file.write("Successfully reauthenticated\n")
            else:
                temp_file.write("Reauth Fail\n")
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
            print("Scroll count:{}\n".format(SCROLL_COUNT))
            driver.execute_script(SCROLL_DOWN_SCRIPT)
            SCROLL_COUNT +=1
            SCROLL_PAUSE = random.randint(500, 2000)
            SCROLL_PAUSE /= 1000
            time.sleep(SCROLL_PAUSE)
            curr_height = driver.execute_script(GET_CURRENT_SCROLL_HEIGHT)
            if curr_height == prev_height:
                break 
            prev_height = curr_height
    temp_file.write("Scrolled {} times\n".format(SCROLL_COUNT))
    s = BeautifulSoup(driver.page_source, 'html.parser')
   # DELETE_THIS = open("./FB_AUTOPSY.html", "w", encoding="utf-8")
    #ELETE_THIS.write(s.prettify())
    #DELETE_THIS.close()
    
    l.append(s.find_all("div", {"class": "x1yztbdb x1n2onr6 xh8yej3 x1ja2u2z"})) #NEW: x1yztbdb x1n2onr6 xh8yej3 x1ja2u2z OLD: x1ja2u2z xh8yej3 x1n2onr6 x1yztbdb
    print("L has {} posts ".format(len(l)))
    for segment in l:
        for post in segment:
            posts.append(post)
    print("FOUND POSTS: {}".format(len(posts)))
    temp_file.write("Scraped Posts: {}\n".format(len(posts)))
    TOTAL_POSTS += len(posts)
    print("TOTAL FOUND POSTS: {}\n".format(TOTAL_POSTS))
    temp_file.write(("Total Posts: {}\n".format(TOTAL_POSTS)))
    temp_file.close()
    shutil.copy("./Program Data/Logs/FB_SCRAPE_LOGS/temp.txt", "./Program Data/Logs/FB_SCRAPE_LOGS/log.txt")
        #Get rid of temporary file
    os.remove("./Program Data/Logs/FB_SCRAPE_LOGS/temp.txt")
    #posts is an array of beautifulsoup objects, one per post 
    for post in posts:
       format_found_post(post, driver, "KEYWORDS")
       
    print("\n*****************************************************************\n")





     #Requests data from location url, formats the return, searches captions and comments for keywords, adds posts to flagged posts
def scrape_flagged_user(driver, username, counter):
    #open temporary file to write to
    temp_file = open("./Program Data/Logs/FB_SCRAPE_LOGS/temp.txt", "w", encoding="utf-8")
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
    url = "https://www.facebook.com/" + username.strip()
    print(url)
    time.sleep(1)
    driver.get(url)
    try:
            WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.CLASS_NAME, 'x9f619 x1n2onr6 x1ja2u2z xdt5ytf x193iq5w xeuugli x1r8uery x1iyjqo2 xs83m0k x78zum5 x1t2pt76')))
    except TimeoutException:
            print("Advacing to scrolling")
    prev_height = driver.execute_script(GET_CURRENT_SCROLL_HEIGHT)
    #No results case
    if("This Page Isn't Available" in driver.page_source):
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
    l.append(s.find_all("div", {"class": "x1yztbdb x1n2onr6 xh8yej3 x1ja2u2z"}))
    for segment in l:
        for post in segment:
            posts.append(post)
    print("FOUND POSTS: {}".format(len(posts)))
    temp_file.write("Scraped Posts: {}\n".format(len(posts)))
    TOTAL_POSTS += len(posts)
    print("TOTAL FOUND POSTS: {}\n".format(TOTAL_POSTS))
    temp_file.write(("Total Posts: {}\n".format(TOTAL_POSTS)))
    temp_file.close()
    shutil.copy("./Program Data/Logs/FB_SCRAPE_LOGS/temp.txt", "./Program Data/Logs/FB_SCRAPE_LOGS/log.txt")
        #Get rid of temporary file
    os.remove("./Program Data/Logs/FB_SCRAPE_LOGS/temp.txt")
    #posts is an array of beautifulsoup objects, one per post 
    for post in posts:
       format_found_post(post, driver, "FLAGGED_USERS")
       
    print("\n*****************************************************************\n")




#Requests the full info for a flagged post
def format_found_post(flagged_post, driver, mode):
    #GRAB THE VALUES WE WANT FROM THE FLAGGED POST, add to array of html code
    global HTML_CODE
    AUTHOR = ''
    LOCATION = ''
    TIMESTAMP = ''
    CAPTION = ''
    POST_LINK = ''
    ACCOUNT_LINK = ''
    IMG_PATH_HTML = ''
    img_hash = ''
    html_str = ''
    TYPE = None
    links = flagged_post.find_all("a", {"class":"x1i10hfl xjbqb8w x6umtig x1b1mbwd xaqea5y xav7gou x9f619 x1ypdohk xt0psk2 xe8uvvx xdj266r x11i5rnm xat24cr x1mh8g0r xexx8yu x4uap5 x18d9i69 xkhd6sd x16tdsg8 x1hl2dhg xggy1nq x1a2a7pz xt0b8zv xzsf02u x1s688f"})
    if(len(links)>0):
        AUTHOR = links[0].get_text()
        account_tag = str(links[0])
        href_index = account_tag.find(" href=")
        href_end = account_tag.find("?", href_index)
        ACCOUNT_LINK = account_tag[(href_index + 7):href_end]
        LOCATION = links[-1].get_text()
        times = flagged_post.find_all("span", {"id": "jsc_c_k9"})
    if(len(times) > 1):
        timestamp_text = times[0].get_text()
    else:
        timestamp_text = ''
    timestamp_obj, unit = convert_timestamp_text(timestamp_text)
    if(unit == 'h' or unit == 'm'):
        TIMESTAMP = timestamp_obj.strftime("%m/%d/%Y %H:%M")
    if(unit == 'no_formatting'):
        TIMESTAMP = timestamp_obj
    if(unit == 'd'):
        TIMESTAMP = timestamp_obj.strftime("%m/%d/%Y")
    captions = flagged_post.find_all("span", {"class":"x193iq5w xeuugli x13faqbe x1vvkbs x1xmvt09 x1lliihq x1s928wv xhkezso x1gmr53x x1cpjm7i x1fgarty x1943h6x xudqn12 x3x7a5m x6prxxf xvq8zen xo1l8bm xzsf02u x1yc453h"})
    if(len(captions)>0):
        CAPTION = captions[0].get_text()
   #While we figure out how to split them, grab links for only posts with a single image
    img_arr = flagged_post.find_all("img", {"class": "x1ey2m1c xds687c x5yr21d x10l6tqk x17qophe x13vifvy xh8yej3 xl1xv1r"})
    if(len(img_arr)>0):
        img_tag = str(img_arr[0])
        src_index = img_tag.find(" src=")
        src_end = img_tag.find('width=', src_index)    
        img_url = img_tag[(src_index + 6):src_end -2]
        img_url = img_url.replace("stp=dst-jpg_p843x403&amp;", "")
        img_url = img_url.replace("amp;", "")
        driver.get(img_url)
        time.sleep(0.25)
        if("URL signature mismatch" in driver.page_source):
            IMG_PATH_HTML = ''
        else:
            driver.save_screenshot("./Program Data/temp_img.png")
            img_hash = hashlib.md5(Image.open("./Program Data/temp_img.png").tobytes())
            hash_str = img_hash.hexdigest()
            img_path = "./Program Data/Images/ImagesFB/" + hash_str + ".png"
            IMG_PATH_HTML = str("../../../Program Data/Images/ImagesFB/"+ hash_str + ".png")
            if(NOT_DEFAULT_DIR):
                path = OUTPUT_DIR + "/ImagesFB/"
                if(not os.path.exists(path)):
                    os.makedirs(path)
                img_path = path + hash_str + ".png"
                IMG_PATH_HTML = path + hash_str + ".png"
            if(not os.path.exists(img_path)):
                shutil.copy("./Program Data/temp_img.png", img_path)
            #Get rid of temporary image
            os.remove("./Program Data/temp_img.png")
         
    else:
        IMG_PATH_HTML = 'MULTIPLE IMAGES OR VIDEO'    
    TYPE = "Image"
    if(TYPE == "Image"):
        post_links = flagged_post.find_all("a", {"class":"x1i10hfl x1qjc9v5 xjbqb8w xjqpnuy xa49m3k xqeqjp1 x2hbi6w x13fuv20 xu3j5b3 x1q0q8m5 x26u7qi x972fbf xcfux6l x1qhh985 xm0m39n x9f619 x1ypdohk xdl72j9 x2lah0s xe8uvvx xdj266r x11i5rnm xat24cr x1mh8g0r x2lwn1j xeuugli xexx8yu x4uap5 x18d9i69 xkhd6sd x1n2onr6 x16tdsg8 x1hl2dhg xggy1nq x1ja2u2z x1t137rt x1o1ewxj x3x9cwd x1e5q0jg x13rtm0m x1q0g3np x87ps6o x1lku1pv x1a2a7pz x1lliihq x1pdlv7q"})
        if(len(post_links) > 0):
            post_links_str = str(post_links[0])
            href_index = post_links_str.find(" href=")
            href_end = post_links_str.find("&", href_index)
            POST_LINK = post_links_str[(href_index + 7):href_end]
            print("AUTHOR: {}\nLOCATION: {}\nTIMESTAMP: {}\nCAPTION: {}\nLINK: {}\nACCOUNT_LINK: {}\nMEDIA_LINK: {}\n".format(AUTHOR, LOCATION, TIMESTAMP, CAPTION, POST_LINK, ACCOUNT_LINK, IMG_PATH_HTML))
            html_str += "<td>" + AUTHOR + "</td><td>" + LOCATION + "</td><td>" + CAPTION + "</td><td><a href=" + POST_LINK + ">link</a><td><a href=" + ACCOUNT_LINK + ">link</a><td><img style='max-width:200px;' src='" + IMG_PATH_HTML + "'></td></tr>"
            if(mode == "KEYWORDS"):
                HTML_CODE_KEYWORDS.append(html_str)
            if(mode == "FLAGGED_USERS"):
                HTML_CODE_FLAGGED_USERS.append(html_str)
        else:
             POST_LINK = "MULTIPLE IMAGES OR VIDEO"
             print("AUTHOR: {}\nLOCATION: {}\nTIMESTAMP: {}\nCAPTION: {}\nLINK: {}\nACCOUNT_LINK: {}\nMEDIA_LINK: {}\n".format(AUTHOR, LOCATION, TIMESTAMP, CAPTION, POST_LINK, ACCOUNT_LINK, IMG_PATH_HTML))
             html_str += "<td>" + AUTHOR + "</td><td>" + LOCATION + "</td><td>" + CAPTION + "</td><td>" + '' + "</td><td><a href=" + ACCOUNT_LINK + ">link</a><td>" + POST_LINK + "</td></tr>"
             if(mode == "KEYWORDS"):
                HTML_CODE_KEYWORDS.append(html_str)
             if(mode == "FLAGGED_USERS"):
                HTML_CODE_FLAGGED_USERS.append(html_str)



#Takes in a time string and returns a datetime object (example: '1d' -> datetime for yesterday)
def convert_timestamp_text(text):
    if(len(text)>1):
        unit = text[-1]
    else:
        return text, "no_formatting"
    #Posts older than a few days will be listed as a date with a time or just a date, in that case return the given string
    if(unit == 's'):
        number = float(text[:-1])
        final_time = datetime.datetime.now() - datetime.timedelta(seconds=number)
    elif(unit == 'm'):
        number = float(text[:-1])
        final_time = datetime.datetime.now() - datetime.timedelta(minutes=number)
    elif(unit == 'h'):
        number = float(text[:-1])
        final_time = datetime.datetime.now() - datetime.timedelta(hours=number)
    elif(unit == 'd'):
        number = float(text[:-1])
        final_time = datetime.datetime.now() - datetime.timedelta(weeks=(number / 7))
    else:
        unit = 'no_formatting'
        final_time = text
    return final_time, unit




#Iterate over array of lines of html code, write to scan output file
def write_html_to_file(mode):
    
    #Fill in the table header and footer of the html document
    if(mode == "KEYWORDS"):
        output_file = open(OUTPUT_DIR + "/" + SCAN_NAME_KEYWORDS + ".html", 'w+', encoding="utf-8")
        global HTML_CODE_KEYWORDS
    
        #Clear out duplicate entries
        HTML_CODE_KEYWORDS = set(HTML_CODE_KEYWORDS)
        HTML_CODE_KEYWORDS = list(HTML_CODE_KEYWORDS)
        if(NOT_DEFAULT_DIR):
            HTML_CODE_KEYWORDS.insert(0, "<html><body><table><head><link rel='stylesheet' href='./styles.css'></head>\n")
        else:
            HTML_CODE_KEYWORDS.insert(0, "<html><body><table><head><link rel='stylesheet' href='../../../styles.css'></head>\n")
        HTML_CODE_KEYWORDS.insert(0, "<html><head><link rel='stylesheet' href='../../../styles.css'></head><body><table>\n")
        HTML_CODE_KEYWORDS.insert(1, "<h1 style='text-align:center;'>" + SCAN_NAME_KEYWORDS + "</h1>")
        HTML_CODE_KEYWORDS.insert(2, "<tr><th>Post Author</th><th>Location</th><th>Caption</th><th>Post Link</th><th>Account Link</th><th>Media</th></tr>")
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
        HTML_CODE_FLAGGED_USERS.insert(2, "<tr><th>Post Author</th><th>Location</th><th>Caption</th><th>Post Link</th><th>Account Link</th><th>Media</th></tr>")
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
    SCAN_NAME_KEYWORDS = "FB KEYWORDS REPORT " + date_and_time_formatted
    SCAN_NAME_FLAGGED = "FB FLAGGED USERS REPORT " + date_and_time_formatted
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
    log_file = open("./Program Data/Logs/FB_SCRAPE_LOGS/log.txt", "w")
    log_file.write("TOTAL FLAGGED POSTS: {}\nSCAN COMPLETE".format(TOTAL_POSTS))
    log_file.close()

if __name__ == "__main__":
    main()


