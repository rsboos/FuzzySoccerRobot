#include "lib/environm.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PI  3.1415

float fuzzificador(float pontoA, float pontoB, float pontoC, float entrada)
{
    //Se a entrada estiver fora do triângulo, retorna zero
    if (entrada < pontoA || entrada > pontoC) return 0.0;

    //senão verifica se está antes do pontoB
    if (entrada <= pontoB)
    {
        return (entrada - pontoA) / (pontoB - pontoA);
    }
    else
    {
        //ou depois do pontoB
        return (pontoC - entrada) / (pontoC - pontoB);
    }
}

//retorna o valor máximo entre 2 valores
float retornaMaximo(float valorA, float valorB)
{
	if (valorA > valorB) {
		return valorA;
	}
	else {
		return valorB;
	}
}

//retorna o valor mínimo entre 2 valores
float retornaMinimo(float valorA, float valorB)
{
	if (valorA < valorB) {
		return valorA;
	}
	else {
		return valorB;
	}
}

int main( int argc, char* argv[] ) {

    // Declaração do objeto que representa o ambiente.
    environm::soccer::clientEnvironm environment;

    if ( argc != 3 ) {
        printf("\nInvalid parameters. Expecting:");
        printf("\nSoccerPlayer SERVER_ADDRESS_STRING SERVER_PORT_NUMBER\n");
        return 0;
    }

    if ( ! environment.connect( argv[1], atoi( argv[2] ) ) ) {
       printf("\nFail connecting to the SoccerMatch.\n");
        return 0;
    }

    //ESQUERDA, FRENTE, DIREITA
    float anguloConjuntoBola[3][3] = {{-PI,-PI/2,0},{-PI/2,0,PI/2},{0,PI/2,PI}};
    float anguloConjuntoAlvo[3][3] = {{-PI,-PI/2,0},{-PI/2,0,PI/2},{0,PI/2,PI}};
    float anguloConjuntoObstaculo[3][3] = {{-PI,-PI/2,0},{-PI/2,0,PI/2},{0,PI/2,PI}};
    float rotacaoConjuntoRobo[3][3] = {{-PI,-PI/2,0},{-PI/2,0,PI/2},{0,PI/2,PI}};

    //PERTO, MÉDIO, LONGE
    float conjuntoDistanciaObstaculo[3][3] = {{0,1,50},{50,500,950},{500,1985,1990}};

    printf( "\nRunning..." );

    while (1)
    {
        //Pega os valores do Servidor(Ambiente)
        float anguloBolaAmbiente = environment.getBallAngle();
        float anguloAlvoAmbiente = environment.getTargetAngle( environment.getOwnGoal() );
        float distanciaObstaculo = environment.getCollision();
        float anguloObstaculo = environment.getObstacleAngle();

        //Desativa todas regras que podem ser ativadas
        bool ativaRegraBolaE = false;
        bool ativaRegraBolaF = false;
        bool ativaRegraBolaD = false;
        bool ativaRegraAlvoE = false;
        bool ativaRegraAlvoF = false;
        bool ativaRegraAlvoD = false;
        bool ativaRegraDistanciaE = false;
        bool ativaRegraDistanciaF = false;
        bool ativaRegraDistanciaD = false;
        bool ativaRegraObstaculoE = false;
        bool ativaRegraObstaculoF = false;
        bool ativaRegraObstaculoD = false;

        //Zera todos valores para que novos sejam calculados
        float valorFuzzificadorBolaE = 0;
        float valorFuzzificadorBolaF = 0;
        float valorFuzzificadorBolaD = 0;
        float valorFuzzificadorAlvoE = 0;
        float valorFuzzificadorAlvoF = 0;
        float valorFuzzificadorAlvoD = 0;
        float valorFuzzificadorDistanciaPequena = 0;
        float valorFuzzificadorDistanciaMedia = 0;
        float valorFuzzificadorDistanciaLonga = 0;
        float valorFuzzificadorObstaculoPequena = 0;
        float valorFuzzificadorObstaculoMedia = 0;
        float valorFuzzificadorObstaculoLonga = 0;

        //Zera os valores de cada Regra de Esquerda, Frente e Direita
        float valorTotalRegrasE = 0;
        float valorTotalRegrasF = 0;
        float valorTotalRegrasD = 0;

        // Verifica ativação de ANGULO BOLA
      
            valorFuzzificadorBolaE = fuzzificador(anguloConjuntoBola[0][0],anguloConjuntoBola[0][1],anguloConjuntoBola[0][2],anguloBolaAmbiente);

            valorFuzzificadorBolaF = fuzzificador(anguloConjuntoBola[1][0],anguloConjuntoBola[1][1],anguloConjuntoBola[1][2],anguloBolaAmbiente);
     
            valorFuzzificadorBolaD = fuzzificador(anguloConjuntoBola[2][0],anguloConjuntoBola[2][1],anguloConjuntoBola[2][2],anguloBolaAmbiente);
        

        // Verifica ativação de ANGULO ALVO
      
            valorFuzzificadorAlvoE = fuzzificador(anguloConjuntoAlvo[0][0],anguloConjuntoAlvo[0][1],anguloConjuntoAlvo[0][2],anguloAlvoAmbiente);
            valorFuzzificadorAlvoF = fuzzificador(anguloConjuntoAlvo[1][0],anguloConjuntoAlvo[1][1],anguloConjuntoAlvo[1][2],anguloAlvoAmbiente);
            valorFuzzificadorAlvoD = fuzzificador(anguloConjuntoAlvo[2][0],anguloConjuntoAlvo[2][1],anguloConjuntoAlvo[2][2],anguloAlvoAmbiente);
       

       

        //Calcula valores das regras, aqui vai o cálculo do Mamdami

            valorTotalRegrasF = retornaMaximo(retornaMinimo(valorFuzzificadorBolaE,valorFuzzificadorAlvoE), valorTotalRegrasF);
            valorTotalRegrasE = retornaMaximo(retornaMinimo(valorFuzzificadorBolaE,valorFuzzificadorAlvoF), valorTotalRegrasE);
            valorTotalRegrasE = retornaMaximo(retornaMinimo(valorFuzzificadorBolaE,valorFuzzificadorAlvoD), valorTotalRegrasE);
            valorTotalRegrasD = retornaMaximo(retornaMinimo(valorFuzzificadorBolaF,valorFuzzificadorAlvoE), valorTotalRegrasD);
            valorTotalRegrasF = retornaMaximo(retornaMinimo(valorFuzzificadorBolaF,valorFuzzificadorAlvoF), valorTotalRegrasF);
            valorTotalRegrasE = retornaMaximo(retornaMinimo(valorFuzzificadorBolaF,valorFuzzificadorAlvoD), valorTotalRegrasE);
            valorTotalRegrasD = retornaMaximo(retornaMinimo(valorFuzzificadorBolaD,valorFuzzificadorAlvoE), valorTotalRegrasD);
            valorTotalRegrasD = retornaMaximo(retornaMinimo(valorFuzzificadorBolaD,valorFuzzificadorAlvoF), valorTotalRegrasD);
            valorTotalRegrasF = retornaMaximo(retornaMinimo(valorFuzzificadorBolaD,valorFuzzificadorAlvoD), valorTotalRegrasF);
      
        std::cout << "Total front: " << valorTotalRegrasF << std::endl;

        //Processo de Defuzificação
        float sumY = 0;
        float sumXY = 0;

        for(float x = -PI; x <= PI; x+= 0.1)
        {
            float y = 0;
            
                y = retornaMaximo(retornaMinimo(fuzzificador(rotacaoConjuntoRobo[0][0],rotacaoConjuntoRobo[0][1],rotacaoConjuntoRobo[0][2], x), valorTotalRegrasE), y);
            


                y = retornaMaximo(retornaMinimo(fuzzificador(rotacaoConjuntoRobo[1][0],rotacaoConjuntoRobo[1][1],rotacaoConjuntoRobo[1][2], x), valorTotalRegrasF), y);
            

          
                y = retornaMaximo(retornaMinimo(fuzzificador(rotacaoConjuntoRobo[2][0],rotacaoConjuntoRobo[2][1],rotacaoConjuntoRobo[2][2], x), valorTotalRegrasD), y);
          
            sumXY += x * y;
            sumY += y;
        }

        float motor = sumXY / sumY;

        float leftMotor  = cos(motor) - sin(motor);
        float rightMotor = cos(motor) + sin(motor);

        // Transmite ação do robô ao ambiente. Fica bloqueado até que todos os
        // robôs joguem. Se erro, retorna false (neste exemplo, sai do laco).
        if ( ! environment.act( leftMotor, rightMotor ) ) {
            break; // Termina a execução se falha ao agir.
        }
    }

    return 0;
}
