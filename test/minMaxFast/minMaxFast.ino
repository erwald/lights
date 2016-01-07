const int analogPin = 0;
 
void setup() {
    Serial.begin(115200);

    TIMSK0 = 0; // turn off timer0 for lower jitter
    ADCSRA = 0xe5; // set the adc to free running mode
    ADMUX = 0x40; // use adc0
    DIDR0 = 0x01; // turn off the digital input for adc0
}
 
void loop() {
    int mn = 1024; // mn only decreases
    int mx = 0;    // mx only increases
 
    // Perform 10000 reads. Update mn and mx for each one.
    //cli();
    for (int i = 0; i < 10000; ++i) {
        while(!(ADCSRA & 0x10)); // wait for adc to be ready
        ADCSRA = 0xf5; // restart adc
        byte m = ADCL; // fetch adc data
        byte j = ADCH;
        int k = (j << 8) | m; // form into an int, 512/256 | m
          // 512 or 256 | m = 
        k -= 0x0200; // form into a signed int, -= 512
        // k <<= 6; // form into a 16b signed int, k = k * 64
     
        int val = (int)k;
        Serial.println(m);
        mn = min(mn, val);
        mx = max(mx, val);
    }
 
    // Send min, max and delta over Serial
    /*Serial.print("m=");
    Serial.print(mn);
    Serial.print(" M=");
    Serial.print(mx);
    Serial.print(" D=");
    Serial.print(mx-mn);
    Serial.println();*/

    
    //sei();
}
