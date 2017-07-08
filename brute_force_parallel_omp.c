#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <omp.h>

#define LIM_INF_GLOBAL '!'
#define LIM_SUP_GLOBAL '~'


char *objetivo;

// Função que valida se a cadeia é a senha buscada
int validacao(char cadeia[]) {
	return (strcmp(cadeia, objetivo) == 0);
}

// Função que varia a string do inicio ao fim
double varia(char **senha, char lim_inf[], char lim_sup[]) {
	double iteracao = -1;
	
	// Descobre tamanho	
	size_t tamanho = strlen(lim_inf);

	// Copia inicio pra uma variável temporária
	char atual[tamanho];
	strcpy(atual, lim_inf);

	// Loop da variação
	while(strcmp(atual, lim_sup) != 0) {
		// Incrementa o final da string
		atual[tamanho - 1]++;
		int i;
		// Percorre a string pra "rodar" os valores
		for(i = tamanho - 1; i >= 0; i--) {
			if(atual[i] == 127) {
				if(i == 0) {
					return 0;
				}
				atual[i] = 33;
				atual[i - 1]++;
			} else {
				i = -1;
				continue;
			}
		}
		// Valida senha
		if(validacao(atual)) {
			// Aloca e retorna
			*senha = (char *) malloc(strlen(lim_inf) + 1);
			(*senha)[strlen(lim_inf)] = '\0';
			strcpy(*senha, atual);
			return iteracao;
		}

		iteracao++;
	}

	return -1;
}

int somatorio(int n_processos, int distribuicao[n_processos]) {
	int i;
	int somatorio = 0;
	for(i = 0; i < n_processos; i++) {
		somatorio += distribuicao[i];
	}

	return somatorio;
}


int *distribui(int n_processos) {
	int *distribuicao = (int *) malloc(sizeof(int) * n_processos);
	int tamanho_intervalo = LIM_SUP_GLOBAL - LIM_INF_GLOBAL + 1;
	int i;
	for(i = 0; i < n_processos; i++) {
		distribuicao[i] = tamanho_intervalo / n_processos;
	}

	somatorio(n_processos, distribuicao);

	int iter = 0;
	while(somatorio(n_processos, distribuicao) < tamanho_intervalo) {
		for(i = 0; i < iter + 1; i++) {
			distribuicao[n_processos - i - 1] += 1;
		}
		iter++;
	}

	return distribuicao;
}

int main() {
	objetivo = (char *) malloc(sizeof(char) * 155);
	printf("Insira a senha objetivo: ");
	scanf("%s", objetivo);
	int n;
	printf("\nInsira o numero de threads: ");
	scanf("%d", &n);

	/*char lim_inf[strlen(objetivo) + 1], lim_sup[strlen(objetivo) + 1];
	int i;
	for(i = 0; i < strlen(objetivo); i++) {
		lim_inf[i] = LIM_INF;
		lim_sup[i] = LIM_SUP;
	}
	lim_inf[strlen(objetivo)] = '\0';
	lim_sup[strlen(objetivo)] = '\0';*/

	/*
	char *senha = NULL;
	int n = 4;
	char lims_inf[n][strlen(objetivo)];
	char lims_sup[n][strlen(objetivo)];
	for(i=0;i<n;i++){
		if(i==0){
			strcpy(lims_inf[i],lim_inf);
		}	
	}
	*/
	
	int i;
	int tamanho_intervalo = LIM_SUP_GLOBAL - LIM_INF_GLOBAL + 1;
	char *intervalo = (char *) malloc(sizeof(char) * tamanho_intervalo);
	for(i = 0; i < tamanho_intervalo; i++) {
		intervalo[i] = i + LIM_INF_GLOBAL;
	}

	// Cria o array de distribuição do Scatterv
	int *distribuicao = (int *) malloc(sizeof(int) * n);
	distribuicao = distribui(n);

	// Cria o array de displs(offsets) pro Scatterv
	int displs[n];
	int offset = 0;
	for(i = 0; i < n; i++) {
		displs[i] = offset;
		offset += distribuicao[i];

	}
	char *senha = NULL; 
	int posicoes[n];
	int soma;
	for(i=0;i<n;i++){
		if(i==0) posicoes[i]=0;
		else posicoes[i] = soma;
		soma += distribuicao[i]	;
	}

	#pragma omp parallel num_threads(n)
	{
	int id = omp_get_thread_num();
	char *limites = malloc(sizeof(char) * distribuicao[id]);
	
	if(id==0){
		printf("33  ");
		printf("%c\n",intervalo[0]);
	}
	else{
		printf("%d  ",posicoes[id]+33);
		printf("%c\n",intervalo[posicoes[id]]);	
	}
	
	//limites = posicoes[id];
	
	char lim_inf = posicoes[id];
	char lim_sup;
	if(id == n-1){
		lim_sup = LIM_SUP_GLOBAL;
	}
	else{
		lim_sup = posicoes[id+1]-1;
	}
	// Constroi as strings de limite e envia pra função
	char str_lim_inf[strlen(objetivo) + 1], str_lim_sup[strlen(objetivo) + 1];
	str_lim_inf[0] = lim_inf;
	str_lim_sup[0] = lim_sup;
	for(i = 1; i < strlen(objetivo); i++) {
		str_lim_inf[i] = LIM_INF_GLOBAL;
		str_lim_sup[i] = LIM_SUP_GLOBAL;
	}
	str_lim_inf[strlen(objetivo)] = '\0';
	str_lim_sup[strlen(objetivo)] = '\0';
	clock_t t_inicio, t_fim;
    	double t_total;
     
    	t_inicio = clock();
	
	double iteracao = varia(&senha, str_lim_inf, str_lim_sup); 
	t_fim = clock();
    	t_total = ((double) (t_fim - t_inicio)) / CLOCKS_PER_SEC;

	if(iteracao != -1) {
		printf("Senha \"%s\" descoberta na iteração %.f em %f segundos\n", senha, iteracao, t_total);
	} else {
		printf("Senha não encontrada\n");
	}
	}

}
