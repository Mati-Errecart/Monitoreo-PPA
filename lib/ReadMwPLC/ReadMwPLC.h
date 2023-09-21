#ifndef ReadMwPLC_h
#define ReadMwPLC_h

#include <Arduino.h>

class ReadMwPLC
{   
     public:
        ReadMwPLC(uint16_t varMw[50]); // conexion plc
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