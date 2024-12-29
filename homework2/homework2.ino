int LEDPin = D8;
String number = "" ;
int i = 0 ;
long pwm_val ;

void setup()
{
  //各協定通訊初始化
  Serial.begin(9600);
  pinMode(LEDPin, OUTPUT);
}

void loop()
{
  //執行command副函式
  command();
}

long command() {
  while (Serial.available()) {
    if (i == 0) {
      number = "";
    }
    // 扣除ASCII碼值
    number += Serial.read() - 48; 
    i++;
  }
  // 字串轉換成整數值
  pwm_val = number.toInt(); 
  i = 0 ;
  
  Serial.println(pwm_val);

  //PWM控制LED
  analogWrite(LEDPin, (pwm_val));
  delay(100);
}