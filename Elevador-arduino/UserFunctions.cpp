#include "include/Automaton.h"
#include "include/botoes.h"
#include "include/controle-movimento.h"
#include "include/display7seg.h"
#include "include/globals.h"
#include "include/latch-74HC595.h"
#include "include/leds.h"
#include "include/motor.h"
#include "include/sensores.h"

// Variável global para rastrear o estado de emergência
bool MODO_EMERGENCIA_ATIVO = false; 

// A função 'pararMotor()' deve estar definida em 'include/motor.h' e 'motor.cpp'
// A função 'desligarLed()' deve estar definida em 'include/leds.h' e 'leds.cpp'

void setupPin()
{
  setupMotor();
  setupLatch(); 
  configurarBotoes(); 
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
}

// This function will be called every Arduino loop, put here something that you want to repeat always
void doEveryLoop(){ //--------------- TODO
  lerBotoes();
  atualizarLedsChamadas();
  verificarAndarAtual();
}

// This set of functions should be implemented in a way to handle the controllable events of the system
bool EventControllable_subir(){
    if (decidirMovimento()==0){
        if(hab_motor_mover){
            Serial.print("s-");
            return true;
        }
    }
    return false;
}

bool EventControllable_parar(){
  if ((pararNoAndarAtual() ||decidirMovimento()==2)&& hab_motor_parar){
    Serial.print("p-");
    return true;
  }
  return false;
}

bool EventControllable_descer(){ //--------------- TODO
    if (decidirMovimento()==1){      
        if(hab_motor_mover){
            Serial.print("d-");
            return true;
        }
    }
    return false;
}

bool EventControllable_abrir_porta(){ //--------------- TODO
  // quando chega no andar solicitado eu tenho que abrir a porta 
  if(abrirPorta && pararNoAndarAtual()){
    Serial.print("a-");
    abrirPorta = false; //desativa a flag do evento
    return true;
  }
	return false;
}


// This set of functions should be implemented in a way to handle the uncontrollable events of the system
bool EventUncontrollable_s_1(){
  if(leitordeSensor(S1)){
    if(evento_andares){
      Serial.print("s1-");
      evento_andares = false;
      return true;
    }
  }
  return false;
}

bool EventUncontrollable_s_2(){
  if(leitordeSensor(S2)){
    if(evento_andares){
      Serial.print("s2-");
      evento_andares = false;
      return true;
    }
  }
  return false;
}

bool EventUncontrollable_s_3(){
  if(leitordeSensor(S3)){
    if( evento_andares){
      Serial.print("s3-");
      evento_andares = false;
      return true;
    }
  }
  return false;
}

bool EventUncontrollable_s_4(){
  if(leitordeSensor(S4)){
    if(evento_andares){
      Serial.print("s4-");
      evento_andares = false;
      return true;
    }
  }
  return false;
}

bool EventUncontrollable_fechar_porta(){
  // Se o contador ainda não foi iniciado, não deve fechar a porta
  if (portaAbertaTimerStart == 0) {
    return false;
  }

  // Calcula o tempo decorrido
  unsigned long tempoDecorrido = millis() - portaAbertaTimerStart;
  // Verifica se passou de 10 segundos
  if (tempoDecorrido > 10000) {
    Serial.print("f-");
    return true;
  } 
  else {
    return false;
  }  
}


// Here the expected actions should be implemented in each state of the system 
void StateActionAutomaton0_MotorState0() //parado
{
    //Serial.println("A0S0: Parado");
    pararMotor();
    hab_motor_parar = false;
    hab_motor_mover = true;
    delay(10);
}

void StateActionAutomaton0_MotorState1() //subindo
{
    //Serial.println("A0S1: Subindo");
    subindo = true;
    ligarMotor(0);
    hab_motor_parar=true;
    hab_motor_mover = false;
    delay(10);
}

void StateActionAutomaton0_MotorState2() // descendo 
{
    //Serial.println("A0S2: Descendo");
    subindo = false;
    ligarMotor(1);
    hab_motor_parar=true;
    hab_motor_mover = false;
    delay(10);
}

void StateActionAutomaton1_AndaresState0() //Andar 1
{
    //Serial.println("A1S0: Andar 1");
    ligarDisplay(1);
    andarAtual[1] = true;
    andarAtual[2]= false;
    andarAtual[3]= false;
    andarAtual[4]= false;
    delay(10);
}

void StateActionAutomaton1_AndaresState1() //Andar 2
{
    //Serial.println("A1S1: Andar 2");
    ligarDisplay(2);
    andarAtual[1] = false;
    andarAtual[2]= true;
    andarAtual[3]= false;
    andarAtual[4]= false;
    delay(10);
}

void StateActionAutomaton1_AndaresState2() //Andar 3
{
    //Serial.println("A1S2: Andar 3");
    ligarDisplay(3);  
    andarAtual[1] = false;
    andarAtual[2]= false;
    andarAtual[3]= true;
    andarAtual[4]= false;
    delay(10);
}

void StateActionAutomaton1_AndaresState3() //Andar 4
{
    //Serial.println("A1S3: Andar 4");
    ligarDisplay(4);
    andarAtual[1] = false;
    andarAtual[2]= false;
    andarAtual[3]= false;
    andarAtual[4]= true;
    delay(10);
}

void StateActionAutomaton2_PortaState0() // Porta fechada 
{ 
    //Serial.println("A2S0: Porta Fechada");
    if (portaAbertaTimerStart != 0) {
        // Apagar LED de porta aberta
        desligarLed(LED_PORTA_ABERTA);
        portaAbertaTimerStart = 0;

        // Acender LED de porta fechada
        ligarLed(LED_PORTA_FECHADA);
    }  

    digitalWrite(BUZZER_PIN, LOW);
    abrirPorta =true;
    delay(10);
}

void StateActionAutomaton2_PortaState1() //porta aberta
{
    //Serial.println("A2S1: Aberta");

    // No primeiro loop, Inicia contador se ainda não tiver sido iniciado
    if (portaAbertaTimerStart == 0) {
        portaAbertaTimerStart = millis();
        //Serial.println("Contador de porta aberta iniciado.");

        // Apagar LED de porta fechada
        desligarLed(LED_PORTA_FECHADA);
  
        // Acender LED de porta aberta
        ligarLed(LED_PORTA_ABERTA);
        digitalWrite(BUZZER_PIN, HIGH);
    }
    int andar = verificarAndarAtual();
    desmarcarChamadaAndar(andar);

    delay(10);
}

