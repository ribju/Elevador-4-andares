# 🚧 Projeto Elevador de 4 Andares com Arduino Due

Este projeto implementa um elevador funcional de 4 andares utilizando a plataforma **Arduino Due**. O sistema possui botões para cada andar, LEDs indicadores, um motor para simular o movimento e um display de 7 segmentos que exibe o andar atual.

---

## 🔧 Componentes Utilizados

- Arduino Due
- 10x Botoes + - 10x LEDs indicadores de chamada
- 1x Motor DC + parafuso 40cm
- Driver de motor L298N
- Display de 7 segmentos (com decodificador 74HC595)
- Resistores, jumpers, conectores
- Fonte externa 7.5V

---

## 🧠 Lógica de Funcionamento

- O sistema aguarda chamadas de qualquer andar.
- O elevador se move até o andar solicitado, acendendo os LEDs e atualizando o display de 7 segmentos.
- Se múltiplos andares forem solicitados, as chamadas são atendidas em ordem de prioridade.
- O motor é controlado com base na diferença entre o andar atual e o solicitado.

---
##  Demonstração e testes


https://github.com/user-attachments/assets/17adba0b-809c-4788-9a1b-2a5236bc02e0


---

