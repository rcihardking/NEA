import json
import re
import sqlite3
from http.server import BaseHTTPRequestHandler, HTTPServer

def licenseHandler(license : str, email : str): # cross references license against database, returns a http response code
    licenseRe = "[A-Z][A-Z][A-Z][A-Z]-[A-Z][A-Z][A-Z][A-Z]-[A-Z][A-Z][A-Z][A-Z]-[A-Z][A-Z][A-Z][A-Z]"
    emailRe = "([a-zA-Z]*|[0-9]*)@([a-zA-Z]*|[0-9]*).com"

    if license == "test" and email == "mrbfdi": # debug statement to bypass license checker, REMOVE LATER!!
        return 200

    # sanatize inputs        
    match = re.fullmatch(licenseRe, license)
    if match == None:
        return 400
    match = re.fullmatch(emailRe, email)
    if match == None:
        return 400


    db = sqlite3.connect("licenses.db")
    cursor = db.cursor()

    res = cursor.execute("SELECT license FROM validLicenses")
    licenseTable = res.fetchall()
    flag = False
    for i in licenseTable: # check if license exists
        if i[0] == license:
            flag = True
            break
            
    if flag == False:
        return 401 # license doesnt exist
    
    res = cursor.execute(f"SELECT email FROM validLicenses WHERE license = '{license}'")
    emailTable = res.fetchone()
    if emailTable[0] == '':
        cursor.execute(f"UPDATE validLicenses SET email = '{email}' WHERE license = '{license}'")
        db.commit()
        return 200 # unclaimed license now owned by user
    elif emailTable[0] == email:
        return 200 # provided license key is owned by user

    return 401 # license is not owned by user


class server(BaseHTTPRequestHandler):

    def do_POST(self):        
        match self.path:
            case "/licenses": # license checker resource
                data = self.rfile.read(int(self.headers["Content-Length"])).decode() # gets the posted data
                try:
                    info = json.loads(data) # parse posted data as json
                    response = licenseHandler(info["license"], info["email"])
                    if response == 400:
                        raise Exception("bad input")

                    self.send_response(response)
                    self.send_header("Content-type", "application/json")
                    self.end_headers()
                except (json.decoder.JSONDecodeError, KeyError, Exception): # malformed data recieved
                    self.send_response(400)
                    self.send_header("Content-type", "application/json")
                    #self.wfile.write("malformed data sent".encode())
                    self.end_headers()
                except: # unknown error
                    self.send_response(500) # assume some server-side error happened
                    self.send_header("Content-type", "application/json")
                    #self.wfile.write("unknown error occurred".encode())
                    self.end_headers()

            case _: # default case; resource doesnt exist
                self.send_response(404)
                self.send_header("Content-type", "")
                self.end_headers()


if __name__ == "__main__":
    try: # start server
        server = HTTPServer(('127.0.0.255', 80), server)
        server.serve_forever()
    except KeyboardInterrupt: # stop server gracefully when a keyboard interrupt happens
        server.socket.close()
        print("stopping...")
