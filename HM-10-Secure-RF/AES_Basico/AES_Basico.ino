/* ....................................................  // The source for the Android application can be found at the following link: https://github.com/Lauszus/ArduinoBlinkLED
  The code for the Android application is heavily based on this guide: http://allaboutee.com/2011/12/31/arduino-adk-board-blink-an-led-with-your-phone-code-and-explanation/ by Miguel
  SegÃƒÂºn la Cerradura sea para Casa o Garaje crearemos una variable en setUp que nos lo indicarÃƒÂ¡
  La seguridad dela informaciÃƒÂ³n en los mensajes enviados al Arduino se basarÃƒÂ¡:
  - Cada Arduino(Cerradura) tendrÃƒÂ¡ un cÃƒÂ³digo de 6 bytes ÃƒÂºnico por Arduino que estarÃƒÂ¡ grabado en el cÃƒÂ³digo.
  - Cada mensaje que se envÃƒÂ­e desde el servidor constarÃƒÂ¡:
  -      2 nÃƒÂºmeros que nos dicen cuÃƒÂ¡nto tenemos que rotar la funciÃƒÂ³n XOR para sacar la informaciÃƒÂ³n. (ver cÃƒÂ³digo de funciÃƒÂ³n desencrypt)
  -      4 carÃƒÂ¡cteres de reserva
  -      2 Una cabecera "@@" o "##" para saber si el mensaje tiene el origen en el Servidor o en la Cerradura.
  -     12 nÃƒÂºmeros que nos dicen cÃƒÂ³mo debe actuarse sobre las I/O del Arduino (ver codigo correspondiente)
  -      4 carÃƒÂ¡cteres de reserva
  -      8 nÃƒÂºmeros que es el cadeu vigente y el que debe coincidir con el almacenado en el Arduino
  -      8 nÃƒÂºmeros que es el prÃƒÂ³ximo cadeu
  NOTAS IMPORTANTES:
  El nÃƒÂºmero de la entrada y la i empleada para pasar por todas las entradas estÃƒÂ¡n desfasados en 1, por ejemplo cuando i=0, la
  entrada es 1.
  Hasta el segundo minuto despuÃƒÂ©s de la conexiÃƒÂ³n no se envÃƒÂ­an acciones 51 de aviso cambio en entrada habilitada

  Cuando el contacto de la entrada estÃƒÂ© abierto la seÃƒÂ±al a la entrada es '1' ya que estÃƒÂ¡ polarizado internamente a '1', asÃƒÂ­ pues cuando
  se cierre estarÃƒÂ¡ a '0'.
  Cuando el contacto estÃƒÂ¡ definido en el servidor como normalmente abierto y el  cuando enviemos el status de las entradas lo invertiremos
  si estÃƒÂ¡ a

  Un mÃƒÂ³dulo estÃƒÂ¡ preparado para 8 entradas y 8 salidas, aunque raramente se alcanzarÃƒÂ¡n todas. En principo los mÃƒÂ³dulos serÃƒÂ¡n de 2 entrads y 2 salidas.
  Normalmente ocurrirÃƒÂ¡ que los mÃƒÂ³dulos tengan o solamente entradas o solamente salidas, aunque tambiÃƒÂ©n puede ser que tengan ambas. En cualquier caso
  distinguiremos entre los mÃƒÂ³dulos de solamente entradas y el resto.
  En los mÃƒÂ³dulos de solamente entradas, son los mÃƒÂ³dulos los que tienen la informaciÃƒÂ³n para enviar, es decir, la entrada disparada. Por ello serÃƒÂ¡n
  ellos los que se despertarÃƒÂ¡n mediante interrupciones de hardware cuando el sensor se dispare, tambiÃƒÂ©n deberÃƒÂ¡n despertarse por watch-dog cada 8sg.
  en que se reiniciarÃƒÂ¡ ÃƒÂ©ste hasta llegar al tiempo que definamos para enviar la prueba de vida.
  En los mÃƒÂ³dulos con alguna salida, en que es el maestro el que envÃƒÂ­a la informaciÃƒÂ³n, tienen que despertarse mÃƒÂ¡s frecuentemente para reducir al mÃƒÂ¡ximo
  el tiempo de espera.

  La forma de trabajar serÃƒÂ¡: cuando la centralita recibe una peticiÃƒÂ³n de operaciÃƒÂ³n, se pondrÃƒÂ¡ en modo de conexiÃƒÂ³n con el mÃƒÂ³dulo del MacBTLE enviado y
  esperarÃƒÂ¡ asÃƒÂ­ hasta que este mÃƒÂ³dulo se despierte y se comuniquen. Una vez terminada la operaciÃƒÂ³n el Arduino desconectarÃƒÂ¡ al mÃƒÂ³dulo y se dormirÃƒÂ¡ y la
  centralita cortarÃƒÂ¡ la comunicaciÃƒÂ³n.
  Cuando un mÃƒÂ³dulo sensor se excite, se despertarÃƒÂ¡ por interrupciÃƒÂ³n hardware y pondrÃƒÂ¡ al HM-10 en espera hasta que la centralita en el polling que hace
  se comunique con ÃƒÂ©l. Una vez terminada la comunicaciÃƒÂ³n el Arduino desconectarÃƒÂ¡ el HM-10 y se dormirÃƒÂ¡ y la centralita cortarÃƒÂ¡ la comunicaciÃƒÂ³n.

*/
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include <AESLib.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>


// Interrupciones
#define ITR_0  2 //InterrupciÃƒÂ³n hardware 0
#define ITR_1  3 //InterrupciÃƒÂ³n hardware 1

// Limitamos el nÃƒÂºmero de salidas a un mÃƒÂ¡ximo de 4 para la Cerradura Garaje
// PORTD maps to Arduino digital pins 0 to 7
// PORTB maps to Arduino digital pins 8 to 13 The two high bits (6 & 7) map to the crystal pins and are not usable
#define OUT_1  4 //PORTD 4
#define OUT_2  5 //PORTD 5
#define OUT_3  6 //PORTD 6
#define OUT_4  7 //PORTD 7
#define FREE1   8 //PA0

// Power HM_10
#define PWR_0  9  //AlimentaciÃƒÂ³n HM-10 directa
#define PWR_D  10 //AlimentaciÃƒÂ³n HM-10 a tavÃƒÂ©s de diodo

//ComunicaciÃƒÂ³n btleSerial
#define BTLE_RX  11 //Pin recepciÃƒÂ³n comunicaciÃƒÂ³n serial con HM_10. Se conecta al PIN TX del HM-10, pin 1
#define BTLE_TX  12 //Pin transmisiÃƒÂ³n comunicaciÃƒÂ³n serial con HM_10. Se conecta al PIN RX del HM-10, pin 2

//RESET
#define RESET  13 // Reset del HM-10

// PORTC maps to Arduino analog pins 0 to 5. Pins 6 & 7 are only accessible on the Arduino Mini
#define IN_1  A0 //PORTC 0
#define IN_2  A1 //PORTC 1
#define IN_3  A2 //PORTC 2
#define IN_4  A3 //PORTC 3
#define FREE2  A4 //PA0
#define FREE3  A5 //PA0

SoftwareSerial btleSerial(BTLE_RX, BTLE_TX); //(RX, TX)

//Primer HM-10
//#define skLlave "fn6uaszg4flow3ng"
//#define codeArduString "rfl2dn"

//Segundo HM-10
#define skLlave "6s5q0hhwt8qayo7e"  //must be 16 bytes 
#define codeArduString "um9gnr"

#define versionCodeString "003"

aes_context datoAES;
uint8_t key[]      = skLlave;
char codeArdu[]    = codeArduString;
char versionCode[] = versionCodeString;

uint8_t iv[17];
String ivString;

char data_desc[33];

// DefiniciÃƒÂ³n de las partes del mensaje
String iv1_string = "";
String mens_reci = "";
String mens_encr_strg = "";
String mens_exci_sali = "";

//Provisional
//String mens_prov_strg;

uint8_t number_read_bytes = 0; // To count 20 bytes from buffer of H´-10
uint8_t number_message_bytes = 0;       // To count total bytes received in message

byte byte_leido;

bool message_finished = false;
bool message_started  = false;
bool mensaje_OK = false;
bool is_encrypt = true;
bool input_changing = true; //Para funciÃ³n sleep
bool out_working = true;    //Para funciÃ³n sleep

bool stop_operation = false;

uint8_t nume_oper;
unsigned long initialMillis;
unsigned long secu_guar;
unsigned long secu_reci;

uint8_t timeON;
uint8_t timeOFF;

// Creamos temporizadores por salida ya que puede haber varias salidas trabajando temporizadas
uint16_t timerOut1;
uint16_t timerOut2;
uint16_t timerOut3;
uint16_t timerOut4;

int operacion;
char operacionDisparada;
//int pasoOff;

unsigned long startTimerOut1;
unsigned long startTimerOut2;
unsigned long startTimerOut3;
unsigned long startTimerOut4;

unsigned long millisStartPaso;
unsigned long timerOff;
unsigned long millis_message_started;

// Variables lectura valores inputs
byte status_input;
byte status_input_before;
byte status_input_stable;
byte enable_input;
byte pattern_input;
bool status_input_changed;
bool status_input_repeat;
int vecesRepetido;
int vecesPruebaVida;

char mArray[17];

unsigned long millisCheckInputs;

//volatile boolean goToBed = true;     // Para dormir o no el Arduino
//volatile boolean only_sensors;       // Para saber si solo se usan entradas o tambiÃƒÂ©n salidas
// Si solo entrads se despertarÃƒÂ¡ cada 8 seg. y si hay salidas cada sg.
//volatile int times_8_seconds; //

bool actu_secu = false;
bool success = false;
int i = 0;
uint8_t n = 0;
uint8_t c1 = 0;
uint8_t c2 = 0;

volatile int f_wdt = 0;

void setup() {
  wdt_reset();
  wdt_disable();

  Serial.begin(115200);           //Inicia comunicaciÃƒÂ³n serial
  btleSerial.begin(9600);         //Inicia comunicaciÃƒÂ³n serial BluetoothLE
  Serial.println(F("Comienzo SetUp"));

  pinMode(OUT_1, OUTPUT);
  pinMode(OUT_2, OUTPUT);
  pinMode(OUT_3, OUTPUT);
  pinMode(OUT_4, OUTPUT);
  pinMode(PWR_0, OUTPUT); // To POWER HM-10 from Arduino
  pinMode(RESET, OUTPUT); // To RESET HM-10

  pinMode(IN_1, INPUT_PULLUP); // Para polarizarlo mediante resistencia interna a +.
  pinMode(IN_2, INPUT_PULLUP);
  pinMode(IN_3, INPUT_PULLUP);
  pinMode(IN_4, INPUT_PULLUP);

  digitalWrite(PWR_0, HIGH);

  // First time we initialized EEprom, all position to 0x00
  if (EEPROM.read(511) == 255) {
    Serial.println(F("Memoria NO inicializada"));
    for (i = 0; i < 512; i++) {
      Serial.print(i);
      Serial.print(" ");
      EEPROM.write(i, 0);
    }
  } else {
    Serial.println(F("Memoria YA inicializada"));
    secu_guar = recu_secu(); // If memory initialized recover sequence
  }

  // Wait for HM-10 to be available
  success = false;
  mens_reci = "";
  i = 0;
  n = 0;
  while (!success) {
    btleSerial.print("AT");
    success = false;
    mens_reci = "";
    i = 0;
    n = 0;
    delay(100);
    while (btleSerial.available() && i < 2) { // Para recoger dos bytes
      mens_reci = mens_reci + (char) btleSerial.read();
      i++;
    }

    Serial.print(F("cadena resultado: "));
    Serial.println(mens_reci);
    c1 = mens_reci.indexOf('O');
    //Serial.print(F("IndexOf O: "));
    //Serial.println(c1);
    c2 = mens_reci.indexOf('K');
    //Serial.print(F("IndexOf K: "));
    //Serial.println(c2);
    if (c2 == c1 + 1) {
      Serial.println(F("Recibida orden AT."));
      success = true;
    }
  }

  mens_reci = ""; //Important for not transferring them to iv1

  // Inicializamos valores.
  initialMillis = millis();

  // Deshabilitamos todas las entradas de inicio
  // Hasta que no se produzca la primera conexiÃƒÂ³n con la Cerradura
  // no se habilitarÃƒÂ¡, si ese caso es requerido, ninguna entradas.
  // Aunque tenemos 4 inputs y 4 outputs los registros los preparamos para 8

  status_input        = PINC;       // Current status of Inputs. Initialize with current values
  status_input_before = PINC;       // Previous status of Inputs.Initialize with current values
  status_input_stable = PINC;       // Initialize with current values
  enable_input         = B00000000;  // If bit HIGH we should inform if change happen in that input
  pattern_input        = B00000000;  // We should inform if input byte change to different state than same bit in patron
  // We will inform to server if bit enable HIGH and statusInputsStable opposite to pattern_input bit
  vecesPruebaVida = 0;

  /*** Setup the WDT ***/
  MCUSR &= ~(1 << WDRF); // Clear the reset flag.
  WDTCSR |= (1 << WDCE) | (1 << WDE); // In order to change WDE or the prescaler, we need to set WDCE
  //WDTCSR = 1<<WDP0 | 1<<WDP3;                      // 8.0 seconds
  //WDTCSR = 1<<WDP3;                                // 4.0 seconds
  WDTCSR = (1 << WDP0) | (1 << WDP1) | (1 << WDP2); // 2.0 seconds
  // WDTCSR = (1 << WDP1) | (1 << WDP2);              // 1.0 seconds
  WDTCSR |= (1 << WDIE); // Enable the WD interrupt (note no reset).

  Serial.println(F("Fin SetUp"));
} // end of setup

/***************************************************
    Name:        enterSleep
    Returns:     Nothing.
    Parameters:  None.
    Description: Enters the arduino into sleep mode.
 ***************************************************/
void enterSleep(void) {
  /* Now is the time to set the sleep mode. In the Atmega8 datasheet
     http://www.atmel.com/dyn/resources/prod_documents/doc2486.pdf on page 35
     there is a list of sleep modes which explains which clocks and
     wake up sources are available in which sleep mode.

     In the avr/sleep.h file, the call names of these sleep modes are to be found:

     The 5 different modes are:
         SLEEP_MODE_IDLE         -the least power savings
         SLEEP_MODE_ADC
         SLEEP_MODE_PWR_SAVE
         SLEEP_MODE_STANDBY
         SLEEP_MODE_PWR_DOWN     -the most power savings

     For now, we want as much power savings as possible, so we
     choose the according
     sleep mode: SLEEP_MODE_PWR_DOWN

  */

  /* Now it is time to enable an interrupt. We do it here so an
     accidentally pushed interrupt button doesn't interrupt
     our running program. if you want to be able to run
     interrupt code besides the sleep function, place it in
     setup() for example.

     In the function call attachInterrupt(A, B, C)
     A   can be either 0 or 1 for interrupts on pin 2 or 3.
     B   Name of a function you want to execute at interrupt for A.
     C   Trigger mode of the interrupt pin. can be:
                 LOW        a low level triggers
                 CHANGE     a change in level triggers
                 RISING     a rising edge of a level triggers
                 FALLING    a falling edge of a level triggers
     In all but the IDLE sleep modes only LOW can be used.
  */

  Serial.println(F("En sleepNow"));
  //attachInterrupt(0, wakeUpNow, CHANGE); // use interrupt 0 (pin 2) and run function. WakeUpNow when pin 2 gets CHANGE

  //Disconnect HM-10
  digitalWrite(PWR_0, LOW);
  delay(10); // To wait until the operation is done before sleeping

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);   // sleep mode is set here
  sleep_enable();                        // enables the sleep bit in the mcucr register so sleep is possible. just a safety pin
  sleep_mode();                           // bzzzzzzz here the device is actually put to sleep!!

  Serial.println(F("Yo sigo...."));
  // THE PROGRAM CONTINUES FROM HERE AFTER WAKING UP
  sleep_disable();         // first thing after waking from sleep: disable sleep...
  /* Re-enable the peripherals. */
  power_all_enable();
  detachInterrupt(0);      // disables interrupt 0 on pin 2 so the wakeUpNow code will not be executed
  // during normal running time.
  /* Re-enable the peripherals. */
  power_all_enable();

  // Conectamos el HM-10
  digitalWrite(PWR_0, HIGH);
  Serial.println(F("A seguir trabajando."));
} // end of enteSleep

void loop() {
  f_wdt = 0;
  wdt_disable();
  // Para entrar en Sleep Mode
  /*
    if(goToBed) {
      goToBed = false; //Don't forget to clear the flag.
      sleepNow();      //Re-enter sleep mode.
    } else {

    }
  */
  if (actu_secu) {
    actu_secu_eepr();   // Record sequence in memory next loop after action
    actu_secu = false;
  }

  /*
    // To avoid going to sleep if action started
    if (millis() - initialMillis > 3000){
      Serial.print(F("millis: "));
      Serial.print(millis());
      Serial.print(F("             initialMillis: "));
      Serial.print(initialMillis);
      Serial.print(F("message_started: "));
      Serial.print(message_started);
      Serial.print(F("   input_changing: "));
      Serial.print(input_changing);
      Serial.print(F("    out_working: "));
      Serial.println(out_working);
      if (!message_started && !input_changing && !out_working){
          //enterSleep1();
      }
    }
  */

  if (!message_started) { // Messsage reception started
    millis_message_started = millis(); // If not continuously initialize message time out

  } else {
    // If started check time out
    if (millis() - millis_message_started > 5000) {
      Serial.println(F("MENSAJE TERMINADO ABORTADO POR TIME_OUT"));
      message_finished = true; // Abort message
      // TO DO QUE PASA SI SALTA UNA ENTRADA DURANTE ESTE TIEMP0-----------------------------------------------------------------------------------------------------------------------------
    }
  }

  while (btleSerial.available()) {
    byte_leido = btleSerial.read();
    number_read_bytes++;
    char a = (char) byte_leido;
    Serial.print(a);
    if (number_read_bytes > 20)
    {
      //Something happens buffer of HM-10 only 20 bytes
      Serial.println(F("MENSAJE TERMINADO ABORTADO POR > 20 BYTES"));
      message_finished = true;
    }
    else
    {
      switch (byte_leido) { // Es el ÃƒÆ’Ã‚Âºltimo byte de cada transmisiÃƒÆ’Ã‚Â³n, que nos dice si ha terminado o continua
        case '&': // Start of message
          is_encrypt = true;
          number_read_bytes     = 0; //Iniciamos contador bytes
          number_message_bytes  = 0; // Para empezar en la posiciÃƒÆ’Ã‚Â³n 0
          message_started = true;
          message_finished = false;
          Serial.println(F("MENSAJE EMPEZADO"));
          millis_message_started = millis(); //Para contar tiempo concedido al mensaje
          break;

        case '#': // Message continue, a new part will come
          if (message_started) {
            number_read_bytes = 0; //Iniciamos contador bytes
            btleSerial.write("ACK0000"); // Order new part
            Serial.println(F("PETICION SIGUIENTE MENSAJE"));
            Serial.println(F("FIN DE RECEPCION CORRECTA TRAMO"));
          }
          break;

        case '@': // End of encrypted message
          if (message_started) {
            message_finished = true;
            Serial.print(F("Numer bytes recibidos: "));
            Serial.println(number_message_bytes);
            // Definitivo encriptado
            if (number_message_bytes == 57) { //antes 89
              mensaje_OK = true;
              Serial.println(F("FIN DE RECEPCION CORRECTA MENSAJE"));
            } else {
              Serial.println(F("FIN DE RECEPCION INCORRECTA MENSAJE"));
            }
          }
          break;

        case '$': // End of non-encrypted message. Only must have less than 16 bytes.
          if (message_started) {
            is_encrypt = false;
            message_finished = true;
            iv1_string = mens_reci; //We use iv1string to save memory
            Serial.print(F("Mensaje para no desencriptar: "));
            Serial.println(iv1_string);

            Serial.println(F("FIN DE RECEPCION MENSAJE SIN ENCRIPTAR"));
          }
          break;

        case 'O': // Sometimes the answer OK from UART put in message. Forget it
          number_read_bytes = number_read_bytes - 1;

          break;

        case 'K': // Sometimes the answer OK from UART put in message. Forget it.
          number_read_bytes = number_read_bytes - 1;

          break;

        default:
          // If message started we get the byte, if not it will loose
          if (message_started) {
            mens_reci = mens_reci + (char) byte_leido;
            number_message_bytes++;
            switch (number_message_bytes) {
              case 16: //First 16 bytes is iv
                iv1_string = mens_reci;
                /*
                  i = iv1_string.indexOf('O'); // Por si se ha colado la O de OK desde la UART
                  if (i != -1){
                  iv1_string = iv1_string.substring(0, i) +  iv1_string.substring(i + 1);
                  }
                  i = iv1_string.indexOf('K'); // Por si se ha colado la K de OK desde la UART
                  if (i != -1){
                  iv1_string = iv1_string.substring(0, i) +  iv1_string.substring(i + 1);
                  }
                */
                Serial.print(F("iv1: "));
                Serial.println(iv1_string);
                mens_reci = "\0";
                break;

              case 48: // After iv, 16 bytes of encripted message
                mens_encr_strg = mens_reci;
                Serial.print(F("mens_prov: "));
                Serial.println(mens_reci);
                mens_reci = "\0";
                break;

              case 57: // After encrited message, info to switch outputs
                mens_exci_sali = mens_reci;
                Serial.print(F("mens_prov: "));
                Serial.println(mens_reci);
                mens_reci = "\0";
                break;
            }

          }
          else
          {
            number_read_bytes    = 0; //Iniciamos contador bytes
            number_message_bytes = 0; // Para empezar en la posiciÃƒÆ’Ã‚Â³n 0
          }
          break;
      }
    }
  }

  // Alalize message when finished
  if (message_finished) {
    message_finished = false; // Inicializamos prÃƒÆ’Ã‚Â³ximo mensaje
    message_started  = false;

    if (mensaje_OK) {
      if (is_encrypt) { // Si estÃ¡ encriptado
        Serial.println(F("MENSAJE RECIBIDO OK"));
        delay(100);
        mensaje_OK = false; // Inicializamos mensaje

        // Definitivo encriptado
        // Inicializamos array empleado en desencriptacion
        for (int i = 0; i < 32; i++) {
          data_desc[i] = '\0';
        }

        data_desc[32] = '\0';

        // desencriptamos
        decrypt_data(mens_encr_strg, iv1_string, data_desc);

        // El mensaje desencriptado esta compuesto por lo siguiente:
        //  0- 5 Clave propia del hardware
        //  6-12 Secuencia de operacion
        // 13    Tipo de operacion
        // 14    Numero de operacion
        // 15    Free
        // After decrypt the string received the 6 first characters must be the ArduCode and the next 7 must
        // be a sequence higher than the one store in the EEprom
        for (int i = 0; i < 6; i++) { // Check ArduCode
          if (data_desc[i] != codeArdu[i]) {
            stop_operation = true;
            Serial.print(F("Fallo chequeo arduino code: "));
            break;
          }
        }

        if (!stop_operation) { // Check sequence. Must be higher than the stored in EEprom
          //secu_reci = secu_reci + ((data_desc[6 + i] - '0') * (pow(10, 6 - i))); // The result is not exact for index 5 and 6 ???????
          secu_reci = ((data_desc[6] - '0') * 1000000) + ((data_desc[7] - '0') * 100000) +  ((data_desc[8] - '0') * 10000) +
                      ((data_desc[9] - '0') * 1000)    + ((data_desc[10] - '0') * 100)   +  ((data_desc[11] - '0') * 10)   +
                      ((data_desc[12] - '0'));

          if (secu_guar >= secu_reci) {
            stop_operation = true;
          } else {
            secu_guar = secu_reci;
            actu_secu = true; // Para actualizar la secuencia en el siguiente loop evitando el cuelgue por falta de memoria
          }
          Serial.print(F("Secuencia almacenada: "));
          Serial.println(secu_guar);
          Serial.print(F("Secuencia recibida: "));
          Serial.println(secu_reci);
        }

        if (!stop_operation) {
          char operation_tipe   = data_desc[13];
          char operation_number = data_desc[14];

          Serial.print("operation_tipe: ");
          Serial.println(operation_tipe);

          Serial.print("operation_number: ");
          Serial.println(operation_number);

          // From here one we will use the string received with IO info
          //It contains:
          // (0)     Output to put ON
          // (1)     Output to put OFF
          // ( 2- 4) Time to be ON
          // ( 5- 7) Time to be OFF
          // (8)     Output to Toggle

          switch (operation_tipe) {
            // Las operaciones 0, 1, 2, 3 vienen del servidor
            case '0': // Operaciones Normales. Excitar salidas.
              if (is_encrypt) { //Por seguridad si no estÃ¡ encriptado no puede encender salidas
                // A partir de la denominacion de la salida en el servidor ponemos las salidas del Arduino
                // Para convertir byte a entero emplearemos la expresion (byte - '0')
                timeON = ((mens_exci_sali.charAt(2) - '0') * 100) + ((mens_exci_sali.charAt(3) - '0') * 10) + ((mens_exci_sali.charAt(4) - '0'));
                Serial.print(F("timeON: "));
                Serial.println(timeON);
                switch (mens_exci_sali.charAt(0)) {
                  case '0': // No hay salidas a poner ON
                    Serial.println(F("No hay Salidas a ON"));
                    break;

                  case '1': // '1' Si tenemos que excitar la salida 1.
                    Serial.println(F("Salida 01 a ON"));
                    digitalWrite(OUT_1, HIGH);
                    if (timeON > 0) {
                      timerOut1 = timeON;
                      startTimerOut1 = millis();
                    }
                    break;

                  case '2': // '2'
                    Serial.println(F("Salida 02 a ON"));
                    digitalWrite(OUT_2, HIGH);
                    if (timeON > 0) {
                      timerOut2 = timeON;
                      startTimerOut2 = millis();
                    }
                    break;

                  case '3': // '3'
                    Serial.println(F("Salida 03 a ON"));
                    digitalWrite(OUT_3, HIGH);
                    if (timeON > 0) {
                      timerOut3 = timeON;
                      startTimerOut3 = millis();
                    }
                    break;

                  case '4': // '4'
                    Serial.println(F("Salida 04 a ON"));
                    digitalWrite(OUT_4, HIGH);
                    if (timeON > 0) {
                      timerOut4 = timeON;
                      startTimerOut4 = millis();
                    }
                    break;
                }

                // Para convertir byte a entero emplearemos la expresion (byte - '0')
                timeOFF = ((mens_exci_sali.charAt(5) - '0') * 100) + ((mens_exci_sali.charAt(6) - '0') * 10) + ((mens_exci_sali.charAt(7) - '0'));
                Serial.print(F("timeOFF: "));
                Serial.println(timeOFF);
                switch (mens_exci_sali.charAt(1)) {
                  case '0': // No hay salidas a poner OFF
                    Serial.println(F("No hay Salidas a OFF"));
                    break;

                  case '1': // '1'
                    Serial.println(F("Salida 01 a OFF"));
                    digitalWrite(OUT_1, LOW);
                    if (timeOFF > 0) {
                      timerOut1 = timeOFF;
                      startTimerOut1 = millis();
                    }
                    break;

                  case '2': // '2'
                    Serial.println(F("Salida 02 a OFF"));
                    digitalWrite(OUT_2, LOW);
                    if (timeOFF > 0) {
                      timerOut2 = timeOFF;
                      startTimerOut2 = millis();
                    }
                    break;

                  case '3': // '3'
                    Serial.println(F("Salida 03 a OFF"));
                    digitalWrite(OUT_3, LOW);
                    if (timeOFF > 0) {
                      timerOut3 = timeOFF;
                      startTimerOut3 = millis();
                    }
                    break;

                  case '4': // '4'
                    Serial.println(F("Salida 04 a OFF"));
                    digitalWrite(OUT_4, LOW);
                    if (timeOFF > 0) {
                      timerOut4 = timeOFF;
                      startTimerOut4 = millis();
                    }
                    break;
                }

                switch (mens_exci_sali.charAt(8)) {
                  case '0': // No hay salidas a poner Toggle
                    Serial.println(F("No hay Salidas a Toggle"));
                    break;

                  case '1': // '1'
                    Serial.println(F("Salida 1 Toggle"));
                    if (digitalRead(OUT_1) == HIGH) {
                      digitalWrite(OUT_1, LOW);
                    } else {
                      digitalWrite(OUT_1, HIGH);
                    }
                    break;

                  case '2': // '2'
                    Serial.println(F("Salida 2 Toggle"));
                    if (digitalRead(OUT_2) == HIGH) {
                      digitalWrite(OUT_2, LOW);
                    } else {
                      digitalWrite(OUT_2, HIGH);
                    }
                    break;

                  case '3': // '3'
                    Serial.println(F("Salida 3 Toggle"));
                    if (digitalRead(OUT_3) == HIGH) {
                      digitalWrite(OUT_3, LOW);
                      Serial.println(F("Salida 03 a OFF"));
                    } else {
                      digitalWrite(OUT_3, HIGH);
                      Serial.println(F("Salida 03 a ON"));
                    }
                    break;

                  case '4': // '4'
                    Serial.println(F("Salida 4 Toggle"));
                    if (digitalRead(OUT_4) == HIGH) {
                      digitalWrite(OUT_4, LOW);
                    } else {
                      digitalWrite(OUT_4, HIGH);
                    }
                    break;
                }

                btleSerial.write("ACK0"); // ACK + operacion normal + realizada con exito
                btleSerial.write(data_desc[14]); // Numero de operacion
                btleSerial.write("01"); // Codigo de accion realizada con exito
                btleSerial.write(status_input_stable); // Estado entradas
                btleSerial.write(PIND); // Estado salidas
                btleSerial.write(versionCode); // Version
                Serial.println(F("ENVIADA RESPUESTA A MENSAJE ENTRADA NORMAL"));
              }

              break;

            case '1': // Se ha dejado por las antiguas operaciones TIMBRE Y ALARMA
              break;

            // Every byte of statusInputsXXXX records tells the status of every input starting from input 1 thats is position 0 (12340000)
            // Solamente empleamos los 4 primeros
            case '2': // Operaciones habilitar Entradas. Entre la 21 y la 24
              if (is_encrypt) { //Por seguridad si no estÃ¡ encriptado no puede encender salidas
                Serial.println(F("Operacion habilitar avisos."));
                Serial.println(operation_number - 1);
                // Para pasar el caracter ASCIII a entero
                nume_oper = (operation_number - '0');
                if (nume_oper > 0 && nume_oper < 9) {
                  Serial.println(F("AVISO HABILITADO."));
                  enable_input = enable_input | (B10000000 >> (nume_oper - 1)); // Para habilitar esta entrada
                }

                btleSerial.write("ACK2"); // ACK + operacion normal + realizada con exito
                btleSerial.write(data_desc[14]); // Numero de operacion
                btleSerial.write("01"); // Realizada con exito
                btleSerial.write(status_input_stable); // Estado entradas
                btleSerial.write(PIND); // Estado salidas
                btleSerial.write(versionCode); // Estado salidas
              }
              break;

            case '3': // Operaciones deshabilitar Entradas. Entre la 31 y la 39
              if (is_encrypt) { //Por seguridad si no estÃ¡ encriptado no puede encender salidas
                Serial.println(F("Operacion deshabilitar avisos."));
                Serial.println(operation_number - 1);
                // Para pasar el caracter ASCIII a entero
                nume_oper = (operation_number - '0');
                if (nume_oper > 0 && nume_oper < 9) {
                  Serial.println(F("AVISO DESHABILITADO."));
                  enable_input = enable_input & (B01111111 >> (nume_oper - 1)); // Para deshabilitar esta entrada
                }

                btleSerial.write("ACK3"); // ACK + operacion normal + realizada con exito
                btleSerial.write(data_desc[14]); // Numero de operacion
                btleSerial.write("01"); // Codigo ACK
                btleSerial.write(status_input_stable); // Estado entradas
                btleSerial.write(PIND); // Estado salidas
                btleSerial.write(versionCode); // Version firmware
              }

              break;

            default:
              Serial.println(F("Operacion No conocida"));
              break;
          } // End of switch
        } else {
          Serial.println(F("Safety filters not passed"));
        } // End of pararOper

      } else { // Si el mensaje no está encriptado
        char operation_tipe   = iv1_string.charAt(0);
        char operation_number = iv1_string.charAt(1);

        // Solamente cabe un tipo de operación, la 4
        switch (operation_tipe) {
          case '4':
            switch (operation_number) {
              case '0': // Operacion 40. El mensaje que se recibe es:
                // 40
                // ,
                // X byte (each bit means input enabled or not) 0 --> DISABLE, 1 --> ENABLE).
                // , separador
                // X byte (each bit means normal state of input closed or open)
                //            (NO) 0 --> Will inform if the contact closes, (NC) 1 --> Will inform if the contact opens
                // Respondemos a la Cerradura con un mensaje conteniendo
                // la secuencia de actuaciones mÃƒÂ¡s los status de entradas y salidas
                vecesPruebaVida = vecesPruebaVida + 1; // Para autorizar el envÃƒÂ­o de mensajes a la Cerradura despuÃƒÂ©s
                // de la segunda vez que se recibe este mensaje.
                if (vecesPruebaVida > 2) {
                  vecesPruebaVida = 2;
                }

                btleSerial.write("ACK4"); // ACK + operacion 4
                btleSerial.write("0");    // Numero de operacion
                btleSerial.write("01");   // Codigo de accion realizada con exito
                btleSerial.write(status_input_stable);   // Estado entradas
                btleSerial.write(PIND);                  // Estado salidas
                btleSerial.write(versionCode);                        // Estado salidas
                Serial.println(F("ENVIADA RESPUESTA A MENSAJE ENTRADA NORMAL-1"));

                break;

              case 1: // Solicitud de prueba de vida
                btleSerial.write("ACK4"); // ACK + operacion normal + realizada con exito
                btleSerial.write("1");    // Numero de operacion
                btleSerial.write("01");   // Codigo de accion realizada con exito
                btleSerial.write(status_input_stable); // Estado entradas
                btleSerial.write(PIND);                  // Estado salidas
                btleSerial.write(versionCode);                        // Estado salidas
                Serial.println(F("ENVIADA RESPUESTA A MENSAJE ENTRADA NORMAL-2"));

                break;
            }

          default:
            Serial.println(F("Operacion No conocida"));
            stop_operation = true;
            break;

        } //Fin case tipo operaciones
      }// Fin if encrypted message
    } // Fin if Mensaje no ha sido OK.
  } // End of finished message

  // ********************************************************************************
  // CHECK IF INPUTS CHANGED
  // We'll check every 300 milliseconds. We consider there is a change when two checks in a row are different to previous
  if (millis() - millisCheckInputs > 200) {
    millisCheckInputs = millis();
    status_input = PINC; // Cogemos estado actual entradas

    if (status_input_before ^ status_input != B00000000) { //If there are changes in relation previous situation
      status_input_changed = true;
      status_input_repeat = false;

    } else {
      status_input_repeat = true;

    }

    // Una vez que la entradas ha cambiado, esperamos cuando se repite la misma
    // configuraciÃƒÂ³n de entradas
    // Si se confirma el cambio:
    if (status_input_changed && status_input_repeat) {
      status_input_changed = false;
      vecesRepetido = 0;
      if (status_input ^ pattern_input == B00000000) {
        // Son iguales
        Serial.println(F("El patron y el estado coinciden. No informamos."));
      } else {
        Serial.println(F("El patron y el estado NO coinciden."));
        // Comprobamos si las entradas que han cambiado estÃ¡n habilitadas.
        if ((status_input ^ pattern_input) & enable_input == B00000000) {
          Serial.println(F("Entradas cambiadas no habilitadas."));
        } else {
          if ((status_input ^ pattern_input) & enable_input == B10000000) {
            btleSerial.write("ACK5");              // ACK + cambio input
            btleSerial.write("1");                 // Numero de operacion
            btleSerial.write("01");                // Codigo de accion realizada con exito
            btleSerial.write(status_input_stable); // Estado entradas
            btleSerial.write(PIND);                // Estado salidas
            btleSerial.write(versionCode);         // Estado salidas
          }

          if ((status_input ^ pattern_input) & enable_input == B01000000) {
            btleSerial.write("ACK5");              // ACK + cambio input
            btleSerial.write("2");                 // Numero de operacion
            btleSerial.write("01");                // Codigo de accion realizada con exito
            btleSerial.write(status_input_stable); // Estado entradas
            btleSerial.write(PIND);                // Estado salidas
            btleSerial.write(versionCode);         // Estado salidas
          }

          if ((status_input ^ pattern_input) & enable_input == B00100000) {
            btleSerial.write("ACK5");              // ACK + cambio input
            btleSerial.write("3");                 // Numero de operacion
            btleSerial.write("01");                // Codigo de accion realizada con exito
            btleSerial.write(status_input_stable); // Estado entradas
            btleSerial.write(PIND);                // Estado salidas
            btleSerial.write(versionCode);         // Estado salidas
          }

          if ((status_input ^ pattern_input) & enable_input == B00010000) {
            btleSerial.write("ACK5");              // ACK + cambio input
            btleSerial.write("4");                 // Numero de operacion
            btleSerial.write("01");                // Codigo de accion realizada con exito
            btleSerial.write(status_input_stable); // Estado entradas
            btleSerial.write(PIND);                // Estado salidas
            btleSerial.write(versionCode);         // Estado salidas
          }
        }
      }
    } else if (status_input_changed && !status_input_repeat) {
      //Tiene que estar dos ciclos de 200ms seguidos para considerar que ha cambiado
      vecesRepetido = vecesRepetido + 1;
      if (vecesRepetido >= 2) {
        vecesRepetido = 0;
        status_input_changed = false;
      }
    } else if (!status_input_changed && status_input_repeat) {
      input_changing = false; // Hasta que la situacion no es estable no se puede dormir
      // Para guardar valor estable de las entradas a enviar con acciones normales
      status_input_stable = status_input;
    }

  } // tiempo > 300ms

  // EVOLUCION OPERACIONES EN CURSO***************************
  // Como el reloj se para cuando el micro duerme hay que estar despierto mientras hay alguna salida excitada.
  // TEMPORIZADORES PENDIENTES
  out_working = false; // De cara a SLEEP para saber si hay algun temporizador contando
  if (startTimerOut1 != 0) {
    out_working = true;
    if (((millis() - startTimerOut1) / 1000) > timerOut1) {
      Serial.print("read OUT_1: ");
      Serial.println(digitalRead(OUT_1));
      Serial.print("startTimerOut1: ");
      Serial.println(startTimerOut1);
      if (digitalRead(OUT_1 == 1)) {
        digitalWrite(OUT_1, LOW);
        Serial.println(F("Salida 01 a OFF"));
      } else {
        digitalWrite(OUT_1, HIGH);
        Serial.println(F("Salida 01 a ON"));
      }
      startTimerOut1 = 0;
      Serial.print("startTimerOut1Despues: ");
      Serial.println(startTimerOut1);
    }
  }

  if (startTimerOut2 != 0) {
    out_working = true;
    if (((millis() - startTimerOut2) / 1000) > timerOut2) {
      Serial.print(F("read OUT_2: "));
      Serial.println(digitalRead(OUT_2));
      if (digitalRead(OUT_2) == 1) {
        digitalWrite(OUT_2, LOW);
      } else {
        digitalWrite(OUT_2, HIGH);
      }
      startTimerOut2 = 0;
    }
  }

  if (startTimerOut3 != 0) {
    out_working = true;
    if (((millis() - startTimerOut3) / 1000) > timerOut3) {
      if (digitalRead(OUT_3) == HIGH) {
        digitalWrite(OUT_3, LOW);
      } else {
        digitalWrite(OUT_3, HIGH);
      }
      startTimerOut3 = 0;
    }
  }

  if (startTimerOut4 != 0) {
    out_working = true;
    if (((millis() - startTimerOut4) / 1000) > timerOut4) {
      if (digitalRead(OUT_4) == HIGH) {
        digitalWrite(OUT_4, LOW);
      } else {
        digitalWrite(OUT_4, HIGH);
      }
      startTimerOut4 = 0;
    }
  }
} // Fin loop

void decrypt_data(String str, String iv_strg, char* data)
{
  Serial.print("\r\nInput: ");
  Serial.println(str);
  Serial.println("0");
  delay(10);

  //char mArray[17];
  iv_strg.toCharArray(mArray, 17);
  for (int i = 0; i < 17; i++) {
    iv[i] = mArray[i];
  }
  Serial.println("1");
  delay(10);
  hexToBytes(str, data);
  Serial.println("2");
  delay(10);
  aes128_cbc_dec(key, iv, data, 16); // original 32

  Serial.println("3");
  delay(10);
  data[16] = '\0';  //17th byte is null
  Serial.print("Decrypted:");
  Serial.println(data);

  //  return data;
}

void hexToBytes(String str, char* data) {
  if (str.length() > 1) {
    int len = str.length();
    for (int i = 0; i < len; i += 2) {
      data[i / 2] = getVal(str[i + 1]) + (getVal(str[i]) << 4);
    }
  }
}

byte getVal(char c) {
  if (c >= '0' && c <= '9') return (byte)(c - '0');
  else return (byte)(c - 'a' + 10);
}

/*
   WTITES SEQUENCE IN EEPROM MEMORY.
   Void.
   The million, cents of thousands, tenth of thousands digits are stored in fix position (507, 508, 509),
   the rest change to use all the memory as it has a limited number of operations.
   Every time that tenths of thousands changes we use a different group of five eeprom position, starting from 0 and finishing in 500.
   As we will use 500 position, we have 100 groups that means that every million we will use all position.
   As the unit position changes all the times, every 2000 thousands times will alternate thousands till unit positions.
*/
void actu_secu_eepr() { // data es el char array desencriptado
  //GRABAMOS SECUENCIA EN EEPROM
  Serial.print(F("Grabamos Secuencia EEPROM: "));
  EEPROM.update(507, data_desc[6]  - '0'); // millones. Converting from char to int
  EEPROM.update(508, data_desc[7]  - '0'); // centenas de millar
  EEPROM.update(509, data_desc[8]  - '0'); // decenas de millar
  uint8_t bloq_mens = ((data_desc[7]  - '0') * 10) + (data_desc[8]  - '0');
  EEPROM.update(510, (data_desc[9] - '0') / 2); // (thousands / 2) record the group of 2000 thousand to rotate

  switch ((data_desc[9] - '0') / 2) { // Every 2000 operations we rotate
    // Use data_desc to save memory
    case 0:
      data_desc[0] = 0;
      data_desc[1] = 1;
      data_desc[2] = 2;
      data_desc[3] = 3;
      data_desc[4] = 4;

      break;

    case 1:
      data_desc[0] = 1;
      data_desc[1] = 2;
      data_desc[2] = 3;
      data_desc[3] = 4;
      data_desc[4] = 0;

      break;

    case 2:
      data_desc[0] = 2;
      data_desc[1] = 3;
      data_desc[2] = 4;
      data_desc[3] = 0;
      data_desc[4] = 1;

      break;

    case 3:
      data_desc[0] = 3;
      data_desc[1] = 4;
      data_desc[2] = 0;
      data_desc[3] = 1;
      data_desc[4] = 2;

      break;

    case 4:
      data_desc[0] = 4;
      data_desc[1] = 0;
      data_desc[2] = 1;
      data_desc[3] = 2;
      data_desc[4] = 3;

      break;
  }

  // Group of five position to use
  EEPROM.update(((5 * bloq_mens) + data_desc[0]), (data_desc[9]  - '0'));
  EEPROM.update(((5 * bloq_mens) + data_desc[1]), (data_desc[10]  - '0'));
  EEPROM.update(((5 * bloq_mens) + data_desc[2]), (data_desc[11]  - '0'));
  EEPROM.update(((5 * bloq_mens) + data_desc[3]), (data_desc[12]  - '0'));
}

/*
   RECOVER SEQUENCE THAT IS IN EEPROM MEMORY.
   Return long with the sequence.
   The million, cents of thousands, tenth of thousands digits are stored in fix position (507, 508, 509),
   the rest change to use all the memory as it has a limited number of operations.
   Every time that tenths of thousands changes we use a different group of five eeprom position, starting from 0.
   As we will use 500 position, we have 100 groups that means that every million we will use all position.
   As the unit position changes all the times, every 2000 thousands times will alternate thousands till unit positions

*/
long recu_secu() {
  long secuencia = (1000000 * (EEPROM.read(507))) + (100000 * (EEPROM.read(508))) + (10000 * (EEPROM.read(509)));
  int grup_eepr  = EEPROM.read(510); // Info with the rotation of thousands till units.
  int bloq_eepr  = (EEPROM.read(508) * 10) + EEPROM.read(509); // Info with the 5 positions to use

  switch (grup_eepr) { // We will use first position of data_desc arrays to save records to avoid hanging of micro due to lack of memory
    case 0:
      data_desc[0] = 0; //thousands
      data_desc[1] = 1; //cents
      data_desc[2] = 2; //tenths
      data_desc[3] = 3; //units
      data_desc[4] = 4; //filler (not used)

      break;

    case 1:
      data_desc[0] = 1;
      data_desc[1] = 2;
      data_desc[2] = 3;
      data_desc[3] = 4;
      data_desc[4] = 0;

      break;

    case 2:
      data_desc[0] = 2;
      data_desc[1] = 3;
      data_desc[2] = 4;
      data_desc[3] = 0;
      data_desc[4] = 1;

      break;

    case 3:
      data_desc[0] = 3;
      data_desc[1] = 4;
      data_desc[2] = 0;
      data_desc[3] = 1;
      data_desc[4] = 2;

      break;

    case 4:
      data_desc[0] = 4;
      data_desc[1] = 0;
      data_desc[2] = 1;
      data_desc[3] = 2;
      data_desc[4] = 3;

      break;
  }

  secuencia = secuencia + (1000 * EEPROM.read((5 * bloq_eepr) + data_desc[0])) + (100 * EEPROM.read((5 * bloq_eepr) + data_desc[1])) +
              (10 * EEPROM.read((5 * bloq_eepr) + data_desc[2])) + EEPROM.read((5 * bloq_eepr) + data_desc[3]);

  Serial.print(F("Secuencia EEPROM: "));
  Serial.println(secuencia);

  return secuencia;
}

