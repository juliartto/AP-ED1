#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>

// MACROS =======================================
// Macro LER_DATA, evita repetição desnecessária na leitura de datas no formato DD/MM/AAAA
#define LER_DATA(mensagem, data) do { \
    printf(mensagem); \
    scanf("%d/%d/%d", &data.dia, &data.mes, &data.ano); \
} while (0)


// HEADERS ======================================
bool ordemNome(char *source,  const char *novo);

// FIM HEADERS ==================================


// ESTRUTURAS ===================================
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

// struct de cada nó da AVL
typedef struct _NoArvore {
    _Paciente paciente;
    struct _NoArvore *noEsquerdo;
    struct _NoArvore *noDireito;
    int altura;
} _NoArvore;

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
//  FIM DAS ESTRUTURAS ==========================


// LISTA DUPLAMENTE ENCADEADA ===================
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


void pesquisarLista(_ListaDupla *lista) {

    char nomePaciente[128];
    printf("Digite o nome do paciente para consulta: ");
    fgets(nomePaciente, sizeof(nomePaciente), stdin);
    nomePaciente[strcspn(nomePaciente, "\n")] = '\0';

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
// FIM DAS OPERAÇÕES DA LISTA =============================

// Retorna se o NOVO nome vem ANTES da SOURCE
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

// INSERIR PACIENTES NA AVL ---------------------
void inserirAVL(_NoArvore **no, _Paciente *paciente) {

    if (*no == NULL) {
        *no = (_NoArvore*) malloc(sizeof(_NoArvore));
        if (*no == NULL) {
            perror("Não foi possível alocar memória.\n");
            return;
        }

        (*no)->noDireito = (*no)->noEsquerdo = NULL;
        (*no)->paciente = *paciente;
        (*no)->altura = 0;

    } else {
        if (ordemNome((*no)->paciente.nome, paciente->nome)) {
            inserirAVL(&(*no)->noEsquerdo, paciente);
        } else {
            inserirAVL(&(*no)->noDireito, paciente);
        }

        (*no)->altura = 1 + maxAlturaFilhos(no);
        int fatorBalanceamento = CalcFatorBalanceamento(no);

        if (fatorBalanceamento > 1) {
            if (CalcFatorBalanceamento(&(*no)->noDireito) < 0) {
                rotacaoSimplesDireita(&(*no)->noDireito);
            }
            rotacaoSEsq(no);
        } else if (fatorBalanceamento < -1) {
            if (CalcFatorBalanceamento(&(*no)->noEsquerdo) > 0) {
                rotacaoSEsq(&(*no)->noEsquerdo);
            }
            rotacaoSimplesDireita(no);
        }
    }
}


// utilizado na função de deletar um nó
_NoArvore* menorFilho(_NoArvore *no) {
    _NoArvore *atual = no;
    while (atual->noEsquerdo != NULL) {
        atual = atual->noEsquerdo;
    }
    return atual;
}


void removerAVL(_NoArvore **no, const char *nomePaciente){
    if (estaVazia(no))
    {
        printf("Paciente não encontrado.\n");
        return;
    }

    if (strcmp(nomePaciente, (*no)->paciente.nome) < 0) {
        removerAVL(&(*no)->noEsquerdo, nomePaciente);
    }
    else if (strcmp(nomePaciente, (*no)->paciente.nome) > 0) {
        removerAVL(&(*no)->noDireito, nomePaciente);
    }

    else {
        // nó com um filho ou sem filhos
        if (((*no)->noEsquerdo == NULL) || ((*no)->noDireito == NULL)) {
            _NoArvore* aux = (*no)->noEsquerdo ? (*no)->noEsquerdo : (*no)->noDireito;

            // no não tem filhos
            if (aux == NULL) {
                aux = (*no);
                aux = NULL;
            } else 
            { // no tem um filho
                (*no) = aux; 
            }
            free(aux);
        } else {
            // nó com 2 filhos
            _NoArvore* aux = menorFilho((*no)->noDireito);

            (*no)->paciente = aux->paciente;
            removerAVL(&(*no)->noDireito, aux->paciente.nome);
        }
    }

    (*no)->altura = 1 + maxAlturaFilhos(no);
    int fatorBalanceamento = CalcFatorBalanceamento(no);

    if (fatorBalanceamento < -1 && CalcFatorBalanceamento(&(*no)->noEsquerdo) <= 0) {
        rotacaoSimplesDireita(no);
        return;
    }

    if (fatorBalanceamento < -1 && CalcFatorBalanceamento(&(*no)->noEsquerdo) > 0) {
        rotacaoSEsq(&(*no)->noEsquerdo);
        rotacaoSimplesDireita(no);
        return;
    }

    if (fatorBalanceamento > 1 && CalcFatorBalanceamento(&(*no)->noDireito) >= 0) {
        rotacaoSEsq(no);
        return;
    }

    if (fatorBalanceamento > 1 && CalcFatorBalanceamento(&(*no)->noDireito) < 0) {
        rotacaoSimplesDireita(&(*no)->noDireito);
        rotacaoSEsq(no);
        return;
    } 
}


void imprimirAVL(_NoArvore **no, FILE *pacientesLiz) {
    if ((*no) != NULL) {
        imprimirAVL(&(*no)->noEsquerdo, pacientesLiz);
        fprintf(pacientesLiz, "<%s, %c, %02d/%02d/%d, %02d/%02d/%d>\n",
            (*no)->paciente.nome, (*no)->paciente.sexo, 
            (*no)->paciente.nascimento.dia, (*no)->paciente.nascimento.mes, (*no)->paciente.nascimento.ano, 
            (*no)->paciente.ultimaConsulta.dia, (*no)->paciente.ultimaConsulta.mes, (*no)->paciente.ultimaConsulta.ano);
        imprimirAVL(&(*no)->noDireito, pacientesLiz);
    }
}


void pesquisarAVL(_NoArvore **no, const char *nomePaciente) {

    if (estaVazia(no))
    {
        printf("Paciente não encontrado.\n");
        return;
    }

    if (strcmp((*no)->paciente.nome, nomePaciente) == 0) {
        printf("%s, %d anos de idade, %d dias desde última consulta\n", (*no)->paciente.nome, (*no)->paciente.idade, (*no)->paciente.diasUltCon);
        return;
    } else if (strcmp((*no)->paciente.nome, nomePaciente) > 0) {
        pesquisarAVL(&(*no)->noEsquerdo, nomePaciente);
    } else {
        pesquisarAVL(&(*no)->noDireito, nomePaciente);
    }
}
// FIM DAS OPERAÇÕES AVL ========================

// CALCULO IDADE
int calcularIdade(_Data nascimento) {

    // Verifica se a data é válida
    if (nascimento.dia < 1 || nascimento.dia > 31 ||
        nascimento.mes < 1 || nascimento.mes > 12 ||
        nascimento.ano < 1900 || nascimento.ano > 2100) {
        printf("Erro: Data de nascimento inválida.\n");
        return -1;
    }

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

int calcularDias(_Data nascimento, _Data ultimaConsulta) {

    // verificacao de validade
    if (ultimaConsulta.dia < 1 || ultimaConsulta.dia > 31 ||
        ultimaConsulta.mes < 1 || ultimaConsulta.mes > 12 ||
        ultimaConsulta.ano < 1900 || ultimaConsulta.ano > 2100) {
        printf("Erro: Data da última consulta inválida.\n");
        return -1;
    }

    if (nascimento.dia < 1 || nascimento.dia > 31 ||
        nascimento.mes < 1 || nascimento.mes > 12 ||
        nascimento.ano < 1900 || nascimento.ano > 2100) {
        printf("Erro: Data de nascimento inválida.\n");
        return -1;
    }

    struct tm dataAtual = {0}, dataUltima = {0}, dataNascimento = {0};

    time_t t = time(NULL);
    localtime_r(&t, &dataAtual);

    dataUltima.tm_mday = ultimaConsulta.dia;
    dataUltima.tm_mon = ultimaConsulta.mes - 1;  
    dataUltima.tm_year = ultimaConsulta.ano - 1900;

    dataNascimento.tm_mday = nascimento.dia;
    dataNascimento.tm_mon = nascimento.mes - 1;
    dataNascimento.tm_year = nascimento.ano - 1900;

    time_t timeAtual = mktime(&dataAtual);
    time_t timeUltima = mktime(&dataUltima);
    time_t timeNascimento = mktime(&dataNascimento);

    if (timeUltima == -1 || timeAtual == -1 || timeNascimento == -1) {
        printf("Erro ao converter datas para time_t.\n");
        return -1;
    }

    if (timeUltima > timeAtual) {
        printf("Erro: A data da última consulta não pode ser posterior à data atual.\n");
        return -1;
    }

    if (timeUltima < timeNascimento) {
        printf("Erro: A data da última consulta não pode ser anterior à data de nascimento.\n");
        return -1;
    }

    // Calcula a diferença em dias
    double diferenca = difftime(timeAtual, timeUltima) / (60 * 60 * 24);

    return (int) diferenca;
}


// ADICIONAR NOVO PACIENTE ----------------------
void adicionarPaciente(_NoArvore **no, _ListaDupla *lista) {

    _Paciente *p = (_Paciente *) malloc(sizeof(_Paciente));

    printf("Nome do(a) paciente: ");
    fgets(p->nome, 128, stdin);  
    p->nome[strcspn(p->nome, "\n")] = '\0';

    do {
        LER_DATA("Digite a data de nascimento (DD/MM/AAAA):\n", p->nascimento);
        p->idade = calcularIdade(p->nascimento);
        while (getchar() != '\n');
    } while (p->idade == -1);
    
    printf("Digite o sexo (M/F): ");
    while (p->sexo != 'M' && p->sexo != 'F') {
        scanf(" %c", &p->sexo);
        while (getchar() != '\n');
    }

    do {
        LER_DATA("Digite a data da última consulta (DD/MM/AAAA): \n", p->ultimaConsulta);
        p->diasUltCon = calcularDias(p->nascimento, p->ultimaConsulta);
        while (getchar() != '\n');
    } while (p->diasUltCon == -1);

    if (p->sexo == 'F') {
        inserirAVL(no, p);
    } else if (p->sexo == 'M') {
        insereLista(lista, p);
    }  

}

void pesquisarPaciente(char sexoPaciente, _NoArvore **no, _ListaDupla *lista) {

    if (sexoPaciente == 'M') {
        pesquisarLista(lista);
    } else if (sexoPaciente == 'F') {
        char nomePaciente[128];
        printf("Digite o nome do paciente para consulta: ");
        fgets(nomePaciente, sizeof(nomePaciente), stdin);
        nomePaciente[strcspn(nomePaciente, "\n")] = '\0';
        pesquisarAVL(no, nomePaciente);
    }

}

// ARQUIVOS DE SAÍDA ============================
void geraRelatorioAndre(_ListaDupla *lista) {

    FILE *novoArquivo;

    novoArquivo = fopen("andre.txt", "w");

    if (novoArquivo == NULL) {
        printf("Erro ao criar relatório\n");
        return;
    }

    imprimeLista(novoArquivo, lista);
    
    fclose(novoArquivo);

}

void arquivoPacientesLiz(_NoArvore **no) {

    FILE *pacientesLiz;

    pacientesLiz = fopen("PacientesLiz.txt", "w");

    if (pacientesLiz == NULL) {
        printf("Erro ao criar relatório\n");
        return;
    }

    imprimirAVL(no, pacientesLiz);
    
    fclose(pacientesLiz);

}
// ==============================================

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
        p->diasUltCon = calcularDias(p->nascimento, p->ultimaConsulta);

        if (p->sexo == 'F') {
            inserirAVL(no, p);
        } else if (p->sexo == 'M') {    // evita linhas vazias
            insereLista(lista, p);
        } 

    }

}

// MENU =========================================
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

// utilizado na alteracao de cadastro
void removeLista(_ListaDupla *lista, const char *nomePaciente) {

    if (lista == NULL || lista->inicio == NULL) {
        printf("%sA lista está vazia.%s\n", ANSIcorVermelho, ANSIcorReset);
        return;
    }

    _NoLista *atual = lista->inicio;

    while (atual != NULL) {

        if (atual->p != NULL && strcmp(atual->p->nome, nomePaciente) == 0) {
            
            // posicao do item na lista
            if (atual->ant != NULL) {
                atual->ant->prox = atual->prox;
            } else {
                lista->inicio = atual->prox;
            }

            if (atual->prox != NULL) {
                atual->prox->ant = atual->ant;
            } else {
                lista->fim = atual->ant;
            }
            
            // libera memoria do no e do paciente
            free(atual->p);
            free(atual);

            printf("%sPaciente removido com sucesso.%s\n", ANSIcorVerde, ANSIcorReset);
            return;

        }

        atual = atual->prox;
    }

    printf("%sPaciente não encontrado.%s\n", ANSIcorVermelho, ANSIcorReset);

}

// altera apenas dentro da lista
void alteraCadastroLista(_ListaDupla *lista) {

    limparTela();
    
    char nomePaciente[128];
    printf("%sDigite o nome do paciente para alterar o cadastro: %s", ANSIcorAzul, ANSIcorReset);
    fgets(nomePaciente, sizeof(nomePaciente), stdin);
    nomePaciente[strcspn(nomePaciente, "\n")] = '\0';

    _NoLista *atual = lista->inicio;
    
    while (atual != NULL) {

        if (atual->p != NULL && strcmp(atual->p->nome, nomePaciente) == 0) {

            // cria uma cópia dos dados do paciente
            _Paciente *novoPaciente = (_Paciente *)malloc(sizeof(_Paciente));
            if (!novoPaciente) {
                printf("%sErro de alocação de memória.%s\n", ANSIcorVermelho, ANSIcorReset);
                return;
            }

            *novoPaciente = *(atual->p); // copia os dados do paciente

            int opcao;

            do {

                limparTela();
                printf("%sAlterar Cadastro:%s\n", ANSIcorCiano, ANSIcorReset);
                printf("1 - Nome\n2 - Data de nascimento\n3 - Última consulta\n4 - Voltar\n");
                printf("Escolha uma opção: ");
                scanf("%d", &opcao);
                while (getchar() != '\n');

                switch (opcao) {
                    case 1:
                        printf("Novo nome: ");
                        fgets(novoPaciente->nome, sizeof(novoPaciente->nome), stdin);
                        novoPaciente->nome[strcspn(novoPaciente->nome, "\n")] = '\0';

                        break;
                    case 2:
                        do {
                            printf("Nova data de nascimento (DD/MM/AAAA): ");
                            scanf("%d/%d/%d", &novoPaciente->nascimento.dia, &novoPaciente->nascimento.mes, &novoPaciente->nascimento.ano);
                            novoPaciente->idade = calcularIdade(novoPaciente->nascimento);
                        } while (novoPaciente->idade == -1);
                        while (getchar() != '\n');

                        break;
                    case 3:
                        do {
                            printf("Nova data da última consulta (DD/MM/AAAA): ");
                            scanf("%d/%d/%d", &novoPaciente->ultimaConsulta.dia, &novoPaciente->ultimaConsulta.mes, &novoPaciente->ultimaConsulta.ano);
                            novoPaciente->diasUltCon = calcularDias(novoPaciente->nascimento, novoPaciente->ultimaConsulta);
                        } while (novoPaciente->diasUltCon == -1);
                        while (getchar() != '\n');

                        break;
                    case 4:
                        break;
                    default:
                        printf("%sOpção inválida!%s\n", ANSIcorVermelho, ANSIcorReset);
                        aguardarEnter();
                }

            } while (opcao != 4);

            // remove o paciente antigo para adicionar o novo
            removeLista(lista, nomePaciente);

            // insere a cópia alterada na lista
            insereLista(lista, novoPaciente);

            printf("%sCadastro alterado com sucesso!%s\n", ANSIcorVerde, ANSIcorReset);

            return;

        }

        atual = atual->prox;
    }

    printf("%sPaciente não encontrado.%s\n", ANSIcorVermelho, ANSIcorReset);
    aguardarEnter();

}

// altera apenas dentro da AVL
void alterarCadastroAVL(_NoArvore **no){

    limparTela();
    
    char nomePaciente[128];
    printf("%sDigite o nome do paciente para alterar o cadastro: %s", ANSIcorAzul, ANSIcorReset);
    fgets(nomePaciente, sizeof(nomePaciente), stdin);
    nomePaciente[strcspn(nomePaciente, "\n")] = '\0';

    _NoArvore *atual = *no;
    
    while (atual != NULL) {
        if (strcmp(atual->paciente.nome, nomePaciente) == 0) {

            // cria uma cópia dos dados do paciente
            _Paciente novoPaciente = atual->paciente;

            int opcao;

            do {
                limparTela();
                printf("%sAlterar Cadastro:%s\n", ANSIcorCiano, ANSIcorReset);
                printf("1 - Nome\n2 - Data de nascimento\n3 - Última consulta\n4 - Voltar\n");
                printf("Escolha uma opção: ");
                scanf("%d", &opcao);
                while (getchar() != '\n');

                switch (opcao) {
                    case 1:
                        printf("Novo nome: ");
                        fgets(novoPaciente.nome, sizeof(novoPaciente.nome), stdin);
                        novoPaciente.nome[strcspn(novoPaciente.nome, "\n")] = '\0';
                        break;
                    case 2:
                        do {
                            printf("Nova data de nascimento (DD/MM/AAAA): ");
                            scanf("%d/%d/%d", &novoPaciente.nascimento.dia, &novoPaciente.nascimento.mes, &novoPaciente.nascimento.ano);
                            novoPaciente.idade = calcularIdade(novoPaciente.nascimento);
                        } while (novoPaciente.idade == -1);
                        while (getchar() != '\n');
                        break;
                    case 3:
                        do {
                            printf("Nova data da última consulta (DD/MM/AAAA): ");
                            scanf("%d/%d/%d", &novoPaciente.ultimaConsulta.dia, &novoPaciente.ultimaConsulta.mes, &novoPaciente.ultimaConsulta.ano);
                            novoPaciente.diasUltCon = calcularDias(novoPaciente.nascimento, novoPaciente.ultimaConsulta);
                        } while (novoPaciente.diasUltCon == -1);
                        while (getchar() != '\n');
                        break;
                    case 4:
                        break;
                    default:
                        printf("%sOpção inválida!%s\n", ANSIcorVermelho, ANSIcorReset);
                        aguardarEnter();
                }
            } while (opcao != 4);

            // Atualiza os dados do paciente na AVL
            atual->paciente = novoPaciente;

            printf("%sCadastro alterado com sucesso!%s\n", ANSIcorVerde, ANSIcorReset);
            aguardarEnter();
            return;
        } else if (strcmp(atual->paciente.nome, nomePaciente) > 0) {
            atual = atual->noEsquerdo;
        } else {
            atual = atual->noDireito;
        }
    }

    printf("%sPaciente não encontrado.%s\n", ANSIcorVermelho, ANSIcorReset);
    aguardarEnter();
}

int menu(_NoArvore **no, _ListaDupla *lista) {

    int opcao;

    do {

        limparTela();
        printf("%sMenu Inicial:%s\n", ANSIcorCiano, ANSIcorReset);
        printf("1 - Pacientes da Liz\n");
        printf("2 - Pacientes do Moises\n");
        printf("3 - Gerar relatórios e sair do sistema\n");
        printf("Escolha uma opção: ");
        
        if (scanf("%d", &opcao) != 1) {  
            printf("%sEntrada inválida! Digite um número válido.%s\n", ANSIcorVermelho, ANSIcorReset);
            while (getchar() != '\n'); 
            aguardarEnter();
            continue;
        }

        while (getchar() != '\n'); 

        switch (opcao) {
            case 1:
                int subOpcao;
                do
                {
                    limparTela();
                    printf("%sMenu Pacientes:%s\n", ANSIcorCiano, ANSIcorReset);
                    printf("1 - Consultar paciente\n");
                    printf("2 - Cadastrar paciente\n");
                    printf("3 - Alterar cadastro\n");
                    printf("4 - Voltar ao menu inicial\n");
                    printf("Escolha uma opção: ");
                    
                    if (scanf("%d", &subOpcao) != 1) {  
                        printf("%sEntrada inválida! Digite um número válido.%s\n", ANSIcorVermelho, ANSIcorReset);
                        while (getchar() != '\n'); 
                        aguardarEnter();
                        continue;
                    }

                    while (getchar() != '\n');

                    switch (subOpcao)
                    {
                        case 1:
                            limparTela();
                            pesquisarPaciente('F', no, lista);
                            aguardarEnter();
                            break;
                        case 2:
                            limparTela();
                            adicionarPaciente(no, lista);
                            aguardarEnter();
                            break;
                        case 3:
                            alterarCadastroAVL(no);
                            aguardarEnter();
                            break;
                        case 4:
                            break;
                        default:
                            printf("%sOpção inválida!%s\n", ANSIcorVermelho, ANSIcorReset);
                            aguardarEnter();
                    }
                } while (subOpcao != 4);
                break;
            case 2: {
                int subOpcao;
                do {
                    limparTela();
                    printf("%sMenu Pacientes:%s\n", ANSIcorCiano, ANSIcorReset);
                    printf("1 - Consultar paciente\n");
                    printf("2 - Cadastrar paciente\n");
                    printf("3 - Alterar cadastro\n");
                    printf("4 - Voltar ao menu inicial\n");
                    printf("Escolha uma opção: ");
                    
                    if (scanf("%d", &subOpcao) != 1) {  
                        printf("%sEntrada inválida! Digite um número válido.%s\n", ANSIcorVermelho, ANSIcorReset);
                        while (getchar() != '\n'); 
                        aguardarEnter();
                        continue;
                    }

                    while (getchar() != '\n'); 

                    switch (subOpcao) {
                        case 1:
                            limparTela();
                            pesquisarPaciente('M', no, lista);
                            aguardarEnter();
                            break;
                        case 2:
                            limparTela();
                            adicionarPaciente(no, lista);
                            aguardarEnter();
                            break;
                        case 3:
                            alteraCadastroLista(lista);
                            aguardarEnter();
                            break;
                        case 4:
                            break;
                        default:
                            printf("%sOpção inválida!%s\n", ANSIcorVermelho, ANSIcorReset);
                            aguardarEnter();
                    }
                } while (subOpcao != 4);
                break;
            }
            case 3:
                printf("%sGerando relatórios...%s\n", ANSIcorAmarelo, ANSIcorReset);
                printf("%sEncerrando o programa...%s\n", ANSIcorMagenta, ANSIcorReset);
                geraRelatorioAndre(lista);
                arquivoPacientesLiz(no);
                return EXIT_SUCCESS;
            default:
                printf("%sOpção inválida!%s\n", ANSIcorVermelho, ANSIcorReset);
                aguardarEnter();
        }
    } while (opcao != 3);

    return EXIT_SUCCESS; 

}

// MAIN =========================================
int main(int argc, char *argv[]) {

    // verifica erro na passagem dos argumentos para a main
    if (argc != 2) {
        perror("Erro ao utilizar o arquivo desejado.");
        return 1;
    }

    // inicia AVL e lista
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

    menu(&raiz, &lista);

    fclose(arquivo);
    
    return 0;

}
