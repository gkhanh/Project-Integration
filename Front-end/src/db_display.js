function updateChart() {
  async function fetchData() {
    const url = "http://127.0.0.1:5500/Front-end/Databases/listofcities.json";
    const response = await fetch(url);
    //Wait until the request has been completed
    const datapoints = await response.json();
    console.log(datapoints);
    return datapoints;
  };

  fetchData().then(datapoints => {
    const city = datapoints.ListOfThings.map(
      function(index){
        return index.Name;
      })
      const population = datapoints.ListOfThings.map(
        function(index){
          return index.Population;
        })
    console.log(city);
    console.log(population);
    myChart.config.data.labels = city;
    myChart.config.data.datasets[0].data = population;
    myChart.update();
  })
}

// setup 
const data = {
  labels: ['Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat', 'Sun'],
  datasets: [{
    label: 'Weekly Sales',
    data: [18, 12, 6, 9, 12, 3, 9],
    backgroundColor: [
      'rgba(255, 26, 104, 0.2)',
      'rgba(54, 162, 235, 0.2)',
      'rgba(255, 206, 86, 0.2)',
      'rgba(75, 192, 192, 0.2)',
      'rgba(153, 102, 255, 0.2)',
      'rgba(255, 159, 64, 0.2)',
      'rgba(0, 0, 0, 0.2)'
    ],
    borderColor: [
      'rgba(255, 26, 104, 1)',
      'rgba(54, 162, 235, 1)',
      'rgba(255, 206, 86, 1)',
      'rgba(75, 192, 192, 1)',
      'rgba(153, 102, 255, 1)',
      'rgba(255, 159, 64, 1)',
      'rgba(0, 0, 0, 1)'
    ],
    borderWidth: 1
  }]
};
// config 
const config = {
  type: 'bar', data, options: {
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