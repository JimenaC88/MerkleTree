#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include "merkle.h"

void print_hex(const unsigned char *hash , size_t len){
 for(size_t i=0; i<len ; i++){
        printf ("%02x", hash[i]);
 }
 printf ("\n");
}
int copiar(char *destino, const char *origen){
    strcpy(destino, origen);
    return 0;
}
char *concatenar(char *destino, char *origen){
    strcat(destino, origen);
    return destino;
}
MerkleNode *nuevo_nodo(char *hash_nodo){
     MerkleNode *nodonuevo=(MerkleNode*)malloc(sizeof(MerkleNode));
     nodonuevo->hash=hash_nodo;
     nodonuevo->left=NULL;
     nodonuevo->right=NULL;
     return nodonuevo;
}
 // Retorna un string de 16 caracteres hexadecimales (mas ’\0 ’).
 // El llamador es responsable de liberar la memoria .
char *hash_data (const char *data){
uint64_t hash = 14695981039346656037ULL ;
for(const unsigned char *p=(const unsigned char*)data; *p ; p++) {
 hash ^= *p ;
 hash *= 1099511628211ULL ;
}

char *result=malloc(17);
snprintf (result, 17, "%016" PRIx64, hash);
return result;
}

MerkleTree *merkle_tree_create(char **data, size_t count){
if(data == NULL && count>0){
 return NULL;
}
MerkleTree *tree=(MerkleTree *)malloc(sizeof(MerkleTree));
if(count == 0){
 char *nullroot=malloc(17);
 strcpy(nullroot, "0000000000000000");
 tree->root=nuevo_nodo(nullroot);
 tree->root = tree->root;
 tree->num_leaves = 0;
 tree->height = 1;
 tree->levels = (MerkleNode ***)malloc(sizeof(MerkleNode **));
 tree->levels[0] = (MerkleNode **)malloc(sizeof(MerkleNode *));
 tree->levels[0][0] = tree->root;
 tree->height = 0;
 tree->level_sizes = malloc(sizeof(size_t));
 tree->level_sizes[0] = 1;
    return tree;
}



 // Paso 1: crear las hojas
MerkleNode **hojas=(MerkleNode**)malloc(count*sizeof(MerkleNode*));
for(size_t i=0; i<count; i++){
 //hojas[i].nodo=(MerkleNode *)malloc(sizeof(MerkleNode));

 char *h = hash_data(data[i]);
 hojas[i]= nuevo_nodo(h);
}
 // Paso 2: subir nivel por nivel hasta quedar un unico nodo
 MerkleNode **nivel_actual = hojas;
 size_t tam_actual = count;
 size_t nivel=1;
 tree->level_sizes =malloc(64*sizeof(size_t));
 tree->levels = malloc(64 * sizeof(MerkleNode **));
 tree->levels[0]=hojas;
 tree->level_sizes[0]=count;
 while(tam_actual>1){

    size_t siguiente_tamanio=(tam_actual+1)/2;
    MerkleNode **siguiente_nivel=NULL;
    //MerkleNode *nodo;
    siguiente_nivel=(MerkleNode**)malloc(siguiente_tamanio*sizeof(MerkleNode*));
for(size_t i=0; i<tam_actual; i+=2){
    MerkleNode *left = nivel_actual[i];
    MerkleNode *right=NULL;

 // Caso borde 2: numero impar en este nivel
if(i+1 < tam_actual){
    right = nivel_actual[i+1];

}else{
 // Duplicar el ultimo nodo creando una copia
    char *copiahash=malloc(17);
    strcpy(copiahash, left->hash);
    right = nuevo_nodo(copiahash);
}
 // Concatenar hash izquierda + hash derecha ( en ese orden )
   char *concatenado = malloc(33);
   strcpy(concatenado, left->hash);
   strcat(concatenado, right->hash);
   char *hash_padre = hash_data(concatenado);
   free(concatenado);
   MerkleNode *padre = nuevo_nodo(hash_padre);
   padre->left = left;
   padre->right = right;

 //agregar padre a siguiente_nivel
 //agregar siguiente_nivel a levels
 //nivel_actual = siguiente_nivel


   siguiente_nivel[i/2]=padre;


}

    tree->levels[nivel]=siguiente_nivel;
    tree->level_sizes[nivel]=siguiente_tamanio;
    nivel_actual=siguiente_nivel;
    tam_actual=siguiente_tamanio;
    nivel++;
}

    tree->root = nivel_actual[0];
    tree->num_leaves=count;
    tree->height = nivel-1;

return tree;

}

char *merkle_tree_get_root(MerkleTree *tree){
      char *root_hash=(char *)malloc(strlen(tree->root->hash)+1);
      strcpy(root_hash, tree->root->hash);
      return root_hash;
}

MerkleProof *merkle_tree_get_proof (MerkleTree *tree , size_t index ){

 // Caso borde 3: indice fuera de rango
 if(tree==NULL||index >= tree->num_leaves){
 return NULL;
 }
 size_t idx_actual = index;
 // Recorrer desde las hojas hasta la raiz
// int ***nivel_nodos=(int ***)malloc(tree->levels*(int **));
 char *hash_hermano=NULL;
 bool es_izquierda=false;
 MerkleProof *proof = (MerkleProof *)malloc(sizeof(MerkleProof));
    if(proof == NULL){
            return NULL;
    }
    proof->length = tree->height;
    proof->steps = (ProofStep *)malloc(tree->height * sizeof(ProofStep));
 for (int nivel = 0; nivel<tree->height; nivel++){
    size_t tam_nivel=tree->level_sizes[nivel];
 if(idx_actual%2==0){ // somos hijo izquierdo
    es_izquierda=false;
 size_t idx_hermano = idx_actual + 1;
  if(idx_hermano < tam_nivel){
     hash_hermano=tree->levels[nivel][idx_hermano]->hash;
 } else{
    // Somos el ultimo nodo impar : el hermano es uno mismo
    hash_hermano=tree->levels[nivel][idx_actual]->hash;
 }
 } else{
    // somos hijo derecho
    size_t idx_hermano=idx_actual-1;
    hash_hermano= tree->levels[nivel][idx_hermano]->hash;
    es_izquierda = true; // hermano a la derecha
 }
 //agregar ProofStep ( hash_hermano , es_izquierda ) a proof
    proof->steps[nivel].hash=(char *)malloc(strlen(hash_hermano)+1);
    strcpy(proof->steps[nivel].hash, hash_hermano);
    proof->steps[nivel].is_left=es_izquierda;
    // Subir un nivel : el indice del padre es idx_actual / 2
    idx_actual = idx_actual / 2;
 }


 return proof;
}

bool verify_proof (const char *leaf_hash , MerkleProof *proof , const char *root){
 if(proof == NULL || leaf_hash == NULL || root == NULL){
 return false;
}

 bool resultado=false;
 char *actual = (char *)malloc(strlen(leaf_hash)+1);
 strcpy(actual, leaf_hash);
 for(int i=0; i<proof->length; i++){
        char *concatenado=malloc(36);
//8 SI paso . is_left :
 if(proof->steps[i].is_left==1){
// Hermano a la izquierda -> concatenar ( hermano + actual )

   strcpy(concatenado, proof->steps[i].hash);
   strcat(concatenado, actual);
 }else if(proof->steps[i].is_left!=1){
   // Hermano a la derecha -> concatenar ( actual + hermano )
    strcpy(concatenado, actual);
    strcat(concatenado, proof->steps[i].hash);
}
   char *nuevo = hash_data(concatenado);
   free(concatenado);
   free(actual);
   actual = nuevo;
 }
 if(strcmp(actual, root)==0){
    resultado=true;
 }else{
    resultado=false;
 }
 free(actual);
 return resultado;
}
void liberar_nodos_recursivo(MerkleNode *nodo) {
    if (nodo == NULL) return;
    liberar_nodos_recursivo(nodo->left);
    liberar_nodos_recursivo(nodo->right);
    free(nodo->hash);
    free(nodo);
}
void merkle_tree_free(MerkleTree *tree){
    if(tree==NULL){
        return;
    }
    liberar_nodos_recursivo(tree->root);
    if(tree->levels!=NULL){
        for(size_t i=0; i<tree->height; i++){
            free(tree->levels[i]);
        }
        free(tree->levels);
    }
    free(tree->level_sizes);
    free(tree);

}

void merkle_proof_free(MerkleProof *proof){
     if(proof==NULL){
        return;
     }
     if(proof->steps!=NULL){
     for(size_t i=0; i<proof->length; i++){
        free(proof->steps[i].hash);
     }
     free(proof->steps);
     }
     free(proof);
}
