#include "settings.h"

/*
This is a function that sends data to the app.
Data packets are sent using OsmAnd-like protocol:
id=name&lat={0}&lon={1}&timestamp={2}&speed={3}&altitude={4}
*/
String sendToPhone(DATA *package) {

  String result;

  result += F("&name=");  //other tracker's name
  result += package->name;  //NAME_LENGTH bytes

  result += F("&lat=");       // cordinates
  result += package->lat;  // latitude
  result += F("&lon=");       // record separator
  result += package->lon;  // longitute

  //Date and time format: 2023-06-07T15:21:00Z
  result += F("&timestamp=");      // record separator
  result += package->year + 2000;  // year
  result += F("-");                // data separator
  if (package->month < 10) result += F("0");
  result += package->month;        // month
  result += F("-");                // data separator
  if (package->day < 10) result += F("0");
  result += package->day;          // day
  result += F("T");                // record separator
  if (package->hour < 10) result += F("0");
  result += package->hour;         // hour
  result += F(":");                // time separator
  if (package->minute < 10) result += F("0");
  result += package->minute;       // minute
  result += F(":");                // time separator
  if (package->second < 10) result += F("0");
  result += package->second;       // second
  result += F("Z");                // UTC

  // Sensors and additional data
  result += F("&sat="); 
  result += package->sat;  // satellites  1 byte

  result += F("&hdop="); 
  result += package->hdop;  // HDOP  1 byte

  // Add more data here if you need ...
  // result += "&speed=";       // data's name in app
  // result += package->speed;   // value

  result += "&batt=";
  result += package->battery;

  // result += "&alienSensor=";
  // result += package->sensor1;

  // result += "&C-137-level=";
  // result += package->sensor2;

  return result;
}



/*

void sendToPhone(DATA *package) {

  Serial.print(F("&name="));  //other tracker's name
  Serial.print(package->name);  //NAME_LENGTH bytes

  Serial.print(F("&lat="));       // cordinates
  Serial.print(package->lat, 6);  // latitude
  Serial.print(F("&lon="));       // record separator
  Serial.print(package->lon, 6);  // longitute

  //Date and time format: 2023-06-07T15:21:00Z
  Serial.print(F("&timestamp="));      // record separator
  Serial.print(package->year + 2000);  // year
  Serial.print(F("-"));                // data separator
  if (package->month < 10) Serial.print(F("0"));
  Serial.print(package->month);        // month
  Serial.print(F("-"));                // data separator
  if (package->day < 10) Serial.print(F("0"));
  Serial.print(package->day);          // day
  Serial.print(F("T"));                // record separator
  if (package->hour < 10) Serial.print(F("0"));
  Serial.print(package->hour);         // hour
  Serial.print(F(":"));                // time separator
  if (package->minute < 10) Serial.print(F("0"));
  Serial.print(package->minute);       // minute
  Serial.print(F(":"));                // time separator
  if (package->second < 10) Serial.print(F("0"));
  Serial.print(package->second);       // second
  Serial.print(F("Z"));                // UTC

  // Sensors and additional data
  Serial.print(F("&sat=")); 
  Serial.print(package->sat);  // satellites  1 byte

  Serial.print(F("&hdop=")); 
  Serial.print(package->hdop);  // HDOP  1 byte

  // Add more data here if you need ...
  // Serial.print("&speed=");       // data's name in app
  // Serial.print(package->speed);   // value

  Serial.print("&batt=");
  Serial.print(package->battery);

  // Serial.print("&alienSensor=");
  // Serial.print(package->sensor1);

  // Serial.print("&C-137-level=");
  // Serial.print(package->sensor2);
}

*/