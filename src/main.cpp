#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ModbusIP_ESP8266.h>
#include <SPI.h> 
#include <ReadMwPLC.h>
//#include <HTTPClient.h>

#define WIFI_SSID "Canal10-Otto 2023"//  "camelias-externos" //"Redmi Note 8"
#define WIFI_PASSWORD "43294797otto"//"lcinvita2" // "0000mati" 


/// arreglo para leer todas las variables //
uint16_t varM221[10]={102,103,105,106,107,118,115,110,120,113}; 
ReadMwPLC PLC221(varM221);

ModbusIP mb;  //objeto ModbusIP

void setup()
{
    Serial.begin(9600);
    int LED = 2; // led conectando wifi
    pinMode(LED, OUTPUT); 
    pinMode(5, INPUT);
    
    /////////////////////////////////SECUENCIA CONEXION WIFI //////////////////////////
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Conectando a red camelias-externos");
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

      mb.client();
	
    ////////////////////// CONEXION CON PLC //////////////////////////// 

    //PLC221.Conect(); COMENTADO PARA PROBAR WSP
}

const int32_t showDelay = 60000;   // muestra valores cada 5min 300000 mills
uint32_t showLast = 0;

void loop()
{
    //PLC221.ReadBegin(); //COMENTADO PARA PROBAR WSP
    
    
  if (millis() - showLast > showDelay) { // Muestra el valor del registro cada 5 segundos
    showLast = millis();
    Serial.println(" -------------------MONITOREO PPA CAMELIAS SALA DE PELADO------------------- ");
    PLC221.getEstado();
    Serial.println();
    
    //PLC221.UpdateDB();  // ACTUALIZO DATOS BASE DE DATOS //COMENTADO PARA PROBAR WSP
  }
  
}
