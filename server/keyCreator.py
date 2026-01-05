import sqlite3
import random
import string

db = sqlite3.connect("licenses.db")
cursor = db.cursor()

# check if table exists in db, create one if it doesnt
res = cursor.execute("SELECT name FROM sqlite_master")
if (len(res.fetchall()) != 1):
    cursor.execute("CREATE TABLE validLicenses(license, email)")

while True:
    try:
        choice = int(input("1. Create new license key \n2. Print table\n3. Clear table\n"))
    except KeyboardInterrupt: # exit gracefully if a keyboard interrupt happens
        break
    except ValueError:
        print("Choose a number")

    match choice:
        case 1: # add new license
            while True:
                newLicense = ""
                for i in range(4): # random generate a new license
                    for j in range(4):
                        newLicense = newLicense + random.choice(string.ascii_uppercase)
                    if i != 3:
                        newLicense = newLicense + "-"
                
                res = cursor.execute("SELECT license FROM validLicenses")
                licenses = res.fetchall()

                licenseTable = res.fetchall()
                flag = False
                for i in licenseTable: # check if license already exists in table
                    if i[0] == license:
                        flag = True
                        break
                if flag == True: # generate a new license if it does
                    continue

                print(newLicense)
                cursor.execute(f"INSERT INTO validLicenses VALUES ('{newLicense}', '')")
                db.commit()
                break
        case 2: # loop through rows and print each row
            for r in cursor.execute("SELECT license, email FROM validLicenses"):
                print(r)
        case 3: # clear & recreate table
            cursor.execute("DROP TABLE validLicenses")
            cursor.execute("CREATE TABLE validLicenses(license, email)")
            db.commit()
        case _: # default case; invalid input
            print("Choose a valid number")

    print("\n")