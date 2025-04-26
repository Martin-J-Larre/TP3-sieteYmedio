//
//  main.cpp
//  TP3-sieteYmedio
//
//  Created by Martin  on 4/15/25.
//

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <errno.h>

using namespace std;


int mazo[40];

// Metodo para cargar el array con las 40 cartas
// 4 veces del 1 al 10 simulando los 4 palos

void cargarMazo() {
    for (int p = 0; p < 4; p++){
        for (int n = 0; n < 10; n++){
            mazo[p*10+n] = n+1;
        }
    }
}

// Metodo para sacar una carta con el valor 1, 2, 3, 4, 5, 6, 7, 10, 11, 12.
// Se toma un numero random del 0 al 39 donde ese número es usado como indice
// para obtener el valor alojado en el si este numero < 7 se devuelve el valor
// original y si es mayor a 7 se le suman 2 para evitar retornar los numeros 8,
// 9 y 10 retornando en su lugar 10, 11 y 12. Se pone un 0 en el indice del array
// al numero retornado para evitarlo por si vuelve a salir ese indice.

int sacarCarta(){
    srand(time(0));
    int x, d;
    x = rand()%40;
    while (mazo[x] == 0) x = (rand()%40);
    d = (mazo[x] > 7) ? mazo[x] + 2 : mazo[x];
    mazo[x] = 0;
    
    return d;
}



int main(int argc, const char * argv[]) {
    int num;
    cout << "************ SIETE Y MEDIO ************" <<endl;
    cout << "=======================================" <<endl;
    cout << "\n";
    cout << "Ingresa la cantidad de jugadores: ";
    cin >> num;
    cout << "\n";
    
    // Array 3D de pipes que nos permite contar con un
    // array 2D de pipe bidireccional por jugador con la
    // ayuda del pid de cada jugador (proceso) en el indice
    // nos permitira identifica su pipe en cada iteracion
    // En la primera dimencion el pdi y las otras dos
    // dimenciones los fd1 y fd2
    
    int pipes[num][2][2];

    // Array de pid retornado en cada iteracion del fork()
    // nos va a permitir usar de index para identificar los pipes
    // de cada jugador para poder leer y escribir cada pipe en el
    // array 3D
    
    int* ids = new int[num];
    
    // El array de estados le va a permitir saber como esta el estado
    // de cada proceso al padre, si sigue jugando o no.
    int* estados = new int[num];
    
    // Se crea un pipe para cada jugador (Proceso)
    for(int i = 0; i < num; i++){
        if (pipe(pipes[i][0]) == -1){
            cout <<"Error al crear el pipe: " << i <<endl;
        }
        if (pipe(pipes[i][1]) == -1){
            cout <<"Error al crear el pipe: "<< i <<endl;
        }
    }
    
    // Se crean los jugadores (procesos) y se guarda los pid en el array de ids.
    for(int i = 0; i < num; i++){
        ids[i] = fork();
        if(ids[i] == -1){
            cout <<"Error creando el Jugador (Proceso Hijo)" <<endl;
        }
        // Corre el proceso hijo, en cada iteracion de jugadores
        if(ids[i] == 0){
            float puntos= 0.0;
            int carta;
            int estado = 0; // estado = 0 (pide otra carta) || estado = 1 (Arriesga) || estado = 2 (Abandona)
                
                // Lee la carta del padre, verifica los puntos y actualiza el estado
                do{
                    if(read(pipes[i][0][0], &carta, sizeof(int)) == -1){
                        cout <<"Error al leer el pipe del jugador(proceso)"<< i <<endl;
                    }
                    if(carta > 7){
                        puntos += 0.5;
                    }else{
                        puntos += carta;
                    }
                    // Mas de 7.5 puntos abandona
                    if(puntos > 7.5){
                        estado = 2;
                    }
                    // Entre 5 y 7.5 arriega la jugada o gana.
                    if(puntos > 5 && puntos <= 7.5){
                        estado = 1;
                    }
                    // Datos del jugador
                    cout << "Jugador "<< i+1 <<" Lee carta: "<< carta <<" Puntos: "<< puntos <<endl;
                    // Envia el estado de cada jugador (proceso)al padre
                    if(write(pipes[i][1][1], &estado, sizeof(int)) == -1){
                        cout <<"Error al escribir e89.+-*/=====l pipe del jugador(proceso)"<< i <<endl;
                    }
                }while(estado == 0);
                // Lee y envia los puntos finales
                if(read(pipes[i][0][0], &carta, sizeof(int)) == -1){
                        cout <<"Error al leer el pipe del jugador(proceso)"<< i <<endl;
                    }
                if(write(pipes[i][1][1], &puntos, sizeof(float)) == -1){
                        cout <<"Error al escribir el pipe del jugador(proceso)" << i <<endl;
                    }
                    return 0;
                }
            }
    // Empieza el padre
    cargarMazo();
    int repartirCartas;
    
    // Ronda para repartir carta a cada jugador(proceso)
    for (int j=0; j<num; j++) {
        repartirCartas = sacarCarta();
        if(write(pipes[j][0][1], &repartirCartas, sizeof(int)) == -1){
            cout <<"Error Proceso Padre al escribir el Pipe" <<endl;
        }
    }
    
    bool continuar = true;
        // Si hay jugadores en estado de continuar el juego, se les
        // reparten más cartas
        do{
            continuar = false;
            for(int i = 0; i < num; i++){
                if(estados[i] == 0){
                    // Lee los estados
                    if(read(pipes[i][1][0], &estados[i], sizeof(int)) == -1){
                        cout <<"Error Proceso Padre al leer el Pipe" <<endl;
                    }
                    // enviar cartas si hay jugadores
                    if(estados[i] == 0){
                        continuar = true;
                        repartirCartas = sacarCarta();
                        if(write(pipes[i][0][1], &repartirCartas, sizeof(int)) == -1){
                            cout <<"Error Proceso Padre al escribir el Pipe" <<endl;
                        }
                    }
                }

            }
        }while(continuar);
    
    cout << "\n";
    cout << "*************** Posiciones ***************" <<endl;
    cout << "------------------------------------------" <<endl;
        
        float puntos;
        int ganador = -1;
        float diferenciaMeta;
        float metaGanador = 7.5;
        
        // Obtiene los puntos de cada jugador (proceso)
        for(int j = 0; j < num; j++){
            puntos = 0;
            if(write(pipes[j][0][1], &puntos, sizeof(int)) == -1){
                cout << "Error Proceso Padre al escribir en el Pipe" <<endl;
            }
            if(read(pipes[j][1][0], &puntos, sizeof(float)) == -1){
                cout << "Error Proceso Padre al leer en el Pipe" <<endl;
            }

            // Verifica en el array de estado si el jugador arriesgo
            // verifica que jugador se acerco mas a la meta y lo
            // proclama ganador
            
            if(estados[j] == 1){
                diferenciaMeta = 7.5 - puntos;
                if(diferenciaMeta < metaGanador){
                    ganador = j;
                    metaGanador = diferenciaMeta;
                }
            }
            // Imprime los resultados de cada jugador
            cout <<"Jugador numero: " << j+1 << " -> ";
        
            if(estados[j] == 1){
                cout <<"Arriesgo";
            }else{
                cout <<"Abandono";
            }
            cout <<" -> Puntos: "<< puntos <<endl;
        }
        
        // Impreme el jugador ganador o si abandonadon el juego
        cout << endl <<"**** El ganador es el jugador número ";
        
        if(ganador == -1){
            cout <<"Ningun jugador gano, todos abandonaron el juego" <<endl;
        }else{
            cout << ganador + 1 << " ****" <<endl;
        }
    
        // Espera que termine cada jugador(proceso) hijo
        cout << "\n";
        for(int i=0; i<num; i++){
            while(wait(NULL) != -1 || errno != ECHILD);
        }

    return 0;
    
}
