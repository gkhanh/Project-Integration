var $element = document.getElementById("myChart");
var ctx = $element.getContext("2d");
var myChart;
var data = {}, processedData = {}, orderClosingByMonth = {};
var labels = [];
//Graph design
var jsonData = $.ajax({
    url: './Databases/listofcities.json',
    dataType: 'json',
}).done(function (results) {
    //get values that only needed
    processedData = processData(results);
    data = {
        labels: processedData.labels,
        datasets: [{
            label: "City population",
            fillColor: "rgba(151,187,205,0.2)",
            strokeColor: "rgba(151,187,205,1)",
            pointColor: "rgba(151,187,205,1)",
            pointStrokeColor: "#fff",
            pointHighlightFill: "#fff",
            pointHighlightStroke: "rgba(151,187,205,1)",
            data: processedData.data
        }]
    };

    myChart = new Chart(ctx, {
        type: 'line',
        data: data,
        options: {
            "scales": {
                "yAxes": [{
                    "ticks": {
                        "beginAtZero": true
                    }
                }]
            }
        }
    });
});


var processData = function(jsonData) {
    var jsonVal = jsonData["1","2","3","4","5"]
    var dataSet = [];
    var numbr;
    var locale = "en-us";
    var months = Object.keys(jsonVal).map(function(item) {
      numbr = new numbr(item);
  
      return date.toLocaleDateString(locale, {
        month: "long"
      });
    }).filter(function(elem, index, self) {
      return index == self.indexOf(elem);
    });
  
    function sortByMonth(arr) {
      var exactMonths = ["January", "February", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"
      ];
      arr.sort(function(a, b) {
        return exactMonths.indexOf(a) - exactMonths.indexOf(b);
      });
      return arr;
    };
  
    labels = sortByMonth(months);
  
    for (var i = 0, total = labels.length; i < total; i++) {
      orderClosingByMonth[labels[i]] = {
        close: 0,
        allValue: [],
        allKey: []
      }
    }
  
    var thisMonth;
    Object.keys(jsonVal).filter(function(item) {
      date = new Date(item + " 00:00:00");
      thisMonth = date.toLocaleDateString(locale, {
        month: "long"
      });
  
      if (orderClosingByMonth[thisMonth]["close"] < jsonVal[item]["4. close"]) {
        orderClosingByMonth[thisMonth]["close"] = jsonVal[item]["4. close"];
      }
  
      orderClosingByMonth[thisMonth]["allKey"].push(item);
      orderClosingByMonth[thisMonth]["allValue"].push(parseFloat(jsonVal[item]["4. close"]));
  
      return 0;
    });
  
    for (var i in orderClosingByMonth) {
      dataSet.push(orderClosingByMonth[i].close);
    }
    ///debugger;
    
    return {
      labels: labels,
      data: dataSet
    }
  };