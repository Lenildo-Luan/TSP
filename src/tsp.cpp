#include "readData.h"
#include <random>
#include <bits/stdc++.h>
#include <fstream>
#include <iostream>
#include <cmath>
#include <algorithm>

using namespace std;

typedef struct{
  int pos;
  int vertice;
  int custo;
} tInsercao;

typedef struct{
  int pos1;
  int vertice1;
  int pos2;
  int vertice2;
  int custo;
} tSwap;

// Pega da instâncias
double ** matrizAdj; // matriz de adjacencia
int dimension; // quantidade total de vertices

// Untils
void printData(); // Mostra matriz de adjacencia
void printSolucao(vector<int> &solucao, int tamanhoArray); // Mostra a solução inicial gerada pel algorítimo escolhido
void custoSolucao(int *custoTotal, vector<int> solucao, int tamanhoArray); // Mostra o custo da solução gerada
bool compareByCost(const tInsercao &data1, const tInsercao &data2);

//GILS-RVND
void construtivo(vector<int> &solucao, int coleta, int deposito);
void rvnd(vector<int> &solucao);

// Vizinhanças
void reinsertion(vector<int> &solucao);
void swap(vector<int> &solucao);

//MAIN
int main(int argc, char** argv){
    readData(argc, argv, &dimension, &matrizAdj);
    //printData();

    vector<int> solucao;
    int custoTotal, coleta, deposito;

    coleta = 1;
    deposito = 1;

    construtivo(solucao, coleta, deposito);
    swap(solucao);
    reinsertion(solucao);
   
    printSolucao(solucao, dimension);
    custoSolucao(&custoTotal, solucao, dimension);

    cout <<  "Custo: " << custoTotal << endl;

    return 0;
}

//GILS-RVND
void construtivo(vector<int> &solucao, int coleta, int deposito){
  // Inicia variáveis
  vector<int> verticesList; // Lista de vertices faltando
  vector<tInsercao> bestVerticesList; // Lista dos X melhores verticesList

  mt19937 mt(42); // Gerador de números aleatórios
  uniform_real_distribution<float> linear_f(0.0, 0.5); // Distribuição linear de reais para gerar alpha

  float alpha = 0.0;
  int rBest;
  int randomVertice1, randomVertice2, randomVertice3;
  int tamanhoSolucao;
  int verticesRestantes;
  int distanciaVertice;
  tInsercao insercao;
  //Adiciona coleta ao vector
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
  }

  // Adiciona deposito ao vector
  solucao.push_back(deposito);

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

    // Gera Alfa
    alpha = linear_f(mt);

    // Adiciona novo vertice à solução conforme alpha
    uniform_int_distribution<int> linear_i_alpha(0, int((bestVerticesList.size()-1)*alpha));
    rBest = linear_i_alpha(mt);
    solucao.emplace(solucao.begin() + bestVerticesList[rBest].pos, bestVerticesList[rBest].vertice);
    
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

  // // Mostra solução
  // for (size_t i = 0; i < solucao.size(); i++){
  //   cout << solucao[i] << " ";
  //   //cout << solucaoInicial[i] << " ";
  // }

}
void rvnd(vector<int> &solucao){

}

// Vizinhanças
void reinsertion(vector<int> &solucao){
  // Inicia variáveis
  vector<tInsercao> vizinhanca;

  mt19937 mt(42); // Gerador de números aleatórios

  int tamanhoVizinhanca;
  int tamanhoSolucao = solucao.size();
  int verticeVizinho;
  int custoSolucaoAnterior;
  int custoSolucaoAtual;
  int custoRetiradaVizinho;
  tInsercao insercao;
  tInsercao vizinho;
  tInsercao vertice;

  // Preenche vectors
  for (size_t i = 1; i < tamanhoSolucao-1; i++) {
    vizinho.pos = i;
    vizinho.vertice = solucao[i];
    vizinhanca.push_back(vizinho);
  }

  //Verifica custo da solução
  custoSolucao(&custoSolucaoAnterior, solucao, dimension);
  custoSolucaoAtual = custoSolucaoAnterior;

  // Busca ótimo local
  while(1){
    // Seleciona vizinho aleatório
    uniform_int_distribution<int> linear_i(0, vizinhanca.size()-1); // Distribuição linear de inteiros para escolher vizinho
    verticeVizinho = linear_i(mt);
    vizinho = vizinhanca[verticeVizinho];

    // Encontra ótimo local do vizinho na vizinhança
      // Calcula custo da retirada
    custoRetiradaVizinho = matrizAdj[solucao[vizinho.pos-1]][solucao[vizinho.pos+1]] - (matrizAdj[solucao[vizinho.pos-1]][vizinho.vertice] + matrizAdj[vizinho.vertice][solucao[vizinho.pos+1]]);

      // Salva custo e a posição da inserção
    insercao.custo = (matrizAdj[solucao[vizinho.pos-1]][vizinho.vertice] + matrizAdj[vizinho.vertice][solucao[vizinho.pos+1]]) - matrizAdj[solucao[vizinho.pos-1]][solucao[vizinho.pos+1]];
    insercao.pos = vizinho.pos;

    for (size_t i = 0; i < tamanhoSolucao-1; i++) {
      if (solucao[i] == vizinho.vertice || solucao[i+1] == vizinho.vertice) {
        continue;
      } else if (insercao.custo > (matrizAdj[solucao[i]][vizinho.vertice] + matrizAdj[vizinho.vertice][solucao[i+1]]) - (matrizAdj[solucao[i]][solucao[i+1]])){
        insercao.pos = i+1;
        insercao.custo = (matrizAdj[solucao[i]][vizinho.vertice] + matrizAdj[vizinho.vertice][solucao[i+1]]) - matrizAdj[solucao[i]][solucao[i+1]];
      }
    }
    
    // Testa se causou alguma melhora
    if((custoSolucaoAnterior + insercao.custo + custoRetiradaVizinho) < custoSolucaoAnterior){
      solucao.emplace(solucao.begin() + insercao.pos, vizinho.vertice);

      if(insercao.pos > vizinho.pos) solucao.erase(solucao.begin() + vizinho.pos);
      else solucao.erase(solucao.begin() + vizinho.pos + 1);

      //Grava novo custo
      custoSolucaoAnterior = custoSolucaoAnterior + insercao.custo + custoRetiradaVizinho;

      // Atualiza vizinhança se necessário
      vizinhanca.clear();
      
      for (size_t i = 1; i < tamanhoSolucao-1; i++) {
        vizinho.pos = i;
        vizinho.vertice = solucao[i];
        vizinhanca.push_back(vizinho);
      } 
      
    } else {
      vizinhanca.erase(vizinhanca.begin() + verticeVizinho);
    }

    // Se não tiver mais vizinhos quebra o while
    if (vizinhanca.size() == 0) {
      break;
    }
  }
}

void swap(vector<int> &solucao){
  //Inicia variáveis
  vector<tSwap> listaSwaps;
  vector<tSwap> listaSwapsRaiz;

  mt19937 mt(42); // Gerador de números aleatórios

  int tamanhoSolucao = solucao.size();
  int custoSolucaoSwap;
  int custoSwap;
  int randomSwap;
  tSwap swap;

  //Gera lista de possíveis swaps
  for (size_t i = 1; i < tamanhoSolucao - 1; i++) {
    for (size_t j = 1; j < tamanhoSolucao - 1; j++) {
      if(j == i || j == i - 1 || j == i + 1) continue;
      swap.pos1 = i;
      swap.vertice1 = solucao[i];
      swap.pos2 = j;
      swap.vertice2 = solucao[j];
      listaSwapsRaiz.push_back(swap);
    }
  }

  listaSwaps = listaSwapsRaiz;

  //Custo da solução
  custoSolucao(&custoSolucaoSwap, solucao, dimension);

  //Encontra mínimo local
  while (1){
    // Escolhe swar aleatório
    uniform_int_distribution<int> linear_i(0, listaSwaps.size()-1);
    randomSwap = linear_i(mt);
    swap = listaSwaps[randomSwap];

    //Calcula custo do swap
    //cout << "Teste - 1" << endl;
    custoSwap = 
      ( //Troca vertice 1 para o 2
        (matrizAdj[solucao[swap.pos1 - 1]][swap.vertice2] + matrizAdj[swap.vertice2][solucao[swap.pos1 + 1]]) -
        (matrizAdj[solucao[swap.pos1 - 1]][swap.vertice1] + matrizAdj[swap.vertice1][solucao[swap.pos1 + 1]]) 
      ) + ( //Troca vertice 2 para o 1
        (matrizAdj[solucao[swap.pos2 - 1]][swap.vertice1] + matrizAdj[swap.vertice1][solucao[swap.pos2 + 1]]) -
        (matrizAdj[solucao[swap.pos2 - 1]][swap.vertice2] + matrizAdj[swap.vertice2][solucao[swap.pos2 + 1]])
      );
    //cout << "Teste -- 2" << endl;
    // cout << solucao[swap.pos1 - 1] << " | " << solucao[swap.pos1 + 1] << " | " << solucao[swap.pos1 - 1] << swap.vertice << swap. "Custo calculo: " << (matrizAdj[solucao[swap.pos1 - 1]][solucao[swap.vertice1]] + matrizAdj[solucao[swap.vertice1]][solucao[swap.pos1 + 1]]) << endl;
    // cout << "Custo Troca: " << custoSolucaoSwap + custoSwap << " | Custo anterior: " << custoSolucaoSwap << endl;
    //Testa se houve melhora 
    if(custoSolucaoSwap + custoSwap < custoSolucaoSwap){ //Se houve
      //cout << "Teste --- 3" << endl;
      //Adiciona primeiro vertice
      solucao.emplace(solucao.begin() + swap.pos1, swap.vertice2);
      solucao.erase(solucao.begin() + swap.pos1 + 1);
      //Adiciona segundo vertice
      solucao.emplace(solucao.begin() + swap.pos2, swap.vertice1);
      solucao.erase(solucao.begin() + swap.pos2 + 1);
      //cout << "Teste ---- 4" << endl;
      //Salva novo custo
      custoSolucaoSwap = custoSolucaoSwap + custoSwap;

      //Cria nova lista de swaps possíveis
      listaSwaps.clear();
      for (size_t i = 1; i < tamanhoSolucao - 1; i++) {
        for (size_t j = 1; j < tamanhoSolucao - 1; j++) {
          if(j == i || j == i - 1 || j == i + 1) continue;
          swap.pos1 = i;
          swap.vertice1 = solucao[i];
          swap.pos2 = j;
          swap.vertice2 = solucao[j];
          listaSwaps.push_back(swap);
        }
      }
    } else { //Caso não
      //Apaga swap
      //cout << "Teste ----- 5 | " << randomSwap << " | " << listaSwaps[randomSwap].pos1 << endl;
      listaSwaps.erase(listaSwaps.begin() + randomSwap);
      //cout << "Teste ------ 6" << endl;
    }

    //Caso não tenha mais swaps possíveis, sai do loop
    if(listaSwaps.size() == 0) break;
  }
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
