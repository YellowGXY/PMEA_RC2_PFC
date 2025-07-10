#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "funciones.h"
#include <sys/stat.h>
#ifdef _WIN32
#include <direct.h>
#endif

// Separador de rutas multiplataforma
#ifdef _WIN32
    #define PATH_SEPARATOR "\\"
#else
    #define PATH_SEPARATOR "/"
#endif

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
    (void)mesActual; // Suprimir warning de parametro no usado
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
            printf(VERDE "\nLISTO - Plantilla para carga masiva creada:\n");
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
                printf(VERDE "\nPERFECTO - %d registros importados\n" RESET, registros_importados);
                printf(AMARILLO "Datos organizados automaticamente por mes\n" RESET);
                printf(CIAN "Ya puede ver reportes y alertas con sus datos\n" RESET);
                // --- ACTUALIZAR mesActual[] SEGUN FECHA MAS RECIENTE ---
                int anio = config_fechas.anio_inicio;
                int mes = config_fechas.mes_inicio; // mes_inicio es 1-12
                int dia = config_fechas.dia_inicio;
                // Ajustar mesActual[] para todas las zonas
                for (int i = 0; i < *numZonasPtr; i++) {
                    // Buscar el mes correspondiente en la zona
                    int mes_idx = (mes > 0) ? (mes - 1) : 0;
                    if (mes_idx < zonas[i].numMeses) {
                        mesActual[i] = mes_idx;
                    } else if (zonas[i].numMeses > 0) {
                        mesActual[i] = zonas[i].numMeses - 1;
                    } else {
                        mesActual[i] = 0;
                    }
                }
                guardarMesActual(mesActual, *numZonasPtr);
                printf(VERDE "Fecha del sistema y mes actual de cada zona actualizados segun el ultimo registro importado.\n" RESET);
                printf(CIAN "Fecha del sistema: %02d/%02d/%d | Mes actual: %d\n" RESET, dia, mes, anio, mes);
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
void mostrarTablaReferenciaColores() {
    printf("\n+-----------+-------------------------------+\n");
    printf("| Color     | Significado                  |\n");
    printf("+-----------+-------------------------------+\n");
    printf("| VERDE     | Bueno / Bajo / Normal         |\n");
    printf("| AMARILLO  | Moderado / Precaucion         |\n");
    printf("| ROJO      | Alto / Peligroso / Alerta     |\n");
    printf("| MAGENTA   | Historico / Informativo       |\n");
    printf("| CIAN      | Informativo / Neutro          |\n");
    printf("+-----------+-------------------------------+\n\n");
}

void mostrarReporteMensual(struct Zona *zona, int mes) {
    mostrarTablaReferenciaColores();
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
    }        // Mostrar datos pagina por pagina
    for (int pagina = 0; pagina < totalPaginas; pagina++) {
        int diaInicio = pagina * diasPorPagina;
        int diaFin;
        if (diaInicio + diasPorPagina < totalDias) {
            diaFin = diaInicio + diasPorPagina;
        } else {
            diaFin = totalDias;
        }
        
        printf(NEGRITA AZUL "\n+-----+------------+--------+--------+--------+--------+------------------------------+\n" RESET);
        printf(NEGRITA AZUL "| Dia |   Fecha    |  CO2   |  SO2   |  NO2   | PM2.5  | Nivel PM2.5                |\n" RESET);
        printf(NEGRITA AZUL "+-----+------------+--------+--------+--------+--------+------------------------------+\n" RESET);
        
        for (int d = diaInicio; d < diaFin; d++) {
            struct DatosAmbientales *dia = &zona->meses[mes].dias[d];
            
            // Verificar si el dia tiene datos validos
            if (strlen(dia->fecha) == 0 || dia->co2 == 0 && dia->so2 == 0 && dia->no2 == 0 && dia->pm25 == 0) {
                printf("| %3d | %10s | %s%6s%s | %s%6s%s | %s%6s%s | %s%6s%s | %s%-26s%s |\n",
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
                
                printf("| %3d | %10s | %s%6.2f%s | %s%6.1f%s | %s%6.1f%s | %s%6.1f%s | %s%-26s%s |\n",
                    d+1, dia->fecha, 
                    color_co2, dia->co2, RESET,
                    color_so2, dia->so2, RESET,
                    color_no2, dia->no2, RESET,
                    color_pm25, dia->pm25, RESET,
                    color_pm25, nivel_pm25, RESET);
            }
        }
        
        printf(NEGRITA AZUL "+-----+------------+--------+--------+--------+--------+------------------------------+\n" RESET);
        printf(NEGRITA CIAN "Pagina %d de %d (dias %d-%d de %d)\n" RESET, pagina + 1, totalPaginas, diaInicio + 1, diaFin, totalDias);
        
        // Solo mostrar estadisticas en la ultima pagina
        if (pagina == totalPaginas - 1) {
            // Tabla de promedios
            printf(NEGRITA AZUL "\n+-------------------------------------------------------------------------------+\n" RESET);
            printf(NEGRITA AZUL "|                           PROMEDIOS DEL MES                                   |\n" RESET);
            printf(NEGRITA AZUL "+-------------------------------------------------------------------------------+\n" RESET);
            printf(NEGRITA AZUL "| Contaminante |   Promedio   |     Unidad     |            Estado              |\n" RESET);
            printf(NEGRITA AZUL "+-------------------------------------------------------------------------------+\n" RESET);
            
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
            
            printf("| %-12s | %s%10.2f%s | %-14s | %s%-28s%s |\n", 
                   "CO2", colorCO2, promCO2, RESET, "ppm", colorCO2, estadoCO2, RESET);
            printf("| %-12s | %s%10.2f%s | %-14s | %s%-28s%s |\n", 
                   "SO2", colorSO2, promSO2, RESET, "ug/m3", colorSO2, estadoSO2, RESET);
            printf("| %-12s | %s%10.2f%s | %-14s | %s%-28s%s |\n", 
                   "NO2", colorNO2, promNO2, RESET, "ug/m3", colorNO2, estadoNO2, RESET);
            printf("| %-12s | %s%10.2f%s | %-14s | %s%-28s%s |\n", 
                   "PM2.5", colorPM25, promPM25, RESET, "ug/m3", colorPM25, estadoPM25, RESET);
            printf(NEGRITA AZUL "+-------------------------------------------------------------------------------+\n" RESET);
            
            // Tabla de resumen de alertas PM2.5
            printf(NEGRITA AZUL "\n+-------------------------------------------------------------------------------+\n" RESET);
            printf(NEGRITA AZUL "|                      RESUMEN DE ALERTAS PM2.5                                 |\n" RESET);
            printf(NEGRITA AZUL "+-------------------------------------------------------------------------------+\n" RESET);
            printf(NEGRITA AZUL "| Nivel Alerta |   Dias   | Porcentaje |                 Descripcion            |\n" RESET);
            printf(NEGRITA AZUL "+-------------------------------------------------------------------------------+\n" RESET);
            
            float porcVerde = (verde * 100.0) / totalDias;
            float porcAmarilla = (amarilla * 100.0) / totalDias;
            float porcNaranja = (naranja * 100.0) / totalDias;
            float porcRoja = (roja * 100.0) / totalDias;
            
            printf("| %sBUENO%s        | %8d | %9.1f%% | %-40s |\n", 
                   VERDE, RESET, verde, porcVerde, "Calidad del aire satisfactoria");
            printf("| %sMODERADO%s     | %8d | %9.1f%% | %-40s |\n", 
                   AMARILLO, RESET, amarilla, porcAmarilla, "Aceptable para mayoria");
            printf("| %sALTO%s         | %8d | %9.1f%% | %-40s |\n", 
                   MAGENTA, RESET, naranja, porcNaranja, "Danino para grupos sensibles");
            printf("| %sPELIGROSO%s     | %8d | %9.1f%% | %-40s |\n", 
                   ROJO, RESET, roja, porcRoja, "Danino para toda la poblacion");
            printf(NEGRITA AZUL "+-------------------------------------------------------------------------------+\n" RESET);
            
            // Evaluacion general del mes
            printf(NEGRITA AZUL "\n+-------------------------------------------------------------------------------+\n" RESET);
            printf(NEGRITA AZUL "|                        EVALUACION GENERAL DEL MES                             |\n" RESET);
            printf(NEGRITA AZUL "+-------------------------------------------------------------------------------+\n" RESET);
            
            if (roja > totalDias * 0.3) {
                printf(ROJO "| MES CRITICO: Mas del 30%% de dias con niveles peligrosos                    |\n" RESET);
            } else if (naranja + roja > totalDias * 0.5) {
                printf(AMARILLO "| MES PREOCUPANTE: Mas del 50%% de dias con niveles altos                     |\n" RESET);
            } else if (verde > totalDias * 0.7) {
                printf(VERDE "| MES EXCELENTE: Mas del 70%% de dias con buena calidad del aire              |\n" RESET);
            } else {
                printf(CIAN "| MES REGULAR: Calidad del aire variable, requiere monitoreo                  |\n" RESET);
            }
            
            printf(NEGRITA AZUL "+-------------------------------------------------------------------------------+\n" RESET);
        }
        
        // Pausa entre paginas (excepto la ultima)
        if (pagina < totalPaginas - 1) {
            printf(AMARILLO "Presione cualquier tecla para continuar a la siguiente pagina...\n" RESET);
            getchar();
        }
    }
}

// --- Menu de reportes con tabla colorizada ---
void menuReportes(struct Zona zonas[], int numZonas) {
    while (1) {
        printf(CIAN "\n+============================================+\n");
        printf("|      " NEGRITA "MENU DE REPORTES HISTORICOS" RESET CIAN "         |\n");
        printf("+============================================+\n");
        printf("| %s+ 1. Reporte mensual por zona          %s |\n", VERDE, RESET CIAN);
        printf("| %s+ 2. Tabla resumen todas las zonas     %s |\n", AZUL, RESET CIAN);
        printf("| %s! 3. Alertas y recomendaciones         %s |\n", AMARILLO, RESET CIAN);
        printf("| %s* 4. Exportar tabla de zonas           %s |\n", MAGENTA, RESET CIAN);
        printf("| %s* 5. Exportar alertas y recomendaciones%s |\n", CIAN, RESET CIAN);
        printf("| %s< 6. Volver al menu principal          %s |\n", ROJO, RESET CIAN);
        printf("+============================================+\n" RESET);
        printf(AMARILLO "> Los reportes muestran datos historicos ya registrados\n" RESET);
        
        int opcion_menu = leerEnteroSeguro("Seleccione opcion: ", 1, 6);
        
        if (opcion_menu == 1) {
            printf(AMARILLO "Zonas disponibles:\n" RESET);
            printf("+----+------------------------------+\n");
            printf("| %-2s | %-28s |\n", "N", "Zona");
            printf("+----+------------------------------+\n");
            for (int i = 0; i < numZonas; i++) {
                printf("| %-2d | %-28s |\n", i+1, zonas[i].nombre);
            }
            printf("+----+------------------------------+\n");
            
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
            // Tabla resumen con colores por peligrosidad y estado textual
            printf("\n+----+--------------------+------------+------------+------------+------------+------------+------------+------------+------------+\n");
            printf("| N  | Zona               | CO2        | Estado     | SO2        | Estado     | NO2        | Estado     | PM2.5      | Estado     |\n");
            printf("+----+--------------------+------------+------------+------------+------------+------------+------------+------------+------------+\n");
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
                // Estado textual y color solo en el estado
                const char* color_co2 = obtenerColorContaminante(prom_co2, TIPO_CO2);
                const char* color_so2 = obtenerColorContaminante(prom_so2, TIPO_SO2);
                const char* color_no2 = obtenerColorContaminante(prom_no2, TIPO_NO2);
                const char* color_pm25 = obtenerColorContaminante(prom_pm25, TIPO_PM25);
                char* estadoCO2 = obtenerNivelPeligrosidad(prom_co2, TIPO_CO2);
                char* estadoSO2 = obtenerNivelPeligrosidad(prom_so2, TIPO_SO2);
                char* estadoNO2 = obtenerNivelPeligrosidad(prom_no2, TIPO_NO2);
                char* estadoPM25 = obtenerNivelPeligrosidad(prom_pm25, TIPO_PM25);
                printf("| %-2d | %-18s | %10.1f | %s%-10s%s | %10.1f | %s%-10s%s | %10.1f | %s%-10s%s | %10.1f | %s%-10s%s |\n",
                    i+1, zonas[i].nombre,
                    prom_co2, color_co2, estadoCO2, RESET,
                    prom_so2, color_so2, estadoSO2, RESET,
                    prom_no2, color_no2, estadoNO2, RESET,
                    prom_pm25, color_pm25, estadoPM25, RESET);
            }
            printf("+----+--------------------+------------+------------+------------+------------+------------+------------+------------+------------+\n");
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

// --- Menu de checkpoints ---
void menuCheckpoints(struct Zona zonas[], int numZonas, int mesActual[]) {
    (void)mesActual; // Suprimir warning de parametro no usado
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
                printf(ROJO "Ya esta en el primer mes.\n" RESET);
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

// --- Menu de pronosticos ---
void menuPronosticos(struct Zona zonas[], int numZonas, int mesActual[]) {
    while (1) {
        printf(CIAN "\n+============================================+\n");
        printf("|      " NEGRITA "MENU DE PRONOSTICOS PREDICTIVOS" RESET CIAN "        |\n");
        printf("+============================================+\n");
        printf("| %s+ 1. Pronostico predictivo por zona    %s |\n", VERDE, RESET CIAN);
        printf("| %s+ 2. Pronostico general multicapa      %s |\n", AZUL, RESET CIAN);
        printf("| %s! 3. Prediccion de alertas PM2.5       %s |\n", AMARILLO, RESET CIAN);
        printf("| %s< 4. Volver al menu principal          %s |\n", ROJO, RESET CIAN);
        printf("+============================================+\n" RESET);
        printf(AMARILLO "> Los pronosticos predicen valores futuros basados en datos historicos\n" RESET);
        
        int opcion = leerEnteroSeguro("Seleccione opcion: ", 1, 4);
        
        if (opcion == 1) {
            // Pronostico por zona
            printf(AMARILLO "Zonas disponibles:\n" RESET);
            for (int i = 0; i < numZonas; i++) {
                printf("%d. %s\n", i+1, zonas[i].nombre);
            }
            
            int zona_idx = leerEnteroSeguro("Seleccione zona: ", 1, numZonas) - 1;
            if (zona_idx >= 0 && zona_idx < numZonas) {
                generarPronosticoZonaMejorado(&zonas[zona_idx], mesActual[zona_idx]);
            }
            
        } else if (opcion == 2) {
            // Pronostico general
            generarPronosticoGeneral(zonas, numZonas, mesActual);
            
        } else if (opcion == 3) {
            // Prediccion de alertas PM2.5
            predecirAlertasPM25(zonas, numZonas, mesActual);
            
        } else if (opcion == 4) {
            break;
        }
    }
}

// Funcion mejorada para generar pronostico usando datos multimes
void generarPronosticoZonaMejorado(struct Zona *zona, int mesActual) {
    // LOGICA MEJORADA: Recopilar datos del mes actual Y meses anteriores
    float sumCO2 = 0, sumSO2 = 0, sumNO2 = 0, sumPM25 = 0;
    int numDatos = 0;
    int diasMinimos = 3; // Necesitamos al menos 3 dias de datos
    int maxDatosDeseados = 10; // Maximo 10 dias para mejor precision
    
    // Cabecera decorativa con solo caracteres ASCII
    printf(NEGRITA CIAN "\n");
    printf("+================================================================+\n");
    printf("| %s      SISTEMA DE PRONOSTICO INTELIGENTE DE CALIDAD AEREA    %s |\n", AMARILLO, RESET CIAN);
    printf("+================================================================+\n");
    printf("| %s ZONA:%s %-51s |\n", VERDE, RESET CIAN, zona->nombre);
    printf("+================================================================+\n" RESET);
    
    // PASO 1: Buscar datos empezando por el mes actual y retrocediendo
    printf("| %s[RECOPILACION DE DATOS]%s                                    |\n", VERDE, RESET);
    printf("+----------------------------------------------------------------+\n");
    
    // Buscar datos del mes actual hacia atras
    for (int mes = mesActual; mes >= 0 && numDatos < maxDatosDeseados; mes--) {
        if (mes < zona->numMeses && zona->meses[mes].numDias > 0) {
            int datosDelMes = 0;
            
            // Recopilar datos validos del mes
            for (int d = 0; d < zona->meses[mes].numDias && numDatos < maxDatosDeseados; d++) {
                // Verificar que el dia tiene datos reales
                if (strlen(zona->meses[mes].dias[d].fecha) > 0 && 
                    (zona->meses[mes].dias[d].co2 > 0 || zona->meses[mes].dias[d].so2 > 0 || 
                     zona->meses[mes].dias[d].no2 > 0 || zona->meses[mes].dias[d].pm25 > 0)) {
                    
                    sumCO2 += zona->meses[mes].dias[d].co2;
                    sumSO2 += zona->meses[mes].dias[d].so2;
                    sumNO2 += zona->meses[mes].dias[d].no2;
                    sumPM25 += zona->meses[mes].dias[d].pm25;
                    numDatos++;
                    datosDelMes++;
                }
            }
            
            if (datosDelMes > 0) {
                const char* tipoMes;
                const char* colorMes;
                if (mes == mesActual) {
                    tipoMes = "ACTUAL";
                    colorMes = VERDE;
                } else {
                    tipoMes = "HISTORICO";
                    colorMes = MAGENTA;
                }
                printf("| %s+ Mes %d (%s):%s %d dias encontrados [Total: %d]%s       |\n", 
                       colorMes, mes + 1, tipoMes, RESET, 
                       datosDelMes, numDatos, RESET);
            }
        }
    }
    
    // Verificar si tenemos suficientes datos
    if (numDatos < diasMinimos) {
        printf("| %s! ADVERTENCIA:%s Solo %d de %d dias minimos encontrados      |\n", 
               AMARILLO, RESET, numDatos, diasMinimos);
        if (numDatos == 0) {
            printf("+----------------------------------------------------------------+\n");
            printf("| %s  SIN DATOS DISPONIBLES - PRONOSTICO ESTIMADO  %s           |\n", ROJO, RESET);
            printf("+----------------------------------------------------------------+\n");
            printf("\n+-------------+----------------+----------+----------+\n");
            printf("| %sContaminante%s | %sValor Estimado%s | %sUnidad%s   | %sEstado%s   |\n", 
                   AMARILLO, RESET, AMARILLO, RESET, AMARILLO, RESET, AMARILLO, RESET);
            printf("+-------------+----------------+----------+----------+\n");
            printf("| %-11s | %s%14.3f%s | %-8s | %s%-8s%s |\n", "CO2", CIAN, 0.040f, RESET, "ppm", VERDE, "Normal", RESET);
            printf("| %-11s | %s%14.3f%s | %-8s | %s%-8s%s |\n", "SO2", CIAN, 0.008f, RESET, "ug/m3", VERDE, "Bajo", RESET);
            printf("| %-11s | %s%14.3f%s | %-8s | %s%-8s%s |\n", "NO2", CIAN, 0.025f, RESET, "ug/m3", VERDE, "Normal", RESET);
            printf("| %-11s | %s%14.1f%s | %-8s | %s%-8s%s |\n", "PM2.5", CIAN, 8.0f, RESET, "ug/m3", VERDE, "Bueno", RESET);
            printf("+-------------+----------------+----------+----------+\n");
            return;
        }
    } else {
        printf("| %s+ EXITO:%s %d dias de datos recopilados correctamente       |\n", 
               VERDE, RESET, numDatos);
    }
    
    printf("+----------------------------------------------------------------+\n");
    
    // Calcular promedios y tendencias
    float promCO2 = sumCO2 / numDatos;
    float promSO2 = sumSO2 / numDatos;
    float promNO2 = sumNO2 / numDatos;
    float promPM25 = sumPM25 / numDatos;
    
    // Tabla de datos recopilados con diseno ASCII
    printf(NEGRITA AZUL "\n");
    printf("+================================================================+\n");
    printf("| %s       DATOS RECOPILADOS PARA ANALISIS PREDICTIVO       %s   |\n", VERDE, RESET AZUL);
    printf("+================================================================+\n");
    printf("| %sDias analizados:%s %s%d%s %s(multiples meses)%s                   |\n", 
           NEGRITA, RESET AZUL, VERDE, numDatos, RESET AZUL, CIAN, RESET AZUL);
    printf("+================================================================+\n" RESET);
    
    printf(NEGRITA "\n+-------------+----------------+----------+----------+\n");
    printf("| %sContaminante%s | %sPromedio Real%s  | %sUnidad%s   | %sCalidad%s  |\n", 
           AMARILLO, RESET, AMARILLO, RESET, AMARILLO, RESET, AMARILLO, RESET);
    printf("+-------------+----------------+----------+----------+\n");
    // CO2
    const char* colorCO2;
    const char* estadoCO2;
    if (promCO2 > 0.05) {
        colorCO2 = ROJO;
        estadoCO2 = "Alto";
    } else {
        colorCO2 = VERDE;
        estadoCO2 = "Normal";
    }
    printf("| %-11s | %s%14.3f%s | %-8s | %s%-8s%s |\n", 
           "CO2", colorCO2, promCO2, RESET, "ppm", colorCO2, estadoCO2, RESET);
    
    // SO2
    const char* colorSO2;
    const char* estadoSO2;
    if (promSO2 > 20) {
        colorSO2 = ROJO;
        estadoSO2 = "Alto";
    } else if (promSO2 > 10) {
        colorSO2 = AMARILLO;
        estadoSO2 = "Moderado";
    } else {
        colorSO2 = VERDE;
        estadoSO2 = "Bajo";
    }
    printf("| %-11s | %s%14.1f%s | %-8s | %s%-8s%s |\n", 
           "SO2", colorSO2, promSO2, RESET, "ug/m3", colorSO2, estadoSO2, RESET);
    
    // NO2
    const char* colorNO2;
    const char* estadoNO2;
    if (promNO2 > 40) {
        colorNO2 = ROJO;
        estadoNO2 = "Alto";
    } else if (promNO2 > 25) {
        colorNO2 = AMARILLO;
        estadoNO2 = "Moderado";
    } else {
        colorNO2 = VERDE;
        estadoNO2 = "Normal";
    }
    printf("| %-11s | %s%14.1f%s | %-8s | %s%-8s%s |\n", 
           "NO2", colorNO2, promNO2, RESET, "ug/m3", colorNO2, estadoNO2, RESET);
    
    // PM2.5
    const char* colorPM25;
    const char* estadoPM25;
    if (promPM25 > 35) {
        colorPM25 = ROJO;
        estadoPM25 = "Danino";
    } else if (promPM25 > 12) {
        colorPM25 = AMARILLO;
        estadoPM25 = "Moderado";
    } else {
        colorPM25 = VERDE;
        estadoPM25 = "Bueno";
    }
    printf("| %-11s | %s%14.1f%s | %-8s | %s%-8s%s |\n", 
           "PM2.5", colorPM25, promPM25, RESET, "ug/m3", colorPM25, estadoPM25, RESET);
    
    printf("+-------------+----------------+----------+----------+\n");
    
    // Calcular tendencia y pronostico inteligente basado en datos historicos
    float factorTendencia = 0.0f;
    float pronosticoCO2, pronosticoSO2, pronosticoNO2, pronosticoPM25;
    
    // ALGORITMO MEJORADO: Calcular tendencia real si tenemos suficientes datos
    if (numDatos >= diasMinimos) {
        // Si tenemos datos del mes actual, usar esos como base para la tendencia
        if (mesActual < zona->numMeses && zona->meses[mesActual].numDias > 0) {
            float sumActualCO2 = 0, sumActualSO2 = 0, sumActualNO2 = 0, sumActualPM25 = 0;
            int datosActuales = 0;
            
            // Calcular promedio del mes actual
            for (int d = 0; d < zona->meses[mesActual].numDias; d++) {
                if (strlen(zona->meses[mesActual].dias[d].fecha) > 0) {
                    sumActualCO2 += zona->meses[mesActual].dias[d].co2;
                    sumActualSO2 += zona->meses[mesActual].dias[d].so2;
                    sumActualNO2 += zona->meses[mesActual].dias[d].no2;
                    sumActualPM25 += zona->meses[mesActual].dias[d].pm25;
                    datosActuales++;
                }
            }
            
            if (datosActuales > 0) {
                float promActualCO2 = sumActualCO2 / datosActuales;
                float promActualSO2 = sumActualSO2 / datosActuales;
                float promActualNO2 = sumActualNO2 / datosActuales;
                float promActualPM25 = sumActualPM25 / datosActuales;
                
                // Calcular tendencia comparando mes actual vs promedio histórico
                float tendenciaCO2 = (promActualCO2 - promCO2) / promCO2;
                float tendenciaSO2 = (promActualSO2 - promSO2) / promSO2;
                float tendenciaNO2 = (promActualNO2 - promNO2) / promNO2;
                float tendenciaPM25 = (promActualPM25 - promPM25) / promPM25;
                
                // Aplicar tendencia calculada (limitada entre -20% y +20%)
                if (tendenciaCO2 > 0.2f) {
                    tendenciaCO2 = 0.2f;
                } else if (tendenciaCO2 < -0.2f) {
                    tendenciaCO2 = -0.2f;
                }
                
                if (tendenciaSO2 > 0.2f) {
                    tendenciaSO2 = 0.2f;
                } else if (tendenciaSO2 < -0.2f) {
                    tendenciaSO2 = -0.2f;
                }
                
                if (tendenciaNO2 > 0.2f) {
                    tendenciaNO2 = 0.2f;
                } else if (tendenciaNO2 < -0.2f) {
                    tendenciaNO2 = -0.2f;
                }
                
                if (tendenciaPM25 > 0.2f) {
                    tendenciaPM25 = 0.2f;
                } else if (tendenciaPM25 < -0.2f) {
                    tendenciaPM25 = -0.2f;
                }
                
                pronosticoCO2 = promActualCO2 * (1.0f + tendenciaCO2);
                pronosticoSO2 = promActualSO2 * (1.0f + tendenciaSO2);
                pronosticoNO2 = promActualNO2 * (1.0f + tendenciaNO2);
                pronosticoPM25 = promActualPM25 * (1.0f + tendenciaPM25);
            } else {
                // Sin datos actuales, usar promedio histórico con variación mínima
                factorTendencia = 0.02f; // 2% de variación conservadora
                pronosticoCO2 = promCO2 * (1.0f + factorTendencia);
                pronosticoSO2 = promSO2 * (1.0f + factorTendencia);
                pronosticoNO2 = promNO2 * (1.0f + factorTendencia);
                pronosticoPM25 = promPM25 * (1.0f + factorTendencia);
            }
        } else {
            // Solo datos históricos, usar variación conservadora
            factorTendencia = 0.03f; // 3% de variación
            pronosticoCO2 = promCO2 * (1.0f + factorTendencia);
            pronosticoSO2 = promSO2 * (1.0f + factorTendencia);
            pronosticoNO2 = promNO2 * (1.0f + factorTendencia);
            pronosticoPM25 = promPM25 * (1.0f + factorTendencia);
        }
    } else {
        // Pocos datos, usar estimación muy conservadora
        factorTendencia = 0.01f; // 1% de variación mínima
        pronosticoCO2 = promCO2 * (1.0f + factorTendencia);
        pronosticoSO2 = promSO2 * (1.0f + factorTendencia);
        pronosticoNO2 = promNO2 * (1.0f + factorTendencia);
        pronosticoPM25 = promPM25 * (1.0f + factorTendencia);
    }
    
    // Tabla de pronosticos con diseno ASCII
    printf(NEGRITA MAGENTA "\n");
    printf("+================================================================+\n");
    printf("| %s       PRONOSTICO PARA PROXIMAS 24-48 HORAS       %s        |\n", CIAN, RESET MAGENTA);
    printf("+================================================================+\n" RESET);
    
    printf(NEGRITA "\n+-------------+----------------+----------+----------+\n");
    printf("| %sContaminante%s | %sValor Previsto%s | %sUnidad%s   | %sTendencia%s|\n", 
           AMARILLO, RESET, AMARILLO, RESET, AMARILLO, RESET, AMARILLO, RESET);
    printf("+-------------+----------------+----------+----------+\n");
    
    // Pronosticos con tendencias mejoradas y diseno llamativo
    float tendCO2 = pronosticoCO2 - promCO2;
    float pctCO2;
    if (promCO2 != 0) {
        pctCO2 = (tendCO2 / promCO2) * 100;
    } else {
        pctCO2 = 0;
    }
    const char* trendCO2;
    const char* colorTrendCO2;
    if (pctCO2 > 1.0f) {
        trendCO2 = "+ SUBE";
        colorTrendCO2 = ROJO;
    } else if (pctCO2 < -1.0f) {
        trendCO2 = "- BAJA";
        colorTrendCO2 = VERDE;
    } else {
        trendCO2 = "= Est";
        colorTrendCO2 = AMARILLO;
    }
    printf("| %-11s | %s%14.3f%s | %-8s | %s%-8s%s |\n", 
           "CO2", CIAN, pronosticoCO2, RESET, "ppm", colorTrendCO2, trendCO2, RESET);
    
    float tendSO2 = pronosticoSO2 - promSO2;
    float pctSO2;
    if (promSO2 != 0) {
        pctSO2 = (tendSO2 / promSO2) * 100;
    } else {
        pctSO2 = 0;
    }
    const char* trendSO2;
    const char* colorTrendSO2;
    if (pctSO2 > 2.0f) {
        trendSO2 = "+ SUBE";
        colorTrendSO2 = ROJO;
    } else if (pctSO2 < -2.0f) {
        trendSO2 = "- BAJA";
        colorTrendSO2 = VERDE;
    } else {
        trendSO2 = "= Est";
        colorTrendSO2 = AMARILLO;
    }
    printf("| %-11s | %s%14.1f%s | %-8s | %s%-8s%s |\n", 
           "SO2", CIAN, pronosticoSO2, RESET, "ug/m3", colorTrendSO2, trendSO2, RESET);
    
    float tendNO2 = pronosticoNO2 - promNO2;
    float pctNO2;
    if (promNO2 != 0) {
        pctNO2 = (tendNO2 / promNO2) * 100;
    } else {
        pctNO2 = 0;
    }
    const char* trendNO2;
    const char* colorTrendNO2;
    if (pctNO2 > 2.0f) {
        trendNO2 = "+ SUBE";
        colorTrendNO2 = ROJO;
    } else if (pctNO2 < -2.0f) {
        trendNO2 = "- BAJA";
        colorTrendNO2 = VERDE;
    } else {
        trendNO2 = "= Est";
        colorTrendNO2 = AMARILLO;
    }
    printf("| %-11s | %s%14.1f%s | %-8s | %s%-8s%s |\n", 
           "NO2", CIAN, pronosticoNO2, RESET, "ug/m3", colorTrendNO2, trendNO2, RESET);
    
    float tendPM25 = pronosticoPM25 - promPM25;
    float pctPM25;
    if (promPM25 != 0) {
        pctPM25 = (tendPM25 / promPM25) * 100;
    } else {
        pctPM25 = 0;
    }
    const char* trendPM25;
    const char* colorTrendPM25;
    if (pctPM25 > 3.0f) {
        trendPM25 = "+ SUBE";
        colorTrendPM25 = ROJO;
    } else if (pctPM25 < -3.0f) {
        trendPM25 = "- BAJA";
        colorTrendPM25 = VERDE;
    } else {
        trendPM25 = "= Est";
        colorTrendPM25 = AMARILLO;
    }
    printf("| %-11s | %s%14.1f%s | %-8s | %s%-8s%s |\n", 
           "PM2.5", CIAN, pronosticoPM25, RESET, "ug/m3", colorTrendPM25, trendPM25, RESET);
    
    printf("+-------------+----------------+----------+----------+\n");
    
    // Evaluacion final con diseno ASCII
    printf(NEGRITA "\n");
    printf("+================================================================+\n");
    printf("| %s       EVALUACION Y RECOMENDACIONES INTELIGENTES       %s   |\n", VERDE, RESET);
    printf("+================================================================+\n" RESET);
    
    printf("\n+----------------------------------------------------------------+\n");
    if (pronosticoPM25 > 75) {
        printf("| %s! ALERTA ROJA: Niveles peligrosos de PM2.5 previstos !%s    |\n", ROJO, RESET);
        printf("| %s! Recomendacion: Evitar actividades al aire libre          %s |\n", AMARILLO, RESET);
        printf("| %s+ Permanecer en interiores con ventanas cerradas           %s |\n", CIAN, RESET);
    } else if (pronosticoPM25 > 35) {
        printf("| %s! PRECAUCION: PM2.5 en niveles moderados !                %s |\n", AMARILLO, RESET);
        printf("| %s+ Recomendacion: Limitar ejercicio intenso al exterior     %s |\n", VERDE, RESET);
        printf("| %s* Considerar uso de mascarilla en exteriores               %s |\n", AZUL, RESET);
    } else {
        printf("| %s+ CONDICIONES ACEPTABLES: Calidad del aire normal +        %s |\n", VERDE, RESET);
        printf("| %s+ Recomendacion: Actividades normales permitidas           %s |\n", CIAN, RESET);
        printf("| %s+ Condiciones ideales para actividades al aire libre      %s |\n", AMARILLO, RESET);
    }
    
    // Agregar evaluacion adicional para otros contaminantes
    if (pronosticoCO2 > 0.06) {
        printf("| %s! ADVERTENCIA: Niveles altos de CO2 previstos              %s |\n", ROJO, RESET);
    }
    if (pronosticoNO2 > 40) {
        printf("| %s! PRECAUCION: Niveles elevados de NO2 previstos            %s |\n", AMARILLO, RESET);
    }
    
    printf("+----------------------------------------------------------------+\n");
}

// Funcion para generar pronostico general
void generarPronosticoGeneral(struct Zona zonas[], int numZonas, int mesActual[]) {
    mostrarTablaReferenciaColores();
    float totalCO2 = 0, totalSO2 = 0, totalNO2 = 0, totalPM25 = 0;
    int totalDias = 0;
    int zonasConDatos = 0;
    int zonasConDatosReales = 0;
    int diasMinimos = 4;
    for (int i = 0; i < numZonas; i++) {
        if (mesActual[i] < zonas[i].numMeses && zonas[i].meses[mesActual[i]].numDias >= diasMinimos) {
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
    if (zonasConDatosReales < 1) {
       
        printf("+-----------------+-------------------+----------+----------+\n");
        printf("| Contaminante    | Valor Estimado    | Unidad   | Estado   |\n");
        printf("+-----------------+-------------------+----------+----------+\n");
        printf("| %-15s | %17.3f | %-8s | %-8s |\n", "CO2", 0.040f, "ppm", "Normal");
        printf("| %-15s | %17.3f | %-8s | %-8s |\n", "SO2", 0.008f, "ug/m3", "Bajo");
        printf("| %-15s | %17.3f | %-8s | %-8s |\n", "NO2", 0.025f, "ug/m3", "Normal");
        printf("| %-15s | %17.1f | %-8s | %-8s |\n", "PM2.5", 8.0f, "ug/m3", "Bueno");
        printf("+-----------------+-------------------+----------+----------+\n");
        return;
    }
    for (int i = 0; i < numZonas; i++) {
        if (mesActual[i] < zonas[i].numMeses && zonas[i].meses[mesActual[i]].numDias >= diasMinimos) {
            int tiene_datos_reales = 0;
            for (int d = 0; d < zonas[i].meses[mesActual[i]].numDias; d++) {
                if (strlen(zonas[i].meses[mesActual[i]].dias[d].fecha) > 0) {
                    tiene_datos_reales = 1;
                    break;
                }
            }
            if (tiene_datos_reales) {
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
    float promedioCO2 = totalCO2 / totalDias;
    float promedioSO2 = totalSO2 / totalDias;
    float promedioNO2 = totalNO2 / totalDias;
    float promedioPM25 = totalPM25 / totalDias;
    // Tendencias
    const char* tendenciaCO2 = "Estable";
    const char* tendenciaSO2 = "Estable";
    const char* tendenciaNO2 = "Estable";
    const char* tendenciaPM25 = "Estable";
    // (Aqui puedes calcular tendencias reales si lo deseas)
    printf("+-----------------+-------------------+----------+-----------------+\n");
    printf("| Contaminante    | Promedio Regional | Unidad   | Tendencia       |\n");
    printf("+-----------------+-------------------+----------+-----------------+\n");
    printf("| %-15s | %17.3f | %-8s | %-15s |\n", "CO2", promedioCO2, "ppm", tendenciaCO2);
    printf("| %-15s | %17.3f | %-8s | %-15s |\n", "SO2", promedioSO2, "ug/m3", tendenciaSO2);
    printf("| %-15s | %17.3f | %-8s | %-15s |\n", "NO2", promedioNO2, "ug/m3", tendenciaNO2);
    printf("| %-15s | %17.1f | %-8s | %-15s |\n", "PM2.5", promedioPM25, "ug/m3", tendenciaPM25);
    printf("+-----------------+-------------------+----------+-----------------+\n");
    if (promedioPM25 > 75) {
        printf("Calidad del aire PELIGROSA en la region\n");
    } else if (promedioPM25 > 35) {
        printf("Calidad del aire MODERADA en la region\n");
    } else {
        printf("Calidad del aire BUENA en la region\n");
    }
    printf("Zonas analizadas: %d de %d (con datos reales)\n", zonasConDatosReales, numZonas);
}

// ===== FUNCIONES DE CONFIGURACION DE FECHAS =====

/**
 * Guarda la configuracion de fechas en un archivo
 */
int guardarConfiguracionFechas(struct ConfiguracionFechas *config) {
    crearCarpetaSistema();
    char ruta_config[64];
    snprintf(ruta_config, sizeof(ruta_config), "sistema_archivos%sconfig_fechas.dat", PATH_SEPARATOR);
    FILE *archivo = fopen(ruta_config, "wb");
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
    char ruta_config[64];
    snprintf(ruta_config, sizeof(ruta_config), "sistema_archivos%sconfig_fechas.dat", PATH_SEPARATOR);
    FILE *archivo = fopen(ruta_config, "rb");
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
    char ruta_mes[64];
    snprintf(ruta_mes, sizeof(ruta_mes), "sistema_archivos%smes_actual.dat", PATH_SEPARATOR);
    FILE *archivo = fopen(ruta_mes, "wb");
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
    char ruta_mes[64];
    snprintf(ruta_mes, sizeof(ruta_mes), "sistema_archivos%smes_actual.dat", PATH_SEPARATOR);
    FILE *archivo = fopen(ruta_mes, "rb");
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

// ===== FUNCION PARA GENERAR MESES VACIOS AUTOMATICAMENTE =====

void crearMesesVaciosHasta(struct Zona zonas[], int numZonas, int mesObjetivo) {
    printf(AMARILLO "Creando meses vacios hasta el mes %d...\n" RESET, mesObjetivo + 1);
    
    for (int i = 0; i < numZonas; i++) {
        // Encontrar el mes actual mas alto para esta zona
        int mesMaximo = 0;
        for (int m = 0; m < 12; m++) {
            if (zonas[i].meses[m].numDias > 0) {
                mesMaximo = m;
            }
        }
        
        // Crear meses vacios desde el siguiente al maximo hasta el objetivo
        for (int m = mesMaximo + 1; m <= mesObjetivo; m++) {
            if (m < 12 && zonas[i].meses[m].numDias == 0) {
                // Inicializar mes vacio
                zonas[i].meses[m].numDias = 0;
                
                // Limpiar todos los dias del mes
                for (int d = 0; d < 31; d++) {
                    zonas[i].meses[m].dias[d].co2 = 0.0;
                    zonas[i].meses[m].dias[d].so2 = 0.0;
                    zonas[i].meses[m].dias[d].no2 = 0.0;
                    zonas[i].meses[m].dias[d].pm25 = 0.0;
                    strcpy(zonas[i].meses[m].dias[d].fecha, "");
                }
                
                // Actualizar numMeses si es necesario
                if (zonas[i].numMeses <= m) {
                    zonas[i].numMeses = m + 1;
                }
                
                printf(CIAN "  Zona %s: Mes %d creado (vacio)\n" RESET, zonas[i].nombre, m + 1);
                
                // Guardar el mes vacio
                guardarMes(&zonas[i], m);
            }
        }
    }
    
    printf(VERDE "Meses vacios creados exitosamente.\n" RESET);
}

// ===== FUNCION PARA VERIFICAR Y CREAR MESES NECESARIOS =====

void verificarYCrearMesesNecesarios(struct Zona zonas[], int numZonas, int mesDestino) {
    // Verificar si necesitamos crear meses intermedios
    int necesitaCrear = 0;
    int mesMaximoGlobal = 0;
    
    // Encontrar el mes maximo con datos en todas las zonas
    for (int i = 0; i < numZonas; i++) {
        for (int m = 0; m < 12; m++) {
            if (zonas[i].meses[m].numDias > 0 && m > mesMaximoGlobal) {
                mesMaximoGlobal = m;
            }
        }
    }
    
    // Si el mes destino es mayor que el maximo actual, crear meses intermedios
    if (mesDestino > mesMaximoGlobal + 1) {
        printf(AMARILLO "Detectado salto de meses: del mes %d al mes %d\n" RESET, 
               mesMaximoGlobal + 1, mesDestino + 1);
        
        char respuesta = leerCaracterSeguro("Crear meses intermedios vacios? (s/n): ");
        if (respuesta == 's' || respuesta == 'S') {
            crearMesesVaciosHasta(zonas, numZonas, mesDestino - 1);
        }
    }
}

// --- Funciones de prediccion (sin implementar) ---
void predecirAlertasPM25(struct Zona zonas[], int numZonas, int mesActual[]) {
    printf("\n+----------------------------------------------------------+\n");
    printf("|   Prediccion de alertas PM2.5                           |\n");
    printf("+----------------------------------------------------------+\n");

    for (int i = 0; i < numZonas; i++) {
        int mes_ultimo = zonas[i].numMeses - 1;
        if (mes_ultimo < 0) mes_ultimo = 0;
        float suma_pm25 = 0;
        int numero_dias = zonas[i].meses[mes_ultimo].numDias;
        if (numero_dias == 0) continue;

        for (int d = 0; d < numero_dias; d++) {
            suma_pm25 += zonas[i].meses[mes_ultimo].dias[d].pm25;
        }

        float promedio_pm25 = suma_pm25 / numero_dias;
        const char *alerta = "Bueno";
        const char *color = VERDE;

        if (promedio_pm25 > 75) {
            alerta = "Peligroso";
            color = ROJO;
        } else if (promedio_pm25 > 35) {
            alerta = "Moderado";
            color = AMARILLO;
        } else if (promedio_pm25 > 12) {
            alerta = "Alto";
            color = MAGENTA;
        }

        printf("Zona: %s | PM2.5 Promedio: %.2f | Alerta: %s%s%s\n", 
               zonas[i].nombre, promedio_pm25, color, alerta, RESET);
    }
}

void mostrarTendenciasContaminacion(struct Zona zonas[], int numZonas, int mesActual[]) {
    printf("\n+----------------------------------------------------------+\n");
    printf("|   Analisis de tendencias de contaminacion               |\n");
    printf("+----------------------------------------------------------+\n");

    for (int i = 0; i < numZonas; i++) {
        printf("\nZona: %s\n", zonas[i].nombre);

        int total_meses = zonas[i].numMeses;
        if (total_meses < 2) {
            printf("Datos insuficientes para un análisis completo.\n");
            if (total_meses == 1 && zonas[i].meses[0].numDias > 0) {
                float suma_pm25 = 0;
                for (int d = 0; d < zonas[i].meses[0].numDias; d++) {
                    suma_pm25 += zonas[i].meses[0].dias[d].pm25;
                }
                float promedio_pm25 = suma_pm25 / zonas[i].meses[0].numDias;
                printf("Promedio PM2.5 del único mes disponible: %.2f\n", promedio_pm25);
            }
            continue;
        }

        // Variables para regresión lineal
        float sum_x = 0, sum_y = 0, sum_xy = 0, sum_x2 = 0;
        int n = 0;

        for (int mes = 0; mes < total_meses; mes++) {
            if (zonas[i].meses[mes].numDias == 0) continue;

            float suma_pm25 = 0;
            for (int d = 0; d < zonas[i].meses[mes].numDias; d++) {
                suma_pm25 += zonas[i].meses[mes].dias[d].pm25;
            }

            float promedio_pm25 = suma_pm25 / zonas[i].meses[mes].numDias;

            // Usar el índice del mes como "x" y el promedio de PM2.5 como "y"
            sum_x += mes;
            sum_y += promedio_pm25;
            sum_xy += mes * promedio_pm25;
            sum_x2 += mes * mes;
            n++;
        }

        if (n < 2) {
            printf("No hay suficientes datos válidos para analizar tendencias.\n");
            continue;
        }

        // Calcular pendiente (m) de la regresión lineal: m = (n*sum_xy - sum_x*sum_y) / (n*sum_x2 - sum_x^2)
        float pendiente = (n * sum_xy - sum_x * sum_y) / (n * sum_x2 - sum_x * sum_x);

        if (pendiente > 0) {
            printf("Tendencia: Aumento en PM2.5 (pendiente: %.2f)\n", pendiente);
        } else if (pendiente < 0) {
            printf("Tendencia: Disminucion en PM2.5 (pendiente: %.2f)\n", pendiente);
        } else {
            printf("Tendencia: Estabilidad en PM2.5\n");
        }
    }
}

void mostrarAlertasYRecomendaciones(struct Zona zonas[], int numZonas) {
    printf("\n+----+--------------------+---------------------+-----------------------------+\n");
    printf("| N  | Zona               | Alerta PM2.5        | Recomendacion               |\n");
    printf("+----+--------------------+---------------------+-----------------------------+\n");
    for (int i = 0; i < numZonas; i++) {
        int mes_ultimo = zonas[i].numMeses - 1;
        if (mes_ultimo < 0) mes_ultimo = 0;
        float suma_pm25 = 0;
        int numero_dias = zonas[i].meses[mes_ultimo].numDias;
        if (numero_dias == 0) numero_dias = 1;
        for (int d = 0; d < numero_dias; d++) {
            suma_pm25 += zonas[i].meses[mes_ultimo].dias[d].pm25;
        }
        float prom_pm25 = suma_pm25 / numero_dias;
        const char *color = VERDE;
        const char *alerta = "Bueno";
        const char *recomendacion = "Sin restricciones. Aire limpio.";
        if (prom_pm25 > 75) {
            color = ROJO;
            alerta = "Peligroso";
            recomendacion = "Evite salir. Riesgo alto para salud.";
        } else if (prom_pm25 > 35) {
            color = AMARILLO;
            alerta = "Moderado";
            recomendacion = "Limite ejercicio y actividades al aire libre.";
        } else if (prom_pm25 > 12) {
            color = MAGENTA;
            alerta = "Alto";
            recomendacion = "Personas sensibles deben evitar exposicion prolongada.";
        }
        printf("| %-2d | %-18s | %s%-19s%s | %-27s |\n", i + 1, zonas[i].nombre, color, alerta, RESET, recomendacion);
    }
    printf("+----+--------------------+---------------------+-----------------------------+\n");
}

void exportarReporteTabla(struct Zona zonas[], int numZonas) {
    #ifdef _WIN32
        _mkdir("reportes");
        _mkdir("reportes/exportaciones");
    #else
        mkdir("reportes", 0777);
        mkdir("reportes/exportaciones", 0777);
    #endif

    // Obtener la hora actual
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    // Crear nombre de archivo con marca de tiempo
    char nombre_archivo[128];
    snprintf(nombre_archivo, sizeof(nombre_archivo), "reportes/exportaciones/tabla_zonas_%04d%02d%02d_%02d%02d%02d.txt",
             tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    FILE *archivo = fopen(nombre_archivo, "w");
    if (!archivo) {
        printf(ROJO "Error al crear el archivo de reporte." RESET "\n");
        return;
    }

    // Encabezado del informe
    fprintf(archivo, "INFORME DE TABLA DE ZONAS\n");
    fprintf(archivo, "Fecha de generación: %04d-%02d-%02d %02d:%02d:%02d\n\n",
            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    // Resumen de datos
    fprintf(archivo, "Resumen:\n");
    fprintf(archivo, "- Total de zonas analizadas: %d\n", numZonas);

    float suma_general_pm25 = 0;
    int total_dias = 0;
    for (int i = 0; i < numZonas; i++) {
        int mes_ultimo = zonas[i].numMeses - 1;
        if (mes_ultimo < 0) mes_ultimo = 0;
        for (int d = 0; d < zonas[i].meses[mes_ultimo].numDias; d++) {
            suma_general_pm25 += zonas[i].meses[mes_ultimo].dias[d].pm25;
            total_dias++;
        }
    }
    float promedio_general_pm25 = (total_dias > 0) ? suma_general_pm25 / total_dias : 0;
    fprintf(archivo, "- Promedio general de PM2.5: %.2f\n\n", promedio_general_pm25);

    // Detalles por zona
    fprintf(archivo, "+----+--------------------+---------------------+-----------------------------+\n");
    fprintf(archivo, "| N  | Zona               | Alerta PM2.5        | Recomendacion               |\n");
    fprintf(archivo, "+----+--------------------+---------------------+-----------------------------+\n");
    for (int i = 0; i < numZonas; i++) {
        int mes_ultimo = zonas[i].numMeses - 1;
        if (mes_ultimo < 0) mes_ultimo = 0;
        float suma_pm25 = 0;
        int numero_dias = zonas[i].meses[mes_ultimo].numDias;
        if (numero_dias == 0) numero_dias = 1;
        for (int d = 0; d < numero_dias; d++) {
            suma_pm25 += zonas[i].meses[mes_ultimo].dias[d].pm25;
        }
        float prom_pm25 = suma_pm25 / numero_dias;
        const char *alerta = "Bueno";
        const char *recomendacion = "Sin restricciones. Aire limpio.";
        if (prom_pm25 > 75) {
            alerta = "Peligroso";
            recomendacion = "Evite salir. Riesgo alto para salud.";
        } else if (prom_pm25 > 35) {
            alerta = "Moderado";
            recomendacion = "Limite ejercicio y actividades al aire libre.";
        } else if (prom_pm25 > 12) {
            alerta = "Alto";
            recomendacion = "Personas sensibles deben evitar exposicion prolongada.";
        }
        fprintf(archivo, "| %-2d | %-18s | %-19s | %-27s |\n", i + 1, zonas[i].nombre, alerta, recomendacion);
    }
    fprintf(archivo, "+----+--------------------+---------------------+-----------------------------+\n");
    fclose(archivo);
    printf(VERDE "Reporte exportado exitosamente a '%s'." RESET "\n", nombre_archivo);
}

void exportarAlertasYRecomendaciones(struct Zona zonas[], int numZonas) {
    #ifdef _WIN32
        _mkdir("reportes");
        _mkdir("reportes/exportaciones");
    #else
        mkdir("reportes", 0777);
        mkdir("reportes/exportaciones", 0777);
    #endif

    // Obtener la hora actual
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    // Crear nombre de archivo con marca de tiempo
    char nombre_archivo[128];
    snprintf(nombre_archivo, sizeof(nombre_archivo), "reportes/exportaciones/alertas_recomendaciones_%04d%02d%02d_%02d%02d%02d.txt",
             tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    FILE *archivo = fopen(nombre_archivo, "w");
    if (!archivo) {
        printf(ROJO "Error al crear el archivo de alertas y recomendaciones." RESET "\n");
        return;
    }

    // Encabezado del informe
    fprintf(archivo, "INFORME DE ALERTAS Y RECOMENDACIONES\n");
    fprintf(archivo, "Fecha de generación: %04d-%02d-%02d %02d:%02d:%02d\n\n",
            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    // Resumen de datos
    fprintf(archivo, "Resumen:\n");
    fprintf(archivo, "- Total de zonas analizadas: %d\n\n", numZonas);

    // Detalles por zona
    for (int i = 0; i < numZonas; i++) {
        int mes_ultimo = zonas[i].numMeses - 1;
        if (mes_ultimo < 0) mes_ultimo = 0;
        float suma_pm25 = 0;
        int numero_dias = zonas[i].meses[mes_ultimo].numDias;
        if (numero_dias == 0) numero_dias = 1;
        for (int d = 0; d < numero_dias; d++) {
            suma_pm25 += zonas[i].meses[mes_ultimo].dias[d].pm25;
        }
        float prom_pm25 = suma_pm25 / numero_dias;
        const char *alerta = "Bueno";
        const char *recomendacion = "Sin restricciones. Aire limpio.";
        if (prom_pm25 > 75) {
            alerta = "Peligroso";
            recomendacion = "Evite salir. Riesgo alto para salud.";
        } else if (prom_pm25 > 35) {
            alerta = "Moderado";
            recomendacion = "Limite ejercicio y actividades al aire libre.";
        } else if (prom_pm25 > 12) {
            alerta = "Alto";
            recomendacion = "Personas sensibles deben evitar exposicion prolongada.";
        }
        fprintf(archivo, "Zona: %s\nPromedio PM2.5: %.2f\nAlerta: %s\nRecomendacion: %s\n\n",
                zonas[i].nombre, prom_pm25, alerta, recomendacion);
    }
    fclose(archivo);
    printf(VERDE "Alertas y recomendaciones exportadas exitosamente a '%s'." RESET "\n", nombre_archivo);
}
