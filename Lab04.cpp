/* 
**********

Laboratorio 4

Universidad Del Valle de Guatemala

Autores:
- Sofía Rueda			Carné: 19099
- Diego Ruiz			Carné: 1
- Martín España 		Carné: 19258

Fecha de creación: 13/09/2020

**********

Laboratorio 4: Cifrado de textos

**********
*/

#include <iostream>
#include <stdio.h>		//printf
#include <cstdlib>		//rand
#include <cmath>		//pow
#include <ctime>		//time_t, clock, difftime
#include <pthread.h>	//treads
#include <fstream> 		//file processing
#include <stdlib.h>		//binary conversion
#include <string>		//string usage
#include<sstream>		//string input/output

using namespace std;

//Variables globales
string const fileName = "FUENTE.txt";	//Nombre del archivo fuente
string const keyWord = "Gu4T"; 			//Palabra clave de cifrado
string cypherDone = "";					//Variable que almacenara el texto cifrado
int bufferLength = 8;					//Espacio del buffer (cuantas letras se cifran seguidas)

//Subrutina para leer el archivo fuente y convertirlo a un string
string readFile(){
	//Se crea el stream para leer el archivo fuente
	ifstream fileStream(fileName,ios::in);
	
	//Si hay errores al leer el archivo, e.g. no se encuentra el archivo se cancela la operacion.
	if(!fileStream)
	{
		cerr<<"Error al leer "<<fileName<<endl;
		exit(EXIT_FAILURE);
	}
	
	//Variale que contendra el texto
	string result;
	
	if(fileStream){
		ostringstream receiver;
		receiver<<fileStream.rdbuf();
		result =receiver.str();
	}
	
	return result;
}

//Subrutina para cifrar un string de 8 caracteres (Aquí se utiliza el XOR con cada pthread)
void *cypherText(void *argument){
	//Se hace la conversion del argumento a un string y se guarda en oldString
	string &oldString = *(static_cast<string*>(argument));
	
	//PRUEBA
	cypherDone += oldString;
	
	pthread_exit(NULL);

}

//Main
int main(){
	
	int a = 0; //contador de las 8 letras
	int rc;
	int option;
	string text;
	text = readFile();
	bool active = true;
	pthread_t threadID;
	
	cout<<"Universidad del Valle de Guatemala"<<endl;
	cout<<"Programación de Microprocesadores"<<endl;
	cout<<"Laboratorio 4: Cifrado de textos"<<endl;
	cout<<"Autores: \nSofía Rueda	Carné: 19099"<<endl;
	cout<<"Diego Ruiz	Carné: 1 \nMartín España	Carné: 19258"<<endl;
	
	while(active){
		cout<<"\nMenu de Opciones \n¿Qué desea hacer? \n1. Cifrar texto. \n2. Descifrar texto. \n3. Salir."<<endl;
		cin>>option;
		
		//Si se elige cifrar el texto
		if(option == 1){
			if(text==""){
				cout<<"El archivo "<<fileName<<" esta vacio."<<endl;
			}
			else{

				string temporary = "";

				//For loop donde se crean los threads que cifraran los grupos de 8 caracteres
				for(int i = 0; i<text.length();i++){

					temporary += text[i];
					
					if(a == (bufferLength-1)){
						//Se reinicia el contador de caracteres
						a==0;
						rc = pthread_create(&threadID,NULL,cypherText,static_cast<void*>(&temporary));
						
						//Se verifica que no hubo errores
						if(rc){
							printf("ERROR; return code from pthread_create() is %d\n", rc);
							exit(-1);
						}
						
						//Se espera a que el thread termine
						rc = pthread_join(threadID, NULL);
						
						//Se verifica que no hubo errores
						if(rc){
							printf("ERROR; return code from pthread_join() is %d\n", rc);
							exit(-1);
						}
						
						temporary = "";
					}
					else{
						a++;
					}
				}
			}
			cout<<cypherDone<<endl;
		}
		else if(option == 2){
			cout<<"Opción 2"<<endl;
		}
		else if(option == 3){
			cout<<"Gracias por utilizar el programa. ¡Vuelva pronto!"<<endl;
			active = false;
		}
	}

	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	exit(0);
}