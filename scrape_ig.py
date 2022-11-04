# UnderCover Recovery 
# Windows Desktop Application for scraping and storing keywords and flagged users from social media sites 
# Developed by Tyler and Logan 
# UAA CSCE Capstone Project Fall 2022
# Script to scrape Instagram data using region csv selection and keywords list
# Format for use: python3 scrape_ig.py REGION_SELECTION (int 1-6) OUTPUT FOLDER (CAN PUT DEFAULT)
# To use with test region (much smaller): python3 scrape_ig.py 6 DEFAULT

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
import pickle



#Initalization of global variables
REGION_RESOLUTION_TABLE = {1:"./Program Data/Regions/ALASKA_IG.csv", 2:"./Program Data/Regions/ANCHORAGE_IG.csv", 3:"./Program Data/Regions/BETHEL_IG.csv", 4:"./Program Data/Regions/FAIRBANKS_IG.csv", 5:"./Program Data/Regions/JUNEAU_IG.csv", 6:"./Program Data/Regions/TESTING_IG.csv"}
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
AND_FLAG = False
AND_LIST = []
BEFORE_FLAG = False
BEFORE_LIST = []
AFTER_FLAG = False
AFTER_LIST = []


#Fills global variable with urls from the last column of the .csv file
def get_urls():
    global LOCATION_URLS
    #Resolve region number into relative path
    region_file = REGION_RESOLUTION_TABLE[int(sys.argv[1])]
    with open(region_file, newline='', encoding="utf-8") as csvfile:
        csv_data = csv.reader(csvfile, delimiter=',')
        for row in csv_data:
            LOCATION_URLS[row[1]] = row[-1]
    #First pair of values are header values
    LOCATION_URLS.pop("Location Name", None)



#Fills global variable with value from wordlist. Offers logical operator AND and date limiting functionality through the following:
#   To AND keywords together, users may put in keyword file "wordone+wordtwo"
#   To look for words only after a certain date, users may put in keyword file "word AFTER MM/DD/YYYY"
#   To look for words only before a certain date, users may put in keyword file "word BEFORE MM/DD/YYYY" 
def get_keywords():
    global KEYWORDS
    global AND_FLAG
    global BEFORE_FLAG
    global AFTER_FLAG
    keywords_file = open("./Program Data/Wordlists/keywords.txt", "r+")
    #First 7 lines of keyword file are user instructions, skip to grab terms
    lines = keywords_file.readlines()[7:]
    keywords_list = lines[0].split(",")
    for word in keywords_list:
        word=word.strip()
        #If logical AND used, add to array of lists 
        if '+' in word:
            AND_FLAG = True
            terms = word.split("+")
            global AND_LIST
            AND_LIST.append(terms)
            continue
        #If after date filter used, add to array of tuples in the form (term, epoch_time_UTC)
        if 'AFTER' in word:
            AFTER_FLAG = True
            term_and_date = word.split("AFTER")
            term = term_and_date[0]
            date = term_and_date[1].split("/")
            #datetime takes arguments (year, month, day, hour, minute), timestamp converts to seconds since epoch
            datetime_obj = datetime.datetime(int(date[2]), int(date[0]), int(date[1]), 0, 0)
            epoch_time = datetime_obj.timestamp()
            global AFTER_LIST 
            AFTER_LIST.append((term, epoch_time))
            continue
        #If before date filter used, add to array of tuples in the form (term, epoch_time_UTC)
        print(word)
        if'BEFORE' in word:
            BEFORE_FLAG = True
            term_and_date = word.split("BEFORE")
            term = term_and_date[0]
            date = term_and_date[1].split("/")
            #datetime takes arguments (year, month, day, hour, minute), timestamp converts to seconds since epoch
            datetime_obj = datetime.datetime(int(date[2]), int(date[0]), int(date[1]), 0, 0)
            epoch_time = datetime_obj.timestamp()
            global BEFORE_LIST 
            BEFORE_LIST.append((term, epoch_time))
            continue
    #Add all non-special terms to KEYWORDS list
        KEYWORDS.append(word)


    

#Reads cookie value from pickle file, sets session cookies then returns session
#   Cookie value is written to pickle file from the authenticator program, users can't move on to 
#   the scraper until that value is set
def get_cookie():
    session = requests.Session()
    cookies = pickle.load(open("./Program Data/Logs/IG_AUTH_LOGS/ig_cookies.pkl", "rb"))
    c = cookies[0]
    session.cookies.set(c['name'], c['value'])
    return session



#Establishes if users are using a custom output directory, and if not set the global variable OUTPUT_DIR to the default output
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
    #open temporary file to write to
    temp_file = open("./Program Data/Logs/IG_SCRAPE_LOGS/temp.txt", "w", encoding="utf-8")
    ALL_POSTS = []
    MEDIA_ARRAYS =[]
    print("\n*****************************************************************\n")
    print("Scraping Location {}...".format(location))
    temp_file.write(location + "\n")
    print("Location {}/{}".format(COUNTER, NUM_LOCATIONS))
    temp_file.write("Location {}/{}\n".format(COUNTER, NUM_LOCATIONS))
    response = session.get(LOCATION_URLS[location] + "/?__a=1")
    if(response.status_code != 200):
        print("Error")
        print(response.status_code)
        return
    print("Response Status is {}".format(response))
    temp_file.write("Response Status is {}\n".format(response.status_code))
    response_dict = json.loads(response.content)
    sections = response_dict["native_location_data"]["recent"]["sections"]
    for i in range(len(sections)):
        try:
            MEDIA_ARRAYS.append(sections[i]["layout_content"]["medias"])
        except KeyError:
            continue
    for array in MEDIA_ARRAYS:
        for post in array:
            ALL_POSTS.append(post["media"])
    
    print("Number of found posts: {}\n".format(len(ALL_POSTS)))
    temp_file.write("Scraped Posts: {}\n".format(len(ALL_POSTS)))
    global TOTAL_POSTS
    global FLAGGED_POSTS
    TOTAL_POSTS += len(ALL_POSTS)
    print("Total posts searched: {}".format(TOTAL_POSTS))
    temp_file.write(("Total Posts: {}\n".format(TOTAL_POSTS)))
    flagged = 0
    for post in ALL_POSTS:
        if(post["caption"] is not None):
            caption = post["caption"]["text"]
        else:
            caption = " "
        user = post["user"]["username"]

        #Check if post contains KEYWORDS or was authored by a FLAGGED USER
        for word in KEYWORDS:
            search_words = [(" " + word + " "), (" " + word + "."), (" " + word + "!"), (" " + word + "?"), ("#" + word + " ")]
            for author in FLAGGED_USERS:
                for word in search_words:
                    if (word in caption) or (user == author): 
                        FLAGGED_POSTS.append(post)
                        format_found_post(post)
                        flagged +=1
        
        #If using AND operator, caption must contain all in list
        if(AND_FLAG):
            for group in AND_LIST:
                flagged = False
                match = []
                for word in group:
                    variations = [(" " + word + " "), (" " + word + "."), (" " + word + "!"), (" " + word + "?"), ("#" + word + " "), (word + " ")]
                    for var in variations:
                        if var in caption:
                            match.append(True)
                            #only have to match one variation
                            break
                
                #If at least one variation is met for each term in group, post is flagged
                if(len(match) == len(group)):
                    FLAGGED_POSTS.append(post)
                    format_found_post(post)
                    flagged +=1
     
     
        #If using BEFORE operator, caption must contain term and be posted before the time stamp
        if(BEFORE_FLAG):
            post_epoch = post.get("taken_at")
            if post_epoch is not None:
                post_epoch = int(post_epoch)
            else:
                #Can't compare posts that don't have a timestamp
                continue
            for term_and_date in BEFORE_LIST:
                term = term_and_date[0]
                epoch_time = term_and_date[1]
                term = term.strip()
                search_words = [(" " + term + " "), (" " + term + "."), (" " + term + "!"), (" " + term + "?"), ("#" + term + " ")]
                for word in search_words:
                    if((word in caption) and (int(epoch_time) > post_epoch)):
                        FLAGGED_POSTS.append(post)
                        format_found_post(post)
                        flagged +=1

        #If using AFTER operator, caption must contain term and be posted after the time stamp
        if(AFTER_FLAG):
            post_epoch = post.get("taken_at")
            if post_epoch is not None:
                post_epoch = int(post_epoch)
            else:
                #Can't compare posts that don't have a timestamp
                continue
            for term_and_date in AFTER_LIST:
                term = term_and_date[0]
                epoch_time = term_and_date[1]
                term = term.strip()
                search_words = [(" " + term + " "), (" " + term + "."), (" " + term + "!"), (" " + term + "?"), ("#" + term + " ")]
                for word in search_words:
                    if((word in caption) and (int(epoch_time) < post_epoch)):
                        FLAGGED_POSTS.append(post)
                        format_found_post(post)
                        flagged +=1
          
                        

    global FOUND_FLAGGED 
    FOUND_FLAGGED += flagged
    print("Found {} flagged posts at this location\nTotal Flagged Posts: {}".format(flagged, len(FLAGGED_POSTS)))
    temp_file.write("Flagged Posts: {}\nTotal Flagged Posts: {}\n".format(flagged, len(FLAGGED_POSTS)))
    #copy temp file into log file
    temp_file.close()
    shutil.copy("./Program Data/Logs/IG_SCRAPE_LOGS/temp.txt", "./Program Data/Logs/IG_SCRAPE_LOGS/log.txt")
        #Get rid of temporary image
    os.remove("./Program Data/Logs/IG_SCRAPE_LOGS/temp.txt")
    print("\n*****************************************************************\n")
   



#Requests the full info for a flagged post
def format_found_post(flagged_post):
    #GRAB THE VALUES WE WANT FROM THE FLAGGED POST, add to array of html code
    global HTML_CODE
    lat = flagged_post.get("lat")
    lng = flagged_post.get("lng")
    lat_lng = str(lat) + ", " + str(lng)
    html_str = '<tr>'
    username = flagged_post["user"]["username"]
    full_name = flagged_post["user"]["full_name"]
    caption = ''
    if(flagged_post["caption"] is not None):
        caption = flagged_post["caption"]["text"] 
    timestamp_epoch = flagged_post.get("taken_at")
    timestamp = datetime.datetime.utcfromtimestamp(timestamp_epoch)
    profile_link = "https://instagram.com/" + username 
    link = "https://www.instagram.com/p/" + flagged_post.get("code")
    try:
        img_url = flagged_post["image_versions2"]["candidates"][0]["url"] 
        #Retrieve image and store temporarily, compute hash and store in images
        urlretrieve(img_url, "./Program Data/temp_img.jpg")
        img_hash = hashlib.md5(Image.open("./Program Data/temp_img.jpg").tobytes())
        hash_str = img_hash.hexdigest()
        img_path = "./Program Data/Images/ImagesIG/" + hash_str + ".jpg"
        img_path_html = str("../../../Program Data/Images/ImagesIG/"+ hash_str + ".jpg")
        #Save if novel image
        if(not os.path.exists(img_path)):
            shutil.copy("./Program Data/temp_img.jpg", img_path)
        #Get rid of temporary image
        os.remove("./Program Data/temp_img.jpg")
    except KeyError:
        img_path_html = ""
    #Compile into HTML string
    print("Writing HTML String")
    if(img_path_html == ""):
        html_str += "<td>" + timestamp.strftime("%m/%d/%Y %H:%M:%S") + "</td><td>" + lat_lng + "</td><td>" + username + "</td><td>" + full_name + "</td><td><a href=" + profile_link + ">link</a></td><td>" + caption + "</td><td><a href=" + link + ">link</a></td><td>Multiple Images or Video</td></tr>"
    else:
        html_str += "<td>" + timestamp.strftime("%m/%d/%Y %H:%M:%S") + "</td><td>" + lat_lng + "</td><td>" + username + "</td><td>" + full_name + "</td><td><a href=" + profile_link + ">link</a></td><td>" + caption + "</td><td><a href=" + link + ">link</a></td><td><img style='max-width:200px;' src='" + img_path_html + "'></td></tr>"
    HTML_CODE.append(html_str)
    

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
    HTML_CODE.insert(2, "<tr><th>Date/Time</th><th>Lat/Long</th><th>Username</th><th>Full Name</th><th>Profile</th><th>Caption/Comment</th><th>Post</th><th>Media</th></tr>")
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
    global SCAN_NAME 
    currTime = datetime.datetime.now()
    date_and_time_formatted = MONTH_RESOLUTION_TABLE[currTime.month] + " "+str(currTime.day) +" "+ str(currTime.year) + " @ " + str(currTime.hour) + "." + str(currTime.minute) + "." + str(currTime.second) 
    SCAN_NAME = "IG SCAN REPORT " + date_and_time_formatted
    #Opening and then closing the log with in write mode clears the previous scan log 
    clear_log = open("./Program Data/Logs/IG_SCRAPE_LOGS/log.txt", "w", encoding="utf-8")
    clear_log.close()
    COUNTER = 1
    NUM_LOCATIONS = len(LOCATION_URLS)
    #Each location in LOCATION_URLS is scraped and checked for keywords and logical conditions, flaggged post information is written into an array of HTML strings
    for place in LOCATION_URLS:
        #Random Latency to avoid being flagged as a bot
        delay = random.randint(200, 3000)
        delay /= 1000
        print("delay is {}".format(delay))
        time.sleep(delay)
        scrape_location(COUNTER, NUM_LOCATIONS, session, place)
        COUNTER += 1
    
    #All the flagged post information has been formatted and is in array of HTML strings, write to report file
    write_html_to_file()
    #Write final summary to be displayed in GUI
    log_file = open("./Program Data/Logs/IG_SCRAPE_LOGS/log.txt", "w")
    log_file.write("TOTAL SCRAPED POSTS: {}\nTOTAL FLAGGED POSTS: {}\nSCAN COMPLETE".format(TOTAL_POSTS, FOUND_FLAGGED))
    log_file.close()
    session.close()



if __name__ == "__main__":
    main()

