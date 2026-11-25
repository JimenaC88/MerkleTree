#ifndef MERKLE_H_INCLUDED
#define MERKLE_H_INCLUDED
#include <stdbool.h>
#include <stddef.h>
typedef struct MerkleNode {
char *hash; // Hash en formato hexadecimal ( string )
struct MerkleNode *left;
struct MerkleNode *right;
}MerkleNode;

typedef struct {
MerkleNode *root;
size_t num_leaves;
MerkleNode ***levels;
size_t *level_sizes;
size_t height;
}MerkleTree;

typedef struct{
char *hash;
int is_left; // 1 si el hermano esta a la izquierda
}ProofStep;

typedef struct{
ProofStep *steps;
size_t length;
}MerkleProof;

char *hash_data(const char *data) ; // proporcionada
MerkleTree *merkle_tree_create(char **data, size_t count);
char *merkle_tree_get_root(MerkleTree *tree);
MerkleProof *merkle_tree_get_proof(MerkleTree *tree, size_t index );
bool verify_proof(const char *leaf_hash, MerkleProof *proof ,const char * root);
void merkle_tree_free(MerkleTree *tree);
void merkle_proof_free(MerkleProof *proof);

#endif // MERKLE_H_INCLUDED
