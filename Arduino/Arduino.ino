/*
  Controle de Tensao por PWM

  Autor: Thiago Elias dos Santros

  Controla a tensao na saida de um amplificador operacional com ganho fixo a 
  partir de um Sinal PWM filtrado por um circuito RC.

  Controlado por serial, as mensagems sao do tipo A99999F onde os numeros sao
  a tensao em milivolts; por exemplo ao escrever "A12000F" no monitor seria 
  temos uma saida de 12000 mV (12 V).

  Criado 08/04/2023.    

*/
unsigned int tensao_saida = 24000; //tensao de saida em mV
enum {INICIO, DEZENA_DE_MILHAR, MILHAR, CENTENA, DEZENA, UNIDADE,FIM} estMQ; // definindo os estados para a maquina de estados
byte DEZENA_DE_MILHAR1, MILHAR1, CENTENA1, DEZENA1, UNIDADE1;

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
  tensao_saida =  a*10000 + b*1000 + c*100 + d*10 + e;
}

void processaMQ(char m){
  byte valor = m - 48;  
  switch(estMQ){
    case INICIO:
      if(m=='A'){
      estMQ = DEZENA_DE_MILHAR;
      }break;
    
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
     Serial.println(" Os dados foram salvos" +(String)tensao_saida);
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

int calcularTensaodeSaida(int v){
   int v_out = int(v/158); // 149 e (V_porta(5v))(ganhoDoAmp-op(7,6))/ResolucaoPWM(255)  em milivolts
   return v_out;
}


void setup() {
  pinMode(5, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
}

// the loop function runs over and over again forever
void loop() {
  int a = calcularTensaodeSaida(tensao_saida);
  analogWrite(5, a);
  //delay(200000); 
  
  
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(1000);                      // wait for a second
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
  delay(1000);
}
