int val = 0;
int imin = 1024;
int imax = 0;
int ipre = 0;
int icount = 0; 

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(0, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  val = analogRead(0);
  if(imin > val)
    imin = val;
  if(imax < val)
    imax = val;
  if(25 < abs(ipre - val))
  {
     ipre = val;
     icount++;
     Serial.print(icount);
     //Serial.print(" Min : "); Serial.print(imin); 
     //Serial.print(" Max : "); Serial.print(imax); 
     Serial.print(" val : "); Serial.println(val);
  }
  
    
  delayMicroseconds(1);
}
