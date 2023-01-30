#include "lib/include.h"

//configuración de UART4 que corresponde al PC4 y PC5 pag 895
extern void Configurar_UART0(void)
{
    //CONFIGURACIÓN DE LOS PINES
    //Paso 1 (RCGCUART) pag.344 habilita UART4  0->Disable 1->Enable
    SYSCTL->RCGCUART  = (1<<3);   
    //Paso 2 (RCGCGPIO) pag.340 habilitaR puerto C 
    SYSCTL->RCGCGPIO |= (1<<2);     
    //(GPIOAFSEL) pag.688 Habilita función alternativa
    GPIOC->AFSEL = (1<<7) | (1<<6); //GPIO Port Control (GPIOPCTL) Habilita función alternativa en el pin C4 y C5 pag.688
    //Indicar entradas y salidas pag. 895 
            //      Tx(in) | Rx(out)
    GPIOC -> DIR |= (1<<7) | (0<<6); //Transmisionn -> 1 y recepción -> 0
    //DesabilitaR resistencias pull up Y pull down 
    GPIOC -> PUR |= (0<<6) | (0<<7);
    GPIOC -> PDR |= (0<<6) | (0<<7);
    //Para indicar que el pin 6 y 7 funcionan como UART, de acuerdo a los bits de la tabla de AFSEL pag. 672
    GPIOC->PCTL = (1<<28) | (1<<24);  //pag. 1351 para conocer que valor poner
    // ejemplo con A1 y A0: (1<<0) | (1<<4); otra forma de ponerlo: (GPIOA->PCTL&0xFFFFFF00) | 0x00000011;
    // Habilitar el pin como digital GPIO (GPIODEN) pag.682
    GPIOC->DEN = (1<<6) | (1<<7);//PC6 y pc7
    // Desabilita/Limpia los registros pag. 918
    UART3->CTL = (0<<9) | (0<<8) | (0<<0); //bit 9 es de Tx, bit 8 de Rx, y 0 al uart en genral. 
    // UART Integer Baud-Rate Divisor (UARTIBRD) pag.914
    /*     velocidad  (estandar*baud Rate)
    BRD = 80*10^6 / (16 * 19200) = 260.4       yo: 80MHZ Baud-rate 19200
    UARTFBRD[DIVFRAC] = integer(0.4 * 64 + 0.5) = 27 */
    UART3->IBRD = 260;
    // UART Fractional Baud-Rate Divisor (UARTFBRD) pag.915 */
    UART3->FBRD = 27;  /* (parte fraccionaria del calculo) */
    //  UART Line Control (UARTLCRH) pag.916 bits
    UART3->LCRH = (0x3<<5);  //Para que se tengan 8 bits, y |(1<<4); activar el bit de paridad pero solo con el uart0
    //  UART Clock Configuration(UARTCC) pag.939
    UART3->CC =(0<<0); /*PARA USAR EL RELOJ INTERNO*/
    //Disable UART0 UART Control (UARTCTL) pag.918
    UART3->CTL = (1<<0) | (1<<8) | (1<<9);   //se habilitan los pines de recepción, el pin 8 y 9 se deben prender juntos
}

extern char readChar(void)
{
    //UART FR flag pag 911
    //UART DR data 906
    int v;  //BANDERA
    char c;  
    while((UART3->FR & (1<<4)) != 0 ); //se mantiene en el ciclo cuando no llega un dato 
    v = UART3->DR & 0xFF; //DR - registro del dato y 0xff es la mascara 
    c = v; //convertir el entero a caracter
    return c; //solo regresa un caracter 
}
extern void printChar(char c) //mandar un caracter
{
    while((UART3->FR & (1<<5)) != 0 ); //pregunta si el dato 5 está desocupado, si lo está sigue con la linea de datos y transmite
    UART3->DR = c; 
}
extern void printString(char* string)  //para enviar cadena 
{
    while(*string) //string es la direccion de memoria para saber si está vacía o np, si está vacia se sale del ciclo
    {
        printChar(*(string++)); 
    }
}

extern char * readString(char delimitador) //para invertir 
{

   int i=0;
   char *string = (char *)calloc(10,sizeof(char));
   char c = readChar();
   while(c != delimitador)
   {
       *(string+i) = c;
       i++;
       if(i%10==0)
       {
           string = realloc(string,(i+10)*sizeof(char));
       }
       c = readChar();
   }
   return string;
}
//Experimento 2

//El envio es su nombre  (jesus) 

// invertirlo y regresarlo con numeros consecutivos
// entre letras (s1u2s3e5j) 

