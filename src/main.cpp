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
vector< vector< vector< vector< int > > > > reinsertionMatrix;

// Untils
void printData(); // Mostra matriz de adjacencia
void printSolucao(vector<int> &solucao, int tamanhoArray); // Mostra a solução inicial gerada pel algorítimo escolhido
void custoSolucao(int *custoTotal, vector<int> solucao, int tamanhoArray); // Mostra o custo da solução gerada
bool compareByCost(const tInsercao &data1, const tInsercao &data2);

//Preproces
void preProcessReinsertion();

//GILS-RVND
int construtivo(vector<int> &sol, int depot, float alpha);
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

  preProcessReinsertion();

  vector<int> sol;
  int costSol;
  int maxIls = (dimension > 150) ? dimension/2 : dimension; 

  costSol = gilsRvnd(sol, 50, maxIls);
  cout << "Custo: " << costSol << endl;

  return 0;
}

//Preproces
void preProcessReinsertion(){
  vector< vector< vector< vector< int > > > > reinsertionMatrix;
  vector< vector< vector< int > > > de;
  for(int i = 1; i < 48; i++){
    for(int j = 1; j < 48; j++){
      for(int k = 1; k < 48; k++){
        for(int l = 1; l < 48; l++){
          reinsertionMatrix[i][j][k][l] = (matrizAdj[i][k] + matrizAdj[l][j]) - (matrizAdj[i][j]);
        }
      }
    }
  }
}

//GILS-RVND
int construtivo(vector<int> &sol, int depot, float alpha){
  // Inicia variáveis
  vector<int> verticesList; // Lista de vertices faltando
  vector<tInsercao> bestVerticesList; // Lista dos X melhores verticesList

  random_device rd;
  mt19937 mt(rd()); // Gerador de números aleatórios

  int rBest;
  int randomVertice;
  int sizeSol;
  int sizeVerticesList;
  int costSol = 0;
  tInsercao insertion;

  //Adiciona coleta ao vector
  sol.clear();

  sol.push_back(depot);

  // Gera lista de vertices faltantes
  for (size_t i = 1; i <= dimension; i++) {
    if(i == depot) continue;
    verticesList.push_back(i);
  }

  // Escolhe três vertices de forma aleatória
  for (size_t i = 0; i < 3; i++) {
    uniform_int_distribution<int> linear_i(0, verticesList.size()-1); // Distribuição linear de inteiros para escolher vertice inicial
    randomVertice = linear_i(mt);

    sol.push_back(verticesList[randomVertice]);
    verticesList.erase(verticesList.begin() + randomVertice);

    costSol += matrizAdj[sol[i]][sol[i+1]];
  }

  // Adiciona deposito ao vector
  sol.push_back(depot);

  costSol += matrizAdj[sol[3]][sol[4]];

  // Gera solução
  while(1) {
    sizeSol = sol.size();
    sizeVerticesList = verticesList.size();

    // Gera lista de custo de vertices
    for (size_t i = 0; i < sizeVerticesList; i++) { // Itera sobre vértices restantes
      for (size_t j = 1; j < sizeSol; j++) { // Itera sobre a solução
        insertion.vertice = verticesList[i];
        insertion.pos = j;
        insertion.custo = (matrizAdj[sol[j-1]][verticesList[i]] + matrizAdj[verticesList[i]][sol[j]]) - matrizAdj[sol[j-1]][sol[j]];
        bestVerticesList.push_back(insertion);
      }
    }

    //Arruma lista pelo valor do custo
    sort(bestVerticesList.begin(), bestVerticesList.end(), compareByCost);

    // Adiciona novo vertice à solução conforme alpha
    uniform_int_distribution<int> linear_i_alpha(0, int((bestVerticesList.size()-1)*alpha));
    rBest = linear_i_alpha(mt);
    sol.emplace(sol.begin() + bestVerticesList[rBest].pos, bestVerticesList[rBest].vertice);

    //Calcula custo
    costSol += bestVerticesList[rBest].custo;

    // Reseta Vectors
    for (size_t i = 0; i < sizeVerticesList; i++) {
      if(bestVerticesList[rBest].vertice == verticesList[i]) {
        verticesList.erase(verticesList.begin() + i);
        break;
      }
    }
    bestVerticesList.clear();

    // Se não tiverem mais candidatos o loop acaba
    if(sizeVerticesList == 1) {
      break;
    }
  }

  return costSol;
}

int rvnd(vector<int> &currentSol, int currentCost){
  random_device rd;
  mt19937 mt(rd());

  vector<int> currentNeighborhood;
  vector<int> neighborhood = {1, 2, 3, 4, 5};
  vector<int> sol;

  int randomNeighbor = 0;
  int random = 0;
  int newCost = 0;
  int cost = currentCost;

  sol = currentSol;
  currentNeighborhood = neighborhood;

  while(1){
    uniform_int_distribution<int> linear_i(0, currentNeighborhood.size() - 1);
    randomNeighbor = currentNeighborhood[linear_i(mt)];

    switch(randomNeighbor){
      case 1:
        newCost = reinsertion(sol, 1, currentCost);
        break;
      case 2:
        newCost = reinsertion(sol, 2, currentCost);
        break;
      case 3:
        newCost = reinsertion(sol, 3, currentCost);
        break;
      case 4:
        newCost = swap(sol, currentCost);
        break;
      case 5:
        newCost = twoOptN(sol, currentCost);
        break;
      default:
        break;
    }

    if(newCost < currentCost){
      currentCost = newCost;

      currentSol.clear();
      currentSol = sol;

      currentNeighborhood.clear();
      currentNeighborhood = neighborhood;

    } else {
      currentNeighborhood.erase(currentNeighborhood.begin() + random);
    }

    if(currentNeighborhood.empty()) break;
  }

  return currentCost;
}

int gilsRvnd(vector<int> &bestBestSol, int Imax, int Iils){
  //Declara Variáveis
  random_device rd;
  mt19937 mt(rd());
  uniform_real_distribution<float> linear_f(0.0, 0.5); // Distribuição linear de reais para gerar alpha

  vector<int> currentSol;
  vector<int> bestSol;

  int bestBestCost = INT32_MAX;
  int currentCost = 0;
  int bestCost = 0;
  int depot = 1;
  int interILS;
  int sizeSol;
  float alpha = 0.0;

  //Busca Melhor Solução
  for (size_t i = 0; i < Imax; i++) {
    //Gera Alfa e o interador de ILS
    alpha = linear_f(mt);
    interILS = 0;

    //Calcula solução parcial com o método construtivo
    currentSol.clear();
    currentCost = construtivo(currentSol, depot, alpha);
    sizeSol = currentSol.size();

    //Registra a solução parcial como melhor solução
    bestCost = currentCost;
    bestSol = currentSol;

    //Busca o melhor ótimo local a partir da solução encontrada no construtivo
    while(interILS < Iils){
      //Busca melhor ótimo local da solução parcial
      currentCost = rvnd(currentSol, currentCost);

      //Testa se houve melhora
      if(currentCost < bestCost){
        //Registra a solução parcial como melhor solução
        bestCost = currentCost;
        bestSol = currentSol;

        //Zera o iterador
        interILS = 0;
      }
      
      //Pertuba a solução
      currentSol = bestSol;
      currentCost = bestCost;
      currentCost = doubleBridge(currentSol, currentCost);
     
      //Soma o interador
      interILS++;
    }

    //Testa se houve melhora
    if(bestCost < bestBestCost){
      bestBestCost = bestCost;
      bestBestSol = bestSol;
    }
  }

  return bestBestCost;
}

// Vizinhanças
int reinsertion(vector<int> &sol, int subseqSize, int cost){
  // Inicia variáveis
  vector<int> aux;

  int deltaCost = 0, retreatCost = 0, insertionCost = 0, sizeSol = sol.size();
  int iterSize = sizeSol - subseqSize;
  bool flag = false;
  tReinsercao insertion;

  //Procura local de melhor reinserção
  for(size_t i = 1; i < iterSize; i++){ 
    retreatCost = matrizAdj[sol[i-1]][sol[i+subseqSize]] - (matrizAdj[sol[i-1]][sol[i]] + matrizAdj[sol[i+subseqSize-1]][sol[i+subseqSize]]);

    for(size_t y = 0; y < i - 1; y++){
      //insertionCost = (matrizAdj[sol[y]][sol[i]] + matrizAdj[sol[i+subseqSize-1]][sol[y+1]]) - matrizAdj[sol[y]][sol[y+1]];
      insertionCost = reinsertionMatrix[y][y+1][i][i+subseqSize-1];

      if((retreatCost + insertionCost) < deltaCost){
        flag = true;

        deltaCost = insertionCost + retreatCost;
        insertion.posVertice = i;
        insertion.posInsercao = y+1;
        insertion.vertice = sol[i];
      }
    }

    for(size_t y = i + subseqSize + 1; y < sizeSol - 1; y++){
      insertionCost = (matrizAdj[sol[y]][sol[i]] + matrizAdj[sol[i+subseqSize-1]][sol[y+1]]) - matrizAdj[sol[y]][sol[y+1]];

      if((retreatCost + insertionCost) < deltaCost){
        flag = true;

        deltaCost = insertionCost + retreatCost;
        insertion.posVertice = i;
        insertion.posInsercao = y+1;
        insertion.vertice = sol[i];
      }
    }
  }

  //Aplica reinserção
  if(flag){
    cost += deltaCost;

    for(size_t i = 0; i < subseqSize; i++){
      aux.push_back(sol[insertion.posVertice]);
      sol.erase(sol.begin() + insertion.posVertice);
    }

    if(insertion.posInsercao > insertion.posVertice) insertion.posInsercao -= subseqSize; 

    sol.insert(sol.begin() + insertion.posInsercao, aux.begin(), aux.begin() + aux.size());
  } 

  return cost;
}

int swap(vector<int> &sol, int cost){
  //Inicia variáveis
  int deltaCost = 0, retreatCost = 0, insertionCost = 0;
  int sizeSol = sol.size();
  bool flag = false;
  tSwap swap;

  //Aplica reinserção
  for(size_t i = 1; i < sizeSol; i++){
    for(size_t y = i+2; y < sizeSol - 1; y++){
      retreatCost =   matrizAdj[sol[i-1]][sol[i]] + matrizAdj[sol[i]][sol[i+1]] + 
                      matrizAdj[sol[y-1]][sol[y]] + matrizAdj[sol[y]][sol[y+1]];

      insertionCost = matrizAdj[sol[i-1]][sol[y]] + matrizAdj[sol[y]][sol[i+1]] +
                      matrizAdj[sol[y-1]][sol[i]] + matrizAdj[sol[i]][sol[y+1]];
          
      if((insertionCost - retreatCost) < deltaCost){
        flag = true;

        deltaCost = insertionCost - retreatCost;
        swap.pos1 = i;
        swap.vertice1 = sol[i];
        swap.pos2 = y;
        swap.vertice2 = sol[y];
      }
    }
  }

  if(flag){
    cost += deltaCost;

    sol.erase(sol.begin() + swap.pos2);
    sol.emplace(sol.begin() + swap.pos2, swap.vertice1);

    sol.erase(sol.begin() + swap.pos1);
    sol.emplace(sol.begin() + swap.pos1, swap.vertice2);
  }

  return cost;
}

int twoOptN(vector<int> &sol, int cost){
  //Inicia variáveis
  int deltaCost = 0, retreatCost = 0, insertionCost = 0;
  int verticeEmplace = 0;
  int sizeSol = sol.size();
  int sizeSwap;
  bool flag = false;
  tSwap swap;

  for(size_t i = 0; i < sizeSol; i++){
    for(size_t y = i + 2; y < sizeSol; y++){
      retreatCost = matrizAdj[sol[i]][sol[i+1]] + matrizAdj[sol[y-1]][sol[y]];
      insertionCost = matrizAdj[sol[i]][sol[y-1]] + matrizAdj[sol[i+1]][sol[y]];

      if((insertionCost - retreatCost) < deltaCost){
        flag = true;

        deltaCost = insertionCost - retreatCost;
        swap.pos1 = i + 1;
        swap.vertice1 = sol[i];
        swap.pos2 = y - 1;
        swap.vertice2 = sol[y];
      }
    }
  }

  if(flag){
    cost += deltaCost;
    sizeSwap = swap.pos2 - swap.pos1;

    for(size_t i = 0; i < sizeSwap; i++){
      verticeEmplace = sol[swap.pos2];
      sol.erase(sol.begin() + swap.pos2);
      sol.emplace(sol.begin() + swap.pos1 + i, verticeEmplace);
    }
  }

  return cost;
}

//Pertubações
int doubleBridge(vector<int> &sol, int cost){
  random_device rd;
  mt19937_64 mt(rd());
  uniform_int_distribution<int> linear_bme20(2, dimension / 3);
  uniform_int_distribution<int> linear_bma20(2, dimension / 10);

  vector<int> subSeq1, subSeq2;

  int sizeSubSeq1, sizeSubSeq2;
  int initSubSeq1, initSubSeq2;
  int initialCost, finalCost, deltaCost = 0;

  sizeSubSeq1 = linear_bme20(mt);
  sizeSubSeq2 = linear_bme20(mt);
  
  uniform_int_distribution<int> linear_p1(1, sol.size() - sizeSubSeq1 - 1);
  uniform_int_distribution<int> linear_p2(1, sol.size() - sizeSubSeq2 - 1);

  while(1){
    initSubSeq1 = linear_p1(mt);
    initSubSeq2 = linear_p2(mt);

    if((initSubSeq2 <= (initSubSeq1 - sizeSubSeq2) && initSubSeq1 > sizeSubSeq2) || (initSubSeq2 >= (initSubSeq1 + sizeSubSeq1) && ((sol.size() - 1) - initSubSeq1 - (sizeSubSeq1 - 1)) > sizeSubSeq2)) break;
  }

  for(size_t i = 0; i < sizeSubSeq1; i++){
    subSeq1.push_back(sol[initSubSeq1 + i]);
  }

  for(size_t i = 0; i < sizeSubSeq2; i++){
    subSeq2.push_back(sol[initSubSeq2 + i]);
  }

  if(initSubSeq1 > initSubSeq2){
    if((initSubSeq1 - sizeSubSeq2) == initSubSeq2){
      deltaCost = (matrizAdj[sol[initSubSeq2 - 1]][sol[initSubSeq1]] + matrizAdj[sol[initSubSeq1 + sizeSubSeq1 - 1]][sol[initSubSeq2]] + matrizAdj[sol[initSubSeq2 + sizeSubSeq2 - 1]][sol[initSubSeq1 + sizeSubSeq1]]) -
                  (matrizAdj[sol[initSubSeq2 - 1]][sol[initSubSeq2]] + matrizAdj[sol[initSubSeq2 + sizeSubSeq2 - 1]][sol[initSubSeq1]] + matrizAdj[sol[initSubSeq1 + sizeSubSeq1 - 1]][sol[initSubSeq1 + sizeSubSeq1]]);
    } else {
      initialCost = (matrizAdj[sol[initSubSeq1 - 1]][sol[initSubSeq1 + sizeSubSeq1]] + matrizAdj[sol[initSubSeq2 - 1]][sol[initSubSeq2 + sizeSubSeq2]]) - 
                    (matrizAdj[sol[initSubSeq1 - 1]][sol[initSubSeq1]] + matrizAdj[sol[initSubSeq1 + sizeSubSeq1 - 1]][sol[initSubSeq1 + sizeSubSeq1]] + 
                      matrizAdj[sol[initSubSeq2 - 1]][sol[initSubSeq2]] + matrizAdj[sol[initSubSeq2 + sizeSubSeq2 - 1]][sol[initSubSeq2 + sizeSubSeq2]]);

      finalCost =(matrizAdj[sol[initSubSeq1 - 1]][sol[initSubSeq2]] + matrizAdj[sol[initSubSeq2 + sizeSubSeq2 - 1]][sol[initSubSeq1 + sizeSubSeq1]] + 
                  matrizAdj[sol[initSubSeq2 - 1]][sol[initSubSeq1]] + matrizAdj[sol[initSubSeq1 + sizeSubSeq1 - 1]][sol[initSubSeq2 + sizeSubSeq2]]) - 
                  (matrizAdj[sol[initSubSeq1 - 1]][sol[initSubSeq1 + sizeSubSeq1]] + matrizAdj[sol[initSubSeq2 - 1]][sol[initSubSeq2 + sizeSubSeq2]]);

      deltaCost = finalCost + initialCost;
    }

    sol.erase(sol.begin() + initSubSeq2, sol.begin() + initSubSeq2 + (sizeSubSeq2));
    sol.insert(sol.begin() + initSubSeq2, subSeq1.begin(), subSeq1.begin() + subSeq1.size());

    sol.erase(sol.begin() + initSubSeq1 + (sizeSubSeq1 - sizeSubSeq2), sol.begin() + initSubSeq1 + (sizeSubSeq1 - sizeSubSeq2) + (sizeSubSeq1));
    sol.insert(sol.begin() + initSubSeq1 + (sizeSubSeq1 - sizeSubSeq2), subSeq2.begin(), subSeq2.begin() + subSeq2.size());

  } else {
    if(initSubSeq1 + sizeSubSeq1 == initSubSeq2){
      deltaCost = (matrizAdj[sol[initSubSeq1 - 1]][sol[initSubSeq2]] + matrizAdj[sol[initSubSeq2 + sizeSubSeq2 - 1]][sol[initSubSeq1]] + matrizAdj[sol[initSubSeq1 + sizeSubSeq1 - 1]][sol[initSubSeq2 + sizeSubSeq2]]) -
                  (matrizAdj[sol[initSubSeq1 - 1]][sol[initSubSeq1]] + matrizAdj[sol[initSubSeq1 + sizeSubSeq1 - 1]][sol[initSubSeq2]] + matrizAdj[sol[initSubSeq2 + sizeSubSeq2 - 1]][sol[initSubSeq2 + sizeSubSeq2]]);
    
    } else {
      initialCost = (matrizAdj[sol[initSubSeq1 - 1]][sol[initSubSeq1 + sizeSubSeq1]] + matrizAdj[sol[initSubSeq2 - 1]][sol[initSubSeq2 + sizeSubSeq2]]) - 
                    (matrizAdj[sol[initSubSeq1 - 1]][sol[initSubSeq1]] + matrizAdj[sol[initSubSeq1 + sizeSubSeq1 - 1]][sol[initSubSeq1 + sizeSubSeq1]] + 
                      matrizAdj[sol[initSubSeq2 - 1]][sol[initSubSeq2]] + matrizAdj[sol[initSubSeq2 + sizeSubSeq2 - 1]][sol[initSubSeq2 + sizeSubSeq2]]);

      finalCost =(matrizAdj[sol[initSubSeq1 - 1]][sol[initSubSeq2]] + matrizAdj[sol[initSubSeq2 + sizeSubSeq2 - 1]][sol[initSubSeq1 + sizeSubSeq1]] + 
                  matrizAdj[sol[initSubSeq2 - 1]][sol[initSubSeq1]] + matrizAdj[sol[initSubSeq1 + sizeSubSeq1 - 1]][sol[initSubSeq2 + sizeSubSeq2]]) - 
                  (matrizAdj[sol[initSubSeq1 - 1]][sol[initSubSeq1 + sizeSubSeq1]] + matrizAdj[sol[initSubSeq2 - 1]][sol[initSubSeq2 + sizeSubSeq2]]);

      deltaCost = finalCost + initialCost;
    }

    sol.erase(sol.begin() + initSubSeq1, sol.begin() + initSubSeq1 + (sizeSubSeq1));
    sol.insert(sol.begin() + initSubSeq1, subSeq2.begin(), subSeq2.begin() + subSeq2.size());

    sol.erase(sol.begin() + initSubSeq2 + (sizeSubSeq2 - sizeSubSeq1), sol.begin() + initSubSeq2 + (sizeSubSeq2 - sizeSubSeq1) + (sizeSubSeq2));
    sol.insert(sol.begin() + initSubSeq2 + (sizeSubSeq2 - sizeSubSeq1), subSeq1.begin(), subSeq1.begin() + subSeq1.size());
  }

  return cost + deltaCost;
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

void printSolucao(vector<int> &sol, int arraySize){
  cout << endl << "Solucao: [ ";

  for(size_t i = 0; i < sol.size(); i++){
    cout << sol[i] << " ";
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
