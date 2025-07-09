#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "funciones.h"

#define ANSI_RESET   "\x1b[0m"
#define ANSI_NEGRITA "\x1b[1m"
#define ANSI_CIAN    "\x1b[36m"
#define ANSI_AMARILLO "\x1b[33m"
#define ANSI_VERDE   "\x1b[32m"
#define ANSI_ROJO    "\x1b[31m"
#define ANSI_MAGENTA "\x1b[35m"
#define ANSI_AZUL    "\x1b[34m"

// Variable global para configuracion de fechas
struct ConfiguracionFechas config_fechas = {2025, 1, 1, 1};

// Funcion para cargar o solicitar fecha inicial al usuario
void cargarOSolicitarFechaInicial() {
    // Intentar cargar configuracion existente
    if (cargarConfiguracionFechas(&config_fechas)) {
        // Configuracion cargada exitosamente
        printf(ANSI_VERDE "Configuracion de fechas cargada exitosamente.\n" ANSI_RESET);
        printf(ANSI_CIAN "Fecha del sistema: %02d/%02d/%d\n" ANSI_RESET,
               config_fechas.dia_inicio, config_fechas.mes_inicio, config_fechas.anio_inicio);
        return;
    }
    
    // Si no existe configuracion, solicitar al usuario
    printf(ANSI_NEGRITA ANSI_CIAN "\n+-----------------------------------------------+\n");
    printf("|        CONFIGURACION INICIAL DEL SISTEMA     |\n");
    printf("+-----------------------------------------------+\n" ANSI_RESET);
    
    printf(ANSI_AMARILLO "Primera ejecucion detectada.\n");
    printf("Por favor, ingrese la fecha actual del sistema:\n" ANSI_RESET);
    config_fechas.anio_inicio = leerEnteroSeguro("Anio (ej. 2024): ", 1900, 2100);
    config_fechas.mes_inicio = leerEnteroSeguro("Mes (1-12): ", 1, 12);
    config_fechas.dia_inicio = leerEnteroSeguro("Dia (1-31): ", 1, 31);
    
    config_fechas.usar_fechas_automaticas = 1;
    
    printf(ANSI_VERDE "Fecha establecida: %02d/%02d/%d\n" ANSI_RESET, 
           config_fechas.dia_inicio, config_fechas.mes_inicio, config_fechas.anio_inicio);
    
    // Guardar la configuracion para futuras ejecuciones
    guardarConfiguracionFechas(&config_fechas);
    printf(ANSI_CIAN "Continuando al menu principal...\n" ANSI_RESET);
}

// Prototipos exportados a funciones.c o funciones2.c
void menuConfiguracion(struct Zona zonas[], int *numZonasPtr, int mesActual[]);
void menuIngresoManual(struct Zona zonas[], int numZonas, int mesActual[]);
void menuReportes(struct Zona zonas[], int numZonas);
void menuCheckpoints(struct Zona zonas[], int numZonas, int mesActual[]);
void menuSiguienteMes(struct Zona zonas[], int numZonas, int mesActual[]);
void menuPronosticos(struct Zona zonas[], int numZonas, int mesActual[]);
void guardarMes(struct Zona *zona, int mes);
void inicializarZonas(struct Zona zonas[], int *numZonas);

int main() {
    struct Zona zonas[MAX_ZONAS];
    int numZonas = MAX_ZONAS;
    int mesActual[MAX_ZONAS] = {0};

    // Cargar configuracion de fechas o solicitarla si es la primera vez
    cargarOSolicitarFechaInicial();

    inicializarZonas(zonas, &numZonas);
    
    // Cargar datos existentes de las zonas desde checkpoints
    cargarDatosZonas(zonas, numZonas);
    
    // Cargar el estado de los meses actuales
    cargarMesActual(mesActual, numZonas);
    
    // Ajustar mes actual basado en la fecha del sistema
    ajustarMesActualSegunFecha(mesActual, numZonas);

    // Mostrar informacion de inicio
    printf(ANSI_VERDE "Sistema iniciado en el mes: %d (basado en fecha actual: %02d/%02d/%d)\n" ANSI_RESET,
           mesActual[0] + 1, config_fechas.dia_inicio, config_fechas.mes_inicio, config_fechas.anio_inicio);

    while (1) {
        printf(ANSI_NEGRITA ANSI_AZUL "\n+----------------------------------------------------------+\n");
        printf("|        SISTEMA INTEGRAL DE GESTION Y PREDICCION AIRE     |\n");
        printf("+----------------------------------------------------------+\n");
        printf("| 1. Configuracion                                         |\n");
        printf("| 2. Ingreso manual de datos                               |\n");
        printf("| 3. Reportes                                              |\n");
        printf("| 4. Checkpoints (guardar/recuperar mes)                   |\n");
        printf("| 5. Siguiente mes                                         |\n");
        printf("| 6. Pronosticos de calidad del aire                       |\n");
        printf("| 7. Salir                                                 |\n");
        printf("+----------------------------------------------------------+\n" ANSI_RESET);
        
        int opcion = leerEnteroSeguro("Seleccione una opcion: ", 1, 7);

        if (opcion == 1) {
            menuConfiguracion(zonas, &numZonas, mesActual);
        } else if (opcion == 2) {
            menuIngresoManual(zonas, numZonas, mesActual);
        } else if (opcion == 3) {
            menuReportes(zonas, numZonas);
        } else if (opcion == 4) {
            menuCheckpoints(zonas, numZonas, mesActual);
        } else if (opcion == 5) {
            menuSiguienteMes(zonas, numZonas, mesActual);
        } else if (opcion == 6) {
            menuPronosticos(zonas, numZonas, mesActual);
        } else if (opcion == 7) {
            // Guardar todos los meses antes de salir
            for (int i = 0; i < numZonas; i++) {
                guardarMes(&zonas[i], mesActual[i]);
            }
            // Guardar configuracion de fechas al salir
            guardarConfiguracionFechas(&config_fechas);
            // Guardar estado de meses actuales
            guardarMesActual(mesActual, numZonas);
            printf(ANSI_MAGENTA "Datos y configuracion guardados. Saliendo del sistema.\n" ANSI_RESET);
            break;
        } else {
            printf(ANSI_ROJO "Opcion invalida.\n" ANSI_RESET);
        }
    }
    return 0;
}
