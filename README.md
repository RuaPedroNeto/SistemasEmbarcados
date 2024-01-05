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

A arquitetura do sistema embarcado é ilustrada pela imagem abaixo.

![Arquitetura do projeto embarcado](https://i.ibb.co/vX0XfNx/imagem-2024-01-03-202929061.png)

## PID
Para o controle PID de um motor DC é necessário as equações da dinâmica do sistema conforme abaixo:

![Equações constitutivas de um motor DC](https://github.com/RuaPedroNeto/SistemasEmbarcados/blob/main/docs/images/EQ.png)

E portanto, para controle de velocidade, a função transferência final é como abaixo:

![Função Transferência](https://github.com/RuaPedroNeto/SistemasEmbarcados/blob/main/docs/images/TF.png)

Onde:

* (J) Momento de inercia do rotor [kg.m^2]
* (b) Constante de fricção viscosa do moto [N.m.s]
* (Ke) Constante de força eletromotriz [V/rad/sec]
* (Kt) Constante de torque do motor [N.m/Amp]
* (R) Resistências elétrica [Ohm]
* (L) Indutância elétrica [H]

##

