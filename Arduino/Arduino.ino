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

*/
#define Saida_A 5
#define Saida_B 6

unsigned int tensao_saida[] = {0, 3300}; //tensao de saida em mV 
enum {INICIO, DEZENA_DE_MILHAR, MILHAR, CENTENA, DEZENA, UNIDADE,FIM} estMQ; // definindo os estados para a maquina de estados
byte DEZENA_DE_MILHAR1, MILHAR1, CENTENA1, DEZENA1, UNIDADE1;
byte SaidaEscolida;

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
      if(m=='A' || m =='B'){
      estMQ = DEZENA_DE_MILHAR;
      SaidaEscolida = m;
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
      mudarTensaodeSaida_A();
      mudarTensaodeSaida_B();       
      Serial.println(" Os dados foram salvos" +(String)tensao_saida[0] +(String)tensao_saida[1]);
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
  int v_out = int(tensao_saida[0]/129); // 129 e ((V_porta)(ganho Amp-op))/ResolucaoPWM ===> (5 x 6.6)/255   (em milivolts)
  if  (v_out > 255) v_out = 255;
  analogWrite(Saida_A, v_out);
}

void mudarTensaodeSaida_B(){
  int v_out = int(tensao_saida[1]/62); // 62 e ((V_porta)(ganho Amp-op))/ResolucaoPWM ===> (5 x 3.2)/255   (em milivolts) 
  if  (v_out > 255) v_out = 255;
  analogWrite(Saida_B, v_out);
}

void setup() {
  pinMode(Saida_A, OUTPUT);
  pinMode(Saida_B, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  mudarTensaodeSaida_A();
  mudarTensaodeSaida_B();
  Serial.begin(9600);
}


void loop() {  
  
  // Somente para indicar que o programa esta funcionando corretamente.
  digitalWrite(LED_BUILTIN, HIGH);  
  delay(1000);                      
  digitalWrite(LED_BUILTIN, LOW);   
  delay(1000);
}
