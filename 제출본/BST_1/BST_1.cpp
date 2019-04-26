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
	Node *p;		//�θ���
	Node *left;		//���� �ڽĳ��
	Node *right;	//������ �ڽĳ��
	char *word;	//�ܾ�
	char *upperWord;	//�ܾ� �˻縦 ���� ��� �빮�ڷ� �ٲ� �迭
	char **explaines;	//�ܾ� ����
	char **classes;	//�ܾ��� ǰ���
	int explainCount;
};

Node *treeRoot = NULL;
int treeNodeCount = 0;

//�Լ� �ĺ���
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

//����� ���ĵǾ� ������ Ȯ�� -> inorderŽ�� �غ�.
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

//postorder : Ʈ�� ���� ����(�����Ҵ� ����) : �ڽ� ���� �� ���� �����ϱ� ����.
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

//����� ��Ʃ��������� strsep�� ���� ���ͳ��� �����Ͽ� ������ ���� �����߽��ϴ�.
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

//���� �а� Ʈ�� ����
void read_file() {
	FILE *fp = fopen("shuffled_dict.txt", "r");
	char buffer1[BUFFER_SIZE];
	char *word, *wordClass, *meaning;
	while (1) {
		fgets(buffer1, BUFFER_SIZE, fp);
		if (strlen(buffer1) <= strlen("\n") || feof(fp))
			break;
		char *buffer2 = buffer1;
		//word �ڸ���
		word = strdup(strsep(&buffer2, "("));
		word[strlen(word) - 1] = '\0';	//���� ����
		//class�ڸ���
		wordClass = strdup(strsep(&buffer2, ")"));
		//���� ���� ����
		meaning = strdup(strsep(&buffer2, "\n"));

		//���� �ܾ �ִ��� �˻� : �빮�ڷ� ���� word�� �˻�
 		char *upperWord = strdup(word);
		upperWord = makeUpper(upperWord);

		Node *tmp = tree_search(treeRoot, upperWord);
		free(upperWord);

		if (treeRoot == NULL)
			treeRoot = make_node(word, wordClass, meaning);
		else if(tmp != NULL){//������ �� �ܾ��� ��忡 ����, ǰ�� ���ϰ� count++, word�����Ҵ� ����
			tmp->classes[tmp->explainCount] = wordClass;
			tmp->explaines[tmp->explainCount] = meaning;
			tmp->explainCount++;
			free(word);
		}
		else {//���� �ܾ� ������ ���� �߰�
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

//�˻��� ��带 ���� �빮�ڷ� �ٲ� �˻��Ѵ�.
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

//Ʈ�� �߰� : z�� �߰��� ���, root�� NULL�̸� �� ��尡 root�� ��.
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

		//Ű���� �Է� : �Է� ������ ��� �Է� ��ٸ�.
		fgets(commandLine, COMMAND_SIZE, stdin);
		commandLine[strlen(commandLine) - 1] = '\0';
		if ( strlen(commandLine)<= 0)
			continue;
		command = strtok(commandLine, " \n");

		if (strcmp(command, "size") == 0) {
			printf("Ʈ���� ����� �ܾ� : %d��\n", treeNodeCount);
			continue;
		}
		else if (strcmp(command, "find") == 0) {
			extra = strdup(strtok(NULL, "\n"));
			if (extra == NULL) {
				printf("�߸��� �Է¹���Դϴ�. \n");
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
			printf("����\n");
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
			//���� �ܾ� �ִ��� Ȯ��
			char *upperWord = strdup(word);
			upperWord = makeUpper(upperWord);
			Node *tmp = tree_search(treeRoot, upperWord);
			free(upperWord);

			if (tmp != NULL) {//������ �� �ܾ��� ��忡 ����, ǰ�� ���ϰ� count++, word�����Ҵ� ����
				char *cpyWordClass = strdup(wordClass);
				char *cpyMeaning = strdup(meaning);
				tmp->classes[tmp->explainCount] = cpyWordClass;
				tmp->explaines[tmp->explainCount] = cpyMeaning;
				tmp->explainCount++;
			}
			else {//���� �ܾ� ������ ���� �߰�
				char *cpyWord = strdup(word);
				char *cpyWordClass = strdup(wordClass);
				char *cpyMeaning = strdup(meaning);
				Node *newNode = make_node(cpyWord, cpyWordClass, cpyMeaning);
				tree_insert(newNode);
				treeNodeCount++;
			}
		}
		else if (strcmp(command, "delete") == 0) {//delete : �Է��� �ܾ� ����
			extra = strdup(strtok(NULL, "\n"));
			extra = makeUpper(extra);
			Node *result = tree_search(treeRoot, extra);
			if (result == NULL)
				printf("Not found.\n");
			else {
				erase_node(tree_delete(result));
				printf("���������� �����߽��ϴ�.\n");
			}
			free(extra);
		}
		else if (strcmp(command, "deleteall") == 0) {//deleteall : ������ �ܾ ����� ���� �а� ����.
			extra = strtok(NULL, " \n");
			deleteAll(extra);
			printf("���������� �����߽��ϴ�.\n");
		}
		else {//�������� �ʴ� command. ���Է�
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
		//�빮�ڷ� ����
		char *upperDeleteWord = strdup(deleteWord);
		upperDeleteWord = makeUpper(upperDeleteWord);
		//Ʈ������ ã��
		Node *deleteNode = tree_search(treeRoot, upperDeleteWord);
		free(upperDeleteWord);

		if (deleteNode != NULL)
			erase_node(tree_delete(deleteNode));
	}
	fclose(fp);

}

Node *tree_delete(Node *z) {
	Node *y = NULL;	//���������� �����ϰ� �� ���� y�� �ȴ�.
	Node *x = NULL;	//������ �� ������ ���

	//�� �� �ϳ� ������ ������ ���� �׳� z
	if ((z->left == NULL) || (z->right == NULL))
		y = z;
	else
		y = tree_successor(z);	//�ڽ��� �� �� ���� �� successorã��, z��ſ� successor�� y�� �����ϰ� �� ������ z�� �ű�.

	//x�� ���� �� �����ϱ� ���� �ʿ��� ���(�ڽ��� ���� ��� x = NULL)
	if (y->left != NULL)
		x = y->left;
	else
		x = y->right;

	//x�� ������ �θ� ����
	if (x != NULL)
		x->p = y->p;
	//y�� �θ� ���� : y�� ��Ʈ�̴�.
	if(y->p == NULL)
		treeRoot = x;
	else if(y == y->p->left)	//y�� ���� �ڽ��̸� ���ʿ� x����.
		y->p->left = x;
	else
		y->p->right = x;	//y�� ������ �ڽ��̸� �����ʿ� x����.
	//y�� z�� successor
	if (y != z) {
		//y�� ����� z�� ���� ����
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

//successor���ϱ�
Node *tree_successor(Node *x) {
	if (x->right != NULL)	//������ ��Ʈ���� �ִٸ� -> ������ ��Ʈ���� �ּҰ��� successor
		return tree_minimum(x->right);
	//������ ��Ʈ���� ���ٸ� -> �θ� Ÿ�� �ö󰡼� ó������ x�� ���� �ڽ��� �Ǵ� ����� y(x�� �θ�)
	Node *y = x->p;	//y�� x�� �θ���
	//y = NULL : y�� root, x�� �θ��� ������ ���
	while ((y != NULL) && (x == y->right)) {
		x = y;
		y = y->p;
	}
	return y;
}

//�ּڰ� ���ϱ�
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
