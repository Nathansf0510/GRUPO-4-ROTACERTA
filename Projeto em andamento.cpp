#include <iostream>
#include <string>
using namespace std;

const int MAX = 100;

string codigo[MAX], destino[MAX];
double peso[MAX], volume[MAX];
bool prioridade[MAX], refrigerada[MAX];
int total = 0; // quantidade de encomendas cadastradas

void cadastrar() {
    if (total == MAX) {
        cout << "Cadastro cheio\n";
        return;
    }

    string cod, dest;
    double pesoEncomenda, volumeEncomenda;
    int prioritaria, precisaRefrigeracao;

  cout << "Codigo: ";
  getline(cin, cod);
    for (int i = 0; i < total; i++) {
        if (codigo[i] == cod) {
            cout << "Codigo ja existe\n";
            return;
        }
    }
    cout << "Destino: ";
    getline(cin, dest);
    cout << "Peso (kg): ";
    cin >> pesoEncomenda;
    cout << "Volume (m3): ";
    cin >> volumeEncomenda;
    if (pesoEncomenda <= 0 || volumeEncomenda <= 0) {
        cout << "Peso e volume precisam ser positivos\n";
        return;
    }
    cout << "Prioritaria (1-Sim ou 0-Nao): ";
    cin >> prioritaria;
    cout << "Precisa de refrigeracao (1-Sim ou 0-Nao): ";
    cin >> precisaRefrigeracao;

    codigo[total] = cod;
    destino[total] = dest;
    peso[total] = pesoEncomenda;
    volume[total] = volumeEncomenda;
    prioridade[total] = (prioritaria == 1);
    refrigerada[total] = (precisaRefrigeracao == 1);
    total++;
}

// plano 'A' = ordem de cadastro, plano 'B' = menor volume primeiro
void montarPlano(char plano, string dest, double capPeso, double capVol,
    bool veiculoRefrigerado, int& qtdPrio, int& qtdTotal) {
    int ordem[MAX];
    int n = 0;
    for (int i = 0; i < total; i++) {
        if (destino[i] == dest) {
            ordem[n] = i;
            n++;
        }
    }

    // so troca quando precisa, entao mantem a ordem de cadastro nos empates
    for (int a = 0; a < n; a++) {
        for (int b = 0; b < n - 1 - a; b++) {
            int x = ordem[b];
            int y = ordem[b + 1];
            bool trocar = false;
            if (!prioridade[x] && prioridade[y]) {
                trocar = true;
            }
            else if (prioridade[x] == prioridade[y] && plano == 'B' && volume[x] > volume[y]) {
                trocar = true;
            }
            if (trocar) {
                ordem[b] = y;
                ordem[b + 1] = x;
            }
        }
    }

    string selecionadas = "", pendentes = "";
    qtdPrio = 0;
    qtdTotal = 0;

    for (int i = 0; i < n; i++) {
        int e = ordem[i];
        if (refrigerada[e] && !veiculoRefrigerado) {
            pendentes += " " + codigo[e] + " - falta de refrigeracao\n";
        }
        else if (peso[e] > capPeso) {
            pendentes += " " + codigo[e] + " - excesso de peso\n";
        }
        else if (volume[e] > capVol) {
            pendentes += " " + codigo[e] + " - excesso de volume\n";
        }
        else {
            selecionadas += " " + codigo[e] + "\n";
            capPeso -= peso[e];
            capVol -= volume[e];
            qtdTotal++;
            if (prioridade[e]) qtdPrio++;
        }
    }

    cout << "\nPlano " << plano << "\n";
    cout << "Selecionadas:\n" << selecionadas;
    cout << "Pendentes:\n" << pendentes;
    cout << "Restante: " << capPeso << " kg e " << capVol << " m3\n";
}

void planejar() {
    string dest;
    double capPeso, capVol;
    int r;

    cout << "Destino: ";
    getline(std::cin, dest);
    cout << "Capacidade (kg): ";
    cin >> capPeso;
    cout << "Capacidade (m3): ";
    cin >> capVol;
    cout << "Veiculo refrigerado (1-sim 0-nao): ";
    cin >> r;

    int prioA, totalA, prioB, totalB;
    montarPlano('A', dest, capPeso, capVol, r == 1, prioA, totalA);
    montarPlano('B', dest, capPeso, capVol, r == 1, prioB, totalB);

    cout << "\nRecomendacao: ";
    if (prioA > prioB) cout << "Plano A\n";
    else if (prioB > prioA) cout << "Plano B\n";
    else if (totalA > totalB) cout << "Plano A\n";
    else if (totalB > totalA) cout << "Plano B\n";
    else cout << "os planos sao equivalentes\n";
}

int main() {
    int opcao;
    do {
        cout << "\n1 - Cadastrar\n2 - Planejar carga\n0 - Sair\nOpcao: ";
        cin >> opcao;
        cin.ignore();
        if (opcao == 1) cadastrar();
        else if (opcao == 2) planejar();
    } while (opcao != 0);
    return 0;
}
