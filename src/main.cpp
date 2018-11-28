#include "readData.h"
#include "Util.h"
#include "json.hpp"

#include <string>
#include <random>
#include <bits/stdc++.h>
#include <fstream>
#include <iostream>
#include <cmath>
#include <algorithm>

using namespace std;
using json = nlohmann::json;

typedef struct{
  int pos;
  int vertice;
  int custo;
} tInsercao;

typedef struct{
  int posVertice;
  int posInsercao;
  int vertice;
} tReinsercao;

typedef struct{
  int pos1;
  int vertice1;
  int pos2;
  int vertice2;
} tSwap;

// Pega da instâncias
double ** matrizAdj; // matriz de adjacencia
int dimension; // quantidade total de vertices

//Benchmark
json benchmarkReinsercao; // Json
json benchmarkSwap; // Json
json benchmarkTwoOpt; // Json
json benchmarkDoubleBridge; // Json
json benchmark; // Json

int qtdMelhorasReinsercao = 0;
int qtdReinsercoes = 0;
double tempoTotalReinsercao = 0;
double tempoReinsercao = 0;

int qtdMelhorasSwap = 0;
double tempoTotalSwap = 0;
double tempoSwap = 0;

int qtdMelhorasTwoOpt = 0;
double tempoTotalTwoOpt = 0;
double tempoTwoOpt = 0;

int qtdMelhorasDoubleBridge = 0;
double tempoTotalDoubleBridge = 0;
double tempoDoubleBridge = 0;
//

// Untils
void printData(); // Mostra matriz de adjacencia
void printSolucao(vector<int> &solucao, int tamanhoArray); // Mostra a solução inicial gerada pel algorítimo escolhido
void custoSolucao(int *custoTotal, vector<int> solucao, int tamanhoArray); // Mostra o custo da solução gerada
bool compareByCost(const tInsercao &data1, const tInsercao &data2);

//GILS-RVND
int construtivo(vector<int> &solucao, int coleta, int deposito, float alpha);
int rvnd(vector<int> &solucao, int custoDaSolucaoAnterior);
int gilsRvnd(vector<int> &solucao, int Imax, int Iils);

// Vizinhanças
int reinsertion(vector<int> &solucao, int blocoSize, int custoDaSolucaoAnterior);
int swap(vector<int> &solucao, int custoDaSolucaoAnterior);
int twoOptN(vector<int> &solucao, int custoDaSolucaoAnterior);

//Pertubações
int doubleBridge(vector<int> &solucao, int custoDaSolucaoAnterior);

//MAIN
int main(int argc, char** argv){
    readData(argc, argv, &dimension, &matrizAdj);
    //printData();

    vector<int> solucao;
    
    int custoSoluca, custo;
    int flag = 0;
    int maxIls = (dimension > 150) ? dimension/2 : dimension; 
    double tempoInicial;

    for(int i = 0; i < 10; i++){
      tempoInicial = cpuTime();

      custoSoluca = gilsRvnd(solucao, 50, maxIls);

      cout << "Custo: " << custoSoluca << endl;
      cout << "Tempo total: " << cpuTime() - tempoInicial << endl;
      cout << "Tempo das reinsercoes: " << tempoTotalReinsercao << endl;
      cout << "Tempo dos swaps: " << tempoTotalSwap << endl;
      cout << "Tempo dos two opt n: " << tempoTotalTwoOpt<< endl;
      cout << "Tempo dos double bridges: " << tempoTotalDoubleBridge << endl;

      tempoTotalReinsercao = 0;
      tempoTotalDoubleBridge = 0;
      tempoTotalSwap = 0;
      tempoTotalTwoOpt = 0;

      tempoInicial = 0;

      cout << endl;
    }

    printSolucao(solucao, dimension);

    // //Declara variáveis do método construtivo
    // random_device rd;
    // mt19937 mt(rd());
    // uniform_real_distribution<float> linear_f1(0.0, 0.5);
    // int coleta = 1;
    // int deposito = 1;
    // float alpha = 0.0;

    // //Gera solução inicial
    // while(1){
    //   alpha = linear_f1(mt);
    //   custoSoluca = construtivo(solucao, coleta, deposito, alpha);
    //   printSolucao(solucao, dimension);

    //   custoSoluca = reinsertion(solucao, 3, custoSoluca);
    //   printSolucao(solucao, dimension);

    //   custoSolucao(&custo, solucao, solucao.size());
    //   cout << "Custo: " << custoSoluca << " - Testado: " << custo << endl;

    //   if(custoSoluca != custo) break;
    // }

    //Benchmark
    benchmark["tempo"] = cpuTime() - tempoInicial;

    string arquivo = argv[1];
    string instancia;
    string barra ("/");
    string ponto (".");

    //Benchmark
    for(size_t i = 0; i < arquivo.size(); i++){
      if(flag == 2 && arquivo[i] == ponto[0]) break;
      else if(flag == 2) instancia.push_back(arquivo[i]);
      else if(arquivo[i] == barra[0]) flag++;
    }

    //Benchmark
    ofstream benchmarkArchiveReinsercao("../archive/" + instancia + "_reinsercao_benchmark" + ".json");
    ofstream benchmarkArchiveSwap("../archive/" + instancia + "_swap_benchmark" + ".json");
    ofstream benchmarkArchiveTwoOpt("../archive/" + instancia + "_twoOpt_benchmark" + ".json");
    ofstream benchmarkArchiveDoubleBridge("../archive/" + instancia + "_doubleBridge_benchmark" + ".json");
    ofstream benchmarkArchive("../archive/" + instancia + "_time" + ".json");


    //Benchmark
    benchmarkArchiveReinsercao << setw(4) << benchmarkReinsercao << endl;
    benchmarkArchiveSwap << setw(4) << benchmarkSwap << endl;
    benchmarkArchiveTwoOpt << setw(4) << benchmarkTwoOpt << endl;
    benchmarkArchiveDoubleBridge << setw(4) << benchmarkDoubleBridge << endl;
    benchmarkArchive << setw(4) << benchmark << endl;

    return 0;
}

//GILS-RVND
int construtivo(vector<int> &solucao, int coleta, int deposito, float alpha){
  // Inicia variáveis
  vector<int> verticesList; // Lista de vertices faltando
  vector<tInsercao> bestVerticesList; // Lista dos X melhores verticesList

  random_device rd;
  mt19937 mt(rd()); // Gerador de números aleatórios

  int rBest;
  int randomVertice1, randomVertice2, randomVertice3;
  int tamanhoSolucao;
  int verticesRestantes;
  int distanciaVertice;
  int custoSolucao = 0;
  tInsercao insercao;

  //Adiciona coleta ao vector
  solucao.clear();

  solucao.push_back(coleta);

  // Gera lista de vertices faltantes
  for (size_t i = 1; i <= dimension; i++) {
    if(i == coleta || i == deposito) continue;
    verticesList.push_back(i);
  }

  // Escolhe três vertices de forma aleatória
  for (size_t i = 0; i < 3; i++) {
    uniform_int_distribution<int> linear_i(0, verticesList.size()-1); // Distribuição linear de inteiros para escolher vertice inicial
    randomVertice1 = linear_i(mt);

    solucao.push_back(verticesList[randomVertice1]);
    verticesList.erase(verticesList.begin() + randomVertice1);

    custoSolucao += matrizAdj[solucao[i]][solucao[i+1]];
  }

  // Adiciona deposito ao vector
  solucao.push_back(deposito);

  custoSolucao += matrizAdj[solucao[3]][solucao[4]];

  // Gera solução
  while(1) {
    tamanhoSolucao = solucao.size();
    verticesRestantes = verticesList.size();
    distanciaVertice = 0;

    // Gera lista de custo de vertices
    for (size_t i = 0; i < verticesRestantes; i++) { // Itera sobre vértices restantes
      for (size_t j = 1; j < tamanhoSolucao; j++) { // Itera sobre a solução
        insercao.vertice = verticesList[i];
        insercao.pos = j;
        insercao.custo = (matrizAdj[solucao[j-1]][verticesList[i]] + matrizAdj[verticesList[i]][solucao[j]]) - matrizAdj[solucao[j-1]][solucao[j]];
        bestVerticesList.push_back(insercao);
      }
    }

    //Arruma lista pelo valor do custo
    sort(bestVerticesList.begin(), bestVerticesList.end(), compareByCost);

    // Adiciona novo vertice à solução conforme alpha
    uniform_int_distribution<int> linear_i_alpha(0, int((bestVerticesList.size()-1)*alpha));
    rBest = linear_i_alpha(mt);
    solucao.emplace(solucao.begin() + bestVerticesList[rBest].pos, bestVerticesList[rBest].vertice);

    //Calcula custo
    custoSolucao += bestVerticesList[rBest].custo;

    // Reseta Vectors
    for (size_t i = 0; i < verticesRestantes; i++) {
      if(bestVerticesList[rBest].vertice == verticesList[i]) {
        verticesList.erase(verticesList.begin() + i);
        break;
      }
    }
    bestVerticesList.clear();

    // Se não tiverem mais candidatos o loop acaba
    if(verticesRestantes == 1) {
      break;
    }
  }

  return custoSolucao;

  // // Mostra solução
  // for (size_t i = 0; i < solucao.size(); i++){
  //   cout << solucao[i] << " ";
  //   //cout << solucaoInicial[i] << " ";
  // }

}

int rvnd(vector<int> &solucao, int custoDaSolucaoAnterior){
  vector<int> vizinhanca = {1, 2, 3, 4, 5};
  vector<int> vizinhancaInicial = {1, 2, 3, 4, 5};
  vector<int> solucaoTeste;

  random_device rd;
  mt19937 mt(rd());

  int solucaoSize = solucao.size();
  int vizinhancaSize = vizinhanca.size();
  int vizinhoRandom = 0;
  int random = 0;
  int novoCusto = 0;
  int custoAnterior = custoDaSolucaoAnterior;

  solucaoTeste = solucao;

  while(1){
    uniform_int_distribution<int> linear_i(0, vizinhanca.size() - 1);
    random = linear_i(mt);
    vizinhoRandom = vizinhanca[random];

    switch(vizinhoRandom){
      case 1:
        novoCusto = reinsertion(solucaoTeste, 1, custoAnterior);
        break;
      case 2:
        novoCusto = reinsertion(solucaoTeste, 2, custoAnterior);
        break;
      case 3:
        novoCusto = reinsertion(solucaoTeste, 3, custoAnterior);
        break;
      case 4:
        novoCusto = swap(solucaoTeste, custoAnterior);
        break;
      case 5:
        novoCusto = twoOptN(solucaoTeste, custoAnterior);
        break;
      default:
        break;
    }

    if(novoCusto < custoAnterior){
      custoAnterior = novoCusto;

      solucao.clear();
      solucao = solucaoTeste;

      vizinhanca.clear();
      vizinhanca = vizinhancaInicial;

    } else {
      vizinhanca.erase(vizinhanca.begin() + random);
    }

    //printSolucao(solucao, solucao.size());
    //cout << novoCusto << endl;

    if(vizinhanca.empty()) break;
  }

  return custoAnterior;
}

int gilsRvnd(vector<int> &solucaoFinal, int Imax, int Iils){
  //Declara Variáveis
  random_device rd;
  mt19937 mt(rd());
  uniform_real_distribution<float> linear_f(0.0, 0.5); // Distribuição linear de reais para gerar alpha

  vector<int> solucaoParcial;
  vector<int> solucaoMelhor;

  int custoFinal = INT32_MAX;
  int custoParcial = 0;
  int custoMelhor = 0;
  int coleta = 1;
  int deposito = 1;
  int interILS;
  int solucaoSize;
  float alpha = 0.0;

  //Busca Melhor Solução
  for (size_t i = 0; i < Imax; i++) {
    //Gera Alfa e o interador de ILS
    alpha = linear_f(mt);
    interILS = 0;

    //Calcula solução parcial com o método construtivo
    solucaoParcial.clear();
    custoParcial = construtivo(solucaoParcial, coleta, deposito, alpha);
    solucaoSize = solucaoParcial.size();

    //Registra a solução parcial como melhor solução
    custoMelhor = custoParcial;
    solucaoMelhor = solucaoParcial;

    //Busca o melhor ótimo local a partir da solução encontrada no construtivo
    while(interILS < Iils){
      //Busca melhor ótimo local da solução parcial
      custoParcial = rvnd(solucaoParcial, custoParcial);

      //Testa se houve melhora
      if(custoParcial < custoMelhor){
        //Registra a solução parcial como melhor solução
        custoMelhor = custoParcial;
        solucaoMelhor = solucaoParcial;

        qtdMelhorasDoubleBridge++;

        //Zera o iterador
        interILS = 0;
      }
      

      //Pertuba a solução
      custoParcial = doubleBridge(solucaoParcial, custoParcial);

      //Soma o interador
      interILS++;
    }

    //Testa se houve melhora
    if(custoMelhor < custoFinal){
      custoFinal = custoMelhor;
      solucaoFinal = solucaoMelhor;
    }

    //Benchmark
    benchmarkReinsercao[i]["Iteração"] = to_string(i);
    benchmarkReinsercao[i]["qtdMelhoras"] = qtdMelhorasReinsercao;
    benchmarkReinsercao[i]["tempo"] = tempoReinsercao;

    tempoTotalReinsercao += tempoReinsercao;
    qtdMelhorasReinsercao = 0;
    tempoReinsercao = 0;

    //Benchmark
    benchmarkSwap[i]["Iteração"] = to_string(i);
    benchmarkSwap[i]["qtdMelhoras"] = qtdMelhorasSwap;
    benchmarkSwap[i]["tempo"] = tempoSwap;

    tempoTotalSwap += tempoSwap;
    qtdMelhorasSwap = 0;
    tempoSwap = 0;

    //Benchmark
    benchmarkTwoOpt[i]["Iteração"] = to_string(i);
    benchmarkTwoOpt[i]["qtdMelhoras"] = qtdMelhorasTwoOpt;
    benchmarkTwoOpt[i]["tempo"] = tempoTwoOpt;

    tempoTotalTwoOpt += tempoTwoOpt;
    qtdMelhorasTwoOpt = 0;
    tempoTwoOpt = 0;

    //Benchmark
    benchmarkDoubleBridge[i]["Iteração"] = to_string(i);
    benchmarkDoubleBridge[i]["qtdMelhoras"] = qtdMelhorasDoubleBridge;
    benchmarkDoubleBridge[i]["tempo"] = tempoDoubleBridge;

    tempoTotalDoubleBridge += tempoDoubleBridge;
    qtdMelhorasDoubleBridge = 0;
    tempoDoubleBridge = 0;

  }

  return custoFinal;
}

// Vizinhanças
int reinsertion(vector<int> &solucao, int blocoSize, int custoDaSolucaoAnterior){
  // Inicia variáveis
  vector<int> aux;

  int deltaCusto = 0;
  int custoRetirada = 0;
  int custoInsercao = 0;
  int custoDaSolucao = custoDaSolucaoAnterior;
  int solucaoSize = solucao.size();
  int iterSize = solucaoSize - blocoSize;
  double tempoInicial = cpuTime();
  bool flag = false, flag2 = false;
  tReinsercao insercao;

  qtdReinsercoes++;

  //Procura local de melhor reinserção
  for(size_t i = 1; i < iterSize; i++){ 
    custoRetirada = matrizAdj[solucao[i-1]][solucao[i+blocoSize]] - (matrizAdj[solucao[i-1]][solucao[i]] + matrizAdj[solucao[i+blocoSize-1]][solucao[i+blocoSize]]);

    for(size_t y = 0; y < solucaoSize - 1; y++){
      if(y <= i + blocoSize - 1 && y >= i - 1) continue;
      custoInsercao = (matrizAdj[solucao[y]][solucao[i]] + matrizAdj[solucao[i+blocoSize-1]][solucao[y+1]]) - matrizAdj[solucao[y]][solucao[y+1]];
      //cout << i << " " << y << endl;
      if((custoRetirada + custoInsercao) < deltaCusto){
        //cout << i << " - " << y << endl;
        if(i < (y+1)) flag = false;
        else flag = true;

        deltaCusto = custoInsercao + custoRetirada;
        insercao.posVertice = i;
        insercao.posInsercao = y+1;
        insercao.vertice = solucao[i];
      }
    }

    // for(size_t y = i + blocoSize; y < solucaoSize - 1; y++){
    //   custoInsercao = (matrizAdj[solucao[y]][solucao[i]] + matrizAdj[solucao[i+blocoSize-1]][solucao[y+1]]) - matrizAdj[solucao[y]][solucao[y+1]];
    //   //cout << custoInsercao << endl;
    //   if((custoRetirada + custoInsercao) < deltaCusto){
    //     flag = true;
    //     flag2 = false;
        
    //     deltaCusto = custoInsercao + custoRetirada;
    //     insercao.posVertice = i;
    //     insercao.posInsercao = y+1;
    //     insercao.vertice = solucao[i];
    //     cout << "for1" << endl;
    //   }
    // }

    // for(int y = i - 2; y >= 0; y--){
    //   custoInsercao = (matrizAdj[solucao[y]][solucao[i]] + matrizAdj[solucao[i+blocoSize-1]][solucao[y+1]]) - matrizAdj[solucao[y]][solucao[y+1]];
      
    //   if((custoRetirada + custoInsercao) < deltaCusto){
    //     //cout << custoRetirada + custoInsercao << endl;
    //     flag = true;
    //     flag2 = true;
    //     //cout << "Entrou" << endl;
    //     deltaCusto = custoInsercao + custoRetirada;
    //     insercao.posVertice = i;
    //     insercao.posInsercao = y+1;
    //     insercao.vertice = solucao[i];
    //     cout << "for2" << endl;
    //   }
    // }
  }

  //Aplica reinserção
  if(flag){
    custoDaSolucao = custoDaSolucao + deltaCusto;
    //cout << insercao.posVertice << " " << insercao.posInsercao << " " << blocoSize; 

    //printSolucao(solucao, solucao.size());

    for(size_t i = 0; i < blocoSize; i++){
      aux.push_back(solucao[insercao.posVertice]);
      solucao.erase(solucao.begin() + insercao.posVertice);
    }

    if(insercao.posInsercao > insercao.posVertice) insercao.posInsercao -= blocoSize; 

    solucao.insert(solucao.begin() + insercao.posInsercao, aux.begin(), aux.begin() + aux.size());

    //printSolucao(solucao, solucao.size());
    //printSolucao(aux, aux.size());

    // if(blocoSize == 1){
    //   if(!flag){
    //     solucao.insert(solucao.begin() + insercao.posInsercao, solucao[insercao.posVertice]);
    //     solucao.erase(solucao.begin() + insercao.posVertice);
    //   } else {
    //     solucao.insert(solucao.begin() + insercao.posInsercao, solucao[insercao.posVertice]);
    //     solucao.erase(solucao.begin() + insercao.posVertice + blocoSize);
    //   }

    // } else {
    //   if(!flag){

    //   } else {

    //   }
    // }

    // if(blocoSize == 1 && flag2 == false){
    //   solucao.insert(solucao.begin() + insercao.posInsercao, solucao[insercao.posVertice]);
    //   solucao.erase(solucao.begin() + insercao.posVertice);
    // } else if(flag2 == false) {
    //   solucao.insert(solucao.begin() + insercao.posInsercao, solucao.begin() + insercao.posVertice, solucao.begin() + insercao.posVertice + blocoSize);
    //   solucao.erase(solucao.begin() + insercao.posVertice, solucao.begin() + insercao.posVertice + blocoSize);
    // }
    // //cout << "Eita: " << flag2 << endl;
    // if(blocoSize == 1 && flag2 == true){
    //   solucao.insert(solucao.begin() + insercao.posInsercao, solucao[insercao.posVertice]);
    //   solucao.erase(solucao.begin() + insercao.posVertice + blocoSize);
    // } else if(flag2 == true) {
    //   printSolucao(solucao, solucao.size());  
    //   cout << solucao[insercao.posVertice] << " " << solucao[insercao.posVertice + blocoSize] << endl;
    //   solucao.insert(solucao.begin() + insercao.posInsercao, solucao.begin() + insercao.posVertice + blocoSize, solucao.begin() + insercao.posVertice + blocoSize + blocoSize);
    //   printSolucao(solucao, solucao.size());
    //   solucao.erase(solucao.begin() + insercao.posVertice + blocoSize, solucao.begin() + insercao.posVertice + (blocoSize*2));
    // }

    //cout << deltaCusto << endl; 
    
    //cout << insercao.posVertice << " " << insercao.posInsercao <<  " " << flag2 << " " << insercao.posVertice + blocoSize << endl; 

    // if(custoDaSolucao < 3323){
    //   printSolucao(solucao, solucao.size());
    //   cout <<  custoDaSolucao << endl; 
    //   cout << flag2 << endl;

    //   return 0;
    // }
    

    flag = false;
    flag2 = false;
    deltaCusto = 0;

  } 
  
  //Benchmark
  if(custoDaSolucao < custoDaSolucaoAnterior){
    qtdMelhorasReinsercao++;
  }

  tempoReinsercao += cpuTime() - tempoInicial;

  return custoDaSolucao;
}

int swap(vector<int> &solucao, int custoDaSolucaoAnterior){
  //Inicia variáveis
  int deltaCusto = 0;
  int custoRetirada = 0;
  int custoInsercao = 0;
  int custoDaSolucao = custoDaSolucaoAnterior;
  int solucaoSize = solucao.size();
  double tempoInicial = cpuTime();
  bool flag = false;
  tSwap swap;


  //Aplica reinserção
  for(size_t i = 1; i < solucaoSize - 2; i++){
    for(size_t y = i+1; y < solucaoSize - 1; y++){
      if(y == i+1){
        custoRetirada = matrizAdj[solucao[i-1]][solucao[y+1]] - (matrizAdj[solucao[i-1]][solucao[i]] + matrizAdj[solucao[i]][solucao[y]] + matrizAdj[solucao[y]][solucao[y+1]]);
        custoInsercao = (matrizAdj[solucao[i-1]][solucao[y]] + matrizAdj[solucao[y]][solucao[i]] + matrizAdj[solucao[i]][solucao[y+1]]) - matrizAdj[solucao[i-1]][solucao[y+1]];
      } else {
        custoRetirada = matrizAdj[solucao[i-1]][solucao[i]] + matrizAdj[solucao[i]][solucao[i+1]] +
                        matrizAdj[solucao[y-1]][solucao[y]] + matrizAdj[solucao[y]][solucao[y+1]];

        custoInsercao = matrizAdj[solucao[i-1]][solucao[y]] + matrizAdj[solucao[y]][solucao[i+1]] +
                        matrizAdj[solucao[y-1]][solucao[i]] + matrizAdj[solucao[i]][solucao[y+1]];
      }

      if((custoInsercao - custoRetirada) < deltaCusto){
        flag = true;

        deltaCusto = custoInsercao - custoRetirada;
        swap.pos1 = i;
        swap.vertice1 = solucao[i];
        swap.pos2 = y;
        swap.vertice2 = solucao[y];
      }
    }
  }

  if(flag){
    custoDaSolucao = custoDaSolucao + deltaCusto;

    solucao.erase(solucao.begin() + swap.pos2);
    solucao.emplace(solucao.begin() + swap.pos2, swap.vertice1);

    solucao.erase(solucao.begin() + swap.pos1);
    solucao.emplace(solucao.begin() + swap.pos1, swap.vertice2);

    flag = false;
    deltaCusto = 0;

  }


  //Benchmark
  if(custoDaSolucao < custoDaSolucaoAnterior){
    qtdMelhorasSwap++;
  }

  tempoSwap += cpuTime() - tempoInicial;

  return custoDaSolucao;
}

int twoOptN(vector<int> &solucao, int custoDaSolucaoAnterior){
  //Inicia variáveis
  int deltaCusto = 0;
  int custoRetirada = 0;
  int custoInsercao = 0;
  int custoDaSolucao = custoDaSolucaoAnterior;
  int aux = 0;
  int solucaoSize = solucao.size();
  int sizeSwap;
  double tempoInicial = cpuTime();
  bool flag = false;
  tSwap swap;


  for(size_t i = 1; i < solucaoSize - 3; i++){
    for(size_t y = i + 3; y < solucaoSize; y++){
      custoRetirada = matrizAdj[solucao[i]][solucao[i+1]] + matrizAdj[solucao[y-1]][solucao[y]];
      custoInsercao = matrizAdj[solucao[i]][solucao[y-1]] + matrizAdj[solucao[i+1]][solucao[y]];

      if((custoInsercao - custoRetirada) < deltaCusto){
        flag = true;

        deltaCusto = custoInsercao - custoRetirada;
        swap.pos1 = i + 1;
        swap.vertice1 = solucao[i];
        swap.pos2 = y - 1;
        swap.vertice2 = solucao[y];
      }
    }
  }

  if(flag){
    custoDaSolucao = custoDaSolucao + deltaCusto;

    sizeSwap = swap.pos2 - swap.pos1;
    for(size_t i = 0; i < sizeSwap; i++){
      aux = solucao[swap.pos2];
      solucao.erase(solucao.begin() + swap.pos2);
      solucao.emplace(solucao.begin() + swap.pos1 + i, aux);
    }

    flag = false;
    deltaCusto = 0;

  }
  
  
  //Benchmark
  if(custoDaSolucao < custoDaSolucaoAnterior){
    qtdMelhorasTwoOpt++;
  }

  tempoTwoOpt+= cpuTime() - tempoInicial;

  return custoDaSolucao;
}

//Pertubações
int doubleBridge(vector<int> &solucao, int custoDaSolucaoAnterior){
  random_device rd;
  mt19937 mt(rd());
  int sizeBlock = (solucao.size() - 2) / 3;
  uniform_int_distribution<int> linear_i(1, sizeBlock);
  uniform_int_distribution<int> linear_p1(1, ((solucao.size() - 2)/2) - sizeBlock);
  uniform_int_distribution<int> linear_p2(((solucao.size() - 2)/2) + 1, solucao.size() - 1 - sizeBlock);
  int bloco1 = linear_i(mt), bloco2 = linear_i(mt);
  int pos1 = linear_p1(mt), pos2 = linear_p2(mt);
  int custoInicial;
  int custoFinal;
  int deltaCusto = 0;
  int aux;
  double tempoInicial = cpuTime();

  custoInicial = (matrizAdj[solucao[pos1 - 1]][solucao[pos1 + bloco1]] + matrizAdj[solucao[pos2 - 1]][solucao[pos2 + bloco2]]) - 
                 (matrizAdj[solucao[pos1 - 1]][solucao[pos1]] + matrizAdj[solucao[pos1 + bloco1 - 1]][solucao[pos1 + bloco1]] + 
                  matrizAdj[solucao[pos2 - 1]][solucao[pos2]] + matrizAdj[solucao[pos2 + bloco2 - 1]][solucao[pos2 + bloco2]]);

  custoFinal =(matrizAdj[solucao[pos1 - 1]][solucao[pos2]] + matrizAdj[solucao[pos2 + bloco2 - 1]][solucao[pos1 + bloco1]] + 
               matrizAdj[solucao[pos2 - 1]][solucao[pos1]] + matrizAdj[solucao[pos1 + bloco1 - 1]][solucao[pos2 + bloco2]]) - 
              (matrizAdj[solucao[pos1 - 1]][solucao[pos1 + bloco1]] + matrizAdj[solucao[pos2 - 1]][solucao[pos2 + bloco2]]);

  for(size_t i = 0; i < bloco1; i++){
    aux = solucao[pos1];
    solucao.emplace(solucao.begin() + pos2, aux);
    solucao.erase(solucao.begin() + pos1);
    
  }

  for(size_t i = 0; i < bloco2; i++){
    aux = solucao[pos2 + i];
    solucao.erase(solucao.begin() + pos2 + i);
    solucao.emplace(solucao.begin() + pos1 + i, aux);
  }

  deltaCusto = custoFinal + custoInicial;

  tempoDoubleBridge += cpuTime() - tempoInicial;

  return custoDaSolucaoAnterior + deltaCusto;
}

// Untils
void printData() {
  cout << endl << "dimension: " << dimension << endl;
  for (size_t i = 1; i <= dimension; i++) {
    for (size_t j = 1; j <= dimension; j++) {
      cout << matrizAdj[i][j] << " ";
    }
    cout << endl;
  }
}

void printSolucao(vector<int> &solucao, int tamanhoArray){
  cout << endl << "Solucao: [ ";

  for(size_t i = 0; i < solucao.size(); i++){
    cout << solucao[i] << " ";
  }

  cout << "]" << endl;
}

void custoSolucao(int *custoTotal, vector<int> solucao, int tamanhoArray) {
  *custoTotal = 0;

  for(size_t i = 0; i < solucao.size()-1; i++){
    *custoTotal += matrizAdj[solucao[i]][solucao[i + 1]];
  }
}

bool compareByCost(const tInsercao &data1, const tInsercao &data2){
  return data1.custo < data2.custo;
}
