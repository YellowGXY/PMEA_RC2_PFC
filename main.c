#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "funciones.h"

int main() {
    struct Sistema sistema = {0};
    int opcion;
    char buf[16];

    // Si no existe datos_hist.dat, inicializar con datos de muestreo
    FILE *f = fopen("datos_hist.dat", "rb");
    if (!f) {
        inicializarSistema(&sistema);
    } else {
        fclose(f);
        cargarDatosHistoricos(&sistema, "datos_hist.dat");
    }

    while (1) {
        mostrarMenuPrincipal();
        printf("Seleccione una opcion: ");
        if (!fgets(buf, sizeof(buf), stdin)) {
            continue;
        }
        if (buf[0] == 'h' || buf[0] == 'H') {
            ayudaMenu("Principal");
            continue;
        }
        if (sscanf(buf, "%d", &opcion) != 1) {
            printf("Entrada invalida.\n");
            continue;
        }
        if (opcion == 9) {
            printf("Saliendo del sistema.\n");
            break;
        }
        manejarOpcion(opcion, &sistema);
    }
    return 0;
}
