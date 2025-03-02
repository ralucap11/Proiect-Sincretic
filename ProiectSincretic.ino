#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

double Setpoint = 30;
double moving_sp = Setpoint;
double Input, Output;
const int outputPin = 3;

double kp = 5.0, ki = 0.1, kd = 1.0;
double eroare = 0, suma_erori = 0, eroare_anterioara = 0, derivativa = 0;
const double dt = 0.05;

volatile unsigned long secunde = 0;
unsigned int ore = 0, minute = 0, sec = 0;
unsigned long previousMillis = 0;

unsigned long uptime = 0; 
int timp_inc = 300;
int timp_men = 600;
int timp_rac = 300;

#define LM35_SENSOR_CHANNEL 0x00
#define ADC_REF_VOLTAGE 5.0

const int butonCrestere = 6;
const int butonScadere = 7;

void init_adc() {
    ADMUX |= (1 << REFS0);
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    ADCSRA |= (1 << ADEN);
}

float read_temperature() {
    ADMUX &= 0xF0;
    ADMUX |= LM35_SENSOR_CHANNEL;
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC)) {}
    uint16_t adc_value = ADC;

    float voltage = (float)adc_value * ADC_REF_VOLTAGE / 1023.0;
    float temperature = (voltage * 100.0)+10; // LM35

    // Aplicare filtrare pentru a reduce fluctuațiile
    static float filtered_temperature = 0.0;
    filtered_temperature = 0.9 * filtered_temperature + 0.1 * temperature;
    return filtered_temperature;
}

void init_timer1() {
    TCCR1A = 0;
    TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);
    OCR1A = 15624;
    TIMSK1 = (1 << OCIE1A);
}

ISR(TIMER1_COMPA_vect) {
    secunde++;
    uptime++;
    sec = secunde % 60;
    minute = (secunde / 60) % 60;
    ore = (secunde / 3600) % 24;
}

void PWM_control(double temperatura_citita) {
    eroare = moving_sp - temperatura_citita;
    suma_erori += eroare * dt;

    if (suma_erori > 100) suma_erori = 100;
    if (suma_erori < -100) suma_erori = -100;

    derivativa = (eroare - eroare_anterioara) / dt;

    Output = (kp * eroare) + (ki * suma_erori) + (kd * derivativa);

    if (Output > 255) Output = 255;
    if (Output < 0) Output = 0;

    analogWrite(outputPin, Output);
    eroare_anterioara = eroare;
}

void afisare_timp() {
    int min = 0, sec = 0, remaining = 0;

    lcd.setCursor(0, 1);
    lcd.print("P: ");
    lcd.print(moving_sp, 0);

    if (uptime <= timp_inc) {
        lcd.print(" TInc:");
        remaining = timp_inc - uptime;
        moving_sp = Setpoint * (timp_inc - remaining) / timp_inc;
    } 
    else if (uptime <= (timp_inc + timp_men)) {
        lcd.print(" TMen:");
        remaining = (timp_inc + timp_men) - uptime;
        moving_sp = Setpoint;
    } 
    else if (uptime <= (timp_inc + timp_men + timp_rac)) {
        lcd.print(" TRac:");
        remaining = (timp_inc + timp_men + timp_rac) - uptime;
        moving_sp = Setpoint - Setpoint * (timp_rac - remaining) / timp_rac;
    } 
    else {
        lcd.print(" Oprit");
        remaining = 0;
        moving_sp = 0;
    }

    min = remaining / 60;
    sec = remaining % 60;

    lcd.print(" ");
    lcd.print(min);
    lcd.print(":");
    if (sec < 10) lcd.print("0");
    lcd.print(sec);
}

void afisare(double temp) {
    lcd.setCursor(0, 0);
    lcd.print("Temp= ");
    lcd.print(temp, 1); // Afișează o zecimală
    lcd.print(" C");

    lcd.setCursor(0, 2);
    lcd.print("Output= ");
    lcd.print(Output);

    Serial.print("Temp: ");
    Serial.print(temp, 1);
    Serial.print(" °C, SP: ");
    Serial.print(moving_sp);
    Serial.print(", Output: ");
    Serial.println(Output);
}

void setup() {
    lcd.init();
    lcd.backlight();
    init_adc();
    init_timer1();
    pinMode(outputPin, OUTPUT);
    pinMode(butonCrestere, INPUT_PULLUP);
    pinMode(butonScadere, INPUT_PULLUP);
    Serial.begin(9600);
    sei();
}

void loop() {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= dt * 1000) {
        previousMillis = currentMillis;

        if (digitalRead(butonCrestere) == LOW) {
            Setpoint += 1;
            delay(200);
        }

        if (digitalRead(butonScadere) == LOW) {
            Setpoint -= 1;
            delay(200);
        }

        double temperatura_citita = read_temperature();

        PWM_control(temperatura_citita);

        afisare(temperatura_citita);
        afisare_timp();

        delay(1000);
    }
}
