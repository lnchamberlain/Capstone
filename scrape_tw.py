
import requests

payload = {
    'inUserName': 'thetyranttyler',
    'inUserPass': 'News!@on12Speed'
}

with requests.Session() as s:
    p = s.post('https://twitter.com/i/flow/login', data=payload)
    #print(p.text)
    
    r = s.get('https://twitter.com/home')
    with open(r'/home/wisteria/CapstoneProject/Capstone/twitter_scrape_test.txt', mode='wb') as file:
        file.write(r.content)
        file.close()
        
# IF you look at the twitter_scrape_test.txt you'll see that the request worked,
# but that twitter hashes the content in line 6.
# Looks like we'll need to work around it with selenium.??
# More research required.
