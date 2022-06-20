
var longtitude;
var latitude;
/*var ref = firebase.database().ref("users");
ref.on("value", function (snapshot) {
  snapshot.forEach(function (childSnapshot) {
    var childData = childSnapshot.val();
    var longtitude = childData.longtitude;
    var latitude = childData.latitude;
    console.log(longtitude);
    console.log(latitude);
  });
});*/

// Initialize and add the map
function initMap() {
  // The location of ESP32
  const esp32Pos = { lat: 52.2207, lng: 6.8866};
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
window.initMap = initMap;