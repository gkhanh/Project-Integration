// Initialize and add the map
function initMap() {
    // The location of ESP32
    const esp32Pos = { lat: 35, lng: 21.031 };
    // The map, centered at Uluru
    const map = new google.maps.Map(document.getElementById("map"), {
      zoom: 4,
      center: esp32Pos,
    });
    // The marker, positioned at Uluru
    const marker = new google.maps.Marker({
      position: esp32Pos,
      map: map,
    });
  }
  
  window.initMap = initMap;