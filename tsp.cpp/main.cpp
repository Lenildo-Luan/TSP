
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

double ** matrizAdj; // matriz de adjacencia
int dimension; // quantidade total de vertices

//Heurísticas construtivas
void vizinhoMaisProximo(int solucaoInicial[]); // Solução inicial pelo vizinho mais próximo
void insercaoMaisBarata(int solucaoInicial[]); // Solução inicial pela inserção mais Barata
void insercaoMaisBarataAprimorada(int solucaoInicial[]);

// Untils
void printData(); // Mostra matriz de adjacencia
void printSolucao(int array[], int tamanhoArray); // Mostra a solução inicial gerada pel algorítimo escolhido
void custoSolucao(int *custoTotal, int array[], int tamanhoArray); // Mostra o custo da solução gerada
bool compareByCost(const tInsercao &data1, const tInsercao &data2);

//GILS-RVND
void construtivo(int solucaoInicial[]);
void rvnd(int solucaoInicial[]);

int main(int argc, char** argv) {
    readData(argc, argv, &dimension, &matrizAdj);
    printData();

    int solucaoInicial[dimension];
    int custoTotal = 0;

    for (size_t i = 1; i <= dimension; i++) {
      solucaoInicial[i] = 0;
    }

    construtivo(solucaoInicial);
    rvnd(solucaoInicial);

    printSolucao(solucaoInicial, dimension);
    custoSolucao(&custoTotal, solucaoInicial, dimension);

    cout <<  "Custo: " << custoTotal << endl;

    return 0;
}

//GILS-RVND
void construtivo(int solucaoInicial[]){
  // Inicia variáveis
  vector<int> solucao; // Vector da solução
  vector<int> verticesList; // Lista de vertices faltando
  vector<tInsercao> bestVerticesList; // Lista dos X melhores verticesList

  mt19937 mt(42); // Gerador de números aleatórios
  uniform_int_distribution<int> linear_i(1, dimension); // Distribuição linear de inteiros para escolher vertice inicial
  uniform_real_distribution<float> linear_f(0.0, 0.5); // Distribuição linear de reais para gerar alpha

  float alpha = 0.0;
  int rBest;
  int randomVertice1, randomVertice2, randomVertice3;
  int tamanhoSolucao;
  int verticesRestantes;
  int distanciaVertice;
  tInsercao insercao;

  // Escolhe três vertices de forma aleatória
  while(1){
    randomVertice1 = linear_i(mt);
    randomVertice2 = linear_i(mt);
    randomVertice3 = linear_i(mt);

    if(randomVertice1 != randomVertice2 &&
       randomVertice1 != randomVertice3 &&
       randomVertice2 != randomVertice3) break;
  }
  solucao.push_back(randomVertice1);
  solucao.push_back(randomVertice2);
  solucao.push_back(randomVertice3);

  // Gera lisra de vertices faltantes
  for (size_t i = 1; i <= dimension; i++) {
    if(i == randomVertice1 || i == randomVertice2 || i == randomVertice3) continue;
    verticesList.push_back(i);
  }

  // Gera solução
  while(1) {
    tamanhoSolucao = solucao.size();
    verticesRestantes = verticesList.size();
    distanciaVertice = 0;

    // Gera lista de custo de vertices
    for (size_t i = 0; i < verticesRestantes; i++) {
      insercao.vertice = verticesList[i];
      insercao.pos = 0;
      insercao.custo = (matrizAdj[solucao[0]][verticesList[i]] + matrizAdj[verticesList[i]][solucao[tamanhoSolucao-1]]) - matrizAdj[solucao[0]][solucao[tamanhoSolucao-1]];
      bestVerticesList.push_back(insercao);
      for (size_t j = 1; j < tamanhoSolucao; j++) {
        insercao.vertice = verticesList[i];
        insercao.pos = j;
        insercao.custo = (matrizAdj[solucao[j]][verticesList[i]] + matrizAdj[verticesList[i]][solucao[j-1]]) - matrizAdj[solucao[j]][solucao[j-1]];
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

  // Preenche o array solução na main
  for (size_t i = 1; i <= dimension; i++){
    solucaoInicial[i] = solucao[i-1];
    //cout << solucaoInicial[i] << " ";
  }

}
void rvnd(int solucaoInicial[]){
  // Inicia variáveis
  vector<int> solucao;
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
    solucao.push_back(solucaoInicial[i+1]);

    vizinho.pos = i;
    vizinho.vertice = solucaoInicial[i+1];
    vizinhanca.push_back(vizinho);
  }
  solucao.push_back(solucaoInicial[1]);

  // for (size_t i = 0; i <= dimension; i++) {
  //   cout << solucao[i] << " ";
  // }
  // cout << endl;

  //Verifica custo da solução
  custoSolucao(&custoSolucaoAnterior, solucaoInicial, dimension);
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
    }
    else custoRetiradaVizinho = matrizAdj[solucao[vizinho.pos-1]][solucao[vizinho.pos+1]] - (matrizAdj[solucao[vizinho.pos-1]][vizinho.vertice] + matrizAdj[vizinho.vertice][solucao[vizinho.pos+1]]);

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

  //Retorna solução
  for (size_t i = 1; i <= dimension; i++){
    solucaoInicial[i] = solucao[i-1];
    //cout << solucaoInicial[i] << " ";
  }

}

void vizinhoMaisProximo(int solucaoInicial[]) {
  int v_repeticao[dimension]; // Array de vizinhos já escolhidos

  // Inicia arrays
  for(size_t i = 1; i <= dimension; i++){
    solucaoInicial[i] = 1;
    v_repeticao[i] = 0;
  }

  // Gera a solução
  for (size_t i = 1; i <= dimension; i++) { // Itera sobre os vizinhos
    int flag = 0;

    for (size_t j = 1; j <= dimension; j++) { // Itera sobre as distâncias
      if(v_repeticao[j] || matrizAdj[i][j] == 1){ // Verifica se o vizinho já foi escolhido ou se está na diagonal
        if(!flag) solucaoInicial[i]++;
        continue;
      }
      //cout << j << " --- " << matrizAdj[i][j] << " | " << matrizAdj[i][solucaoInicial[i]] << endl;
      if(matrizAdj[i][j] <= matrizAdj[i][solucaoInicial[i]]) { // Adiciona vizinho à solução
        flag++;
        solucaoInicial[i] = j;
        //cout << j << " === " << matrizAdj[i][j] << " | " << solucaoInicial[i] << " | " << (matrizAdj[i][j] < matrizAdj[i][solucaoInicial[i]]) << endl;
      }
    }
    v_repeticao[solucaoInicial[i]] = 1; // Adiciona vizinho a array de vizinhos utilizados
  }
}
void insercaoMaisBarata(int solucaoInicial[]) {
  // Inicia variáveis
  vector<int> solucao;
  int custoFinal = 0;

  // Escolhe dois vertices iniciais
  if(matrizAdj[1][2] < matrizAdj[2][1]){
    solucao.push_back(1);
    solucao.push_back(2);
    custoFinal = matrizAdj[1][2];
  } else {
    solucao.push_back(2);
    solucao.push_back(1);
    custoFinal = matrizAdj[2][1];
  }

  // Realiza inserções
  for (size_t i = 1; i <= dimension - 2; i++) {
    int tamanhoVector = solucao.size();
    int posicaoDaInsercao = 0;
    int custoDaInsercao = matrizAdj[tamanhoVector + 1][solucao[0]];

    for (size_t j = 1; j <= tamanhoVector; j++) {
    //  cout << custoDaInsercao << " | ";
      if(j == tamanhoVector){
        if( ((matrizAdj[solucao[j - 1]][tamanhoVector + 1] + matrizAdj[tamanhoVector + 1][solucao[0]]) - matrizAdj[solucao[0]][solucao[j-1]]) < custoDaInsercao) {
          //cout << endl << matrizAdj[tamanhoVector + 1][solucao[j]] + matrizAdj[tamanhoVector + 1][solucao[j - 1]] << " : " << matrizAdj[solucao[j]][solucao[j - 1]]  << " : " << (matrizAdj[tamanhoVector + 1][solucao[j]] + matrizAdj[tamanhoVector + 1][solucao[j - 1]] - matrizAdj[solucao[j]][solucao[j - 1]])  << " : " << custoDaInsercao << endl;
          custoDaInsercao = matrizAdj[solucao[j - 1]][tamanhoVector + 1] + matrizAdj[tamanhoVector + 1][solucao[0]] - matrizAdj[solucao[0]][solucao[j-1]];
          posicaoDaInsercao = 0;
        }
      } else if( ((matrizAdj[solucao[j - 1]][tamanhoVector + 1] + matrizAdj[tamanhoVector + 1][solucao[j]]) - matrizAdj[solucao[j]][solucao[j-1]]) < custoDaInsercao) {
        //cout << endl << matrizAdj[tamanhoVector + 1][solucao[j]] + matrizAdj[tamanhoVector + 1][solucao[j - 1]] << " : " << matrizAdj[solucao[j]][solucao[j - 1]]  << " : " << (matrizAdj[tamanhoVector + 1][solucao[j]] + matrizAdj[tamanhoVector + 1][solucao[j - 1]] - matrizAdj[solucao[j]][solucao[j - 1]])  << " : " << custoDaInsercao << endl;
        custoDaInsercao = matrizAdj[tamanhoVector + 1][solucao[j]] + matrizAdj[tamanhoVector + 1][solucao[j - 1]] - matrizAdj[solucao[j]][solucao[j - 1]];
        posicaoDaInsercao = j;
      }
    }
    auto it = solucao.emplace( solucao.begin() + posicaoDaInsercao, tamanhoVector + 1);

    //custoFinal = 0;

    //cout << "Solucao: ";

    // for (size_t j = 0; j < solucao.size() - 1; j++){
    //   custoFinal += matrizAdj[solucao[j]][solucao[j+1]];
    //   cout << solucao[j] << " ";
    //   if(j == solucao.size() - 2) cout << solucao[j + 1] << " ";
    //   // cout << endl << "Vertices: " << solucao[j] << "-" << solucao[j+1];
    //   // cout << " Custo: "  << matrizAdj[solucao[j]][solucao[j+1]] << " | ";
    // }

    //cout << endl << "CustoFinal: " << custoFinal + matrizAdj[solucao[solucao.size() - 1]][solucao[0]] << endl << endl;

  }

  //cout << endl << "Custo Final: " << custoFinal + matrizAdj[solucao[solucao.size() - 1]][solucao[0]] << endl ;

  for (size_t i = 1; i <= dimension; i++){
    solucaoInicial[i] = solucao[i-1];
    //cout << solucaoInicial[i] << " ";
  }
}
void insercaoMaisBarataAprimorada(int solucaoInicial[]) {
  // Inicia variáveis
  vector<int> solucao;
  vector<int> listaVizinhos;
  vector<vector<int>  > listaCandidatos;
  int custoFinal;

  // for (size_t i = 0; i < dimension - 2; i++) {
  //   vector<int> row;
  //   for (size_t j = 0; j < 3; j++) {
  //     row.push_back(0);
  //   }
  //   listaCandidatos.push_back(row);
  // }

  for (size_t i = 0; i < dimension - 2; i++) {
    listaVizinhos.push_back(i+3);
  }

  // Seleciona dois vertices
  if(matrizAdj[1][2] < matrizAdj[2][1]){
    solucao.push_back(1);
    solucao.push_back(2);
    custoFinal = matrizAdj[1][2];
  } else {
    solucao.push_back(2);
    solucao.push_back(1);
    custoFinal = matrizAdj[2][1];
  }

  // Faz inserções
  for (size_t i = 1; i <= dimension - 2; i++){ // Para que todos os vizinhos sejam colocados
    vector<int> insercaoInfo;
    int tamanhoVector = solucao.size();
    int qtdVizinhosRestantes = listaVizinhos.size();
    int melhorCusto = 0;
    int posicao = 0;
    int vizinho = 0;
    int posVizinho = 0;

    for (size_t j = 0; j < qtdVizinhosRestantes; j++){ // Itera sobre todos os vizinhos restantes
      insercaoInfo.push_back(matrizAdj[listaVizinhos[j]][solucao[0]] + matrizAdj[listaVizinhos[j]][solucao[tamanhoVector - 1]] - matrizAdj[solucao[0]][solucao[tamanhoVector-1]]);
      insercaoInfo.push_back(0);
      insercaoInfo.push_back(listaVizinhos[j]);
      insercaoInfo.push_back(j);
      //cout << "Custo vizinho " << listaVizinhos[j] << " " << matrizAdj[listaVizinhos[j]][solucao[0]] + matrizAdj[listaVizinhos[j]][solucao[tamanhoVector - 1]] << endl;

      for (size_t k = 1; k <= tamanhoVector; k++) { // Itera sobre os vizinhos já selecionados
        if(k == tamanhoVector){
          if( ((matrizAdj[solucao[k - 1]][listaVizinhos[j]] + matrizAdj[listaVizinhos[j]][solucao[0]]) - matrizAdj[solucao[0]][solucao[k-1]]) < insercaoInfo[0]) {
            insercaoInfo[0] = matrizAdj[solucao[k - 1]][listaVizinhos[j]] + matrizAdj[listaVizinhos[j]][solucao[0]] - matrizAdj[solucao[0]][solucao[k-1]];
            insercaoInfo[1] = 0;
            insercaoInfo[2] = listaVizinhos[j];
            insercaoInfo[3] = j;
          }
        } else if( ((matrizAdj[solucao[k - 1]][listaVizinhos[j]] + matrizAdj[listaVizinhos[j]][solucao[k]]) - matrizAdj[solucao[k]][solucao[k-1]]) < insercaoInfo[0]) {
            insercaoInfo[0] = matrizAdj[listaVizinhos[j]][solucao[k]] + matrizAdj[listaVizinhos[j]][solucao[k - 1]] - matrizAdj[solucao[k]][solucao[k - 1]];
            insercaoInfo[1] = k;
            insercaoInfo[2] = listaVizinhos[j];
            insercaoInfo[3] = j;
        }
      }
      listaCandidatos.push_back(insercaoInfo);
      insercaoInfo.clear();

    }
    melhorCusto = listaCandidatos[0][0];
    posicao = listaCandidatos[0][1];
    vizinho = listaCandidatos[0][2];
    posVizinho = listaCandidatos[0][3];

    //cout << listaCandidatos.size();
      for (size_t j = 0; j < listaCandidatos.size(); j++) {
        //cout << endl << "Custo: " << listaCandidatos[j][0] << " Pos: " << listaCandidatos[j][1] << " Viz: " << listaCandidatos[j][2];
        if(listaCandidatos[j][0] < melhorCusto){
          melhorCusto = listaCandidatos[j][0];
          posicao = listaCandidatos[j][1];
          vizinho = listaCandidatos[j][2];
          posVizinho = listaCandidatos[j][3];
        }
    }
    //cout << endl << "Vizinho escolhido: " << vizinho << endl;

    auto it = solucao.emplace(solucao.begin() + posicao, vizinho);
    it = listaVizinhos.erase(listaVizinhos.begin() + posVizinho);
    listaCandidatos.clear();

    // custoFinal = 0;
    //
    // cout << "Solucao: ";
    //
    // for (size_t j = 0; j < solucao.size() - 1; j++){
    //   custoFinal += matrizAdj[solucao[j]][solucao[j+1]];
    //   cout << solucao[j] << " ";
    //   if(j == solucao.size() - 2) cout << solucao[j + 1] << " ";
    //   // cout << endl << "Vertices: " << solucao[j] << "-" << solucao[j+1];
    //   //cout << " Custo: "  << matrizAdj[solucao[j]][solucao[j+1]] << " | ";
    // }

    //cout << endl << "CustoFinal: " << custoFinal + matrizAdj[solucao[solucao.size() - 1]][solucao[0]] << endl << endl;
  }

  //cout << endl << "Custo Final: " << custoFinal  << endl ;

  for (size_t i = 1; i <= dimension; i++){
    solucaoInicial[i] = solucao[i-1];
    //cout << solucaoInicial[i] << " ";
  }
}

void printData() {
  cout << endl << "dimension: " << dimension << endl;
  for (size_t i = 1; i <= dimension; i++) {
    for (size_t j = 1; j <= dimension; j++) {
      cout << matrizAdj[i][j] << " ";
    }
    cout << endl;
  }
}
void printSolucao(int array[], int tamanhoArray){
  cout << endl << "Solucao: [ ";

  for(size_t i = 1; i <= dimension; i++){
    cout << array[i] << " ";
  }

  cout << "]" << endl;
}
void custoSolucao(int *custoTotal, int array[], int tamanhoArray) {
  *custoTotal = 0;

  for(size_t i = 1; i < tamanhoArray; i++){
    *custoTotal += matrizAdj[array[i]][array[i + 1]];
    //cout << i << " - " << matrizAdj[array[i]][array[i + 1]] << " - " << array[i+1] << endl;
  }
  *custoTotal += matrizAdj[array[tamanhoArray]][array[1]];
}
bool compareByCost(const tInsercao &data1, const tInsercao &data2){
  return data1.custo < data2.custo;
}
