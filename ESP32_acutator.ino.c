#include <FirebaseESP32.h> 
#include <WiFi.h>

//defining the Firebase and WiFi connection information
#define FIREBASE_HOST "https://garbagedisposal-8d74b.firebaseio.com" 
#define FIREBASE_AUTH "znYg2kfgZLK9b39JE8HXCVezg9SnpGKCdtQHpomu" 
#define WIFI_SSID "*******************"
#define WIFI_PASSWORD "*******************"

//Defining FirebaseESP32 data object for data sending and receiving 
FirebaseData firebaseData;

// LED connections

//LEDs for Dev's data 
int greenLED1=15; 
int redLED1=16; 

//LEDs for kain's data 
int redLED = 23;
int greenLED = 22;

void setup() 
{
	Serial.begin(9600);// Initiating a serial communication

	WiFi.begin(WIFI_SSID, WIFI_PASSWORD); 
	Serial.print("Connecting to Wi-Fi");
	while (WiFi.status() != WL_CONNECTED)
	{
		Serial.print("."); 
		delay(300);
    }
	Serial.println(); 
	Serial.print("Connected with IP: "); 
	Serial.println(WiFi.localIP()); 
	Serial.println();
	
	Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH); //setting up Firebase info 

	Firebase.reconnectWiFi(true); //Enable auto reconnect the WiFi when connection lost

	//initialize the digital pins for LEDs 
	pinMode(greenLED1, OUTPUT); 
	pinMode(redLED1, OUTPUT); 
	pinMode(greenLED, OUTPUT); 
	pinMode(redLED, OUTPUT);
}

void loop()
{
	// Dev's data
	if(Firebase.get(firebaseData, "/Binlevel_Dev")) 
	{
		//Success
		Serial.print("Get variant data success, type = "); 
		Serial.println(firebaseData.dataType()); //prints the data type
		if (firebaseData.dataType() == "json") 
		{
			Serial.println();
			FirebaseJson &json = firebaseData.jsonObject(); //Print all object data
			Serial.println("Pretty printed JSON data:");
			String jsonStr;
			json.toString(jsonStr, true); Serial.println(jsonStr);
			Serial.println();
			
			//calculations
			Serial.println("Iterate JSON data:"); 
			Serial.println();
			size_t len = json.iteratorBegin(); String key, value = "";
			int type = 0;
			int sum1 = 0;
			int average1 = 0;
			for (size_t i = 0; i < len; i++) 
			{
				json.iteratorGet(i, type, key, value);
				sum1 = sum1 + value.toInt(); 
			}
 
    		average1 = sum1/len; 
    		Serial.print("Average = "); 
    		Serial.println(average1);

			float difference1 = 0.00;
			float squares1 = 0.00;
			float sum_of_squares1 = 0.00; float variance1 = 0.00;
			for (size_t i = 0; i < len; i++)
			{
				json.iteratorGet(i, type, key, value);
				difference1 = value.toInt() - average1;
				squares1 = sq(difference1);
				sum_of_squares1 = sum_of_squares1 + squares1;
			}
			variance1 = sum_of_squares1/len; 
			Serial.print("Variance ="); 
			Serial.println(variance1);
			
			float standard_deviation1 = sqrt(variance1); 
			Serial.print("Standard deviation = "); 
			Serial.println(standard_deviation1);
			
			//LED code 
			if(average1>10)
			{
				digitalWrite(greenLED1, HIGH); // turn the LED on (HIGH is the voltage level) 
				delay(1000); // wait for a second
				digitalWrite(greenLED1, LOW); // turn the LED off by making the voltage LOW 
				delay(1000);
			}
			else
			{
				digitalWrite(redLED1, HIGH); // turn the LED on (HIGH is the voltage level) 
				delay(1000); // wait for a second
				digitalWrite(redLED1, LOW); // turn the LED off by making the voltage LOW
				delay(1000); 
			}
		} 
	}

	//Kain's data
	if(Firebase.get(firebaseData, "/binLevel_kain")) 
	{
		//Success
		Serial.print("Get variant data success, type = "); 
		Serial.println(firebaseData.dataType()); //prints the data type
		if (firebaseData.dataType() == "json")
 		{
			Serial.println();
			FirebaseJson &json = firebaseData.jsonObject(); //Print all object data
			Serial.println("Pretty printed JSON data:");
			String jsonStr;
			json.toString(jsonStr, true); 
			Serial.println(jsonStr);
			Serial.println();

			//calculations
			Serial.println("Iterate JSON data:"); 
			Serial.println();
			size_t len = json.iteratorBegin(); 
			String key, value = "";
			int type = 0;
			int sum2 = 0;
			int average2 = 0;
			for (size_t i = 0; i < len; i++)
			{
				json.iteratorGet(i, type, key, value);
				sum2 = sum2 + value.toInt(); 
			}

			average2 = sum2/len; 
			Serial.print("Average = "); 
			Serial.println(average2);

			float difference2 = 0.00;
			float squares2 = 0.00;
			float sum_of_squares2 = 0.00; 
			float variance2 = 0.00;
			for (size_t i = 0; i < len; i++)
			{
				json.iteratorGet(i, type, key, value);
				difference2 = value.toInt() - average2;
				squares2 = sq(difference2);
				sum_of_squares2 = sum_of_squares2 + squares2;
			}
			variance2 = sum_of_squares2/len; 
			Serial.print("Variance ="); 
			Serial.println(variance2);

			float standard_deviation2 = sqrt(variance2); 
			Serial.print("Standard deviation = "); 
			Serial.println(standard_deviation2);
			
			//LED code 
			if(average2>10)
			{
 
				digitalWrite(greenLED, HIGH); // turn the LED on (HIGH is the voltage level) 
				delay(1000); // wait for a second
				digitalWrite(greenLED, LOW); // turn the LED off by making the voltage LOW 
				delay(1000);
			}
			else
			{
				digitalWrite(redLED, HIGH); // turn the LED on (HIGH is the voltage level) 
				delay(1000); // wait for a second
				digitalWrite(redLED, LOW); // turn the LED off by making the voltage LOW
				delay(1000); 
			}
		} 
	}
}
