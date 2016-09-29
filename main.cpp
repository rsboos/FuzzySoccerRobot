// Inclus�o da biblioteca que implementa a interface com o SoccerMatch. 
#include "lib/environm.h"

#include "stdio.h"
#include "stdlib.h"
#include "math.h"

// Recebe por par�metro o endere�o e a porta do SoccerMatch. 
int main( int argc, char* argv[] ) {

    float   ballAngle, targetAngle, leftMotor, rightMotor;

    // Declara��o do objeto que representa o ambiente. 
    environm::soccer::clientEnvironm environment;

    if ( argc != 3 ) {
        printf( "\nSoccerPlayer SERVER_ADDRESS_STRING SERVER_PORT_NUMBER\n" );
        return 0; // Cancela a execu��o se par�metros inv�lidos. 
    }

    // Conecta-se ao SoccerMatch. Sup�e que SoccerMatch est� rodando na m�quina 
    // local e que um dos rob�s esteja na porta 1024. Porta e local podem mudar. 
    if ( ! environment.connect( argv[1], atoi( argv[2] ) ) ) {
        printf( "\nFail connecting to the SoccerMatch.\n" );
        return 0; // Cancela a execu��o se n�o conseguiu conectar-se. 
    }

    // La�o de execu��o de a��es. 
    while ( 1 ) {

        // Deve obter os dados desejados do ambiente. M�todos do clientEnvironm. 
        // Exemplos de m�todos que podem ser utilizados. 
        ballAngle = environment.getBallAngle();
        targetAngle = environment.getTargetAngle( environment.getOwnGoal() );


        // A partir dos dados obtidos, deve inferir que a��es executar. Neste 
        // exemplo as for�as destinadas a cada rob� s�o guardadas em leftMotor e 
        // rightMotor. Esta etapa deve ser substituida pelo controlador fuzzy. 
        ballAngle = ballAngle - targetAngle;
        if ( ballAngle < -M_PI ) {
            ballAngle += 2 * M_PI;
        }
        if ( ballAngle > M_PI ) {
            ballAngle -= 2 * M_PI;
        }
        if ( ballAngle < ( -M_PI / 2 ) ) {
            ballAngle = -M_PI / 2;
        }
        if ( ballAngle > ( M_PI / 2 ) ) {
            ballAngle = M_PI / 2;
        }
        leftMotor  = cos( ballAngle ) - sin( ballAngle );
        rightMotor = cos( ballAngle ) + sin( ballAngle );


        // Transmite a��o do rob� ao ambiente. Fica bloqueado at� que todos os 
        // rob�s joguem. Se erro, retorna false (neste exemplo, sai do la�o). 
        if ( ! environment.act( leftMotor, rightMotor ) ) {
            break; // Termina a execu��o se falha ao agir. 
        }
    }

    return 0;
}
