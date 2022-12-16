#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace std;

struct sportsmens{
 int nomer;
 char famailiy[10];
 int rost;
 float ves;
}; struct sportsmens *z1, *z2;

sportsmens* array_of_sportsmens = new sportsmens[50];
FILE* Fs;

int reading() {
    int nzap = 0;
    if ((Fs = fopen("File.txt", "rb")) == NULL) {
        cout << ("Unable to open file!\n");
        return 0; }
    do {
        fread(&array_of_sportsmens[nzap], sizeof(array_of_sportsmens), 1, Fs);
        nzap++;
        if (feof(Fs)) break;
        } while(1);

    nzap--;

    cout << "File created!\n";
    fclose(Fs);
    system("pause");
    return 0;
}

int outp_to_file() {
    FILE* Fs;
    int i = 0, nzap = 0;

    if ((Fs = fopen("File.txt", "wb")) == NULL) {
        cout << "Unable to open file!\n";
        return 0;
    }

    for (int i = 0; i < nzap; i++)
        fwrite(&array_of_sportsmens[i], sizeof(array_of_sportsmens), 1, Fs);

    cout << "File created!";
    fclose(Fs);
    system("pause");
}

int input_in_array() {
    int q = 1, i = 0, nzap = 0;
    while (q != 0) {
        cout << "\nEnter number: ";
        cin >> array_of_sportsmens[nzap].nomer;

        cout << "\nEnter last name: ";
        cin >> array_of_sportsmens[nzap].famailiy[i];
        fflush(stdin);

        cout << "\nEnter height: ";
        cin >> array_of_sportsmens[nzap].rost;

        cout << "\nEnter weight: ";
        cin >> array_of_sportsmens[nzap].ves;

        nzap++;
        cout << "\nEnter 0 to end or any other number to continue: ";
        cin >> q;
        if(q == 0)
            break;
        cout << endl;
    }
}

int content(){
    int nzap=0;

    for (int i = 0; i < nzap; i++) {
        cout << array_of_sportsmens[nzap].nomer;
        cout << array_of_sportsmens[nzap].famailiy;
        cout << array_of_sportsmens[nzap].rost;
        cout << array_of_sportsmens[nzap].ves << endl;
    }

    system("pause");
    return 0;
}

int sort() {
    int k = 1, nzap = 0;

    while (nzap == k) {
        system("cls");
        k++;
    }

    cout<<"Sorting completed!\n";
    system("pause");
}

int dob() {
    int s, i = 0, nzap = 0;
    cout << "Select line number: ";
    cin >> s;

    for (i = nzap-1; i > s ;i--) {
        array_of_sportsmens[nzap].nomer = array_of_sportsmens[nzap-1].nomer+1;
        array_of_sportsmens[nzap].famailiy == array_of_sportsmens[nzap-1].famailiy;
        array_of_sportsmens[nzap].rost = array_of_sportsmens[nzap-1].rost;
        array_of_sportsmens[nzap].ves = array_of_sportsmens[nzap-1].ves;
    }
    input_in_array();

    return 0;
}

int zamena() {
    int nzap = 0;
    cout << "Select line number: ";
    cin >> nzap;
    z1 = &array_of_sportsmens[nzap];

    cout << "Enter a new element: " << endl;
    input_in_array();
    z2 = &array_of_sportsmens[nzap];
    z1 = z2;

    return 0;
}

int main()
{
    int rez = 1, nzap = 0;
    while(rez != 0) {
        system("cls");
        cout << "Menu:\n";
        cout << "1 - Enter data\n";
        cout << "2 - Entering data into a file\n";
        cout << "3 - Data output from file\n";
        cout << "4 - Show\n";
        cout << "5 - Sort\n";
        cout << "6 - Adding an element before the selected element\n";
        cout << "7 - Element replacement\n";
        cout << "8 - Exit\n";
        cout << "-----------------------\n";
        cout << "Number of databases - " << nzap << "\n";
        cout << "-----------------------\n";
        cout << "Enter action number:";
        cin >> rez;

        switch(rez) {
            case 1:input_in_array(); break;
            case 2:outp_to_file(); break;
            case 3:reading(); break;
            case 4:content(); break;
            case 5:sort(); break;
            case 6:dob(); break;
            case 7:zamena(); break;
            case 8: outp_to_file(); exit(0); break;
            default :break;
        }
    }

return 0;
}
