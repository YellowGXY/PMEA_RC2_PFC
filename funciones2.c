#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
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

// --- Menu de configuracion ---
void menuConfiguracion(struct Zona zonas[], int *numZonasPtr, int mesActual[]) {
    (void)mesActual; // Suprimir warning de parámetro no usado
    while (1) {
        printf(CIAN "\n+-----------------------------------------------+\n");
        printf("|          " NEGRITA "MENU DE CONFIGURACION" RESET CIAN "                |\n");
        printf("+-----------------------------------------------+\n");
        printf("| 1. Cambiar nombre de zona                     |\n");
        printf("| 2. Generar datos de muestreo                  |\n");
        printf("| 3. Configurar fechas de inicio                |\n");
        printf("| 4. Crear plantilla para datos                 |\n");
        printf("| 5. Importar datos                             |\n");
        printf("| 6. Editar fecha actual del sistema            |\n");
        printf("| 7. Volver                                     |\n");
        printf("+-----------------------------------------------+\n" RESET);
        
        int opcion_menu = leerEnteroSeguro("Seleccione opcion: ", 1, 7);
        
        if (opcion_menu == 1) {
            printf(AMARILLO "Zonas disponibles:\n" RESET);
            for (int indice_zona = 0; indice_zona < *numZonasPtr; indice_zona++) {
                printf("Zona %d: %s\n", indice_zona+1, zonas[indice_zona].nombre);
            }
            int zona_seleccionada = leerEnteroSeguro("Seleccione zona a renombrar: ", 1, *numZonasPtr);
            if (zona_seleccionada >= 1 && zona_seleccionada <= *numZonasPtr) {
                char nombre_nuevo[32];
                leerCadenaSegura("Nuevo nombre para la zona: ", nombre_nuevo, 32);
                strncpy(zonas[zona_seleccionada-1].nombre, nombre_nuevo, 32);
                printf(VERDE "Nombre de la zona %d cambiado a %s.\n" RESET, zona_seleccionada, nombre_nuevo);
            } else {
                printf(ROJO "Zona invalida.\n" RESET);
            }
        } else if (opcion_menu == 2) {
            printf(AMARILLO "Estado actual de meses:\n" RESET);
            for (int indice_zona = 0; indice_zona < *numZonasPtr; indice_zona++) {
                printf("  %s: Mes actual %d (%d dias)\n", zonas[indice_zona].nombre, 
                       mesActual[indice_zona] + 1,
                       zonas[indice_zona].meses[mesActual[indice_zona]].numDias);
            }
            printf(VERDE "\nGenerando datos de muestreo para mes actual...\n" RESET);
            
            for (int indice_zona = 0; indice_zona < *numZonasPtr; indice_zona++) {
                generarDatosAleatoriosMes(&zonas[indice_zona], mesActual[indice_zona]);
            }
            
            guardarMesActual(mesActual, *numZonasPtr);
        } else if (opcion_menu == 3) {
            configurarFechasInicio(&config_fechas);
        } else if (opcion_menu == 4) {
            char nombre_archivo[64];
            leerCadenaSegura("Nombre del archivo (ejemplo: datos_enero.txt): ", nombre_archivo, 64);
            exportarPlantillaDatos(nombre_archivo);
            printf(VERDE "\n¡LISTO! Plantilla para carga masiva creada:\n");
            printf("1. Abra %s en Bloc de Notas\n", nombre_archivo);
            printf("2. Complete los datos siguiendo el formato mostrado\n");
            printf("3. Puede agregar datos para multiples dias/meses\n");
            printf("4. Guarde y use la opcion 5 para importar\n" RESET);
        } else if (opcion_menu == 5) {
            char nombre_archivo[64];
            leerCadenaSegura("Nombre del archivo con sus datos: ", nombre_archivo, 64);
            printf(AMARILLO "Importando datos desde %s...\n" RESET, nombre_archivo);
            int registros_importados = importarDatosDesdeArchivo(zonas, *numZonasPtr, nombre_archivo);
            if (registros_importados > 0) {
                printf(VERDE "\n¡PERFECTO! %d registros importados\n" RESET, registros_importados);
                printf(AMARILLO "Datos organizados automaticamente por mes\n" RESET);
                printf(CIAN "Ya puede ver reportes y alertas con sus datos\n" RESET);
            } else {
                printf(ROJO "No se pudo importar. Verifique el formato del archivo\n" RESET);
                printf(AMARILLO "Formato requerido: Zona,Fecha,CO2,SO2,NO2,PM25\n" RESET);
            }
        } else if (opcion_menu == 6) {
            printf(AMARILLO "Fecha actual del sistema: %02d/%02d/%d\n" RESET, 
                   config_fechas.dia_inicio, config_fechas.mes_inicio, config_fechas.anio_inicio);
            printf("Ingrese nueva fecha:\n");
            config_fechas.anio_inicio = leerEnteroSeguro("Anio: ", 1900, 2100);
            config_fechas.mes_inicio = leerEnteroSeguro("Mes (1-12): ", 1, 12);
            config_fechas.dia_inicio = leerEnteroSeguro("Dia (1-31): ", 1, 31);
            
            printf(VERDE "Nueva fecha establecida: %02d/%02d/%d\n" RESET, 
                   config_fechas.dia_inicio, config_fechas.mes_inicio, config_fechas.anio_inicio);
            
            guardarConfiguracionFechas(&config_fechas);
        } else if (opcion_menu == 7) {
            break;
        } else {
            printf(ROJO "Opcion invalida.\n" RESET);
        }
    }
}

// --- Reporte mensual decorado con colores por nivel de peligrosidad ---
void mostrarReporteMensual(struct Zona *zona, int mes) {
    if (mes >= zona->numMeses) {
        printf(ROJO "Mes no disponible.\n" RESET);
        return;
    }
    
    if (zona->meses[mes].numDias == 0) {
        printf(AMARILLO "No hay datos para el mes %d en la zona %s.\n" RESET, mes + 1, zona->nombre);
        return;
    }
    
    int totalDias = zona->meses[mes].numDias;
    int diasPorPagina = 7;
    int totalPaginas = (totalDias + diasPorPagina - 1) / diasPorPagina;
    
    float sumCO2=0, sumSO2=0, sumNO2=0, sumPM25=0;
    int verde=0, amarilla=0, naranja=0, roja=0;
    
    // Calcular estadisticas generales
    for (int d = 0; d < totalDias; d++) {
        struct DatosAmbientales *dia = &zona->meses[mes].dias[d];
        sumCO2 += dia->co2;
        sumSO2 += dia->so2;
        sumNO2 += dia->no2;
        sumPM25 += dia->pm25;
        
        char* nivel_pm25 = obtenerNivelPeligrosidad(dia->pm25, TIPO_PM25);
        if (strcmp(nivel_pm25, "Bueno") == 0) verde++;
        else if (strcmp(nivel_pm25, "Moderado") == 0) amarilla++;
        else if (strcmp(nivel_pm25, "Alto") == 0) naranja++;
        else roja++;
    }
    
    // Mostrar datos página por página
    for (int pagina = 0; pagina < totalPaginas; pagina++) {
        int diaInicio = pagina * diasPorPagina;
        int diaFin = (diaInicio + diasPorPagina < totalDias) ? diaInicio + diasPorPagina : totalDias;
        
        printf(NEGRITA AZUL "\n+-----+------------+--------+--------+--------+--------+--------------------+\n" RESET);
        printf(NEGRITA AZUL "| Dia |   Fecha    |  CO2   |  SO2   |  NO2   | PM2.5  | Nivel PM2.5        |\n" RESET);
        printf(NEGRITA AZUL "+-----+------------+--------+--------+--------+--------+--------------------+\n" RESET);
        
        for (int d = diaInicio; d < diaFin; d++) {
            struct DatosAmbientales *dia = &zona->meses[mes].dias[d];
            
            // Verificar si el día tiene datos válidos
            if (strlen(dia->fecha) == 0 || dia->co2 == 0 && dia->so2 == 0 && dia->no2 == 0 && dia->pm25 == 0) {
                printf("| %3d | %10s | %s%6s%s | %s%6s%s | %s%6s%s | %s%6s%s | %s%-18s%s |\n",
                    d+1, "SIN DATOS", 
                    AMARILLO, "N/A", RESET,
                    AMARILLO, "N/A", RESET,
                    AMARILLO, "N/A", RESET,
                    AMARILLO, "N/A", RESET,
                    AMARILLO, "Sin datos", RESET);
            } else {
                // Obtener colores para cada contaminante
                const char* color_co2 = obtenerColorContaminante(dia->co2, TIPO_CO2);
                const char* color_so2 = obtenerColorContaminante(dia->so2, TIPO_SO2);
                const char* color_no2 = obtenerColorContaminante(dia->no2, TIPO_NO2);
                const char* color_pm25 = obtenerColorContaminante(dia->pm25, TIPO_PM25);
                
                // Nivel y color PM2.5
                char* nivel_pm25 = obtenerNivelPeligrosidad(dia->pm25, TIPO_PM25);
                
                printf("| %3d | %10s | %s%6.2f%s | %s%6.1f%s | %s%6.1f%s | %s%6.1f%s | %s%-18s%s |\n",
                    d+1, dia->fecha, 
                    color_co2, dia->co2, RESET,
                    color_so2, dia->so2, RESET,
                    color_no2, dia->no2, RESET,
                    color_pm25, dia->pm25, RESET,
                    color_pm25, nivel_pm25, RESET);
            }
        }
        
        printf(NEGRITA AZUL "+-----+------------+--------+--------+--------+--------+----------------------+\n" RESET);
        printf(NEGRITA CIAN "Pagina %d de %d (dias %d-%d de %d)\n" RESET, pagina + 1, totalPaginas, diaInicio + 1, diaFin, totalDias);
        
        // Solo mostrar estadísticas en la última página
        if (pagina == totalPaginas - 1) {
            // Tabla de promedios
            printf(NEGRITA AZUL "\n+--------------------------------------------------------------------------+\n" RESET);
            printf(NEGRITA AZUL "|                           PROMEDIOS DEL MES                             |\n" RESET);
            printf(NEGRITA AZUL "+--------------------------------------------------------------------------+\n" RESET);
            printf(NEGRITA AZUL "| Contaminante |   Promedio   |     Unidad     |        Estado            |\n" RESET);
            printf(NEGRITA AZUL "+--------------------------------------------------------------------------+\n" RESET);
            
            float promCO2 = sumCO2/totalDias;
            float promSO2 = sumSO2/totalDias;
            float promNO2 = sumNO2/totalDias;
            float promPM25 = sumPM25/totalDias;
            
            const char* colorCO2 = obtenerColorContaminante(promCO2, TIPO_CO2);
            const char* colorSO2 = obtenerColorContaminante(promSO2, TIPO_SO2);
            const char* colorNO2 = obtenerColorContaminante(promNO2, TIPO_NO2);
            const char* colorPM25 = obtenerColorContaminante(promPM25, TIPO_PM25);
            
            char* estadoCO2 = obtenerNivelPeligrosidad(promCO2, TIPO_CO2);
            char* estadoSO2 = obtenerNivelPeligrosidad(promSO2, TIPO_SO2);
            char* estadoNO2 = obtenerNivelPeligrosidad(promNO2, TIPO_NO2);
            char* estadoPM25 = obtenerNivelPeligrosidad(promPM25, TIPO_PM25);
            
            printf("| %-12s | %s%10.2f%s | %-14s | %s%-20s%s |\n", 
                   "CO2", colorCO2, promCO2, RESET, "ppm", colorCO2, estadoCO2, RESET);
            printf("| %-12s | %s%10.2f%s | %-14s | %s%-20s%s |\n", 
                   "SO2", colorSO2, promSO2, RESET, "ug/m3", colorSO2, estadoSO2, RESET);
            printf("| %-12s | %s%10.2f%s | %-14s | %s%-20s%s |\n", 
                   "NO2", colorNO2, promNO2, RESET, "ug/m3", colorNO2, estadoNO2, RESET);
            printf("| %-12s | %s%10.2f%s | %-14s | %s%-20s%s |\n", 
                   "PM2.5", colorPM25, promPM25, RESET, "ug/m3", colorPM25, estadoPM25, RESET);
            printf(NEGRITA AZUL "+--------------------------------------------------------------------------+\n" RESET);
            
            // Tabla de resumen de alertas PM2.5
            printf(NEGRITA AZUL "\n+--------------------------------------------------------------------------+\n" RESET);
            printf(NEGRITA AZUL "|                      RESUMEN DE ALERTAS PM2.5                           |\n" RESET);
            printf(NEGRITA AZUL "+--------------------------------------------------------------------------+\n" RESET);
            printf(NEGRITA AZUL "| Nivel Alerta |   Días   | Porcentaje |           Descripción            |\n" RESET);
            printf(NEGRITA AZUL "+--------------------------------------------------------------------------+\n" RESET);
            
            float porcVerde = (verde * 100.0) / totalDias;
            float porcAmarilla = (amarilla * 100.0) / totalDias;
            float porcNaranja = (naranja * 100.0) / totalDias;
            float porcRoja = (roja * 100.0) / totalDias;
            
            printf("| %sBUENO%s        | %8d | %9.1f%% | %-32s |\n", 
                   VERDE, RESET, verde, porcVerde, "Calidad del aire satisfactoria");
            printf("| %sMODERADO%s     | %8d | %9.1f%% | %-32s |\n", 
                   AMARILLO, RESET, amarilla, porcAmarilla, "Aceptable para mayoria");
            printf("| %sALTO%s         | %8d | %9.1f%% | %-32s |\n", 
                   MAGENTA, RESET, naranja, porcNaranja, "Danino para grupos sensibles");
            printf("| %sPELIGROSO%s     | %8d | %9.1f%% | %-32s |\n", 
                   ROJO, RESET, roja, porcRoja, "Danino para toda la poblacion");
            printf(NEGRITA AZUL "+--------------------------------------------------------------------------+\n" RESET);
            
            // Evaluación general del mes
            printf(NEGRITA AZUL "\n+--------------------------------------------------------------------------+\n" RESET);
            printf(NEGRITA AZUL "|                        EVALUACION GENERAL DEL MES                       |\n" RESET);
            printf(NEGRITA AZUL "+--------------------------------------------------------------------------+\n" RESET);
            
            if (roja > totalDias * 0.3) {
                printf(ROJO "| MES CRITICO: Mas del 30%% de dias con niveles peligrosos              |\n" RESET);
            } else if (naranja + roja > totalDias * 0.5) {
                printf(AMARILLO "| MES PREOCUPANTE: Mas del 50%% de dias con niveles altos               |\n" RESET);
            } else if (verde > totalDias * 0.7) {
                printf(VERDE "| MES EXCELENTE: Mas del 70%% de dias con buena calidad del aire        |\n" RESET);
            } else {
                printf(CIAN "| MES REGULAR: Calidad del aire variable, requiere monitoreo            |\n" RESET);
            }
            
            printf(NEGRITA AZUL "+--------------------------------------------------------------------------+\n" RESET);
        }
        
        // Pausa entre páginas (excepto la última)
        if (pagina < totalPaginas - 1) {
            printf(AMARILLO "Presione cualquier tecla para continuar a la siguiente pagina...\n" RESET);
            getchar();
        }
    }
}

// --- Menú de reportes con tabla colorizada ---
void menuReportes(struct Zona zonas[], int numZonas) {
    while (1) {
        printf(CIAN "\n+--------------------------------------+\n");
        printf("|      " NEGRITA "MENU DE REPORTES" RESET CIAN "                |\n");
        printf("+--------------------------------------+\n");
        printf("| 1. Reporte mensual por zona          |\n");
        printf("| 2. Tabla resumen de todas las zonas  |\n");
        printf("| 3. Mostrar alertas y recomendaciones |\n");
        printf("| 4. Exportar tabla de zonas           |\n");
        printf("| 5. Exportar alertas y recomendaciones|\n");
        printf("| 6. Volver                            |\n");
        printf("+--------------------------------------+\n" RESET);
        
        int opcion_menu = leerEnteroSeguro("Seleccione opcion: ", 1, 6);
        
        if (opcion_menu == 1) {
            printf(AMARILLO "Zonas disponibles:\n" RESET);
            printf("+----+------------------+\n");
            printf("| %-2s | %-16s |\n", "N", "Zona");
            printf("+----+------------------+\n");
            for (int i = 0; i < numZonas; i++) {
                printf("| %-2d | %-16s |\n", i+1, zonas[i].nombre);
            }
            printf("+----+------------------+\n");
            
            int zona_seleccionada = leerEnteroSeguro("Seleccione zona: ", 1, numZonas);
            if (zona_seleccionada >= 1 && zona_seleccionada <= numZonas) {
                int maximo_meses = zonas[zona_seleccionada-1].numMeses;
                if (maximo_meses <= 0) maximo_meses = 1;
                
                char mensaje_mes[64];
                snprintf(mensaje_mes, sizeof(mensaje_mes), "Mes (1-%d): ", maximo_meses);
                int mes_seleccionado = leerEnteroSeguro(mensaje_mes, 1, maximo_meses);
                mes_seleccionado = mes_seleccionado - 1;
                
                if (mes_seleccionado >= 0 && mes_seleccionado < MAX_MESES) {
                    mostrarReporteMensual(&zonas[zona_seleccionada-1], mes_seleccionado);
                } else {
                    printf(ROJO "Mes invalido.\n" RESET);
                }
            } else {
                printf(ROJO "Zona invalida.\n" RESET);
            }
        } else if (opcion_menu == 2) {
            // Tabla resumen con colores por peligrosidad
            printf(VERDE "\n+----+--------------------+------------+------------+------------+------------+\n");
            printf("| %-2s | %-18s | %-10s | %-10s | %-10s | %-10s |\n", "N", "Zona", "CO2", "SO2", "NO2", "PM2.5");
            printf("+----+--------------------+------------+------------+------------+------------+\n");
            
            for (int i = 0; i < numZonas; i++) {
                int mes_ultimo = zonas[i].numMeses-1;
                if (mes_ultimo < 0) mes_ultimo = 0;
                
                float suma_co2=0, suma_so2=0, suma_no2=0, suma_pm25=0;
                int numero_dias = zonas[i].meses[mes_ultimo].numDias;
                if (numero_dias == 0) numero_dias = 1;
                
                for (int d = 0; d < numero_dias; d++) {
                    struct DatosAmbientales *datos_dia = &zonas[i].meses[mes_ultimo].dias[d];
                    suma_co2 += datos_dia->co2;
                    suma_so2 += datos_dia->so2;
                    suma_no2 += datos_dia->no2;
                    suma_pm25 += datos_dia->pm25;
                }
                
                float prom_co2 = suma_co2/numero_dias;
                float prom_so2 = suma_so2/numero_dias;
                float prom_no2 = suma_no2/numero_dias;
                float prom_pm25 = suma_pm25/numero_dias;
                
                // Obtener colores
                const char* color_co2 = obtenerColorContaminante(prom_co2, TIPO_CO2);
                const char* color_so2 = obtenerColorContaminante(prom_so2, TIPO_SO2);
                const char* color_no2 = obtenerColorContaminante(prom_no2, TIPO_NO2);
                const char* color_pm25 = obtenerColorContaminante(prom_pm25, TIPO_PM25);
                
                printf("| %-2d | %-18s | %s%10.1f%s | %s%10.1f%s | %s%10.1f%s | %s%10.1f%s |\n",
                    i+1, zonas[i].nombre,
                    color_co2, prom_co2, RESET,
                    color_so2, prom_so2, RESET,
                    color_no2, prom_no2, RESET,
                    color_pm25, prom_pm25, RESET);
            }
            printf("+----+--------------------+------------+------------+------------+------------+\n" RESET);
            
            // Leyenda de colores
            printf(NEGRITA "\nLeyenda de colores:\n" RESET);
            printf("%sBUENO%s - %sMODERADO%s - %sALTO%s - %sPELIGROSO%s\n",
                   VERDE, RESET, AMARILLO, RESET, MAGENTA, RESET, ROJO, RESET);
                   
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

// --- Generar datos de muestreo para un mes completo ---
void generarDatosAleatoriosMes(struct Zona *zona, int mes) {
    // Dias por mes
    int dias_mes[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int max_dias = dias_mes[mes];
    
    // Ajustar anio bisiesto
    extern struct ConfiguracionFechas config_fechas;
    if (mes == 1 && ((config_fechas.anio_inicio % 4 == 0 && config_fechas.anio_inicio % 100 != 0) || 
                     (config_fechas.anio_inicio % 400 == 0))) {
        max_dias = 29;
    }
    
    // Datos base segun la ciudad
    float base_pm25[] = {10.0f, 25.0f, 45.0f, 70.0f, 20.0f};
    float base_co2[] = {390.0f, 410.0f, 440.0f, 470.0f, 450.0f};
    
    int zonaIndex = 0;
    if (strcmp(zona->nombre, "Cuenca") == 0) zonaIndex = 1;
    else if (strcmp(zona->nombre, "Guayaquil") == 0) zonaIndex = 2;
    else if (strcmp(zona->nombre, "Loja") == 0) zonaIndex = 3;
    else if (strcmp(zona->nombre, "Ambato") == 0) zonaIndex = 4;
    
    // Generar datos para todo el mes
    for (int d = 0; d < max_dias; d++) {
        zona->meses[mes].dias[d].pm25 = base_pm25[zonaIndex] + (d * 0.2f);
        zona->meses[mes].dias[d].co2 = base_co2[zonaIndex] + (d * 1.0f);
        zona->meses[mes].dias[d].so2 = 5.0f + (d * 0.3f);
        zona->meses[mes].dias[d].no2 = 20.0f + (d * 2.0f);
        
        // Generar fecha
        snprintf(zona->meses[mes].dias[d].fecha, sizeof(zona->meses[mes].dias[d].fecha), 
                "%04d-%02d-%02d", config_fechas.anio_inicio, mes+1, d+1);
    }

    zona->meses[mes].numDias = max_dias;
    
    if (mes >= zona->numMeses) {
        zona->numMeses = mes + 1;
    }
    
    printf(VERDE "Generados %d dias completos para %s mes %d\n" RESET, 
           max_dias, zona->nombre, mes + 1);
}

// --- Menu de siguiente mes ---
void menuSiguienteMes(struct Zona zonas[], int numZonas, int mesActual[]) {
    printf(AMARILLO "\n=== SIGUIENTE MES ===\n" RESET);
    printf("Avanzando al siguiente mes para todas las zonas...\n");
    
    // Encontrar el mes mas alto existente
    int mes_maximo = 0;
    for (int i = 0; i < numZonas; i++) {
        if (zonas[i].numMeses > mes_maximo) {
            mes_maximo = zonas[i].numMeses;
        }
    }
    
    // El proximo mes consecutivo sera el mes_maximo (no volver al mes 0)
    int proximo_mes = mes_maximo;
    
    // Verificar que no exceda el limite
    if (proximo_mes >= MAX_MESES) {
        printf(ROJO "No se puede avanzar mas. Limite de meses alcanzado (%d).\n" RESET, MAX_MESES);
        printf("Presione Enter para continuar...\n");
        getchar();
        return;
    }
    
    // Avanzar el mes para todas las zonas al siguiente consecutivo
    for (int i = 0; i < numZonas; i++) {
        mesActual[i] = proximo_mes;
        
        // Asegurar que la zona tenga suficientes meses
        if (zonas[i].numMeses <= proximo_mes) {
            zonas[i].numMeses = proximo_mes + 1;
        }
    }
    
    // Guardar el nuevo estado
    guardarMesActual(mesActual, numZonas);
    printf(VERDE "Todas las zonas han avanzado al mes %d.\n" RESET, proximo_mes + 1);
    printf("Presione Enter para continuar...\n");
    getchar();
}

// --- Menu de ingreso manual ---
void menuIngresoManual(struct Zona zonas[], int numZonas, int mesActual[]) {
    printf(AMARILLO "Zonas disponibles:\n" RESET);
    printf("+----+----------------------+------------+\n");
    printf("| %-2s | %-20s | %-10s |\n", "N", "Zona", "Mes");
    printf("+----+----------------------+------------+\n");
    for (int i = 0; i < numZonas; i++) {
        printf("| %-2d | %-20s | %-10d |\n", i+1, zonas[i].nombre, mesActual[i]+1);
    }
    printf("+----+----------------------+------------+\n");
    
    int zonaSel = leerEnteroSeguro("\nSeleccione zona (0 para cancelar): ", 0, numZonas);
    
    if (zonaSel == 0) {
        return;
    }
    
    if (zonaSel >= 1 && zonaSel <= numZonas) {
        ingresarDatosManualMes(&zonas[zonaSel-1], mesActual[zonaSel-1], &config_fechas);
        guardarMesActual(mesActual, numZonas);
    } else {
        printf(ROJO "Zona invalida.\n" RESET);
    }
}

void cargarSemanaTemporal(struct Zona *zona, int semana) {
    char nombre_archivo[64];
    snprintf(nombre_archivo, sizeof(nombre_archivo), "temp_%s_semana%d.dat", zona->nombre, semana + 1);
    
    FILE *archivo = fopen(nombre_archivo, "rb");
    if (archivo) {
        fread(&zona->semanas[semana], sizeof(struct Semana), 1, archivo);
        fclose(archivo);
    }
}

void guardarSemanaTemporal(struct Zona *zona, int semana) {
    char nombre_archivo[64];
    snprintf(nombre_archivo, sizeof(nombre_archivo), "temp_%s_semana%d.dat", zona->nombre, semana + 1);
    
    FILE *archivo = fopen(nombre_archivo, "wb");
    if (archivo) {
        fwrite(&zona->semanas[semana], sizeof(struct Semana), 1, archivo);
        fclose(archivo);
    }
}

void limpiarDatosTemporales(struct Zona *zona, int semana) {
    char nombre_archivo[64];
    snprintf(nombre_archivo, sizeof(nombre_archivo), "temp_%s_semana%d.dat", zona->nombre, semana + 1);
    remove(nombre_archivo);
}

// --- Menú de checkpoints ---
void menuCheckpoints(struct Zona zonas[], int numZonas, int mesActual[]) {
    (void)mesActual; // Suprimir warning de parámetro no usado
    while (1) {
        printf(CIAN "\n+--------------------------------------+\n");
        printf("|      " NEGRITA "MENU DE CHECKPOINTS" RESET CIAN "             |\n");
        printf("+--------------------------------------+\n");
        printf("| 1. Guardar mes actual                |\n");
        printf("| 2. Cargar mes anterior               |\n");
        printf("| 3. Mostrar meses disponibles         |\n");
        printf("| 4. Limpiar datos temporales          |\n");
        printf("| 5. Volver                            |\n");
        printf("+--------------------------------------+\n" RESET);
        
        int op = leerEnteroSeguro("Seleccione opcion: ", 1, 5);
        
        if (op == 1) {
            // Guardar permanentemente el mes actual
            for (int i = 0; i < numZonas; i++) {
                guardarMes(&zonas[i], mesActual[i]);
            }
            // Guardar estado de meses actuales
            guardarMesActual(mesActual, numZonas);
            printf(VERDE "Checkpoint guardado permanentemente para todas las zonas (mes actual).\n" RESET);
            
        } else if (op == 2) {
            // Cargar mes anterior
            int maxMes = 0;
            for (int i = 0; i < numZonas; i++) {
                if (mesActual[i] > maxMes) maxMes = mesActual[i];
            }
            
            if (maxMes == 0) {
                printf(ROJO "Ya está en el primer mes.\n" RESET);
                continue;
            }
            
            char mensaje_mes[64];
            snprintf(mensaje_mes, sizeof(mensaje_mes), "Mes a cargar (1-%d): ", maxMes);
            int mesSel = leerEnteroSeguro(mensaje_mes, 1, maxMes);
            mesSel = mesSel - 1;
            
            for (int i = 0; i < numZonas; i++) {
                cargarMes(&zonas[i], mesSel);
                mesActual[i] = mesSel;
            }
            
            // Guardar nuevo estado
            guardarMesActual(mesActual, numZonas);
            printf(VERDE "Cargado mes %d para todas las zonas.\n" RESET, mesSel+1);
            
        } else if (op == 3) {
            // Mostrar meses disponibles
            printf(AMARILLO "\nMeses disponibles para cada zona:\n" RESET);
            for (int i = 0; i < numZonas; i++) {
                printf("%-12s: ", zonas[i].nombre);
                for (int mes = 0; mes < MAX_MESES; mes++) {
                    char nombre_carpeta[32];
                    char nombre_archivo[96];
                    
                    snprintf(nombre_carpeta, sizeof(nombre_carpeta), "mes_%d", mes + 1);
                    snprintf(nombre_archivo, sizeof(nombre_archivo), "%s\\%s_mes%d.dat", 
                             nombre_carpeta, zonas[i].nombre, mes + 1);
                    
                    FILE *f = fopen(nombre_archivo, "rb");
                    if (f) {
                        fclose(f);
                        printf(VERDE "%d " RESET, mes + 1);
                    } else {
                        printf(ROJO "%d " RESET, mes + 1);
                    }
                }
                printf("\n");
            }
            printf(VERDE "Verde: Mes disponible" RESET " | " ROJO "Rojo: Mes no disponible\n" RESET);
            
        } else if (op == 4) {
            // Limpiar datos temporales
            printf(AMARILLO "Limpiando archivos temporales...\n" RESET);
            for (int i = 0; i < numZonas; i++) {
                for (int mes = 0; mes < MAX_MESES; mes++) {
                    char tempFile[64];
                    snprintf(tempFile, sizeof(tempFile), "temp_%s_mes%d.dat", zonas[i].nombre, mes+1);
                    if (remove(tempFile) == 0) {
                        printf("Eliminado: %s\n", tempFile);
                    }
                }
            }
            printf(VERDE "Limpieza completada.\n" RESET);
            
        } else if (op == 5) {
            break;
        }
        
        printf("Presione Enter para continuar...\n");
        getchar();
    }
}

// --- Menú de pronósticos ---
void menuPronosticos(struct Zona zonas[], int numZonas, int mesActual[]) {
    while (1) {
        printf(CIAN "\n+--------------------------------------+\n");
        printf("|      " NEGRITA "MENU DE PRONOSTICOS" RESET CIAN "               |\n");
        printf("+--------------------------------------+\n");
        printf("| 1. Pronostico por zona               |\n");
        printf("| 2. Pronostico general todas las zonas|\n");
        printf("| 3. Prediccion de alertas PM2.5       |\n");
        printf("| 4. Tendencias de contaminacion       |\n");
        printf("| 5. Volver                            |\n");
        printf("+--------------------------------------+\n" RESET);
        
        int opcion = leerEnteroSeguro("Seleccione opcion: ", 1, 5);
        
        if (opcion == 1) {
            // Pronóstico por zona
            printf(AMARILLO "Zonas disponibles:\n" RESET);
            for (int i = 0; i < numZonas; i++) {
                printf("%d. %s\n", i+1, zonas[i].nombre);
            }
            
            int zona_idx = leerEnteroSeguro("Seleccione zona: ", 1, numZonas) - 1;
            if (zona_idx >= 0 && zona_idx < numZonas) {
                generarPronosticoZona(&zonas[zona_idx], mesActual[zona_idx]);
            }
            
        } else if (opcion == 2) {
            // Pronóstico general
            generarPronosticoGeneral(zonas, numZonas, mesActual);
            
        } else if (opcion == 3) {
            // Predicción de alertas PM2.5
            predecirAlertasPM25(zonas, numZonas, mesActual);
            
        } else if (opcion == 4) {
            // Tendencias
            mostrarTendenciasContaminacion(zonas, numZonas, mesActual);
            
        } else if (opcion == 5) {
            break;
        }
    }
}

// Funcion para generar pronostico de una zona especifica
void generarPronosticoZona(struct Zona *zona, int mesActual) {
    if (mesActual >= zona->numMeses || zona->meses[mesActual].numDias == 0) {
        printf(ROJO "No hay datos suficientes para generar pronostico en esta zona.\n" RESET);
        return;
    }
    
    // Validar dias minimos para pronostico confiable
    int diasMinimos = 5; // Minimo 5 dias para calculo de tendencia
    if (zona->meses[mesActual].numDias < diasMinimos) {
        printf(ROJO "ADVERTENCIA: Solo hay %d dias de datos. Se requieren al menos %d dias para un pronostico confiable.\n" RESET, 
               zona->meses[mesActual].numDias, diasMinimos);
        printf(AMARILLO "Mostrando pronostico con datos limitados...\n" RESET);
    }
    
    printf(NEGRITA AZUL "\n+----------------------------------------------------------+\n");
    printf("|              PRONOSTICO PARA ZONA: %-20s|\n", zona->nombre);
    printf("+----------------------------------------------------------+\n" RESET);
    
    // Verificar que los datos son reales y no de ejemplo
    int tiene_datos_reales = 0;
    for (int d = 0; d < zona->meses[mesActual].numDias; d++) {
        if (strlen(zona->meses[mesActual].dias[d].fecha) > 0) {
            tiene_datos_reales = 1;
            break;
        }
    }
    
    if (!tiene_datos_reales) {
        printf(ROJO "ERROR: No se encontraron datos reales para esta zona.\n" RESET);
        printf(AMARILLO "Importe datos reales o use 'Configuracion > Generar datos de muestreo'.\n" RESET);
        return;
    }
    
    // Calcular promedios del mes actual
    float sumCO2 = 0, sumSO2 = 0, sumNO2 = 0, sumPM25 = 0;
    int numDias = zona->meses[mesActual].numDias;
    
    for (int d = 0; d < numDias; d++) {
        sumCO2 += zona->meses[mesActual].dias[d].co2;
        sumSO2 += zona->meses[mesActual].dias[d].so2;
        sumNO2 += zona->meses[mesActual].dias[d].no2;
        sumPM25 += zona->meses[mesActual].dias[d].pm25;
    }
    
    float promCO2 = sumCO2 / numDias;
    float promSO2 = sumSO2 / numDias;
    float promNO2 = sumNO2 / numDias;
    float promPM25 = sumPM25 / numDias;
    
    // Calcular tendencia (variación entre primeros y últimos días)
    float tendenciaCO2 = 0, tendenciaSO2 = 0, tendenciaNO2 = 0, tendenciaPM25 = 0;
    
    if (numDias >= diasMinimos) {
        float inicialCO2 = 0, inicialSO2 = 0, inicialNO2 = 0, inicialPM25 = 0;
        float finalCO2 = 0, finalSO2 = 0, finalNO2 = 0, finalPM25 = 0;
        
        int diasParaTendencia = (numDias >= 7) ? 3 : 1; // Usar 3 días si hay 7+, sino 1
        
        // Promedio de primeros días
        for (int d = 0; d < diasParaTendencia; d++) {
            inicialCO2 += zona->meses[mesActual].dias[d].co2;
            inicialSO2 += zona->meses[mesActual].dias[d].so2;
            inicialNO2 += zona->meses[mesActual].dias[d].no2;
            inicialPM25 += zona->meses[mesActual].dias[d].pm25;
        }
        
        // Promedio de últimos días
        for (int d = numDias - diasParaTendencia; d < numDias; d++) {
            finalCO2 += zona->meses[mesActual].dias[d].co2;
            finalSO2 += zona->meses[mesActual].dias[d].so2;
            finalNO2 += zona->meses[mesActual].dias[d].no2;
            finalPM25 += zona->meses[mesActual].dias[d].pm25;
        }
        
        tendenciaCO2 = (finalCO2 / diasParaTendencia) - (inicialCO2 / diasParaTendencia);
        tendenciaSO2 = (finalSO2 / diasParaTendencia) - (inicialSO2 / diasParaTendencia);
        tendenciaNO2 = (finalNO2 / diasParaTendencia) - (inicialNO2 / diasParaTendencia);
        tendenciaPM25 = (finalPM25 / diasParaTendencia) - (inicialPM25 / diasParaTendencia);
    }
    
    // Pronóstico para próximos días (con tendencia)
    float pronosticoCO2 = promCO2 + tendenciaCO2;
    float pronosticoSO2 = promSO2 + tendenciaSO2;
    float pronosticoNO2 = promNO2 + tendenciaNO2;
    float pronosticoPM25 = promPM25 + tendenciaPM25;
    
    printf("Promedios actuales (basados en %d dias de datos reales):\n", numDias);
    printf("  CO2: %.2f ppm, SO2: %.2f ug/m3, NO2: %.2f ug/m3, PM2.5: %.2f ug/m3\n",
           promCO2, promSO2, promNO2, promPM25);
    
    printf(AMARILLO "\nPronostico para proximos dias:\n" RESET);
    printf("  CO2: %.2f ppm", pronosticoCO2);
    printf(" %s(%.2f%s)\n", tendenciaCO2 > 0 ? ROJO "↑" : VERDE "↓", tendenciaCO2, RESET);
    printf("  SO2: %.2f ug/m3", pronosticoSO2);
    printf(" %s(%.2f%s)\n", tendenciaSO2 > 0 ? ROJO "↑" : VERDE "↓", tendenciaSO2, RESET);
    printf("  NO2: %.2f ug/m3", pronosticoNO2);
    printf(" %s(%.2f%s)\n", tendenciaNO2 > 0 ? ROJO "↑" : VERDE "↓", tendenciaNO2, RESET);
    printf("  PM2.5: %.2f ug/m3", pronosticoPM25);
    printf(" %s(%.2f%s)\n", tendenciaPM25 > 0 ? ROJO "↑" : VERDE "↓", tendenciaPM25, RESET);
    
    // Evaluacion del pronostico
    printf(NEGRITA "\nEvaluacion del pronostico:\n" RESET);
    if (pronosticoPM25 > 75) {
        printf(ROJO "- Alerta alta: PM2.5 puede alcanzar niveles peligrosos\n" RESET);
    } else if (pronosticoPM25 > 35) {
        printf(AMARILLO "- Precaucion: PM2.5 puede estar en niveles moderados\n" RESET);
    } else {
        printf(VERDE "- Calidad del aire aceptable para PM2.5\n" RESET);
    }
    
    if (pronosticoCO2 > 800) {
        printf(ROJO "- Alerta: CO2 puede alcanzar niveles altos\n" RESET);
    } else if (pronosticoCO2 > 600) {
        printf(AMARILLO "- Precaucion: CO2 en niveles moderados\n" RESET);
    } else {
        printf(VERDE "- Niveles de CO2 aceptables\n" RESET);
    }
    
    printf(NEGRITA AZUL "+----------------------------------------------------------+\n" RESET);
}

// Funcion para generar pronostico general
void generarPronosticoGeneral(struct Zona zonas[], int numZonas, int mesActual[]) {
    printf(NEGRITA AZUL "\n+----------------------------------------------------------+\n");
    printf("|                  PRONOSTICO GENERAL                      |\n");
    printf("+----------------------------------------------------------+\n" RESET);
    
    float totalCO2 = 0, totalSO2 = 0, totalNO2 = 0, totalPM25 = 0;
    int totalDias = 0;
    int zonasConDatos = 0;
    int zonasConDatosReales = 0;
    int diasMinimos = 4; // Minimo 4 dias para cada zona
    
    // Validar que hay datos reales suficientes
    for (int i = 0; i < numZonas; i++) {
        if (mesActual[i] < zonas[i].numMeses && zonas[i].meses[mesActual[i]].numDias >= diasMinimos) {
            // Verificar que los datos son reales
            int tiene_datos_reales = 0;
            for (int d = 0; d < zonas[i].meses[mesActual[i]].numDias; d++) {
                if (strlen(zonas[i].meses[mesActual[i]].dias[d].fecha) > 0) {
                    tiene_datos_reales = 1;
                    break;
                }
            }
            
            if (tiene_datos_reales) {
                zonasConDatosReales++;
            }
        }
    }
    
    if (zonasConDatosReales < 3) {
        printf(ROJO "ERROR: Se requieren al menos 3 zonas con datos reales para el pronostico general.\n" RESET);
        printf(AMARILLO "Actualmente hay %d zonas con datos validos.\n" RESET, zonasConDatosReales);
        printf(AMARILLO "Importe datos reales o use 'Configuracion > Generar datos de muestreo'.\n" RESET);
        return;
    }
    
    // Continuar con el calculo si hay suficientes datos
    for (int i = 0; i < numZonas; i++) {
        if (mesActual[i] < zonas[i].numMeses && zonas[i].meses[mesActual[i]].numDias >= diasMinimos) {
            // Verificar que los datos son reales
            int tiene_datos_reales = 0;
            for (int d = 0; d < zonas[i].meses[mesActual[i]].numDias; d++) {
                if (strlen(zonas[i].meses[mesActual[i]].dias[d].fecha) > 0) {
                    tiene_datos_reales = 1;
                    break;
                }
            }
            
            if (tiene_datos_reales) {
                zonasConDatosReales++;
                zonasConDatos++;
                int numDias = zonas[i].meses[mesActual[i]].numDias;
                totalDias += numDias;
                
                for (int d = 0; d < numDias; d++) {
                    totalCO2 += zonas[i].meses[mesActual[i]].dias[d].co2;
                    totalSO2 += zonas[i].meses[mesActual[i]].dias[d].so2;
                    totalNO2 += zonas[i].meses[mesActual[i]].dias[d].no2;
                    totalPM25 += zonas[i].meses[mesActual[i]].dias[d].pm25;
                }
            }
        }
    }
    
    if (zonasConDatosReales == 0) {
        printf(ROJO "ERROR: No se encontraron datos reales en ninguna zona.\n" RESET);
        printf(AMARILLO "Importe datos reales o use 'Configuracion > Generar datos de muestreo'.\n" RESET);
        return;
    }
    
    if (zonasConDatosReales < 2) {
        printf(AMARILLO "ADVERTENCIA: Solo %d zona con datos reales. Se requieren al menos 2 zonas para un pronostico general confiable.\n" RESET, zonasConDatosReales);
    }
    
    float promedioCO2 = totalCO2 / totalDias;
    float promedioSO2 = totalSO2 / totalDias;
    float promedioNO2 = totalNO2 / totalDias;
    float promedioPM25 = totalPM25 / totalDias;
    
    printf("Promedios regionales (basados en %d dias de %d zonas con datos reales):\n", totalDias, zonasConDatosReales);
    printf("  CO2: %.2f ppm\n", promedioCO2);
    printf("  SO2: %.2f ug/m3\n", promedioSO2);
    printf("  NO2: %.2f ug/m3\n", promedioNO2);
    printf("  PM2.5: %.2f ug/m3\n", promedioPM25);
    
    // Evaluación general
    printf(NEGRITA "\nEvaluacion general de la region:\n" RESET);
    if (promedioPM25 > 75) {
        printf(ROJO "- Calidad del aire PELIGROSA en la region\n" RESET);
    } else if (promedioPM25 > 35) {
        printf(AMARILLO "- Calidad del aire MODERADA en la region\n" RESET);
    } else {
        printf(VERDE "- Calidad del aire BUENA en la region\n" RESET);
    }
    
    printf("Zonas analizadas: %d de %d (con datos reales)\n", zonasConDatosReales, numZonas);
    printf(NEGRITA AZUL "+----------------------------------------------------------+\n" RESET);
}

// Funcion para predecir alertas PM2.5
void predecirAlertasPM25(struct Zona zonas[], int numZonas, int mesActual[]) {
    printf(NEGRITA AZUL "\n+----------------------------------------------------------+\n");
    printf("|              PREDICCION DE ALERTAS PM2.5                |\n");
    printf("+----------------------------------------------------------+\n" RESET);
    
    int alertas_altas = 0;
    int alertas_moderadas = 0;
    int zonas_buenas = 0;
    int zonas_con_datos_reales = 0;
    int diasMinimos = 3; // Minimo 3 dias para calcular alertas
    
    for (int i = 0; i < numZonas; i++) {
        if (mesActual[i] < zonas[i].numMeses && zonas[i].meses[mesActual[i]].numDias >= diasMinimos) {
            // Verificar que los datos son reales
            int tiene_datos_reales = 0;
            for (int d = 0; d < zonas[i].meses[mesActual[i]].numDias; d++) {
                if (strlen(zonas[i].meses[mesActual[i]].dias[d].fecha) > 0) {
                    tiene_datos_reales = 1;
                    break;
                }
            }
            
            if (tiene_datos_reales) {
                zonas_con_datos_reales++;
                
                if (zonas[i].meses[mesActual[i]].numDias < diasMinimos) {
                    printf("%-20s ", zonas[i].nombre);
                    printf(AMARILLO "DATOS INSUFICIENTES (%d dias, min %d)\n" RESET, 
                           zonas[i].meses[mesActual[i]].numDias, diasMinimos);
                    continue;
                }
                
                float sumPM25 = 0;
                int numDias = zonas[i].meses[mesActual[i]].numDias;
                
                for (int d = 0; d < numDias; d++) {
                    sumPM25 += zonas[i].meses[mesActual[i]].dias[d].pm25;
                }
                
                float promedioPM25 = sumPM25 / numDias;
                
                printf("%-20s PM2.5: %.2f - ", zonas[i].nombre, promedioPM25);
                
                if (promedioPM25 > 75) {
                    printf(ROJO "ALERTA ALTA\n" RESET);
                    alertas_altas++;
                } else if (promedioPM25 > 35) {
                    printf(AMARILLO "ALERTA MODERADA\n" RESET);
                    alertas_moderadas++;
                } else {
                    printf(VERDE "BUENA\n" RESET);
                    zonas_buenas++;
                }
            }
        }
    }
    
    if (zonas_con_datos_reales == 0) {
        printf(ROJO "ERROR: No se encontraron datos reales en ninguna zona.\n" RESET);
        printf(AMARILLO "Importe datos reales o use 'Configuracion > Generar datos de muestreo'.\n" RESET);
        return;
    }
    
    printf(NEGRITA "\nResumen de alertas (basado en %d zonas con datos reales):\n" RESET, zonas_con_datos_reales);
    printf(ROJO "Alertas altas: %d\n" RESET, alertas_altas);
    printf(AMARILLO "Alertas moderadas: %d\n" RESET, alertas_moderadas);
    printf(VERDE "Zonas buenas: %d\n" RESET, zonas_buenas);
    
    printf(NEGRITA AZUL "+----------------------------------------------------------+\n" RESET);
}

// Función para mostrar tendencias
void mostrarTendenciasContaminacion(struct Zona zonas[], int numZonas, int mesActual[]) {
    printf(NEGRITA AZUL "\n+----------------------------------------------------------+\n");
    printf("|             TENDENCIAS DE CONTAMINACION                 |\n");
    printf("+----------------------------------------------------------+\n" RESET);
    
    for (int i = 0; i < numZonas; i++) {
        if (mesActual[i] < zonas[i].numMeses && zonas[i].meses[mesActual[i]].numDias >= 7) {
            int numDias = zonas[i].meses[mesActual[i]].numDias;
            
            // Calcular tendencia de PM2.5
            float inicialPM25 = 0, finalPM25 = 0;
            
            for (int d = 0; d < 3; d++) {
                inicialPM25 += zonas[i].meses[mesActual[i]].dias[d].pm25;
            }
            
            for (int d = numDias - 3; d < numDias; d++) {
                finalPM25 += zonas[i].meses[mesActual[i]].dias[d].pm25;
            }
            
            float tendencia = (finalPM25 / 3) - (inicialPM25 / 3);
            
            printf("%-20s PM2.5: ", zonas[i].nombre);
            if (tendencia > 5) {
                printf(ROJO "↑ EMPEORANDO (+%.2f)\n" RESET, tendencia);
            } else if (tendencia < -5) {
                printf(VERDE "↓ MEJORANDO (%.2f)\n" RESET, tendencia);
            } else {
                printf(AMARILLO "→ ESTABLE (%.2f)\n" RESET, tendencia);
            }
        }
    }
    
    printf(NEGRITA AZUL "+----------------------------------------------------------+\n" RESET);
}

// ===== FUNCIONES DE CONFIGURACION DE FECHAS =====

/**
 * Guarda la configuracion de fechas en un archivo
 */
int guardarConfiguracionFechas(struct ConfiguracionFechas *config) {
    crearCarpetaSistema();
    FILE *archivo = fopen("sistema_archivos\\config_fechas.dat", "wb");
    if (!archivo) {
        return 0;
    }
    
    fwrite(config, sizeof(struct ConfiguracionFechas), 1, archivo);
    fclose(archivo);
    return 1;
}

/**
 * Carga la configuracion de fechas desde un archivo
 */
int cargarConfiguracionFechas(struct ConfiguracionFechas *config) {
    FILE *archivo = fopen("sistema_archivos\\config_fechas.dat", "rb");
    if (!archivo) {
        return 0;
    }
    
    fread(config, sizeof(struct ConfiguracionFechas), 1, archivo);
    fclose(archivo);
    return 1;
}

/**
 * Guarda el estado actual de los meses en un archivo
 */
void guardarMesActual(int mesActual[], int numZonas) {
    crearCarpetaSistema();
    FILE *archivo = fopen("sistema_archivos\\mes_actual.dat", "wb");
    if (archivo) {
        fwrite(&numZonas, sizeof(int), 1, archivo);
        fwrite(mesActual, sizeof(int), numZonas, archivo);
        fclose(archivo);
    }
}

/**
 * Carga el estado actual de los meses desde un archivo
 */
void cargarMesActual(int mesActual[], int numZonas) {
    FILE *archivo = fopen("sistema_archivos\\mes_actual.dat", "rb");
    if (archivo) {
        int numZonasCargadas;
        fread(&numZonasCargadas, sizeof(int), 1, archivo);
        if (numZonasCargadas == numZonas) {
            fread(mesActual, sizeof(int), numZonas, archivo);
        }
        fclose(archivo);
    }
}

/**
 * Crea la carpeta para archivos del sistema si no existe
 */

// Función para crear carpetas del sistema
void crearCarpetasReportes() {
    // Crear carpeta principal para reportes
    #ifdef _WIN32
        system("mkdir reportes 2>nul");
        system("mkdir reportes\\tablas 2>nul");
        system("mkdir reportes\\alertas 2>nul");
        system("mkdir reportes\\estadisticas 2>nul");
        system("mkdir reportes\\exportaciones 2>nul");
    #else
        system("mkdir -p reportes/tablas");
        system("mkdir -p reportes/alertas");
        system("mkdir -p reportes/estadisticas");
        system("mkdir -p reportes/exportaciones");
    #endif
}

// Función para generar nombre de archivo único
void generarNombreUnico(const char* carpeta, const char* nombre_base, const char* extension, char* nombre_final) {
    char ruta_completa[256];
    int contador = 1;
    
    // Intentar nombre original
    snprintf(nombre_final, 256, "%s.%s", nombre_base, extension);
    snprintf(ruta_completa, sizeof(ruta_completa), "%s\\%s", carpeta, nombre_final);
    
    FILE* test = fopen(ruta_completa, "r");
    if (test == NULL) {
        // El archivo no existe, usar nombre original
        fclose(test);
        return;
    }
    fclose(test);
    
    // Si existe, buscar nombre disponible
    while (1) {
        snprintf(nombre_final, 256, "%s_%d.%s", nombre_base, contador, extension);
        snprintf(ruta_completa, sizeof(ruta_completa), "%s\\%s", carpeta, nombre_final);
        
        FILE* test = fopen(ruta_completa, "r");
        if (test == NULL) {
            // Nombre disponible encontrado
            return;
        }
        fclose(test);
        contador++;
    }
}

// Función para solicitar nombre de archivo al usuario
void solicitarNombreArchivo(const char* tipo_archivo, char* nombre_usuario) {
    printf(AMARILLO "Ingrese el nombre para el archivo de %s: " RESET, tipo_archivo);
    printf(CIAN "(sin extension, presione Enter para usar nombre por defecto): " RESET);
    
    if (fgets(nombre_usuario, 64, stdin) != NULL) {
        // Eliminar salto de línea
        size_t len = strlen(nombre_usuario);
        if (len > 0 && nombre_usuario[len-1] == '\n') {
            nombre_usuario[len-1] = '\0';
        }
        
        // Si está vacío, usar nombre por defecto
        if (strlen(nombre_usuario) == 0) {
            if (strcmp(tipo_archivo, "tabla de zonas") == 0) {
                strcpy(nombre_usuario, "tabla_zonas");
            } else if (strcmp(tipo_archivo, "alertas y recomendaciones") == 0) {
                strcpy(nombre_usuario, "alertas_recomendaciones");
            } else if (strcmp(tipo_archivo, "estadisticas") == 0) {
                strcpy(nombre_usuario, "estadisticas_mensuales");
            } else {
                strcpy(nombre_usuario, "reporte");
            }
        }
    } else {
        strcpy(nombre_usuario, "reporte");
    }
}

// Implementaciones básicas de funciones faltantes
void mostrarAlertasYRecomendaciones(struct Zona zonas[], int numZonas) {
    printf(AMARILLO "\n=== ALERTAS Y RECOMENDACIONES ===\n" RESET);
    
    for (int i = 0; i < numZonas; i++) {
        printf("\n%s%s:%s\n", NEGRITA, zonas[i].nombre, RESET);
        
        // Verificar PM2.5 más reciente
        float pm25_actual = 0.0f;
        if (zonas[i].numMeses > 0) {
            int mes_actual = zonas[i].numMeses - 1;
            if (zonas[i].meses[mes_actual].numDias > 0) {
                int dia_actual = zonas[i].meses[mes_actual].numDias - 1;
                pm25_actual = zonas[i].meses[mes_actual].dias[dia_actual].pm25;
            }
        }
        
        // Generar alerta según PM2.5
        if (pm25_actual <= 12.0f) {
            printf("  %s✓ Calidad del aire: BUENA%s\n", VERDE, RESET);
        } else if (pm25_actual <= 35.0f) {
            printf("  %s! Calidad del aire: MODERADA%s\n", AMARILLO, RESET);
            printf("  Recomendacion: Limitar actividades al aire libre prolongadas\n");
        } else if (pm25_actual <= 55.0f) {
            printf("  %s⚠ Calidad del aire: NO SALUDABLE%s\n", MAGENTA, RESET);
            printf("  Recomendacion: Evitar actividades extenuantes al aire libre\n");
        } else {
            printf("  %s✗ Calidad del aire: PELIGROSA%s\n", ROJO, RESET);
            printf("  Recomendacion: Evitar todas las actividades al aire libre\n");
        }
    }
}

void exportarReporteTabla(struct Zona zonas[], int numZonas) {
    // Crear carpetas necesarias
    crearCarpetasReportes();
    
    // Solicitar nombre de archivo
    char nombre_usuario[64];
    solicitarNombreArchivo("tabla de zonas", nombre_usuario);
    
    // Generar nombre único
    char nombre_final[256];
    generarNombreUnico("reportes\\tablas", nombre_usuario, "txt", nombre_final);
    
    // Crear ruta completa
    char ruta_completa[512];
    snprintf(ruta_completa, sizeof(ruta_completa), "reportes\\tablas\\%s", nombre_final);
    
    FILE *archivo = fopen(ruta_completa, "w");
    if (!archivo) {
        printf(ROJO "Error: No se pudo crear el archivo de reporte\n" RESET);
        return;
    }
    
    // Obtener fecha y hora actual
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    
    fprintf(archivo, "+------------------------------------------------------------------------------+\n");
    fprintf(archivo, "|               REPORTE COMPLETO DE CALIDAD DEL AIRE - TABLA DE ZONAS         |\n");
    fprintf(archivo, "+------------------------------------------------------------------------------+\n");
    fprintf(archivo, "| Generado: %02d/%02d/%d %02d:%02d:%02d                                            |\n", 
            tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900, 
            tm->tm_hour, tm->tm_min, tm->tm_sec);
    fprintf(archivo, "| Zonas analizadas: %d                                                        |\n", numZonas);
    fprintf(archivo, "+------------------------------------------------------------------------------+\n\n");
    
    for (int i = 0; i < numZonas; i++) {
        fprintf(archivo, "+------------------------------------------------------------------------------+\n");
        fprintf(archivo, "| ZONA: %-70s |\n", zonas[i].nombre);
        fprintf(archivo, "+------------------------------------------------------------------------------+\n");
        fprintf(archivo, "| Meses con datos: %-2d                                                        |\n", zonas[i].numMeses);
        
        if (zonas[i].numMeses > 0) {
            int mes_actual = zonas[i].numMeses - 1;
            fprintf(archivo, "| Dias en el ultimo mes: %-2d                                                 |\n", zonas[i].meses[mes_actual].numDias);
            
            if (zonas[i].meses[mes_actual].numDias > 0) {
                // Calcular estadísticas del mes
                float sumCO2 = 0, sumSO2 = 0, sumNO2 = 0, sumPM25 = 0;
                int verde = 0, amarilla = 0, naranja = 0, roja = 0;
                
                for (int d = 0; d < zonas[i].meses[mes_actual].numDias; d++) {
                    struct DatosAmbientales *dia = &zonas[i].meses[mes_actual].dias[d];
                    sumCO2 += dia->co2;
                    sumSO2 += dia->so2;
                    sumNO2 += dia->no2;
                    sumPM25 += dia->pm25;
                    
                    if (dia->pm25 <= 12) verde++;
                    else if (dia->pm25 <= 35) amarilla++;
                    else if (dia->pm25 <= 75) naranja++;
                    else roja++;
                }
                
                int totalDias = zonas[i].meses[mes_actual].numDias;
                
                fprintf(archivo, "+------------------------------------------------------------------------------+\n");
                fprintf(archivo, "|                            PROMEDIOS DEL MES                                |\n");
                fprintf(archivo, "+------------------------------------------------------------------------------+\n");
                fprintf(archivo, "| CO2:   %8.3f ppm     | SO2:   %8.1f mg/m3                              |\n", sumCO2/totalDias, sumSO2/totalDias);
                fprintf(archivo, "| NO2:   %8.1f mg/m3   | PM2.5: %8.1f mg/m3                              |\n", sumNO2/totalDias, sumPM25/totalDias);
                
                fprintf(archivo, "+------------------------------------------------------------------------------+\n");
                fprintf(archivo, "|                          DISTRIBUCION DE ALERTAS PM2.5                     |\n");
                fprintf(archivo, "+------------------------------------------------------------------------------+\n");
                fprintf(archivo, "| BUENO: %2d dias (%5.1f%%)  | MODERADO: %2d dias (%5.1f%%)                    |\n", 
                        verde, (verde*100.0)/totalDias, amarilla, (amarilla*100.0)/totalDias);
                fprintf(archivo, "| ALTO:  %2d dias (%5.1f%%)  | PELIGROSO: %2d dias (%5.1f%%)                   |\n", 
                        naranja, (naranja*100.0)/totalDias, roja, (roja*100.0)/totalDias);
                
                // Última medición
                int dia_actual = zonas[i].meses[mes_actual].numDias - 1;
                struct DatosAmbientales *ultimo_dato = &zonas[i].meses[mes_actual].dias[dia_actual];
                
                fprintf(archivo, "+------------------------------------------------------------------------------+\n");
                fprintf(archivo, "|                             ULTIMA MEDICION                                 |\n");
                fprintf(archivo, "+------------------------------------------------------------------------------+\n");
                fprintf(archivo, "| Fecha: %-10s                                                        |\n", ultimo_dato->fecha);
                fprintf(archivo, "| CO2: %8.3f ppm  | SO2: %8.1f mg/m3  | Estado: %-15s        |\n", 
                        ultimo_dato->co2, ultimo_dato->so2, 
                        ultimo_dato->pm25 <= 12 ? "BUENO" : ultimo_dato->pm25 <= 35 ? "MODERADO" : ultimo_dato->pm25 <= 75 ? "ALTO" : "PELIGROSO");
                fprintf(archivo, "| NO2: %8.1f mg/m3 | PM2.5: %6.1f mg/m3  |                                |\n", 
                        ultimo_dato->no2, ultimo_dato->pm25);
                
                // Evaluación general
                fprintf(archivo, "+------------------------------------------------------------------------------+\n");
                fprintf(archivo, "|                           EVALUACION GENERAL                                |\n");
                fprintf(archivo, "+------------------------------------------------------------------------------+\n");
                
                if (roja > totalDias * 0.3) {
                    fprintf(archivo, "| ESTADO: CRITICO - Mas del 30%% de dias con niveles peligrosos               |\n");
                } else if (naranja + roja > totalDias * 0.5) {
                    fprintf(archivo, "| ESTADO: PREOCUPANTE - Mas del 50%% de dias con niveles altos               |\n");
                } else if (verde > totalDias * 0.7) {
                    fprintf(archivo, "| ESTADO: EXCELENTE - Mas del 70%% de dias con buena calidad                  |\n");
                } else {
                    fprintf(archivo, "| ESTADO: REGULAR - Calidad variable, requiere monitoreo                     |\n");
                }
            } else {
                fprintf(archivo, "| Sin datos disponibles para el mes actual                                    |\n");
            }
        } else {
            fprintf(archivo, "| Sin datos historicos disponibles                                            |\n");
        }
        
        fprintf(archivo, "+------------------------------------------------------------------------------+\n\n");
    }
    
    // Resumen general
    fprintf(archivo, "+------------------------------------------------------------------------------+\n");
    fprintf(archivo, "|                              RESUMEN REGIONAL                               |\n");
    fprintf(archivo, "+------------------------------------------------------------------------------+\n");
    fprintf(archivo, "Total de zonas monitoreadas: %d\n", numZonas);
    fprintf(archivo, "Archivo generado automaticamente por el Sistema de Gestion de Calidad del Aire\n");
    fprintf(archivo, "Para mas informacion, consulte los reportes individuales por zona.\n");
    
    fclose(archivo);
    printf(VERDE "Tabla de zonas exportada exitosamente a: %s\n" RESET, ruta_completa);
}

void exportarAlertasYRecomendaciones(struct Zona zonas[], int numZonas) {
    // Crear carpetas necesarias
    crearCarpetasReportes();
    
    // Solicitar nombre de archivo
    char nombre_usuario[64];
    solicitarNombreArchivo("alertas y recomendaciones", nombre_usuario);
    
    // Generar nombre único
    char nombre_final[256];
    generarNombreUnico("reportes\\alertas", nombre_usuario, "txt", nombre_final);
    
    // Crear ruta completa
    char ruta_completa[512];
    snprintf(ruta_completa, sizeof(ruta_completa), "reportes\\alertas\\%s", nombre_final);
    
    FILE *archivo = fopen(ruta_completa, "w");
    if (!archivo) {
        printf(ROJO "Error: No se pudo crear el archivo de alertas\n" RESET);
        return;
    }
    
    // Obtener fecha y hora actual
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    
    fprintf(archivo, "+------------------------------------------------------------------------------+\n");
    fprintf(archivo, "|                   ALERTAS Y RECOMENDACIONES DE CALIDAD DEL AIRE             |\n");
    fprintf(archivo, "+------------------------------------------------------------------------------+\n");
    fprintf(archivo, "| Generado: %02d/%02d/%d %02d:%02d:%02d                                            |\n", 
            tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900, 
            tm->tm_hour, tm->tm_min, tm->tm_sec);
    fprintf(archivo, "| Zonas evaluadas: %d                                                         |\n", numZonas);
    fprintf(archivo, "+------------------------------------------------------------------------------+\n\n");
    
    // Estadísticas generales
    int zonas_buenas = 0, zonas_moderadas = 0, zonas_altas = 0, zonas_peligrosas = 0;
    float pm25_promedio_regional = 0.0f;
    int zonas_con_datos = 0;
    
    for (int i = 0; i < numZonas; i++) {
        fprintf(archivo, "+------------------------------------------------------------------------------+\n");
        fprintf(archivo, "| ZONA: %-70s |\n", zonas[i].nombre);
        fprintf(archivo, "+------------------------------------------------------------------------------+\n");
        
        // Verificar datos disponibles
        if (zonas[i].numMeses > 0) {
            int mes_actual = zonas[i].numMeses - 1;
            if (zonas[i].meses[mes_actual].numDias > 0) {
                zonas_con_datos++;
                
                // Calcular estadísticas del mes
                float sumCO2 = 0, sumSO2 = 0, sumNO2 = 0, sumPM25 = 0;
                int dias_peligrosos = 0, dias_altos = 0, dias_moderados = 0, dias_buenos = 0;
                
                for (int d = 0; d < zonas[i].meses[mes_actual].numDias; d++) {
                    struct DatosAmbientales *dia = &zonas[i].meses[mes_actual].dias[d];
                    sumCO2 += dia->co2;
                    sumSO2 += dia->so2;
                    sumNO2 += dia->no2;
                    sumPM25 += dia->pm25;
                    
                    if (dia->pm25 <= 12) dias_buenos++;
                    else if (dia->pm25 <= 35) dias_moderados++;
                    else if (dia->pm25 <= 75) dias_altos++;
                    else dias_peligrosos++;
                }
                
                int totalDias = zonas[i].meses[mes_actual].numDias;
                float promPM25 = sumPM25 / totalDias;
                pm25_promedio_regional += promPM25;
                
                // Última medición
                int dia_actual = zonas[i].meses[mes_actual].numDias - 1;
                struct DatosAmbientales *ultimo_dato = &zonas[i].meses[mes_actual].dias[dia_actual];
                
                // Determinar nivel de alerta
                char estado[20];
                char color_estado[10];
                if (promPM25 <= 12.0f) {
                    strcpy(estado, "BUENO");
                    strcpy(color_estado, "VERDE");
                    zonas_buenas++;
                } else if (promPM25 <= 35.0f) {
                    strcpy(estado, "MODERADO");
                    strcpy(color_estado, "AMARILLO");
                    zonas_moderadas++;
                } else if (promPM25 <= 75.0f) {
                    strcpy(estado, "ALTO");
                    strcpy(color_estado, "NARANJA");
                    zonas_altas++;
                } else {
                    strcpy(estado, "PELIGROSO");
                    strcpy(color_estado, "ROJO");
                    zonas_peligrosas++;
                }
                fprintf(archivo, "+------------------------------------------------------------------------------+\n");
                fprintf(archivo, "|                               ESTADO ACTUAL                                  |\n");
                fprintf(archivo, "+------------------------------------------------------------------------------+\n");
                fprintf(archivo, "| Nivel de Alerta: %-15s | Color: %-10s                        |\n", estado, color_estado);
                fprintf(archivo, "| PM2.5 Promedio: %6.1f mg/m3       | Ultima medicion: %-10s           |\n", promPM25, ultimo_dato->fecha);
                fprintf(archivo, "| PM2.5 Actual: %6.1f mg/m3         | Dias analizados: %-2d                |\n", ultimo_dato->pm25, totalDias);
                
                fprintf(archivo, "+------------------------------------------------------------------------------+\n");
                fprintf(archivo, "|                            DISTRIBUCION MENSUAL                             |\n");
                fprintf(archivo, "+------------------------------------------------------------------------------+\n");
                fprintf(archivo, "| Buenos: %2d dias (%5.1f%%)    | Moderados: %2d dias (%5.1f%%)              |\n", 
                        dias_buenos, (dias_buenos*100.0)/totalDias, dias_moderados, (dias_moderados*100.0)/totalDias);
                fprintf(archivo, "| Altos:  %2d dias (%5.1f%%)    | Peligrosos: %2d dias (%5.1f%%)             |\n", 
                        dias_altos, (dias_altos*100.0)/totalDias, dias_peligrosos, (dias_peligrosos*100.0)/totalDias);
                
                fprintf(archivo, "+------------------------------------------------------------------------------+\n");
                fprintf(archivo, "|                                RECOMENDACIONES                              |\n");
                fprintf(archivo, "+------------------------------------------------------------------------------+\n");
                
                if (strcmp(estado, "BUENO") == 0) {
                    fprintf(archivo, "| - Condiciones normales para todas las actividades al aire libre            |\n");
                    fprintf(archivo, "| - Calidad del aire saludable para toda la poblacion                         |\n");
                    fprintf(archivo, "| - No se requieren precauciones especiales                                   |\n");
                } else if (strcmp(estado, "MODERADO") == 0) {
                    fprintf(archivo, "| - Aceptable para la mayoria de las personas                                 |\n");
                    fprintf(archivo, "| - Personas sensibles deben limitar actividades prolongadas al aire libre   |\n");
                    fprintf(archivo, "| - Monitorear sintomas en grupos vulnerables                                 |\n");
                } else if (strcmp(estado, "ALTO") == 0) {
                    fprintf(archivo, "| - RIESGO PARA GRUPOS SENSIBLES (ninos, ancianos, asmaticos)                 |\n");
                    fprintf(archivo, "| - Evitar actividades extenuantes al aire libre                              |\n");
                    fprintf(archivo, "| - Considerar usar mascarillas en exteriores                                 |\n");
                    fprintf(archivo, "| - Mantener ventanas cerradas                                                 |\n");
                } else {
                    fprintf(archivo, "| - PELIGRO PARA TODA LA POBLACION                                             |\n");
                    fprintf(archivo, "| - EVITAR TODAS LAS ACTIVIDADES AL AIRE LIBRE                                |\n");
                    fprintf(archivo, "| - Usar mascarillas N95 si debe salir                                        |\n");
                    fprintf(archivo, "| - Mantener espacios cerrados con purificadores de aire                      |\n");
                    fprintf(archivo, "| - Buscar atencion medica si presenta sintomas respiratorios                 |\n");
                }
                
                fprintf(archivo, "+------------------------------------------------------------------------------+\n");
                fprintf(archivo, "|                              OTROS CONTAMINANTES                            |\n");
                fprintf(archivo, "+------------------------------------------------------------------------------+\n");
                fprintf(archivo, "| CO2 Promedio: %6.3f ppm         | SO2 Promedio: %6.1f mg/m3               |\n", 
                        sumCO2/totalDias, sumSO2/totalDias);
                fprintf(archivo, "| NO2 Promedio: %6.1f mg/m3       | Estado general: %-15s        |\n", 
                        sumNO2/totalDias, estado);
                
            } else {
                fprintf(archivo, "|                          SIN DATOS DISPONIBLES                              |\n");
                fprintf(archivo, "+------------------------------------------------------------------------------+\n");
                fprintf(archivo, "| No hay mediciones disponibles para esta zona en el periodo actual           |\n");
                fprintf(archivo, "| Recomendacion: Iniciar monitoreo para evaluar calidad del aire              |\n");
            }
        } else {
            fprintf(archivo, "|                          SIN DATOS HISTORICOS                               |\n");
            fprintf(archivo, "+------------------------------------------------------------------------------+\n");
            fprintf(archivo, "| No se han registrado datos historicos para esta zona                        |\n");
            fprintf(archivo, "| Recomendacion: Establecer estacion de monitoreo                             |\n");
        }
        
        fprintf(archivo, "+------------------------------------------------------------------------------+\n\n");
    }
    
    // Resumen regional
    fprintf(archivo, "+------------------------------------------------------------------------------+\n");
    fprintf(archivo, "|                              RESUMEN REGIONAL                               |\n");
    fprintf(archivo, "+------------------------------------------------------------------------------+\n");
    fprintf(archivo, "| Zonas monitoreadas: %d de %d                                                 |\n", zonas_con_datos, numZonas);
    
    if (zonas_con_datos > 0) {
        float promedio_regional = pm25_promedio_regional / zonas_con_datos;
        fprintf(archivo, "| PM2.5 Promedio Regional: %6.1f mg/m3                                        |\n", promedio_regional);
        fprintf(archivo, "+------------------------------------------------------------------------------+\n");
        fprintf(archivo, "| DISTRIBUCION DE ZONAS POR NIVEL DE ALERTA:                                  |\n");
        fprintf(archivo, "| - Buenas: %2d zonas (%5.1f%%)                                                |\n", 
                zonas_buenas, (zonas_buenas*100.0)/zonas_con_datos);
        fprintf(archivo, "| - Moderadas: %2d zonas (%5.1f%%)                                             |\n", 
                zonas_moderadas, (zonas_moderadas*100.0)/zonas_con_datos);
        fprintf(archivo, "| - Altas: %2d zonas (%5.1f%%)                                                 |\n", 
                zonas_altas, (zonas_altas*100.0)/zonas_con_datos);
        fprintf(archivo, "| - Peligrosas: %2d zonas (%5.1f%%)                                            |\n", 
                zonas_peligrosas, (zonas_peligrosas*100.0)/zonas_con_datos);
        
        // Alerta regional
        fprintf(archivo, "+------------------------------------------------------------------------------+\n");
        fprintf(archivo, "|                              ALERTA REGIONAL                                |\n");
        fprintf(archivo, "+------------------------------------------------------------------------------+\n");
        
        if (zonas_peligrosas > zonas_con_datos * 0.3) {
            fprintf(archivo, "| ALERTA ROJA: Mas del 30%% de zonas en niveles peligrosos                    |\n");
            fprintf(archivo, "| ACCION REQUERIDA: Medidas de emergencia en salud publica                   |\n");
        } else if (zonas_altas + zonas_peligrosas > zonas_con_datos * 0.5) {
            fprintf(archivo, "| ALERTA NARANJA: Mas del 50%% de zonas en niveles preocupantes               |\n");
            fprintf(archivo, "| ACCION REQUERIDA: Implementar medidas de mitigacion                         |\n");
        } else if (zonas_buenas > zonas_con_datos * 0.7) {
            fprintf(archivo, "| ESTADO VERDE: Mas del 70%% de zonas con buena calidad del aire              |\n");
            fprintf(archivo, "| ACCION: Mantener programas de monitoreo actuales                            |\n");
        } else {
            fprintf(archivo, "| ESTADO AMARILLO: Calidad del aire variable en la region                     |\n");
            fprintf(archivo, "| ACCION: Reforzar monitoreo y medidas preventivas                            |\n");
        }
    }
    
    fprintf(archivo, "+------------------------------------------------------------------------------+\n");
    fprintf(archivo, "\nDocumento generado automaticamente por el Sistema de Gestion de Calidad del Aire\n");
    fprintf(archivo, "Para actualizaciones, ejecute nuevamente el sistema con datos recientes.\n");
    
    fclose(archivo);
    printf(VERDE "Alertas y recomendaciones exportadas exitosamente a: %s\n" RESET, ruta_completa);
}