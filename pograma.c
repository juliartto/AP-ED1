#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<stdbool.h>
#include <time.h>

// ESTRUTURAS -----------------------------------

// struct com os dados dos pacientes
typedef struct {
    char nome[99];
    char sexo;
    int nascimento;
    char ultimaConsulta[11];
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


// INSERIR PACIENTES NA AVL ---------------------
void inserirAVL(_NoArvore **no, _Paciente paciente){

    // cria a raíz da árvore caso ela esteja vazia
    if (estaVazia(no))
    {
        (*no) = (_NoArvore*) malloc(sizeof(_NoArvore));
        if ((*no) == NULL)
        {
            perror("Não foi possível alocar memória.\n");
            return;
        }

        (*no)->noDireito = (*no)->noEsquerdo = NULL;
        (*no)->paciente.nascimento = paciente.nascimento;
        (*no)->altura = 0;
    }
    
    // adicionar um novo nó
    if ((*no)->paciente.nascimento > paciente.nascimento)
    {
        inserir(&(*no)->noEsquerdo, paciente.nascimento);
    }else {
        inserir(&(*no)->noDireito, paciente.nascimento);
    }

    (*no)->altura = 1 + maxAlturaFilhos(no);

    int fatorBalanceamento = CalcFatorBalanceamento(no);

    switch (fatorBalanceamento)
    {
    case 2: // rotação à esquerda
        int fbFilho = CalcFatorBalanceamento(&(*no)->noDireito);

        switch (fbFilho)
        {
        case +1: // rotação simples à esquerda
            rotacaoSEsq(no);
            break;
        case -1: // rotação dupla à esquerda
            rotacaoSimplesDireita(&(*no)->noDireito);
            rotacaoSEsq(no);
            break;
        }

        break;

    case -2: // rotação à direita
        int fbFilho = CalcFatorBalanceamento(&(*no)->noEsquerdo);

        switch (fbFilho)
        {
        case -1: // rotação simples à direita
            rotacaoSimplesDireita(no);
            break;
        
        case +1: // rotação dupla à direita
            rotacaoSEsq(&(*no)->noEsquerdo);
            rotacaoSimplesDireita(no);
            break;
        }

        break;
    }
}
// ==============================================

// ADICIONAR NOVO PACIENTE ----------------------
void adicionarPaciente(_NoArvore **no){

    _Paciente paciente;
    int dia, mes, ano;

    printf("Nome do(a) paciente: ");
    fgets(paciente.nome, 100, stdin);
    paciente.nome[strcspn(paciente.nome, "\n")] = '\0';
    printf("Digite a data de nascimento (DD/MM/AAAA): ");
    scanf("%d/%d/%d", &dia, &mes, &ano);
    paciente.nascimento = dia * 1000000 + mes * 10000 + ano;
    printf("Digite o sexo (M/F): ");
    scanf(" %c", &paciente.sexo);
    printf("Digite a data da última consulta (DD/MM/AAAA): ");
    fgets(paciente.ultimaConsulta, 100, stdin);
    paciente.ultimaConsulta[strcspn(paciente.ultimaConsulta, "\n")] = '\0';

    if (paciente.sexo == 'F') {
        inserirAVL(no, paciente);
    }else
    {
        //inserirLista(); 
    }  
}


// SEPARANDO OS DADOS DOS PACIENTES -------------
void separaDados(FILE *arquivo, _NoArvore **no){

    char linha[200];
    while (fgets(linha, sizeof(linha), arquivo)) {
        _Paciente paciente;
        int dia, mes, ano;
        sscanf(linha, "<%[^,], %c, %d/%d/%d, %[^>]>", paciente.nome, &paciente.sexo, &dia, &mes, &ano, paciente.ultimaConsulta);
        paciente.nascimento = dia * 1000000 + mes * 10000 + ano; // faza data e nascimento um numero inteiro para ser usado de chave
        
        // pacientes mulheres são colocadas na AVL
        if (paciente.sexo == 'F') {
            inserirAVL(no, paciente);
        }else
        {
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