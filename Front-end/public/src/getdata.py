import urllib.request
import json 
from firebase import firebase

def getResponse(url):
    operUrl = urllib.request.urlopen(url)
    if(operUrl.getcode()==200):
        data = operUrl.read()
        jsonData = json.loads(data)
    else:
        print("Error receiving data", operUrl.getcode())
    return jsonData

def main():
    urlData = "https://projectintegration-c0d16-default-rtdb.europe-west1.firebasedatabase.app/"
    jsonData = getResponse(urlData)
    with open("./Databases/test.json", "w") as f:
        f.write(jsonData)