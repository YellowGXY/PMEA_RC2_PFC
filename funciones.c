#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#ifdef _WIN32
    #include <direct.h>
#else
    #include <sys/types.h>
    #include <unistd.h>
#endif
#include "funciones.h"

// Separador de rutas multiplataforma
#ifdef _WIN32
    #define PATH_SEPARATOR "\\"
#else
    #define PATH_SEPARATOR "/"
#endif

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

// Limites OMS para contaminantes
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
        zonas[indice_zona].numMeses = 0;
        
        // Inicializar todos los meses a cero
        for (int indice_mes = 0; indice_mes < MAX_MESES; indice_mes++) {
            zonas[indice_zona].meses[indice_mes].numDias = 0;
        }
        
        // Establecer umbrales por defecto basados en estandares internacionales
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
    // Configuracion por defecto: anio actual, enero 1
    config->anio_inicio = 2025;
    config->mes_inicio = 1;
    config->dia_inicio = 1;
    config->usar_fechas_automaticas = 1; // Por defecto usar fechas automaticas
}

/**
// Inicializa el sistema con datos NORMALES/SALUDABLES de demostracion
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
    
    // Calcular promedios dividiendo por el numero de zonas
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
 * NOTA: Esta funcion esta obsoleta y solo se mantiene para compatibilidad
 * Se recomienda usar las funciones de pronostico basadas en datos reales del menu de pronosticos
 */
void predecirContaminacion(struct Sistema *sistema, float prediccion[]) {
    printf(ANSI_AMARILLO "ADVERTENCIA: Esta funcion usa datos de ejemplo del sistema basico.\n" ANSI_RESET);
    printf(ANSI_AMARILLO "Para pronosticos con datos reales, use: Menu Principal > Reportes > Pronosticos\n" ANSI_RESET);
    
    float promedios[4];
    calcularPromedios(sistema, promedios);
    float pm_promedio = promedios[3]; // Promedio global de PM2.5
    
    // Calcular prediccion para cada zona usando modelo hibrido
    for (int indice_zona = 0; indice_zona < sistema->numZonas; indice_zona++) {
        // 70% valor actual + 30% promedio global para suavizar fluctuaciones
        prediccion[indice_zona] = sistema->zonas[indice_zona].pm25 * 0.7f + pm_promedio * 0.3f;
    }
    
    // Guardar predicciones en archivo binario para analisis posterior
    crearCarpetaSistema();
    char ruta_predicciones[64];
    snprintf(ruta_predicciones, sizeof(ruta_predicciones), "sistema_archivos%sdatos_pred.dat", PATH_SEPARATOR);
    FILE *archivo = fopen(ruta_predicciones, "wb");
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
        // Alerta por PM2.5 alto (limite OMS: 35 ug/m3)
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
    
    // Mostrar recomendaciones especificas para cada alerta
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
    printf("+----------------------------+--------+--------+--------+--------+\n");
    printf("| Zona                       |  CO2   |  SO2   |  NO2   | PM2.5  |\n");
    printf("+----------------------------+--------+--------+--------+--------+\n");
    // Datos de cada zona
    for (int indice_zona = 0; indice_zona < sistema->numZonas; indice_zona++) {
        printf("| %-26s | %6.2f | %6.2f | %6.2f | %6.1f |\n",
            sistema->zonas[indice_zona].nombre, sistema->zonas[indice_zona].co2, sistema->zonas[indice_zona].so2,
            sistema->zonas[indice_zona].no2, sistema->zonas[indice_zona].pm25);
    }
    // Pie de la tabla
    printf("+----------------------------+--------+--------+--------+--------+\n");
}

// ===============================
// FUNCIONES ROBUSTAS DE ENTRADA DE DATOS
// ===============================

/**
 * Limpia el buffer de entrada estandar
 */
void limpiarBufferEntrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/**
 * Valida que una cadena contenga un entero valido dentro del rango especificado
 * Retorna 1 si es valido, 0 si no
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
 * Valida que una cadena contenga un float valido dentro del rango especificado
 * Retorna 1 si es valido, 0 si no
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
 * Lee un entero de forma segura con validacion de rango
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
        if (len > 0 && len < (size_t)tamano_max) {
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
    char ruta_historial[64];
    snprintf(ruta_historial, sizeof(ruta_historial), "sistema_archivos%sdatos_hist.dat", PATH_SEPARATOR);
    FILE *archivo = fopen(ruta_historial, "rb");
    
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
    crearCarpetaSistema();
    char ruta_predicciones[64];
    snprintf(ruta_predicciones, sizeof(ruta_predicciones), "sistema_archivos%spredicciones.txt", PATH_SEPARATOR);
    FILE *archivo_predicciones = fopen(ruta_predicciones, "a");
    
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
    snprintf(mensaje, sizeof(mensaje), "Seleccione zona (0-%d): ", numero_zonas-1);
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
    
    umbrales->so2.min = leerFloatSeguro("SO2 minimo (ug/m3): ", 0.0, 1000.0);
    umbrales->so2.max = leerFloatSeguro("SO2 maximo (ug/m3): ", umbrales->so2.min, 1000.0);
    
    umbrales->no2.min = leerFloatSeguro("NO2 minimo (ug/m3): ", 0.0, 1000.0);
    umbrales->no2.max = leerFloatSeguro("NO2 maximo (ug/m3): ", umbrales->no2.min, 1000.0);
    
    umbrales->pm25.min = leerFloatSeguro("PM2.5 minimo (ug/m3): ", 0.0, 500.0);
    umbrales->pm25.max = leerFloatSeguro("PM2.5 maximo (ug/m3): ", umbrales->pm25.min, 500.0);
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
        zona->semanas[numero_semana].dias[indice_dia].so2 = leerFloatSeguro("SO2 (ug/m3): ", 0.0, 100.0);
        zona->semanas[numero_semana].dias[indice_dia].no2 = leerFloatSeguro("NO2 (ug/m3): ", 0.0, 300.0);
        zona->semanas[numero_semana].dias[indice_dia].pm25 = leerFloatSeguro("PM2.5 (ug/m3): ", 0.0, 500.0);
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
    snprintf(nombre_archivo, sizeof(nombre_archivo), "%s_semana%d.dat", zona->nombre, numero_semana+1);
    
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
 * Guarda los datos de un mes específico en un archivo checkpoint
 * Formato: mes_X/nombre_zona_mesX.dat
 */
void guardarMes(struct Zona *zona, int numero_mes) {
    char nombre_carpeta[32];
    char nombre_archivo[96];
    
    // Validar parametros de entrada
    if (zona == NULL || numero_mes < 0 || numero_mes >= MAX_MESES) {
        printf("Error: Parametros invalidos para guardar mes (zona=%p, mes=%d)\n", 
               (void*)zona, numero_mes);
        return;
    }
    
    // Crear nombre de carpeta: mes_1, mes_2, etc.
    snprintf(nombre_carpeta, sizeof(nombre_carpeta), "mes_%d", numero_mes + 1);
    
    // Crear carpeta usando funciones mas seguras
    #ifdef _WIN32
        if (_mkdir(nombre_carpeta) != 0) {
            // Verificar si la carpeta ya existe o si hay otro error
            struct stat st;
            if (stat(nombre_carpeta, &st) != 0) {
                printf("Error: No se pudo crear la carpeta %s\n", nombre_carpeta);
                return;
            }
        }
    #else
        if (mkdir(nombre_carpeta, 0755) != 0) {
            // Verificar si la carpeta ya existe o si hay otro error
            struct stat st;
            if (stat(nombre_carpeta, &st) != 0) {
                printf("Error: No se pudo crear la carpeta %s\n", nombre_carpeta);
                return;
            }
        }
    #endif
    
    // Crear ruta completa del archivo (usar separador correcto)
    #ifdef _WIN32
        snprintf(nombre_archivo, sizeof(nombre_archivo), "%s\\%s_mes%d.dat", 
                 nombre_carpeta, zona->nombre, numero_mes + 1);
    #else
        snprintf(nombre_archivo, sizeof(nombre_archivo), "%s/%s_mes%d.dat", 
                 nombre_carpeta, zona->nombre, numero_mes + 1);
    #endif
    
    // Verificar si hay datos para guardar
    if (zona->meses[numero_mes].numDias == 0) {
        printf("Advertencia: El mes %d de la zona %s no tiene datos para guardar\n", 
               numero_mes + 1, zona->nombre);
        // Aun asi proceder con el guardado para crear el archivo
    }
    
    FILE *archivo = fopen(nombre_archivo, "wb");
    if (archivo) {
        size_t bytes_escritos = fwrite(&zona->meses[numero_mes], sizeof(struct Mes), 1, archivo);
        fclose(archivo);
        
        if (bytes_escritos == 1) {
            printf("Mes %d guardado exitosamente en %s (dias: %d)\n", 
                   numero_mes + 1, nombre_archivo, zona->meses[numero_mes].numDias);
        } else {
            printf("Error: Fallo al escribir datos completos en %s\n", nombre_archivo);
        }
    } else {
        printf("Error: No se pudo abrir el archivo %s para escritura\n", nombre_archivo);
        printf("Verifique permisos de escritura en el directorio actual\n");
    }
}

/**
 * Carga los datos de una semana específica desde un archivo checkpoint


 */
void cargarSemana(struct Zona *zona, int numero_semana) {
    char nombre_archivo[64];
    snprintf(nombre_archivo, sizeof(nombre_archivo), "%s_semana%d.dat", zona->nombre, numero_semana+1);
    
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
 * Carga los datos de un mes específico desde un archivo checkpoint
 * Formato: mes_X/nombre_zona_mesX.dat
 */
void cargarMes(struct Zona *zona, int numero_mes) {
    char nombre_carpeta[32];
    char nombre_archivo[96];
    
    // Crear nombre de carpeta: mes_1, mes_2, etc.
    snprintf(nombre_carpeta, sizeof(nombre_carpeta), "mes_%d", numero_mes + 1);
    
    // Crear ruta completa del archivo (usar separador correcto)
    #ifdef _WIN32
        snprintf(nombre_archivo, sizeof(nombre_archivo), "%s\\%s_mes%d.dat", 
                 nombre_carpeta, zona->nombre, numero_mes + 1);
    #else
        snprintf(nombre_archivo, sizeof(nombre_archivo), "%s/%s_mes%d.dat", 
                 nombre_carpeta, zona->nombre, numero_mes + 1);
    #endif
    
    FILE *archivo = fopen(nombre_archivo, "rb");
    if (archivo) {
        fread(&zona->meses[numero_mes], sizeof(struct Mes), 1, archivo);
        fclose(archivo);
        printf("Mes %d cargado de %s\n", numero_mes + 1, nombre_archivo);
        
        // Actualizar número de meses si es necesario
        if (numero_mes >= zona->numMeses) {
            zona->numMeses = numero_mes + 1;
        }
    } else {
        printf("No existe checkpoint para el mes %d en %s\n", numero_mes + 1, nombre_archivo);
    }
}

/**
 * Guarda el estado actual de las semanas en un archivo
 */
void guardarSemanaActual(int semanaActual[], int numZonas) {
    crearCarpetaSistema();
    char ruta_semanas[64];
    snprintf(ruta_semanas, sizeof(ruta_semanas), "sistema_archivos%ssemanas_actuales.dat", PATH_SEPARATOR);
    FILE *archivo = fopen(ruta_semanas, "wb");
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
    char ruta_semanas[64];
    snprintf(ruta_semanas, sizeof(ruta_semanas), "sistema_archivos%ssemanas_actuales.dat", PATH_SEPARATOR);
    FILE *archivo = fopen(ruta_semanas, "rb");
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
 * Carga los datos de las zonas desde sus archivos de checkpoint organizados por mes
 */
void cargarDatosZonas(struct Zona zonas[], int numZonas) {
    for (int i = 0; i < numZonas; i++) {
        // Buscar el archivo de checkpoint más reciente para esta zona
        for (int mes = MAX_MESES - 1; mes >= 0; mes--) {
            char nombre_carpeta[32];
            char nombre_archivo[96];
            
            // Crear nombre de carpeta: mes_1, mes_2, etc.
            snprintf(nombre_carpeta, sizeof(nombre_carpeta), "mes_%d", mes + 1);
            
            // Crear ruta completa del archivo
            snprintf(nombre_archivo, sizeof(nombre_archivo), "%s\\%s_mes%d.dat", 
                     nombre_carpeta, zonas[i].nombre, mes + 1);
            
            FILE *archivo = fopen(nombre_archivo, "rb");
            if (archivo) {
                fread(&zonas[i].meses[mes], sizeof(struct Mes), 1, archivo);
                fclose(archivo);
                
                // Actualizar el número de meses de la zona
                if (mes >= zonas[i].numMeses) {
                    zonas[i].numMeses = mes + 1;
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
    
    config->anio_inicio = leerEnteroSeguro("Anio de inicio (2020-2030): ", 2020, 2030);
    config->mes_inicio = leerEnteroSeguro("Mes de inicio (1-12): ", 1, 12);
    config->dia_inicio = leerEnteroSeguro("Dia de inicio (1-31): ", 1, 31);
    config->usar_fechas_automaticas = leerEnteroSeguro("Usar fechas automaticas para nuevos registros? (1=Si, 0=No): ", 0, 1);
    
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
 * Calcula una fecha automatica basada en la configuracion y dias relativos
 */
void calcularFechaAutomatica(struct ConfiguracionFechas *config, int dia_relativo, char *fecha_resultado) {
    // Dias por mes (anio no bisiesto)
    int dias_mes[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    int anio = config->anio_inicio;
    int mes = config->mes_inicio;
    int dia = config->dia_inicio + dia_relativo;
    
    // Ajustar anio bisiesto
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
            // Recalcular anio bisiesto para el nuevo anio
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
    
    zona->semanas[semana].numDias = 0; // Reiniciar contador de dias
    
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
 * Permite ingresar datos manualmente para un mes específico con fechas automáticas
 */
void ingresarDatosManualMes(struct Zona *zona, int numero_mes, struct ConfiguracionFechas *config) {
    if (numero_mes >= MAX_MESES) {
        printf("Mes invalido. Debe estar entre 0 y %d.\n", MAX_MESES-1);
        return;
    }
    
    printf("\n+---------------------------------------------------------------+\n");
    printf("| INGRESO MANUAL DE DATOS - ZONA: %-28s |\n", zona->nombre);
    printf("| MES: %-3d                                                     |\n", numero_mes+1);
    printf("+---------------------------------------------------------------+\n");
    printf("| Instrucciones:                                                |\n");
    printf("| - Ingrese solo numeros (sin acentos ni caracteres especiales)|\n");
    printf("| - Valores fuera de rango seran rechazados                    |\n");
    printf("| - Escriba 'c' para cancelar en cualquier momento             |\n");
    printf("+---------------------------------------------------------------+\n\n");
    
    // Determinar numero de dias del mes
    int dias_mes[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int max_dias = dias_mes[numero_mes % 12];
    
    // Ajustar anio bisiesto si es febrero
    if (numero_mes % 12 == 1) { // Febrero
        int anio = config->anio_inicio + (numero_mes / 12);
        if ((anio % 4 == 0 && anio % 100 != 0) || (anio % 400 == 0)) {
            max_dias = 29;
        }
    }
    
    zona->meses[numero_mes].numDias = 0; // Reiniciar contador de dias
    
    for (int d = 0; d < max_dias; d++) {
        printf("--- DIA %d ---\n", d+1);
        
        // Manejo de fechas automáticas
        if (config->usar_fechas_automaticas) {
            int dia_relativo = (numero_mes * 30) + d; // Aproximación para cálculo
            calcularFechaAutomatica(config, dia_relativo, zona->meses[numero_mes].dias[d].fecha);
            printf("Fecha automatica: %s\n", zona->meses[numero_mes].dias[d].fecha);
        } else {
            char fecha_temp[12];
            leerCadenaSegura("Fecha (YYYY-MM-DD): ", fecha_temp, 11);
            
            if (fecha_temp[0] == 'c' || fecha_temp[0] == 'C') {
                printf("Ingreso cancelado.\n");
                return;
            }
            
            strncpy(zona->meses[numero_mes].dias[d].fecha, fecha_temp, 11);
            zona->meses[numero_mes].dias[d].fecha[10] = '\0';
        }
        
        // Ingreso de contaminantes con validación
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
                zona->meses[numero_mes].dias[d].co2 = valor;
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
                zona->meses[numero_mes].dias[d].so2 = valor;
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
                zona->meses[numero_mes].dias[d].no2 = valor;
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
                zona->meses[numero_mes].dias[d].pm25 = valor;
                break;
            } else {
                printf("Error: Valor invalido. Debe estar entre 0.0 y 500.0\n");
            }
        }
        
        zona->meses[numero_mes].numDias = d + 1;
        printf("Dia %d registrado correctamente.\n\n", d+1);
    }
    
    // Actualizar el número de meses si es necesario
    if (numero_mes >= zona->numMeses) {
        zona->numMeses = numero_mes + 1;
    }
    
    printf("Mes %d completado para la zona %s.\n", numero_mes+1, zona->nombre);
    
    // Guardar automáticamente el mes completado
    guardarMes(zona, numero_mes);
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
    
    // Instrucciones para carga masiva - igual que el archivo datos
    fprintf(f, "PLANTILLA PARA CARGA MASIVA DE 1 MES COMPLETO\n");
    fprintf(f, "=============================================\n\n");
    fprintf(f, "Formato: Zona,Fecha,CO2,SO2,NO2,PM25\n");
    fprintf(f, "- Zona: Quito, Cuenca, Guayaquil, Loja, Ambato\n");
    fprintf(f, "- Fecha: YYYY-MM-DD (fechas consecutivas para formar un mes)\n");
    fprintf(f, "- CO2: 0.0-1.0 (ppm)\n");
    fprintf(f, "- SO2: 0.0-100.0 (ug/m³)\n");
    fprintf(f, "- NO2: 0.0-300.0 (ug/m³)\n");
    fprintf(f, "- PM25: 0.0-500.0 (ug/m³)\n\n");
    
    fprintf(f, "INSTRUCCIONES:\n");
    fprintf(f, "1. Incluya datos para todas las zonas\n");
    fprintf(f, "2. Use fechas consecutivas para formar un mes completo\n");
    fprintf(f, "3. El sistema detectara automaticamente el mes y organizara los datos\n");
    fprintf(f, "4. Puede incluir datos parciales (no es necesario completar 30-31 dias)\n\n");
    
    // Datos de ejemplo exactos del archivo datos
    fprintf(f, "# Ejemplo de datos para ENERO 2024 - Reemplazar con valores reales\n");
    fprintf(f, "Quito,2024-01-01,0.040,8.0,25.0,15.0\n");
    fprintf(f, "Quito,2024-01-02,0.042,8.5,26.0,16.0\n");
    fprintf(f, "Quito,2024-01-03,0.038,7.5,24.0,14.0\n");
    fprintf(f, "Quito,2024-01-04,0.041,8.2,25.5,15.5\n");
    fprintf(f, "Quito,2024-01-05,0.039,7.8,24.5,15.0\n\n");
    
    fprintf(f, "Cuenca,2024-01-01,0.042,9.0,27.0,18.0\n");
    fprintf(f, "Cuenca,2024-01-02,0.044,9.5,28.0,19.0\n");
    fprintf(f, "Cuenca,2024-01-03,0.040,8.5,26.0,17.0\n");
    fprintf(f, "Cuenca,2024-01-04,0.043,9.2,27.5,18.5\n");
    fprintf(f, "Cuenca,2024-01-05,0.041,8.8,26.5,17.5\n\n");
    
    fprintf(f, "Guayaquil,2024-01-01,0.045,12.0,30.0,22.0\n");
    fprintf(f, "Guayaquil,2024-01-02,0.047,12.5,31.0,23.0\n");
    fprintf(f, "Guayaquil,2024-01-03,0.043,11.5,29.0,21.0\n");
    fprintf(f, "Guayaquil,2024-01-04,0.046,12.2,30.5,22.5\n");
    fprintf(f, "Guayaquil,2024-01-05,0.044,11.8,29.5,21.5\n\n");
    
    fprintf(f, "Loja,2024-01-01,0.037,7.0,23.0,13.0\n");
    fprintf(f, "Loja,2024-01-02,0.039,7.5,24.0,14.0\n");
    fprintf(f, "Loja,2024-01-03,0.035,6.5,22.0,12.0\n");
    fprintf(f, "Loja,2024-01-04,0.038,7.2,23.5,13.5\n");
    fprintf(f, "Loja,2024-01-05,0.036,6.8,22.5,12.5\n\n");
    
    fprintf(f, "Ambato,2024-01-01,0.041,8.5,26.0,16.0\n");
    fprintf(f, "Ambato,2024-01-02,0.043,9.0,27.0,17.0\n");
    fprintf(f, "Ambato,2024-01-03,0.039,8.0,25.0,15.0\n");
    fprintf(f, "Ambato,2024-01-04,0.042,8.7,26.5,16.5\n");
    fprintf(f, "Ambato,2024-01-05,0.040,8.2,25.5,15.5\n\n");
    
    fprintf(f, "# Continuar con mas dias segun sea necesario...\n");
    fprintf(f, "# Para un mes completo, incluir hasta el dia 28, 29, 30 o 31 segun corresponda\n");
    
    fclose(f);
    printf(ANSI_VERDE "Plantilla para carga masiva creada: %s\n" ANSI_RESET, nombreArchivo);
    printf(ANSI_CIAN "La plantilla incluye ejemplos para 5 dias de todas las zonas\n" ANSI_RESET);
    printf(ANSI_AMARILLO "Edite el archivo y agregue mas dias segun necesite\n" ANSI_RESET);
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
    int errores = 0;
    int duplicados = 0;
    int meses_detectados[MAX_MESES] = {0}; // Para rastrear qué meses se están cargando
    // int dias_faltantes[MAX_ZONAS][MAX_MESES] = {0}; // Para rastrear dias faltantes por zona y mes (no usado actualmente)
    
    printf(ANSI_AMARILLO "Importando datos desde %s...\n" ANSI_RESET, nombreArchivo);
    printf(ANSI_CIAN "Procesando carga masiva de datos mensuales...\n" ANSI_RESET);
    
    while (fgets(linea, sizeof(linea), f)) {
        lineas_procesadas++;
        
        // Saltar lineas de comentario y lineas vacias
        if (linea[0] == '#' || linea[0] == '\n' || linea[0] == '\r') {
            continue;
        }
        
        // Parsear linea manualmente
        char zona[32], fecha[12];
        float co2, so2, no2, pm25;
        
        // Buscar las comas y extraer cada campo
        char *ptr = linea;
        char *coma;
        int leidos = 0;
        
        // Extraer zona
        coma = strchr(ptr, ',');
        if (coma) {
            int len = coma - ptr;
            if (len > 31) len = 31;
            strncpy(zona, ptr, len);
            zona[len] = '\0';
            ptr = coma + 1;
            leidos++;
            
            // Extraer fecha
            coma = strchr(ptr, ',');
            if (coma) {
                len = coma - ptr;
                if (len > 11) len = 11;
                strncpy(fecha, ptr, len);
                fecha[len] = '\0';
                ptr = coma + 1;
                leidos++;
                
                // Extraer los 4 valores numericos
                if (sscanf(ptr, "%f,%f,%f,%f", &co2, &so2, &no2, &pm25) == 4) {
                    leidos = 6;
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
                // Validar rangos
                if (co2 >= 0 && co2 <= 1000 && so2 >= 0 && so2 <= 100 && 
                    no2 >= 0 && no2 <= 300 && pm25 >= 0 && pm25 <= 500) {
                    
                    // Extraer mes y dia de la fecha
                    int anio, mes, dia;
                    if (sscanf(fecha, "%d-%d-%d", &anio, &mes, &dia) == 3) {
                        mes--; // Convertir a indice (0-11)
                        dia--; // Convertir a indice (0-30)
                        
                        if (mes >= 0 && mes < MAX_MESES && dia >= 0 && dia < MAX_DIAS_MES) {
                            // Verificar si el dato ya existe (evitar duplicados)
                            struct DatosAmbientales *dato_existente = &zonas[zona_idx].meses[mes].dias[dia];
                            if (strlen(dato_existente->fecha) > 0) {
                                duplicados++;
                                printf(ANSI_AMARILLO "Duplicado encontrado: %s en %s (%s)\n" ANSI_RESET, 
                                       zona, fecha, "dato sobrescrito");
                            }
                            
                            // Marcar que este mes se está cargando
                            meses_detectados[mes] = 1;
                            
                            // Asegurar que el mes existe
                            if (mes >= zonas[zona_idx].numMeses) {
                                zonas[zona_idx].numMeses = mes + 1;
                            }
                            
                            // Agregar el dato
                            struct DatosAmbientales *dato = &zonas[zona_idx].meses[mes].dias[dia];
                            dato->co2 = co2;
                            dato->so2 = so2;
                            dato->no2 = no2;
                            dato->pm25 = pm25;
                            strncpy(dato->fecha, fecha, 11);
                            dato->fecha[10] = '\0';
                            
                            if (dia >= zonas[zona_idx].meses[mes].numDias) {
                                zonas[zona_idx].meses[mes].numDias = dia + 1;
                            }
                            
                            importados++;
                            
                            if (importados % 50 == 0) {
                                printf(ANSI_VERDE "Importados: %d registros\n" ANSI_RESET, importados);
                            }
                        }
                    }
                } else {
                    errores++;
                    printf(ANSI_AMARILLO "Linea %d: Valores fuera de rango para %s (CO2:%.2f SO2:%.2f NO2:%.2f PM25:%.2f)\n" ANSI_RESET, 
                           lineas_procesadas, zona, co2, so2, no2, pm25);
                }
            } else {
                errores++;
                printf(ANSI_AMARILLO "Linea %d: Zona '%s' no encontrada\n" ANSI_RESET, lineas_procesadas, zona);
            }
        } else {
            errores++;
            printf(ANSI_AMARILLO "Linea %d: Formato incorrecto\n" ANSI_RESET, lineas_procesadas);
        }
    }
    
    fclose(f);
    
    // Analisis de dias faltantes
    printf(ANSI_CIAN "\nAnalizando integridad de datos...\n" ANSI_RESET);
    for (int mes = 0; mes < MAX_MESES; mes++) {
        if (meses_detectados[mes]) {
            printf(ANSI_AMARILLO "Mes %d:\n" ANSI_RESET, mes + 1);
            for (int zona_idx = 0; zona_idx < numZonas; zona_idx++) {
                int dias_con_datos = 0;
                for (int dia = 0; dia < zonas[zona_idx].meses[mes].numDias; dia++) {
                    if (strlen(zonas[zona_idx].meses[mes].dias[dia].fecha) > 0) {
                        dias_con_datos++;
                    }
                }
                printf("  %s: %d dias con datos\n", zonas[zona_idx].nombre, dias_con_datos);
            }
        }
    }
    
    if (importados > 0) {
        printf(ANSI_VERDE "\n=== RESUMEN DE IMPORTACION ===\n" ANSI_RESET);
        printf(ANSI_VERDE "Registros importados: %d\n" ANSI_RESET, importados);
        printf(ANSI_AMARILLO "Errores encontrados: %d\n" ANSI_RESET, errores);
        printf(ANSI_AMARILLO "Duplicados sobrescritos: %d\n" ANSI_RESET, duplicados);
        printf(ANSI_AMARILLO "Lineas procesadas: %d\n" ANSI_RESET, lineas_procesadas);
        
        // Mostrar resumen de meses cargados
        printf(ANSI_CIAN "Meses cargados: ");
        for (int i = 0; i < MAX_MESES; i++) {
            if (meses_detectados[i]) {
                printf("%d ", i + 1);
            }
        }
        printf("\n" ANSI_RESET);
        
        // CREAR MESES VACIOS AUTOMATICAMENTE
        int mes_minimo = -1, mes_maximo = -1;
        for (int i = 0; i < MAX_MESES; i++) {
            if (meses_detectados[i]) {
                if (mes_minimo == -1) mes_minimo = i;
                mes_maximo = i;
            }
        }
        
        if (mes_minimo != -1 && mes_minimo > 0) {
            printf(ANSI_AMARILLO "Detectados datos desde mes %d. Creando meses vacios del 1 al %d...\n" ANSI_RESET, 
                   mes_minimo + 1, mes_minimo);
            
            for (int zona_idx = 0; zona_idx < numZonas; zona_idx++) {
                for (int mes = 0; mes < mes_minimo; mes++) {
                    // Inicializar mes vacio si no existe
                    if (zonas[zona_idx].meses[mes].numDias == 0) {
                        zonas[zona_idx].meses[mes].numDias = 0;
                        for (int d = 0; d < MAX_DIAS_MES; d++) {
                            zonas[zona_idx].meses[mes].dias[d].co2 = 0;
                            zonas[zona_idx].meses[mes].dias[d].so2 = 0;
                            zonas[zona_idx].meses[mes].dias[d].no2 = 0;
                            zonas[zona_idx].meses[mes].dias[d].pm25 = 0;
                            strcpy(zonas[zona_idx].meses[mes].dias[d].fecha, "");
                        }
                        printf(ANSI_VERDE "  Mes %d vacio creado para zona %s\n" ANSI_RESET, 
                               mes + 1, zonas[zona_idx].nombre);
                    }
                }
                // Actualizar numMeses para incluir los meses vacios
                if (zonas[zona_idx].numMeses < mes_maximo + 1) {
                    zonas[zona_idx].numMeses = mes_maximo + 1;
                }
            }
        }
        
        // Validar automáticamente los datos importados
        int problemas = validarDatosImportados(zonas, numZonas, meses_detectados);
        
        // Manejar dias faltantes
        manejarDiasFaltantes(zonas, numZonas, meses_detectados);
        
        // Generar reporte completo de resumen
        generarResumenImportacion(zonas, numZonas, meses_detectados, importados, errores, duplicados);
        
        // Guardar automaticamente los meses cargados
        for (int zona_idx = 0; zona_idx < numZonas; zona_idx++) {
            for (int mes = 0; mes < MAX_MESES; mes++) {
                if (meses_detectados[mes] && zonas[zona_idx].meses[mes].numDias > 0) {
                    guardarMes(&zonas[zona_idx], mes);
                }
            }
        }
        
        // Usar lógica inteligente para determinar el mes actual
        int mes_sistema_inteligente = determinarMesActualInteligente(zonas, numZonas, meses_detectados);
        
        // Actualizar el mes actual del sistema
        int mes_actual[MAX_ZONAS];
        for (int i = 0; i < numZonas; i++) {
            mes_actual[i] = mes_sistema_inteligente;
        }
        guardarMesActual(mes_actual, numZonas);
        
        printf(ANSI_VERDE "Sistema configurado inteligentemente al mes %d\n" ANSI_RESET, 
               mes_sistema_inteligente + 1);
        
        // Actualizar fecha del sistema según los datos importados
        actualizarFechaSegunDatos(zonas, numZonas);
        
        printf(ANSI_VERDE "Datos guardados automaticamente en carpetas por mes\n" ANSI_RESET);
        
        // Mensaje final basado en la calidad de los datos
        if (problemas == 0) {
            printf(ANSI_VERDE "\nIMPORTACION COMPLETADA EXITOSAMENTE\n" ANSI_RESET);
        } else {
            printf(ANSI_AMARILLO "\nIMPORTACION COMPLETADA CON ADVERTENCIAS\n" ANSI_RESET);
        }
    } else {
        printf(ANSI_ROJO "No se pudo importar ningun dato.\n" ANSI_RESET);
        printf(ANSI_AMARILLO "Verifique el formato:\n");
        printf("  - Zona,Fecha,CO2,SO2,NO2,PM25\n");
        printf("  - Fecha: YYYY-MM-DD\n");
        printf("  - Sin espacios extra\n");
        printf("  - Valores numericos validos\n" ANSI_RESET);
    }
    
    return importados;
}

/**
 * Función para detectar la fecha más reciente en los datos importados
 */
void actualizarFechaSegunDatos(struct Zona zonas[], int numZonas) {
    extern struct ConfiguracionFechas config_fechas;
    
    int anio_mas_reciente = 0;
    int mes_mas_reciente = 0;
    int dia_mas_reciente = 0;
    
    // Buscar la fecha más reciente en todos los datos
    for (int zona_idx = 0; zona_idx < numZonas; zona_idx++) {
        for (int mes = 0; mes < zonas[zona_idx].numMeses; mes++) {
            for (int dia = 0; dia < zonas[zona_idx].meses[mes].numDias; dia++) {
                struct DatosAmbientales *dato = &zonas[zona_idx].meses[mes].dias[dia];
                
                // Verificar si hay fecha válida
                if (strlen(dato->fecha) > 8) {
                    int anio, mes_fecha, dia_fecha;
                    if (sscanf(dato->fecha, "%d-%d-%d", &anio, &mes_fecha, &dia_fecha) == 3) {
                        // Comparar fechas para encontrar la más reciente
                        int fecha_actual = anio * 10000 + mes_fecha * 100 + dia_fecha;
                        int fecha_mas_reciente = anio_mas_reciente * 10000 + mes_mas_reciente * 100 + dia_mas_reciente;
                        
                        if (fecha_actual > fecha_mas_reciente) {
                            anio_mas_reciente = anio;
                            mes_mas_reciente = mes_fecha;
                            dia_mas_reciente = dia_fecha;
                        }
                    }
                }
            }
        }
    }
    
    // Actualizar configuración si se encontró una fecha válida
    if (anio_mas_reciente > 0) {
        config_fechas.anio_inicio = anio_mas_reciente;
        config_fechas.mes_inicio = mes_mas_reciente;
        config_fechas.dia_inicio = dia_mas_reciente;
        
        guardarConfiguracionFechas(&config_fechas);
        
        printf(ANSI_VERDE "Fecha del sistema actualizada a: %02d/%02d/%d\n" ANSI_RESET,
               dia_mas_reciente, mes_mas_reciente, anio_mas_reciente);
    }
}

/**
 * Devuelve el color ANSI según el nivel de peligrosidad del contaminante
 */
const char* obtenerColorContaminante(float valor, int tipo_contaminante) {
    switch (tipo_contaminante) {
        case TIPO_CO2:
            if (valor <= 400) return ANSI_VERDE;
            else if (valor <= 600) return ANSI_AMARILLO;
            else if (valor <= 1000) return ANSI_MAGENTA;
            else return ANSI_ROJO;
            
        case TIPO_SO2:
            if (valor <= 20) return ANSI_VERDE;
            else if (valor <= 40) return ANSI_AMARILLO;
            else if (valor <= 60) return ANSI_MAGENTA;
            else return ANSI_ROJO;
            
        case TIPO_NO2:
            if (valor <= 53) return ANSI_VERDE;
            else if (valor <= 100) return ANSI_AMARILLO;
            else if (valor <= 200) return ANSI_MAGENTA;
            else return ANSI_ROJO;
            
        case TIPO_PM25:
            if (valor <= 12) return ANSI_VERDE;
            else if (valor <= 35) return ANSI_AMARILLO;
            else if (valor <= 55) return ANSI_MAGENTA;
            else return ANSI_ROJO;
            
        default:
            return ANSI_RESET;
    }
}

/**
 * Devuelve una descripción del nivel de peligrosidad
 */
char* obtenerNivelPeligrosidad(float valor, int tipo_contaminante) {
    switch (tipo_contaminante) {
        case TIPO_CO2:
            if (valor <= 400) return "Bueno";
            else if (valor <= 600) return "Moderado";
            else if (valor <= 1000) return "Alto";
            else return "Peligroso";

        case TIPO_SO2:
            if (valor <= 20) return "Bueno";
            else if (valor <= 40) return "Moderado";
            else if (valor <= 60) return "Alto";
            else return "Peligroso";
            
        case TIPO_NO2:
            if (valor <= 53) return "Bueno";
            else if (valor <= 100) return "Moderado";
            else if (valor <= 200) return "Alto";
            else return "Peligroso";
            
        case TIPO_PM25:
            if (valor <= 12) return "Bueno";
            else if (valor <= 35) return "Moderado";
            else if (valor <= 55) return "Alto";
            else return "Peligroso";
            
        default:
            return "Desconocido";
    }
}

/**
 * Crea la carpeta para archivos del sistema si no existe
 * Multiplataforma: funciona en Windows, Linux y macOS
 */
void crearCarpetaSistema() {
    #ifdef _WIN32
        if (_mkdir("sistema_archivos") != 0) {
            // Verificar si la carpeta ya existe
            struct stat st;
            if (stat("sistema_archivos", &st) != 0) {
                printf("Error: No se pudo crear la carpeta sistema_archivos\n");
                printf("Verifique permisos de escritura en el directorio actual\n");
            }
        }
    #else
        if (mkdir("sistema_archivos", 0755) != 0) {
            // Verificar si la carpeta ya existe
            struct stat st;
            if (stat("sistema_archivos", &st) != 0) {
                printf("Error: No se pudo crear la carpeta sistema_archivos\n");
                printf("Verifique permisos de escritura en el directorio actual\n");
            }
        }
    #endif
}

// Función para ajustar el mes actual según la fecha del sistema
void ajustarMesActualSegunFecha(int mesActual[], int numZonas) {
    extern struct ConfiguracionFechas config_fechas;
    
    // Obtener fecha actual del sistema (8 de julio de 2025)
    int dia_actual = 8;
    int mes_actual = 7;  // Julio
    int anio_actual = 2025;
    
    // Calcular el mes que debería estar activo
    int mes_objetivo = mes_actual - 1; // Mes anterior al actual (junio = 6)
    if (mes_objetivo < 1) {
        mes_objetivo = 12;
    }
    mes_objetivo--; // Convertir a índice (0-11)
    
    // Si los datos son muy antiguos (enero 2024 vs julio 2025), ajustar al mes anterior
    if (config_fechas.anio_inicio < anio_actual || 
        (config_fechas.anio_inicio == anio_actual && config_fechas.mes_inicio < mes_actual)) {
        
        // Actualizar todos los meses actuales al mes objetivo
        for (int i = 0; i < numZonas; i++) {
            mesActual[i] = mes_objetivo;
        }
        
        // Actualizar la configuración del sistema
        config_fechas.mes_inicio = mes_objetivo + 1;
        config_fechas.anio_inicio = anio_actual;
        config_fechas.dia_inicio = dia_actual;
        
        guardarConfiguracionFechas(&config_fechas);
        guardarMesActual(mesActual, numZonas);
        
        printf(ANSI_CIAN "Mes ajustado automaticamente a: %d (mes anterior al actual)\n" ANSI_RESET, mes_objetivo + 1);
    }
}

// Funcion para inicializar el mes actual con datos basicos si esta vacio
void inicializarMesActualSiVacio(struct Zona zonas[], int numZonas, int mesActual[]) {
    for (int i = 0; i < numZonas; i++) {
        int mes = mesActual[i];
        
        // Verificar si el mes actual esta vacio
        if (zonas[i].meses[mes].numDias == 0) {
            printf("Inicializando mes %d para zona %s con datos basicos...\n", 
                   mes + 1, zonas[i].nombre);
            
            // Agregar un dia inicial con datos basicos/saludables
            zonas[i].meses[mes].numDias = 1;
            
            // Crear fecha inicial
            extern struct ConfiguracionFechas config_fechas;
            snprintf(zonas[i].meses[mes].dias[0].fecha, sizeof(zonas[i].meses[mes].dias[0].fecha),
                     "%04d-%02d-%02d", config_fechas.anio_inicio, config_fechas.mes_inicio, config_fechas.dia_inicio);
            
            // Datos basicos saludables por defecto
            zonas[i].meses[mes].dias[0].co2 = 380.0f;   // Nivel normal
            zonas[i].meses[mes].dias[0].so2 = 5.0f;     // Nivel bajo
            zonas[i].meses[mes].dias[0].no2 = 20.0f;    // Nivel normal
            zonas[i].meses[mes].dias[0].pm25 = 8.0f;    // Nivel saludable
            
            // Actualizar numMeses si es necesario
            if (mes >= zonas[i].numMeses) {
                zonas[i].numMeses = mes + 1;
            }
        }
    }
}

/**
 * Diagnostica problemas comunes en la configuración y sugiere soluciones
 */
void diagnosticarConfiguracion() {
    extern struct ConfiguracionFechas config_fechas;
    
    printf("=== DIAGNOSTICO DE CONFIGURACION ===\n");
    
    // Verificar año de inicio
    if (config_fechas.anio_inicio < 2020 || config_fechas.anio_inicio > 2030) {
        printf(ANSI_ROJO "Error: El año de inicio debe estar entre 2020 y 2030\n" ANSI_RESET);
        printf("Solucion: Ajuste el año de inicio en la configuracion\n");
    } else {
        printf(ANSI_VERDE "Año de inicio: %d (OK)\n" ANSI_RESET, config_fechas.anio_inicio);
    }
    
    // Verificar mes de inicio
    if (config_fechas.mes_inicio < 1 || config_fechas.mes_inicio > 12) {
        printf(ANSI_ROJO "Error: El mes de inicio debe estar entre 1 y 12\n" ANSI_RESET);
        printf("Solucion: Ajuste el mes de inicio en la configuracion\n");
    } else {
        printf(ANSI_VERDE "Mes de inicio: %d (OK)\n" ANSI_RESET, config_fechas.mes_inicio);
    }
    
    // Verificar dia de inicio
    if (config_fechas.dia_inicio < 1 || config_fechas.dia_inicio > 31) {
        printf(ANSI_ROJO "Error: El dia de inicio debe estar entre 1 y 31\n" ANSI_RESET);
        printf("Solucion: Ajuste el dia de inicio en la configuracion\n");
    } else {
        printf(ANSI_VERDE "Dia de inicio: %d (OK)\n" ANSI_RESET, config_fechas.dia_inicio);
    }
    
    // Verificar uso de fechas automaticas
    if (config_fechas.usar_fechas_automaticas != 0 && config_fechas.usar_fechas_automaticas != 1) {
        printf(ANSI_ROJO "Error: La opcion de fechas automaticas es invalida (debe ser 0 o 1)\n" ANSI_RESET);
        printf("Solucion: Ajuste la opcion de fechas automaticas en la configuracion\n");
    } else {
        char fecha_automatica[16];
        if (config_fechas.usar_fechas_automaticas) {
            strcpy(fecha_automatica, "Si");
        } else {
            strcpy(fecha_automatica, "No");
        }
        printf(ANSI_VERDE "Uso de fechas automaticas: %s (OK)\n", fecha_automatica);
    }
    
    printf("=====================================\n");
}

/**
 * Función para validar datos importados
 */
int validarDatosImportados(struct Zona zonas[], int numZonas, int meses_detectados[]) {
    int problemas = 0;
    
    printf(ANSI_CIAN "Validando datos importados...\n" ANSI_RESET);
    
    for (int i = 0; i < numZonas; i++) {
        for (int mes = 0; mes < MAX_MESES; mes++) {
            if (meses_detectados[mes] && zonas[i].meses[mes].numDias > 0) {
                // Verificar consistencia de datos
                for (int dia = 0; dia < zonas[i].meses[mes].numDias; dia++) {
                    struct DatosAmbientales *datos = &zonas[i].meses[mes].dias[dia];
                    
                    // Validar rangos de contaminantes
                    if (datos->co2 < 0 || datos->co2 > 1000) {
                        printf(ANSI_AMARILLO "Advertencia: CO2 fuera de rango en %s, mes %d, día %d: %.2f\n" ANSI_RESET,
                               zonas[i].nombre, mes+1, dia+1, datos->co2);
                        problemas++;
                    }
                    
                    if (datos->so2 < 0 || datos->so2 > 100) {
                        printf(ANSI_AMARILLO "Advertencia: SO2 fuera de rango en %s, mes %d, día %d: %.2f\n" ANSI_RESET,
                               zonas[i].nombre, mes+1, dia+1, datos->so2);
                        problemas++;
                    }
                    
                    if (datos->no2 < 0 || datos->no2 > 200) {
                        printf(ANSI_AMARILLO "Advertencia: NO2 fuera de rango en %s, mes %d, día %d: %.2f\n" ANSI_RESET,
                               zonas[i].nombre, mes+1, dia+1, datos->no2);
                        problemas++;
                    }
                    
                    if (datos->pm25 < 0 || datos->pm25 > 500) {
                        printf(ANSI_AMARILLO "Advertencia: PM2.5 fuera de rango en %s, mes %d, día %d: %.2f\n" ANSI_RESET,
                               zonas[i].nombre, mes+1, dia+1, datos->pm25);
                        problemas++;
                    }
                    
                    // Verificar que la fecha no esté vacía
                    if (strlen(datos->fecha) == 0) {
                        printf(ANSI_AMARILLO "Advertencia: Fecha vacía en %s, mes %d, día %d\n" ANSI_RESET,
                               zonas[i].nombre, mes+1, dia+1);
                        problemas++;
                    }
                }
            }
        }
    }
    
    if (problemas == 0) {
        printf(ANSI_VERDE "Validación completada: No se encontraron problemas\n" ANSI_RESET);
    } else {
        printf(ANSI_AMARILLO "Validación completada: Se encontraron %d problemas\n" ANSI_RESET, problemas);
    }
    
    return problemas;
}

/**
 * Funcion para manejar dias faltantes
 */
void manejarDiasFaltantes(struct Zona zonas[], int numZonas, int meses_detectados[]) {
    printf(ANSI_CIAN "Analizando dias faltantes en los datos...\n" ANSI_RESET);
    
    int total_dias_faltantes = 0;
    
    for (int i = 0; i < numZonas; i++) {
        for (int mes = 0; mes < MAX_MESES; mes++) {
            if (meses_detectados[mes] && zonas[i].meses[mes].numDias > 0) {
                // Calcular dias esperados en el mes (simplificado: 30 dias por mes)
                int dias_esperados = 30;
                int dias_reales = zonas[i].meses[mes].numDias;
                
                if (dias_reales < dias_esperados) {
                    int dias_faltantes = dias_esperados - dias_reales;
                    total_dias_faltantes += dias_faltantes;
                    
                    printf(ANSI_AMARILLO "Zona %s, mes %d: %d dias faltantes (tiene %d de %d)\n" ANSI_RESET,
                           zonas[i].nombre, mes+1, dias_faltantes, dias_reales, dias_esperados);
                    
                    // Ofrecer completar con datos promedio
                    if (dias_reales > 0) {
                        // Calcular promedios de los dias existentes
                        float sum_co2 = 0, sum_so2 = 0, sum_no2 = 0, sum_pm25 = 0;
                        for (int d = 0; d < dias_reales; d++) {
                            sum_co2 += zonas[i].meses[mes].dias[d].co2;
                            sum_so2 += zonas[i].meses[mes].dias[d].so2;
                            sum_no2 += zonas[i].meses[mes].dias[d].no2;
                            sum_pm25 += zonas[i].meses[mes].dias[d].pm25;
                        }
                        
                        float avg_co2 = sum_co2 / dias_reales;
                        float avg_so2 = sum_so2 / dias_reales;
                        float avg_no2 = sum_no2 / dias_reales;
                        float avg_pm25 = sum_pm25 / dias_reales;
                        
                        printf(ANSI_CIAN "  Promedios calculados: CO2=%.2f, SO2=%.2f, NO2=%.2f, PM2.5=%.2f\n" ANSI_RESET,
                               avg_co2, avg_so2, avg_no2, avg_pm25);
                    }
                }
            }
        }
    }
    
    if (total_dias_faltantes == 0) {
        printf(ANSI_VERDE "Analisis completado: No se encontraron dias faltantes\n" ANSI_RESET);
    } else {
        printf(ANSI_AMARILLO "Analisis completado: %d dias faltantes en total\n" ANSI_RESET, total_dias_faltantes);
    }
}

/**
 * Función para generar resumen de importación
 */
void generarResumenImportacion(struct Zona zonas[], int numZonas, int meses_detectados[], 
                              int total_importados, int total_errores, int total_duplicados) {
    printf(ANSI_NEGRITA "\n+=========================================+\n");
    printf("|     RESUMEN DE IMPORTACIÓN DE DATOS     |\n");
    printf("+=========================================+\n" ANSI_RESET);
    
    printf(ANSI_CIAN "Estadisticas generales:\n" ANSI_RESET);
    
    char color_importados[16];
    if (total_importados > 0) {
        strcpy(color_importados, ANSI_VERDE);
    } else {
        strcpy(color_importados, ANSI_ROJO);
    }
    printf("  • Total de registros importados: %s%d%s\n", 
           color_importados, total_importados, ANSI_RESET);
    
    char color_errores[16];
    if (total_errores > 0) {
        strcpy(color_errores, ANSI_ROJO);
    } else {
        strcpy(color_errores, ANSI_VERDE);
    }
    printf("  • Total de errores encontrados: %s%d%s\n", 
           color_errores, total_errores, ANSI_RESET);
    
    char color_duplicados[16];
    if (total_duplicados > 0) {
        strcpy(color_duplicados, ANSI_AMARILLO);
    } else {
        strcpy(color_duplicados, ANSI_VERDE);
    }
    printf("  • Total de duplicados evitados: %s%d%s\n", 
           color_duplicados, total_duplicados, ANSI_RESET);
    
    printf(ANSI_CIAN "\nMeses procesados:\n" ANSI_RESET);
    int meses_procesados = 0;
    for (int m = 0; m < MAX_MESES; m++) {
        if (meses_detectados[m]) {
            printf("  • Mes %d: %s✓ Procesado%s\n", m+1, ANSI_VERDE, ANSI_RESET);
            meses_procesados++;
        }
    }
    
    if (meses_procesados == 0) {
        printf("  • %sNingún mes fue procesado%s\n", ANSI_ROJO, ANSI_RESET);
    }
    
    printf(ANSI_CIAN "\nDatos por zona:\n" ANSI_RESET);
    for (int i = 0; i < numZonas; i++) {
        printf("  • %s:\n", zonas[i].nombre);
        int total_dias_zona = 0;
        
        for (int m = 0; m < MAX_MESES; m++) {
            if (meses_detectados[m] && zonas[i].meses[m].numDias > 0) {
                printf("    - Mes %d: %d dias\n", m+1, zonas[i].meses[m].numDias);
                total_dias_zona += zonas[i].meses[m].numDias;
            }
        }
        
        if (total_dias_zona == 0) {
            printf("    - %sNo se importaron datos para esta zona%s\n", ANSI_AMARILLO, ANSI_RESET);
        } else {
            printf("    - %sTotal: %d dias de datos%s\n", ANSI_VERDE, total_dias_zona, ANSI_RESET);
        }
    }
    
    // Recomendaciones
    printf(ANSI_CIAN "\nRecomendaciones:\n" ANSI_RESET);
    if (total_errores > 0) {
        printf("  • %sRevise el formato de los datos con errores%s\n", ANSI_AMARILLO, ANSI_RESET);
    }
    if (total_duplicados > 0) {
        printf("  • %sVerifique los datos duplicados en el archivo fuente%s\n", ANSI_AMARILLO, ANSI_RESET);
    }
    if (total_importados > 0) {
        printf("  • %sUse la opción de checkpoints para guardar los datos importados%s\n", ANSI_VERDE, ANSI_RESET);
    }
    
    printf(ANSI_NEGRITA "\n+=========================================+\n" ANSI_RESET);
}

// Función para determinar mes actual inteligente
int determinarMesActualInteligente(struct Zona zonas[], int numZonas, int meses_detectados[]) {
    printf(ANSI_CIAN "Determinando mes actual inteligente...\n" ANSI_RESET);
    
    // Encontrar el mes más reciente con datos
    int mes_mas_reciente = -1;
    int max_dias_globales = 0;
    
    for (int m = MAX_MESES - 1; m >= 0; m--) {
        if (meses_detectados[m]) {
            int total_dias_mes = 0;
            
            // Contar dias totales en este mes para todas las zonas
            for (int i = 0; i < numZonas; i++) {
                total_dias_mes += zonas[i].meses[m].numDias;
            }
            
            if (total_dias_mes > max_dias_globales) {
                max_dias_globales = total_dias_mes;
                mes_mas_reciente = m;
            }
        }
    }
    
    // Si no encontramos datos, usar el mes actual basado en la fecha del sistema
    if (mes_mas_reciente == -1) {
        extern struct ConfiguracionFechas config_fechas;
        mes_mas_reciente = config_fechas.mes_inicio - 1; // Convertir a índice (0-11)
        if (mes_mas_reciente < 0) mes_mas_reciente = 0;
        
        printf(ANSI_AMARILLO "No se encontraron datos importados, usando mes del sistema: %d\n" ANSI_RESET, 
               mes_mas_reciente + 1);
    } else {
        printf(ANSI_VERDE "Mes mas reciente con datos detectado: %d (con %d dias totales)\n" ANSI_RESET, 
               mes_mas_reciente + 1, max_dias_globales);
    }
    
    return mes_mas_reciente;
}
