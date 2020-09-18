/* 
**********

Laboratorio 4

Universidad Del Valle de Guatemala

Autores:
- Sofía Rueda			Carné: 19099
- Diego Ruiz			Carné: 18761
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
#include <stdlib.h>		//binary convertion
#include <string>		//string usage
#include <sstream>		//string input/output
#include <vector>		//dinamic arrays
#include <unistd.h>		//usleep
#include <bitset>		//binary convertion
#include <semaphore.h>  //semaphore 

using namespace std;

/*******************
Variables globales
*******************/
string const fileName = "FUENTE.txt";	//Nombre del archivo fuente 								(editable)
string const keyWord = "Gu4T"; 			//Palabra clave de cifrado									(editable)
string cypherDone = "";					//Variable que almacenara el texto cifrado					(no editable)
int const bufferLength = 8;				//Espacio del buffer (cuantas letras se cifran seguidas)	(¡¡¡peligroso editar!!!)
int const threadCount = 4;				//Numero de threads utilizados								(editable)

/*----- Variables globales compartidas -----*/
int counter = 0;
pthread_cond_t cola_llena, cola_vacia; 
pthread_mutex_t mutex_forvar; 
sem_t count_sem, barrier_sem, done_sem;


/***************************************************************
Subrutina para leer el archivo fuente y convertirlo a un string
***************************************************************/
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


/*****************************************************************************************
Subrutina para cifrar un string de 8 caracteres (Aquí se utiliza el XOR con cada pthread)
*****************************************************************************************/
void *cypherText(void *argument){
	
	//Se hace la conversion del argumento a un string y se guarda en oldString
	string &oldString = *(static_cast<string*>(argument));
	string newString = "";
	string newStringTwo = "";

	pthread_mutex_lock(&mutex_forvar);
	counter++;

	if(counter == threadCount){
		counter = 0;

		pthread_cond_broadcast(&cola_vacia);
		pthread_cond_wait(&cola_llena, &mutex_forvar);
	}
	else
	{
		counter++;
	}

	
	//***********************************************
	//Se hace el cifrado (ABCD EFGH) --> (efgh abcd)*
	//***********************************************
	//Ciclos for que incluyen los ultimos cuatro caracteres del string 
	
	//Ronda 1
	int a=0;
	for(int i=bufferLength-4;i<bufferLength;i++){
		newString += (oldString[i]^keyWord[a]);
		a++;
	}
	for(int j=0;j<bufferLength-4;j++){
		newString += oldString[j];
	}
	
	//Ronda 2
	oldString="";
	a=0;
	for(int i=bufferLength-4;i<bufferLength;i++){
		oldString += (newString[i]^keyWord[a]);
	}
	for(int j=0;j<bufferLength-4;j++){
		oldString += newString[j];
	}
	
	//Ronda 3
	newString="";
	a=0;
	for(int i=bufferLength-4;i<bufferLength;i++){
		newString += (oldString[i]^keyWord[a]);
	}
	for(int j=0;j<bufferLength-4;j++){
		newString += oldString[j];
	}
	
	//Ronda 4
	oldString="";
	a=0;
	for(int i=bufferLength-4;i<bufferLength;i++){
		oldString += (newString[i]^keyWord[a]);
	}
	for(int j=0;j<bufferLength-4;j++){
		oldString += newString[j];
	}
	
	//Se añade el string cifrado al texto global cifrado
	cypherDone += oldString;

	//Se finaliza el thread
	pthread_mutex_unlock(&mutex_forvar);
	pthread_exit(NULL);

}

//Main
int main(){
	
	//Inicialización de variables
	int rc;
	int option;
	string text;
	bool active = true;
	
	//Se lee el archivo
	text = readFile();
	
	//Se inicializan las variables de pthread, de mutex y de cond
	pthread_t threadID;
	pthread_mutex_init(&mutex_forvar, NULL);
	pthread_cond_init(&cola_llena, NULL); 
	pthread_cond_init(&cola_vacia, NULL);
	
	//Se muestra el banner del programa.
	cout<<"Universidad del Valle de Guatemala"<<endl;
	cout<<"Programación de Microprocesadores"<<endl;
	cout<<"Laboratorio 4: Cifrado de textos"<<endl;
	cout<<"Autores: \nSofía Rueda	Carné: 19099"<<endl;
	cout<<"Diego Ruiz	Carné: 18761 \nMartín España	Carné: 19258"<<endl;
	
	while(active){
		//Se muestra el menu y se solicita una opción.
		cout<<"\nMenu de Opciones \n¿Qué desea hacer? \n1. Cifrar texto. \n2. Salir."<<endl;
		cin>>option;
		
		//Si se elige cifrar el texto...
		if(option == 1){
			if(text==""){
				cout<<"El archivo "<<fileName<<" esta vacio."<<endl;
			}
			else{
				cypherDone = "";			//Se vacía el texto cifrado en caso deseen repetir el proceso
				string temporary = "";		//Variable que sirve para separar el texto
				int letterGroups;			//Cantidad de grupos de 8 caracteres que se formaran a partir del texto
				int a = 0; 					//Contador de las 8 letras
				int rept;					//Contador de las repeticiones (1-8)
				
				//Se determina cuantos grupos de caracteres se crearan a partir del tamaño del buffer definido
				if(((text.length())%bufferLength)>0){
					letterGroups = (int)(text.length()/bufferLength)+1;
				}
				else{
					letterGroups = (int)(text.length()/bufferLength);
				}
				
				//Se crea el array de la longitud de cantidad de grupos de 8 caracteres
				string collection [letterGroups];
				
				//Se recorre el texto y se separa en sus respectivos grupos, los cuales se almacenan en collection
				for(int i=0;i<letterGroups;i++){
					rept = 0;
					
					//Si se estan comparando las últimas letras esto asegura que no va a sobrepasar el limite del texto (out of bounds prevention)
					if((a+bufferLength-1)>(text.length()-1)){
						while(rept < bufferLength && (a+rept)<(text.length()-1)){
							temporary += text[a+rept];
							rept++;
						}
					}
					
					//Si se comparan las letras de cualquier otra parte del texto se pueden separar normalmente
					else{
						while(rept < bufferLength){
							temporary += text[a+rept];
							rept++;
						}
					}
					
					collection[i] = temporary;
					a+=bufferLength;
					temporary = "";
				}
				
				/*****************************************************************************************
				IMPORTANTE: EN ESTE PUNTO, TODOS LOS GRUPOS DE 8 CARACTERES ESTAN EN EL ARRAY "COLLECTION"
				*****************************************************************************************/
				
				//Se crea un vector con los grupos de caracteres (STACK)
				vector<string> stack;
				for(int i=0;i<letterGroups;i++){
					stack.push_back(collection[i]);
				}
				
				/***********************************************************************************************
				Se asigna un grupo de 8 caracteres a cada thread hasta que se agoten los grupos de 8 caracteres.
				Para ello se utiliza la estructura de datos "stack" a través de un vector.
				***********************************************************************************************/
				while(stack.size()>0){
					for(int j=0;j<threadCount && stack.size()>0;j++){
						
						temporary = stack.back();
						stack.pop_back();

						
						rc = pthread_create(&threadID,NULL,cypherText,static_cast<void*>(&temporary));
						usleep(1000);
						
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
					}
				}
				//********************************************************/
				
				//**********************************************************
				//Se crea el archivo de salida y se escribe el texto cifrado
				ofstream outFile("salida.bin",ios::binary);
				if(!outFile)
				{
					cerr<<"Error al crear salida.bin"<<endl;
					exit(EXIT_FAILURE);
				}
				outFile<<cypherDone;
				//**********************************************************
				
			}
			//Si sobrevive hasta aca, la operación fue exitosa
			cout<<"El texto cifrado se ha escrito exitosamente."<<endl;
		}
		
		//Si se elige salir del programa...
		else if(option == 2){
			cout<<"Gracias por utilizar el programa. ¡Vuelva pronto!"<<endl;
			active = false;
		}
		
		//Si ingresan una opción no valida...
		else{
			cout<<"Por favor, ingrese una opción válida"<<endl;
		}
	}

	exit(0);
}
