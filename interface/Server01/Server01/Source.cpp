#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <io.h>
#include <fcntl.h>
#include <time.h>
#include <tchar.h>

#define MAPX = 1920
#define MAPY = 1080

typedef struct obj {
	int id;
	int tipo;
	int x;
	int y;
	int tamx;
	int tamy;
	char bitmap[1024];
	struct obj * prox;
}obj;

obj * objectos;

int criaobj();
int listaobjectos();
int apagaobjecto();

int _tmain() {     // main main main main main main main main main main main main main main main main main main main main
	int a;
	
	
	while (1)
	{
		printf("\n	Opcoes\n\n");
		printf("[ 1 ] -> Cria objecto\n");
		printf("[ 2 ] -> Lista objecto\n");
		printf("[ 3 ] -> \n");
		printf("[ 4 ] -> Apaga objecto\n");
		printf("[ 5 ] -> Sair\n");
		scanf_s("%d", &a);
		switch (a)
		{
		case 1:
			criaobj();
			system("pause");
			break;
		case 2:
			system("cls");
			listaobjectos();
			system("pause");
			break;
		case 3:
			break;
		case 4:
			apagaobjecto();
			system("pause");
			break;
		case 5:
			return 0;
			break;
		default:
			break;
		}
		system("cls");
	}


}


int criaobj() {
	//adiciona objectos a lista fazer na aula de segurança
	obj * temp;
	obj * temp2;
	int i=0;
	
	int tipo;
	int x;
	int y;
	int tamx;
	int tamy;
	printf("\nTipo:");
	scanf_s("%d", &tipo);
	printf("\nx:");
	scanf_s("%d", &x);
	printf("\ny:");
	scanf_s("%d", &y);
	printf("\ntamx:");
	scanf_s("%d", &tamx);
	printf("\ntamy");
	scanf_s("%d", &tamy);

	temp = (obj*)malloc(sizeof(obj));
	temp->tipo=tipo;
	temp->x=x;
	temp->y=y;
	temp->tamx=tamx;
	temp->tamy=tamy;
	temp->prox = NULL;
	
	if (objectos == NULL) 
	{
		temp->id = i;
		objectos = temp;
	}
	else
	{
		temp2 = objectos;
		while (temp2->prox != NULL) {
			temp2 = temp2->prox;
			i++;
		}
		i++;
		temp->id = i;
		temp2->prox = temp;
	}

	return 0;
}

int listaobjectos() {
	obj *temp;
	temp = objectos;
	if (objectos != NULL) {
		printf("	id: %d\n	tipo: %d\n	x: %d\n	y: %d\n	Largura: %d\n	Altura: %d\n\n	********************************\n",temp->id,temp->tipo, temp -> x, temp->y, temp->tamx, temp->tamy );
		while (temp->prox != NULL) {
			temp = temp->prox;
			printf("	id: %d\n	tipo: %d\n	x: %d\n	y: %d\n	Largura: %d\n	Altura: %d\n\n	********************************\n", temp->id, temp->tipo, temp->x, temp->y, temp->tamx, temp->tamy);
		}
	}
	printf("\n");
	return 0;
}

int apagaobjecto() {
	obj * temp;
	obj * temp2;
	int id;
	printf("\nid:\n");
	scanf_s("%d",&id);
	
	temp = objectos;
	temp2 = objectos;
	while (temp != NULL){
		if (id == temp->id) {
			if (temp==objectos) {
				objectos = objectos->prox;
				free(temp);
				return 0;
			}
			temp2->prox = temp->prox;
			free(temp);
			return 0;
		}
		temp2 = temp;
		temp = temp->prox;
	}
	
	return 0;
}


