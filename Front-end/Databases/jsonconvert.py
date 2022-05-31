import mysql.connector
import json

#connection = mysql.connector.connect(user = 'id18940854_group7', password = '@Projectintegrationg7', host = 'localhost', database = 'id18940854_esp32_data')
connection = mysql.connector.connect(user = 'root', password = '05022002', host = 'localhost', database = 'world')
if connection: 
    print("Connection successfully")
else:
    print("Connection Not Established")

select_table = "SELECT * FROM city LIMIT 8"
cursor = connection.cursor(dictionary=True)
cursor.execute(select_table)
result = cursor.fetchall()
jsonResult = {"ListOfThings": result}
#for row in result:
    #mydict.add(row[0],{"name":row[1],"countrycode":row[2],"district":row[3],"population":row[4]})
stud_json = json.dumps(jsonResult, indent=2, sort_keys=True)
with open("./Databases/listofcities.json", "w") as f:
    f.write(stud_json)
print(stud_json)