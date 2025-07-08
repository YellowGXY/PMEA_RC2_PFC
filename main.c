#include <stdio.h>
#include <stdlib.h>
#include "funciones.h"

int main() {
    Zona zonas[MAX_ZONAS];
    int numZonas = MAX_ZONAS;
    int opcion, zonaSel, semanaSel;

    inicializarZonas(zonas, &numZonas);

    while (1) {
        printf("\n--- MENU PRINCIPAL ---\n");
        printf("1. Configurar zonas y umbrales\n");
        printf("2. Generar datos aleatorios para una semana\n");
        printf("3. Ingresar datos manualmente\n");
        printf("4. Reporte semanal\n");
        printf("5. Guardar semana (checkpoint)\n");
        printf("6. Recuperar semana\n");
        printf("7. Salir\n");
        printf("Seleccione opcion: ");
        scanf("%d", &opcion);

        if (opcion == 1) {
            menuConfiguracionZona(zonas, numZonas);
        } else if (opcion == 2) {
            printf("Seleccione zona (0-%d): ", numZonas-1);
            scanf("%d", &zonaSel);
            printf("Semana (0-%d): ", MAX_SEMANAS-1);
            scanf("%d", &semanaSel);
            generarDatosAleatoriosSemana(&zonas[zonaSel], semanaSel);
        } else if (opcion == 3) {
            printf("Seleccione zona (0-%d): ", numZonas-1);
            scanf("%d", &zonaSel);
            printf("Semana (0-%d): ", MAX_SEMANAS-1);
            scanf("%d", &semanaSel);
            ingresarDatosManualSemana(&zonas[zonaSel], semanaSel);
        } else if (opcion == 4) {
            printf("Seleccione zona (0-%d): ", numZonas-1);
            scanf("%d", &zonaSel);
            printf("Semana (0-%d): ", MAX_SEMANAS-1);
            scanf("%d", &semanaSel);
            mostrarReporteSemanal(&zonas[zonaSel], semanaSel);
        } else if (opcion == 5) {
            printf("Seleccione zona (0-%d): ", numZonas-1);
            scanf("%d", &zonaSel);
            printf("Semana (0-%d): ", MAX_SEMANAS-1);
            scanf("%d", &semanaSel);
            guardarSemana(&zonas[zonaSel], semanaSel);
        } else if (opcion == 6) {
            printf("Seleccione zona (0-%d): ", numZonas-1);
            scanf("%d", &zonaSel);
            printf("Semana (0-%d): ", MAX_SEMANAS-1);
            scanf("%d", &semanaSel);
            cargarSemana(&zonas[zonaSel], semanaSel);
        } else if (opcion == 7) {
            break;
        }
    }
    return 0;
}
