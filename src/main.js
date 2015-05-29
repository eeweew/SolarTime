// Function to send a message to the Pebble using AppMessage API

var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

function utcstring_to_localtime(timestring) {
	var timestring_split = timestring.split(" ");
	var timestring_time = timestring_split[0].split(":");
	
	if (timestring_split[1] == "PM") {
		if (timestring_time[0] != 12) {
			timestring_time[0] = parseInt(timestring_time[0]) + 12;	
		}
	}
	else if (timestring_time[0] == 12) {
		timestring_time[0] = 0;
	}
	
	var d = new Date();
	
	var offset = d.getTimezoneOffset();
	console.log(offset);
	d.setHours(timestring_time[0]);
	d.setMinutes(timestring_time[1]);
	d.setSeconds(timestring_time[2]);
	
	
	d.setMinutes(d.getMinutes()-offset);
	
	var minutes = d.getHours()*60 + d.getMinutes();
	
	return minutes;
}


function locationSuccess(pos) {	
  var url = 'http://api.sunrise-sunset.org/json?lat=' + pos.coords.latitude + '&lng=' + pos.coords.longitude;
	console.log(pos.coords.latitude);
	console.log(pos.coords.longitude);
	
	xhrRequest(url, 'GET', 
    function(responseText) {
      // responseText contains a JSON object with weather info
      var json = JSON.parse(responseText);
			
			console.log(json.results.sunrise);
			console.log(json.results.sunset);
			console.log(json.results.solar_noon);
			
			var sunrise_utc_raw = json.results.sunrise;
			var sunset_utc_raw = json.results.sunset;
			var solar_noon_utc_raw = json.results.solar_noon;
			
			var sunrise = utcstring_to_localtime(sunrise_utc_raw);
			var sunset = utcstring_to_localtime(sunset_utc_raw);
			var solar_noon = utcstring_to_localtime(solar_noon_utc_raw);
			
			console.log(solar_noon);
			console.log(sunrise);
			console.log(sunset);
			
			if (solar_noon > sunset) {
				sunset = sunset + 720;
			}
			
			var solar_noon_offset = solar_noon - 720;
			
			console.log(sunrise);
			console.log(sunset);
			
			var daylength = (sunset-sunrise)/60;
			
			console.log(daylength);

			var height = Math.round((-1*Math.cos((Math.PI*(24-daylength))/24))*32 + 32);
//			var height = 50;
			console.log(height);
			
			var dictionary = {
//			'SUNSET_KEY': sunset,
//			'SUNRISE_KEY': sunrise,
			'HEIGHT_KEY': height,
			'NOON_KEY' : solar_noon_offset
			};
			
			Pebble.sendAppMessage(dictionary,
				function(e) {
					console.log('Info sent to Pebble successfully!');
				},
				function(e) {
					console.log('Error sending weather info to Pebble!');
				}
			);
			
    }      
  );
}

function locationError(err) {
  console.log('Error requesting location!');
}

function getData() {
  navigator.geolocation.getCurrentPosition(
    locationSuccess,
    locationError,
    {timeout: 15000, maximumAge: 60000}
  );
}

// Called when JS is ready
Pebble.addEventListener("ready",
							function(e) {
								
								//getData();
							});
												
// Called when incoming message from the Pebble is received
Pebble.addEventListener("appmessage",
							function(e) {
								console.log("Received Status: " + e.payload.status);
								getData();
							});

Pebble.addEventListener("showConfiguration",
  function(e) {
    //Load the remote config page
    Pebble.openURL("https://dl.dropboxusercontent.com/u/29028018/PebbleSettings/SolarTime.html");
  }
);

Pebble.addEventListener("webviewclosed",
  function(e) {
    //Get JSON dictionary
    var configuration = JSON.parse(decodeURIComponent(e.response));
    console.log("Configuration window returned: " + JSON.stringify(configuration));
 
    //Send to Pebble, persist there
		var dictionary = {
		'THICK_KEY': configuration.curve_style,
		'TYPE_KEY': configuration.font,
    'FONT_KEY' : configuration.font_style,
    'HOR_KEY' : configuration.hor_style,
		'INVERT_KEY' : configuration.b_color,
		'TIMEDATE_KEY': configuration.timedate_style
		};	
		
    Pebble.sendAppMessage(dictionary, 
      function(e) {
        console.log("Sending settings data...");
      },
      function(e) {
        console.log("Settings feedback failed!");
      }
    );
  }
);