# Simulação de Sistema de Detecção de Incêndios com Comunicação entre Sensores

## Objetivo
Implementar um sistema de simulação de detecção de incêndios utilizando programação paralela e distribuída com `pthreads` e monitores para comunicação e sincronização entre as threads. A finalidade é desenvolver funções que utilizem monitores para realizar a comunicação segura entre diferentes threads que simulam nós sensores em uma matriz. É necessário implementar uma thread que gera incêndios aleatórios periodicamente.

## Descrição
Este projeto simula um sistema de monitoramento de incêndios em uma floresta dividida em uma grade de células. Cada célula contém um nó sensor que monitora o estado da sua área. Esses nós comunicam entre si quando detectam incêndios, propagando mensagens aos vizinhos. Se um nó estiver localizado na borda da matriz, ele envia uma mensagem para uma thread central responsável por registrar e gerenciar os eventos de incêndio e iniciar um combate ao fogo. O sistema também inclui uma thread separada para gerar incêndios aleatórios.

## Especificações do Sistema
- **Tamanho da Floresta**: Representar a floresta como uma matriz 30x30 com os seguintes estados:
  - `-`: área livre.
  - `T`: célula monitorada por um nó sensor ativo.
  - `@`: célula em chamas (fogo ativo).
  - `/`: célula queimada
 


