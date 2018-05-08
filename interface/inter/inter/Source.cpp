#include<stdio.h>

int interfacee(int x, int y);

void main() {
	interfacee(20,20);
}

int interfacee(int x, int y) {
	int e, i,t=0;
	for (e = -1;e<=y+1;e++) {
		for (i = -1;i<=x+1;i++) {
			t = 0;
			if (e==-1 && t == 0) {
				printf("___");
				t = 1;
			}
			if (e > y && t == 0) {
				printf("---");
				t = 1;
			}
			if (i == -1 && t == 0) {
				printf(" | ");
				t = 1;
			}
			if (i > x && t == 0) {
				printf(" | ");
				t = 1;
			}
			if (t==0) {
				printf(" * ");
			}
		}
		printf("\n");
	}
	return 0;
}