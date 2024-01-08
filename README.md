# SistemasEmbarcados

**Departamento de Engenharia Aeronáutica da Escola de Engenharia de São Carlos/Universidade de São Paulo**

**SAA0356 - Sistemas Embarcados para Veículos Aéreos**

**Professor Doutor Glauco Augusto de Paula Caurin**

**2023**

## Aluno

Pedro Rua Neto - 10309441 (Desenvolvimento de software, simulações, revisão bibliográfica, considerações de hardware, relatório)

## Resumo
O objetivo deste projeto é desenvolver um motor para uma esteira de academia, desta forma, é requerido o controle PID para um motor DC (MAXON 118754) utilizando uma placa VS50 Colibri Viola e uma EPOS2 70/10 via protocolo CAN. Para interação com o usuário e definição da velocidade de refêrencia, é necessário a implementação de dois botões e uma display 7 segmentos. Neste relatório consta o desenvolvimento conceitual do projeto. A primeira parte é a definição da arquitetura do problema, referente à comunicação entre os hardwares. Então, foi definido a malha de controle fechada por meio do simulink e realizado o tuning dos ganhos do controlador. Referente ao protocolo CAN e interface entre a VF50 e a EPOS, foram feitas considerações de hardware e software, declarando quais portas devem ser utilizadas, e quais funções da biblioteca fornecida pela MAXON podem ser utéis. Foi também desenvolvido o código e o esquemático dos botões e do display que faz a interface com o usuário. Por fim, foram realizadas indicações sobre como realizar a compilação cruzada e a implementação do código desenvolvido na placa utilizando uma host LINUX.

## Introdução
Para o desenvolvimento do projeto desta disciplina, foi proposto o uso de uma VS50 Colibri Viola para o controle de velocidade utilizando PID em um motor DC simulando a funcionalidade de um motor de esteira de academia, portanto também é necessário que o sistema se adeque conforme a interação com o usuário, sendo necessário a implementação de dois botões e uma display de 7 segmentos.

## Arquitetura
De acordo com o objetivo deste projeto, a partir de um host com linux será compilado o código de controle em uma placa VS50 Colibri Viola, a qual envia o sinal de controle para o controlador do amplificador de potência (EPOS), que libera a tensão requerida para o motor MAXON (118754). Com auxílio de um encoder já embutido no motor, o sinal de velocidade angular é lido pela placa. Outra interface de entrada do usuário deve ser adicionada para estabelecer a velocidade desejada (velocidade de referência), para isso, dois botões seriam utilizados para aumentar ou diminuir a velocidade. Consequentemente, um display de velocidade de referência pode ser implementado para verificação.

Conforme o código de controle, a placa deve comparar os valores de velocidade de referência e velocidade medida pelo encoder, a partir da diferença desses valores, o código implementa o PID sobre o sinal de tensão para o motor de acordo com os ganhos Kp, Ki, e Kd, os quais devem ser configurados por um processo de tuning previamente.

A arquitetura do sistema embarcado é ilustrada pela imagem abaixo.

![Arquitetura do projeto embarcado](https://i.ibb.co/vX0XfNx/imagem-2024-01-03-202929061.png)

## PID
Para o controle PID de um motor DC é necessário as equações da dinâmica do sistema conforme abaixo:

$Js\dot{\theta} + b\dot{\theta} = K_t i$

$Lsi + Ri = V - K_e \dot{\theta}$

Rearranjando os termos, a função transferência do processo é como abaixo:

$T(s) = \frac{K_t}{(Ls+R)(Js+b) + K_t K_e}$

Onde, conforme datasheet (https://www.maxongroup.com/maxon/view/product/motor/dcmotor/re/re25/118754):

* (J) Momento de inercia do rotor [kg.m^2] = 10.6*10^-7
* (b) Constante de fricção viscosa do moto [N.m.s] = 7.72*10^-7
* (Ke) Constante de força eletromotriz [V/rad/sec] = 35.6*10^-3
* (Kt) Constante de torque do motor [N.m/Amp] = 35.6*10^-3
* (R) Resistências elétrica [Ohm] = 5.29
* (L) Indutância elétrica [H] = 0.551*10^-3

Nota: 
$b = \frac{Ktorque . Inoload}{\dot{\theta noload}}$

Detalhe: Este é um modelo simplificado que não considera a caixa de redução. Um modelo mais completo pode ser desenvolvido como descrito pelo pdf: https://edisciplinas.usp.br/pluginfile.php/4051964/mod_resource/content/2/AulaMatlabSCI2017.pdf

E para o controle PID a entrada de tensão é dada por:

$V = (\dot{\theta{_ref}} - \dot{\theta}) (K_p + \frac{K_I}{s} + K_d s)$

Para encontrar os valores de ganhos, foi utilizado o software Simulink.

Define-se o controle de malha fechada de acordo o diagrama de blocos abaixo

![Diagrama de blocos - PID Malha Fechada](https://github.com/RuaPedroNeto/SistemasEmbarcados/blob/main/docs/images/ClosedLoop.png)

O PID implementado pelo simulink é da seguinte forma:

$PID(s) = (P + \frac{I}{s} + D \frac{N}{1+N \frac{1}{s}})$

Notas: 
* Estabeleceu-se a saturação de tensão como +-42 V, para que o controle não gere soluções que extrapolem a capacidade do motor.

Para definição dos ganhos, os requisitos para esse projeto foram:
* 50-60% da resposta em 200 ms, que é equivalente ao tempo de resposta do usuário;
* Overshoot = 0 %;
* Estabilidade.

E portanto o resultado final é ilustrado na figura a seguir.

![Tuning PID - Simulink](https://github.com/RuaPedroNeto/SistemasEmbarcados/blob/main/docs/images/tuningpid.png)

![Resultado final - Resposta do motor com controle PI a entrada step unitário](https://github.com/RuaPedroNeto/SistemasEmbarcados/blob/main/docs/images/response.jpg)

Portanto, o sistema será superamortecido e os ganhos proporcionais e derivativos foram nulos, logo o controle que atende os requisitos do projeto é apenas integrativo com ganho Ki = 0.15721.

O file simulink se encontra em ".../src/tuninpid.slx"

## Interface EPOS2 70/10
A configuração de hardware deve ser montada conforme instruções contidas no documento EPOS2 70/10 Hardware references. A imagem abaixo ilustra esta configuração.

![Interface com a EPOS](https://github.com/RuaPedroNeto/SistemasEmbarcados/blob/main/docs/images/EPOS_CONFIG.png)

Serão utilizadas as portas de alimentação (Power Supply - 42 V), motor e encoder (J2 e J4 respectivamente), e portas CAN para comunicação com a placa colibri viola VF50 (J7/J8).

Nota: Existe a possibilidade de utilizar o wizard do controlador da EPOS para fazer a configuração e o tuning de uma maneira mais rápida. (Vide video: https://www.youtube.com/watch?v=1A3wtirKTC8).

### Protocolo CAN
O uso dos pinos com protocolo CAN na EPOS é feito conforme especificações do datasheet:

![EPOS - CAN - Especificações](https://github.com/RuaPedroNeto/SistemasEmbarcados/blob/main/docs/images/Can%20data.png)

![EPOS - CAN - Hardware](https://github.com/RuaPedroNeto/SistemasEmbarcados/blob/main/docs/images/CAN_Conn.png)

Enquanto, conforme datasheet da placa Colibri VF50, os pinos para comunicação CAN são o 63 para receiver (RX) e 55 para transmitter (TX).

![VF50 - CAN - Especificações](https://github.com/RuaPedroNeto/SistemasEmbarcados/blob/main/docs/images/Colibri_CAN.png)

Detalhes na implementação do cabeamento devem ser seguidos conforme video: https://www.youtube.com/watch?v=YBrU_eZM110
 (Ex: Implementação de resistor de 120 Ohms entre CAN-H e CAN-L; Espaçamento de no máximo 30 cm dos hardwares com o barramento; Entrelaçar cabos CAN_L e CAN_H; Blindagem eletromagnética)

Para se comunicar com a EPOS e utilizar o protocolo CAN é necessário bibliotecas especifícas. De acordo com o guia "EPOS Command Library", a biblioteca a ser utilizada para linux é a "libEposCmd.so" (https://github.com/yoshito-n-students/eposx_hardware/blob/devel/eposx_library/lib/arm/v7/libEposCmd.so.6.5.1.0) (http://www.maxonmotor.com/medias/sys_master/root/8815100330014/EPOS-Linux-Library-En.zip)

Para utilizar o protocolo CAN, as funções utilizadas são:

````
VCS_Send_CAN_Frame()

VCS_Read_CAN_Frame()
````

![Funções de transmição de dados via CAN](https://github.com/RuaPedroNeto/SistemasEmbarcados/blob/main/docs/images/CAN_Function.png)

Para inicializar a comunicação da EPOS com a placa, a função utilizada é:

````VCS_OpenDevice()````

![Função para iniciar a comunicação entre a EPOS e o novo dispositivo](https://github.com/RuaPedroNeto/SistemasEmbarcados/blob/main/docs/images/fncs/opendevice.png)

*Detalhe*: Conforme o guia, para estabelecer corretamente os parâmetros da VCS_OpenDevice(), use as funções VCS_GetDeviceNameSelection(), VCS_GetProtocolStackNameSelection(), VCS_Get_InterfaceNameSelection(), e VCS_GetPortNameSelection().
Ex: VCS_OpenDevice(char* EPOS2, char* CANopen, char* , char* CAN0)

Ao final do código lembre-se de fechar a porta da EPOS utilizando a função:
VCS_CloseDevice(1);

Para identificar o tipo e os parêmetros do motor, use as funções

````
VCS_SetMotorType()

VCS_SetDcMotorParameterEx()
````
![Função para identificar o tipo do motor](https://github.com/RuaPedroNeto/SistemasEmbarcados/blob/main/docs/images/fncs/motortype.png)

![Função para identificar os parâmetros do motor](https://github.com/RuaPedroNeto/SistemasEmbarcados/blob/main/docs/images/fncs/motorparam.png)

Para identificar o tipo e os parâmetros do encoder, use as funções:

````
VCS_SetSensorType()

VCS_SetIncEncoderParameter()
````

![Função para identificar o tipo do encoder](https://github.com/RuaPedroNeto/SistemasEmbarcados/blob/main/docs/images/fncs/sensortype.png)

![Função para identificar os parâmetros do encoder](https://github.com/RuaPedroNeto/SistemasEmbarcados/blob/main/docs/images/fncs/encoder_param.png)

É possível utilizar funções prontas para controle PID desta biblioteca como VCS_SetControllerGain(), vide EPOS Command Library.pdf, porém será utilizado o código desenvolvido previamente pelo Tiago.

Para configurar entradas e saídas digitais utilize as funções

````
VCS_DigitalInputConfiguration()

VCS_DigitalOutputConfiguration()
````
![Funções para configurar as entradas e saídas digitais](https://github.com/RuaPedroNeto/SistemasEmbarcados/blob/main/docs/images/fncs/diginpconfig.png)

![Funções para configurar as entradas e saídas digitais](https://github.com/RuaPedroNeto/SistemasEmbarcados/blob/main/docs/images/fncs/digoutconfig.png)

Para configurar entradas/saídas analógicas, utilize:

````
VCS_AnalogInputConfiguration()

VCS_AnalogOutputConfiguration()
````

![Funções para configurar as entradas e saídas analógicas](https://github.com/RuaPedroNeto/SistemasEmbarcados/blob/main/docs/images/fncs/analoginpconfig.png)

![Funções para configurar as entradas e saídas analógicas](https://github.com/RuaPedroNeto/SistemasEmbarcados/blob/main/docs/images/fncs/analogoutconfig.png)

Para ler ou escrever nos pinos de GPIO, utilize:


````
VCS_GetAllDigitalInputs()

VCS_GetAllDigitalOutputs()

VCS_SetAllDigitalOutputs()

VCS_GetAnalogInput()

VCS_SetAnalogOutput()

````

(Descrição completa em EPOS Command Library)

Para realizar a leitura da velocidade com o encoder, utilize a função:

````
VCS_GetVelocityls()
````

![Função para ler a velocidade do encoder](https://github.com/RuaPedroNeto/SistemasEmbarcados/blob/main/docs/images/fncs/getveloc.png)

Para realizar o controle de velocidade do motor, a biblioteca fornece duas funções. A primeira estabelece o modo de operação como controle de velocidade, e a segunda estabelece o valor de velocidade a ser seguido:

````
VCS_SetOperationMode(); ou VCS_ActivateVelocityMode();

VCS_SetVelocityMust()
````

Portanto, após leitura do guia EPOS Command Library, não há informações sobre como indicar à EPOS o valor de tensão que o código de PID retorna para ser aplicado ao motor. Desta forma, o uso da EPOS simplifica muito o trabalho, sendo apenas necessário usar as funções de controle e modo de operação dá própria biblioteca. 

Em casos em que o desenvolvedor não possua uma EPOS a disposição, seria necessário entender como passar a informação de tensão aplicada ao motor para a eletrônica de potência do próprio motor. Por exemplo, este é um caso em que o desenvolvedor utiliza um arduino e uma ESCON 50/5 para controlar a posição de um motor EC da MAXON (https://www.youtube.com/watch?v=XPmuIOJvFx0). Vide descrição do vídeo para acesso ao código. Neste caso foi necessário a conversão da tensão que o PID retorna, para um valor entre 255 e 0 que a ESCON 50/5 traduz e controla o motor. A parte do código citada é descrita abaixo.

````
void control() 
{
  desired_step = (int)(desired_angle * step_to_deg); // Conversão angulo - passo
  
  error = desired_step - pulseCount; // Definição do erro para o controle

  double pid_out = error * Kp+ Kd * (error-prev_error); // Equacao de controle
  prev_error = error;
  
  if (pid_out > 0){  // Saturacao para o controle.
    if (pid_out > 100)
      pid_out = 100;
    else if (pid_out < 10)
      pid_out = 10;
  }

  else { // Saturacao para o controle.
    if (pid_out < -100)
      pid_out = -100;
    else if (pid_out > -10)
      pid_out = -10;
  }
  
  motor_start(pid_out); // Chama a funcao motor_start dando como parametro o valor de tensao calculado pela funcao de controle
}

void motor_start(double spd){
  if (spd != 0 && status1 == 1) { // se o parametro recebido pela funcao é diferente de zero, ativa o pino descrito pela variável en => Liga o pino do motor
    digitalWrite(en, HIGH);

    double out = map(spd, -100, 100, 255, 0); // Transforma o valor de tensao dado pelo pid em uma escala de 255 a 0 
    analogWrite(sped, out); // envia o valor de tensao transformado para a ESCON 50/5
  }
  else{ // se o parametro recebido pela funcao é igual a zero, desativa o pino descrito pela variável en => Desliga o pino do motor
    digitalWrite(en, LOW);
  }
````

## Interface com usuário
Para realizar a interface com usuário, será implementado dois botões e um display de 7 segmentos para indicar a velocidade de referência.
O esquemático é ilustrado pela figura abaixo:

![Esquemático de conexões para os botões e o display 7 segmentos](https://github.com/RuaPedroNeto/SistemasEmbarcados/blob/main/docs/images/Imagem1.png)

Enquanto na VF50, os pinos disponíveis para conexão GPIO são ilustrados pela figura a seguir. Mais informações no datasheet: https://docs.toradex.com/102879-colibri-arm-viola-carrier-board-datasheet.pdf

![Pinos para GPIO - Colibri VF50 - Viola](https://github.com/RuaPedroNeto/SistemasEmbarcados/blob/main/docs/images/PINOS_VIOLA.png)

Portanto, como alimentação 5V e GND pode-se utilizar os pinos 12 e 2 respectivamente. E seguindo datasheet da Viola, os pinos 8 ao 16 podem ser usados como I/O. Logo pode-se usar a seguinte configuração:

Wire   Pin  SODIMM Number

a      16   45

b      15   79

c      14   85

d      13   97

e      12   101

f      11   103

g      10   133

inc     9   98

red     8   135


Desta forma um possível código que realiza a função de incremento e decremento é descrito abaixo:
Nota: Download da biblioteca GPIO para acessar os pinos da Viola (https://developer.toradex.com/windows-ce/knowledge-base/gpio-lib-api#details)
(https://developer.toradex.com/windows-ce/knowledge-base/how-to-use-gpio-library/)

````
#include <stdlib.h>
#include <stdio.h>
#include "GpioLib.h"

int a = 45;//SODIMM Correspondente ao LED a pino 16
int b = 79;//SODIMM Correspondente ao LED b pino 15
int c = 85;//SODIMM Correspondente ao LED c pino 14
int d = 97;//SODIMM Correspondente ao LED d pino 13
int e = 101;//SODIMM Correspondente ao LED e pino 12
int f = 103;//SODIMM Correspondente ao LED f pino 11
int g = 133;//SODIMM Correspondente ao LED g pino 10
int b1Pin = 98;//SODIMM Correspondente ao botão +
int b2Pin = 135;//SODIMM Correspondente ao botão -


void setup()
{

InitGPIOLib();		// Inicializa a GPIO

  SetPinAltFn(a, -1, DIR_OUT);//Define a como saída
  SetPinAltFn(b, -1, DIR_OUT);//Define b como saída
  SetPinAltFn(c, -1, DIR_OUT);//Define c como saída
  SetPinAltFn(d, -1, DIR_OUT);//Define d como saída
  SetPinAltFn(e, -1, DIR_OUT);//Define e como saída
  SetPinAltFn(f, -1, DIR_OUT);//Define f como saída
  SetPinAltFn(g, -1, DIR_OUT);//Define g como saída

SetPinAltFn(b1Pin, -1, DIR_IN) //Define b1Pin como entrada
SetPinAltFn(b2Pin, -1, DIR_IN) //Define b2Pin como entrada

}

//Função para escrever o nº zero
void zero() {
  SetPinLevel(a, 1);
  SetPinLevel(b, 1);
  SetPinLevel(c, 1);
  SetPinLevel(d, 1);
  SetPinLevel(e, 1);
  SetPinLevel(f, 1);
  SetPinLevel(g, 0);
  delay(100);
}
//Função para escrever o nº um
void um() {
  SetPinLevel(a, 0);
  SetPinLevel(b, 1);
  SetPinLevel(c, 1);
  SetPinLevel(d, 0);
  SetPinLevel(e, 0);
  SetPinLevel(f, 0);
  SetPinLevel(g, 0);
  delay(100);
}
//Função para escrever o nº dois
void dois() {
  SetPinLevel(a, 1);
  SetPinLevel(b, 1);
  SetPinLevel(c, 0);
  SetPinLevel(d, 1);
  SetPinLevel(e, 1);
  SetPinLevel(f, 0);
  SetPinLevel(g, 1);
  delay(100);
}
//Função para escrever o nº três
void tres() {
  SetPinLevel(a, 1);
  SetPinLevel(b, 1);
  SetPinLevel(c, 1);
  SetPinLevel(d, 1);
  SetPinLevel(e, 0);
  SetPinLevel(f, 0);
  SetPinLevel(g, 1);
  delay(100);
}
//Função para escrever o nº quatro
void quatro() {
  SetPinLevel(a, 0);
  SetPinLevel(b, 1);
  SetPinLevel(c, 1);
  SetPinLevel(d, 0);
  SetPinLevel(e, 0);
  SetPinLevel(f, 1);
  SetPinLevel(g, 1);
  delay(100);
}
//Função para escrever o nº cinco
void cinco() {
  SetPinLevel(a, 1);
  SetPinLevel(b, 0);
  SetPinLevel(c, 1);
  SetPinLevel(d, 1);
  SetPinLevel(e, 0);
  SetPinLevel(f, 1);
  SetPinLevel(g, 1);
  delay(100);
}
//Função para escrever o nº seis
void seis() {
  SetPinLevel(a, 1);
  SetPinLevel(b, 0);
  SetPinLevel(c, 1);
  SetPinLevel(d, 1);
  SetPinLevel(e, 1);
  SetPinLevel(f, 1);
  SetPinLevel(g, 1);
  delay(100);
}
//Função para escrever o nº sete
void sete() {
  SetPinLevel(a, 1);
  SetPinLevel(b, 1);
  SetPinLevel(c, 1);
  SetPinLevel(d, 0);
  SetPinLevel(e, 0);
  SetPinLevel(f, 0);
  SetPinLevel(g, 0);
  delay(100);
}
//Função para escrever o nº oito
void oito() {
  SetPinLevel(a, 1);
  SetPinLevel(b, 1);
  SetPinLevel(c, 1);
  SetPinLevel(d, 1);
  SetPinLevel(e, 1);
  SetPinLevel(f, 1);
  SetPinLevel(g, 1);
  delay(100);
}
//Função para escrever o nº nove
void nove() {
  SetPinLevel(a, 1);
  SetPinLevel(b, 1);
  SetPinLevel(c, 1);
  SetPinLevel(d, 1);
  SetPinLevel(e, 0);
  SetPinLevel(f, 1);
  SetPinLevel(g, 1);
  delay(100);
}


void main(){

unsigned char ucStatus_inc; // Variavel de travamento do incremento.
unsigned char ucStatus_dec; // Variavel de travamento do decremento.
unsigned int  uiContador;   // Variavel de armazenamento do contador.
unsigned int  uiValor;      // Variavel auxiliar para exibição do contador.

ucStatus_inc=0;      // Inicializa a variavel com o valor 0.
ucStatus_dec=0;      // Inicializa a variavel com o valor 0.
uiContador=0;        // Inicializa a variavel com o valor 0.

	while(1){            // loop infinito

// Le Tecla E Incrementa Contador.

		if((GetPinLevel(b1Pin)==0)&&(ucStatus_inc==0)){   // Incrementa somente uma vez quando a tecla for pressionada.
			ucStatus_inc=1;                       // Variavel de travamento do incremento.
			uiContador++;                         
				if(uiContador>9){
					uiContador=9;
				}
		}

		if((GetPinLevel(b1Pin)==1)&&(ucStatus_inc==1)){   // Volta a disponibilizar a opção de incremento quando a tecla for solta.
		ucStatus_inc=0;
		}

// Le tecla e decrementa contador.
		if((GetPinLevel(b2Pin)==0)&&(ucStatus_dec==0)){   // Decrementa somente uma vez quando a tecla for pressionada.
			ucStatus_dec=1;                       // Variavel de travamento do decremento.
			uiContador--;                         // Esse operador aritmetico(--) realiza o mesmo que variavel = variavel - 1.
				if(uiContador<0){
					uiContador=0;
				}
		}


		if((GetPinLevel(b2Pin)==1)&&(ucStatus_dec==1)){   // Volta a disponibilizar a opção de incremento quando a tecla for solta.
			ucStatus_dec=0;
		}

  switch (uicontador) {
    case 0:
      zero();//Executa a função zero
      break;
    case 1:
      um();//Executa a função um
      break;
    case 2:
      dois();//Executa a função dois
      break;
    case 3:
      tres();//Executa a função três
      break;
    case 4:
      quatro();//Executa a função quatro
      break;
    case 5:
      cinco();//Executa a função cinco
      break;
    case 6:
      seis();//Executa a função seis
      break;
    case 7:
      sete();//Executa a função sete
      break;
    case 8:
      oito();//Executa a função oito
      break;
    case 9:
      nove();//Executa a função nove
      break;

  }

}
}
````
## Implementação e compilação

Para realizar a compilação do código PID é necessário a configuração da toolchain por meio do pacote SDK adequado para a placa VF50, pois a arquitetura do processador da placa é diferente da arquitetura do processador do host, portanto é necessário o processo de compilação cruzada. O pacote SDK se encontra no próprio site da Toradex. (https://drive.google.com/file/d/1hs9FL2272A4lUoBjPSdQIjZ0Vn0fZ6ZQ/view)

Então, é necessário fazer a conexão via ethernet com o seguinte comando no terminal do host, utilizando protocolo SSH:

```
ssh root@192.168.1.100
```

Para compilar o código e carregá-lo na placa, são necessários o seguintes comandos no terminal:

```
$CC -Wall main.c -o control
$CC -Wall pid.c -o pid

scp control root@192.168.1.100:/home/root
scp pid root@192.168.1.100:/home/root
scp pid.h root@192.168.1.100:/home/root
scp pid_constants.h root@192.168.1.100:/home/root
```

No terminal da placa, o comando para rodar o código é o seguinte:

```
./control
```

## Considerações Finais
Devido à indisponibilidades de horários, pois eu já estava trabalhando enquanto frequentava este curso, não foi possível testar e validar os conceitos e códigos desenvolvidos deste projeto. Para finalizar o controle do motor DC, seria apenas necessário entender como enviar a informação de tensão para a EPOS, afim de que ela consiga aplicá-la ao motor DC, sem o uso das funções prontas da biblioteca fornecida pela MAXON.

## To do

- ??? Conversão do valor do valor de tensão calculado pelo pid para o valor enviado para a EPOS. Qual a equação/Função que realiza isso? => Checar guia EPOS Command Library
- ??? Criar esboço do código: inicialização da epos, comunicação, e transmissão de dados. (Ler e entender as funções)
