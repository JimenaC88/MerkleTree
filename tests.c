#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "merkle.h"

static int g_total  = 0;
static int g_fallos = 0;

static void reportar(const char *nombre, int ok, const char *detalle) {
    g_total++;
    if (ok) {
        printf("  [PASS] %-55s\n", nombre);
    } else {
        g_fallos++;
        printf("  [FAIL] %-55s  -> %s\n", nombre, detalle ? detalle : "");
    }
}
static void test_arbol_nulo(void){
    MerkleTree *tree=merkle_tree_create(NULL, 3);
    char *root=merkle_tree_get_root(tree);
    MerkleProof *proof=merkle_tree_get_proof(tree, 0);
    int ok_hash = (proof==NULL && root==NULL);
    reportar("Entrada invalida", ok_hash, "No retorna NULL");
    merkle_proof_free(proof);
    free(root);
    merkle_tree_free(tree);
}
static void arbol_de_una_sola_hoja(void){
        char *data[]={"Una hoja que es la raíz"};

        MerkleTree *tree=merkle_tree_create(data, 1);
        char *root=merkle_tree_get_root(tree);
        MerkleProof *proof=merkle_tree_get_proof(tree, 0);
        char *leaf=hash_data(data[0]);
        int ok_raiz=(proof!=NULL && proof->length==0 && strcmp(root, leaf)==0);
        reportar("El arbol cuenta con una sola hoja que es la raiz", ok_raiz, "No se asigna 0000000000000000 al hash de la raiz del arbol de una hoja");

        int ok_ver=verify_proof(leaf, proof, root);
        reportar("Verify proof checkea la raiz", ok_ver, "No se pudo verificar la prueba");
        merkle_proof_free(proof);
        free(root);
        merkle_tree_free(tree);
        free(leaf);
}

static void indice_fuera_de_rango(void){
        char *data[]={"T1", "T2","T3"};
        MerkleTree *tree=merkle_tree_create(data, 3);
        char *root=merkle_tree_get_root(tree);
        MerkleProof *proof=merkle_tree_get_proof(tree, 4);
        int ok_index=(proof==NULL);
        reportar("No se puede acceder a una hoja fuera de rango", ok_index, "Se crean hojas fuera del índice");
        char *root_hash=hash_data(data[2]);
        int ok_ver=!verify_proof(root_hash, proof, root);
        reportar("Se verifico que el indice esta fuera de rango", ok_ver, "No se verifican los indices fuera de rango");
        merkle_proof_free(proof);
        free(root);
        merkle_tree_free(tree);
        free(root_hash);
}
static void orden_de_concatenacion(void){
        char *data[]={"T1","T2","T3","T4","T5"};
        MerkleTree *tree=merkle_tree_create(data, 5);
        char *root=merkle_tree_get_root(tree);
        for(size_t i=0; i<5; i++){
            MerkleProof *proof=merkle_tree_get_proof(tree, i);
            if(proof->steps->is_left==false){
            char *root_hash=hash_data(data[i]);
            int ok_ver=verify_proof(root_hash, proof, root);
            reportar("La hoja tiene un hermano derecho", ok_ver, "El orden de concatenacion esta ERROR");
            free(root_hash);
            }
            merkle_proof_free(proof);
        }

        free(root);
        merkle_tree_free(tree);
}
static void efecto_avalancha(void){
        char *data[]={"T1","T2","T3"};
        MerkleTree *tree=merkle_tree_create(data, 3);
        char *root1=merkle_tree_get_root(tree);
        MerkleProof *proof=merkle_tree_get_proof(tree, 2);
        char *data2[]={"T1","T2","T3."};
        MerkleTree *tree1=merkle_tree_create(data2, 3);
        char *root2=merkle_tree_get_root(tree1);
        MerkleProof *proof1=merkle_tree_get_proof(tree1, 2);
        int ok_proof=(proof!=NULL &&proof1!=NULL && tree!=NULL && tree1!=NULL && strcmp(root1, root2)!=0);
        reportar("Se respeta el efecto avalancha", ok_proof, "No se altero la raiz al cambiar una hoja ERROR");
        char *root_hash=hash_data(data2[2]);
        int ok_avalancha=!verify_proof(root_hash, proof, root1);
        reportar("Se verifico el efecto avalancha", ok_avalancha, "No se altero la raiz del arbol al cambiar una hoja");
        free(root_hash);
        root_hash=hash_data(data[2]);
        int ok_avalancha2=!verify_proof(root_hash, proof1, root2);
        reportar("Se verifico el efecto avalancha", ok_avalancha2, "No se altero la raiz del arbol al cambiar una hoja");
        merkle_proof_free(proof);
        merkle_proof_free(proof1);
        free(root1);
        free(root2);
        merkle_tree_free(tree);
        merkle_tree_free(tree1);
        free(root_hash);
}
static void una_sola_hoja(void){
    char *data[]={"Unica hoja"};
    MerkleTree *tree=merkle_tree_create(data, 1);
    char *root=merkle_tree_get_root(tree);
    MerkleProof *proof=merkle_tree_get_proof(tree, 0);
    int ok_const =(proof!=NULL && tree!=NULL && tree->num_leaves==1);
    reportar("Se creo un arbol de una sola hoja que no es la raiz", ok_const, "No se crea un arbol con una sola hoja");
    char *leaf=hash_data(data[0]);
    int ok_ver =verify_proof(leaf, proof, root);
    reportar("Verify proof checkea la hoja", ok_ver, "No se pudo verificar la hoja checkeada");
    merkle_proof_free(proof);
    free(root);
    merkle_tree_free(tree);
    free(leaf);
}
static void test_nodo_izquierdo_sin_hermano_derecho(void){
    char *data[]={"T1", "T2", "T3"};
    MerkleTree *tree=merkle_tree_create(data, 3);
    char *root=merkle_tree_get_root(tree);
    MerkleProof *proof=merkle_tree_get_proof(tree, 2);
    char *hash_T3=hash_data("T3");
    int ok_hash = (proof != NULL && proof->length == 2 && strcmp(proof->steps->hash, hash_T3) == 0);
    reportar("Impar (T3): hermano nivel 0 es el propio hash H3", ok_hash, "El hash hermano en nivel 0 no coincide con H3");
    int ok_dir = (proof != NULL && proof->steps[0].is_left==false && proof->steps[1].is_left == true);
    reportar("Impar (T3): direcciones (der en niv 0, izq en niv 1)", ok_dir, "is_left incorrecto en alguno de los pasos");
    int ok_verif = verify_proof(hash_T3, proof, root);
    reportar("Impar (T3): verify_proof valida la inclusion", ok_verif, "verify_proof devolvio false para T3");
    free(hash_T3);
    merkle_proof_free(proof);
    free(root);
    merkle_tree_free(tree);
}
int ejecutarTodasLasPruebas(void) {
    g_fallos=0;
    g_total=0;
    printf("\n============================================\n");
    printf("      BATERIA DE PRUEBAS AUTOMATICAS\n");
    printf("============================================\n");
    test_arbol_nulo();
    arbol_de_una_sola_hoja();
    indice_fuera_de_rango();
    orden_de_concatenacion();
    efecto_avalancha();
    una_sola_hoja();
    test_nodo_izquierdo_sin_hermano_derecho();

    printf("\n============================================\n");
    printf("  RESUMEN: %d pruebas, %d fallos\n", g_total, g_fallos);
    printf("  RESULTADO: %s\n",
           (g_fallos == 0) ? "TODAS LAS PRUEBAS PASARON" : "HAY FALLOS");
    printf("============================================\n\n");
    return g_fallos;
}
