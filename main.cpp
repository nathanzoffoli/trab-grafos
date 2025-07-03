#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include "Gerenciador.h"

std::vector<std::string> splitString(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <arquivo_grafo.txt>\n";
        return 1;
    }
    
    std::ifstream arquivo(argv[1]);
    if (!arquivo.is_open()) {
        std::cerr << "Erro ao abrir arquivo!\n";
        return 1;
    }
    
    std::vector<char> dados;
    std::string linha;
    
    // Ler tipo do grafo (3 primeiros caracteres)
    std::getline(arquivo, linha);
    for (char c : linha) {
        if (c != ' ') dados.push_back(c);
    }
    
    // Ler ordem do grafo
    std::getline(arquivo, linha);
    dados.push_back(linha[0]);
    int ordem = linha[0] - '0';
    
    // Ler vértices
    for (int i = 0; i < ordem; i++) {
        std::getline(arquivo, linha);
        dados.push_back(linha[0]);
        if (dados[2] == '1') { // Se for ponderado nos vértices
            dados.push_back(linha[2]);
        }
    }
    
    // Ler arestas
    while (std::getline(arquivo, linha)) {
        std::istringstream iss(linha);
        char origem, destino;
        iss >> origem >> destino;
        dados.push_back(origem);
        dados.push_back(destino);
        if (dados[1] == '1') { // Se for ponderado nas arestas
            int peso;
            iss >> peso;
            dados.push_back(peso + '0');
        }
    }
    
    arquivo.close();
    
    Grafo* grafo = new Grafo(dados);
    Gerenciador::comandos(grafo);
    
    delete grafo;
    return 0;
}