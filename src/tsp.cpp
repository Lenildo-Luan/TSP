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
    int custoSolucao, coleta, deposito;

    // coleta = 1;
    // deposito = 1;
    //
    // random_device rd;
    // mt19937 mt(rd());
    // uniform_real_distribution<float> linear_f(0.0, 0.5); // Distribuição linear de reais para gerar alpha
    //
    // float alpha = 0.0;
    // alpha = linear_f(mt);
    //
    // custoSolucao = construtivo(solucao, coleta, deposito, alpha);
    // printSolucao(solucao, dimension);
    // cout <<  "Custo: " << custoSolucao << endl;

    //reinsertion(solucao, 1, custoTotal);
    //swap(solucao, custoTotal);
    //twoOptN(solucao, custoTotal);
    //custoSolucao = rvnd(solucao, custoSolucao);
    //custoSolucao = doubleBridge(solucao, custoSolucao);

    custoSolucao = gilsRvnd(solucao, 50, 4*dimension);

    printSolucao(solucao, dimension);
    cout <<  "Custo: " << custoSolucao << endl;

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
  vector<int> vizinhanca = {1, 2, 3};
  vector<int> solucaoTeste;

  random_device rd;
  mt19937 mt(rd());

  int solucaoSize = solucao.size();
  int vizinhoRandom = 0;
  int random = 0;
  int novoCusto = 0;
  int custoAnterior = custoDaSolucaoAnterior;

  for(size_t i = 0; i < solucaoSize; i++){
    solucaoTeste.push_back(solucao[i]);
  }

  while(1){
    uniform_int_distribution<int> linear_i(0, vizinhanca.size() - 1);
    random = linear_i(mt);
    vizinhoRandom = vizinhanca[random];

    switch(vizinhoRandom){
      case 1:
        novoCusto = reinsertion(solucaoTeste, 1, custoAnterior);
        break;
      case 2:
        novoCusto = swap(solucaoTeste, custoAnterior);
        break;
      case 3:
        novoCusto = twoOptN(solucaoTeste, custoAnterior);
        break;
      default:
        break;
    }

    if(novoCusto < custoAnterior){
      custoAnterior = novoCusto;

      solucao.clear();
      for(size_t i = 0; i < solucaoSize; i++){
        solucao.push_back(solucaoTeste[i]);
      }

      vizinhanca.clear();
      for(size_t i = 0; i < 3; i++){
        vizinhanca.push_back(i);
      }
    } else {
      vizinhanca.erase(vizinhanca.begin() + random);
    }

    if(vizinhanca.size() == 0) break;
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
  float alpha = 0.0;

  //Busca Melhor Solução
  for (size_t i = 0; i < Imax; i++) {
    // Gera Alfa e o interador de ILS
    alpha = linear_f(mt);
    interILS = 0;

    //Calcula solução parcial com o método construtivo
    solucaoParcial.clear();
    custoParcial = construtivo(solucaoParcial, coleta, deposito, alpha);

    //Registra a solução parcial como melhor solução
    custoMelhor = custoParcial;
    solucaoMelhor.clear();
    for (size_t i = 0; i < solucaoParcial.size(); i++){
      solucaoMelhor.push_back(solucaoParcial[i]);
    }

    //Busca o melhor ótimo local a partir da solução encontrada no construtivo
    while(interILS < Iils){
      //Busca melhor ótimo local da solução parcial
      custoParcial = rvnd(solucaoParcial, custoParcial);

      //Testa se houve melhora
      if(custoParcial < custoMelhor){
        //Registra a solução parcial como melhor solução
        custoMelhor = custoParcial;
        solucaoMelhor.clear();
        for (size_t i = 0; i < solucaoParcial.size(); i++){
          solucaoMelhor.push_back(solucaoParcial[i]);
        }

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
      solucaoFinal.clear();
      for (size_t i = 0; i < solucaoMelhor.size(); i++){
        solucaoFinal.push_back(solucaoMelhor[i]);
      }
    }
  }

  return custoFinal;
}

// Vizinhanças
int reinsertion(vector<int> &solucao, int blocoSize, int custoDaSolucaoAnterior){
  // Inicia variáveis
  int deltaCusto = 0;
  int custoRetirada = 0;
  int custoInsercao = 0;
  int custoDaSolucao = custoDaSolucaoAnterior;
  bool flag = false;

  tReinsercao insercao;

  while(1){
    //Procura local de melhor reinserção
    for(size_t i = 1; i < solucao.size() - 1 - blocoSize; i++){
      custoRetirada = matrizAdj[solucao[i-1]][solucao[i+blocoSize]] - (matrizAdj[solucao[i-1]][solucao[i]] + matrizAdj[solucao[i+blocoSize-1]][solucao[i+blocoSize]]);

      for(size_t y = i+blocoSize; y < solucao.size() - blocoSize; y++){
        custoInsercao = (matrizAdj[solucao[y]][solucao[i]] + matrizAdj[solucao[i+blocoSize-1]][solucao[y+1]]) - matrizAdj[solucao[y]][solucao[y+1]];

        if((custoRetirada + custoInsercao) < deltaCusto){
          flag = true;

          deltaCusto = custoInsercao + custoRetirada;
          insercao.posVertice = i;
          insercao.posInsercao = y+1;
          insercao.vertice = solucao[i];
        }
      }
    }

    //Aplica reinserção
    if(flag){
      custoDaSolucao = custoDaSolucao + deltaCusto;

      if(blocoSize == 1){
        solucao.insert(solucao.begin() + insercao.posInsercao, solucao[insercao.posVertice]);
        solucao.erase(solucao.begin() + insercao.posVertice);
      } else {
        solucao.insert(solucao.begin() + insercao.posInsercao, solucao.begin() + insercao.posVertice, solucao.begin() + insercao.posVertice + blocoSize);
        //printSolucao(solucao, solucao.size());
        solucao.erase(solucao.begin() + insercao.posVertice, solucao.begin() + insercao.posVertice + blocoSize);
      }

      flag = false;
      deltaCusto = 0;

    } else {
      break;
    }
  }

  return custoDaSolucao;
}

int swap(vector<int> &solucao, int custoDaSolucaoAnterior){
  //Inicia variáveis
  int deltaCusto = 0;
  int custoRetirada = 0;
  int custoInsercao = 0;
  int custoDaSolucao = custoDaSolucaoAnterior;
  bool flag = false;

  tSwap swap;

  while(1){
    //Aplica reinserção
    for(size_t i = 1; i < solucao.size() - 3; i++){
      for(size_t y = i+2; y < solucao.size()-1; y++){
        custoRetirada = matrizAdj[solucao[i-1]][solucao[i]] + matrizAdj[solucao[i]][solucao[i+1]] +
                        matrizAdj[solucao[y-1]][solucao[y]] + matrizAdj[solucao[y]][solucao[y+1]];

        custoInsercao = matrizAdj[solucao[i-1]][solucao[y]] + matrizAdj[solucao[y]][solucao[i+1]] +
                        matrizAdj[solucao[y-1]][solucao[i]] + matrizAdj[solucao[i]][solucao[y+1]];

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

    } else {
      break;

    }
  }

  return custoDaSolucao;
}

int twoOptN(vector<int> &solucao, int custoDaSolucaoAnterior){
  //Inicia variáveis
  int deltaCusto = 0;
  int custoRetirada = 0;
  int custoInsercao = 0;
  int custoDaSolucao = custoDaSolucaoAnterior;
  int aux = 0;
  bool flag = false;

  tSwap swap;

  custoSolucao(&custoDaSolucao, solucao, dimension);

  while(1){
    for(size_t i = 1; i < solucao.size() - 3; i++){
      for(size_t y = i + 3; y < solucao.size(); y++){
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

      for(size_t i = 0; i < (swap.pos2 - swap.pos1); i++){
        aux = solucao[swap.pos2];
        solucao.erase(solucao.begin() + swap.pos2);
        solucao.emplace(solucao.begin() + swap.pos1 + i, aux);
      }

      flag = false;
      deltaCusto = 0;

    } else {
      break;
    }
  }

  return custoDaSolucao;
}

//Pertubações
int doubleBridge(vector<int> &solucao, int custoDaSolucaoAnterior){
  int sizeBlock = (solucao.size() - 2) / 3;
  int aux1 = 0;
  int aux2 = 0;
  int custoInicial;
  int custoFinal;
  int deltaCusto;

  custoInicial = matrizAdj[solucao[0]][solucao[1]] +
                  matrizAdj[solucao[sizeBlock]][solucao[sizeBlock + 1]] +
                  matrizAdj[solucao[sizeBlock*2]][solucao[sizeBlock*2 + 1]] +
                  matrizAdj[solucao[sizeBlock*3]][solucao[sizeBlock*3 + 1]];

  for(size_t i = 0; i < sizeBlock; i++){
    aux1 = solucao[i+1];
    aux2 = solucao[i+1+(sizeBlock*2)];

    solucao.erase(solucao.begin() + i + 1);
    solucao.emplace(solucao.begin() + i + 1, aux2);

    solucao.erase(solucao.begin() + i + 1 + (sizeBlock*2));
    solucao.emplace(solucao.begin() + i + 1 + (sizeBlock*2), aux1);
  }

  custoFinal = matrizAdj[solucao[0]][solucao[1]] +
                matrizAdj[solucao[sizeBlock]][solucao[sizeBlock + 1]] +
                matrizAdj[solucao[sizeBlock*2]][solucao[sizeBlock*2 + 1]] +
                matrizAdj[solucao[sizeBlock*3]][solucao[sizeBlock*3 + 1]];

  deltaCusto = custoFinal - custoInicial;

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
