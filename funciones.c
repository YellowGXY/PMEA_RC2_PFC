#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "funciones.h"

// Definiciones de colores ANSI
#define ANSI_RESET   "\x1b[0m"
#define ANSI_NEGRITA "\x1b[1m"
#define ANSI_CIAN    "\x1b[36m"
#define ANSI_AMARILLO "\x1b[33m"
#define ANSI_VERDE   "\x1b[32m"
#define ANSI_ROJO    "\x1b[31m"
#define ANSI_MAGENTA "\x1b[35m"
#define ANSI_AZUL    "\x1b[34m"

// Constantes para validacion de datos
#define MIN_TEMP -50.0f
#define MAX_TEMP 60.0f
#define MIN_WIND 0.0f
#define MAX_WIND 100.0f
#define MIN_HUMIDITY 0.0f
#define MAX_HUMIDITY 100.0f
#define MIN_CONTAMINANT 0.0f
#define MAX_PM25 500.0f
#define MAX_OTHER_CONTAMINANTS 1.0f

// Límites OMS para contaminantes
#define OMS_PM25_BUENO 12.0f
#define OMS_PM25_MODERADO 35.0f
#define OMS_PM25_NO_SALUDABLE 55.0f
#define OMS_NO2_BUENO 53.0f
#define OMS_NO2_MODERADO 100.0f
#define OMS_NO2_NO_SALUDABLE 200.0f
#define OMS_CO2_ALERTA 0.05f

// Inicializa zonas con nombres y umbrales por defecto
void inicializarZonas(struct Zona zonas[], int *numero_zonas) {
    char *nombres[] = {"Quito", "Cuenca", "Guayaquil", "Loja", "Ambato"};
    
    for (int indice_zona = 0; indice_zona < *numero_zonas; indice_zona++) {
        // Copiar nombre de forma segura
        strncpy(zonas[indice_zona].nombre, nombres[indice_zona], MAX_NOMBRE_ZONA-1);
        zonas[indice_zona].nombre[MAX_NOMBRE_ZONA-1] = 0;
        zonas[indice_zona].numSemanas = 0;
        
        // Inicializar todas las semanas a cero
        for (int indice_semana = 0; indice_semana < MAX_SEMANAS; indice_semana++) {
            zonas[indice_zona].semanas[indice_semana].numDias = 0;
        }
        
        // Establecer umbrales por defecto basados en estándares internacionales
        zonas[indice_zona].umbrales.co2.min = 380; 
        zonas[indice_zona].umbrales.co2.max = 450;
        zonas[indice_zona].umbrales.so2.min = 5;   
        zonas[indice_zona].umbrales.so2.max = 15;
        zonas[indice_zona].umbrales.no2.min = 20;  
        zonas[indice_zona].umbrales.no2.max = 40;
        zonas[indice_zona].umbrales.pm25.min = 5;  
        zonas[indice_zona].umbrales.pm25.max = 30;
    }
}

/**
 * Inicializa la configuracion de fechas con valores por defecto
 */
void inicializarConfiguracionFechas(struct ConfiguracionFechas *config) {
    // Configuracion por defecto: año actual, enero 1
    config->anio_inicio = 2025;
    config->mes_inicio = 1;
    config->dia_inicio = 1;
    config->usar_fechas_automaticas = 1; // Por defecto usar fechas automáticas
}

/**
// Inicializa el sistema con datos NORMALES/SALUDABLES de demostración
// Los valores son intencionalmente bajos para mostrar estado saludable inicial
// Usar "Generar datos de muestreo" (Configuracion) para datos variados con alertas
 */
void inicializarSistema(struct Sistema *sistema) {
    // Nombres de las ciudades ecuatorianas por defecto
    char *nombres[5] = {"Quito", "Cuenca", "Guayaquil", "Loja", "Ambato"};
    
    // Datos de muestra NORMALES/SALUDABLES para demostracion inicial
    // Formato: {CO2, SO2, NO2, PM2.5, Temperatura, Viento, Humedad}
    float datos[5][7] = {
        {0.038, 0.008, 0.025, 8.5, 18.5, 2.8, 62.0},   // Quito - aire saludable
        {0.040, 0.006, 0.022, 7.2, 16.0, 3.2, 58.0},   // Cuenca - aire saludable  
        {0.042, 0.010, 0.028, 9.8, 26.0, 4.1, 72.0},   // Guayaquil - aire saludable
        {0.036, 0.005, 0.020, 6.5, 22.0, 2.5, 55.0},   // Loja - aire saludable
        {0.039, 0.007, 0.024, 8.0, 14.5, 3.0, 60.0}    // Ambato - aire saludable
    };
    
    sistema->numZonas = 5;
    
    // Inicializar cada zona con sus datos correspondientes
    for (int indice_zona = 0; indice_zona < 5; indice_zona++) {
        // Copiar nombre de zona de forma segura
        strncpy(sistema->zonas[indice_zona].nombre, nombres[indice_zona], 31);
        sistema->zonas[indice_zona].nombre[31] = '\0';
        
        // Asignar valores de contaminantes y condiciones ambientales
        sistema->zonas[indice_zona].co2 = datos[indice_zona][0];
        sistema->zonas[indice_zona].so2 = datos[indice_zona][1];
        sistema->zonas[indice_zona].no2 = datos[indice_zona][2];
        sistema->zonas[indice_zona].pm25 = datos[indice_zona][3];
        sistema->zonas[indice_zona].temperatura = datos[indice_zona][4];
        sistema->zonas[indice_zona].viento = datos[indice_zona][5];
        sistema->zonas[indice_zona].humedad = datos[indice_zona][6];
    }
    
    // Guardar datos inicializados en archivo
    guardarDatos(sistema, "datos_hist.dat");
    printf("Datos guardados correctamente.\n");
}

/**
 * Carga datos historicos desde un archivo binario
 * Devuelve 1 si la carga fue exitosa, 0 en caso contrario
 */
int cargarDatosHistoricos(struct Sistema *sistema, char *ruta_archivo) {
    FILE *archivo = fopen(ruta_archivo, "rb");
    
    if (!archivo) {
        return 0; // No se pudo abrir el archivo
    }
    
    // Leer la estructura completa del sistema
    fread(sistema, sizeof(struct Sistema), 1, archivo);
    fclose(archivo);
    
    return 1; // Carga exitosa
}

/**
 * Guarda los datos del sistema en un archivo binario
 * Devuelve 1 si el guardado fue exitoso, 0 en caso contrario
 */
int guardarDatos(struct Sistema *sistema, char *ruta_archivo) {
    FILE *archivo = fopen(ruta_archivo, "wb");
    
    if (!archivo) {
        return 0; // No se pudo crear el archivo
    }
    
    // Escribir la estructura completa del sistema
    fwrite(sistema, sizeof(struct Sistema), 1, archivo);
    fclose(archivo);
    
    return 1; // Guardado exitoso
}

/**
 * Calcula los promedios de todos los contaminantes para todas las zonas
 * Los guarda en el array promedios: [CO2, SO2, NO2, PM2.5]
 */
void calcularPromedios(struct Sistema *sistema, float promedios[]) {
    float sumas[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    
    // Sumar todos los valores de contaminantes de todas las zonas
    for (int indice_zona = 0; indice_zona < sistema->numZonas; indice_zona++) {
        sumas[0] += sistema->zonas[indice_zona].co2;
        sumas[1] += sistema->zonas[indice_zona].so2;
        sumas[2] += sistema->zonas[indice_zona].no2;
        sumas[3] += sistema->zonas[indice_zona].pm25;
    }
    
    // Calcular promedios dividiendo por el número de zonas
    for (int indice_contaminante = 0; indice_contaminante < 4; indice_contaminante++) {
        if (sistema->numZonas > 0) {
            promedios[indice_contaminante] = sumas[indice_contaminante] / sistema->numZonas;
        } else {
            promedios[indice_contaminante] = 0.0f;
        }
    }
}

/**
 * Predice la contaminacion para las proximas 24 horas usando un modelo ponderado
 * Guarda las predicciones en el array prediccion
 */
void predecirContaminacion(struct Sistema *sistema, float prediccion[]) {
    float promedios[4];
    calcularPromedios(sistema, promedios);
    float pm_promedio = promedios[3]; // Promedio global de PM2.5
    
    // Calcular prediccion para cada zona usando modelo hibrido
    for (int indice_zona = 0; indice_zona < sistema->numZonas; indice_zona++) {
        // 70% valor actual + 30% promedio global para suavizar fluctuaciones
        prediccion[indice_zona] = sistema->zonas[indice_zona].pm25 * 0.7f + pm_promedio * 0.3f;
    }
    
    // Guardar predicciones en archivo binario para análisis posterior
    FILE *archivo = fopen("datos_pred.dat", "wb");
    if (archivo) {
        fwrite(prediccion, sizeof(float), sistema->numZonas, archivo);
        fclose(archivo);
    }
}

/**
 * Emite alertas cuando los niveles de contaminacion superan los limites OMS
 * Llena el array de alertas y actualiza el contador numero_alertas
 */
void emitirAlertas(struct Sistema *sistema, float prediccion[], char alertas[][64], int *numero_alertas) {
    *numero_alertas = 0;
    
    // Verificar cada zona para detectar niveles peligrosos
    for (int indice_zona = 0; indice_zona < sistema->numZonas; indice_zona++) {
        // Alerta por PM2.5 alto (límite OMS: 35 µg/m³)
        if (prediccion[indice_zona] > OMS_PM25_MODERADO) {
            snprintf(alertas[*numero_alertas], 64, "Alerta: %s PM2.5 > 35 ug/m3", sistema->zonas[indice_zona].nombre);
            (*numero_alertas)++;
        }
        
        // Alerta adicional por CO2 alto
        if (sistema->zonas[indice_zona].co2 > OMS_CO2_ALERTA) {
            snprintf(alertas[*numero_alertas], 64, "Alerta: %s CO2 > 0.05 ppm", sistema->zonas[indice_zona].nombre);
            (*numero_alertas)++;
        }
    }
}

/**
 * Genera recomendaciones basadas en las alertas emitidas
 */
void generarRecomendaciones(char alertas[][64], int numero_alertas) {
    if (numero_alertas == 0) {
        printf("No hay alertas. Calidad del aire aceptable.\n");
        return;
    }
    
    printf("Recomendaciones:\n");
    
    // Mostrar recomendaciones específicas para cada alerta
    for (int indice_alerta = 0; indice_alerta < numero_alertas; indice_alerta++) {
        printf("- %s: Limite actividades al aire libre.\n", alertas[indice_alerta]);
    }
    
    // Recomendaciones generales cuando hay alertas
    if (numero_alertas > 0) {
        printf("- Considere usar mascarilla en exteriores.\n");
        printf("- Mantenga puertas y ventanas cerradas.\n");
    }
}

/**
 * Muestra una tabla formateada con todas las zonas y sus contaminantes
 */
void mostrarTablaZonas(struct Sistema *sistema) {
    // Encabezado de la tabla
    printf("+--------+-------+-------+-------+-------+\n");
    printf("| Zona   | CO2   | SO2   | NO2   | PM2.5 |\n");
    printf("+--------+-------+-------+-------+-------+\n");
    
    // Datos de cada zona
    for (int indice_zona = 0; indice_zona < sistema->numZonas; indice_zona++) {
        printf("| %-6s | %5.2f | %5.2f | %5.2f | %5.1f |\n",
            sistema->zonas[indice_zona].nombre, sistema->zonas[indice_zona].co2, sistema->zonas[indice_zona].so2,
            sistema->zonas[indice_zona].no2, sistema->zonas[indice_zona].pm25);
    }
    
    // Pie de la tabla
    printf("+--------+-------+-------+-------+-------+\n");
}

// ===============================
// FUNCIONES ROBUSTAS DE ENTRADA DE DATOS
// ===============================

/**
 * Limpia el buffer de entrada estándar
 */
void limpiarBufferEntrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/**
 * Valida que una cadena contenga un entero válido dentro del rango especificado
 * Retorna 1 si es válido, 0 si no
 */
int validarEnteroEnRango(const char *entrada, int min, int max) {
    char *endptr;
    long valor = strtol(entrada, &endptr, 10);
    
    // Verificar que toda la cadena fue procesada y no hay overflow
    if (*endptr != '\0' && *endptr != '\n' && *endptr != '\r') {
        return 0;
    }
    
    // Verificar rango
    if (valor < min || valor > max) {
        return 0;
    }
    
    return 1;
}

/**
 * Valida que una cadena contenga un float válido dentro del rango especificado
 * Retorna 1 si es válido, 0 si no
 */
int validarFloatEnRango(const char *entrada, float min, float max) {
    char *endptr;
    double valor = strtod(entrada, &endptr);
    
    // Verificar que toda la cadena fue procesada
    if (*endptr != '\0' && *endptr != '\n' && *endptr != '\r') {
        return 0;
    }
    
    // Verificar rango
    if (valor < min || valor > max) {
        return 0;
    }
    
    return 1;
}

/**
 * Lee un entero de forma segura con validación de rango
 */
int leerEnteroSeguro(const char *mensaje, int min, int max) {
    char buffer[256];
    int valor;
    
    while (1) {
        printf("%s", mensaje);
        
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            printf("Error de lectura. Intente nuevamente.\n");
            continue;
        }
        
        // Remover salto de línea si existe
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
        
        if (validarEnteroEnRango(buffer, min, max)) {
            sscanf(buffer, "%d", &valor);
            return valor;
        }
        
        printf("Entrada invalida. Debe ser un numero entero entre %d y %d.\n", min, max);
    }
}

/**
 * Lee un float de forma segura con validación de rango
 */
float leerFloatSeguro(const char *mensaje, float min, float max) {
    char buffer[256];
    float valor;
    
    while (1) {
        printf("%s", mensaje);
        
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            printf("Error de lectura. Intente nuevamente.\n");
            continue;
        }
        
        // Remover salto de línea si existe
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
        
        if (validarFloatEnRango(buffer, min, max)) {
            sscanf(buffer, "%f", &valor);
            return valor;
        }
        
        printf("Entrada invalida. Debe ser un numero decimal entre %.2f y %.2f.\n", min, max);
    }
}

/**
 * Lee un carácter de forma segura
 */
char leerCaracterSeguro(const char *mensaje) {
    char buffer[256];
    
    while (1) {
        printf("%s", mensaje);
        
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            printf("Error de lectura. Intente nuevamente.\n");
            continue;
        }
        
        // Verificar que hay al menos un carácter
        if (strlen(buffer) > 0 && buffer[0] != '\n') {
            return buffer[0];
        }
        
        printf("Entrada invalida. Debe ingresar al menos un caracter.\n");
    }
}

/**
 * Lee una cadena de forma segura con validación de longitud
 */
void leerCadenaSegura(const char *mensaje, char *destino, int tamano_max) {
    char buffer[512];
    
    while (1) {
        printf("%s", mensaje);
        
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            printf("Error de lectura. Intente nuevamente.\n");
            continue;
        }
        
        // Remover salto de línea si existe
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
            len--;
        }
        
        // Verificar longitud
        if (len > 0 && len < tamano_max) {
            strcpy(destino, buffer);
            return;
        }
        
        if (len == 0) {
            printf("Entrada invalida. Debe ingresar al menos un caracter.\n");
        } else {
            printf("Entrada demasiado larga. Maximo %d caracteres.\n", tamano_max - 1);
        }
    }
}

/**
 * Muestra el historial de datos de todas las zonas desde archivo
 */
void mostrarHistorialZonas() {
    struct Sistema sistema;
    FILE *archivo = fopen("datos_hist.dat", "rb");
    
    if (!archivo) {
        printf("No hay historial disponible.\n");
        return;
    }
    
    // Cargar datos historicos
    fread(&sistema, sizeof(struct Sistema), 1, archivo);
    fclose(archivo);
    
    printf("Historial de zonas:\n");
    mostrarTablaZonas(&sistema);
}

// Muestra informacion detallada de una zona especifica
void mostrarDetalleZona(struct Sistema *sistema) {
    if (sistema->numZonas == 0) {
        printf("No hay zonas cargadas.\n");
        return;
    }
    
    // Mostrar lista de zonas disponibles
    printf("Seleccione zona para ver detalles (0-%d, c para cancelar):\n", sistema->numZonas-1);
    for (int indice_zona = 0; indice_zona < sistema->numZonas; indice_zona++) {
        printf("%d. %s\n", indice_zona, sistema->zonas[indice_zona].nombre);
    }
    
    // Solicitar seleccion al usuario
    int indice_seleccionado = leerEnteroSeguro("Opcion: ", 0, sistema->numZonas-1);
    if (indice_seleccionado < 0) {
        return; // Usuario canceló
    }
    
    // Mostrar detalles de la zona seleccionada
    struct Zona *zona_seleccionada = &sistema->zonas[indice_seleccionado];
    printf("+--------------------------------------+\n");
    printf("| Detalle de zona: %-18s |\n", zona_seleccionada->nombre);
    printf("+--------------------------------------+\n");
    printf("| CO2:        %6.3f ppm                |\n", zona_seleccionada->co2);
    printf("| SO2:        %6.3f ppm                |\n", zona_seleccionada->so2);
    printf("| NO2:        %6.3f ppm                |\n", zona_seleccionada->no2);
    printf("| PM2.5:      %6.1f ug/m3              |\n", zona_seleccionada->pm25);
    printf("| Temp:       %6.1f C                  |\n", zona_seleccionada->temperatura);
    printf("| Viento:     %6.1f m/s                |\n", zona_seleccionada->viento);
    printf("| Humedad:    %6.1f %%                  |\n", zona_seleccionada->humedad);
    printf("+--------------------------------------+\n");
}

// Busca una zona por nombre (búsqueda insensible a mayúsculas/minúsculas)
void buscarZonaPorNombre(struct Sistema *sistema) {
    if (sistema->numZonas == 0) {
        printf("No hay zonas cargadas.\n");
        return;
    }
    
    char nombre_buscado[32];
    printf("Ingrese nombre de la zona a buscar (c para cancelar): ");
    
    if (!fgets(nombre_buscado, sizeof(nombre_buscado), stdin)) {
        return; // Error de lectura
    }
    
    if (nombre_buscado[0] == 'c' || nombre_buscado[0] == 'C') {
        return; // Usuario canceló
    }
    
    // Eliminar salto de línea
    nombre_buscado[strcspn(nombre_buscado, "\n")] = 0;
    
    // Buscar zona comparando nombres (insensible a mayúsculas)
    for (int indice_zona = 0; indice_zona < sistema->numZonas; indice_zona++) {
        int nombres_iguales = 1;
        
        // Comparación caracter por caracter ignorando mayúsculas
        for (int indice_caracter = 0; indice_caracter < 32; indice_caracter++) {
            char caracter_buscado = nombre_buscado[indice_caracter];
            char caracter_zona = sistema->zonas[indice_zona].nombre[indice_caracter];
            
            // Convertir a minúsculas para comparación
            if (caracter_buscado >= 'A' && caracter_buscado <= 'Z') {
                caracter_buscado = caracter_buscado + ('a' - 'A');
            }
            if (caracter_zona >= 'A' && caracter_zona <= 'Z') {
                caracter_zona = caracter_zona + ('a' - 'A');
            }
            
            if (caracter_buscado != caracter_zona) {
                nombres_iguales = 0;
                break;
            }
            
            if (caracter_buscado == 0 && caracter_zona == 0) {
                break; // Fin de ambas cadenas
            }
        }
        
        if (nombres_iguales) {
            struct Zona *zona_encontrada = &sistema->zonas[indice_zona];
            printf("Zona encontrada:\n");
            printf("CO2: %.3f, SO2: %.3f, NO2: %.3f, PM2.5: %.1f, Temp: %.1f, Viento: %.1f, Humedad: %.1f\n",
                zona_encontrada->co2, zona_encontrada->so2, zona_encontrada->no2, zona_encontrada->pm25, 
                zona_encontrada->temperatura, zona_encontrada->viento, zona_encontrada->humedad);
            return;
        }
    }
    
    printf("Zona no encontrada.\n");
}

// Calcula el Índice de Calidad del Aire (ICA) basado en PM2.5
// Devuelve una descripción del nivel de calidad del aire
char* calcularICA(float pm25) {
    if (pm25 <= OMS_PM25_BUENO) {
        return "Bueno";
    } else if (pm25 <= OMS_PM25_MODERADO) {
        return "Moderado";
    } else if (pm25 <= OMS_PM25_NO_SALUDABLE) {
        return "No saludable";
    } else {
        return "Peligroso";
    }
}

/**
// Muestra alertas con colores según el nivel de ICA
 */
void mostrar_alerta(float pm25, char* zona) {
    char* nivel = calcularICA(pm25);
    
    if (strcmp(nivel, "Bueno") == 0) {
        printf("Zona %s: " ANSI_VERDE "%s" ANSI_RESET " Aire saludable.\n", zona, nivel);
    } else if (strcmp(nivel, "Moderado") == 0) {
        printf("Zona %s: " ANSI_AMARILLO "%s" ANSI_RESET " Evitar esfuerzo fisico prolongado.\n", zona, nivel);
    } else if (strcmp(nivel, "No saludable") == 0) {
        printf("Zona %s: " ANSI_ROJO "%s" ANSI_RESET " Riesgo para grupos sensibles.\n", zona, nivel);
    } else {
        printf("Zona %s: " ANSI_MAGENTA "%s" ANSI_RESET " Evitar salir, usar mascarilla.\n", zona, nivel);
    }
}

/**
 * Registra predicciones en archivo de texto delimitado por punto y coma


 */
void registrarPredicciones(struct Sistema *sistema, float prediccion[]) {
    FILE *archivo_predicciones = fopen("predicciones.txt", "a");
    
    if (!archivo_predicciones) {
        return; // No se pudo abrir el archivo
    }
    
    // Escribir predicciones para cada zona
    for (int indice_zona = 0; indice_zona < sistema->numZonas; indice_zona++) {
        const char* nivel_calidad = calcularICA(prediccion[indice_zona]);
        fprintf(archivo_predicciones, "%s;%.1f;%.1f;%s\n", 
                sistema->zonas[indice_zona].nombre, sistema->zonas[indice_zona].pm25, 
                prediccion[indice_zona], nivel_calidad);
    }
    
    fclose(archivo_predicciones);
}

/**
 * Menú para editar nombre y umbrales de una zona específica


 */
void menuConfiguracionZona(struct Zona zonas[], int numero_zonas) {
    char mensaje[64];
    sprintf(mensaje, "Seleccione zona (0-%d): ", numero_zonas-1);
    int indice_zona_seleccionada = leerEnteroSeguro(mensaje, 0, numero_zonas-1);
    
    if (indice_zona_seleccionada >= 0 && indice_zona_seleccionada < numero_zonas) {
        printf("Nombre actual: %s\n", zonas[indice_zona_seleccionada].nombre);
        leerCadenaSegura("Nuevo nombre: ", zonas[indice_zona_seleccionada].nombre, MAX_NOMBRE_ZONA);
        editarUmbrales(&zonas[indice_zona_seleccionada].umbrales);
    } else {
        printf("Zona invalida.\n");
    }
}

/**
 * Permite editar los umbrales de contaminantes para una zona

 */
void editarUmbrales(struct Umbrales *umbrales) {
    printf("Editar umbrales (min max):\n");
    
    umbrales->co2.min = leerFloatSeguro("CO2 minimo (ppm): ", 0.0, 10000.0);
    umbrales->co2.max = leerFloatSeguro("CO2 maximo (ppm): ", umbrales->co2.min, 10000.0);
    
    umbrales->so2.min = leerFloatSeguro("SO2 minimo (µg/m³): ", 0.0, 1000.0);
    umbrales->so2.max = leerFloatSeguro("SO2 maximo (µg/m³): ", umbrales->so2.min, 1000.0);
    
    umbrales->no2.min = leerFloatSeguro("NO2 minimo (µg/m³): ", 0.0, 1000.0);
    umbrales->no2.max = leerFloatSeguro("NO2 maximo (µg/m³): ", umbrales->no2.min, 1000.0);
    
    umbrales->pm25.min = leerFloatSeguro("PM2.5 minimo (µg/m³): ", 0.0, 500.0);
    umbrales->pm25.max = leerFloatSeguro("PM2.5 maximo (µg/m³): ", umbrales->pm25.min, 500.0);
}

/**
 * Cambia el nombre de una zona de forma interactiva

 */
void cambiarNombreZona(char *nombreZona) {
    char nuevoNombre[MAX_NOMBRE_ZONA];
    leerCadenaSegura("Ingrese el nuevo nombre para la zona: ", nuevoNombre, MAX_NOMBRE_ZONA);
    
    strncpy(nombreZona, nuevoNombre, MAX_NOMBRE_ZONA-1);
    nombreZona[MAX_NOMBRE_ZONA-1] = 0;
    printf("Nombre de zona actualizado a: %s\n", nombreZona);
}

/**
 * Permite ingresar datos manualmente para una semana específica


 */
void ingresarDatosManualSemana(struct Zona *zona, int numero_semana) {
    if (numero_semana >= MAX_SEMANAS) {
        printf("Semana invalida. Debe estar entre 0 y %d.\n", MAX_SEMANAS-1);
        return;
    }
    
    zona->semanas[numero_semana].numDias = MAX_DIAS_SEMANA;
    
    for (int indice_dia = 0; indice_dia < MAX_DIAS_SEMANA; indice_dia++) {
        printf("\n--- Día %d ---\n", indice_dia+1);
        
        leerCadenaSegura("Fecha (YYYY-MM-DD): ", zona->semanas[numero_semana].dias[indice_dia].fecha, 11);
        zona->semanas[numero_semana].dias[indice_dia].co2 = leerFloatSeguro("CO2 (ppm): ", 0.0, 1.0);
        zona->semanas[numero_semana].dias[indice_dia].so2 = leerFloatSeguro("SO2 (µg/m³): ", 0.0, 100.0);
        zona->semanas[numero_semana].dias[indice_dia].no2 = leerFloatSeguro("NO2 (µg/m³): ", 0.0, 300.0);
        zona->semanas[numero_semana].dias[indice_dia].pm25 = leerFloatSeguro("PM2.5 (µg/m³): ", 0.0, 500.0);
    }
    
    // Actualizar el número de semanas si es necesario
    if (numero_semana >= zona->numSemanas) {
        zona->numSemanas = numero_semana + 1;
    }
}

/**
 * Guarda los datos de una semana específica en un archivo checkpoint


 */
void guardarSemana(struct Zona *zona, int numero_semana) {
    char nombre_archivo[64];
    sprintf(nombre_archivo, "%s_semana%d.dat", zona->nombre, numero_semana+1);
    
    FILE *archivo = fopen(nombre_archivo, "wb");
    if (archivo) {
        fwrite(&zona->semanas[numero_semana], sizeof(struct Semana), 1, archivo);
        fclose(archivo);
        printf("Semana guardada en %s\n", nombre_archivo);
    } else {
        printf("Error: No se pudo guardar la semana en %s\n", nombre_archivo);
    }
}

/**
 * Carga los datos de una semana específica desde un archivo checkpoint


 */
void cargarSemana(struct Zona *zona, int numero_semana) {
    char nombre_archivo[64];
    sprintf(nombre_archivo, "%s_semana%d.dat", zona->nombre, numero_semana+1);
    
    FILE *archivo = fopen(nombre_archivo, "rb");
    if (archivo) {
        fread(&zona->semanas[numero_semana], sizeof(struct Semana), 1, archivo);
        fclose(archivo);
        printf("Semana %d cargada de %s\n", numero_semana+1, nombre_archivo);
        
        // Actualizar número de semanas si es necesario
        if (numero_semana >= zona->numSemanas) {
            zona->numSemanas = numero_semana + 1;
        }
    } else {
        printf("No existe checkpoint para esa semana.\n");
    }
}

/**
 * Guarda el estado actual de las semanas en un archivo
 */
void guardarSemanaActual(int semanaActual[], int numZonas) {
    FILE *archivo = fopen("semanas_actuales.dat", "wb");
    if (archivo) {
        fwrite(&numZonas, sizeof(int), 1, archivo);
        fwrite(semanaActual, sizeof(int), numZonas, archivo);
        fclose(archivo);
    }
}

/**
 * Carga el estado actual de las semanas desde un archivo
 */
void cargarSemanaActual(int semanaActual[], int numZonas) {
    FILE *archivo = fopen("semanas_actuales.dat", "rb");
    if (archivo) {
        int numZonasCargadas;
        fread(&numZonasCargadas, sizeof(int), 1, archivo);
        if (numZonasCargadas == numZonas) {
            fread(semanaActual, sizeof(int), numZonas, archivo);
        }
        fclose(archivo);
    }
}

/**
 * Carga los datos de las zonas desde sus archivos de checkpoint
 */
void cargarDatosZonas(struct Zona zonas[], int numZonas) {
    for (int i = 0; i < numZonas; i++) {
        // Buscar el archivo de checkpoint más reciente para esta zona
        for (int semana = MAX_SEMANAS - 1; semana >= 0; semana--) {
            char nombre_archivo[64];
            sprintf(nombre_archivo, "%s_semana%d.dat", zonas[i].nombre, semana + 1);
            
            FILE *archivo = fopen(nombre_archivo, "rb");
            if (archivo) {
                fread(&zonas[i].semanas[semana], sizeof(struct Semana), 1, archivo);
                fclose(archivo);
                
                // Actualizar el número de semanas de la zona
                if (semana >= zonas[i].numSemanas) {
                    zonas[i].numSemanas = semana + 1;
                }
            }
        }
    }
}

/**
 * Configura las fechas de inicio para el sistema de registro automático
 */
void configurarFechasInicio(struct ConfiguracionFechas *config) {
    printf("Configuracion de fechas de inicio para registro automatico\n");
    printf("=======================================================\n");
    
    config->anio_inicio = leerEnteroSeguro("Año de inicio (2020-2030): ", 2020, 2030);
    config->mes_inicio = leerEnteroSeguro("Mes de inicio (1-12): ", 1, 12);
    config->dia_inicio = leerEnteroSeguro("Dia de inicio (1-31): ", 1, 31);
    config->usar_fechas_automaticas = leerEnteroSeguro("¿Usar fechas automaticas para nuevos registros? (1=Si, 0=No): ", 0, 1);
    
    printf("Fecha de inicio configurada: %04d-%02d-%02d\n", 
           config->anio_inicio, config->mes_inicio, config->dia_inicio);
    
    if (config->usar_fechas_automaticas) {
        printf("Las fechas se generaran automaticamente a partir de esta fecha.\n");
    } else {
        printf("Se pedira la fecha manualmente para cada registro.\n");
    }
    
    // Guardar automáticamente la configuración
    guardarConfiguracionFechas(config);
}

/**
 * Calcula una fecha automática basada en la configuración y días relativos
 */
void calcularFechaAutomatica(struct ConfiguracionFechas *config, int dia_relativo, char *fecha_resultado) {
    // Días por mes (año no bisiesto)
    int dias_mes[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    int anio = config->anio_inicio;
    int mes = config->mes_inicio;
    int dia = config->dia_inicio + dia_relativo;
    
    // Ajustar año bisiesto
    if ((anio % 4 == 0 && anio % 100 != 0) || (anio % 400 == 0)) {
        dias_mes[1] = 29;
    }
    
    // Ajustar día y mes si excede los límites
    while (dia > dias_mes[mes-1]) {
        dia -= dias_mes[mes-1];
        mes++;
        if (mes > 12) {
            mes = 1;
            anio++;
            // Recalcular año bisiesto para el nuevo año
            if ((anio % 4 == 0 && anio % 100 != 0) || (anio % 400 == 0)) {
                dias_mes[1] = 29;
            } else {
                dias_mes[1] = 28;
            }
        }
    }
    
    sprintf(fecha_resultado, "%04d-%02d-%02d", anio, mes, dia);
}

/**
 * Versión mejorada del ingreso manual de datos con mejor validación y fechas automáticas
 */
void ingresarDatosManualMejorado(struct Zona *zona, int semana, struct ConfiguracionFechas *config) {
    if (semana >= MAX_SEMANAS) {
        printf("Semana invalida. Debe estar entre 0 y %d.\n", MAX_SEMANAS-1);
        return;
    }
    
    printf("\n+---------------------------------------------------------------+\n");
    printf("| INGRESO MANUAL DE DATOS - ZONA: %-28s |\n", zona->nombre);
    printf("| SEMANA: %-3d                                                  |\n", semana+1);
    printf("+---------------------------------------------------------------+\n");
    printf("| Instrucciones:                                                |\n");
    printf("| - Ingrese solo numeros (sin acentos ni caracteres especiales)|\n");
    printf("| - Valores fuera de rango seran rechazados                    |\n");
    printf("| - Escriba 'c' para cancelar en cualquier momento             |\n");
    printf("+---------------------------------------------------------------+\n\n");
    
    zona->semanas[semana].numDias = 0; // Reiniciar contador de días
    
    for (int d = 0; d < MAX_DIAS_SEMANA; d++) {
        printf("--- DIA %d ---\n", d+1);
        
        // Manejo de fechas
        if (config->usar_fechas_automaticas) {
            int dia_relativo = (semana * MAX_DIAS_SEMANA) + d;
            calcularFechaAutomatica(config, dia_relativo, zona->semanas[semana].dias[d].fecha);
            printf("Fecha automatica: %s\n", zona->semanas[semana].dias[d].fecha);
        } else {
            char fecha_temp[12];
            leerCadenaSegura("Fecha (YYYY-MM-DD): ", fecha_temp, 11);
            
            if (fecha_temp[0] == 'c' || fecha_temp[0] == 'C') {
                printf("Ingreso cancelado.\n");
                return;
            }
            
            strncpy(zona->semanas[semana].dias[d].fecha, fecha_temp, 11);
            zona->semanas[semana].dias[d].fecha[10] = '\0';
        }
        
        // Ingreso de contaminantes con validación mejorada
        char entrada[32];
        float valor;
        
        // CO2
        while (1) {
            leerCadenaSegura("CO2 en ppm (0.0-1.0) o 'c' para cancelar: ", entrada, 31);
            
            if (entrada[0] == 'c' || entrada[0] == 'C') {
                printf("Ingreso cancelado.\n");
                return;
            }
            
            if (validarEntradaNumerica(entrada, &valor, 0.0f, 1.0f)) {
                zona->semanas[semana].dias[d].co2 = valor;
                break;
            } else {
                printf("Error: Valor invalido. Debe estar entre 0.0 y 1.0\n");
            }
        }
        
        // SO2
        while (1) {
            leerCadenaSegura("SO2 en ug/m3 (0.0-100.0) o 'c' para cancelar: ", entrada, 31);
            
            if (entrada[0] == 'c' || entrada[0] == 'C') {
                printf("Ingreso cancelado.\n");
                return;
            }
            
            if (validarEntradaNumerica(entrada, &valor, 0.0f, 100.0f)) {
                zona->semanas[semana].dias[d].so2 = valor;
                break;
            } else {
                printf("Error: Valor invalido. Debe estar entre 0.0 y 100.0\n");
            }
        }
        
        // NO2
        while (1) {
            leerCadenaSegura("NO2 en ug/m3 (0.0-300.0) o 'c' para cancelar: ", entrada, 31);
            
            if (entrada[0] == 'c' || entrada[0] == 'C') {
                printf("Ingreso cancelado.\n");
                return;
            }
            
            if (validarEntradaNumerica(entrada, &valor, 0.0f, 300.0f)) {
                zona->semanas[semana].dias[d].no2 = valor;
                break;
            } else {
                printf("Error: Valor invalido. Debe estar entre 0.0 y 300.0\n");
            }
        }
        
        // PM2.5
        while (1) {
            leerCadenaSegura("PM2.5 en ug/m3 (0.0-500.0) o 'c' para cancelar: ", entrada, 31);
            
            if (entrada[0] == 'c' || entrada[0] == 'C') {
                printf("Ingreso cancelado.\n");
                return;
            }
            
            if (validarEntradaNumerica(entrada, &valor, 0.0f, 500.0f)) {
                zona->semanas[semana].dias[d].pm25 = valor;
                break;
            } else {
                printf("Error: Valor invalido. Debe estar entre 0.0 y 500.0\n");
            }
        }
        
        zona->semanas[semana].numDias = d + 1;
        printf("Dia %d registrado correctamente.\n\n", d+1);
    }
    
    // Actualizar el número de semanas si es necesario
    if (semana >= zona->numSemanas) {
        zona->numSemanas = semana + 1;
    }
    
    printf("Semana %d completada para la zona %s.\n", semana+1, zona->nombre);
}

/**
 * Limpia el buffer de entrada para evitar problemas con caracteres residuales
 */
void limpiarBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        // Limpiar buffer
    }
}

/**
 * Valida entrada numérica y convierte a float con rango específico
 */
int validarEntradaNumerica(char *entrada, float *resultado, float min, float max) {
    char *endptr;
    float valor = strtof(entrada, &endptr);
    
    // Verificar si la conversión fue exitosa y está en rango
    if (*endptr == '\0' && valor >= min && valor <= max) {
        *resultado = valor;
        return 1;
    }
    return 0;
}

/**
 * Exporta una plantilla simple y fácil de usar para carga masiva
 * Solo requiere completar valores separados por comas
 */
void exportarPlantillaDatos(const char* nombreArchivo) {
    FILE *f = fopen(nombreArchivo, "w");
    if (!f) {
        printf(ANSI_ROJO "Error: No se pudo crear el archivo %s\n" ANSI_RESET, nombreArchivo);
        return;
    }
    
    // Instrucciones súper simples
    fprintf(f, "PLANTILLA SIMPLE PARA DATOS AMBIENTALES\n");
    fprintf(f, "=======================================\n\n");
    fprintf(f, "Formato: Zona,Fecha,CO2,SO2,NO2,PM25\n");
    fprintf(f, "- Zona: Quito, Cuenca, Guayaquil, Loja, Ambato\n");
    fprintf(f, "- Fecha: YYYY-MM-DD\n");
    fprintf(f, "- CO2: 0.0-1.0 (ppm)\n");
    fprintf(f, "- SO2: 0.0-100.0 (ug/m³)\n");
    fprintf(f, "- NO2: 0.0-300.0 (ug/m³)\n");
    fprintf(f, "- PM25: 0.0-500.0 (ug/m³)\n\n");
    
    // Datos de ejemplo
    fprintf(f, "# Datos de ejemplo - Reemplazar con valores reales\n");
    fprintf(f, "Quito,2024-07-01,0.040,8.0,25.0,15.0\n");
    fprintf(f, "Cuenca,2024-07-01,0.042,9.0,27.0,18.0\n");
    fprintf(f, "Guayaquil,2024-07-01,0.045,12.0,30.0,22.0\n");
    fprintf(f, "Loja,2024-07-01,0.037,7.0,23.0,13.0\n");
    fprintf(f, "Ambato,2024-07-01,0.041,8.5,26.0,16.0\n");
    
    fclose(f);
    printf(ANSI_VERDE "Plantilla creada: %s\n" ANSI_RESET, nombreArchivo);
}

/**
 * Importa datos desde un archivo con formato: Zona,Fecha,CO2,SO2,NO2,PM25
 */
int importarDatosDesdeArchivo(struct Zona zonas[], int numZonas, const char* nombreArchivo) {
    FILE *f = fopen(nombreArchivo, "r");
    if (!f) {
        printf(ANSI_ROJO "Error: No se pudo abrir %s\n" ANSI_RESET, nombreArchivo);
        return 0;
    }
    
    char linea[256];
    int importados = 0;
    int lineas_procesadas = 0;
    
    printf(ANSI_AMARILLO "Importando datos desde %s...\n" ANSI_RESET, nombreArchivo);
    
    while (fgets(linea, sizeof(linea), f)) {
        lineas_procesadas++;
        
        // Saltar líneas de comentario y líneas vacías
        if (linea[0] == '#' || linea[0] == '\n' || linea[0] == '\r') {
            continue;
        }
        
        // Parsear línea manualmente para evitar problemas de compatibilidad
        char zona[32], fecha[12];
        float co2, so2, no2, pm25;
        
        // Buscar las comas y extraer cada campo
        char *ptr = linea;
        char *coma;
        int leidos = 0;
        
        // Extraer zona (hasta la primera coma)
        coma = strchr(ptr, ',');
        if (coma) {
            int len = coma - ptr;
            if (len > 31) len = 31;
            strncpy(zona, ptr, len);
            zona[len] = '\0';
            ptr = coma + 1;
            leidos++;
            
            // Extraer fecha (hasta la segunda coma)
            coma = strchr(ptr, ',');
            if (coma) {
                len = coma - ptr;
                if (len > 11) len = 11;
                strncpy(fecha, ptr, len);
                fecha[len] = '\0';
                ptr = coma + 1;
                leidos++;
                
                // Extraer los 4 valores numéricos
                if (sscanf(ptr, "%f,%f,%f,%f", &co2, &so2, &no2, &pm25) == 4) {
                    leidos = 6; // Total de campos leídos correctamente
                }
            }
        }
        
        if (leidos == 6) {
            // Buscar la zona
            int zona_idx = -1;
            for (int i = 0; i < numZonas; i++) {
                if (strcmp(zonas[i].nombre, zona) == 0) {
                    zona_idx = i;
                    break;
                }
            }
            
            if (zona_idx >= 0) {
                // Validar que los datos estén en rangos razonables
                if (co2 >= 0 && co2 <= 1000 && so2 >= 0 && so2 <= 100 && 
                    no2 >= 0 && no2 <= 300 && pm25 >= 0 && pm25 <= 500) {
                    
                    // Encontrar semana y día disponible
                    int semana = zonas[zona_idx].numSemanas;
                    if (semana >= MAX_SEMANAS) semana = MAX_SEMANAS - 1;
                    
                    int dia = (semana < zonas[zona_idx].numSemanas) ? 
                              zonas[zona_idx].semanas[semana].numDias : 0;
                    
                    if (dia >= MAX_DIAS_SEMANA) {
                        semana++;
                        dia = 0;
                        if (semana >= MAX_SEMANAS) {
                            printf(ANSI_AMARILLO "Zona %s llena, saltando resto de datos\n" ANSI_RESET, zona);
                            continue;
                        }
                    }
                    
                    // Asegurar que la semana existe
                    if (semana >= zonas[zona_idx].numSemanas) {
                        zonas[zona_idx].numSemanas = semana + 1;
                        zonas[zona_idx].semanas[semana].numDias = 0;
                    }
                    
                    // Agregar el dato
                    struct DatosAmbientales *dato = &zonas[zona_idx].semanas[semana].dias[dia];
                    dato->co2 = co2;
                    dato->so2 = so2;
                    dato->no2 = no2;
                    dato->pm25 = pm25;
                    strncpy(dato->fecha, fecha, 11);
                    dato->fecha[10] = '\0';
                    
                    zonas[zona_idx].semanas[semana].numDias++;
                    importados++;
                    
                    // Mostrar progreso cada 10 registros
                    if (importados % 10 == 0) {
                        printf(ANSI_VERDE "Importados: %d registros\n" ANSI_RESET, importados);
                    }
                } else {
                    printf(ANSI_AMARILLO "Linea %d: Valores fuera de rango\n" ANSI_RESET, lineas_procesadas);
                }
            } else {
                printf(ANSI_AMARILLO "Linea %d: Zona '%s' no encontrada\n" ANSI_RESET, lineas_procesadas, zona);
            }
        }
    }
    
    fclose(f);
    
    if (importados > 0) {
        printf(ANSI_VERDE "\n¡EXITO! Se importaron %d registros\n" ANSI_RESET, importados);
        printf(ANSI_AMARILLO "Resumen:\n" ANSI_RESET);
        for (int i = 0; i < numZonas; i++) {
            int total = 0;
            for (int s = 0; s < zonas[i].numSemanas; s++) {
                total += zonas[i].semanas[s].numDias;
            }
            if (total > 0) {
                printf("  %s: %d registros\n", zonas[i].nombre, total);
            }
        }
    } else {
        printf(ANSI_ROJO "No se pudo importar ningun dato.\n" ANSI_RESET);
        printf(ANSI_AMARILLO "Verifique que el archivo tenga el formato correcto:\n");
        printf("Zona,Fecha,CO2,SO2,NO2,PM25\n");
        printf("Ejemplo: Quito,2024-07-01,400.0,5.0,25.0,15.0\n" ANSI_RESET);
    }
    
    return importados;
}

/**
 * Guarda la configuración de fechas en un archivo dedicado
 */
int guardarConfiguracionFechas(struct ConfiguracionFechas *config) {
    FILE *archivo = fopen("config_fechas.dat", "wb");
    
    if (!archivo) {
        return 0; // No se pudo crear el archivo
    }
    
    // Escribir la configuración de fechas
    fwrite(config, sizeof(struct ConfiguracionFechas), 1, archivo);
    fclose(archivo);
    
    printf(ANSI_VERDE "Configuracion de fechas guardada correctamente.\n" ANSI_RESET);
    return 1; // Guardado exitoso
}

/**
 * Carga la configuración de fechas desde un archivo dedicado
 */
int cargarConfiguracionFechas(struct ConfiguracionFechas *config) {
    FILE *archivo = fopen("config_fechas.dat", "rb");
    
    if (!archivo) {
        // Si no existe el archivo, usar configuración por defecto
        inicializarConfiguracionFechas(config);
        return 0; // Archivo no encontrado, usando valores por defecto
    }
    
    // Leer la configuración de fechas
    fread(config, sizeof(struct ConfiguracionFechas), 1, archivo);
    fclose(archivo);
    
    printf(ANSI_VERDE "Configuracion de fechas cargada: %04d-%02d-%02d\n" ANSI_RESET,
           config->anio_inicio, config->mes_inicio, config->dia_inicio);
    return 1; // Carga exitosa
}
