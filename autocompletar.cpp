#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;

// Classe Lista
template <typename T>
class Lista {
protected:
    vector<T> elementos;

public:
    void inserir(const T& elemento) {
        elementos.push_back(elemento);
    }

    size_t tamanho() const {
        return elementos.size();
    }

    T& operator[](size_t index) {
        return elementos[index];
    }

    void imprimir() const {
        for (const auto& elemento : elementos) {
            cout << elemento << endl;
        }
    }
};

// Classe ListaOrdenada
template <typename T>
class ListaOrdenada : public Lista<T> {
public:
    void ordenar() {
        sort(this->elementos.begin(), this->elementos.end());
    }

    void ordenarPorFuncao(bool (*funcaoComparacao)(const T&, const T&)) {
        sort(this->elementos.begin(), this->elementos.end(), funcaoComparacao);
    }
};

// Classe Termo
class Termo {
private:
    string termo;
    long peso;

public:
    Termo() : termo(""), peso(0) {}
    Termo(string termo, long peso) : termo(termo), peso(peso) {}

    string getTermo() const { return termo; }
    long getPeso() const { return peso; }

    static int compararPeloPeso(Termo t1, Termo t2) {
        return t2.peso - t1.peso; // Decrescente
    }

    static int compararPeloPrefixo(Termo t1, Termo t2, int r) {
        string prefixo1 = t1.termo.substr(0, r);
        string prefixo2 = t2.termo.substr(0, r);
        if (prefixo1 > prefixo2) return 1;
        if (prefixo1 < prefixo2) return -1;
        return 0;
    }

    bool operator<(const Termo& outro) const {
        return termo < outro.termo;
    }

    friend ostream& operator<<(ostream& out, const Termo& t) {
        out << t.peso << "\t" << t.termo;
        return out;
    }
};

// Classe Autocompletar
class Autocompletar {
private:
    ListaOrdenada<Termo> termos;

    int buscaBinaria(const string& prefixo, int r) {
        int inicio = 0, fim = termos.tamanho() - 1;
        while (inicio <= fim) {
            int meio = (inicio + fim) / 2;
            string termoAtual = termos[meio].getTermo().substr(0, r);
            if (termoAtual < prefixo)
                inicio = meio + 1;
            else if (termoAtual > prefixo)
                fim = meio - 1;
            else
                return meio;
        }
        return -1;
    }

public:
    void carregarTermos(const string& arquivo) {
        ifstream inFile(arquivo);
        string linha, termo;
        long peso;

        while (getline(inFile, linha)) {
            linha.erase(0, linha.find_first_not_of(" \t"));
            linha.erase(linha.find_last_not_of(" \t") + 1);

            if (linha.empty()) continue;

            size_t tabPos = linha.find('\t');
            if (tabPos == string::npos) continue;

            peso = stol(linha.substr(0, tabPos));
            termo = linha.substr(tabPos + 1);

            if (!termo.empty()) {
                termos.inserir(Termo(termo, peso));
            }
        }

        termos.ordenar();
    }

    vector<Termo> procurar(const string& prefixo, int k) {
        int r = prefixo.size();
        int indice = buscaBinaria(prefixo, r);
        if (indice == -1) return {};

        vector<Termo> resultados;
        for (int i = indice; i < termos.tamanho(); ++i) {
            if (termos[i].getTermo().substr(0, r) == prefixo)
                resultados.push_back(termos[i]);
            else
                break;
        }

        sort(resultados.begin(), resultados.end(),
             [](const Termo& t1, const Termo& t2) { return t2.getPeso() - t1.getPeso(); });

        if (resultados.size() > k) resultados.resize(k);
        return resultados;
    }
};

// Função principal
int main(int argc, char* argv[]) {
    string arquivo = argv[1];
    int k = stoi(argv[2]);

    Autocompletar autocompletar;
    autocompletar.carregarTermos(arquivo);

    while (true) {
        string prefixo;
        cout << "Entre com o termo a ser auto-completado (digite \"sair\" para encerrar): ";
        getline(cin, prefixo);

        if (prefixo == "sair") break;

        vector<Termo> resultados = autocompletar.procurar(prefixo, k);
        for (const auto& termo : resultados) {
            cout << termo << endl;
        }
    }

    return 0;
}
