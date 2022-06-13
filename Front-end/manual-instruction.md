This is the manual instruction for how to establish our webserver (both locally and remotely)

1. Access this website: https://console.firebase.google.com/u/5/project/projectintegration-c0d16/database/projectintegration-c0d16-default-rtdb/data 
   to view firebase project.
   Information below is our project email:
   - email: projectg72022@gmail.com
   - password: Projectint

2. After cloning the whole folder (Front-end) to PC, open cmd and type:
  - 'firebase serve': establish webserver locally
  - 'firebase deploy': establish webserver remotely
*Note: if run the server locally, uncomment the line 7 of db_display.js (path: /Front-end/public/src/db_display.js)
       if run the server remotely, uncomment/leave the line 8 of db_display.js (path: /Front-end/public/src/db_display.js)
       Because this function is to initiate and draw the graphs.
       
3. After run the connection code for esp32, you can see the data sent from esp32 to firebase in 'Realtime database' section. 
   It will be in JSON form. You have to download it and put it in the following path: /Front-end/public and 
   rename the file same as the following : projectintegration-c0d16-default-rtdb-export (or change the name in the code).
   Just make the name match together.
   
