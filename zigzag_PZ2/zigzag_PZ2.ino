//    一応なやつ。
//    電池残量が満タンの時はこれをベースに。

int PIN_VrefR = 3;  int PIN_VrefL = 6;
int PIN_IN1R  = 4;  int PIN_IN1L  = 7;
int PIN_IN2R  = 5;  int PIN_IN2L  = 8;


void rotate_R(int direction_R1, int direction_R2){    //  右モータの回転方向を設定する
  digitalWrite(PIN_IN1R, direction_R1);
  digitalWrite(PIN_IN2R, direction_R2);
}

void rotate_L(int direction_L1, int direction_L2){    //  左モータの回転方向を設定する
  digitalWrite(PIN_IN1L, direction_L1);
  digitalWrite(PIN_IN2L, direction_L2);              
}

void move(int speed_R, int speed_L, int time){        //  右・左両方のモータをspeedの割合でtimeミリ秒回転させる
  analogWrite(PIN_VrefR, speed_R);
  analogWrite(PIN_VrefL, speed_L);
  delay(time);
}


void setup() {
  analogReference(DEFAULT);
  
  for (int i = 4; i <= 13; i++) {
    if(i != 6) { pinMode(i, OUTPUT); }                                      //  6ピンを除く4～13ピンを出力用に設定
  }

  rotate_R(0, 1);      // 正転  (初期設定)右モータを前進に設定
  rotate_L(0, 1);      // 正転  (初期設定)左モータを前進に設定
}

void loop() {
  int val[5];
  int val_MAX = 0;                              //  ピンの中で最も電圧値が高い数値が入る。
  int val_MAX_NUMBER = 0;                       //  最も電圧値の高いピンの番号が入る。(i = 0  == A0 的な。)
  int D = 100;

  for (int i = 0; i <= 4; i++) {                //  最も電圧値が高いアナログピンを調べる。
    val[i] = analogRead(A0 + i);                //  valにはA0～A4までのピンの電圧値が換算された0-1023までの数値が格納される。
    
    if(val_MAX < val[i]){                       //  最も電圧値が高いフォトリフレクタに対応するアナログピンを調べる。
      val_MAX = val[i];                         //  val_MAXよりも大きい電圧値の時、val_MAXの値を更新。
      val_MAX_NUMBER = i;                       //  最も電圧値が高いアナログピンの番号を格納（A0 = 0, A1 = 1, ...）      
    }
    digitalWrite(i + 9, LOW);                 //  黒線上の最も中心に位置するフォトリフレクタに対応するLED以外のLEDを消灯させる。 
  }
  digitalWrite(val_MAX_NUMBER + 9, HIGH);     //  黒線上の最も中心に位置するフォトリフレクタに対応するLEDを光らせる。 

  switch(val_MAX_NUMBER){
    //  右にコースアウトした時、その場で左右のモータを逆回転させ、コースを修正する。（重度の場合）
    case 0 : if(val[1] > 714){ move(180, 125, 2); }               //  軽度のコースアウトである場合。
             else {                                               //  重度のコースアウトである場合。
               rotate_R(0, 1);      // 正転
               rotate_L(1, 0);      // 反転
               move(183, 183, 2);
               rotate_R(0, 1);      // 正転
               rotate_L(0, 1);      // 正転
              }
               break;

    //  コースの右側を走行している時
    case 1 : if(val[0] > val[2])        { move(180, 160, 3); }
             else if(val[0] == val[2] ) { move(185, 165, 3); }
             else if(val[0] < val[2] )  { move(185, 170, 3); }             
             break;         

    //  コースの中央を走行している時                      
    case 2 : if(val[1] > val[3])        { move(200, 197, 1); }
             else if(val[1] == val[3] ) { move(200, 200, 1); }
             else if(val[1] < val[3] )  { move(197, 200, 1); }
             move(200, 200, 2);
             break;

    //  コースの左側を走行している時     
    case 3 : if(val[3] < val[4])        { move(170, 185, 3); }
             else if(val[3] == val[4] ) { move(165, 185, 3); }
             else if(val[3] > val[4] )  { move(160, 180, 3); }
             break;

    //  左にコースアウトした時、その場で左右のモータを逆回転させ、コースを修正する。
    case 4 : if(val[3] > 714){ move(145, 120, 2); }               //  軽度のコースアウトである場合。
             else {                                               //  重度のコースアウトである場合。
               rotate_R(1, 0);      // 反転
               rotate_L(0, 1);      // 正転
               move(183, 183, 2);
               rotate_R(0, 1);      // 正転
               rotate_L(0, 1);      // 正転
             }  
             break;                     
  }
  
}


// すぐに端っこに行ってしまうのが問題点




//  case0 及び case4で左右のモータを逆回転したのち、両方のモータを正転に直しているので、命令速度を早く出来た。
//  case1 - 3 では、再度、モータの回転方向を設定し直さなくてもよい（？）
//  一周約40秒以内の比率(最速34秒)
//  5vに換算し直さないことにより、計算量が減り、プログラムが回るスピードが速くなる。
//  さらに、最大の電圧値を精密に計算でき、中央にあるフォトリフレクタの正確な位置を求めることができる。
//    iをグローバル変数にして記憶させておくのも手かと思う。つまり、前の状態を記憶させておく。どのピンが中央であったかを。
