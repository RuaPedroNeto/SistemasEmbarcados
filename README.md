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
De acordo com o objetivo deste projeto, a partir de um host com linux será compilado o código de controle em uma placa VS50 Colibri Viola, a qual envia o sinal de controle para o controlador do amplificador de potência (EPOS), que libera a tensão requerida para o motor MAXON (10 V). Com auxílio de um encoder já embutido no motor, o sinal de velocidade angular é lido pela placa. Outra interface de entrada do usuário deve ser adicionada para estabelecer a velocidade desejada (velocidade de referência), para isso, dois botões seriam utilizados para aumentar ou diminuir a velocidade. Consequentemente, um display de velocidade de referência pode ser implementado para verificação.

Conforme o código de controle, a placa deve comparar os valores de velocidade de referência e velocidade medida pelo encoder, a partir da diferença desses valores, o código implementa o PID sobre o sinal de tensão para o motor de acordo com os ganhos Kp, Ki, e Kd, os quais devem ser configurados por um processo de tuning previamente.

A arquitetura do sistema embarcado é ilustrada pela imagem abaixo.

![Arquitetura do projeto embarcado](https://i.ibb.co/vX0XfNx/imagem-2024-01-03-202929061.png)

## PID
Para o controle PID de um motor DC é necessário as equações da dinâmica do sistema conforme abaixo:

$Js\dot{\theta} + b\dot{\theta} = K_t i$

$Lsi + Ri = V - K_e \dot{\theta}$

Rearranjando os termos, a função transferência do processo é como abaixo:

$T(s) = \frac{K_t}{(Ls+R)(Js+b) + K_t K_e}$

Onde, conforme datasheet:

* (J) Momento de inercia do rotor [kg.m^2] = 5,65.10^-8
* (b) Constante de fricção viscosa do moto [N.m.s] = 2,706.10^-7
* (Ke) Constante de força eletromotriz [V/rad/sec] = 6,92.10^-3
* (Kt) Constante de torque do motor [N.m/Amp] = 6,94.10^-3
* (R) Resistências elétrica [Ohm] = 6,6
* (L) Indutância elétrica [H] = 0,223.10^-3

Nota: 
$b = \frac{Ktorque . Inoload}{\dot{\theta noload}}$

E para o controle PID a entrada de tensão é dada por:

$V = (\dot{\theta{_ref}} - \dot{\theta}) (K_p + \frac{K_I}{s} + K_d s)$

Tuning!!! -> Matlab/simulink

## Implementação e compilação

Para compilar o código na placa VF50, primeiro é necessário fazer a conexão via ethernet com o seguinte comando no terminal do host, utilizando protocolo SSH:

```
ssh root@192.168.1.100
```

Estabelecida a conexão, para realizar a compilação do código PID é necessário a configuração da toolchain por meio do pacote SDK adequado para a placa VF50, pois a arquitetura do processador da placa é diferente da arquitetura do processador do host, portanto é necessário o processo de compilação cruzada. O pacote SDK se encontra no próprio site da Toradex. (https://drive.google.com/file/d/1hs9FL2272A4lUoBjPSdQIjZ0Vn0fZ6ZQ/view)

Para compilar o código e carregá-lo na placa, são necessários o seguintes comandos no terminal:

```
$CC -Wall controle.c -o pid

scp pid root@192.168.1.100:/home/root
``` 

## To do

- ??? Tuning dos ganhos
- ??? Implementação do código PID => Interface com a EPOS
- ??? Checar pinos de input/output no datasheet da placa (código acende led)
- ??? Protocolo CAN
- ??? Explicar Toolchain
