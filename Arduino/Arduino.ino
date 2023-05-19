/*
  Controle de Tensao por PWM

  Autor: Thiago Elias dos Santros

  Controla a tensao na saida de um amplificador operacional com ganho fixo a 
  partir de um Sinal PWM filtrado por um circuito RC.

  Controlado por serial, as mensagems sao do tipo A99999F onde os numeros sao
  a tensao em milivolts; por exemplo ao escrever "A12000F" no monitor seria 
  temos uma saida de 12000 mV (12 V).
  A letra A no incio significa que a alteração e na saida "A", caso seja "B"
  significa que a alteração e para a saida "B"

  Criado 08/04/2023.    
  Atualizado 11/05/2023.
  Atualizado 18/05/2023.
  
  Referencias:
  https://forum.arduino.cc/t/16bit-7khz-pwm-output/547229/15

*/
const unsigned int TOP = 0x0FFF;//4095(dec) // PWM de 12 bits a 3906 Hz, se prescaler = 1 

unsigned int tensao_saida[] = {5000, 3300}; //tensao de saida em mV 
enum {INICIO, DEZENA_DE_MILHAR, MILHAR, CENTENA, DEZENA, UNIDADE,FIM} estMQ; // definindo os estados para a maquina de estados
byte DEZENA_DE_MILHAR1, MILHAR1, CENTENA1, DEZENA1, UNIDADE1;
byte SaidaEscolida;

void PWMconfig(){

  TCCR1A = 0;               // Configura com  "Normal port operation, OC1A/OC1B disconnected".
  TCCR1B = 0;               // Configura com  "Normal port operation, OC1A/OC1B disconnected".
  TIMSK1 = 0;  // Timer/Counter1 Interrupt Mask Register
  TIFR1 = 0;   // Timer/Counter1 Interrupt Flag Register
  ICR1 = TOP;
  OCR1A = 0;  // Pwm no pino 9 com duty circle em 0%
  OCR1B = 0;  // Pwm no pino 10 com duty circle em 0%
  TCCR1B |= (1 << CS10); // prescaler = 1
  TCCR1A |= (1 << WGM11);   // Configura o PWM no modo "fast PWM" com TOP = ICR1
  TCCR1B |= (1 << WGM13) | (1 << WGM12) ; // Configura o PWM no modo "fast PWM" com TOP = ICR1
    // Enable Fast PWM on Pin 9: Set OC1A at BOTTOM and clear OC1A on OCR1A compare
  TCCR1A |= (1 << COM1A1);
  pinMode(9, OUTPUT);
  // Enable Fast PWM on Pin 10: Set OC1B at BOTTOM and clear OC1B on OCR1B compare
  TCCR1A |= (1 << COM1B1);
  pinMode(10, OUTPUT);   
  
}

void msg_incoreta(){
  char n;
  switch(estMQ){
    case DEZENA_DE_MILHAR:
      n = "1";
      break;
    case MILHAR:
      n = "2";
      break;   
    case CENTENA:
      n = "3";
      break;
    case DEZENA:
      n = "4";
      break;            
    case UNIDADE:
      n = "5";
      break;
    }                                
    Serial.println(" Byte "+ (String)n +" de tensao nao reconhecido:");
    Serial.println(" A mensagem deve conter o formato A99999F");  
}

void Tensao_requisitada(int a,int  b,int  c,int  d,int  e){
  
  byte i = 255;
  if (SaidaEscolida == 'A') { i=0;}
  else if (SaidaEscolida == 'B') { i=1;}
  
  if (i !=255){  
    tensao_saida[i] =  a*10000 + b*1000 + c*100 + d*10 + e;
  }
}

void processaMQ(char m){
  
  byte valor = m - 48;  
  switch(estMQ){
    case INICIO:
      if(m=='A'){
      estMQ = DEZENA_DE_MILHAR;
      SaidaEscolida = m;
      break;
      }else if(m=='B'){
      estMQ = DEZENA_DE_MILHAR;
      SaidaEscolida = m;
      break;
      }
    
    case DEZENA_DE_MILHAR: // estado para Esperar mensagem iniciar   
    	if (valor > -1 & valor < 7){
        DEZENA_DE_MILHAR1  = valor;
        estMQ = MILHAR;  
      }
      else{
        msg_incoreta();
        estMQ = INICIO;
      }break;
    
      case MILHAR: // estado para Esperar mensagem iniciar   
    	if (valor > -1 & valor < 10){
        MILHAR1  = valor;
        estMQ = CENTENA;  
      }
      else{
        msg_incoreta();
        estMQ = INICIO;
      }break;

      case CENTENA: // estado para Esperar mensagem iniciar   
    	if (valor > -1 & valor < 10){
        CENTENA1  = valor;
        estMQ = DEZENA;  
      }
      else{
        msg_incoreta();
        estMQ = INICIO;
      }break;

      case DEZENA: // estado para Esperar mensagem iniciar   
    	if (valor > -1 & valor < 10){
        DEZENA1  = valor;
        estMQ = UNIDADE;  
      }
      else{
        msg_incoreta();
        estMQ = INICIO;
      }break; 

      case UNIDADE: // estado para Esperar mensagem iniciar   
    	if (valor > -1 & valor < 10){
        UNIDADE1  = valor;
        estMQ = FIM;  
      }
      else{
        msg_incoreta();
        estMQ = INICIO;
      }break;      
    
    case FIM:
    if(m=='F'){ // efetivamente executar o processamento
      Tensao_requisitada(DEZENA_DE_MILHAR1, MILHAR1, CENTENA1, DEZENA1, UNIDADE1);
      mudarTensaodeSaida_A();
      mudarTensaodeSaida_B();       
      Serial.println(" Os dados foram salvos" +(String)tensao_saida[0] +" E "+(String)tensao_saida[1]);
     }
    else{
      Serial.println(" BYTE de filalizaçao incorreto, nada foi alterado.");
      msg_incoreta();
    }
    estMQ=INICIO;
    break;
  }
}

void serialEvent(){
  char ch;
  while(Serial.available()){
    ch=Serial.read(); 
    processaMQ(ch);
  }
}

// E interesantes usar funcoes diferentes para cada saida para que seja possivel usar saidas com "resolucoes" e ruidos menores
void mudarTensaodeSaida_A(){
  int v_out = int(tensao_saida[0]/7.3260); // 7.3260 e ((V_porta)(ganho Amp-op))/ResolucaoPWM ===> (5 x 6)/4095   (em milivolts)
  if  (v_out > TOP) v_out = TOP;
  OCR1A = v_out; // Altera o duty circle do PWM do pino 9
}

void mudarTensaodeSaida_B(){
  int v_out = int(tensao_saida[1]/3.9072); // 3.9072 e ((V_porta)(ganho Amp-op))/ResolucaoPWM ===> (5 x 3.2)/4095   (em milivolts) 
  if  (v_out > TOP) v_out = TOP;
  OCR1B = v_out; // Altera o duty circle do PWM do pino 10
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  PWMconfig();
  mudarTensaodeSaida_A();
  mudarTensaodeSaida_B();
  Serial.begin(9600);
}


void loop() {  

}
