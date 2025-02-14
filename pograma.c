#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

// MACROS ---------------------------------------
// Macro LER_DATA, evita repetição desnecessária na leitura de datas no formato DD/MM/AAAA
#define LER_DATA(mensagem, data) do { \
    printf(mensagem); \
    scanf("%d/%d/%d", &data.dia, &data.mes, &data.ano); \
} while (0)

// ESTRUTURAS -----------------------------------
typedef struct _Data {
    int dia;
    int mes;
    int ano;
} _Data;

// struct com os dados dos pacientes
typedef struct {
    char nome[128];
    char sexo;
    int idade, diasUltCon;
    _Data nascimento;
    _Data ultimaConsulta;
} _Paciente;

// struct de cada nó da AVL
typedef struct _NoArvore {
    _Paciente paciente;
    struct _NoArvore *noEsquerdo;
    struct _NoArvore *noDireito;
    int altura;
} _NoArvore;

// OPERAÇÕES AVL ================================
// INICIAR --------------------------------------
void iniciarAVL(_NoArvore **raiz) {
    (*raiz) = NULL;
}


// ESTÁ VAZIA -----------------------------------
bool estaVazia(_NoArvore **raiz) {
    return (*raiz == NULL);
}


// ALTURA MÁXIMA --------------------------------
int maxAlturaFilhos(_NoArvore **no) {
    int alturaEsquerda;
    int alturaDireita;

    if ((*no)->noEsquerdo != NULL)
    {
        alturaEsquerda = (*no)->noEsquerdo->altura;
    }else
    {
        alturaEsquerda = -1;
    }

    if ((*no)->noDireito != NULL)
    {
        alturaDireita = (*no)->noDireito->altura;
    }else
    {
        alturaDireita = -1;
    }

    return alturaEsquerda > alturaDireita ? alturaEsquerda : alturaDireita;
}

//CALCULA O FATOR DE BALANCEAMENTO --------------
int CalcFatorBalanceamento(_NoArvore **no) {
    int alturaEsquerda, AlturaDireita;

    AlturaDireita = (*no)->noDireito != NULL ? (*no)->noDireito->altura : -1;
    alturaEsquerda = (*no)->noEsquerdo != NULL ? (*no)->noEsquerdo->altura : -1;
    return AlturaDireita - alturaEsquerda;
}


// ROTAÇÃO SIMPLES ESQUERDA ---------------------
void rotacaoSEsq(_NoArvore **no) {
     _NoArvore *aux = (*no)->noDireito;

    (*no)->noDireito = aux->noEsquerdo;
    aux->noEsquerdo = (*no);
    (*no) = aux;
    (*no)->noEsquerdo->altura = 1 + maxAlturaFilhos(&(*no)->noEsquerdo);
    (*no)->altura = 1 + maxAlturaFilhos(no);
}


// ROTAÇÃO SIMPLES DIREITA ----------------------
void rotacaoSimplesDireita(_NoArvore **no) {
    _NoArvore *aux=(*no)->noEsquerdo;
    (*no)->noEsquerdo = aux->noDireito;
    aux->noDireito = (*no);
    (*no) = aux;
    (*no)->noDireito->altura = 1 + maxAlturaFilhos(&(*no)->noDireito);
    (*no)->altura = 1 + maxAlturaFilhos(no);

}

// dps vc termina aqui, deixei comentado pro programa compilar. ass: paludeto

// INSERIR PACIENTES NA AVL ---------------------
// void inserirAVL(_NoArvore **no, _Paciente paciente){

//     // cria a raíz da árvore caso ela esteja vazia
//     if (estaVazia(no))
//     {
//         (*no) = (_NoArvore*) malloc(sizeof(_NoArvore));
//         if ((*no) == NULL)
//         {
//             perror("Não foi possível alocar memória.\n");
//             return;
//         }

//         (*no)->noDireito = (*no)->noEsquerdo = NULL;
//         (*no)->paciente.nascimento = paciente.nascimento;
//         (*no)->altura = 0;
//     }
    
//     // eh por ordem alfabetica, tem que checar os valores ascii, ass: paludeto
//     // adicionar um novo nó
//     // if ((*no)->paciente.nascimento > paciente.nascimento)
//     // {
//     //     inserir(&(*no)->noEsquerdo, paciente.nascimento);
//     // }else {
//     //     inserir(&(*no)->noDireito, paciente.nascimento);
//     // }

//     (*no)->altura = 1 + maxAlturaFilhos(no);

//     int fatorBalanceamento = CalcFatorBalanceamento(no);

//     switch (fatorBalanceamento)
//     {
//     case 2: // rotação à esquerda
//         int fbFilho = CalcFatorBalanceamento(&(*no)->noDireito);

//         switch (fbFilho)
//         {
//         case +1: // rotação simples à esquerda
//             rotacaoSEsq(no);
//             break;
//         case -1: // rotação dupla à esquerda
//             rotacaoSimplesDireita(&(*no)->noDireito);
//             rotacaoSEsq(no);
//             break;
//         }

//         break;

//     case -2: // rotação à direita
//         int fbFilho = CalcFatorBalanceamento(&(*no)->noEsquerdo);

//         switch (fbFilho)
//         {
//         case -1: // rotação simples à direita
//             rotacaoSimplesDireita(no);
//             break;
        
//         case +1: // rotação dupla à direita
//             rotacaoSEsq(&(*no)->noEsquerdo);
//             rotacaoSimplesDireita(no);
//             break;
//         }

//         break;
//     }
// }
// ==============================================

// CALCULO IDADE
int calcularIdade(_Data nascimento) {

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    int idade = tm.tm_year + 1900 - nascimento.ano;

    if (tm.tm_mon < nascimento.mes || (tm.tm_mon == nascimento.mes && tm.tm_mday < nascimento.dia)) {
        idade--;
    }

    return idade;

}

int calcularDias(_Data ultimaConsulta) {

    struct tm data_atual = {0}, data_ultima = {0};

    // Data atual
    time_t t = time(NULL);
    localtime_r(&t, &data_atual);

    // Data da última consulta
    data_ultima.tm_mday = ultimaConsulta.dia;
    data_ultima.tm_mon = ultimaConsulta.mes - 1;  // tm_mon vai de 0 a 11
    data_ultima.tm_year = ultimaConsulta.ano - 1900; // tm_year começa desde 1900

    // Converte ambas as datas para time_t
    time_t time_atual = mktime(&data_atual);
    time_t time_ultima = mktime(&data_ultima);

    if (time_ultima == -1 || time_atual == -1) {
        printf("Erro ao converter datas para time_t.\n");
        return -1;
    }

    // Calcula a diferença em dias
    double diferenca = difftime(time_atual, time_ultima) / (60 * 60 * 24);

    return (int)diferenca;
     
}

// nao esquecer de adicionar no .txt tbm, meter um fprintf, mudanca precisa ser persistente. ass: paludeto
// ADICIONAR NOVO PACIENTE ----------------------
void adicionarPaciente(_NoArvore **no){

    _Paciente paciente;

    printf("Nome do(a) paciente: ");
    fgets(paciente.nome, 128, stdin);  
    paciente.nome[strcspn(paciente.nome, "\n")] = '\0';

    LER_DATA("Digite a data de nascimento (DD/MM/AAAA):\n", paciente.nascimento);
    paciente.idade = calcularIdade(paciente.nascimento);

    printf("Digite o sexo (M/F): ");
    scanf("%c", &paciente.sexo);

    printf("Digite a data da última consulta (DD/MM/AAAA): ");
    LER_DATA("Digite a data de nascimento (DD/MM/AAAA):\n", paciente.ultimaConsulta);
    paciente.diasUltCon = calcularDias(paciente.ultimaConsulta);

    if (paciente.sexo == 'F') {
        // inserirAVL(no, paciente);
    }else {
        //inserirLista(); 
    }  
}


// SEPARANDO OS DADOS DOS PACIENTES -------------
// inserir parametro lista (paludeto)
void separaDados(FILE *arquivo, _NoArvore **no){

    // Tamanho do buffer p/ cada linha
    char linha[256];

    while (fgets(linha, sizeof(linha), arquivo)) {

        _Paciente p;
    
        sscanf(linha, "<%[^,], %c, %d/%d/%d, %d/%d/%d>",
           p.nome, &p.sexo,
           &p.nascimento.dia, &p.nascimento.mes, &p.nascimento.ano,
           &p.ultimaConsulta.dia, &p.ultimaConsulta.mes, &p.ultimaConsulta.ano);
        
        p.idade = calcularIdade(p.nascimento);
        p.diasUltCon = calcularDias(p.ultimaConsulta);

        // pacientes mulheres são colocadas na AVL
        if (p.sexo == 'F') {
            // inserirAVL(no, p);
        } else {
            //inserirLista(); 
        }  

    }

}


// MAIN -----------------------------------------
int main(int argc, char *argv[]) {

    // verifica erro na passagem dos argumentos para a main
     if (argc != 2) {
        perror("Erro ao utilizar o arquivo desejado.");
        return 1;
    }

    // inicia a árvore AVL
    _NoArvore *raiz;
    iniciarAVL(&raiz);

    // abre o arquivo de pacientes
    FILE *arquivo = fopen(argv[1], "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return 1;
    }

    // separa os dados de cada paciente e insere na estrutura correta
    separaDados(arquivo, &raiz);

    fclose(arquivo);
    
    return 0;
}