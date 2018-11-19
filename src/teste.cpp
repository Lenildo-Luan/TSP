// #include "readData.h"

// #include <string>
// #include <random>
// #include <bits/stdc++.h>
// #include <fstream>
// #include <iostream>
// #include <cmath>
// #include <algorithm>

// using namespace std;

// typedef struct{
//   int pos;
//   int vertice;
//   int custo;
// } tInsercao;

// typedef struct{
//   int posVertice;
//   int posInsercao;
//   int vertice;
// } tReinsercao;

// typedef struct{
//   int pos1;
//   int vertice1;
//   int pos2;
//   int vertice2;
// } tSwap;

// // Pega da instâncias
// double ** matrizAdj; // matriz de adjacencia
// int dimension; // quantidade total de vertices

// // Untils
// void printData(); // Mostra matriz de adjacencia
// void printSolucao(vector<int> &solucao, int tamanhoArray); // Mostra a solução inicial gerada pel algorítimo escolhido
// void custoSolucao(int *custoTotal, vector<int> solucao, int tamanhoArray); // Mostra o custo da solução gerada
// bool compareByCost(const tInsercao &data1, const tInsercao &data2);
// int obtemTemperaturaInicial(vector<int> &solucao, int custo, int beta, float gama, int iteracoesPorTemperatura, int temperaturaInicial);

// //Meta-heuristic
// int rvnd(vector<int> &solucao, int custoDaSolucaoAnterior);
// int gilsRvnd(vector<int> &solucao, int Imax, int Iils);
// int simulatedAnnealing(vector<int> &solucao, int custo, float alpha, int iteracoesPorTemperatura, int temperaturaInicial);
// int vns(vector<int> &solucao, int custo, int Imax);

// //Heuristic
// int construtivo(vector<int> &solucao, int coleta, int deposito, float alpha);

// //Busca local
// int reinsertion(vector<int> &solucao, int blocoSize, int custoDaSolucaoAnterior);
// int swap(vector<int> &solucao, int custoDaSolucaoAnterior);
// int twoOptN(vector<int> &solucao, int custoDaSolucaoAnterior);

// //Vizinhanças
// vector<int> reinsercaoAleatoria(vector<int> solucao, int *custoParcial, int custo);

// //Pertubações
// int doubleBridge(vector<int> &solucao, int custoDaSolucaoAnterior);

// //MAIN
// int main(int argc, char** argv){
//     //Obtem valores da instância
//     readData(argc, argv, &dimension, &matrizAdj);
//     //printData();

//     //Declara variáveis da solução
//     vector<int> solucao;
//     int custo;

//     //Declara variáveis da meta-heurística simulatedAnnealing
//     uniform_real_distribution<float> linear_f2(0.0, 1.0);
//     int temperaturaInicial = 1;
//     int iteracoesPorTemperatura = 10*dimension;
//     int beta = 2;
//     float gama = 0.95;

//     //Declara variáveis do método construtivo
//     random_device rd;
//     mt19937 mt(rd());
//     uniform_real_distribution<float> linear_f1(0.0, 0.5);
//     int coleta = 1;
//     int deposito = 1;
//     float alpha = 0.0;

//     //Gera solução inicial
//     // alpha = linear_f1(mt);
//     // custo = construtivo(solucao, coleta, deposito, alpha);

//     //Meta-heurística simulatedAnnealing
//     // alpha = linear_f2(mt);
//     // temperaturaInicial = obtemTemperaturaInicial(solucao, custo, beta, gama, iteracoesPorTemperatura, temperaturaInicial);
//     // custo = simulatedAnnealing(solucao, custo, alpha, iteracoesPorTemperatura, temperaturaInicial);

//     //VNS
//     //custo = vns(solucao, custo, 50);

//     //GILS-RVND
//     custo = gilsRvnd(solucao, 50, 4*dimension);

//     printSolucao(solucao, dimension);
//     //custoSolucao(&custo, solucao, solucao.size());
//     cout << "Custo: " << custo << endl;
//     cout << "Temperatura inicial: " << temperaturaInicial << endl;

//     return 0;
// }

// //Meta-Heuristic
// int rvnd(vector<int> &solucao, int custoDaSolucaoAnterior){
//   vector<int> vizinhanca = {1, 2, 3, 4, 5};
//   vector<int> solucaoTeste;

//   random_device rd;
//   mt19937 mt(rd());

//   int solucaoSize = solucao.size();
//   int vizinhancaSize = vizinhanca.size();
//   int vizinhoRandom = 0;
//   int random = 0;
//   int novoCusto = 0;
//   int custoAnterior = custoDaSolucaoAnterior;

//   for(size_t i = 0; i < solucaoSize; i++){
//     solucaoTeste.push_back(solucao[i]);
//   }

//   while(1){
//     uniform_int_distribution<int> linear_i(0, vizinhanca.size() - 1);
//     random = linear_i(mt);
//     vizinhoRandom = vizinhanca[random];

//     switch(vizinhoRandom){
//       case 1:
//         novoCusto = reinsertion(solucaoTeste, 1, custoAnterior);
//         break;
//       case 2:
//         novoCusto = reinsertion(solucaoTeste, 2, custoAnterior);
//         break;
//       case 3:
//         novoCusto = reinsertion(solucaoTeste, 3, custoAnterior);
//         break;
//       case 4:
//         novoCusto = swap(solucaoTeste, custoAnterior);
//         break;
//       case 5:
//         novoCusto = twoOptN(solucaoTeste, custoAnterior);
//         break;
//       default:
//         break;
//     }

//     if(novoCusto < custoAnterior){
//       custoAnterior = novoCusto;

//       solucao.clear();
//       for(size_t i = 0; i < solucaoSize; i++){
//         solucao.push_back(solucaoTeste[i]);
//       }

//       vizinhanca.clear();
//       for(size_t i = 1; i <= vizinhancaSize; i++){
//         vizinhanca.push_back(i);
//       }
//     } else {
//       vizinhanca.erase(vizinhanca.begin() + random);
//     }

//     if(vizinhanca.size() == 0) break;
//   }

//   return custoAnterior;
// }

// int gilsRvnd(vector<int> &solucaoFinal, int Imax, int Iils){
//   //Declara Variáveis
//   random_device rd;
//   mt19937 mt(rd());
//   uniform_real_distribution<float> linear_f(0.0, 0.5); // Distribuição linear de reais para gerar alpha

//   vector<int> solucaoParcial;
//   vector<int> solucaoMelhor;

//   int custoFinal = INT32_MAX;
//   int custoParcial = 0;
//   int custoMelhor = 0;
//   int coleta = 1;
//   int deposito = 1;
//   int interILS;
//   int solucaoSize;
//   float alpha = 0.0;

//   //Busca Melhor Solução
//   for (size_t i = 0; i < Imax; i++) {
//     //Gera Alfa e o interador de ILS
//     alpha = linear_f(mt);
//     interILS = 0;

//     //Calcula solução parcial com o método construtivo
//     solucaoParcial.clear();
//     custoParcial = construtivo(solucaoParcial, coleta, deposito, alpha);
//     solucaoSize = solucaoParcial.size();

//     //Registra a solução parcial como melhor solução
//     custoMelhor = custoParcial;
//     solucaoMelhor.clear();
//     for (size_t i = 0; i < solucaoSize; i++){
//       solucaoMelhor.push_back(solucaoParcial[i]);
//     }

//     //Busca o melhor ótimo local a partir da solução encontrada no construtivo
//     while(interILS < Iils){
//       //Busca melhor ótimo local da solução parcial
//       custoParcial = rvnd(solucaoParcial, custoParcial);

//       //Testa se houve melhora
//       if(custoParcial < custoMelhor){
//         //Registra a solução parcial como melhor solução
//         custoMelhor = custoParcial;
//         solucaoMelhor.clear();
//         for (size_t i = 0; i < solucaoSize; i++){
//           solucaoMelhor.push_back(solucaoParcial[i]);
//         }

//         //Zera o iterador
//         interILS = 0;
//       }

//       //Pertuba a solução
//       custoParcial = doubleBridge(solucaoParcial, custoParcial);

//       //Soma o interador
//       interILS++;
//     }

//     //Testa se houve melhora
//     if(custoMelhor < custoFinal){
//       custoFinal = custoMelhor;
//       solucaoFinal.clear();    uniform_real_distribution<float> linear_f2(0.0, 1.0);
//     int temperaturaInicial = 1;
//     int iteracoesPorTemperatura = dimension;
//     int beta = 2;
//     float gama = 0.95;
//       for (size_t i = 0; i < solucaoSize; i++){
//         solucaoFinal.push_back(solucaoMelhor[i]);
//       }
//     }
//   }

//   return custoFinal;
// }

// int simulatedAnnealing(vector<int> &solucao, int custo, float alpha, int iteracoesPorTemperatura, int temperaturaInicial){
//   random_device rd;
//   mt19937 mt(rd());
//   uniform_int_distribution<int> linear_i(4, solucao.size()-1);
//   uniform_real_distribution<float> linear_f(0.0, 1.0);

//   vector<int> bestSol = solucao;
//   vector<int> partialSol;

//   int iterT = 0;
//   int temperatura = temperaturaInicial;
//   int solucaoSize = solucao.size();
//   int deltaCusto;
//   float x;

//   int bestCost = custo, partialCost = custo;

//   while (temperatura > 0) {
//     while (iterT < iteracoesPorTemperatura) {
//       iterT++;

//       partialSol = reinsercaoAleatoria(solucao, &partialCost, custo);
//       partialCost = rvnd(partialSol, partialCost);

//       deltaCusto = partialCost - custo;

//       if(deltaCusto < 0){
//         custo = partialCost;
//         solucao = partialSol;

//         if(partialCost < bestCost){
//           bestCost = partialCost;
//           bestSol = partialSol;
//         }
//       } else {
//         x = linear_f(mt);

//         if(x < exp(-(deltaCusto) / temperatura) ){
//           //partialCost = doubleBridge(partialSol, partialCost);
//           custo = partialCost;
//           solucao = partialSol;
//         }
//       }
//     }

//     temperatura = alpha * temperatura;
//     iterT = 0;
//   }

//   solucao = bestSol;

//   return bestCost;
// }

// int vns(vector<int> &melhorSolucao, int melhorCusto, int Imax){
//   vector<int> vizinhanca = {1};
//   vector<int> solucaoCorrente;

//   int Ivnd = 0;
//   int vizinhancaSize = vizinhanca.size();
//   int vizinhancaCorrente = 0;
//   int custoCorrente;

//   while (Ivnd < Imax) {
//     vizinhancaCorrente = 0;

//     while (vizinhancaCorrente < vizinhancaSize) {
//       switch(vizinhancaCorrente){
//         case 0:
//           solucaoCorrente = reinsercaoAleatoria(melhorSolucao, &custoCorrente, melhorCusto);
//           break;
//         default:
//           cout << "Algo deu errado!" << endl;
//           break;
//       }

//       custoCorrente = rvnd(solucaoCorrente, custoCorrente);

//       if(custoCorrente < melhorCusto){
//         melhorCusto = custoCorrente;
//         melhorSolucao = solucaoCorrente;
//         vizinhancaCorrente = 1;
//       } else {
//         vizinhancaCorrente++;
//       }
//     }

//     Ivnd++;
//   }

//   return melhorCusto;
// }

// //Heuristic
// int construtivo(vector<int> &solucao, int coleta, int deposito, float alpha){
//   // Inicia variáveis
//   vector<int> verticesList; // Lista de vertices faltando
//   vector<tInsercao> bestVerticesList; // Lista dos X melhores verticesList

//   random_device rd;
//   mt19937 mt(rd()); // Gerador de números aleatórios

//   int rBest;
//   int randomVertice1, randomVertice2, randomVertice3;
//   int tamanhoSolucao;
//   int verticesRestantes;
//   int distanciaVertice;
//   int custoSolucao = 0;
//   tInsercao insercao;

//   solucao.clear();

//   //Adiciona coleta ao vector
//   solucao.push_back(coleta);

//   // Gera lista de vertices faltantes
//   for (size_t i = 1; i <= dimension; i++) {
//     if(i == coleta || i == deposito) continue;
//     verticesList.push_back(i);
//   }

//   // Escolhe três vertices de forma aleatória
//   for (size_t i = 0; i < 3; i++) {
//     uniform_int_distribution<int> linear_i(0, verticesList.size()-1); // Distribuição linear de inteiros para escolher vertice inicial
//     randomVertice1 = linear_i(mt);

//     solucao.push_back(verticesList[randomVertice1]);
//     verticesList.erase(verticesList.begin() + randomVertice1);

//     custoSolucao += matrizAdj[solucao[i]][solucao[i+1]];
//   }

//   // Adiciona deposito ao vector
//   solucao.push_back(deposito);

//   custoSolucao += matrizAdj[solucao[3]][solucao[4]];

//   // Gera solução
//   while(1) {
//     tamanhoSolucao = solucao.size();
//     verticesRestantes = verticesList.size();
//     distanciaVertice = 0;

//     // Gera lista de custo de vertices
//     for (size_t i = 0; i < verticesRestantes; i++) { // Itera sobre vértices restantes
//       for (size_t j = 1; j < tamanhoSolucao; j++) { // Itera sobre a solução
//         insercao.vertice = verticesList[i];
//         insercao.pos = j;
//         insercao.custo = (matrizAdj[solucao[j-1]][verticesList[i]] + matrizAdj[verticesList[i]][solucao[j]]) - matrizAdj[solucao[j-1]][solucao[j]];
//         bestVerticesList.push_back(insercao);
//       }
//     }

//     //Arruma lista pelo valor do custo
//     sort(bestVerticesList.begin(), bestVerticesList.end(), compareByCost);

//     // Adiciona novo vertice à solução conforme alpha
//     uniform_int_distribution<int> linear_i_alpha(0, int((bestVerticesList.size()-1)*alpha));
//     rBest = linear_i_alpha(mt);
//     solucao.emplace(solucao.begin() + bestVerticesList[rBest].pos, bestVerticesList[rBest].vertice);

//     //Calcula custo
//     custoSolucao += bestVerticesList[rBest].custo;

//     // Reseta Vectors
//     for (size_t i = 0; i < verticesRestantes; i++) {
//       if(bestVerticesList[rBest].vertice == verticesList[i]) {
//         verticesList.erase(verticesList.begin() + i);
//         break;
//       }
//     }
//     bestVerticesList.clear();

//     // Se não tiverem mais candidatos o loop acaba
//     if(verticesRestantes == 1) {
//       break;
//     }
//   }

//   return custoSolucao;

//   // // Mostra solução
//   // for (size_t i = 0; i < solucao.size(); i++){
//   //   cout << solucao[i] << " ";
//   //   //cout << solucaoInicial[i] << " ";
//   // }

// }

// //Busca local
// int reinsertion(vector<int> &solucao, int blocoSize, int custoDaSolucaoAnterior){
//   // Inicia variáveis
//   int deltaCusto = 0;
//   int custoRetirada = 0;
//   int custoInsercao = 0;
//   int custoDaSolucao = custoDaSolucaoAnterior;
//   int solucaoSize = solucao.size();
//   bool flag = false;
//   tReinsercao insercao;

//   while(1){
//     //Procura local de melhor reinserção
//     for(size_t i = 1; i < solucaoSize - 1 - blocoSize; i++){
//       custoRetirada = matrizAdj[solucao[i-1]][solucao[i+blocoSize]] - (matrizAdj[solucao[i-1]][solucao[i]] + matrizAdj[solucao[i+blocoSize-1]][solucao[i+blocoSize]]);

//       for(size_t y = i+blocoSize; y < solucaoSize - blocoSize; y++){
//         custoInsercao = (matrizAdj[solucao[y]][solucao[i]] + matrizAdj[solucao[i+blocoSize-1]][solucao[y+1]]) - matrizAdj[solucao[y]][solucao[y+1]];

//         if((custoRetirada + custoInsercao) < deltaCusto){
//           flag = true;

//           deltaCusto = custoInsercao + custoRetirada;
//           insercao.posVertice = i;
//           insercao.posInsercao = y+1;
//           insercao.vertice = solucao[i];
//         }
//       }
//     }

//     //Aplica reinserção
//     if(flag){
//       custoDaSolucao = custoDaSolucao + deltaCusto;

//       if(blocoSize == 1){
//         solucao.insert(solucao.begin() + insercao.posInsercao, solucao[insercao.posVertice]);
//         solucao.erase(solucao.begin() + insercao.posVertice);
//       } else {
//         solucao.insert(solucao.begin() + insercao.posInsercao, solucao.begin() + insercao.posVertice, solucao.begin() + insercao.posVertice + blocoSize);
//         solucao.erase(solucao.begin() + insercao.posVertice, solucao.begin() + insercao.posVertice + blocoSize);
//       }

//       flag = false;
//       deltaCusto = 0;

//     } else {
//       break;
//     }
//   }

//   return custoDaSolucao;
// }

// int swap(vector<int> &solucao, int custoDaSolucaoAnterior){
//   //Inicia variáveis
//   int deltaCusto = 0;
//   int custoRetirada = 0;
//   int custoInsercao = 0;
//   int custoDaSolucao = custoDaSolucaoAnterior;
//   int solucaoSize = solucao.size();
//   bool flag = false;
//   tSwap swap;

//   while(1){
//     //Aplica reinserção
//     for(size_t i = 1; i < solucaoSize - 3; i++){
//       for(size_t y = i+2; y < solucaoSize - 1; y++){
//         custoRetirada = matrizAdj[solucao[i-1]][solucao[i]] + matrizAdj[solucao[i]][solucao[i+1]] +
//                         matrizAdj[solucao[y-1]][solucao[y]] + matrizAdj[solucao[y]][solucao[y+1]];

//         custoInsercao = matrizAdj[solucao[i-1]][solucao[y]] + matrizAdj[solucao[y]][solucao[i+1]] +
//                         matrizAdj[solucao[y-1]][solucao[i]] + matrizAdj[solucao[i]][solucao[y+1]];

//         if((custoInsercao - custoRetirada) < deltaCusto){
//           flag = true;

//           deltaCusto = custoInsercao - custoRetirada;
//           swap.pos1 = i;
//           swap.vertice1 = solucao[i];
//           swap.pos2 = y;
//           swap.vertice2 = solucao[y];
//         }
//       }
//     }

//     if(flag){
//       custoDaSolucao = custoDaSolucao + deltaCusto;

//       solucao.erase(solucao.begin() + swap.pos2);
//       solucao.emplace(solucao.begin() + swap.pos2, swap.vertice1);

//       solucao.erase(solucao.begin() + swap.pos1);
//       solucao.emplace(solucao.begin() + swap.pos1, swap.vertice2);

//       flag = false;
//       deltaCusto = 0;

//     } else {
//       break;

//     }
//   }

//   return custoDaSolucao;
// }

// int twoOptN(vector<int> &solucao, int custoDaSolucaoAnterior){
//   //Inicia variáveis
//   int deltaCusto = 0;
//   int custoRetirada = 0;
//   int custoInsercao = 0;
//   int custoDaSolucao = custoDaSolucaoAnterior;
//   int aux = 0;
//   int solucaoSize = solucao.size();
//   int sizeSwap;
//   bool flag = false;
//   tSwap swap;

//   while(1){
//     for(size_t i = 1; i < solucaoSize - 3; i++){
//       for(size_t y = i + 3; y < solucaoSize; y++){
//         custoRetirada = matrizAdj[solucao[i]][solucao[i+1]] + matrizAdj[solucao[y-1]][solucao[y]];
//         custoInsercao = matrizAdj[solucao[i]][solucao[y-1]] + matrizAdj[solucao[i+1]][solucao[y]];

//         if((custoInsercao - custoRetirada) < deltaCusto){
//           flag = true;

//           deltaCusto = custoInsercao - custoRetirada;
//           swap.pos1 = i + 1;
//           swap.vertice1 = solucao[i];
//           swap.pos2 = y - 1;
//           swap.vertice2 = solucao[y];
//         }
//       }
//     }

//     if(flag){
//       custoDaSolucao = custoDaSolucao + deltaCusto;

//       sizeSwap = swap.pos2 - swap.pos1;
//       for(size_t i = 0; i < sizeSwap; i++){
//         aux = solucao[swap.pos2];
//         solucao.erase(solucao.begin() + swap.pos2);
//         solucao.emplace(solucao.begin() + swap.pos1 + i, aux);
//       }

//       flag = false;
//       deltaCusto = 0;

//     } else {
//       break;
//     }
//   }

//   return custoDaSolucao;
// }

// //Vizinhança
// vector<int> reinsercaoAleatoria(vector<int> solucao, int *custoParcial, int custo){
//   random_device rd;
//   mt19937 mt(rd());
//   uniform_int_distribution<int> linear_i(1, solucao.size()-2);
//   int custoRetirada = 0, custoReinsercao = 0;
//   int randomRetirada = 0, randomInsercao = 0;
//   int vertice;

//   while(randomInsercao == randomRetirada){
//     randomRetirada = linear_i(mt);
//     randomInsercao = linear_i(mt);
//   }

//   vertice = solucao[randomRetirada];

//   custoRetirada = matrizAdj[solucao[randomRetirada-1]][solucao[randomRetirada+1]] -
//                  (matrizAdj[solucao[randomRetirada-1]][vertice] + matrizAdj[vertice][solucao[randomRetirada+1]]);

//   solucao.erase(solucao.begin() + randomRetirada);
//   solucao.emplace(solucao.begin() + randomInsercao, vertice);

//   custoReinsercao = (matrizAdj[solucao[randomInsercao-1]][vertice] + matrizAdj[vertice][solucao[randomInsercao+1]]) -
//                      matrizAdj[solucao[randomInsercao-1]][solucao[randomInsercao+1]];


//   *custoParcial = custo + (custoReinsercao + custoRetirada);

//   return solucao;
// }

// //Pertubações
// int doubleBridge(vector<int> &solucao, int custoDaSolucaoAnterior){
//   int sizeBlock = (solucao.size() - 2) / 3;
//   int aux1 = 0;
//   int aux2 = 0;
//   int custoInicial;
//   int custoFinal;
//   int deltaCusto;

//   custoInicial = matrizAdj[solucao[0]][solucao[1]] +
//                   matrizAdj[solucao[sizeBlock]][solucao[sizeBlock + 1]] +
//                   matrizAdj[solucao[sizeBlock*2]][solucao[sizeBlock*2 + 1]] +
//                   matrizAdj[solucao[sizeBlock*3]][solucao[sizeBlock*3 + 1]];

//   for(size_t i = 0; i < sizeBlock; i++){
//     aux1 = solucao[i+1];
//     aux2 = solucao[i+1+(sizeBlock*2)];

//     solucao.erase(solucao.begin() + i + 1);
//     solucao.emplace(solucao.begin() + i + 1, aux2);

//     solucao.erase(solucao.begin() + i + 1 + (sizeBlock*2));
//     solucao.emplace(solucao.begin() + i + 1 + (sizeBlock*2), aux1);
//   }

//   custoFinal = matrizAdj[solucao[0]][solucao[1]] +
//                 matrizAdj[solucao[sizeBlock]][solucao[sizeBlock + 1]] +
//                 matrizAdj[solucao[sizeBlock*2]][solucao[sizeBlock*2 + 1]] +
//                 matrizAdj[solucao[sizeBlock*3]][solucao[sizeBlock*3 + 1]];

//   deltaCusto = custoFinal - custoInicial;

//   return custoDaSolucaoAnterior + deltaCusto;
// }

// // Untils
// void printData() {
//   cout << endl << "dimension: " << dimension << endl;
//   for (size_t i = 1; i <= dimension; i++) {
//     for (size_t j = 1; j <= dimension; j++) {
//       cout << matrizAdj[i][j] << " ";
//     }
//     cout << endl;
//   }
// }

// void printSolucao(vector<int> &solucao, int tamanhoArray){
//   cout << endl << "Solucao: [ ";

//   for(size_t i = 0; i < solucao.size(); i++){
//     cout << solucao[i] << " ";
//   }

//   cout << "]" << endl;
// }

// void custoSolucao(int *custoTotal, vector<int> solucao, int tamanhoArray) {
//   *custoTotal = 0;

//   for(size_t i = 0; i < solucao.size()-1; i++){
//     *custoTotal += matrizAdj[solucao[i]][solucao[i + 1]];
//   }
// }

// bool compareByCost(const tInsercao &data1, const tInsercao &data2){
//   return data1.custo < data2.custo;
// }

// int obtemTemperaturaInicial(vector<int> &solucao, int custo, int beta, float gama, int iteracoesPorTemperatura, int temperaturaInicial){
//   //Declara variáveis aleatórias
//   random_device rd;
//   mt19937 mt(rd());
//   uniform_real_distribution<float> linear_f1(0.0, 1.0);
//   //uniform_real_distribution<float> linear_f2(0.0, 0.5);

//   vector<int> solucaoParcial;

//   //Declara variáveis
//   float x;
//   float alpha;
//   int randomInsercao = 0, randomRetirada = 0;
//   int temperatura = temperaturaInicial;
//   int vizinhosAceitos;
//   int deltaCusto;
//   int custoParcial = custo;
//   int coleta = 1, deposito = 1;

//   // for (size_t i = 0; i < solucao.size(); i++) {
//   //   solucaoParcial.push_back(solucao[i]);
//   // }

//   while(1){
//     //Zera quantidade de vizinhos aceitos
//     vizinhosAceitos = 0;

//     for (size_t i = 1; i <= iteracoesPorTemperatura; i++) {
//       //Gera vizinho aleatório
//       // alpha = linear_f2(mt);
//       // custoParcial = construtivo(solucao, coleta, deposito, alpha);

//       solucaoParcial = reinsercaoAleatoria(solucao, &custoParcial, custo);

//       //Calcula delta custo
//       deltaCusto = custoParcial - custo;

//       //Verifica se houve melhora
//       if(deltaCusto < 0){
//         //Adiciona vizinho aceito
//         vizinhosAceitos++;
//       } else {
//         //Calcula probabilidade de aceitar vizinho pior
//         x = linear_f1(mt);
//         if(x < exp(-(deltaCusto) / temperatura) ){
//           vizinhosAceitos++;
//         }
//       }
//     }

//     if(vizinhosAceitos >= gama * iteracoesPorTemperatura){
//       break;
//     } else {
//       temperatura *= beta;
//     }
//   }

//   return temperatura;
// }

// // int solucaoAleatoria(vector<int> &solucao, int custo, int coleta, int deposito){
// //   vector<int> listaVertices;
// //
// //   for (size_t i = 1; i <= dimension; i++) {
// //     if(i == coleta || i == deposito) continue;
// //     listaVertices.push_back(i);
// //   }
// //
// //   solucao.clear();
// //   solucao.push_back(coleta);
// //
// //   for (size_t i = 0; i < dimension; i++) {
// //     listaVertices.push_back(i);
// //   }
// //
// //   solucao.push_back(deposito);
// // }
