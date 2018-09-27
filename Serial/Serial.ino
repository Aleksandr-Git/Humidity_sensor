String input_Serial = "";

boolean dataReady = 0;

void setup(){
  Serial.begin(9600);
}

void loop(){
  while (Serial.available()){
    char incomingChar = (char)Serial.read();
    if (incomingChar == 'C'){     
      dataReady = 0;
      input_Serial = "";
     
      continue;
    }
   
    if (incomingChar != '!'){
      input_Serial += incomingChar;
    }
    else{
      dataReady = 1;
    }
  }

  if (dataReady && input_Serial == "Mode_A"){
    Serial.println("Ok");
    dataReady = 0;
    }

//  if (dataReady)
//  {
//    int Vol = input_Serial.toInt();
//    Serial.print("Vol - ");
//    Serial.println(Vol);
//    Serial.println(input_Serial.toInt());    
   
//    dataReady = 0;
//  }
}
