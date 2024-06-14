// ---------------------------------------------------------------- // 
// Garbage Bin Ultrasonic Sensor HC-SR04
// ---------------------------------------------------------------- //

// ---------- Libraries
#include <WiFi.h>
#include <WiFiMulti.h>
#include <WebServer.h>
#include <Wire.h>
#include <FirebaseESP32.h>
#include <FirebaseESP32HTTPClient.h> #include <FirebaseJson.h>

// ---------- Deffinitions
#define echoPin 32 // attach pin D2 Arduino to pin Echo of HC-SR04 
#define trigPin 33 //attach pin D3 Arduino to pin Trig of HC-SR04

// ---------- Firebase
#define FIREBASE_HOST "https://garbagedisposal-8d74b.firebaseio.com/" 
#define FIREBASE_AUTH "znYg2kfgZLK9b39JE8HXCVezg9SnpGKCdtQHpomu"

// ---------- Ask Sensors
const char* https_host = "api.asksensors.com"; // ASKSENSORS host name
const int https_port = 443; // https port
const char* https_fingerprint = "B5 C3 1B 2C 0D 5D 9B E5 D6 7C B6 EF 50 3A AD 3F 9F 1E 44 75"; // ASKSENSORS HTTPS SHA1 certificate
const char* apiKeyIn = "JWprULpuQGBSaqhtSk2r74emngnfWmzl"; // API KEY IN
const unsigned int writeInterval = 10000; // write interval (in ms)

// ---------- Wi-Fi
#define WIFI_SSID "*******************" //Enter SSID here
#define WIFI_PASSWORD "*******************" //Enter Password here

// ---------- Variables
long duration; // variable for the duration of sound wave travel int distance; // variable for the distance measurement
String status; // variable for the full/empty status of bin
//For statistical analysis
int average2 = 0;
int average10 = 0;
float difference2 = 0.00;
float squares2 = 0.00;
float sum_of_squares2 = 0.00;
float variance2 = 0.00;
float standard_deviation2 = 0.00;

// ---------- Wi-Fi and Setup 
WebServer httpServer(80); 
WiFiClient client; 
FirebaseData firebaseData; 
WiFiMulti WiFiMulti; 
HTTPClient ask;

// ---------- Setup

void setup() 
{
	pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT 
	pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT

	Serial.begin(9600); // Serial communication is started with a baudrate speed of 9600 
	Serial.println("Ultrasonic Sensor HC-SR04 Garbage Bin");
	Serial.println("");

	//Wi-Fi 
	Serial.println("Connecting to "); 
	Serial.println(WIFI_SSID);

	//Sets to station mode 
	WiFi.mode(WIFI_STA);
	//Connect to your specified Wi-Fi network 
	WiFi.begin(WIFI_SSID, WIFI_PASSWORD); //Checks if Wi-Fi is connected
	while (WiFi.status() != WL_CONNECTED) {
		delay(1000);
		Serial.print("."); 
	}
	Serial.println(""); 
	Serial.println("WiFi connected..!"); 
	Serial.print("Got IP: "); 
	Serial.println(WiFi.localIP());
	
	//Firebase
	Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH); //Set Firebase info 
	Firebase.reconnectWiFi(true); //Enables reconnectto Wi-Fi if connection lost
	
	//HTTP
	httpServer.on("/", handle_OnConnect); 
	httpServer.onNotFound(handle_NotFound); 
	httpServer.begin();
	Serial.println("HTTP server started");
}
// ---------- Loop 
void loop() 
{
	// Clears the trigPin condition 
	digitalWrite(trigPin, LOW); 
	delay(2000);

	// Sets the trigPin HIGH (ACTIVE) for 10 microseconds 
	digitalWrite(trigPin, HIGH);
	delayMicroseconds(10);
	digitalWrite(trigPin, LOW);

	// Reads the echoPin, returns the sound wave travel time in microseconds
	duration = pulseIn(echoPin, HIGH); 
	// Calculating the distance
	distance = (duration-10) * 0.034 / 2;
	// distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back) // Displays the distance on the Serial Monitor
	Serial.print("\nDistance: ");
	Serial.print(distance);
	Serial.println(" cm"); Serial.print("The Bin is: "); 
	if (distance < 10) {
		Serial.print("Full!");
		status = "Full";
	} else if (distance < 30) {
		Serial.print("Not full, but contains trash.");
		status = "Not full, but contains trash"; 
	} else {
		Serial.print("Empty.");
		status = "Empty"; 
	}

	//Firebase
	//Send
	if((Firebase.push(firebaseData, "/binLevel_kain", distance)) &&
	(Firebase.push(firebaseData, "/binStatus_kain", status))) 
	{
		//Success
		Serial.println("Set Firebase data success ");
	} 
	else 
	{
		//Failed?, get the error reason from firebaseData
		Serial.print("Error in setting Firebase data ");
		Serial.println(firebaseData.errorReason()); 
	}

	//Get
	if(Firebase.get(firebaseData, "/binLevel_kain"))
	{
		//Success
		Serial.print("Get variant data success, type = "); 
		Serial.println(firebaseData.dataType());
		if (firebaseData.dataType() == "json") 
		{
			Serial.println();
			FirebaseJson &json = firebaseData.jsonObject(); 
			//Print all object data
			Serial.println("Pretty printed JSON data:");
			String jsonStr;
			json.toString(jsonStr, true); Serial.println(jsonStr);
			Serial.println();
			Serial.println("Iterate JSON data:"); Serial.println();
			size_t len = json.iteratorBegin(); String key, value = "";
			int type = 0; 
			int sum2 = 0;
			for (size_t i = 0; i < len; i++) 
			{
				json.iteratorGet(i, type, key, value);
				sum2 = sum2 + value.toInt(); 
			}
			Serial.print("Length = "); 
			Serial.println(len);

			average2 = sum2/len; 
			Serial.print("Average = "); 
			Serial.println(average2);

			for (size_t i = 0; i < len; i++) 
			{
				json.iteratorGet(i, type, key, value);
				difference2 = value.toInt() - average2;
				squares2 = sq(difference2);
				sum_of_squares2 = sum_of_squares2 + squares2;
			}
			
			variance2 = sum_of_squares2/len; 
			Serial.print("Variance = "); 
			Serial.println(variance2);

			standard_deviation2 = sqrt(variance2); 
			Serial.print("Standard Deviation = "); 
			Serial.println(standard_deviation2);
		}
	} 
	else 
	{
			//Failed?, get the error reason from firebaseData 
			Serial.print("\nError in getting Firebase data, reason: "); 
			Serial.println(firebaseData.errorReason());
	}

	//Asksensors
	if (!client.connect(https_host, https_port)) 
	{ 
		Serial.println("connection failed");
		return;
	}else 
	{
		// Create a URL for the request
		String url = "https://api.asksensors.com/write/";
    	url += apiKeyIn;
		url += "?module1=";
		url += distance;
		url += "&module2=";
		url += average2;
		url += "&module3=";
		url += variance2;
		url += "&module4=";
		url += standard_deviation2;

		Serial.print("********** requesting URL: "); 
		Serial.println(url);

		ask.begin(url); //Specify the URL

		//Check for the returning code int httpCode = ask.GET();
		if (httpCode > 0) 
		{
			String payload = ask.getString(); S
			erial.println(httpCode); 
			Serial.println(payload);
		} else 
		{
			Serial.println("Error on HTTP request"); 
		}
		
		ask.end(); //End
		Serial.println("********** End "); 
		Serial.println("*****************************************************");
	}
	
	client.stop(); // stop client 
	delay(writeInterval); // delay

	//HTTP
	httpServer.handleClient(); 
}

// ---------- HTML
void handle_OnConnect() 
{
	httpServer.send(200, "text/html", Sendhtml(distance, status)); 
}

void handle_NotFound()
{
	httpServer.send(404, "text/plain", "Not found");
}
 
String Sendhtml(int distance, String binStaus)
{
	String htmlDec = "<!DOCTYPE html> <html>\n";
	htmlDec +="<head><meta http-equiv=\"refresh\" content=\"10\" name=\"viewport\"
	content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
	htmlDec +="<title>ESP32 Sensors</title>\n";
	htmlDec +="<style>html { font-family: Arial; display: inline-block; margin: 0px auto;
	text-align: center;}\n";
	htmlDec +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\n"; htmlDec +="p {font-size: 24px;color: #444444;margin-bottom: 10px;}\n";
	htmlDec +="</style>\n";
	htmlDec +="</head>\n";
	htmlDec +="<body>\n";
	htmlDec +="<div id=\"webpage\">\n";
	htmlDec +="<h1>ESP32 Trash Bin Ultrasonic Sensor</h1>\n";
	htmlDec +="<p>The trash level is: ";
	htmlDec +=distance;
	htmlDec +=" cm</p>";
	htmlDec +="<p>The bin status is: ";
	htmlDec +=status;
	htmlDec +=".</p>";
	htmlDec +="</body>\n";
	htmlDec +="</html>\n";
	return htmlDec;
}