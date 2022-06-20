var longtitude;
var latitude;

// Initialize and add the map
function initMap() {
    // The location of ESP32
    const esp32Pos = { lat: 52.2207, lng: 6.8866 };
    // The map, centered at ESP32
    const map = new google.maps.Map(document.getElementById("map"), {
      zoom: 4,
      center: esp32Pos,
    });
    // The marker, positioned at ESP32
    const marker = new google.maps.Marker({
      position: esp32Pos,
      map: map,
    });
  }
  
function getLatitude(){

}
  window.initMap = initMap;