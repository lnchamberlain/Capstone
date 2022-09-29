# UnderCover Recovery
# Windows Desktop Application for scraping and storing keywords and flagged users from social media sites
# Developed by Tyler and Logan
# UAA CSCE Capstone Project Fall 2022
# Program for opening a file location in file explorer

import sys
import os
import subprocess

def main(argv):
    arg = argv[1]
    FILE_PATH = os.getcwd()
    FILE_PATH += "/Program Data/"
    print(arg)
    path = os.path.normpath(FILE_PATH)
    print(path)
    if os.path.isdir(path):
        print("ISDIR")
    #Check which file location to open
    if(arg == "Results_FB"):
        FILE_PATH += "FoundPosts/FoundPostsFB/"
    if(arg == "Results_IG"):
        FILE_PATH += "FoundPosts/FoundPostsIG/"
    if(arg == "Results_TW"):
        FILE_PATH += "FoundPosts/FoundPostsTW/"
    if(arg == "Wordlist"):
        FILE_PATH += "Wordlists/"
    if(arg == "Flagged_FB"):
        FILE_PATH += "FlaggedUsers/FBFlaggedUsers/"
    if(arg == "Flagged_IG"):
        FILE_PATH += "FlaggedUsers/IGFlaggedUsers/"
    if(arg == "Flagged_TW"):
        FILE_PATH += "FlaggedUsers/TWFlaggedUsers/"
    PATH = os.path.abspath(FILE_PATH)
    WIN_FORMAT_PATH = PATH.replace('/', '\\')
    webbrowser.open(WIN_FORMAT_PATH)
    os.startfile(WIN_FORMAT_PATH)
        
if __name__ == "__main__":
    main(sys.argv)
