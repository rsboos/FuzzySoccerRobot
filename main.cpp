// Inclusão da biblioteca que implementa a interface com o SoccerMatch. 
#include "lib/environm.h"

#include <iostream>
#include <algorithm>
#include <map>
#include <vector>
#include "stdio.h"
#include "stdlib.h"
#include "math.h"

#define LEFT 0
#define FRONT 1
#define RIGHT 2

float fuzzyfy(float input, float startPoint, float middlePoint, float endPoint) {
    if (input < startPoint || input > endPoint) // not in fuzzy set
        return 0;
    else if (input <= middlePoint)
        return (input - startPoint) / (middlePoint - startPoint);
    else
        return (endPoint - input) / (endPoint - middlePoint);
}

// Recebe por parâmetro o endereço e a porta do SoccerMatch. 
int main( int argc, char* argv[] ) {

    float   ballAngle, targetAngle, leftMotor, rightMotor;

    // Declaração do objeto que representa o ambiente. 
    environm::soccer::clientEnvironm environment;

    if ( argc != 3 ) {
        printf( "\nSoccerPlayer SERVER_ADDRESS_STRING SERVER_PORT_NUMBER\n" );
        return 0; // Cancela a execução se parâmetros inválidos. 
    }

    // Conecta-se ao SoccerMatch. Supõe que SoccerMatch está rodando na máquina 
    // local e que um dos robôs esteja na porta 1024. Porta e local podem mudar. 
    if ( ! environment.connect( argv[1], atoi( argv[2] ) ) ) {
        printf( "\nFail connecting to the SoccerMatch.\n" );
        return 0; // Cancela a execução se não conseguiu conectar-se. 
    }

    // this vector structure defines the lines for left, front and right fuzzy sets
    /*std::vector<float> left;
    left.push_back(-M_PI);
    left.push_back(-M_PI/2);
    left.push_back(0);
    std::vector<float> front;
    front.push_back(-M_PI/2);
    front.push_back(0);
    front.push_back(M_PI/2);
    std::vector<float> right;
    right.push_back(0);
    right.push_back(M_PI/2);
    right.push_back(M_PI);
    std::vector< std::vector<float> > angleSet;
    angleSet.push_back(left);
    angleSet.push_back(front);
    angleSet.push_back(right);
    */
    std::vector<float> fuzzyValBall;
    fuzzyValBall.resize(3);
    std::vector<float> fuzzyValTarget;
    fuzzyValTarget.resize(3);
    std::vector<float> totalValue;
    totalValue.resize(3);
    float angleSet[3][3] = {{-M_PI,-M_PI/2,0},{-M_PI/2,0,M_PI/2},{0,M_PI/2,M_PI}};



    // Laço de execução de ações. 
    while ( 1 ) {
        for (int i =0; i < totalValue.size(); i++) {
            totalValue[i] = 0;
        }       
        // Deve obter os dados desejados do ambiente. Métodos do clientEnvironm. 
        // Exemplos de métodos que podem ser utilizados. 
        ballAngle = environment.getBallAngle();
        targetAngle = environment.getTargetAngle( environment.getOwnGoal() );
        float ObstAngle = environment.getObstacleAngle();   


        for (int i=0; i < 3; i++) {
            fuzzyValBall[i] = fuzzyfy(ballAngle,angleSet[i][LEFT],angleSet[i][FRONT],angleSet[i][RIGHT]);
            fuzzyValTarget[i] = fuzzyfy(targetAngle,angleSet[i][LEFT],angleSet[i][FRONT],angleSet[i][RIGHT]);
        }

        // Calculate rules values, for left, front and right. This is based on Mamdami's method.

        totalValue[FRONT] = std::max(std::min(fuzzyValBall[LEFT],fuzzyValTarget[LEFT]),totalValue[FRONT]); // Rule 1

        totalValue[LEFT] = std::max(std::min(fuzzyValBall[LEFT],fuzzyValTarget[FRONT]),totalValue[LEFT]); // Rule 2

        totalValue[LEFT] = std::max(std::min(fuzzyValBall[LEFT],fuzzyValTarget[RIGHT]),totalValue[LEFT]); // Rule 3

        totalValue[RIGHT] = std::max(std::min(fuzzyValBall[FRONT],fuzzyValTarget[LEFT]),totalValue[RIGHT]); // Rule 4

        totalValue[FRONT] = std::max(std::min(fuzzyValBall[FRONT],fuzzyValTarget[FRONT]),totalValue[FRONT]); // Rule 5

        totalValue[LEFT] = std::max(std::min(fuzzyValBall[FRONT],fuzzyValTarget[RIGHT]),totalValue[LEFT]); // Rule 6

        totalValue[RIGHT] = std::max(std::min(fuzzyValBall[RIGHT],fuzzyValTarget[LEFT]),totalValue[RIGHT]); // Rule 7

        totalValue[RIGHT] = std::max(std::min(fuzzyValBall[RIGHT],fuzzyValTarget[FRONT]),totalValue[RIGHT]); // Rule 8

        totalValue[FRONT] = std::max(std::min(fuzzyValBall[RIGHT],fuzzyValTarget[RIGHT]),totalValue[FRONT]); // Rule 9


            std::cout << "Total front: " << totalValue[FRONT] << std::endl;
         float sumY = 0;
        float sumXY = 0;

        for(float x = -M_PI; x <= M_PI; x+= 0.1) {
            float y = 0;
            for (int i=0; i < 3; i++) {
                    y = std::max(std::min(fuzzyfy(x,angleSet[i][0],angleSet[i][1],angleSet[i][2]), totalValue[i]), y);
            }
            sumXY +=x*y;
            sumY+=y;
        }
        
        float motor = sumXY / sumY;
        //motor /= 1.5;

        float leftMotor  = cos(motor) - sin(motor);
        float rightMotor = cos(motor) + sin(motor);
        //leftMotor = 0.1;
        //rightMotor = 0.1;

        // Transmite ação do robô ao ambiente. Fica bloqueado até que todos os 
        // robôs joguem. Se erro, retorna false (neste exemplo, sai do laço). 
        if ( ! environment.act( leftMotor, rightMotor ) ) {
            break; // Termina a execução se falha ao agir. 
        }
    }

    return 0;
}
