# Introdução
Este projeto é parte do treinamento do projeto [EmbarcaTech](https://embarcatech.softex.br/), voltado para o ensino de programação no ramo dos Sistemas Embarcados.
O projeto pode ser rodado em uma placa Raspberry Pi Pico W ou em seus derivados, como a placa [BitDogLab](https://github.com/BitDogLab/BitDogLab). 

# Componentes
O projeto em questão utiliza os seguintes periféricos:
- LED RGB -> Exibe a cor relativa ao estado do semáforo (Aberto, Atenção, Fechado);
- Botão -> Pressionado pelo usuário a fim de mudar o estado do semáforo (de fechado para aberto);
- Display OLED -> Exibe ao pedestre mensagens de ordem segundo o estado do semáforo (Siga, Atenção, Aguarde).

# Funcionamento
O semáforo é voltado para o gerenciamento de fluxo de pedestres em um ambiente urbano. 
Inicialmente, o semáforo encontra-se no sinal vermelho e exibe ao pedestre na tela a mensagem de *sinal fechado*.
Este estado se altera para o de Atenção automaticamente após 8 segundos. Durante esse período, caso o pedestre pressione o botão, o estado do semáforo é altamente alterado para o de Atenção, sem que haja a necessidade de aguardar todo o período determinado. A partir desse momento, o sinal amarelo sinaliza que os carros devem se preparar para parar. Na tela, é exibida uma mensagem de *sinal de atenção*.
Por fim, o semáforo passa do estado de Atenção para o estado Aberto, representado pela luz verde no LED. No OLED, a mensagem exibida é a de *sinal aberto*.
