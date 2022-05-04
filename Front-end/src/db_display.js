window.onload = function () {
  var dataPoints = [];
  var chart = new CanvasJS.Chart("chartContainer",{
    title:{
      text:"Rendering Chart with dataPoints from External JSON"
    },
    data: [{
      type: "line",
      dataPoints : dataPoints,
    }]
  });
  $.getJSON("./Databases/listofcities.json", function(data) {  
    $.each(data, function(key, value){
      dataPoints.push({x: value[0], y: parseInt(value[1])});
    });	
    chart.render();
  });
  }