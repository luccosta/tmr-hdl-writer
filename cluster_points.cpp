#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <memory>
#include <iterator>
#include <cstdlib>
#include <cmath>
using namespace std;

class Circuito;
struct Matrices{double M[2][2];};

struct Matriz
{
	double SF[4];
};

struct l_porta
{
	string outp, gate, name;
	vector <string> inps;
	vector <string> tagL;
	bool clusteRoot;
};

class Circuito
{
	friend class Porta_Logica;
	double R; // Confiabilidade total do circuito
	vector <string> input;
	map <string, Matriz> output; // Estruturas associativas com a chave sendo o nome
	map <string, Matriz> wire;	// dado ao elemento e o valor sendo o apontador espec�fico

	vector <double> coeficiente; // Coeficiente an de cada funout
	vector <l_porta> lp; // Linhas de Porta
	vector <string> funout_r; // Funouts reconvergentes
	vector <string> CRL; // Cluster Root List
	vector <string> gates; // Portas no invent�rio "models"

	// Auxiliares
	vector <string> output_aux;
	vector <string> wire_aux;
public:
	Circuito(string arq){
		R = 0;
		setup();
		LeituraNetlist(arq);
	}
	void setup();
	void LeituraNetlist(string);
};

void Circuito::setup(){
	ifstream mod;
	mod.open("models.txt");
	string aux, line, prev;

	if (mod.is_open()){ // Coleta das portas no invent�rio
		mod >> aux;
		while(getline(mod, line)){
			if (prev != aux)
				gates.push_back(aux);
			prev = aux;
			mod >> aux;
		}
	}
}

void Circuito::LeituraNetlist(string arq){
	ifstream nl;
	nl.open(arq.data());
	string aux, aux2, aux3;
	string sub; // String tempor�ria utilizada para armazenar substrings que diminuem gradualmente
	int n_entradas, start = -1;
	unsigned int fixo, it, add = 0;
	char final;
	string porta, saida;

	l_porta tp; // Cada elemento � correspondente a uma linha do Verilog (tempor�rio)

	if (nl.is_open())
	{
		while(getline(nl, aux))
		{
			if ((aux[0] == '/' && aux[1] == '/') || aux == "") {start++;}
				// 		Coment�rio				Linha em branco
			else {
				if (aux[0]!= ' ')
					aux2 = aux.substr(0, aux.find(' '));
				aux3 = '.';

				if (aux2 == "module"){start++;}
					// Desnecess�rio. Somente necess�rio para reutiliza��o no Verilog
				if (aux2 == "input" || aux2 == "output" || aux2 == "wire"){ // Inicializa��o dos elementos b�sicos
					start++;
					if (aux[0] != ' ')
						it = aux.find(' ') + 1;
					else if (aux2 == "wire")
						it = 5;
					else if (aux2 == "input")
						it = 6;
					else if (aux2 == "output")
						it = 7;
					fixo = it;

					while(it < aux.find_last_of(',') && aux.find_last_of(',') != -1){
						sub = aux.substr(it, aux.size() - it);
						aux3 = sub.substr(0, sub.find(','));
						if (aux2 == "input")
							input.push_back(aux3);
						if (aux2 == "output")
							output_aux.push_back(aux3);
						if (aux2 == "wire")
							wire_aux.push_back(aux3);
						it = fixo + sub.find_first_of(',') + 1;
						fixo = it;
					}
					if (aux.find(';') != -1){ // Leitura do ultimo elemento antes do ;
						sub = aux.substr(it, aux.size() - it);
						if (aux2 == "input")
							input.push_back(sub.substr(0, sub.size() - 1));
						if (aux2 == "output")
						   	output_aux.push_back(sub.substr(0, sub.size() - 1));
						if (aux2 == "wire")
						   	wire_aux.push_back(sub.substr(0, sub.size() - 1));
					}
				}

				// Desenvolvimento das portas
				for(vector <string>::iterator itr = gates.begin(); itr != gates.end(); ++itr)
				if (aux2 == *itr){
					tp.inps.clear();

					if (aux2 == "buf") // GAMBIARRA
						add = 1;

					it = aux.find(' ') + 1;
					sub = aux.substr(it, aux.size() - it);
					porta = sub.substr(0, sub.find(" ")); // Nome da porta l�gica
					n_entradas = - 48 + (int)porta[aux2.size() + add]; // N�mero de entradas da porta l�gica
					it = sub.find(' ') + 2;
					sub = sub.substr(it, aux.size() - it); // Elimina��o do nome da porta e do primeiro parentese
					saida = sub.substr(0, sub.find(','));

					add = 0;

					tp.gate = aux2;
					tp.name = porta;
					tp.outp = saida;

					// ARGUMENTOS DE ENTRADA
					for (int i = 0; i < n_entradas; i++){
						if (i == n_entradas - 1)
							final = ')';
						else
							final = ',';
						it = sub.find(',') + 2;
						sub = sub.substr(it, sub.size() - it);
						aux3 = sub.substr(0, sub.find(final));
						tp.inps.push_back(aux3);
					}
					lp.push_back(tp);
				}
			}
		}
	}

	// Captura de funouts reconvergentes
	for(vector <l_porta>::iterator busca = lp.begin(); busca !=lp.end() - 1; ++busca)
		for(vector <l_porta>::iterator rpeat = busca+1; rpeat !=lp.end(); ++rpeat)
			for(vector <string>::iterator it = (*busca).inps.begin(); it !=(*busca).inps.end(); ++it)
				for(vector <string>::iterator itr = (*rpeat).inps.begin(); itr !=(*rpeat).inps.end(); ++itr)
					if (*it == *itr)
						funout_r.push_back(*it);

	// Apagando repetidos
	//if (!funout_r.empty())
	//	for(vector <string>::iterator itra = funout_r.begin(); itra !=funout_r.end() - 1; ++itra)
	//		for(vector <string>::iterator itr = itra + 1; itr !=funout_r.end(); ++itr)
	//			if (*itra == *itr){
	//				funout_r.erase(itr);
	//			}

	// CLUSTERIZA��O
	bool out, alredyExist;
	map <string, vector<string> > tagMemory; // <outp, pr�ximos n�s>
	vector <string> auxS;

	// Forma��o tagMemory
	for (vector<l_porta>::reverse_iterator it = lp.rbegin(); it != lp.rend(); ++it)
	{
		for (vector<string>::iterator itra = (it->inps).begin(); itra != (it->inps).end(); ++itra){
			alredyExist = false;
			for(map<string, vector<string> >::iterator itr = tagMemory.begin(); itr != tagMemory.end(); ++itr)
				if (*itra == itr -> first){
					alredyExist=true;
					(itr->second).push_back(it->outp);
				}

			if (!alredyExist){
				auxS.push_back(it->outp);
				tagMemory.insert(pair<string, vector<string> > (*itra, auxS));
				auxS.clear();
			}
		}
	}

	// Forma��o tagList
	for (vector<l_porta>::reverse_iterator it = lp.rbegin(); it != lp.rend(); ++it)
	{
		out = false;
		for (vector<string>::iterator iter = output_aux.begin(); iter != output_aux.end(); ++iter)
			if (it->outp == *iter)
				out = true;

		if (out) {
			it->tagL.push_back(it->outp);
		}
		else {
			for (map<string, vector<string> >::iterator itra = tagMemory.begin(); itra != tagMemory.end(); ++itra){
				if (itra -> first == it->outp)
					for (vector<string>::iterator ite = (itra->second).begin(); ite != (itra->second).end(); ++ite)
						for (vector<l_porta>::iterator itr = (it+1).base()+1; itr != lp.end(); ++itr)
							if (itr->outp == *ite)
							{
								for(vector<string>::iterator iter = (itr->tagL).begin(); iter != (itr->tagL).end(); ++iter)
									(it->tagL).push_back(*iter);
							}
			}

			for(vector<string>::iterator iter = (it->tagL).begin(); iter != (it->tagL).end(); ++iter) // Exclus�o dos repetidos
			{
				alredyExist = false;
				for(vector<string>::iterator itr = auxS.begin(); itr != auxS.end(); ++itr)
					if (*iter == *itr)
						alredyExist = true;

				if(!alredyExist){
					auxS.push_back(*iter);
				}
			}
			it->tagL = auxS;
			auxS.clear();
		}
	}

	// Forma��o Cluster Root List
	for (vector<l_porta>::reverse_iterator it = lp.rbegin(); it != lp.rend(); ++it)
	{
		it->clusteRoot = false;
		out = false;
		for (vector<string>::iterator iter = output_aux.begin(); iter != output_aux.end(); ++iter)
			if (it->outp == *iter)
				out = true;

		if (out) {
			CRL.push_back(it->outp);
			it->clusteRoot = true;
		}
		else {
			it->clusteRoot = true;
			for (map<string, vector<string> >::iterator itra = tagMemory.begin(); itra != tagMemory.end(); ++itra)
				if (itra -> first == it->outp)
					for (vector<string>::iterator ite = (itra->second).begin(); ite != (itra->second).end(); ++ite)
						for (vector<l_porta>::iterator itr = (it+1).base()+1; itr != lp.end(); ++itr)
							if (itr->outp == *ite)
								if (((it->tagL).size() == (itr->tagL).size())) // Tamanho da TL do pai igual a do filho
									it->clusteRoot = false;

			if (it->clusteRoot == true)
				CRL.push_back(it->outp);
		}
	}

	cout << "Cluster Root List: ";
	for(vector<string>::iterator iter = CRL.begin(); iter != CRL.end(); ++iter)
		cout << *iter << " ";
	cout << endl;
}

int main(int argc, char** argv)
{
    string arq;

	cout << fixed;
	cout << setprecision(6);

	arq = argv[1]; // Definição do arquivo verilog

	Circuito Main(arq);

    return 0;
}