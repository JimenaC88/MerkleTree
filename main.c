#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "merkle.h"
#include "tests.h"
int main (int argc, char *argv[]) {
 if (argc > 1 && strcmp(argv[1], "--test") == 0){
        return ejecutarTodasLasPruebas();
 }
 char *data[] = {"T1", "T2", "T3", "T4"};
 MerkleTree *tree = merkle_tree_create(data , 4);
 char *root = merkle_tree_get_root(tree);
 printf ("Raiz : %s\n", root);

 MerkleProof * proof = merkle_tree_get_proof(tree , 0); // Prueba para T1
 char *leaf_hash = hash_data ("T1");
 bool valid = verify_proof ( leaf_hash , proof , root );
 printf ("Prueba valida : %s\n", valid ? "Si" : "No");

// Caso negativo : intentar verificar T2 con la prueba de T1
 char * leaf_hash_t2 = hash_data ("T2");
 bool invalid = verify_proof ( leaf_hash_t2 , proof , root );
 printf("Prueba invalida? ( esperado No ): %s\n", invalid ? "Si" : "No");
 free(leaf_hash);
 free(leaf_hash_t2);
 merkle_proof_free(proof);
 free(root);
 merkle_tree_free(tree);
 return 0;
}
