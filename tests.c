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

    test_nodo_izquierdo_sin_hermano_derecho();
    printf("\n============================================\n");
    printf("  RESUMEN: %d pruebas, %d fallos\n", g_total, g_fallos);
    printf("  RESULTADO: %s\n",
           (g_fallos == 0) ? "TODAS LAS PRUEBAS PASARON" : "HAY FALLOS");
    printf("============================================\n\n");
    return g_fallos;
}
