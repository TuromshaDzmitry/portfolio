#include <stdio.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>
#include <string>

const int L = 10;
const int Nsetka = 100;   //[100, 500];
const int Niterac[3] = {100, 1000, 10000};


double topBoundary(double x){
    return -10*x*(x-L);
}


int main() {
    double h = (double)L/(Nsetka+1);
    std::cout<<h<<std::endl;
    double X[Nsetka+1];
    X[0]=0;
    for(int i=1; i<=Nsetka-1; i++) {
        X[i]=X[i-1]+h;   //making grid
    }

    double U1[Nsetka+1][Nsetka+1];
    double U2[Nsetka+1][Nsetka+1];   //copy for next step

    for(int i=0; i<=Nsetka; i++) {   //boundaries
        U1[0][i]=topBoundary(X[i]); //top
        U1[Nsetka][i]=U1[0][i];   //bottom
        U1[i][0]=0; U1[i][Nsetka]=0; //sides

        U2[0][i]=topBoundary(X[i]); //top
        U2[Nsetka][i]=U2[0][i];   //bottom
        U2[i][0]=0; U2[i][Nsetka]=0; //sides
    }

    for(int i=1; i<=Nsetka-1; i++) {  //choosing step to begin from
        for(int j=1; j<=Nsetka-1; j++) {
            U1[i][j] = 0;
            U2[i][j] = 0;
        }
    }

    int numberIterac = 1;
    for(int k=0; k<=Niterac[2]; k++) {
        for(int i=1; i<=Nsetka-1; i++) {
            for(int j=1; j<=Nsetka-1; j++) {
                U2[i][j] = 0.25*(U1[i+1][j]+U1[i-1][j]+U1[i][j-1]+U1[i][j+1]);
            }
        }

        for(int i=0; i<=Nsetka; i++) {
            for(int j=0; j<=Nsetka; j++) {
                U1[i][j] = U2[i][j];
            }
        }

            if(k==Niterac[0]) {
                std::ofstream out( "iteration100.txt" );
                if( !out )
            {
                std::cout << "Couldn't open file to write."  << std::endl;
                return 1;
            }
            for(int i=0; i<=Nsetka; i++) {
                for(int j=0; j<=Nsetka; j++) {
                    out << X[i] <<"	"<< X[j] <<"	"<< U2[i][j] << std::endl;
                }
            }
            out.close();
            } else if(k==Niterac[1]) {
                std::ofstream out( "iteration1000.txt" );
                if( !out )
            {
                std::cout << "Couldn't open file to write."  << std::endl;
                return 1;
            }
            for(int i=0; i<=Nsetka; i++) {
                for(int j=0; j<=Nsetka; j++) {
                    out << X[i] <<"	"<< X[j] <<"	"<< U2[i][j] << std::endl;
                }
            }
            out.close();
            } else if(k==Niterac[2]) {
                std::ofstream out( "iteration10000.txt" );
                if( !out )
            {
                std::cout << "Couldn't open file to write."  << std::endl;
                return 1;
            }
            for(int i=0; i<=Nsetka; i++) {
                for(int j=0; j<=Nsetka; j++) {
                    out << X[i] <<"	"<< X[j] <<"	"<< U2[i][j] << std::endl;
                }
            }
            out.close();
            }

    }
    return 0;
}
