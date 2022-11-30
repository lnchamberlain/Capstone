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
from bs4 import BeautifulSoup

#Simple encryption, adds 10 to each character value, writes it into the right place
def encrypt_and_store(auth, mode):
    enc_username = ''
    enc_password = ''
    for char in auth.username:
        enc_username += chr(ord(char) + 10)
    for char in auth.password:
        enc_password += chr(ord(char) + 10)
    
    
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
                return True
        else:
            print("Log in fail")
            print("writing to log file")
            log_file.write("FAIL")
            log_file.close()
            driver.quit()
            print("DONE")
            return False
        
   
  
        

class IG_AUTH:
    def __init__(self, username, password):
       self.username = username
       self.password = password
       self.cookie = ""

       

    def attempt_login_selenium(self):
        clear_log_file = open("./Program Data/Logs/IG_AUTH_LOGS/log.txt", "w")
        clear_log_file.close()
        #with open("UserAgents.txt", "r") as file: #Filename works, full path better??
        #    line = random.randint(1, 72) #remove hardcoded line value
        #    userAgent = file.readlines()[line] 
        #    userAgent = userAgent.replace("\n","")
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
        if("incorrect" not in driver.page_source):
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
                return True
        else:
            print("Log in fail")
            log_file.write("FAIL")
            log_file.write("FAIL")
            log_file.close()
            driver.quit()
            print("DONE")
            return False
    

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
    
    #The actual login url is https://twitter.com/i/flow/login
    #It prompts for username with a next button who's class is class="css-18t94o4 css-1dbjc4n r-42olwf r-sdzlij r-1phboty r-rs99b7 r-ywje51 r-usiww2 r-2yi16 r-1qi8awa r-1ny4l3l r-ymttw5 r-o7ynqc r-6416eg r-lrvibr r-13qz1uu"
    #Then it reprompts for username AND password, the logon button to submit them is:
    #class="css-18t94o4 css-1dbjc4n r-42olwf r-sdzlij r-1phboty r-rs99b7 r-19yznuf r-64el8z r-1ny4l3l r-1dye5f7 r-o7ynqc r-6416eg r-lrvibr"
    #id might be: data-testid="LoginForm_Login_Button"
    
    def attempt_login(self):
        #opening the file in write mode clears the previous login attempt
        clear_log_file = open("./Program Data/Logs/TW_AUTH_LOGS/log.txt", "w")
        clear_log_file.close()
        chrome_options = Options()
        #--headless makes the window not pop up
        chrome_options.add_argument("--headless")
        driver = selenium.webdriver.Chrome("./chromedriver", options=chrome_options) #"./chromedriver"
        #driver.get("https://twitter.com")
        driver.get("https://twitter.com/i/flow/login")
        print("TW opened")
        time.sleep(1)
        print(driver.title)
        #<input autocapitalize="sentences" autocomplete="on" autocorrect="on" disabled="" name="email" spellcheck="true" type="text" dir="auto" class="r-30o5oe r-1niwhzg r-17gur6a r-1yadl64 r-deolkf r-115tad6 r-poiln3 r-7cikom r-1ny4l3l r-t60dpp r-1dz5y72 r-fdjqy7 r-13qz1uu" value="ExampleEmail@gmail.com">
        #Above is the div class for where the username, email, or phone # is entered in login
        #Below is the input div for where the password if inputed
        #<input autocapitalize="sentences" autocomplete="current-password" autocorrect="on" name="password" spellcheck="true" type="password" dir="auto" class="r-30o5oe r-1niwhzg r-17gur6a r-1yadl64 r-deolkf r-homxoj r-poiln3 r-7cikom r-1ny4l3l r-t60dpp r-1dz5y72 r-fdjqy7 r-13qz1uu" value="ExamplePassword">
        log_file = open("./Program Data/Logs/TW_AUTH_LOGS/log.txt", "w")
        username = driver.find_element(By.NAME, "text")
        #Fill forms
        username.send_keys(self.username)
        time.sleep(5)
        print("Username")
        submit_form = driver.find_element(By.CLASS_NAME, "css-18t94o4.css-1dbjc4n.r-sdzlij.r-1phboty.r-rs99b7.r-ywje51.r-usiww2.r-2yi16.r-1qi8awa.r-1ny4l3l.r-ymttw5.r-o7ynqc.r-6416eg.r-lrvibr.r-13qz1uu")
        time.sleep(1)
        submit_form.click()
        print("Advancing past username")
        time.sleep(5)
        password = driver.find_element(By.NAME, "password")
        password.send_keys(self.password)
        print("Entered Password")
        submit_form = driver.find_element(By.CLASS_NAME, "css-18t94o4.css-1dbjc4n.r-sdzlij.r-1phboty.r-rs99b7.r-19yznuf.r-64el8z.r-1ny4l3l.r-1dye5f7.r-o7ynqc.r-6416eg.r-lrvibr")
        submit_form.click()
        print("Submitted")
        time.sleep(7)

        if("Home / Twitter" in driver.title):
            self.cookie = driver.get_cookies()
            if self.cookie is not None:
                print("Log in success")
                pickle.dump(self.cookie, open("./Program Data/Logs/TW_AUTH_LOGS/TW_cookies.pkl", "wb"))
                log_file.write("SUCCESS")
                log_file.close()
                driver.quit()
                encrypt_and_store(self, "TW")
                print("DONE")
                return True
        else:
            print("Log in fail")
            print("writing to log file")
            log_file.write("FAIL")
            log_file.close()
            driver.quit()
            print("DONE")
            return False
   
   

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
    attempt = auth.attempt_login()
    if(attempt != "Success"):
       return 0 
    else:
        return 1


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




