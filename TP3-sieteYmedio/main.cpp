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

using namespace std;


int mazo[40];

void cargarMazo() {
    for (int p=0; p<4; p++){
        for (int n=0; n<10; n++){
            mazo[p*10+n] = n+1;
        }
    }

int size = sizeof(mazo) / sizeof(mazo[0]);
    for (int i = 0; i < size; i++) {
        std::cout << mazo[i] << " ";
        }
}

int sacarCarta(){
    srand(time(0));
    int x, d;
    string palo;
    
    x = rand()%40;
    while (mazo[x]==0) x = (rand()%40);
    if(x < 10){
        palo = "Oro";
    }
    else if (x > 10 && x < 20) {
        palo = "Bastos";
    }
    else if (x > 20 && x < 30) {
        palo = "Copas";
    } else {
        palo = "Espadas";
    }
    
    d = (mazo[x]>7) ? mazo[x]+2 : mazo[x];
    mazo[x]=0;
    
//    cout << "The value of myVariable x is: " << x << endl;
    cout << "\n La carta es: \"" << d << " de " << palo << "\"" << endl;
    return d;
}



int main(int argc, const char * argv[]) {
    cargarMazo();
    float puntos = 0.0;
    int estado = 0; //sacar otra carta 0 / abandonar 1 / Arriesga 2 /
    
    
    int fd[2];
    if(pipe(fd) == -1){
        cout<<"Error al crear pipe"<<endl;
        return 1;
    }
    
    int id = fork();
    if(id == -1){
        cout<<"Error al crear proceso hijo"<<endl;
        return 2;
    }
    do{
        if(id == 0){ //proceso hijo
            close(fd[0]);
            int carta = sacarCarta();
            write(fd[1], &carta, sizeof(int));
            close(fd[1]);
        }
        else { //proceso padre
            int cartaHijo;
            read(fd[0], &cartaHijo, sizeof(int)); //leo el file descriptor de lectura
            close(fd[0]); //cierro el file descriptor de lectura
            cout<<"Carta jugador x: "<< cartaHijo <<endl;
            if (cartaHijo > 7) {
                cout<<"Otra carta jugador x"<<endl;
                puntos += 0.5;
                estado = 0;
            } else {
                cout<<"Otra carta jugador x"<<endl;
                puntos+= cartaHijo;
                estado = 0;
            }
            if(puntos > 7.5){
                estado = 1;
                cout<<"Jugador x abandona el juego"<<endl;
            }
            if(puntos > 5.0 && puntos <= 7.0){
                estado = 2;
                cout<<"Jugador x arriesga con puntos: "<< puntos <<endl;
            }
            if(puntos == 7.5){
                estado = 3;
                cout<<"Jugador x ganador tiene: "<< puntos <<endl;
            }
        }
    } while(estado == 0);
    
    
    
    
    
//    int numJugadores;
//    
//    cout << " ******** 7 Y MEDIO ********" << endl;
//    cout << " Ingresa la cantidad de jugadores: ";
//    cin >> numJugadores;

    return 0;
    
}
