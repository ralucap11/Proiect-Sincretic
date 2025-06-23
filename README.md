# Proiect-Sincretic

Acest proiect este un controller de temperatură PID cu profil programabil, implementat pe o placă de dezvoltare
Măsoară Temperatura: Citește continuu temperatura ambientală folosind un senzor LM35 conectat la un pin analog. Pentru a stabiliza citirile, aplică un filtru software simplu.

Controlează un Element de Încălzire: Pe baza temperaturii citite, un algoritm de control PID (Proporțional-Integral-Derivativ) calculează câtă putere să livreze unui element de încălzire (cum ar fi un rezistor, un bec sau un ventilator de aer cald). Puterea este controlată printr-un semnal PWM pe pinul 3.

Urmează un Profil de Temperatură: În loc să mențină o singură temperatură constantă, sistemul urmează un ciclu automat format din 3 faze:

Încălzire (TInc): Timp de 5 minute (300 secunde), temperatura țintă (Setpoint) crește treptat până la valoarea maximă setată.
Menținere (TMen): Timp de 10 minute (600 secunde), menține constantă temperatura maximă.
Răcire (TRac): Timp de 5 minute (300 secunde), temperatura țintă scade treptat înapoi la zero.
Oprit: După terminarea ciclului, sistemul se oprește.
Afișează Informații: Toate datele importante (temperatura curentă, temperatura țintă, timpul rămas în faza curentă și puterea de ieșire) sunt afișate în timp real pe un ecran LCD de 20x4 caractere și sunt trimise și pe calculator prin portul serial.

Permite Reglaje: Poți modifica valoarea maximă a temperaturii țintă (Setpoint) în timp real, folosind două butoane (unul pentru creștere, unul pentru scădere).



Atasa se afla si schema in tinkercad
https://www.tinkercad.com/things/dVYca7i99He-proiectul-meu/editel?returnTo=https%3A%2F%2Fwww.tinkercad.com%2Fdashboard&sharecode=JgYJsWEsDiMvdAUazAG8J-oOk5p6z6i-pIWZHusFYVo
