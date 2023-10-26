#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <SPI.h> 
#include <ReadMwPLC.h>

/// arreglo para leer las variables del plc//
uint16_t varM221[]={102,103,105,106,107,118,115,110,120,113}; 
ReadMwPLC PLC221(varM221);

void setup()
{
    Serial.begin(9600);
    int LED = 2; // led conectando wifi
    pinMode(LED, OUTPUT); 
    pinMode(5, INPUT);
    
    /////////////////////////////////SECUENCIA CONEXION WIFI //////////////////////////
    WiFi.begin(SECRET_SSID, SECRET_PASS);
    Serial.print("Conectando a red WiFi");
    while (WiFi.status() != WL_CONNECTED)
    {
      Serial.print(".");
      delay(300);
      // enciende el LED 
      digitalWrite(LED, HIGH);
      delay(1000); // espera 1 segundo 
      digitalWrite(LED, LOW); 
      // apaga el LED
      delay(1000); // espera 1 segundo
    }
      Serial.println("------------------------");
      Serial.println("Conectado con la IP:");
      Serial.println(WiFi.localIP());
      Serial.println("------------------------");
      Serial.print("\nDiagnóstico...\n");
      WiFi.printDiag(Serial); //Muestra la configuración de WiFi por puerto serie
      Serial.println("------------------------\n");
      Serial.printf("MAC Address = %s\n", WiFi.softAPmacAddress().c_str());

    ////////////////////// CONEXION CON PLC //////////////////////////// 
    PLC221.Conect();
}

const int32_t showDelay = 300000;   // delay 5min 300000
uint32_t showLast = 0;

void loop()
{
    PLC221.ReadBegin(); //
    PLC221.getEstado(); ////

  if (millis() - showLast > showDelay) { 
    showLast = millis();
    Serial.println(" -------------------MONITOREO PPA CAMELIAS SALA DE PELADO------------------- ");
    PLC221.UpdateDB();  // ACTUALIZA BASE DE DATOS
    Serial.println();
  }
}
