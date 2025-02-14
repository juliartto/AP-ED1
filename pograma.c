#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>

// MACROS ---------------------------------------
// Macro LER_DATA, evita repetição desnecessária na leitura de datas no formato DD/MM/AAAA
#define LER_DATA(mensagem, data) do { \
    printf(mensagem); \
    scanf("%d/%d/%d", &data.dia, &data.mes, &data.ano); \
} while (0)

// Colocar os headers aqui depois seria bom (ass paludeto)
bool ordemNome(char *source,  const char *novo);

// ESTRUTURAS -----------------------------------
// struct de datas
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

// LISTA DUPLAMENTE ENCADEADA -------------------
typedef struct _NoLista {
    _Paciente *p;
    struct _NoLista *prox;
    struct _NoLista *ant;
} _NoLista;

typedef struct _ListaDupla {
    _NoLista *inicio;
    _NoLista *fim;
    int qtdeElementos; 
} _ListaDupla;

bool listaVazia(_ListaDupla *lista) {

    return lista->qtdeElementos == 0;

}

void inicializaLista(_ListaDupla *lista) {

    lista->inicio = lista->fim = NULL;
    lista->qtdeElementos = 0;

}

void destroiLista(_ListaDupla *lista) {

    _NoLista *atual = lista->inicio;

    while (atual->prox != NULL) {
        _NoLista *temp = atual->prox;
        free(atual);
        atual = temp;
    }

}

void insereLista(_ListaDupla *lista, _Paciente *p) {

    _NoLista *novo = (_NoLista*) malloc(sizeof(_NoLista));

    if (novo == NULL) {
        perror("Erro ao alocar memória\n");
        return;
    }

    novo->p = p;
    novo->prox = novo->ant = NULL;

    if (listaVazia(lista)) {
        lista->inicio = lista->fim = novo;
        lista->qtdeElementos++;
        return;
    }

    _NoLista *aux = lista->inicio;

    // Se o nome vem antes do inicio da lista
    if (ordemNome(novo->p->nome, lista->inicio->p->nome)) {
        novo->prox = lista->inicio;
        lista->inicio->ant = novo;
        lista->inicio = novo;
        lista->qtdeElementos++;
        return;
    }

    // Para inserir no meio da lista, percorrer
    while (aux->prox != NULL && ordemNome(aux->prox->p->nome, novo->p->nome)) {
        aux = aux->prox;
    }

    // Para inserir no final da lista (achou o no sentinela)
    if (aux->prox == NULL) {
        aux->prox = novo;
        novo->ant = aux;
        lista->fim = novo;
    } else {    // Insere entre dois nos
        novo->prox = aux->prox;
        novo->ant = aux;
        aux->prox->ant = novo;
        aux->prox = novo;
    }

    lista->qtdeElementos++;

}


void imprimeLista(FILE *arquivo, _ListaDupla *lista) {

    _NoLista *atual = lista->inicio;

    while (atual != NULL) {

        fprintf(arquivo, "<%s, %c, %02d/%02d/%d, %02d/%02d/%d>\n", 
            atual->p->nome, atual->p->sexo, 
            atual->p->nascimento.dia, atual->p->nascimento.mes, atual->p->nascimento.ano,
            atual->p->ultimaConsulta.dia, atual->p->ultimaConsulta.mes, atual->p->ultimaConsulta.ano);

        atual = atual->prox;
    }

}

// Retorna se o NOVO nome vem ANTES da SOURCE (usa isso julia)
bool ordemNome(char *source,  const char *novo) {

    int i = 0;

    while (source[i] && novo[i]) {

        char c1 = tolower(source[i]);
        char c2 = tolower(novo[i]);

        // Se os caracteres forem diferentes, retorna a comparacao
        if (c1 != c2) {

            return c1 > c2;

        }
        
        i++;

    }

    // Strings ate aqui sao iguais (um nome eh substring do outro), decide pelo tamanho
    return strlen(source) > strlen(novo);

}
// ==============================================

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

    if ((*no)->noEsquerdo != NULL) {
        alturaEsquerda = (*no)->noEsquerdo->altura;
    } else {
        alturaEsquerda = -1;
    }

    if ((*no)->noDireito != NULL) {
        alturaDireita = (*no)->noDireito->altura;
    }else {
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

    // Obtém a data atual
    time_t t = time(NULL);
    struct tm dataAtual = *localtime(&t);

    // Cria a estrutura da data de nascimento
    struct tm dataNascimento = {0};
    dataNascimento.tm_mday = nascimento.dia;
    dataNascimento.tm_mon = nascimento.mes - 1;  // tm_mon vai de 0 a 11
    dataNascimento.tm_year = nascimento.ano - 1900; // tm_year começa desde 1900

    // Converte ambas as datas para time_t (segundos desde a epoch)
    time_t timeAtual = mktime(&dataAtual);
    time_t timeNascimento = mktime(&dataNascimento);

    // Verifica se houve erro na conversão
    if (timeNascimento == -1 || timeAtual == -1) {
        printf("Erro ao converter datas para time_t.\n");
        return -1;
    }

    // Verifica se a data de nascimento é posterior à data atual
    if (timeNascimento > timeAtual) {
        printf("Erro: A data de nascimento não pode ser posterior à data atual.\n");
        return -1;
    }

    // Calcula a idade
    int idade = dataAtual.tm_year + 1900 - nascimento.ano;

    // Ajusta se a pessoa ainda não fez aniversário neste ano
    if (dataAtual.tm_mon < nascimento.mes - 1 || 
       (dataAtual.tm_mon == nascimento.mes - 1 && dataAtual.tm_mday < nascimento.dia)) {
        idade--;
    }

    return idade;

}

int calcularDias(_Data ultimaConsulta) {

    // Inicia os structs "vazios"
    struct tm dataAtual = {0}, dataUltima = {0};

    // Obtém a data atual
    time_t t = time(NULL);
    localtime_r(&t, &dataAtual);

    // Configura a estrutura da data da última consulta
    dataUltima.tm_mday = ultimaConsulta.dia;
    dataUltima.tm_mon = ultimaConsulta.mes - 1;  // tm_mon vai de 0 a 11
    dataUltima.tm_year = ultimaConsulta.ano - 1900; // tm_year começa desde 1900

    // Converte ambas as datas para time_t (segundos desde a epoch)
    time_t timeAtual = mktime(&dataAtual);
    time_t timeUltima = mktime(&dataUltima);

    // Verifica se houve erro na conversão
    if (timeUltima == -1 || timeAtual == -1) {
        printf("Erro ao converter datas para time_t.\n");
        return -1;
    }

    // Verifica se a data da última consulta é posterior à data atual
    if (timeUltima > timeAtual) {
        printf("Erro: A data da última consulta não pode ser posterior à data atual.\n");
        return -1;
    }

    // Calcula a diferença em dias
    double diferenca = difftime(timeAtual, timeUltima) / (60 * 60 * 24);

    return (int) diferenca;

}

// ADICIONAR NOVO PACIENTE ----------------------
void adicionarPaciente(FILE *arquivo, _NoArvore **no, _ListaDupla *lista) {

    _Paciente *p = (_Paciente *) malloc(sizeof(_Paciente));

    printf("Nome do(a) paciente: ");
    fgets(p->nome, 128, stdin);  
    p->nome[strcspn(p->nome, "\n")] = '\0';

    do {
        LER_DATA("Digite a data de nascimento (DD/MM/AAAA):\n", p->nascimento);
        p->idade = calcularIdade(p->nascimento);
    } while (p->idade == -1);
    
    printf("Digite o sexo (M/F): ");
    while (p->sexo != 'M' && p->sexo != 'F') {
        scanf(" %c", &p->sexo);
    }

    do {
        LER_DATA("Digite a data da última consulta (DD/MM/AAAA): \n", p->ultimaConsulta);
        p->diasUltCon = calcularDias(p->ultimaConsulta);
    } while (p->diasUltCon == -1);

    if (p->sexo == 'F') {
        //inserirAVL(no, paciente); julia eh com vc 
    } else if (p->sexo == 'M') {
        insereLista(lista, p);
    }  

    fprintf(arquivo, "<%s, %c, %02d/%02d/%d, %02d/%02d/%d>\n", p->nome, p->sexo, p->nascimento.dia, p->nascimento.mes, p->nascimento.ano,
            p->ultimaConsulta.dia, p->ultimaConsulta.mes, p->ultimaConsulta.ano);

}

void pesquisarLista(char *nomePaciente, _ListaDupla *lista) {

    _NoLista *atual = lista->inicio;

    while (atual != NULL) {

        if (atual->p != NULL && strcmp(atual->p->nome, nomePaciente) == 0) {
            printf("%s, %d anos de idade, %d dias desde última consulta\n", atual->p->nome, atual->p->idade, atual->p->diasUltCon);
            return;
        }

        atual = atual->prox;
    }

    printf("Paciente não encontrado\n");

    return;
}

void pesquisarPaciente(char *nomePaciente, char sexoPaciente, _NoArvore **no, _ListaDupla *lista) {

    if (sexoPaciente == 'M') {
        pesquisarLista(nomePaciente, lista);
    } else if (sexoPaciente == 'F') {
        // pesquisarArvore(nomePaciente, no);
    }

}

// faz algo analogo a isso, julia
void geraRelatorioAndre(_ListaDupla *lista) {

    FILE *novoArquivo;

    novoArquivo = fopen("andre.txt", "w");

    if (novoArquivo == NULL) {
        printf("Erro ao criar relatório\n");
    }

    imprimeLista(novoArquivo, lista);
    
    fclose(novoArquivo);

}

// SEPARANDO OS DADOS DOS PACIENTES (parse) -------------
void separaDados(FILE *arquivo, _NoArvore **no, _ListaDupla *lista){

    // Tamanho do buffer p/ cada linha
    char linha[256];

    while (fgets(linha, sizeof(linha), arquivo)) {

        // Essa alocacao aqui poderia ser otimizada se utilizassemos um buffer de pacientes, porem n temos tempo pra isso
        _Paciente *p = (_Paciente *) malloc(sizeof(_Paciente));
    
        sscanf(linha, "<%[^,], %c, %d/%d/%d, %d/%d/%d>",
           p->nome, &p->sexo,
           &p->nascimento.dia, &p->nascimento.mes, &p->nascimento.ano,
           &p->ultimaConsulta.dia, &p->ultimaConsulta.mes, &p->ultimaConsulta.ano);
        
        p->idade = calcularIdade(p->nascimento);
        p->diasUltCon = calcularDias(p->ultimaConsulta);

        // pacientes mulheres são colocadas na AVL
        if (p->sexo == 'F') {
            // inserirAVL(no, p); julia
        } else if (p->sexo == 'M') {    // evita linhas vazias
            insereLista(lista, p);
        } 

    }

}
// MENU -----------------------------------------
#define ANSIcorVermelho  "\x1b[31m"
#define ANSIcorVerde     "\x1b[32m"
#define ANSIcorAmarelo   "\x1b[33m"
#define ANSIcorAzul      "\x1b[34m"
#define ANSIcorMagenta   "\x1b[35m"
#define ANSIcorCiano     "\x1b[36m"
#define ANSIcorReset     "\x1b[0m"

void limparTela() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void aguardarEnter() {
    printf("\n%sPressione Enter para continuar...%s", ANSIcorAmarelo, ANSIcorReset);
    getchar();
}

int menu(FILE *arquivo, _NoArvore **no, _ListaDupla *lista) {

    int opcao = 0;

    do {

        limparTela();
        printf("%s========================================\n", ANSIcorCiano);
        printf("               MENU PRINCIPAL            \n");
        printf("========================================%s\n", ANSIcorReset);
        printf("%s1.%s Adicionar novo paciente\n", ANSIcorVerde, ANSIcorReset);
        printf("%s2.%s Pesquisar paciente por nome\n", ANSIcorVerde, ANSIcorReset);
        printf("%s3.%s Encerrar e gerar arquivos de saída\n", ANSIcorVerde, ANSIcorReset);
        printf("%s========================================\n", ANSIcorCiano);

        printf("Escolha uma opção: %s", ANSIcorMagenta);
        scanf("%d", &opcao);
        printf(ANSIcorReset);

        while(getchar() != '\n'); // Limpar buffer

        switch(opcao) {

            case 1: {

                limparTela();
                printf("%sAdicionando um novo paciente...%s\n", ANSIcorAzul, ANSIcorReset);

                adicionarPaciente(arquivo, no, lista); // Função que adiciona um paciente na base de dados

                printf("%sPaciente adicionado com sucesso!%s\n", ANSIcorVerde, ANSIcorReset);
                aguardarEnter();
                break;

            }

            case 2: {

                limparTela();
                char nomePaciente[128];
                char sexoPaciente;
            
                // Solicita o nome do paciente
                printf("%sDigite o nome do paciente: %s", ANSIcorAzul, ANSIcorReset);
                fgets(nomePaciente, sizeof(nomePaciente), stdin);
                nomePaciente[strcspn(nomePaciente, "\n")] = '\0'; // Remove a quebra de linha
            
                // Solicita o sexo do paciente e valida a entrada
                do {
                    printf("%sDigite o sexo do paciente (M/F): %s", ANSIcorAzul, ANSIcorReset);
                    scanf(" %c", &sexoPaciente); 
                    while (getchar() != '\n');
                } while (sexoPaciente != 'M' && sexoPaciente != 'F');
            
                // Pesquisa na lista ou AVL, dependendo do sexo
                pesquisarPaciente(nomePaciente, sexoPaciente, no, lista);
            
                aguardarEnter();
                break;
            }

            case 3:

                printf("%sEncerrando o programa e gerando arquivos de saída...%s\n", ANSIcorMagenta, ANSIcorReset);
                // Gera os arquivos de saída para cada especialista
                geraRelatorioAndre(lista);
                // geraRelatorioLiz(no); // julia
                return EXIT_SUCCESS;

            default:

                printf("%sOpção inválida. Tente novamente.%s\n", ANSIcorVermelho, ANSIcorReset);
                aguardarEnter();
                break;

        }

    } while(opcao != 3);

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
    _ListaDupla lista;

    iniciarAVL(&raiz);
    inicializaLista(&lista);

    // abre o arquivo de pacientes
    FILE *arquivo = fopen(argv[1], "r+");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return 1;
    }

    // separa os dados de cada paciente e insere na estrutura correta
    separaDados(arquivo, &raiz, &lista);

    menu(arquivo, &raiz, &lista);

    fclose(arquivo);
    
    return 0;

}