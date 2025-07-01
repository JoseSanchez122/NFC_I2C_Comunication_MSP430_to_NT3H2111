#include "msp430fr60471.h"
#include "sys/cdefs.h"
#include <msp430.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

volatile unsigned char UART3BUFF[512];      //arreglo para almacenar lo que se recive por UART3
volatile unsigned char UART0BUFF[512];      //arreglo para almacenar lo que se recive por UART0
volatile unsigned int INDEX_UART3 = 0;      //indice del arreglo de UART3
volatile unsigned int INDEX_UART0 = 0;      //indice del arreglo de UART0

volatile int UART0_MSG_RECEIVED = 0;        //variables para indicar que se recivio algo por UART
volatile int UART3_MSG_RECEIVED = 0;

void UART0_INIT();
void uartA0_print(const char *cadena);


/*--------------------------------
Funcion: UART0_INIT
Parametros de entrada: N/A
Parametros de salida: N/A
Descripcion general: funcion para inicializar UART0 con 115200 baud
usando SMCLK como fuente de reloj
--------------------------------*/
void UART0_INIT(){
    UCA0IFG &= ~UCTXIFG;                // Limpia flags de transmisión pendientes
    P2SEL0 &= ~(BIT0 | BIT1);           // Configura pines P2.0 (RXD) y P2.1 (TXD)
    P2SEL1 |= BIT0 | BIT1;              // para funciones UART

    PM5CTL0 &= ~LOCKLPM5;               // Desbloquea los pines en modo LPM5

    UCA0CTLW0 = UCSWRST;                // Coloca eUSCI en modo reset
    UCA0CTLW0 |= UCSSEL__SMCLK;         // Selecciona SMCLK como fuente de reloj

    UCA0BRW = 4;                        // Divisor entero para el baud rate
    UCA0MCTLW = UCOS16 | UCBRF_5 | 0x49; // Modulación para 115200 baud

    UCA0CTLW0 &= ~UCSWRST;              // Saca de reset el módulo eUSCI
    UCA0IE |= UCRXIE;                   // Habilita la interrupción de recepción
}



/*--------------------------------
Funcion: uartA0_print
Parametros de entrada: direccion de una cadena de valores char
Parametros de salida: N/A
Descripcion general: recive una cadena string y la imprime por UART0
--------------------------------*/
void uartA0_print(const char *cadena){
    int i = 0;  // Inicialización del índice
    while (cadena[i] != '\0') {  // Verifica el carácter nulo como fin de cadena
        while (!(UCA0IFG & UCTXIFG));  // Espera hasta que el buffer de transmisión esté listo
        UCA0TXBUF = cadena[i];  // Envía el carácter actual
        i++;  // Incrementa el índice
    }
}

/*--------------------------------
Funcion: uartA0_print_1DIG
Parametros de entrada: variable tipo char
Parametros de salida: N/A
Descripcion general: imprime solo un dato tipo char por UART0
--------------------------------*/
void uartA0_print_1DIG(char value){
    while (!(UCA0IFG & UCTXIFG));  
    UCA0TXBUF = value;
}

void uartA0_print_array(uint8_t *array, const uint8_t size){
    uint8_t i = 0;

    for(i=0; i < size; i++){
        uartA0_print_1DIG(array[i]);
    }
    uartA0_print("\n");
}

/*--------------------------------
Funcion: uartA0_print_code
Parametros de entrada: variable tipo int
Parametros de salida: N/A
Descripcion general: recive un entero y lo imprime por UART0
--------------------------------*/
void uartA0_print_code(uint32_t code){
    int i = 0;
    char array[10];

    sprintf(array, "%d", code);

    while (array[i] != '\0') {
        while (!(UCA0IFG & UCTXIFG));  
        UCA0TXBUF = array[i];
        i++;
    }
    while (!(UCA0IFG & UCTXIFG));  
    UCA0TXBUF = '\n';
}

/*--------------------------------
Funcion: uartA0_print_decimal_code
Parametros de entrada: variable tipo float
Parametros de salida: N/A
Descripcion general: recive un numero decimal y lo imprime por UART0
--------------------------------*/
void uartA0_print_decimal_code(float code){
    int i = 0;
    char array[20];
    int intPart; // Parte entera
    int decimalPart; // Parte decimal con 6 dígitos
    float temporary_val;

    if(code < 0.0){
        intPart = (int)code*-1;
        if (intPart == 0){
            temporary_val = (code + 1.0);
            decimalPart = (int)((1-temporary_val)* 1000.0);
            sprintf(array, "-%d.%d", intPart, decimalPart);
        }
        else{
            temporary_val = ((intPart+code)*-1000.0);
            decimalPart = (int)(temporary_val);
            sprintf(array, "-%d.%d", intPart, decimalPart);
        }
    }
   
    if(code >= 0.0){
        intPart = (int)code; 
        decimalPart = (int)((code - intPart) * 1000);
        sprintf(array, "%d.%d", intPart, decimalPart);
    }

    while (array[i] != '\0') {
        while (!(UCA0IFG & UCTXIFG));
        UCA0TXBUF = array[i++];
    }

    while (!(UCA0IFG & UCTXIFG));
    UCA0TXBUF = '\n';
}


/*--------------------------------
Funcion: USCI_A0_ISR
Parametros de entrada: N/A
Parametros de salida: N/A
Descripcion general: interrupcion de UART0, guarda lo que reciva por UCA0RXBUF
en el arreglo UART0BUFF y activa la variable UART0_MSG_RECEIVED para indicar que 
se recivio un mensaje
--------------------------------*/
#pragma vector=EUSCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
{
    switch(__even_in_range(UCA0IV, USCI_UART_UCTXCPTIFG))
    {
        case USCI_NONE: break;
        case USCI_UART_UCRXIFG:
            INDEX_UART3 = 0;                    //Variable para indicar que se recivio dato por UART
            while (!(UCA0IFG & UCTXIFG));
            //UCA3TXBUF = UCA0RXBUF; // pasa los comandos que se le manden al UART0 al UART3
            UART0BUFF[INDEX_UART0] = UCA0RXBUF; //almacenar cada byte en un buffer 
            INDEX_UART0++;                      //incrementar el indice del buffer
            if(UCA0RXBUF == '\n'){              //si se detecta un new line en el buffer indica que ya termino el mensaje
                INDEX_UART0 = 0;                //se resetea el indice para cuando se vuelvan a recivir datos
                UART0_MSG_RECEIVED = 1;         //se levanta la bandera para indicar que llego un mensaje 
            } 
            if (INDEX_UART0==512) INDEX_UART0=0;
            break;
            __no_operation();
            break;
        case USCI_UART_UCTXIFG: break;
        case USCI_UART_UCSTTIFG: break;
        case USCI_UART_UCTXCPTIFG: break;
        default: break;
    }
}

/*--------------------------------
Funcion: READ_UART0_BUFF
Parametros de entrada: cadena que se espera leer
Parametros de salida: variable bool
Descripcion general:  reciva la cadena a leer y la compara con lo almacenado en UART0BUFF
si es igual regresa true si no regresa false
--------------------------------*/
bool READ_UART0_BUFF(const char *expected_result){
    bool verify_cmd = true;
    int i;
    int lng=0;

    while (*expected_result++!='\0') lng++;
    i = lng;

    while (i>=0) {
        *expected_result--;
        i--;
    }

    for(i=0; i<(lng-1); i++){
        if(expected_result[i] != UART0BUFF[i]){
            verify_cmd = false;
        }
    }
    return verify_cmd;
}

/*--------------------------------
Funcion: CLEAR_UART0BUFF
Parametros de entrada: N/A
Parametros de salida: N/A
Descripcion general: limpia todo lo que tenga almacenado UART0BUFF
--------------------------------*/
void CLEAR_UART0BUFF(void){
    int i;
    for (i = 0; i < 512; i++) {
        UART0BUFF[i] = '\0';
    }
}
