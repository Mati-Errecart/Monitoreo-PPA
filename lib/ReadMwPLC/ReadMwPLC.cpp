#include <ReadMwPLC.h>
#include <Arduino.h>
#include <Ethernet.h>
#include <ModbusEthernet.h>
#include <ThingSpeak.h>
#include <ESP8266HTTPClient.h>
#include <UrlEncode.h>

byte mac[] = {0x52, 0x02, 0x91, 0x78, 0xF9, 0xD4}; ///MAC del controlador ESP8266
IPAddress ip(192, 168, 51, 153); // IP del taller
ModbusEthernet MB;               // objeto intancia Modbus TCP
IPAddress PLC(192, 168, 56, 210); // IP del PLC 

int _bit; // bit a leer

//numero de telefeno para notificaciones de wtsp
String num_telefono = "+5493447552991"; 
void sendMessage(String message);

// //// datos thingspeak
const int Campo_1 = 1;
const int Campo_2 = 2;
const int Campo_3 = 3;
const int Campo_4 = 4;
WiFiClient client;


ReadMwPLC::ReadMwPLC(uint16_t varMw[]) //
{   
    for(int i=0; i<10; i++)
    {
        _VarMW[i]= varMw[i];
    } 
}
void ReadMwPLC::Conect()
{   
      #if defined(AVR_LEONARDO)
      while (!Serial) {}        //
      #endif
      Ethernet.init(16);         //CS pin
      Ethernet.begin(mac, ip);  // Inicia enlace Ethernet 
      delay(1000);              // tiempo de espera para conectar el shield Ethernet
      MB.client();

      ThingSpeak.begin(client);
}

void ReadMwPLC::ReadBegin()   // metodo para leer datos 
{   
    if (MB.isConnected(PLC)) // Compruebo si la conexion se ah establecido
    {   for(int i=0; i<10; i++)
        MB.readHreg(PLC, _VarMW[i], &_Valor[i]); // Leo las word del PLC en sala de comando
    }
    else 
    {
        MB.connect(PLC);   // intenta conectarse si no lo hizo
    } 
    delay(100);    //
    MB.task();     // Tarea Modbus local comun
}
  //Realizar monitoreo solo en modo produccion revisar plc con 
void ReadMwPLC::getEstado() 
{   
    Serial.println("----------------TEMPERATURAS SALA DE PELADO-------------------");
    Serial.println("ESCALDADOR 2");
    Serial.println(float(_Valor[6])/10); //escaldador 2
    Serial.println("ESCALDADOR 1");
    Serial.println(float(_Valor[7])/10); //escaldador 1
    Serial.println("CALDERA");
    Serial.println(float(_Valor[8])/10); //caldera
    Serial.println("PELADORAS");
    Serial.println(float(_Valor[9])/10); //peladoras
    Serial.println();

    for(int i=0; i<6; i++)
    {
        if(i==0) // Mw102 FALLAS
        {
            for(int j=12; j<16; j++)
            {
                _bit= bitRead(_Valor[i],j);
            
                switch (j)
                {
                    case 12:
                    if(_bit!=0) 
                    {
                        Serial.println("Falla Q1 VDF M. BACHA SANGRADO");
                        sendMessage("Falla Q1 Variador Mando BACHA SANGRADO!");
                    }
                    break;
                    case 13:
                    if(_bit!=0)
                    {
                        Serial.println("Falla Q2 VDF M. ESCALDADOR");
                        sendMessage("Falla Q2 Variador Mando ESCALDADOR!");
                     }
                    break;
                    case 14:
                    if(_bit!=0)
                    {
                        Serial.println("Falla Q3 VDF Mando ESTIMULADOR");
                        sendMessage("Falla Q3 Variador Mando ESTIMULADOR!");
                    }
                    break;
                    case 15:
                    if(_bit!=0) 
                    {
                        Serial.println("Falla Q4 VDF M. TRANSFER");
                        sendMessage("Falla Q4 Variador Mando TRANSFER!");
                    }
                    break;
                }
            }
            
        }
        if(i==1) ///// Mw103 FALLAS
        {
            for(int j=0; j<8; j++)
            {
                _bit= bitRead(_Valor[i],j);
                switch (j)
                {
                case 0:
                    if(_bit!=0)
                    {
                        Serial.println("Falla Q5 VDF M. PLAYA");
                        sendMessage("Falla Q5 Variador Mando PLAYA!");
                    }
                    break;
                case 1:
                    if(_bit!=0)
                    {
                        Serial.println("Falla VDF ATV31 DEGOLLADOR");
                        sendMessage("Falla Variador ATV31 Mando DEGOLLADOR!");
                    }
                    break; 
                case 3:
                    if(_bit!=0)
                    {
                        Serial.println("Falla U5 VDF M. BACHA SANGRADO");
                        sendMessage("Falla U5 Variador Mando BACHA SANGRADO!");
                    }
                    break;
                case 4:
                    if(_bit!=0)
                    {
                        Serial.println("Falla U2 VDF M. ESCALDADOR");
                        sendMessage("Falla U2 Variador Mando ESCALDADOR!");
                    }
                    break;  
                case 5:
                    if(_bit!=0)
                    {
                        Serial.println("Falla U4 VDF M. ESTIMULADOR");
                        sendMessage("Falla U4 Variador Mando ESTIMULADOR!");
                    }
                    break; 
                case 6:
                    if(_bit!=0)
                    {
                        Serial.println("Falla U6 VDF M. TRANSFER");
                        sendMessage("Falla U6 Variador Mando TRANSFER!");
                    }
                    break;
                case 7:
                    if(_bit!=0)
                    {
                        Serial.println("Falla U8 VDF M. PLAYA");
                        sendMessage("Falla U8 Variador Mando PLAYA!");
                    }
                    break;             
                }
            }
        }
        if(i==2) ///// Mw105
        {
            _bit= bitRead(_Valor[i],15);
            if(_bit!=0)
            {
                Serial.println("PARADA EMERGENCIA: -CHILLERS-");
                sendMessage("PARADA EMERGENCIA: -CHILLERS-");
            }
        }
        if(i==3) ///// Mw106 ESTADO DE NORIA Y PARADAS DE EMERGENCIA
        {
            for(int j=0; j<16; j++)   
            {
                _bit= bitRead(_Valor[i],j);
                switch (j)
                {
                case 0:
                    if(_bit!=0)
                    {
                        Serial.println("NORIA DE PELADO EN MARCHA");
                    }
                    else
                    {
                        Serial.println("NORIA DE PELADO DETENIDA");
                        sendMessage("NORIA DETENIDA");
                    }
                    break;
                case 4:
                    if(_bit!=0)
                    {
                        Serial.println("PARADA EMERGENCIA: -EVISCERADO-");
                        sendMessage("PARADA EMERGENCIA: -EVISCERADO-");
                    }
                    break;
                case 7:
                    if(_bit!=0)
                    {
                        Serial.println("PARADA EMERGENCIA: -PUERTA TABLERO-");
                        sendMessage("PARADA EMERGENCIA: -PUERTA TABLERO-");
                    }
                    break;
                case 8:
                    if(_bit!=0)
                    {
                        Serial.println("PARADA EMERGENCIA: -PLAYA-");
                        sendMessage("PARADA EMERGENCIA: -PLAYA-");

                    }
                    break;
                case 9:
                    if(_bit!=0)
                    {
                        Serial.println("PARADA EMERGENCIA: -DEGOLLADOR-");
                        sendMessage("PARADA EMERGENCIA: -DEGOLLADOR-");
                    }
                    break;
                case 12:
                    if(_bit!=0)
                    {
                        Serial.println("PARADA EMERGENCIA: -TUNEL SANGRADO-");
                        sendMessage("PARADA EMERGENCIA: -TUNEL SANGRADO-");
                    }
                    break;
                case 11:
                    if(_bit!=0)
                    {
                        Serial.println("PARADA EMERGENCIA: -ESCALDADOR 1-");
                        sendMessage("PARADA EMERGENCIA: -ESCALDADOR 1-");
                    }
                    break;
                case 14:
                    if(_bit!=0)
                    {
                        Serial.println("PARADA EMERGENCIA: -ESCALDADOR 2-");
                        sendMessage("PARADA EMERGENCIA: -ESCALDADORR 2-");
                    }
                    break;
                case 15:
                    if(_bit!=0)
                    {
                        Serial.println("PARADA EMERGENCIA: -SACA GARRA-");
                        sendMessage("PARADA EMERGENCIA: -SACA GARRA-");
                    }
                    break;
                }
            }
        }
        if(i==4) ///// Mw107 PARADAS DE EMERGENCIA
        {
            for(int j=0; j<3; j++)
            {
                switch (j)
                {
                case 0:
                    if(_bit!=0)
                    {
                        Serial.println("PARADA EMERGENCIA: -ARRANCA CABEZA-");
                        sendMessage("PARADA EMERGENCIA: -ARRANCA CABEZA-");
                    }
                    break;
                case 1:
                    if(_bit!=0)
                    {
                        Serial.println("PARADA EMERGENCIA: -CORTA TOQUITOS-");
                        sendMessage("PARADA EMERGENCIA: -CORTA TOQUITOS-");
                    }
                    break;
                }
            }
        }
        if(i==5) ///// Mw118 FALLAS
        {
            for(int j=0; j<6; j++)
            {
                _bit= bitRead(_Valor[i],j);
                switch (j)
                {
                case 0:
                    if(_bit!=0)
                    {
                        Serial.println("FALLA Bomba Alimentacion T.Exterior");
                        sendMessage("FALLA Bomba Alimentacion T.Exterior!");
                    }
                    
                    break;
                case 2:
                    if (_bit!=0)
                    {
                        Serial.println("FALLA CALDERA");
                        sendMessage("FALLA CALDERA!");
                    }
                    
                    break;
                case 3:
                    if(_bit!=0)
                    {
                        Serial.println("PARADA EMERGENCIA: -GANCHO CAIDO-");
                        sendMessage("PARADA EMERGENCIA: -GANCHO CAIDO-");
                    }
                    
                    break;
                case 4:
                    if(_bit!=0)
                    {
                        Serial.println("PARADA DE EMERGENCIA PUENTEADA, REVISAR!");
                        sendMessage("PARADA DE EMERGENCIA PUENTEADA, REVISAR!");
                    }
                    
                    break;
                case 5:
                    if(_bit!=0)
                    {
                        Serial.println("ALARMA: -TEMPERATURA BAJA DE ESCALDADORES-");
                        sendMessage("ALERTA, TEMPERATURA BAJA DE ESCALDADORES!");
                    }
                     
                    break;
                default:
                    break;
                }
            }
        }
    }
}
void ReadMwPLC::UpdateDB() //metodo para actualizar base de datos
{
    Serial.println();
    Serial.println("---------------ACTUALIZANDO BASE DE DATOS----------------------");
    ThingSpeak.writeField(SECRET_CH_ID, Campo_1, (float(_Valor[8]))/10, SECRET_WRITE_APIKEY); //120 t° caldera
    ThingSpeak.writeField(SECRET_CH_ID, Campo_2, (float(_Valor[9]))/10, SECRET_WRITE_APIKEY); //113 t° peladoras
    ThingSpeak.writeField(SECRET_CH_ID, Campo_3, (float(_Valor[7]))/10, SECRET_WRITE_APIKEY); //110 t° esc1
    ThingSpeak.writeField(SECRET_CH_ID, Campo_4, (float(_Valor[6]))/10, SECRET_WRITE_APIKEY); //115 t° esc2
}

void sendMessage(String message) //// METODO PARA ENVIAR NOTIFICACIONES A WHATSAPP
{
  // Datos para enviar con HTTP POST
  String url = "http://api.callmebot.com/whatsapp.php?phone=" + num_telefono + "&apikey=" + SECRET_API_KEY + "&text=" + urlEncode(message);
  WiFiClient client;    
  HTTPClient http;
  http.begin(client, url);

  // Especifica encabezado del tipo de contenido
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  
  // Enviar solicitud HTTP POST
  int httpResponseCode = http.POST(url);
  if (httpResponseCode == 200)
  {
    Serial.print("MENSAJE DE WHATSAPP ENVIADO");
  }
  else{
    Serial.println("ERROR al enviar mensaje");
    Serial.print("HTTP response code: ");
    Serial.println(httpResponseCode);
  }

  // Recursos gratuitos
  http.end();
}