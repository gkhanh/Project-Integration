import mysql.connector
import json

#connection = mysql.connector.connect(user = 'id19022493_root', password = '@Projectintg7', host = 'localhost', database = 'id19022493_esp32')
connection = mysql.connector.connect(user = 'root', password = '05022002', host = 'localhost', database = 'esp32data')
if connection: 
    print("Connection successfully")
else:
    print("Connection Not Established")

select_table = "SELECT * FROM led_status LIMIT 8"
cursor = connection.cursor(dictionary=True)
cursor.execute(select_table)
result = cursor.fetchall()
jsonResult = {"ListOfThings": result}
#for row in result:
    #mydict.add(row[0],{"name":row[1],"countrycode":row[2],"district":row[3],"population":row[4]})
stud_json = json.dumps(jsonResult, indent=2, sort_keys=True)
with open("./Databases/test.json", "w") as f:
    f.write(stud_json)
print(stud_json)