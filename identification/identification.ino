// System Identification of DC Motor
//www.github.com/brilianputraa

//Parts:
//L298N H-Bridge Module
//25GA370 1000 RPM DC Motor With Encoder (Hall Sensor)
//The Encoder raise 60-61 pulse per RPM

//Methods:
// To get the system model, I use finite impulse response model (FIR) 
// By inputting PRBS signal into the DC Motor and record the input (PWM Period) as well the velocity from encoder
// Finally, export the dataset from serial to txt file by copy it then paste into notepad

//Resource:
//https://web.stanford.edu/class/archive/ee/ee392m/ee392m.1034/Lecture8_ID.pdf
//http://tziteras.blogspot.com/2009/11/green-noise-experiment.html 

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     int en =  9; // For PWM signal
int ina = 13; // Logic of Left Side H-Bridge Transistor
int inb = 4; // Logic of Right Side H-Bridge Transistor
int phaseA = 2;
unsigned long prevTime = 0;
unsigned long nowTime = 0;
int updateTime = 100; // Change the interval of speed read
volatile long encoderRead = 0;
int speedDC = 0;
float encoderCount = 61;
unsigned long prbs;
int scale = 1000 / updateTime;
unsigned long simTime = millis() + 100000; // Simulation time

void setup(){
  Serial.begin(57600);
  
  pinMode(phaseA, INPUT_PULLUP);
  pinMode(en, OUTPUT);
  pinMode(ina, OUTPUT);
  pinMode(inb, OUTPUT);
  
  encode(); // Init the encoder pin 

  digitalWrite(ina, HIGH); // Enabling the MOTOR
  digitalWrite(inb, LOW);

  prbs = 0x22aa33aaA;
}

void loop(){
  if (millis() <= simTime){
    
      runPRBS();
      digitalWrite(en, prbs & 1);
      Serial.print((prbs & 1) * 5);
      Serial.print(" ");
      delay(updateTime);
      
    // Read the speed in RPM every time interval
    //  if (nowTime - prevTime > updateTime){
    //    prevTime = nowTime;
       speedDC = (float)(encoderRead * 60 * scale / encoderCount);
    //    Serial.print("RPM :");
       Serial.println(speedDC);
       encoderRead = 0; // Reset the value after update speed
    //  }
  }
  else{
      motorOff();
  }
}

void addEncoder(){
// Increment the encoder pulse every detect rising signal
   encoderRead++;  
}

void encode(){
  attachInterrupt(digitalPinToInterrupt(phaseA), addEncoder, RISING);
}

void runPRBS(){
  unsigned long updatePRBS;
  unsigned char xorbit;
  unsigned char c30, c29, c25, c24;
  // Choose the bit you want to shift can be any bit in range of 0-31 (32 bit)
  c30 = (prbs & (1L << 30)) >> 30; // number 30 bit
  c29 = (prbs & (1L << 29)) >> 29; // number 29 bit
  c25 = (prbs & (1L << 25)) >> 25; // number 25 bit
  c24 = (prbs & (1L << 24)) >> 24; // number 24 bit

  // XOR the bit 
  xorbit = c30 ^ c29 ^ c25 ^ c24;

  // Shift and move the bit
  updatePRBS = (prbs << 1) | xorbit;

  // Replace the register
  prbs = updatePRBS;
}

void motorOff(){
   digitalWrite(ina, LOW);
}

