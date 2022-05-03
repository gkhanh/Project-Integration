var express = require('express');
var router = express.Router();
var database = require('../database');

router.get("/", function(request, response, next){
	var query = "SELECT * FROM city WHERE id BETWEEN 1 AND 4";
	database.query(query, function(error, data){
		if(error)
		{
			throw error; 
		}
		else
		{
			response.render('sample_data', {title:'Node.js MySQL CRUD Application', action:'list', sampleData:data});
		}

	});

});

module.exports = router;