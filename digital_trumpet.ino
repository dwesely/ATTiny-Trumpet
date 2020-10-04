#include <avr/sleep.h>

#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397

/*
 * ATTiny85 Trumpet
 * Dan Wesely
 * 10/4/2020
 * 
 * The device plays tones through a speaker to match the frequency of the notes played on 
 * an actual trumpet using the same fingering. 
 * 
 * One button is the "mouthpiece", the embouchure reading determines how high the note will be,
 * the three valve buttons determine which fingering is applied.
 * 
 * The trumpet goes to sleep after 5 minutes to save the battery.
 * 
 * 
 *                            __
 *                  Reset  -1|\/|8-  VCC (+)
 * (Analog Input 3) Pin 3  -2|  |7-  Pin 2 (Analog Input 1, SCK)
 * (Analog Input 2) Pin 4  -3|  |6-  Pin 1 (PWM, MISO)
 *                (-) GND  -4|__|5-  Pin 0 (PWM, AREF, MOSI)
 *
 * 
 * References
 * https://www.arduino.cc/en/Tutorial/toneMelody
 * http://openmusictheory.com/pitches.html
 * https://bobgillis.wordpress.com/2012/08/24/bb-trumpet-fingering-chart-and-overtone-series/
 * 
 * https://www.arduino.cc/reference/en/language/functions/bits-and-bytes/bitread/
 * 
 * https://www.allaboutcircuits.com/technical-articles/switch-bounce-how-to-deal-with-it/
 * 
 */

const int piezo_pin = 0;
const int embouchure_pin = A3;
const int valve_1_pin = A2; // digital pin 4
const int valve_2_pin = A1; // digital pin 2
const int valve_3_pin = 1;
const int valve_threshold = 50;

const unsigned long sleep_threshold = 300000; // power down after 5 minutes
unsigned long last_active = 0; // time to compare for power down

// define results of different fingering and embouchure combinations
// order of fingerings: 111, 101, 011, 110, 100, 010, 000
// binary equivalent:     7,   5,   3,   6,   4,   2,   0
// observed value:       15,  13,  11,  14,  12,  10,   8
// valves_index:          0,   1,   2,   3,   4,   5,   6
const int notes[ 11 ][ 7 ] = {{NOTE_E3,  NOTE_F3,  NOTE_FS3, NOTE_G3,  NOTE_GS3, NOTE_A3,  NOTE_AS3},
                              {NOTE_B3,  NOTE_C4,  NOTE_CS4, NOTE_D4,  NOTE_DS4, NOTE_E4,  NOTE_F4},
                              {NOTE_E4,  NOTE_F4,  NOTE_FS4, NOTE_G4,  NOTE_GS4, NOTE_A4,  NOTE_AS4},
                              {NOTE_GS4, NOTE_A4,  NOTE_AS4, NOTE_B4,  NOTE_C5,  NOTE_CS5, NOTE_D5},
                              {NOTE_B4,  NOTE_C5,  NOTE_CS5, NOTE_D5,  NOTE_DS5, NOTE_E5,  NOTE_F5},
                              {NOTE_D5,  NOTE_DS5, NOTE_E5,  NOTE_F5,  NOTE_FS5, NOTE_G5,  NOTE_GS5},
                              {NOTE_E5,  NOTE_F5,  NOTE_FS5, NOTE_G5,  NOTE_GS5, NOTE_A5,  NOTE_AS5},
                              {NOTE_FS5, NOTE_G5,  NOTE_GS5, NOTE_A5,  NOTE_AS5, NOTE_B5,  NOTE_C6},
                              {NOTE_GS5, NOTE_A5,  NOTE_AS5, NOTE_B5,  NOTE_C6,  NOTE_CS6, NOTE_D6},
                              {NOTE_AS5, NOTE_B5,  NOTE_C6,  NOTE_CS6, NOTE_D6,  NOTE_DS6, NOTE_E6},
                              {NOTE_B5,  NOTE_C6,  NOTE_CS6, NOTE_D6,  NOTE_DS6, NOTE_E6,  NOTE_F6} };

void playTone(int valves, int embouchure_index){
  // given the valve positions and embouchure
  // play a note
  int valves_index;
  if (valves == 0){
    // open
    valves_index = 6;    
  } else if (valves == 2) {
    // two
    valves_index = 5;   
  } else if (valves == 4) {
    // one
    valves_index = 4;   
  } else if (valves == 6) {
    // one and two
    valves_index = 3;   
  } else if (valves == 3) {
    // two and three
    valves_index = 2;   
  } else if (valves == 5) {
    // one and three
    valves_index = 1;   
  } else if (valves == 7) {
    // one, two, and three
    valves_index = 0;   
  } else {
    // invalid fingering
    noTone(piezo_pin);
    return;
  }
  tone(piezo_pin, notes[embouchure_index][valves_index]);
  delay(10);
  
  

// order of fingerings: 111, 101, 011, 110, 100, 010, 000
// binary equivalent:     7,   5,   3,   6,   4,   2,   0
// observed value:       15,  13,  11,  14,  12,  10,   8
// valves_index:          0,   1,   2,   3,   4,   5,   6
           return;
}

void setup()
{
  pinMode(valve_1_pin, INPUT);
  pinMode(valve_2_pin, INPUT);
  pinMode(valve_3_pin, INPUT);
  pinMode(piezo_pin, OUTPUT);
  pinMode(embouchure_pin, INPUT);

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);

}


void enterSleep (void)
{
  sleep_enable();
  sleep_cpu();
}

void loop()
{
  delay(10);
  /*
  read button pin
  encode the analog value to 8bit number
  if tone button active:
    read embouchure
    play tone
  */
  byte encoded_value = 0;
  int observed_value;
  int embouchure;
  int valve_1 = 0;
  int valve_2 = 0;
  int valve_3 = 0;
  
  embouchure = analogRead(embouchure_pin);
  // map to embouchure options
  // "-1" means the mouthpiece button is not pressed, no sounds
  embouchure = map(embouchure, 400, 950, -1, 10);
  
  if (embouchure >= 0){  
    if (analogRead(valve_1_pin) > valve_threshold) { valve_1 = 1;}
    if (analogRead(valve_2_pin) > valve_threshold) { valve_2 = 1;}
    valve_3 = digitalRead(valve_3_pin);
    
    bitWrite(encoded_value, 0, valve_1);
    bitWrite(encoded_value, 1, valve_2);
    bitWrite(encoded_value, 2, valve_3);

    playTone(encoded_value, embouchure);
    last_active = millis();
  }
  else {    
    noTone(piezo_pin);
    if (millis() - last_active > sleep_threshold) {enterSleep();}
  }

}
