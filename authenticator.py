# UnderCover Recovery 
# Windows Desktop Application for scraping and storing keywords and flagged users from social media sites 
# Developed by Tyler and Logan 
# UAA CSCE Capstone Project Fall 2022
# Script to post login requests to social media sites, writes cookie values to log file
#
import requests
import sys
import datetime
import json
import time
import random
import selenium
from selenium.webdriver.common.by import By
from selenium.webdriver.chrome.options import Options
import pickle

#Simple encryption, adds 10 to each character value, writes it into the right place
def encrypt_and_store(auth, mode):
    enc_username = ''
    enc_password = ''
    for char in auth.username:
        enc_username += chr(ord(char) + 10)
    for char in auth.password:
        enc_password += chr(ord(char) + 10)
    
    print(auth.username)
    print(enc_username)
    #open the file in read mode
    f = open("./Program Data/Configuration/user_config.txt", 'r+', encoding="utf-8")
    data = f.read().split("\n")
    #print(data)
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
    cleared_file = open("./Program Data/Configuration/user_config.txt", "w", encoding="utf-8")
    new_data = ''
    for item in data:
        new_data += item + "\n"
    cleared_file.write(new_data)
    cleared_file.close()
   


class FB_AUTH:
    def __init__(self, email, password):
       self.username = email
       self.password = password
       self.cookie = ""
    
    #Log in using Selenium browser emulation
    def attempt_login(self):
        #opening the file in write mode clears the previous login attempt
        clear_log_file = open("./Program Data/Logs/FB_AUTH_LOGS/log.txt", "w")
        clear_log_file.close()
        chrome_options = Options()
        #--headless makes the window not pop up
        chrome_options.add_argument("--headless")
        driver = selenium.webdriver.Chrome("./chromedriver", options=chrome_options)
        driver.get("https://facebook.com")
        print("FB opened")
        time.sleep(1)
        print(driver.title)

        email_form = driver.find_element(By.ID,'email')
        password_form = driver.find_element(By.ID, 'pass')
        #Fill forms
        email_form.send_keys(self.username)
        time.sleep(1)
        print("Entered email")
        password_form.send_keys(self.password)
        time.sleep(1)
        print("Entered Password")
        submit_form = driver.find_element(By.NAME, "login")
        time.sleep(1)
        submit_form.click()
        print("Submitted")
        time.sleep(7)
        log_file = open("./Program Data/Logs/FB_AUTH_LOGS/log.txt", "w")
        #Title will change to Facebook if logged in
        if("log in" not in driver.title and "Log into" not in driver.title):
            self.cookie = driver.get_cookies()
            if self.cookie is not None:
                print("Log in success")
                pickle.dump(self.cookie, open("./Program Data/Logs/FB_AUTH_LOGS/fb_cookies.pkl", "wb"))
                log_file.write("SUCCESS")
                log_file.close()
                driver.quit()
                encrypt_and_store(self, "FB")
                print("DONE")
                return "Success"
        else:
            print("Log in fail")
            print("writing to log file")
            log_file.write("FAIL")
            log_file.close()
            driver.quit()
            print("DONE")
            return "Fail"
        
   
  
        

class IG_AUTH:
    def __init__(self, username, password):
       self.username = username
       self.password = password
       self.cookie = ""


    #REFERENCE CODE: https://github.com/softcoder24/insta_share/blob/master/insta_share/instagram.py

    def attempt_login_selenium(self):
        clear_log_file = open("./Program Data/Logs/IG_AUTH_LOGS/log.txt", "w")
        clear_log_file.close()
        chrome_options = Options()
        #--headless makes the window not pop up
        chrome_options.add_argument("--headless")
        driver = selenium.webdriver.Chrome("./chromedriver", options=chrome_options)
        driver.get("https://instagram.com")
        print("IG opened")
        time.sleep(1)
        username_form = driver.find_element(By.NAME, 'username')
        password_form = driver.find_element(By.NAME, 'password')
        #Fill forms
        username_form.send_keys(self.username)
        time.sleep(1)
        print("Entered username")
        password_form.send_keys(self.password)
        time.sleep(1)
        print("Entered Password")
        submit_form = driver.find_element(By.CLASS_NAME, "_acap")
        time.sleep(1)
        src = driver.page_source
        submit_form.click()
        print("Submitted")
        time.sleep(7)
        log_file = open("./Program Data/Logs/IG_AUTH_LOGS/log.txt", "w")
        #check if page changed
        if(driver.page_source != src):
            print("advanced")
            self.cookie = driver.get_cookies()
            print(self.cookie)
            if self.cookie is not None:
                print("Log in success")
                pickle.dump(self.cookie, open("./Program Data/Logs/IG_AUTH_LOGS/ig_cookies.pkl", "wb"))
                log_file.write("SUCCESS")
                log_file.close()
                driver.quit()
                encrypt_and_store(self, "IG")
                print("DONE")
                return "Success"
        else:
            print("Log in fail")
            log_file.write("FAIL")
            log_file.write("FAIL")
            log_file.close()
            driver.quit()
            print("DONE")
            return "Fail"
    

    #OLD METHOD: requests no longer sending back csrf?? switched to using selenium
    '''
    def attempt_login_requests(self):
        #opening the file in write mode clears the previous login attempt
        clear_log_file = open("./Program Data/Logs/IG_AUTH_LOGS/log.txt", "w")
        clear_log_file.close()
        base_url = 'https://www.instagram.com/accounts/login'
        login_url = 'https://www.instagram.com/accounts/login/ajax/'
        current_time = time.time()
        #Have to get csrftoken to put into post header
        initial_response = requests.get(base_url)
        print(initial_response.content)
        #csrf = initial_response.cookies['csrftoken']
        #print("Inital response is {}".format(initial_response))
        #build payload
        payload = {
            'username': self.username,
            'enc_password': f'#PWD_INSTAGRAM_BROWSER:0:{current_time}:{self.password}',
            'queryParams': {},
            'optIntoOneTap': 'false'       
        } 
        
        with open("UserAgents.txt", "r") as file: #Filename works, full path better??
            line = random.randint(1, 72) #remove hardcoded line value
            userAgent = file.readlines()[line] 
            userAgent = userAgent.replace("\n","")
            
        login_header = {
            "User-Agent": userAgent, 
                          "X-Requested-With": "XMLHttpRequest",
                          "Referer": "https://www.instagram.com/accounts/login",
                          #"x-csrftoken": csrf       
        }
        
        login_attempt_response = requests.post(login_url, data=payload, headers=login_header)
        print(login_attempt_response)
        print(login_attempt_response.content)
        json_data = json.loads(login_attempt_response.text)
        time.sleep(5)
        if json_data["authenticated"]:
            raw_cookies = login_attempt_response.cookies
            self.cookie = json.dumps(raw_cookies.get_dict())
            encrypt_and_store(self, "IG")
            #write cookie to file, last line must be SUCCESS
            log_file = open("./Program Data/Logs/IG_AUTH_LOGS/log.txt", "w", encoding="utf-8")
            log_file.write(self.cookie)
            log_file.write("\n")
            log_file.write("SUCCESS")
            log_file.close()
        else:
            log_file = open("./Program Data/Logs/IG_AUTH_LOGS/log.txt", "w", encoding="utf-8")
            log_file.write("FAIL")
            log_file.close()
            return "Fail"        
        return "Success"
'''

class TW_AUTH:
    def __init__(self, username, password):
       self.username = username
       self.password = password
       self.cookie = ""
   

def attempt_fb_login(email, password):
    auth = FB_AUTH(email, password)
    attempt = auth.attempt_login()
    if(attempt != "Success"):
       return 0 
    else:
        return 1

def attempt_ig_login(username, password):
    #Clear previous log data first by opening in write mode
    auth = IG_AUTH(username, password)
    attempt = auth.attempt_login_selenium()
    if(attempt != "Success"):
        return 0
    else:
        return 1

def attempt_tw_login(username, password):
    auth = TW_AUTH(username, password)


# Entry point, grab argv values and pass to proper function. 
if __name__ == "__main__":
    #takes in a mode, username, and password in argv
    #mode can be FB IG or TW 
    if(len(sys.argv) > 4):
        #close if too few arguments 
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




