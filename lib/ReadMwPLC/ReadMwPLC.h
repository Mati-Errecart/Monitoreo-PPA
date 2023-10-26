#ifndef ReadMwPLC_h
#define ReadMwPLC_h
#include <Arduino.h>

 ////////// WIFI datos
#define SECRET_SSID "camelias-externos" //"Redmi Note 8"
#define SECRET_PASS "lcinvita2" // "0000mati"

//// datos thingspeak
#define SECRET_CH_ID 2251797			// id del canal
#define SECRET_WRITE_APIKEY "WH8KOIMZN34TR97Y"   // llave pra escribir en el canal 

/////// clave api y numero de telefono
#define SECRET_API_KEY "8454636"

class ReadMwPLC
{   
     public:
        ReadMwPLC(uint16_t varMw[100]); // conexion plc
        void Conect();
        void ReadBegin();// inicio lectura  uint16_t varMw[10]                      
        void UpdateDB(); // envio datos a firebase
        
        void getEstado();
    
    private:
        uint16_t _VarMW[50]; // MW de PLC
        uint16_t _Valor[50];
        uint16_t _leido;            
};

#endif