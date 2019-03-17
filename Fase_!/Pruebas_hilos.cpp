

#include <stdlib.h>
#include <iostream>
#include <string>
#include <sstream>
#include <iterator>
#include <thread>
#include <mutex>
#include <queue>
#include <utility>
#include <list>
using namespace std;
//Variable compartidas por los hilos
queue<string> toCount;
queue<pair<string, int>> answerQ;


string input;
bool stop=false;
mutex qInUse;
mutex answerQInUse;

//Funcíón read que hace un hilo
void read();
//Funcion contar palabras que hace un segundo hilo
void count_words(int delay);

int main(int argc, char* argv[]) 
{
	char stop;
	//Se llama a un hilo para que lea el input
	thread reader(read);
	//Mientras otro hilo esta atento a contar palabras en frases
	thread wordCounter(count_words, atoi(argv[1]));
	//Se unen los hilos para finalizar
	reader.join();
	wordCounter.join();

	cout << "End of main\n";
	
	cin>>input;
	cin >> stop;
	return 0;
} 

//Función que lee palabras.
void read()
{
	list <pair<string, int>> countedLines;
	//Primero se captura la entrada
	printf("Digite una frase y se contaran las palabras. Digite solo ENTER para finalizar la lectura\n");

	//Mientras no sea necesario detenerse
	while (!stop)
	{
		
		//obtiene el input, empieza a detenerse si es una frase vacia. De lo contrario lo encola
		getline(cin, input);
		while (!answerQ.empty())
		{
			bool pushed = false;
			while (!pushed)
			{
				if (answerQInUse.try_lock())
				{
					countedLines.emplace_back(answerQ.front());
					answerQInUse.unlock();
					pushed = true;
					answerQ.pop();
				}
			}
		}

		if ('\0' == input[0])
		{
			stop = true;
		}
		else
		{
			if ("1" != input)
			{
				bool pushed = false;
				while (!pushed)
				{
					//Se hace el encolado, verificando que la cola no esté siendo usada
					if (qInUse.try_lock())
					{
						toCount.push(input);
						qInUse.unlock();
						pushed = true;
					}
				}
			}
			else
			{
				//Se hace el encolado, verificando que la cola no esté siendo usada
				bool printed=false;
				cout << "Antes imprimir" << endl;
				while (!printed)
				{
					if (answerQInUse.try_lock())
					{
						for (auto iter =countedLines.begin(); iter != countedLines.end();++iter)
						{
							cout << "La frase: " << (*iter).first<< "," << " tiene " << (*iter).second << " palabras" << endl;
						}
						answerQInUse.unlock();
						printed = true;
					}
				}
			}
			
		}
	}
}

//Función que cuenta palabras

void count_words(int delay)
{
	//Mientras la cola esté llena y no sea necesario detenerse
	while (!toCount.empty() || !stop)
	{
		//Si la cola no esta vacia (tiene frases por procesar)
		if (!toCount.empty())
		{
			//Toma el string del frente, saquelo de la cola y cuente
			string front;
			bool frontTaken = false;
			while (!frontTaken)
			{
				if (qInUse.try_lock())
				{
					front = toCount.front();
					toCount.pop();
					qInUse.unlock();
					frontTaken = true;

				}
			}
			//Cuente las palabras palabras utilizando iteradores (FALTA ARREGLAR)
			std::stringstream stream(front);
			int numPalabra = std::distance(std::istream_iterator<std::string>(stream), std::istream_iterator<std::string>());
			bool answered = false;
			pair<string, int> answer = make_pair(front, numPalabra);
			while (!answered)
			{
				if (answerQInUse.try_lock())
				{
					cout << "Enviando respuesta..." << endl;
					answerQ.push(answer);
					answerQInUse.unlock();
					answered = true;
				}
			}
		}
		//Si no ha de detenerse, utilice el delay pasado por linea de comando.
		if (!stop)
		{
			this_thread::sleep_for(chrono::seconds(delay));
		}

	}
}
  