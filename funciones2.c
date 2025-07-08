#include <stdio.h>
#include <string.h>
#include "funciones.h"

// ANSI colores y decoraciones
#define RESET   "\x1b[0m"
#define NEGRITA "\x1b[1m"
#define CIAN    "\x1b[36m"
#define AMARILLO "\x1b[33m"
#define VERDE   "\x1b[32m"
#define ROJO    "\x1b[31m"
#define MAGENTA "\x1b[35m"
#define AZUL    "\x1b[34m"

// --- Menú de configuración ---
void menuConfiguracion(struct Zona zonas[], int *numZonasPtr) {
    int opcion_menu;
    
    while (1) {
        printf(CIAN "\n+-----------------------------------------------+\n");
        printf("|       " NEGRITA "MENU DE CONFIGURACION" RESET CIAN "                |\n");
        printf("+-----------------------------------------------+\n");
        printf("| 1. Cambiar nombre de zona                     |\n");
        printf("| 2. Ajustar maximo de semanas                  |\n");
        printf("| 3. Ajustar maximo de dias por semana          |\n");
        printf("| 4. Generar datos de muestreo                  |\n");
        printf("| 5. Configurar fechas de inicio                |\n");
        printf("| 6. Crear plantilla para datos                 |\n");
        printf("| 7. Importar datos                             |\n");
        printf("| 8. Editar fecha actual del sistema            |\n");
        printf("| 9. Volver                                     |\n");
        printf("+-----------------------------------------------+\n" RESET);
        printf("Seleccione opcion: ");
        scanf("%d", &opcion_menu);
        
        if (opcion_menu == 1) {
            printf(AMARILLO "Zonas disponibles:\n" RESET);
            for (int indice_zona = 0; indice_zona < *numZonasPtr; indice_zona++) {
                printf("Zona %d: %s\n", indice_zona+1, zonas[indice_zona].nombre);
            }
            int zona_seleccionada;
            printf("Seleccione zona a renombrar: ");
            scanf("%d", &zona_seleccionada);
            if (zona_seleccionada >= 1 && zona_seleccionada <= *numZonasPtr) {
                char nombre_nuevo[32];
                printf("Nuevo nombre para la zona %d: ", zona_seleccionada);
                scanf("%s", nombre_nuevo);
                strncpy(zonas[zona_seleccionada-1].nombre, nombre_nuevo, 32);
                printf(VERDE "Nombre de la zona %d cambiado a %s.\n" RESET, zona_seleccionada, nombre_nuevo);
            } else {
                printf(ROJO "Zona invalida.\n" RESET);
            }
        } else if (opcion_menu == 2) {
            int maximo_semanas_nuevo;
            printf("Nuevo maximo de semanas (1-104): ");
            scanf("%d", &maximo_semanas_nuevo);
            if (maximo_semanas_nuevo >= 1 && maximo_semanas_nuevo <= 104) {
                extern int g_max_semanas;
                g_max_semanas = maximo_semanas_nuevo;
                printf(VERDE "Maximo de semanas actualizado a %d.\n" RESET, g_max_semanas);
            } else {
                printf(ROJO "Valor invalido. Debe estar entre 1 y 104.\n" RESET);
            }
        } else if (opcion_menu == 3) {
            int maximo_dias_nuevo;
            printf("Nuevo maximo de dias por semana (1-7): ");
            scanf("%d", &maximo_dias_nuevo);
            if (maximo_dias_nuevo >= 1 && maximo_dias_nuevo <= 7) {
                extern int g_max_dias_semana;
                g_max_dias_semana = maximo_dias_nuevo;
                printf(VERDE "Maximo de dias por semana actualizado a %d.\n" RESET, g_max_dias_semana);
            } else {
                printf(ROJO "Valor invalido. Debe estar entre 1 y 7.\n" RESET);
            }
        } else if (opcion_menu == 4) {
            // Generar datos de muestreo para todas las zonas en la semana actual
            for (int indice_zona = 0; indice_zona < *numZonasPtr; indice_zona++) {
                int semana_actual = zonas[indice_zona].numSemanas;
                generarDatosAleatoriosSemana(&zonas[indice_zona], semana_actual);
                printf(VERDE "Zona %s: Semana %d generada con datos de muestreo.\n" RESET, zonas[indice_zona].nombre, semana_actual+1);
            }
        } else if (opcion_menu == 5) {
            configurarFechasInicio(&config_fechas);
        } else if (opcion_menu == 6) {
            char nombre_archivo[64];
            printf("Nombre del archivo (ejemplo: mis_datos.txt): ");
            scanf("%s", nombre_archivo);
            exportarPlantillaDatos(nombre_archivo);
            printf(VERDE "\n¡LISTO! Ahora:\n");
            printf("1. Abra %s en Bloc de Notas\n", nombre_archivo);
            printf("2. Cambie los valores de ejemplo por sus datos\n");
            printf("3. Guarde y use la opcion 7 para importar\n" RESET);
        } else if (opcion_menu == 7) {
            char nombre_archivo[64];
            printf("Nombre del archivo con sus datos: ");
            scanf("%s", nombre_archivo);
            printf(AMARILLO "Importando datos desde %s...\n" RESET, nombre_archivo);
            int registros_importados = importarDatosDesdeArchivo(zonas, *numZonasPtr, nombre_archivo);
            if (registros_importados > 0) {
                printf(VERDE "\n¡PERFECTO! %d registros importados\n" RESET, registros_importados);
                printf(AMARILLO "Ya puede ver reportes y alertas con sus datos\n" RESET);
            } else {
                printf(ROJO "No se pudo importar. Verifique el formato del archivo\n" RESET);
            }
        } else if (opcion_menu == 8) {
            // Nueva opción para editar fecha actual
            printf(AMARILLO "Fecha actual del sistema: %02d/%02d/%d\n" RESET, 
                   config_fechas.dia_inicio, config_fechas.mes_inicio, config_fechas.anio_inicio);
            printf("Ingrese nueva fecha:\n");
            printf("Anio: ");
            scanf("%d", &config_fechas.anio_inicio);
            printf("Mes (1-12): ");
            scanf("%d", &config_fechas.mes_inicio);
            printf("Dia (1-31): ");
            scanf("%d", &config_fechas.dia_inicio);
            
            // Validación básica
            if (config_fechas.mes_inicio < 1 || config_fechas.mes_inicio > 12) {
                printf(ROJO "Mes invalido, estableciendo enero.\n" RESET);
                config_fechas.mes_inicio = 1;
            }
            if (config_fechas.dia_inicio < 1 || config_fechas.dia_inicio > 31) {
                printf(ROJO "Dia invalido, estableciendo dia 1.\n" RESET);
                config_fechas.dia_inicio = 1;
            }
            
            printf(VERDE "Nueva fecha establecida: %02d/%02d/%d\n" RESET, 
                   config_fechas.dia_inicio, config_fechas.mes_inicio, config_fechas.anio_inicio);
            
            // Guardar automáticamente la configuración actualizada
            guardarConfiguracionFechas(&config_fechas);
        } else if (opcion_menu == 9) {
            break;
        } else {
            printf(ROJO "Opcion invalida.\n" RESET);
        }
    }
}

// --- Menú de reportes y exportación ---
void exportarReporteTabla(struct Zona zonas[], int numZonas) {
    FILE *f = fopen("reporte_zonas.txt", "w");
    if (!f) {
        printf(ROJO "No se pudo crear el archivo de reporte.\n" RESET);
        return;
    }
    fprintf(f, "+----+------------------+--------+--------+--------+--------+\n");
    fprintf(f, "| %-2s | %-16s | %-6s | %-6s | %-6s | %-6s |\n", "N", "Zona", "CO2", "SO2", "NO2", "PM2.5");
    fprintf(f, "+----+------------------+--------+--------+--------+--------+\n");
    for (int i = 0; i < numZonas; i++) {
        int s = zonas[i].numSemanas-1;
        if (s < 0) {
            s = 0;
        }
        float sumCO2=0, sumSO2=0, sumNO2=0, sumPM25=0;
        int nd = zonas[i].semanas[s].numDias;
        if (nd == 0) {
            nd = 1;
        }
        for (int d = 0; d < nd; d++) {
            struct DatosAmbientales *dia = &zonas[i].semanas[s].dias[d];
            sumCO2 += dia->co2;
            sumSO2 += dia->so2;
            sumNO2 += dia->no2;
            sumPM25 += dia->pm25;
        }
        fprintf(f, "| %-2d | %-16s | %6.1f | %6.1f | %6.1f | %6.1f |\n",
            i+1, zonas[i].nombre, sumCO2/nd, sumSO2/nd, sumNO2/nd, sumPM25/nd);
    }
    fprintf(f, "+----+------------------+--------+--------+--------+--------+\n");
    fclose(f);
    printf(VERDE "Reporte exportado a reporte_zonas.txt\n" RESET);
}

void mostrarAlertasYRecomendaciones(struct Zona zonas[], int numZonas) {
    char alertas[MAX_ALERTAS][128];
    int nAlertas = 0;
    FILE *ftemp = fopen("alertas_temp.txt", "w"); // Archivo temporal de alertas

    // Encabezado de tabla decorada con |, + y -
    printf(NEGRITA CIAN "\n+----+------------------+----------+-------------------------------+---------------------+\n" RESET);
    printf(NEGRITA CIAN "| %-2s | %-16s | %-8s | %-29s | %-19s |\n" RESET, "N", "Zona", "Nivel", "Mensaje", "CO2 (ppm)");
    printf(NEGRITA CIAN "+----+------------------+----------+-------------------------------+---------------------+\n" RESET);

    for (int i = 0; i < numZonas; i++) {
        int s = zonas[i].numSemanas-1;
        if (s < 0) s = 0;
        int nd = zonas[i].semanas[s].numDias;
        float pm25 = 0;
        float co2 = 0;
        
        if (nd > 0) {
            for (int d = 0; d < nd; d++) {
                pm25 += zonas[i].semanas[s].dias[d].pm25;
                co2 += zonas[i].semanas[s].dias[d].co2;
            }
            pm25 = pm25 / nd;
            co2 = co2 / nd;
        } else {
            pm25 = zonas[i].pm25;
            co2 = zonas[i].co2;
        }

        // Niveles OMS para PM2.5 (24h): Verde <=12, Amarillo <=35.4, Naranja <=55.4, Rojo >55.4
        int nivel = 0;
        const char *color, *nivelStr, *mensaje;
        if (pm25 <= 12.0f) {
            nivel = 0; color = VERDE; nivelStr = "VERDE"; mensaje = "Aire saludable";
        } else if (pm25 <= 35.4f) {
            nivel = 1; color = AMARILLO; nivelStr = "AMARILLA"; mensaje = "Precaucion: Moderado";
        } else if (pm25 <= 55.4f) {
            nivel = 2; color = MAGENTA; nivelStr = "NARANJA"; mensaje = "Riesgo: Sensibles";
        } else {
            nivel = 3; color = ROJO; nivelStr = "ROJA"; mensaje = "Peligro: Evite salir";
        }

        printf("| %-2d | %-16s | %s%-8s%s | %-29s | %-19.3f |\n", 
               i+1, zonas[i].nombre, color, nivelStr, RESET, mensaje, co2);

        // Guarda en archivo temporal
        if (ftemp) {
            fprintf(ftemp, "%d|%s|%s|%s|%.3f\n", i+1, zonas[i].nombre, nivelStr, mensaje, co2);
        }

        // Generar alertas para todas las zonas con nivel >= 1 (AMARILLA o superior)
        if (nivel >= 1) {
            snprintf(alertas[nAlertas], 128, "%s: PM2.5 %s", zonas[i].nombre, nivelStr);
            nAlertas++;
        }
        
        // Alerta adicional para CO2 alto
        if (co2 > 0.05f) {
            snprintf(alertas[nAlertas], 128, "%s: CO2 alto (%.3f ppm)", zonas[i].nombre, co2);
            nAlertas++;
        }
    }
    printf(NEGRITA CIAN "+----+------------------+----------+-------------------------------+---------------------+\n" RESET);

    if (ftemp) fclose(ftemp);

    // Recomendaciones decoradas como tabla
    printf(NEGRITA AMARILLO "\n+--------------------------------------------------------------+\n" RESET);
    printf(NEGRITA AMARILLO "|                   RECOMENDACIONES                            |\n" RESET);
    printf(NEGRITA AMARILLO "+--------------------------------------------------------------+\n" RESET);
    
    if (nAlertas == 0) {
        printf("| %-60s |\n", "No hay alertas. Calidad del aire aceptable.");
    } else {
        for (int i = 0; i < nAlertas; i++) {
            printf("| - %-58s |\n", alertas[i]);
            printf("| - %-58s |\n", "  Limite actividades al aire libre");
        }
        printf("| - %-58s |\n", "Considere usar mascarilla en exteriores");
        printf("| - %-58s |\n", "Mantenga puertas y ventanas cerradas");
    }
    printf(NEGRITA AMARILLO "+--------------------------------------------------------------+\n" RESET);
}

// --- Reporte semanal decorado ---
void mostrarReporteSemanal(struct Zona *zona, int semana) {
    if (semana >= zona->numSemanas) {
        printf(ROJO "Semana no disponible.\n" RESET);
        return;
    }
    printf(NEGRITA AZUL "\n+-----+------------+--------+--------+--------+--------+----------------------+\n" RESET);
    printf(NEGRITA AZUL "| Dia |   Fecha    |  CO2   |  SO2   |  NO2   | PM2.5  | Nivel PM2.5          |\n" RESET);
    printf(NEGRITA AZUL "+-----+------------+--------+--------+--------+--------+----------------------+\n" RESET);

    float sumCO2=0, sumSO2=0, sumNO2=0, sumPM25=0;
    int verde=0, amarilla=0, naranja=0, roja=0;
    for (int d = 0; d < zona->semanas[semana].numDias; d++) {
        struct DatosAmbientales *dia = &zona->semanas[semana].dias[d];
        sumCO2 += dia->co2;
        sumSO2 += dia->so2;
        sumNO2 += dia->no2;
        sumPM25 += dia->pm25;
        // Nivel y color
        const char *color, *nivelStr;
        if (dia->pm25 <= 12.0f) { color = VERDE; nivelStr = "VERDE (Saludable)"; verde++; }
        else if (dia->pm25 <= 35.4f) { color = AMARILLO; nivelStr = "AMARILLA (Moderado)"; amarilla++; }
        else if (dia->pm25 <= 55.4f) { color = MAGENTA; nivelStr = "NARANJA (Sensibles)"; naranja++; }
        else { color = ROJO; nivelStr = "ROJA (Peligro)"; roja++; }
        printf("| %3d | %10s | %6.2f | %6.2f | %6.2f | %6.2f | %s%-20s%s |\n",
            d+1, dia->fecha, dia->co2, dia->so2, dia->no2, dia->pm25, color, nivelStr, RESET);
    }
    printf(NEGRITA AZUL "+-----+------------+--------+--------+--------+--------+----------------------+\n" RESET);
    printf(NEGRITA AZUL "| Promedios: CO2=%.2f SO2=%.2f NO2=%.2f PM2.5=%.2f                       |\n" RESET,
        sumCO2/zona->semanas[semana].numDias, sumSO2/zona->semanas[semana].numDias,
        sumNO2/zona->semanas[semana].numDias, sumPM25/zona->semanas[semana].numDias);
    printf(NEGRITA AZUL "| Alertas PM2.5: %sVERDE=%d%s %sAMARILLA=%d%s %sNARANJA=%d%s %sROJA=%d%s                   |\n" RESET,
        VERDE, verde, RESET, AMARILLO, amarilla, RESET, MAGENTA, naranja, RESET, ROJO, roja, RESET);
    printf(NEGRITA AZUL "+--------------------------------------------------------------------------+\n" RESET);
}

// --- Menú de reportes y exportación ---
void exportarAlertasYRecomendaciones(struct Zona zonas[], int numZonas) {
    float prediccion[MAX_ZONAS];
    char alertas[MAX_ALERTAS][64];
    int nAlertas = 0;
    FILE *f = fopen("alertas_recomendaciones.txt", "w");
    if (!f) {
        printf(ROJO "No se pudo crear el archivo de alertas.\n" RESET);
        return;
    }
    for (int i = 0; i < numZonas; i++) {
        int s = zonas[i].numSemanas-1;
        if (s < 0) s = 0;
        int nd = zonas[i].semanas[s].numDias;
        float pm25 = 0;
        if (nd > 0) {
            for (int d = 0; d < nd; d++) {
                pm25 += zonas[i].semanas[s].dias[d].pm25;
            }
            pm25 = pm25 / nd;
        } else {
            pm25 = zonas[i].pm25;
        }
        prediccion[i] = pm25;
    }
    nAlertas = 0;
    for (int i = 0; i < numZonas; i++) {
        if (prediccion[i] > 35.0f) {
            snprintf(alertas[nAlertas], 64, "Alerta: %s PM2.5 > 35 ug/m3", zonas[i].nombre);
            nAlertas++;
        }
        if (zonas[i].co2 > 0.05f) {
            snprintf(alertas[nAlertas], 64, "Alerta: %s CO2 > 0.05 ppm", zonas[i].nombre);
            nAlertas++;
        }
    }
    fprintf(f, "Alertas:\n");
    if (nAlertas == 0) {
        fprintf(f, "No hay alertas. Calidad del aire aceptable.\n");
    } else {
        for (int i = 0; i < nAlertas; i++) {
            fprintf(f, "- %s\n", alertas[i]);
        }
    }
    fprintf(f, "Recomendaciones:\n");
    if (nAlertas == 0) {
        fprintf(f, "- No hay alertas. Calidad del aire aceptable.\n");
    } else {
        for (int i = 0; i < nAlertas; i++) {
            fprintf(f, "- %s: Limite actividades al aire libre.\n", alertas[i]);
        }
        fprintf(f, "- Considere usar mascarilla en exteriores.\n");
        fprintf(f, "- Mantenga puertas y ventanas cerradas.\n");
    }
    fclose(f);
    printf(VERDE "Alertas y recomendaciones exportadas a alertas_recomendaciones.txt\n" RESET);
}

void menuReportes(struct Zona zonas[], int numZonas) {
    int opcion_menu;
    int zona_seleccionada, semana_seleccionada;
    while (1) {
        printf(CIAN "\n+--------------------------------------+\n");
        printf("|      " NEGRITA "MENU DE REPORTES" RESET CIAN "                |\n");
        printf("+--------------------------------------+\n");
        printf("| 1. Reporte semanal por zona          |\n");
        printf("| 2. Tabla resumen de todas las zonas  |\n");
        printf("| 3. Mostrar alertas y recomendaciones |\n");
        printf("| 4. Exportar tabla de zonas           |\n");
        printf("| 5. Exportar alertas y recomendaciones|\n");
        printf("| 6. Volver                            |\n");
        printf("+--------------------------------------+\n" RESET);
        printf("Seleccione opcion: ");
        scanf("%d", &opcion_menu);
        if (opcion_menu == 1) {
            printf(AMARILLO "Zonas disponibles:\n" RESET);
            printf("+----+------------------+\n");
            printf("| %-2s | %-16s |\n", "N", "Zona");
            printf("+----+------------------+\n");
            for (int indice_zona = 0; indice_zona < numZonas; indice_zona++) {
                printf("| %-2d | %-16s |\n", indice_zona+1, zonas[indice_zona].nombre);
            }
            printf("+----+------------------+\n");
            printf("Seleccione zona: ");
            scanf("%d", &zona_seleccionada);
            if (zona_seleccionada >= 1 && zona_seleccionada <= numZonas) {
                int maximo_semanas = zonas[zona_seleccionada-1].numSemanas;
                if (maximo_semanas <= 0) {
                    maximo_semanas = 1;
                }
                printf("Semana (1-%d): ", maximo_semanas);
                scanf("%d", &semana_seleccionada);
                semana_seleccionada = semana_seleccionada - 1;
                if (semana_seleccionada >= 0 && semana_seleccionada < MAX_SEMANAS) {
                    mostrarReporteSemanal(&zonas[zona_seleccionada-1], semana_seleccionada);
                } else {
                    printf(ROJO "Semana invalida.\n" RESET);
                }
            } else {
                printf(ROJO "Zona invalida.\n" RESET);
            }
        } else if (opcion_menu == 2) {
            printf(VERDE "\n+----+------------------+--------+--------+--------+--------+\n");
            printf("| %-2s | %-16s | %-6s | %-6s | %-6s | %-6s |\n", "N", "Zona", "CO2", "SO2", "NO2", "PM2.5");
            printf("+----+------------------+--------+--------+--------+--------+\n");
            for (int indice_zona = 0; indice_zona < numZonas; indice_zona++) {
                int semana_ultima = zonas[indice_zona].numSemanas-1;
                if (semana_ultima < 0) {
                    semana_ultima = 0;
                }
                float suma_co2=0, suma_so2=0, suma_no2=0, suma_pm25=0;
                int numero_dias = zonas[indice_zona].semanas[semana_ultima].numDias;
                if (numero_dias == 0) {
                    numero_dias = 1;
                }
                for (int indice_dia = 0; indice_dia < numero_dias; indice_dia++) {
                    struct DatosAmbientales *datos_dia = &zonas[indice_zona].semanas[semana_ultima].dias[indice_dia];
                    suma_co2 += datos_dia->co2;
                    suma_so2 += datos_dia->so2;
                    suma_no2 += datos_dia->no2;
                    suma_pm25 += datos_dia->pm25;
                }
                printf("| %-2d | %-16s | %6.1f | %6.1f | %6.1f | %6.1f |\n",
                    indice_zona+1, zonas[indice_zona].nombre, suma_co2/numero_dias, suma_so2/numero_dias, suma_no2/numero_dias, suma_pm25/numero_dias);
            }
            printf("+----+------------------+--------+--------+--------+--------+\n" RESET);
        } else if (opcion_menu == 3) {
            mostrarAlertasYRecomendaciones(zonas, numZonas);
        } else if (opcion_menu == 4) {
            exportarReporteTabla(zonas, numZonas);
        } else if (opcion_menu == 5) {
            exportarAlertasYRecomendaciones(zonas, numZonas);
        } else if (opcion_menu == 6) {
            break;
        } else {
            printf(ROJO "Opcion invalida.\n" RESET);
        }
    }
}

// --- Menú de checkpoints ---
void menuCheckpoints(struct Zona zonas[], int numZonas, int semanaActual[]) {
    int op;
    int semanaSel;
    while (1) {
        printf(CIAN "\n+--------------------------------------+\n");
        printf("|      " NEGRITA "MENU DE CHECKPOINTS" RESET CIAN "             |\n");
        printf("+--------------------------------------+\n");
        printf("| 1. Guardar semana actual             |\n");
        printf("| 2. Retroceder a semana anterior      |\n");
        printf("| 3. Recuperar datos temporales        |\n");
        printf("| 4. Volver                            |\n");
        printf("+--------------------------------------+\n" RESET);
        printf("Seleccione opcion: ");
        scanf("%d", &op);
        
        if (op == 1) {
            // Guardar permanentemente la semana actual
            for (int i = 0; i < numZonas; i++) {
                guardarSemana(&zonas[i], semanaActual[i]);
                limpiarDatosTemporales(&zonas[i], semanaActual[i]);
            }
            printf(VERDE "Checkpoint guardado permanentemente para todas las zonas (semana actual).\n" RESET);
            
        } else if (op == 2) {
            // Retroceder a semana anterior
            int maxSemana = 0;
            for (int i = 0; i < numZonas; i++) {
                if (semanaActual[i] > maxSemana) maxSemana = semanaActual[i];
            }
            
            if (maxSemana == 0) {
                printf(ROJO "Ya está en la primera semana.\n" RESET);
                continue;
            }
            
            printf("Semana a cargar (1-%d): ", maxSemana);
            scanf("%d", &semanaSel);
            semanaSel = semanaSel - 1;
            
            if (semanaSel < 0 || semanaSel >= maxSemana) {
                printf(ROJO "Semana inválida.\n" RESET);
                continue;
            }
            
            for (int i = 0; i < numZonas; i++) {
                // Guardar temporalmente la semana actual antes de retroceder
                guardarSemanaTemporal(&zonas[i], semanaActual[i]);
                
                // Cargar la semana anterior
                cargarSemana(&zonas[i], semanaSel);
                semanaActual[i] = semanaSel;
            }
            printf(AMARILLO "Semana %d cargada para todas las zonas (semana actual guardada temporalmente).\n" RESET, semanaSel+1);
            
        } else if (op == 3) {
            // Recuperar datos temporales guardados
            int maxSemanaTemp = 0;
            for (int i = 0; i < numZonas; i++) {
                if (semanaActual[i] > maxSemanaTemp) maxSemanaTemp = semanaActual[i];
            }
            
            printf("Recuperar datos temporales de que semana (1-%d): ", maxSemanaTemp+1);
            scanf("%d", &semanaSel);
            semanaSel = semanaSel - 1;
            
            if (semanaSel < 0 || semanaSel > maxSemanaTemp) {
                printf(ROJO "Semana invalida.\n" RESET);
                continue;
            }
            
            int recuperados = 0;
            for (int i = 0; i < numZonas; i++) {
                char tempFile[64];
                sprintf(tempFile, "temp_%s_semana%d.dat", zonas[i].nombre, semanaSel+1);
                
                FILE *f = fopen(tempFile, "rb");
                if (f) {
                    fclose(f);
                    cargarSemanaTemporal(&zonas[i], semanaSel);
                    semanaActual[i] = semanaSel;
                    recuperados++;
                }
            }
            
            if (recuperados > 0) {
                printf(VERDE "Datos temporales recuperados para %d zonas.\n" RESET, recuperados);
            } else {
                printf(ROJO "No se encontraron datos temporales para esa semana.\n" RESET);
            }
            
        } else if (op == 4) {
            break;
        } else {
            printf(ROJO "Opcion invalida.\n" RESET);
        }
    }
}

// --- Menú de ingreso manual ---
void menuIngresoManual(struct Zona zonas[], int numZonas, int semanaActual[]) {
    printf(AMARILLO "Zonas disponibles:\n" RESET);
    printf("+----+------------------+----------+\n");
    printf("| %-2s | %-16s | %-8s |\n", "N", "Zona", "Semana");
    printf("+----+------------------+----------+\n");
    for (int i = 0; i < numZonas; i++) {
        printf("| %-2d | %-16s | %-8d |\n", i+1, zonas[i].nombre, semanaActual[i]+1);
    }
    printf("+----+------------------+----------+\n");
    
    int zonaSel;
    printf("\nSeleccione zona (0 para cancelar): ");
    scanf("%d", &zonaSel);
    
    if (zonaSel == 0) {
        return;
    }
    
    if (zonaSel >= 1 && zonaSel <= numZonas) {
        ingresarDatosManualMejorado(&zonas[zonaSel-1], semanaActual[zonaSel-1], &config_fechas);
    } else {
        printf(ROJO "Zona invalida.\n" RESET);
    }
}

// --- Guardar datos temporales de semana (checkpoint temporal)
void guardarSemanaTemporal(struct Zona *zona, int semana) {
    char tempFile[64];
    sprintf(tempFile, "temp_%s_semana%d.dat", zona->nombre, semana+1);
    
    FILE *f = fopen(tempFile, "wb");
    if (f) {
        fwrite(&zona->semanas[semana], sizeof(struct Semana), 1, f);
        fclose(f);
        printf(AMARILLO "Datos temporales guardados para %s semana %d\n" RESET, zona->nombre, semana+1);
    } else {
        printf(ROJO "Error: No se pudo crear archivo temporal para %s\n" RESET, zona->nombre);
    }
}

void cargarSemanaTemporal(struct Zona *zona, int semana) {
    char tempFile[64];
    sprintf(tempFile, "temp_%s_semana%d.dat", zona->nombre, semana+1);
    
    FILE *f = fopen(tempFile, "rb");
    if (f) {
        fread(&zona->semanas[semana], sizeof(struct Semana), 1, f);
        fclose(f);
        printf(VERDE "Datos temporales recuperados para %s semana %d\n" RESET, zona->nombre, semana+1);
    }
}

void limpiarDatosTemporales(struct Zona *zona, int semana) {
    char tempFile[64];
    sprintf(tempFile, "temp_%s_semana%d.dat", zona->nombre, semana+1);
    remove(tempFile);
}

void vaciarDatosTemporales() {
    // Esta funcion ahora limpia todos los archivos temporales
    printf(AMARILLO "Limpiando archivos temporales...\n" RESET);
}

// --- Menú de siguiente semana ---
void menuSiguienteSemana(struct Zona zonas[], int numZonas, int semanaActual[]) {
    printf(CIAN "\n+----------------------------------------------+\n");
    printf("|          AVANZAR A SIGUIENTE SEMANA         |\n");
    printf("+----------------------------------------------+\n" RESET);
    
    for (int i = 0; i < numZonas; i++) {
        // Verificar si la semana actual está completa
        if (zonas[i].semanas[semanaActual[i]].numDias == MAX_DIAS_SEMANA) {
            // Guardar permanentemente la semana actual antes de avanzar
            guardarSemana(&zonas[i], semanaActual[i]);
            
            // Limpiar archivos temporales de esta semana ya que se guardó permanentemente
            limpiarDatosTemporales(&zonas[i], semanaActual[i]);
            
            // Avanzar a la siguiente semana
            semanaActual[i] = semanaActual[i] + 1;
            if (semanaActual[i] >= MAX_SEMANAS) {
                semanaActual[i] = MAX_SEMANAS-1;
                printf(AMARILLO "Zona %s: Ya está en la última semana posible (%d)\n" RESET, 
                       zonas[i].nombre, semanaActual[i]+1);
            } else {
                printf(VERDE "Zona %s: Semana %d guardada permanentemente. Avanzando a semana %d\n" RESET, 
                       zonas[i].nombre, semanaActual[i], semanaActual[i]+1);
            }
        } else {
            printf(ROJO "Zona %s: Complete la semana actual (%d/%d días) antes de avanzar.\n" RESET, 
                   zonas[i].nombre, zonas[i].semanas[semanaActual[i]].numDias, MAX_DIAS_SEMANA);
        }
    }
}

// --- Generar datos de muestreo VARIADOS para demostrar todos los tipos de alertas ---
void generarDatosAleatoriosSemana(struct Zona *zona, int semana) {
    // Genera datos predefinidos específicos para cada ciudad ecuatoriana
    // Cada ciudad tendrá un nivel diferente de contaminación para mostrar todas las alertas
    
    // Datos base según la ciudad para generar diferentes niveles de alerta
    // PM2.5: VERDE=≤12, AMARILLA=12-35, NARANJA=35-55, ROJA=>55
    float base_pm25[] = {10.0f, 25.0f, 45.0f, 70.0f, 20.0f}; // Quito=VERDE, Cuenca=AMARILLA, Guayaquil=NARANJA, Loja=ROJA, Ambato=AMARILLA
    float base_co2[] = {390.0f, 410.0f, 440.0f, 470.0f, 0.07f}; // Quito-Ambato=Normal, Ambato=Alto para alerta CO2
    
    // Determinar índice de la ciudad ecuatoriana para asignar el tipo de alerta correspondiente
    int zonaIndex = 0; // Por defecto Quito (VERDE - saludable)
    if (strcmp(zona->nombre, "Cuenca") == 0) zonaIndex = 1;     // AMARILLA - moderado
    else if (strcmp(zona->nombre, "Guayaquil") == 0) zonaIndex = 2; // NARANJA - no saludable para sensibles
    else if (strcmp(zona->nombre, "Loja") == 0) zonaIndex = 3;      // ROJA - peligroso
    else if (strcmp(zona->nombre, "Ambato") == 0) zonaIndex = 4;    // AMARILLA + alerta CO2
    
    // Generar 7 días de datos
    for (int d = 0; d < 7; d++) {
        zona->semanas[semana].dias[d].pm25 = base_pm25[zonaIndex] + (d * 0.5f);
        zona->semanas[semana].dias[d].co2 = base_co2[zonaIndex] + (d * 2.0f);
        zona->semanas[semana].dias[d].so2 = 5.0f + (d * 0.8f);
        zona->semanas[semana].dias[d].no2 = 20.0f + (d * 5.0f);
        snprintf(zona->semanas[semana].dias[d].fecha, sizeof(zona->semanas[semana].dias[d].fecha), "2024-06-%02d", d+1);
    }

    zona->semanas[semana].numDias = 7;
    if (semana >= zona->numSemanas) {
        zona->numSemanas = semana+1;
    }
}

