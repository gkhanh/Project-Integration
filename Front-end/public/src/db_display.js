var timestamp;
var temperature;
var heartbeat;

function updateChart() {
  async function fetchData() {
    //const url = "http://localhost:5000/projectintegration-c0d16-default-rtdb-export.json";
    const url = "https://projectintegration-c0d16.web.app/projectintegration-c0d16-default-rtdb-export.json";
    const response = await fetch(url);
    //Wait until the request has been completed
    const datapoints = await response.json();
    console.log(datapoints);
    return datapoints;
  };

  fetchData().then(datapoints => {
    heartbeat = datapoints.Patient1.map(
      function(index){
        return index.heartbeat;
      }
    )
    temperature = datapoints.Patient1.map(
      function(index){
        return index.gpsLat;
      })
      
    timestamp = datapoints.Patient1.map(
      function(index){
        return index.Time;
      })
    console.log(timestamp);
    console.log(temperature);
    console.log(heartbeat);
    myChart.config.data.labels = timestamp;
    myChart.config.data.datasets[0].data = heartbeat;
    myChart.update();
    
    lineChart.config.data.labels = timestamp;
    lineChart.config.data.datasets[0].data = heartbeat;
    lineChart.update();

  })
}

// setup first chart
const data = {
  labels: timestamp,
  datasets: [{
    label: 'Patient 1 health status',
    data: heartbeat,
    backgroundColor: [
      'rgba(255, 26, 104, 0.2)',
      'rgba(54, 162, 235, 0.2)',
      'rgba(255, 206, 86, 0.2)',
      'rgba(75, 192, 192, 0.2)',
      'rgba(153, 102, 255, 0.2)',
      'rgba(255, 159, 64, 0.2)',
      'rgba(0, 0, 0, 0.2)'
    ],//'transparent',
    borderColor: [
      'rgba(255, 26, 104, 1)',
      'rgba(54, 162, 235, 1)',
      'rgba(255, 206, 86, 1)',
      'rgba(75, 192, 192, 1)',
      'rgba(153, 102, 255, 1)',
      'rgba(255, 159, 64, 1)',
      'rgba(0, 0, 0, 1)'
    ],
    borderWidth: 2
  }]
};
// config 
const config = {
  type: 'line', data, options: {
    scales: {
      y: {
        beginAtZero: true
      }
    }
  }
};
// render init block
const myChart = new Chart(
  document.getElementById('myChart'),
  config
);

//graph 2
const data2 = {
  labels: timestamp,
  datasets: [{
    label: 'Patient 2 health status',
    data: heartbeat,
    backgroundColor: [
      'rgba(255, 26, 104, 0.2)',
      'rgba(54, 162, 235, 0.2)',
      'rgba(255, 206, 86, 0.2)',
      'rgba(75, 192, 192, 0.2)',
      'rgba(153, 102, 255, 0.2)',
      'rgba(255, 159, 64, 0.2)',
      'rgba(0, 0, 0, 0.2)'
    ],//'transparent',
    borderColor: [
      'rgba(255, 26, 104, 1)',
      'rgba(54, 162, 235, 1)',
      'rgba(255, 206, 86, 1)',
      'rgba(75, 192, 192, 1)',
      'rgba(153, 102, 255, 1)',
      'rgba(255, 159, 64, 1)',
      'rgba(0, 0, 0, 1)'
    ],
    borderWidth: 2
  }]
};

//config linechart
const config2 = {
  type: 'line', data, options: {
    scales: {
      y: {
        beginAtZero: true
      }
    }
  }
};

// render init block for 2nd graph
const lineChart = new Chart(
  document.getElementById('lineChart'),
  config2
);