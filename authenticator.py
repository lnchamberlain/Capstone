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
import time

#Simple encryption, adds 10 to each character value, writes it into the right place
def encrypt_and_store(auth, mode):
    enc_username = ''
    enc_password = ''
    for char in auth.username:
        enc_username += chr(ord(char) + 10)
    for char in auth.password:
        enc_password += chr(ord(char) + 10)
    
    #open the file in read mode
    f = open("./Program Data/Configuration/user_config.txt", 'r+')
    data = f.read().split("\n")
    f.close()
    if(mode == "FB"):
        data[1] = enc_username
        data[2] = enc_password
    if(mode == "IG"):
        data[4] = enc_username
        data[5] = enc_password
    if(mode == "TW"):
        data[7] = enc_username
        data[8] = enc_password
    #clear out old data and assemble new string
    cleared_file = open("./Program Data/Configuration/user_config.txt", "w")
    new_data = ''
    for item in data:
        new_data += item + "\n"
    cleared_file.write(new_data)
    cleared_file.close()
    
    log_file = open("./Program Data/Logs/IG_AUTH_LOGS/log.txt", "w")
    #write each element of the cookie to a line, last line must be SUCCESS
    for item in auth.cookie:
        log_file.write("{}:{}\n".format(item, auth.cookie[item]))
    log_file.write("SUCCESS")
    log_file.close()


class FB_AUTH:
    def __init__(self, username, password):
       self.username = username
       self.password = password
       self.cookie = ""
    
    #REFERENCE FOR PROCESS: https://stackoverflow.com/questions/21928368/login-to-facebook-using-python-requests
    
    def attempt_login(self):
        print("This is a stub")


class IG_AUTH:
    def __init__(self, username, password):
       self.username = username
       self.password = password
       self.cookie = ""


    #REFERENCE CODE: https://github.com/softcoder24/insta_share/blob/master/insta_share/instagram.py

    def attempt_login(self):
        base_url = 'https://www.instagram.com/accounts/login'
        login_url = 'https://www.instagram.com/accounts/login/ajax/'
        current_time = time.time()
        #Have to get csrftoken to put into post header
        initial_response = requests.get(base_url)
        csrf = initial_response.cookies['csrftoken']
        print("Inital response is {}".format(initial_response))
        #build payload
        payload = {
            'username': self.username,
            'enc_password': f'#PWD_INSTAGRAM_BROWSER:0:{current_time}:{self.password}',
            'queryParams': {},
            'optIntoOneTap': 'false'       
        } 

        login_header = {
            "User-Agent": "Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko)"
                          " Chrome/77.0.3865.120 Safari/537.36", 
                          "X-Requested-With": "XMLHttpRequest",
                          "Referer": "https://www.instagram.com/accounts/login",
                          "x-csrftoken": csrf       
        }

        login_attempt_response = requests.post(login_url, data=payload, headers=login_header)
        json_data = json.loads(login_attempt_response.text)
        print("Response from login attempt in {}".format(login_attempt_response.text))
        time.sleep(5)
        if json_data["authenticated"]:
            raw_cookies = login_attempt_response.cookies
            self.cookie = raw_cookies.get_dict()
            encrypt_and_store(self, "IG")
        else:
            log_file = open("./Program Data/Logs/IG_AUTH_LOGS/log.txt", "w")
            #log_file.write(login_attempt_response.text)
            #log_file.write("\n")
            log_file.write("FAIL")
            log_file.close()
            return "Fail"        
        return "Success"


class TW_AUTH:
    def __init__(self, username, password):
       self.username = username
       self.password = password
       self.cookie = ""
   

def attempt_fb_login(username, password):
    auth = FB_AUTH(username, password)
   
def attempt_ig_login(username, password):
    #Clear previous log data first by opening in write mode
    #log_file = open("./Program Data/Logs/IG_AUTH_LOGS/log.txt", "w")
    auth = IG_AUTH(username, password)
    attempt = auth.attempt_login()
    if(attempt != "Success"):
        return 0
    else:
        return 1

def attempt_tw_login(username, password):
    auth = TW_AUTH(username, password)


# Entry point, grab argv values and pass to proper function. 
if __name__ == "__main__":
    #takes in a mode, username, and password in argv
    #mode can be 31 (FB) 32 (IG) or 33 (TW) 
    #print(sys.argv)
    if(len(sys.argv) > 4):
        #Send an error to the c++ program 
        sys.exit()
   
    mode_id = sys.argv[1]
    username = sys.argv[2]
    password = sys.argv[3]
    if(mode_id == 'FB'):
        attempt_fb_login(username, password)
    if(mode_id == 'IG'):
        success = attempt_ig_login(username, password)
    if(mode_id == 'TW'):
        attempt_tw_login(username, password)




