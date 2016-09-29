// Inclusão da biblioteca que implementa a interface com o SoccerMatch. 
#include "lib/environm.h"

#include <iostream>
#include <algorithm>
#include <map>
#include <vector>
#include "stdio.h"
#include "stdlib.h"
#include "math.h"

#define HIGH_LEFT 0
#define MEDIUM_LEFT 1
#define LITTLE_LEFT 2
#define FRONT 3
#define HIGH_RIGHT 6
#define MEDIUM_RIGHT 5
#define LITTLE_RIGHT 4

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
    fuzzyValBall.resize(7);
    std::vector<float> fuzzyValObst;
    fuzzyValObst.resize(7);
    std::vector<float> fuzzyValTarget;
    fuzzyValTarget.resize(7);
    std::vector<float> totalValue;
    totalValue.resize(7);
    //float angleSet[3][3] = {{-M_PI,-M_PI/2,0},{-M_PI/2,0,M_PI/2},{0,M_PI/2,M_PI}};

    float angleSet[7][3] = {{-M_PI,-3*M_PI/4,-M_PI/2},{-3*M_PI/4,-M_PI/2,-M_PI/4},{-M_PI/2,-M_PI/4,0},{-M_PI/4,0,M_PI/4},{0,M_PI/4,M_PI/2},{M_PI/4,M_PI/2,3*M_PI/4},{M_PI/2,3*M_PI/4,M_PI}};
    float smallDistance[3] = {0,5,10};

    // Laço de execução de ações. 
    while ( 1 ) {
        for (int i =0; i < totalValue.size(); i++) {
            totalValue[i] = 0;
        }       
   
        float obstDistance = environment.getCollision();
        ballAngle = environment.getBallAngle();
        targetAngle = environment.getTargetAngle( environment.getOwnGoal() );
        float obstAngle = environment.getObstacleAngle();   


        for (int i=0; i < 7; i++) {
            fuzzyValBall[i] = fuzzyfy(ballAngle,angleSet[i][0],angleSet[i][1],angleSet[i][2]);
            fuzzyValTarget[i] = fuzzyfy(targetAngle,angleSet[i][0],angleSet[i][1],angleSet[i][2]);
            fuzzyValObst[i] = fuzzyfy(obstAngle,angleSet[i][0],angleSet[i][1],angleSet[i][2]);
        }

        float fuzzyDistanceVal = fuzzyfy(obstDistance,smallDistance[0],smallDistance[1],smallDistance[2]);

        // near obstacle and to the left, goes a little to the right
            if (targetAngle != obstAngle) {
                totalValue[LITTLE_RIGHT] = std::max(std::min(fuzzyDistanceVal,fuzzyValObst[LITTLE_LEFT]), totalValue[LITTLE_RIGHT] );
                totalValue[LITTLE_LEFT] = std::max(std::min(fuzzyDistanceVal,fuzzyValObst[LITTLE_RIGHT]), totalValue[LITTLE_LEFT] );
            
            for (int i=0; i < 7; i++) {
                totalValue[i] = std::max(std::min(fuzzyValTarget[i],std::min(fuzzyDistanceVal,fuzzyValObst[FRONT])), totalValue[i] );
            }
            }

        // Calculate rules values, for left, front and right. This is based on Mamdami's method.

        for (int i=0; i < 7; i++) {
            if (i == HIGH_LEFT) {
                for (int j = 0; j < 7; j++)
                    totalValue[i] = std::max(std::min(fuzzyValBall[HIGH_LEFT],fuzzyValTarget[j]),totalValue[i]);
                for (int j = 2; j < 7; j++)
                    totalValue[i] = std::max(std::min(fuzzyValBall[MEDIUM_LEFT],fuzzyValTarget[j]),totalValue[i]);
            }
            else if (i == HIGH_RIGHT) {
                for (int j = 0; j < 7; j++)
                    totalValue[i] = std::max(std::min(fuzzyValBall[HIGH_RIGHT],fuzzyValTarget[j]),totalValue[i]);
                for (int j = 0; j < 5; j++)
                    totalValue[i] = std::max(std::min(fuzzyValBall[MEDIUM_RIGHT],fuzzyValTarget[j]),totalValue[i]);
            }
            else if (i == MEDIUM_LEFT) {
                totalValue[i] = std::max(std::min(fuzzyValBall[MEDIUM_LEFT],fuzzyValTarget[MEDIUM_LEFT]),totalValue[i]);
                for (int j = 3; j < 7; j++)
                    totalValue[i] = std::max(std::min(fuzzyValBall[LITTLE_LEFT],fuzzyValTarget[j]),totalValue[i]);
            }
            else if (i == LITTLE_LEFT) {
                totalValue[i] = std::max(std::min(fuzzyValBall[LITTLE_LEFT],fuzzyValTarget[LITTLE_LEFT]),totalValue[i]);
                totalValue[i] = std::max(std::min(fuzzyValBall[MEDIUM_LEFT],fuzzyValTarget[HIGH_LEFT]),totalValue[i]);
                totalValue[i] = std::max(std::min(fuzzyValBall[FRONT],fuzzyValTarget[MEDIUM_RIGHT]),totalValue[i]);
                totalValue[i] = std::max(std::min(fuzzyValBall[FRONT],fuzzyValTarget[HIGH_RIGHT]),totalValue[i]);
            }
            else if (i == FRONT) {
                totalValue[i] = std::max(std::min(fuzzyValBall[FRONT],fuzzyValTarget[LITTLE_LEFT]),totalValue[i]);
                totalValue[i] = std::max(std::min(fuzzyValBall[FRONT],fuzzyValTarget[FRONT]),totalValue[i]);
                totalValue[i] = std::max(std::min(fuzzyValBall[FRONT],fuzzyValTarget[LITTLE_RIGHT]),totalValue[i]);
            }
            else if (i == LITTLE_RIGHT) {
                totalValue[i] = std::max(std::min(fuzzyValBall[LITTLE_RIGHT],fuzzyValTarget[LITTLE_RIGHT]),totalValue[i]);
                totalValue[i] = std::max(std::min(fuzzyValBall[MEDIUM_RIGHT],fuzzyValTarget[HIGH_RIGHT]),totalValue[i]);
                totalValue[i] = std::max(std::min(fuzzyValBall[FRONT],fuzzyValTarget[MEDIUM_LEFT]),totalValue[i]);
                totalValue[i] = std::max(std::min(fuzzyValBall[FRONT],fuzzyValTarget[HIGH_LEFT]),totalValue[i]);
            }
            else if (i == MEDIUM_RIGHT) {
                totalValue[i] = std::max(std::min(fuzzyValBall[MEDIUM_RIGHT],fuzzyValTarget[MEDIUM_RIGHT]),totalValue[i]);
                for (int j = 0; j < 4; j++)
                    totalValue[i] = std::max(std::min(fuzzyValBall[LITTLE_RIGHT],fuzzyValTarget[j]),totalValue[i]);
            }
        }



       /* totalValue[FRONT] = std::max(std::min(fuzzyValBall[LEFT],fuzzyValTarget[LEFT]),totalValue[FRONT]); // Rule 1

        totalValue[LEFT] = std::max(std::min(fuzzyValBall[LEFT],fuzzyValTarget[FRONT]),totalValue[LEFT]); // Rule 2

        totalValue[LEFT] = std::max(std::min(fuzzyValBall[LEFT],fuzzyValTarget[RIGHT]),totalValue[LEFT]); // Rule 3

        totalValue[RIGHT] = std::max(std::min(fuzzyValBall[FRONT],fuzzyValTarget[LEFT]),totalValue[RIGHT]); // Rule 4

        totalValue[FRONT] = std::max(std::min(fuzzyValBall[FRONT],fuzzyValTarget[FRONT]),totalValue[FRONT]); // Rule 5

        totalValue[LEFT] = std::max(std::min(fuzzyValBall[FRONT],fuzzyValTarget[RIGHT]),totalValue[LEFT]); // Rule 6

        totalValue[RIGHT] = std::max(std::min(fuzzyValBall[RIGHT],fuzzyValTarget[LEFT]),totalValue[RIGHT]); // Rule 7

        totalValue[RIGHT] = std::max(std::min(fuzzyValBall[RIGHT],fuzzyValTarget[FRONT]),totalValue[RIGHT]); // Rule 8

        totalValue[FRONT] = std::max(std::min(fuzzyValBall[RIGHT],fuzzyValTarget[RIGHT]),totalValue[FRONT]); // Rule 9 */


            std::cout << "Total front: " << totalValue[FRONT] << std::endl;
         float sumY = 0;
        float sumXY = 0;

        for(float x = -M_PI; x <= M_PI; x+= 0.1) {
            float y = 0;
            for (int i=0; i < 7; i++) {
                    y = std::max(std::min(fuzzyfy(x,angleSet[i][0],angleSet[i][1],angleSet[i][2]), totalValue[i]), y);
            }
            sumXY +=x*y;
            sumY+=y;
        }
        
        float motor = sumXY / sumY;
        //motor /= 1.5;

        float leftMotor  = (float) (cos(motor) - sin(motor)) / 3.0;
        float rightMotor = (float) (cos(motor) + sin(motor)) / 3.0;
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
