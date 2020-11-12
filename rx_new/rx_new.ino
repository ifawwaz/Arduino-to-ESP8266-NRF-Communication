#include <nRF24L01.h>
#include <RF24.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
//Mendeklarasikan fungsi i2c lcd dengan alamat 0x3F untuk lcd 16x2
LiquidCrystal_I2C lcd(0x3F,16,2);

//Mendefinisikan kunci api thingspeak
String apiKey = "CI37IVZ4CUSGEXDY";
//Mendefinisikan ssid dan password wifi (ganti ssid dan password sesuai wifi user)
const char* ssid = "BUSTAMI";
const char* password = "INDONESIA";
 
WiFiClient client;

//Mendefinisikan server api thinkspeak
const char* server = "api.thingspeak.com";

//Mendeklarasikan fungsi RF24 dengan nama "radio" pada pin 2 (CE) dan 15 (CSN)
RF24 radio(2, 15); // CE, CSN
//Mendefinisikan address atau alamat komunikasi NRF
const byte address[6] = "00001";
//Deklarasi variabel untuk menyimpan nilai humidity dan temperatur
float temperature;
float humidity;
void setup()
{
  //memulai komunikasi serial dengan baud rate 115200
  Serial.begin(115200);
  //memulai lcd
  lcd.begin();
  lcd.backlight();
  lcd.setCursor(3,0);
  lcd.print("Monitoring");
  lcd.setCursor(5,1);
  lcd.print("Kompos");
  delay(5000);
  lcd.clear();
  //memulai komunikasi nrf dengan address dari sisi receiver
  radio.begin();
  //menggunakan address atau alamat yang sudah didefinisikan sebelumnya
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
  Serial.print("Receiver Started, ID: ");
  Serial.print("Connecting to ");
  Serial.println(ssid);
  Serial.println();
  //melakukan connect ke wifi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

}
 
void loop()
{
    //mengatur tampilan pada lcd untuk baris atas temperatur dan baris bawah humidity
    lcd.setCursor(0,0);
    lcd.print("Temp : ");
    lcd.setCursor(7,0);
    lcd.print(temperature);
    lcd.setCursor(0,1);
    lcd.print("Hum : ");
    lcd.setCursor(6,1);
    lcd.print(humidity);

    //melakukan pengecekan apabila data nrf diterima
    if (radio.available()) {
      //menyiapkan variabel char array rx untuk penerimaan data
      char rx[32] = "";
      //membaca data yang diterima lalu dimasukkan ke variabel rx
      radio.read(&rx, sizeof(rx));
      //melakukan pengecekan header data apabila H maka dimasukkan ke data humidity
      if(rx[0]=='H'){
        char* hum = &rx[1];
        humidity  = atof(hum);
        //Serial.println(humidity);
      }
      //melakukan pengecekan header data apabila T maka dimasukkan ke data temperatur
      if(rx[0]=='T'){
        char* temp = &rx[1];
        temperature  = atof(temp);
        //Serial.println(temperature);
 
        Serial.println("--- Data retrieved from device ---");
        //melakukan upload data temperatur dan humidity ke server thingspeak melalui port 80
      if (client.connect(server, 80)) { // "184.106.153.149" or api.thingspeak.com
        String postStr = apiKey;
        postStr += "&field1=";
        postStr += String(temperature);
        postStr += "&field2=";
        postStr += String(humidity);
        postStr += "&field3=";
        
        client.print("POST /update HTTP/1.1\n");
        client.print("Host: api.thingspeak.com\n");
        client.print("Connection: close\n");
        client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
        client.print("Content-Type: application/x-www-form-urlencoded\n");
        client.print("Content-Length: ");
        client.print(postStr.length());
        client.print("\n\n");
        client.print(postStr);
        
        Serial.println("---- Data sent to Thingspeak ----");
        Serial.print(", Temperature:");
        Serial.print(temperature);
        Serial.print(", Humidity:");
        Serial.println(humidity);
      }
      delay(500);
      //memutus hubungan ke server
      client.stop();
      }
    } 
    else 
    {
    //Serial.print("No New Message"); 
    }
}
