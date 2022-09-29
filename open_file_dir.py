# UnderCover Recovery
# Windows Desktop Application for scraping and storing keywords and flagged users from social media sites
# Developed by Tyler and Logan
# UAA CSCE Capstone Project Fall 2022
# Program for opening a file location in file explorer

import sys
import os
import subprocess


def main(arg):
    FILE_PATH = os.getcwd()
    #Check which file location to open
    if(arg == "Results_FB"):
        FILE_PATH = "STUB"
    if(arg == "Results_IG"):
        FILE_PATH = "STUB"
    if(arg == "Results_TW"):
        FILE_PATH = "STUB"
    if(arg == "Wordlist"):
        FILE_PATH = "STUB"
    if(arg == "Flagged_FB"):
        FILE_PATH = "STUB"
    if(arg == "Flagged_IG"):
        FILE_PATH = "STUB"
    if(arg == "Flagged_TW"):
        FILE_PATH = "STUB"
    



if __name__ == "__main__":
    main(sys.argv)
