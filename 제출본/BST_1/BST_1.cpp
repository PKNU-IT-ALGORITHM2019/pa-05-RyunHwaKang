#pragma warning(disable:4996)
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>

#define WORD_SIZE 100
#define STRING_SIZE 3000
#define EXPLAIN_SIZE 100
#define BUFFER_SIZE 5000
#define VOLUME 200
#define COMMAND_SIZE 200

typedef struct node Node;
struct node {
	Node *p;		//부모노드
	Node *left;		//왼쪽 자식노드
	Node *right;	//오른쪽 자식노드
	char *word;	//단어
	char *upperWord;	//단어 검사를 위해 모두 대문자로 바꾼 배열
	char **explaines;	//단어 설명
	char **classes;	//단어의 품사들
	int explainCount;
};

Node *treeRoot = NULL;
int treeNodeCount = 0;

//함수 식별자
void read_file();
Node *make_node(char *word, char *word_class, char *meaning);
Node *tree_search(Node *x, char *word);
void tree_insert(Node *z);
char *makeUpper(char *arr);
void process_command();
Node *tree_delete(Node *z);
Node *tree_successor(Node *x);
Node *tree_minimum(Node *x);
void deleteAll(char *fileName);
void erase_node(Node *y);
void post_order_to_delete(Node *x);

//제대로 정렬되어 들어갔는지 확인 -> inorder탐색 해봄.
/*
void inorder_tree_walk(Node *x) {
	if (x == NULL)
		return;
	if (x != NULL)
		inorder_tree_walk(x->left);
	printf("%s\n", x->word);
	for (int i = 0; i < x->explainCount; i++)
		printf("class : %s\nmeaning : %s\n", x->classes[i], x->explaines[i]);
	printf("\n");
	inorder_tree_walk(x->right);
}
*/

//postorder : 트리 전부 삭제(동적할당 해제) : 자식 삭제 후 본인 삭제하기 위해.
void post_order_to_delete(Node *x) {
	if (x != NULL) {
		post_order_to_delete(x->left);
		post_order_to_delete(x->right);
		erase_node(x);
	}

}

void main() {
	read_file();
	process_command();
	post_order_to_delete(treeRoot);
}

//비쥬얼 스튜디오에서는 strsep이 없어 인터넷을 참고하여 원형을 따로 구현했습니다.
char *strsep(char **stringp, const char *delim) {
	char *ptr = *stringp;
	if (ptr == NULL)
		return NULL;
	while (**stringp) {
		if (strchr(delim, **stringp) != NULL) {
			**stringp = 0x00;
			(*stringp)++;
			return ptr;
		}
		(*stringp)++;
	}
	*stringp = NULL;
	return ptr;
}

//파일 읽고 트리 만듬
void read_file() {
	FILE *fp = fopen("shuffled_dict.txt", "r");
	char buffer1[BUFFER_SIZE];
	char *word, *wordClass, *meaning;
	while (1) {
		fgets(buffer1, BUFFER_SIZE, fp);
		if (strlen(buffer1) <= strlen("\n") || feof(fp))
			break;
		char *buffer2 = buffer1;
		//word 자르기
		word = strdup(strsep(&buffer2, "("));
		word[strlen(word) - 1] = '\0';	//띄어쓰기 지움
		//class자르기
		wordClass = strdup(strsep(&buffer2, ")"));
		//남은 설명 저장
		meaning = strdup(strsep(&buffer2, "\n"));

		//같은 단어가 있는지 검사 : 대문자로 만든 word로 검사
 		char *upperWord = strdup(word);
		upperWord = makeUpper(upperWord);

		Node *tmp = tree_search(treeRoot, upperWord);
		free(upperWord);

		if (treeRoot == NULL)
			treeRoot = make_node(word, wordClass, meaning);
		else if(tmp != NULL){//있으면 그 단어의 노드에 설명, 품사 더하고 count++, word동적할당 해제
			tmp->classes[tmp->explainCount] = wordClass;
			tmp->explaines[tmp->explainCount] = meaning;
			tmp->explainCount++;
			free(word);
		}
		else {//같은 단어 없으면 새로 추가
			Node *newNode = make_node(word, wordClass, meaning);
			tree_insert(newNode);
			treeNodeCount++;
		}

	}
	fclose(fp);
}

Node *make_node(char *word, char *word_class, char *meaning) {
	Node *newOne = (Node *)malloc(sizeof(Node));
	newOne->p = NULL;
	newOne->left = NULL;
	newOne->right = NULL;
	newOne->explainCount = 1;
	newOne->word = word;
	//upperWord
	char *tmp = strdup(word);
	tmp = makeUpper(tmp);
	newOne->upperWord = tmp;
	newOne->classes = (char **)malloc(VOLUME * sizeof(char *));
	newOne->classes[0] = (char *)malloc(WORD_SIZE * sizeof(char));
	newOne->classes[0] = word_class;
	newOne->explaines = (char **)malloc(VOLUME * sizeof(char *));
	newOne->explaines[0] = (char *)malloc(STRING_SIZE * sizeof(char *));
	newOne->explaines[0] = meaning;

//	printf("%s\n", newOne->word);
	return newOne;
}

//검사할 노드를 전부 대문자로 바꿔 검사한다.
Node *tree_search(Node *x, char *word) {
	if (x == NULL)
		return NULL;
	if (strcmp(word, x->upperWord) == 0)
		return x;
	else if (strcmp(word, x->upperWord) < 0)
		return tree_search(x->left, word);
	else
		return tree_search(x->right, word);
}

//트리 추가 : z는 추가할 노드, root가 NULL이면 그 노드가 root가 됨.
void tree_insert(Node *z) {
	Node *y = NULL;
	Node *x = treeRoot;

	while (x != NULL) {
		y = x;
		if (strcmp(z->upperWord, x->upperWord) < 0)
			x = x->left;
		else 
			x = x->right;
	}
	z->p = y;
	if (y == NULL)
		treeRoot = z;
	else if (strcmp(z->upperWord, y->upperWord) < 0)
		y->left = z;
	else
		y->right = z;

}

char *makeUpper(char *arr) {
	for (int i = 0; i < (int)strlen(arr); i++)
		arr[i] = toupper(arr[i]);
	return arr;
}

void process_command() {
	char commandLine[COMMAND_SIZE], *command, *extra;
	char word[WORD_SIZE], wordClass[WORD_SIZE], meaning[EXPLAIN_SIZE];

	while (1) {
		printf("$ ");

		//키보드 입력 : 입력 없으면 계속 입력 기다림.
		fgets(commandLine, COMMAND_SIZE, stdin);
		commandLine[strlen(commandLine) - 1] = '\0';
		if ( strlen(commandLine)<= 0)
			continue;
		command = strtok(commandLine, " \n");

		if (strcmp(command, "size") == 0) {
			printf("트리에 저장된 단어 : %d개\n", treeNodeCount);
			continue;
		}
		else if (strcmp(command, "find") == 0) {
			extra = strdup(strtok(NULL, "\n"));
			if (extra == NULL) {
				printf("잘못된 입력방식입니다. \n");
				free(extra);
				continue;
			}
			extra = makeUpper(extra);
			Node *result = tree_search(treeRoot, extra);

			if (result == NULL)
				printf("Not found.\n");
			else {
				printf("word : %s\n", result->word);
				for (int i = 0; i < result->explainCount; i++)
					printf("%d)\nclass : %s\nmeaning : %s\n", i + 1, result->classes[i], result->explaines[i]);
				printf("\n");
			}
			free(extra);
			continue;
		}
		else if (strcmp(command, "exit") == 0) {
			printf("종료\n");
			break;
		}
		else if (strcmp(command, "add") == 0) {//add
			printf("word : ");
			fgets(word, WORD_SIZE, stdin);
			word[strlen(word) - 1] = '\0';
			printf("class : ");
			fgets(wordClass, WORD_SIZE, stdin);
			wordClass[strlen(wordClass) - 1] = '\0';
			printf("meaning : ");
			fgets(meaning, WORD_SIZE, stdin);
			meaning[strlen(meaning) - 1] = '\0';
			//같은 단어 있는지 확인
			char *upperWord = strdup(word);
			upperWord = makeUpper(upperWord);
			Node *tmp = tree_search(treeRoot, upperWord);
			free(upperWord);

			if (tmp != NULL) {//있으면 그 단어의 노드에 설명, 품사 더하고 count++, word동적할당 해제
				char *cpyWordClass = strdup(wordClass);
				char *cpyMeaning = strdup(meaning);
				tmp->classes[tmp->explainCount] = cpyWordClass;
				tmp->explaines[tmp->explainCount] = cpyMeaning;
				tmp->explainCount++;
			}
			else {//같은 단어 없으면 새로 추가
				char *cpyWord = strdup(word);
				char *cpyWordClass = strdup(wordClass);
				char *cpyMeaning = strdup(meaning);
				Node *newNode = make_node(cpyWord, cpyWordClass, cpyMeaning);
				tree_insert(newNode);
				treeNodeCount++;
			}
		}
		else if (strcmp(command, "delete") == 0) {//delete : 입력한 단어 삭제
			extra = strdup(strtok(NULL, "\n"));
			extra = makeUpper(extra);
			Node *result = tree_search(treeRoot, extra);
			if (result == NULL)
				printf("Not found.\n");
			else {
				erase_node(tree_delete(result));
				printf("성공적으로 삭제했습니다.\n");
			}
			free(extra);
		}
		else if (strcmp(command, "deleteall") == 0) {//deleteall : 삭제할 단어가 저장된 파일 읽고 삭제.
			extra = strtok(NULL, " \n");
			deleteAll(extra);
			printf("성공적으로 삭제했습니다.\n");
		}
		else {//존재하지 않는 command. 재입력
			printf("Invalid command\n");
			continue;
		}

	}



}

void deleteAll(char *fileName) {
	FILE *fp = fopen(fileName, "r");
	char buffer[WORD_SIZE], *deleteWord;

	while (1) {
		fgets(buffer, WORD_SIZE, fp);
		if (strlen(buffer) <= strlen("\n") || feof(fp))
			break;
		deleteWord = strtok(buffer, "\n");
		//대문자로 변경
		char *upperDeleteWord = strdup(deleteWord);
		upperDeleteWord = makeUpper(upperDeleteWord);
		//트리에서 찾기
		Node *deleteNode = tree_search(treeRoot, upperDeleteWord);
		free(upperDeleteWord);

		if (deleteNode != NULL)
			erase_node(tree_delete(deleteNode));
	}
	fclose(fp);

}

Node *tree_delete(Node *z) {
	Node *y = NULL;	//실질적으로 삭제하게 될 노드는 y가 된다.
	Node *x = NULL;	//삭제한 후 연결할 노드

	//둘 중 하나 없으면 삭제할 노드는 그냥 z
	if ((z->left == NULL) || (z->right == NULL))
		y = z;
	else
		y = tree_successor(z);	//자식이 둘 다 있을 때 successor찾기, z대신에 successor인 y를 삭제하고 그 내용을 z로 옮김.

	//x는 삭제 후 연결하기 위해 필요한 노드(자식이 없는 경우 x = NULL)
	if (y->left != NULL)
		x = y->left;
	else
		x = y->right;

	//x와 삭제할 부모를 연결
	if (x != NULL)
		x->p = y->p;
	//y의 부모가 없다 : y가 루트이다.
	if(y->p == NULL)
		treeRoot = x;
	else if(y == y->p->left)	//y가 왼쪽 자식이면 왼쪽에 x넣음.
		y->p->left = x;
	else
		y->p->right = x;	//y가 오른쪽 자식이면 오른쪽에 x넣음.
	//y가 z의 successor
	if (y != z) {
		//y의 내용들 z에 값을 복사
		free(z->word);
		z->word = strdup(y->word);
		free(z->upperWord);
		z->upperWord = strdup(y->upperWord);
		int i = 0;
		for (i = 0; i < y->explainCount; i++) {
			if (i >= z->explainCount) {
				z->explaines[i] = strdup(y->explaines[i]);
				z->classes[i] = strdup(y->classes[i]);
			}
			else {
				free(z->classes[i]);
				free(z->explaines[i]);
				z->classes[i] = strdup(y->classes[i]);
				z->explaines[i] = strdup(y->explaines[i]);
			}
		}
		if (i < z->explainCount) {
			while (i < z->explainCount) {
				free(z->classes[i]);
				free(z->explaines[i]);
				i++;
			}
		}
		z->explainCount = y->explainCount;
	}
	
	return y;
}

//successor구하기
Node *tree_successor(Node *x) {
	if (x->right != NULL)	//오른쪽 부트리가 있다면 -> 오른쪽 부트리의 최소값이 successor
		return tree_minimum(x->right);
	//오른쪽 부트리가 없다면 -> 부모를 타고 올라가서 처음으로 x가 왼쪽 자식이 되는 경우의 y(x의 부모)
	Node *y = x->p;	//y는 x의 부모노드
	//y = NULL : y가 root, x가 부모의 오른쪽 노드
	while ((y != NULL) && (x == y->right)) {
		x = y;
		y = y->p;
	}
	return y;
}

//최솟값 구하기
Node *tree_minimum(Node *x) {
	while (x->left != NULL)
		x = x->left;
	return x;
}

void erase_node(Node *y) {
	free(y->word);
	for (int i = 0; i < y->explainCount; i++) {
		free(y->classes[i]);
		free(y->explaines[i]);
	}
	free(y->classes);
	free(y->explaines);
	free(y);

	treeNodeCount--;
}
