#include <nRF24L01.h>
#include <RF24.h>
#include "DHT.h"
#include <EEPROM.h>

//Mendeklarasikan fungsi RF24 dengan nama "radio" pada pin 8 (CE) dan 10 (CSN)
RF24 radio(8, 10); // CE, CSN

//Mendefinisikan pin input dan tipe DHT22 
#define DHTPIN 2
#define DHTTYPE DHT22
//Mendeklarasikan fungsi dht dengan nama "dht"
DHT dht(DHTPIN, DHTTYPE);

//Mendefinisikan address atau alamat komunikasi NRF
const byte address[6] = "00001";
//Deklarasi variabel untuk menyimpan nilai humidity dan temperatur
float humidity, temperature;

void setup()
{
  //memulai komunikasi serial dengan baud rate 115200
  Serial.begin(115200);
  //memulai komunikasi nrf dari sisi transmitter
  radio.begin();
  //menggunakan address atau alamat yang sudah didefinisikan sebelumnya
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
  //memulai pembacaan sensor dht
  dht.begin();
}
 
void loop() 
  {
  //mengambil nilai humidity dan temperatur dari dht22
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  //proses pemberian label pada nilai yang digunakan untuk filter data mana yang humid mana yang temperatur pada receiver nanti
  //memberikan label H didepan nilai humidity sekaligus convert integer dalam bentuk String
  String hum = "H" + String(humidity);
  //memberikan label T didepan nilai temperature sekaligus convert integer dalam bentuk String
  String temp = "T" + String(temperature);
  //deklarasi variable humidity dan temperature dalam bentuk char array sebelum dikirim melalui nrf
  char humchar[10]; 
  char tempchar[10];
  //mengubah String hum dan temp dalam bentuk char array agar dapat dikirimkan melalui nrf
  hum.toCharArray(humchar, 10);
  temp.toCharArray(tempchar, 10);
  //mengirimkan humchar dan tempchar melalui nrf.
  radio.write(&humchar, sizeof(humchar));
  radio.write(&tempchar, sizeof(tempchar));
  Serial.println(humchar);
  Serial.println(tempchar);
  //memberikan delay 15 detik agar pada tiap pengiriman sama dengan update value dari thingspeak
  delay(30000);
  }
