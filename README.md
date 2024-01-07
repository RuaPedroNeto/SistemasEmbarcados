# SistemasEmbarcados

**Departamento de Engenharia Aeronáutica da Escola de Engenharia de São Carlos/Universidade de São Paulo**

**SAA0356 - Sistemas Embarcados para Veículos Aéreos**

**Professor Doutor Glauco Augusto de Paula Caurin**

**2023**

## Aluno

Pedro Rua Neto - 10309441

## Introdução
Para o desenvolvimento do projeto desta disciplina, foi proposto o uso de uma VS50 Colibri Viola para o controle de velocidade utilizando PID em um motor DC simulando a funcionalidade de um motor de esteira de academia, portanto também é necessário que o sistema se adeque conforme a necessidade do usuário.

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
 (Ex: Implementação de resistor de 120 Ohms entre CAN-H e CAN-L; Espaçamento de no máximo 30 cm dos hardwares com o barramento)

Para se comunicar com a EPOS e utilizar o protocolo CAN é necessário bibliotecas especifícas. De acordo com o guia "EPOS Command Library", a biblioteca a ser utilizada para linux é a "libEposCmd.so" (https://github.com/yoshito-n-students/eposx_hardware/blob/devel/eposx_library/lib/arm/v7/libEposCmd.so.6.5.1.0) (http://www.maxonmotor.com/medias/sys_master/root/8815100330014/EPOS-Linux-Library-En.zip)

Para utilizar o protocolo CAN, as funções utilizadas são:

````
VCS_Send_CAN_Frame(parameters)

VCS_Read_CAN_Frame(parameters)
````

![Funções de transmição de dados via CAN](https://github.com/RuaPedroNeto/SistemasEmbarcados/blob/main/docs/images/CAN_Function.png)
 
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
Para otimização do projeto realizado é necessário a implementação e validação dos conceitos desenvolvidos em bancada de teste. Verificado o funcionamento do código de controle do motor, o próximo passo seria a implementação de dois botões e um display 7 segmentos.

Os botões são responsáveis pelo aumento/redução da velocidade de referência, enquanto o display deve mostrar ao usuário este valor.
Um display 7 segmentos necessita de 8 pinos, enquanto os dois botões usaria 4 pinos.

Os pinos disponíveis para conexão GPIO são ilustrados pela figura a seguir.

![Pinos para GPIO - Colibri VF50 - Viola](https://github.com/RuaPedroNeto/SistemasEmbarcados/blob/main/docs/images/PINOS_VIOLA.png)

## To do

- ??? Conversão do valor do valor de tensão calculado pelo pid para o valor enviado para a EPOS. Qual a equação/Função que realiza isso? => Checar guia EPOS Command Library
- ??? Criar esboço do código: inicialização da epos, comunicação, e transmissão de dados. (Ler e entender as funções)
