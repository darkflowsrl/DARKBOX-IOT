window.addEventListener('load', getReadings);

// Create Temp 1
var gaugeTemperature0 = new LinearGauge({
  renderTo: 'gauge-temperature',
  width: 120,
  height: 400,
  units: "Degree Celsius",
  minValue: 0,
  maxValue: 50,
  majorTicks: [
    "0",
    "10",
    "20",
    "30",
    "40",
    "50",
  ],
  minorTicks: 10,
  strokeTicks: true,
  highlights: [
    {
      "from": 25,
      "to": 50,
      "color": "rgba(200, 50, 50, .75)"
    }
  ],
  colorPlate: "transparent",
  borderShadowWidth: 0,
  borders: false,
  needleType: "arrow",
  needleWidth: 2,
  animationDuration: 1500,
  animationRule: "linear",
  tickSide: "both",
  numberSide: "left",
  needleSide: "both",
  barStrokeWidth: 7,
  barBeginCircle: false,
}).draw();

// Create temp 1
var gaugeTemperature1 = new LinearGauge({
  renderTo: 'gauge-temperature1',
  width: 120,
  height: 400,
  units: "Degree Celsius",
  minValue: 0,
  maxValue: 50,
  majorTicks: [
    "0",
    "10",
    "20",
    "30",
    "40",
    "50",
  ],
  minorTicks: 10,
  strokeTicks: true,
  highlights: [
    {
      "from": 25,
      "to": 50,
      "color": "rgba(200, 50, 50, .75)"
    }
  ],
  colorPlate: "transparent",
  borderShadowWidth: 0,
  borders: false,
  needleType: "arrow",
  needleWidth: 2,
  animationDuration: 1500,
  animationRule: "linear",
  tickSide: "both",
  numberSide: "left",
  needleSide: "both",
  barStrokeWidth: 7,
  barBeginCircle: false,
}).draw();

// Create temp 2
var gaugeTemperature2 = new LinearGauge({
  renderTo: 'gauge-temperature2',
  width: 120,
  height: 400,
  units: "Degree Celsius",
  minValue: 0,
  maxValue: 50,
  majorTicks: [
    "0",
    "10",
    "20",
    "30",
    "40",
    "50",
  ],
  minorTicks: 10,
  strokeTicks: true,
  highlights: [
    {
      "from": 25,
      "to": 50,
      "color": "rgba(200, 50, 50, .75)"
    }
  ],
  colorPlate: "transparent",
  borderShadowWidth: 0,
  borders: false,
  needleType: "arrow",
  needleWidth: 2,
  animationDuration: 1500,
  animationRule: "linear",
  tickSide: "both",
  numberSide: "left",
  needleSide: "both",
  barStrokeWidth: 7,
  barBeginCircle: false,
}).draw();

// Create temp 3
var gaugeTemperature3 = new LinearGauge({
  renderTo: 'gauge-temperature3',
  width: 120,
  height: 400,
  units: "Degree Celsius",
  minValue: 0,
  maxValue: 50,
  majorTicks: [
    "0",
    "10",
    "20",
    "30",
    "40",
    "50",
  ],
  minorTicks: 10,
  strokeTicks: true,
  highlights: [
    {
      "from": 25,
      "to": 50,
      "color": "rgba(200, 50, 50, .75)"
    }
  ],
  colorPlate: "transparent",
  borderShadowWidth: 0,
  borders: false,
  needleType: "arrow",
  needleWidth: 2,
  animationDuration: 1500,
  animationRule: "linear",
  tickSide: "both",
  numberSide: "left",
  needleSide: "both",
  barStrokeWidth: 7,
  barBeginCircle: false,
}).draw();

// Create Hum 0
var gaugeHumidity = new RadialGauge({
  renderTo: 'gauge-humidity',
  width: 300,
  height: 300,
  units: "%",
  title: "Humidity",
  minValue: 0,
  maxValue: 100,
  majorTicks: [
    0,
    10,
    20,
    30,
    40,
    50,
    60,
    70,
    80,
    90,
    100,

  ],
  minorTicks: 2,
  strokeTicks: true,
  highlights: [
    {
      "from": 0,
      "to": 50,
      "color": "rgba(0,0, 255, .3)"
    },
    {
      "from": 50,
      "to": 100,
      "color": "rgba(255, 0, 0, .3)"
    }
  ],
  ticksAngle: 225,
  startAngle: 67.5,
  colorMajorTicks: "#ddd",
  colorMinorTicks: "#ddd",
  colorTitle: "#eee",
  colorUnits: "#ccc",
  colorNumbers: "#eee",
  colorPlate: "#222",
  borderShadowWidth: 0,
  borders: true,
  needleType: "arrow",
  needleWidth: 2,
  needleCircleSize: 7,
  needleCircleOuter: true,
  needleCircleInner: false,
  animationDuration: 1500,
  animationRule: "linear",
  colorBorderOuter: "#333",
  colorBorderOuterEnd: "#111",
  colorBorderMiddle: "#222",
  colorBorderMiddleEnd: "#111",
  colorBorderInner: "#111",
  colorBorderInnerEnd: "#333",
  colorNeedleShadowDown: "#333",
  colorNeedleCircleOuter: "#333",
  colorNeedleCircleOuterEnd: "#111",
  colorNeedleCircleInner: "#111",
  colorNeedleCircleInnerEnd: "#222",
  valueBoxBorderRadius: 0,
  colorValueBoxRect: "#222",
  colorValueBoxRectEnd: "#333"
}).draw();

// Function to get current readings @g
function getReadings() {
  var xhr = new XMLHttpRequest();
  xhr.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      var myObj = JSON.parse(this.responseText);
      console.log(myObj);
      var temp0 = myObj.temperature0;
      var temp1 = myObj.temperature1;
      var temp2 = myObj.temperature2;
      var temp3 = myObj.temperature3;
      var hum = myObj.humidity;

      gaugeTemperature0.value = temp0;
      gaugeTemperature1.value = temp1;
      gaugeTemperature2.value = temp2;
      gaugeTemperature3.value = temp3;
      gaugeHumidity.value = hum;
    }
  };
  xhr.open("GET", "/readings", true);
  xhr.send();
}

if (!!window.EventSource) {
  var source = new EventSource('/events');

  source.addEventListener('open', function (e) {
    console.log("Events Connected");
  }, false);

  source.addEventListener('error', function (e) {
    if (e.target.readyState != EventSource.OPEN) {
      console.log("Events Disconnected");
    }
  }, false);

  source.addEventListener('message', function (e) {
    console.log("message", e.data);
  }, false);

  source.addEventListener('new_readings', function (e) {
    console.log("new_readings", e.data);
    var myObj = JSON.parse(e.data);
    console.log(myObj);
    gaugeTemperature0.value = myObj.temperature0;
    gaugeTemperature1.value = myObj.temperature1;
    gaugeTemperature2.value = myObj.temperature2;
    gaugeTemperature3.value = myObj.temperature3;
    gaugeHumidity.value = myObj.humidity;
  }, false);
}