# UnderCover Recovery 
# Windows Desktop Application for scraping and storing keywords and flagged users from social media sites 
# Developed by Tyler and Logan 
# UAA CSCE Capstone Project Fall 2022
# Script to post login requests to social media sites, returns cookie values
#
import requests
import sys
import datetime
import json

class FB_AUTH:
    def __init__(self, username, password):
       self.username = username
       self.password = password
       self.cookie = ""


class IG_AUTH:
    def __init__(self, username, password):
       self.username = username
       self.password = password
       self.cookie = ""


    #REFERENCE CODE: https://github.com/softcoder24/insta_share/blob/master/insta_share/instagram.py

    def attempt_login(self):
        base_url = 'https://www.instagram.com/accounts/login'
        login_url = 'https://www.instagram.com/accounts/login/ajax/'

        current_time = int(datetime.now().timestamp())
        #Have to get csrftoken to put into post header
        initial_response = requests.get(base_url)
        csrf = response.cookies['csrftoken']
        
        #build payload
        payload = {
            'username': self.username,
            'enc_password': f'#PWD_INSTAGRAM_BROWSER:0:{current_time}:{self.password}',
            'queryParams': {},
            'optIntoOneTap': 'false'       
        } 

        login_header = {
            "User-Agent": "Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko)"
                          " Chrome/77.0.3865.120 Safari/537.36", #TODO, find how to grab user-specific configs, this is the example User-Agent Value
                          "X-Requested-With": "XMLHttpRequest",
                          "Referer": "https://www.instagram.com/accounts/login",
                          "x-csrftoken": csrf       
        }

        login_attempt_response = requests.post(login_url, data=payload, headers=login_header)
        json_data = json.loads(login_response.text)

        if json_data["authenticated"]:
            self.cookie = login_response.cookies
               


class TW_AUTH:
    def __init__(self, username, password):
       self.username = username
       self.password = password
       self.cookie = ""
   

def attempt_fb_login(username, password):
    auth = FB_AUTH(username, password)
   
def attempt_ig_login(username, password):
    auth = IG_AUTH(username, password)

def attempt_tw_login(username, password):
    auth = TW_AUTH(username, password)


# Entry point, grab argv values and pass to proper function. 
if __name__ == "__main__":
    #takes in a mode, username, and password in argv
    #mode can be 31 (FB) 32 (IG) or 33 (TW) 
    if(len(sys.argv) < 4):
        #Send an error to the c++ program
        return 0
   
    mode_id = sys.argv[1]
    username = sys.argv[2]
    password = sys.argv[3]
    if(mode_id == 31):
        attempt_fb_login(username, password)
    elif(mode_id == 32):
        attempt_ig_login(username, password)
    elif(mode_id == 33):
        attempt_tw_login(username, password)




