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
    //swap(solucao);

    //printSolucao(solucao, dimension);
    //custoSolucao(&custoTotal, solucao, dimension);

    //cout <<  "Custo: " << custoTotal << endl;

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

  // Mostra solução
  for (size_t i = 0; i < solucao.size(); i++){
    cout << solucao[i] << " ";
    //cout << solucaoInicial[i] << " ";
  }

}
void rvnd(vector<int> &solucao){

}
// Vizinhanças
void reinsertion(vector<int> &solucao){
  // Inicia variáveis
  vector<tInsercao> vizinhanca;

  mt19937 mt(42); // Gerador de números aleatórios

  int tamanhoVizinhanca;
  int verticeVizinho;
  int custoSolucaoAnterior;
  int custoSolucaoAtual;
  int custoRetiradaVizinho;
  tInsercao insercao;
  tInsercao vizinho;

  // Preenche vectors
  for (size_t i = 0; i < dimension; i++) {
    vizinho.pos = i;
    vizinho.vertice = solucao[i];
    vizinhanca.push_back(vizinho);
  }
  solucao.push_back(solucao[0]);

  // for (size_t i = 0; i <= dimension; i++) {
  //   cout << solucao[i] << " ";
  // }
  // cout << endl;

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
    if(vizinho.pos == 0 || vizinho.pos == solucao.size()-1){
      vizinho.pos = 0;
      custoRetiradaVizinho = matrizAdj[solucao[1]][solucao[solucao.size()-2]] - (matrizAdj[solucao[solucao.size()-2]][vizinho.vertice] + matrizAdj[vizinho.vertice][solucao[1]]);
    } else custoRetiradaVizinho = matrizAdj[solucao[vizinho.pos-1]][solucao[vizinho.pos+1]] - (matrizAdj[solucao[vizinho.pos-1]][vizinho.vertice] + matrizAdj[vizinho.vertice][solucao[vizinho.pos+1]]);

    insercao.custo = -custoRetiradaVizinho;
    insercao.pos = vizinho.pos;

    for (size_t i = 0; i < dimension; i++) {
      if (solucao[i] == vizinho.vertice || solucao[i+1] == vizinho.vertice) {
        continue;
      } else if (insercao.custo > ((matrizAdj[solucao[i]][vizinho.vertice] + matrizAdj[vizinho.vertice][solucao[i+1]]) - matrizAdj[solucao[i]][solucao[i+1]])){
        insercao.pos = i+1;
        insercao.custo = (matrizAdj[solucao[i]][vizinho.vertice] + matrizAdj[vizinho.vertice][solucao[i+1]]) - matrizAdj[solucao[i]][solucao[i+1]];
      }
    }

    // Testa se causou alguma melhora
    if((custoSolucaoAnterior + insercao.custo + custoRetiradaVizinho) < custoSolucaoAnterior){
      // Grava solução
      solucao.emplace(solucao.begin() + insercao.pos, vizinho.vertice);
      if(vizinho.pos == 0){
        solucao.erase(solucao.end());
        solucao.emplace(solucao.end(), vizinho.vertice);
      }
      else if(insercao.pos > vizinho.pos) solucao.erase(solucao.begin() + vizinho.pos);
      else solucao.erase(solucao.begin() + vizinho.pos + 1);

      //Grava novo custo
      custoSolucaoAnterior = custoSolucaoAnterior + insercao.custo + custoRetiradaVizinho;

      // Atualiza vizinhança se necessário
      if(vizinhanca.size() < dimension){
        vizinhanca.clear();
        for (size_t i = 0; i < dimension; i++) {
          vizinho.pos = i;
          vizinho.vertice = solucao[i+1];
          vizinhanca.push_back(vizinho);
        }
      }
    } else {
      vizinhanca.erase(vizinhanca.begin() + verticeVizinho);
    }

    // Se não tiver mais vizinhos quebra o while
    if (vizinhanca.size() == 0) {
      break;
    }
    //cout << vizinhanca.size() << " | " << insercao.custo << " | " << vizinho.vertice;
  }
}
// void swap(vector<int> &solucao){
//   //Inicia variáveis
//   vector<tSwap> listaSwaps;
//   vector<tSwap> listaSwapsRaiz;
//
//   mt19937 mt(42); // Gerador de números aleatórios
//
//   int tamanhoSolucao = solucao.size();
//   int listaSwapsSize;
//   int randomSwap;
//   int verticeSwap1;
//   int verticeSwap2;
//   int posSwap1;
//   int posSwap2;
//   int custoSolucaoSwap;
//   tSwap swap;
//
//   //Gera lista de possíveis swaps
//   for (size_t i = 0; i < tamanhoSolucao; i++) {
//     for (size_t j = 0; j < tamanhoSolucao; j++) {
//       swap.pos1 = i;
//       swap.vertice1 = solucao[i];
//       swap.pos2 = j;
//       swap.vertice2 = solucao[j];
//       listaSwapsRaiz.push_back(swap);
//     }
//   }
//
//   listaSwaps = listaSwapsRaiz;
//
//   //Custo da solução
//   custoSolucao(&custoSolucaoSwap, solucao, dimension);
//
//   //Encontra mínimo local
//   while (1){
//     listaSwapsSize = listaSwaps.size();
//     //Escolhe uma troca aleatória
//     uniform_int_distribution<int> linear_i(0, listaSwapsSize-1); // Distribuição linear de inteiros para escolher swap
//     randomSwap = linear_i(mt);
//
//     //Calcula custo do swap
//     verticeSwap1 = listaSwaps[randomSwap].vertice1;
//     verticeSwap2 = listaSwaps[randomSwap].vertice2;
//     posSwap1 = listaSwaps[randomSwap].pos1;
//     posSwap2 = listaSwaps[randomSwap].pos2;
//     posAntSwap1 = ;
//     posAntSwap2 = ;
//     posDepSwap1 = ;
//     posDepSwap2 = ;
//
//     if(posSwap1+1 != posSwap2 || posSwap1-1 != posSwap2) {
//       listaSwaps[randomSwap].custo =
//         ( (matrizAdj[solucao[posSwap1-1]][verticeSwap2] + matrizAdj[verticeSwap2][solucao[posSwap1+1]]) - (matrizAdj[solucao[posSwap1-1]][verticeSwap1] + matrizAdj[verticeSwap1][solucao[posSwap1+1]]) )+
//         ( (matrizAdj[solucao[posSwap2-1]][verticeSwap1] + matrizAdj[verticeSwap1][solucao[posSwap2+1]]) - (matrizAdj[solucao[posSwap2-1]][verticeSwap2] + matrizAdj[verticeSwap2][solucao[posSwap2+1]]) );
//     } else {
//       if(posSwap1 < posSwap2) {
//         listaSwaps[randomSwap].custo =
//           ( (matrizAdj[solucao[posSwap1-1]][verticeSwap1] + matrizAdj[verticeSwap1][verticeSwap2] + matrizAdj[verticeSwap2][solucao[posSwap2+1]]) )-
//           ( (matrizAdj[solucao[posSwap1-1]][verticeSwap2] + matrizAdj[verticeSwap2][verticeSwap1] + matrizAdj[verticeSwap1][solucao[posSwap2+1]]) );
//       } else {
//         listaSwaps[randomSwap].custo =
//           ( (matrizAdj[solucao[posSwap2-1]][verticeSwap2] + matrizAdj[verticeSwap2][verticeSwap1] + matrizAdj[verticeSwap1][solucao[posSwap1+1]]) )-
//           ( (matrizAdj[solucao[posSwap2-1]][verticeSwap1] + matrizAdj[verticeSwap1][verticeSwap2] + matrizAdj[verticeSwap2][solucao[posSwap1+1]]) );
//       }
//     }
//
//     //Testa se ouve melhora no custo da solução
//     if ( (custoSolucaoSwap + listaSwaps[randomSwap].custo) < custoSolucaoSwap) {
//       //Grava solução
//       solucao[posSwap1] = verticeSwap2;
//       solucao[posSwap2] = verticeSwap1;
//
//       //Grava novo custo
//       custoSolucaoSwap = custoSolucaoSwap + listaSwaps[randomSwap].custo;
//
//       //Atualiza vizinhanca cas seja necessário
//       if(listaSwapsSize < dimension){
//         listaSwaps = listaSwapsRaiz;
//       }
//     } else {
//       listaSwaps.erase(listaSwaps.begin() + randomSwap);
//     }
//
//     //cout << endl << listaSwaps[randomSwap].pos1 << " " << listaSwaps[randomSwap].pos2 << " " << listaSwaps[randomSwap].custo << endl;
//     cout << endl << listaSwaps.size() << endl;
//     // Se não tiver mais vizinhos quebra o while
//     if (listaSwaps.size() == 0){
//       break;
//     }
//   }
// }
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

  for(size_t i = 0; i < dimension; i++){
    cout << solucao[i] << " ";
  }

  cout << "]" << endl;
}
void custoSolucao(int *custoTotal, vector<int> solucao, int tamanhoArray) {
  *custoTotal = 0;

  for(size_t i = 0; i < tamanhoArray-1; i++){
    *custoTotal += matrizAdj[solucao[i]][solucao[i + 1]];
    //cout << i << " - " << matrizAdj[array[i]][array[i + 1]] << " - " << array[i+1] << endl;
  }
  *custoTotal += matrizAdj[solucao[tamanhoArray-1]][solucao[0]];
}
bool compareByCost(const tInsercao &data1, const tInsercao &data2){
  return data1.custo < data2.custo;
}
