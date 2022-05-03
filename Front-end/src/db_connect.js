var mysql = require('mysql');

var con = mysql.createConnection({
  host: "localhost",
  user: "root",
  password: "05022002",
  database: "world"
  //insecureAuth : true
});

con.connect(function(err) {
  if (err) throw err;
  console.log("Connected!");
  /*con.query("SELECT * FROM city WHERE id BETWEEN 1 AND 4", function (err, result, fields) {
    if (err){throw err;} 
    else{console.log(result);}
  });*/
});