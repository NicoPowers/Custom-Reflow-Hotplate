void setup() {
  Serial.begin(115200);

}

double randNo = 1;      //random integer between 0 or 1 (say, 0 = heads, 1 = tails)

void loop() {
//  randNo = random(1, 200) / 1.0; /                //generate random int between 0 and 1

  //report results: ,,
  for (int i = 0; i < 5; i++) {
    Serial.print(millis()/1000.0);
    Serial.print(',');
    Serial.print((double)i);
    Serial.print('\n');
    delay(1000); 
  }

  for (int i = 10; i > 0; i--) {
    Serial.print(millis()/1000.0);
    Serial.print(',');
    Serial.print((double)5);
    Serial.print('\n');
    delay(1000); 
  }

  for (int i = 5; i > 0; i--) {
    Serial.print(millis()/1000.0);
    Serial.print(',');
    Serial.print((double)i);
    Serial.print('\n');
    delay(1000); 
  }

  
  

}
