#include <SoftwareSerial.h>
#include<Wire.h>
SoftwareSerial mySerial(12, 13); //Define PIN12 & PIN13 as software UART

const int interrupt0 = 0; // Khai bao ngat
const int Phase_a = 2; //Encoder pha A - day DEN
const int Phase_b = 3; // Encoder pha B - day TRANG
const int autoMode = 4; // Buoc tu dong
const int Syn = 5; // Tu dong - dong bo

int pulse = 0; // Dem xung
double rad = 0; // goc quay Encoder tin hieu

int value; // gia tri doc bien tro
int ManualRadial = 0; // Bien gia tri Manual
int PositionMax =0; // vi tri goc lon nhat tu chan tin hieu
int p; // bien dem 
int delaytime = 0; // thoi gian cho dong co quay 

//I2C variable
int recCommand =0; // Start = 1
int maxRad = 0;

void setup() {
  attachInterrupt(interrupt0, int_, FALLING); // khai bao ngat
  
  pinMode(Phase_a, INPUT_PULLUP); // khai bao chan A encoder tin hieu
  pinMode(Phase_b, INPUT_PULLUP);// khai bao chan B encoder tin hieu
  
  pinMode(autoMode, INPUT_PULLUP); // Khai bao chan cong tac Auto - Manual
  pinMode(Syn, INPUT_PULLUP); // Khai bao chan cong tac dong bo - bat dong bo 

  mySerial.begin(19200);   // Dat gia tri tan so cho giao tiep UART voi Driver

  // khoi tao I2C
 Wire.begin(8);                /* join i2c bus with address 8 */
 Wire.onReceive(receiveEvent); /* register receive event */
 Wire.onRequest(requestEvent); /* register request event */
 Serial.begin(9600);           /* start serial for debug */
}

void loop() 
{
  int bntA_M = digitalRead(autoMode); // Doc tin hieu cong tac Auto - Manual
  int bntS_A = digitalRead(Syn);// Doc tin hieu cong tac dong bo - bat dong bo

  maxRad = PositionMax*0.36;
//  Serial.println(maxRad);
  if(recCommand == 1)
  {
    if(bntA_M == LOW) // Neu chan tin hieu autoMode o muc thap thi goi ham dieu khien manual
    {
      ManControl(); // ham dieu khien manual
    } 
    else  //Neu chan tin hieu khong o muc thap thi chay che do auto
    {
      if(bntS_A == LOW) // Neu chan tin hieu Syn o muc thap thi chay che do Tu dong Dong bo
      {
        SynControl(); //Ham tu dong dong bo
      }
      else  // Neu chan tin hieu Syn khong o muc thap thi chay che do Tu dong Bat dong bo
      {
        AsynControl(); // Ham tu dong bat dong bo
      }
    }
  }
  
}

// I2C function 
void receiveEvent(int howMany) {
  recCommand = Wire.read();
  Serial.println(recCommand);
  //Serial.println();             /* to newline */
}
void requestEvent() {
 Wire.write(maxRad);
}

void AsynControl() // tu dong bat dong bo
{
  p = pulse; // lay gia tri xung dem duoc
  while(p > 56) // chay vong lap neu p > 56 dung de giu cho dong co khong quay truoc khi chan tin hieu ve vi tri cu
  {
    p = pulse; // lay gia tri xung dem duoc
    if(PositionMax < p) PositionMax = p; // tim vi tri lon nhat cua chan tin hieu
    if(p > 336) {
      PositionMax = 336; //gia tri lon goc lon nhat
      
    }
    maxRad = PositionMax*0.36;
    Serial.println(maxRad);
  }
  //Serial.println(maxRad);
  delaytime = PositionMax*13;
  //Serial.println(delaytime);
  Auto_TransPos(PositionMax); // ham gui tin hieu cho dong co chay den vi tri lon nhat
  PositionMax = 0;
  delay(delaytime);
  mySerial.println("{N1 P0 V1000 A2000 }"); // Dieu khien dong co chay ve vi tri 0
  delay(10); // delay 10ms cho gui lenh
}
void SynControl() // tu dong dong bo
{ 
  p = pulse;// lay gia tri xung dem duoc
  //Serial.println(p); 
  Auto_TransPos(p);  // ham gui tin hieu cho dong co chay den vi tri co so xung p
}

void Auto_TransPos( int p)// ham gui tin hieu cho dong co chay
{
  if(p <= 28)
  {
    mySerial.println("{N1 P0 V1000 A2000 }");
  }
  if((28 < p)&&(p <= 56))
  {
    mySerial.println("{N1 P-64 V1000 A2000 }");
  }
  if((56 < p)&&(p <= 84))
  {
    mySerial.println("{N1 P-128 V1000 A2000 }");
  }
  if((84 < p)&&(p <= 112))
  {
    mySerial.println("{N1 P-192 V1000 A2000 }");
  }
  if((112 < p)&&(p <= 140))
  {
    mySerial.println("{N1 P-250 V1000 A2000 }");
  }
  if((140 < p)&&(p <= 168))
  {
    mySerial.println("{N1 P-320 V1000 A2000 }");
  }
  if((168 < p)&&(p <= 196))
  {
    mySerial.println("{N1 P-384 V1000 A2000 }");
  }
  if((196 < p)&&(p <= 224))
  {
    mySerial.println("{N1 P-448 V1000 A2000 }");
  }
  if((224 < p)&&(p <= 252))
  {
    mySerial.println("{N1 P-512 V1000 A2000 }");
  }
  if((252 < p)&&(p <= 280))
  {
    mySerial.println("{N1 P-576 V1000 A2000 }");
  }
  if((280 < p)&&(p <= 308))
  {
    mySerial.println("{N1 P-640 V1000 A2000 }");
  }
  if((308 < p)&&(p <= 336))
  {
    mySerial.println("{N1 P-704 V1000 A2000 }");
  }
  if(336 < p)
  {
    mySerial.println("{N1 P-768 V1000 A2000 }");
  }
  delay(20);
}
void ManControl() // Ham dieu khien bang tay
{
  value = analogRead(A0); // doc tin hieu bien tro
  ManualRadial = map(value,0,1023,0,100); // chia tin hieu bien tro thanh tu 0 - 100
  //Serial.println(ManualRadial); // in gia tri len desktop
  if( ManualRadial > 55) // Neu gia tri ManualRadial lon hon 55 thi cho dong co quay nguoc chieu kim dong ho
  {
    //Serial.println("quay TRAI");
    mySerial.println("{N1 P-1000 V1000 A2000 }");
    delay(10);
  }
  else if(ManualRadial < 30) // Neu gia tri ManualRadial lon hon 30 thi cho dong co quay cung chieu kim dong ho
  {
    //Serial.println("quay PHAI");
    mySerial.println("{N1 P1000 V1000 A2000 }");
    delay(10);
  }
  else //Neu 30 <  ManualRadial < 55 thi dung dong co  
  {
    //Serial.println("STOP");
    mySerial.println("{N1 S }");
    delay(10);
  }
}

void int_() // Ham ngat (Interrupt)
{
  if(digitalRead (Phase_b) == LOW) //Neu chan B encoder co gia tri LOW ( = 0) thi tang bien dem xung pulse them 1
  {
    pulse++;
  }
  else //Neu chan B encoder co gia tri HIGH ( = 1) thi giam bien dem xung pulse di 1
  {
    pulse--;
  }
}
