#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "funciones.h"

// Inicializa el sistema con datos de muestreo y guarda en datos_hist.dat
void inicializarSistema(Sistema *s) {
    static const char *nombres[5] = {"Centro", "Norte", "Sur", "Este", "Oeste"};
    float datos[5][7] = {
        {0.04, 0.02, 0.03, 35.2, 22.5, 3.2, 60.0},
        {0.05, 0.01, 0.04, 40.1, 23.0, 2.8, 55.0},
        {0.03, 0.03, 0.02, 30.5, 21.0, 4.0, 65.0},
        {0.06, 0.02, 0.05, 50.0, 24.0, 3.5, 58.0},
        {0.04, 0.01, 0.03, 33.3, 22.0, 3.0, 62.0}
    };
    s->numZonas = 5;
    for (int i = 0; i < 5; i++) {
        strncpy(s->zonas[i].nombre, nombres[i], 31);
        s->zonas[i].nombre[31] = '\0';
        s->zonas[i].co2 = datos[i][0];
        s->zonas[i].so2 = datos[i][1];
        s->zonas[i].no2 = datos[i][2];
        s->zonas[i].pm25 = datos[i][3];
        s->zonas[i].temperatura = datos[i][4];
        s->zonas[i].viento = datos[i][5];
        s->zonas[i].humedad = datos[i][6];
    }
    guardarDatos(s, "datos_hist.dat");
    printf("Datos guardados correctamente.\n");
}

// Carga datos historicos desde archivo binario
int cargarDatosHistoricos(Sistema *s, const char *ruta) {
    FILE *f = fopen(ruta, "rb");
    if (!f) return 0;
    fread(s, sizeof(Sistema), 1, f);
    fclose(f);
    return 1;
}

// Guarda datos en archivo binario
int guardarDatos(Sistema *s, const char *ruta) {
    FILE *f = fopen(ruta, "wb");
    if (!f) return 0;
    fwrite(s, sizeof(Sistema), 1, f);
    fclose(f);
    return 1;
}

// Calcula promedios de contaminantes para todas las zonas
void calcularPromedios(Sistema *s, float promedios[]) {
    float sumas[4] = {0};
    for (int i = 0; i < s->numZonas; i++) {
        sumas[0] += s->zonas[i].co2;
        sumas[1] += s->zonas[i].so2;
        sumas[2] += s->zonas[i].no2;
        sumas[3] += s->zonas[i].pm25;
    }
    for (int j = 0; j < 4; j++)
        promedios[j] = sumas[j] / s->numZonas;
}

// Predice contaminacion para 24h futuras (simple: +5%)
void predecirContaminacion(Sistema *s, float prediccion[]) {
    for (int i = 0; i < s->numZonas; i++) {
        prediccion[i] = s->zonas[i].pm25 * 1.05f;
    }
    // Extra: guarda prediccion en archivo datos_pred.dat
    FILE *f = fopen("datos_pred.dat", "wb");
    if (f) {
        fwrite(prediccion, sizeof(float), s->numZonas, f);
        fclose(f);
    }
}

// Emite alertas si se superan limites OMS
void emitirAlertas(Sistema *s, float prediccion[], char alertas[][64], int *nAlertas) {
    *nAlertas = 0;
    for (int i = 0; i < s->numZonas; i++) {
        if (prediccion[i] > 35.0f) {
            snprintf(alertas[*nAlertas], 64, "Alerta: %s PM2.5 > 35 ug/m3", s->zonas[i].nombre);
            (*nAlertas)++;
        }
        // Extra: alerta por CO2
        if (s->zonas[i].co2 > 0.05f) {
            snprintf(alertas[*nAlertas], 64, "Alerta: %s CO2 > 0.05 ppm", s->zonas[i].nombre);
            (*nAlertas)++;
        }
    }
}

// Genera recomendaciones segun alertas
void generarRecomendaciones(char alertas[][64], int nAlertas) {
    if (nAlertas == 0) {
        printf("No hay alertas. Calidad del aire aceptable.\n");
        return;
    }
    printf("Recomendaciones:\n");
    for (int i = 0; i < nAlertas; i++) {
        printf("- %s: Limite actividades al aire libre.\n", alertas[i]);
    }
    // Extra: recomendaciones generales
    if (nAlertas > 0) {
        printf("- Considere usar mascarilla en exteriores.\n");
        printf("- Mantenga puertas y ventanas cerradas.\n");
    }
}

// Muestra tabla de zonas y contaminantes
void mostrarTablaZonas(Sistema *s) {
    printf("+--------+-------+-------+-------+-------+\n");
    printf("| Zona   | CO2   | SO2   | NO2   | PM2.5 |\n");
    printf("+--------+-------+-------+-------+-------+\n");
    for (int i = 0; i < s->numZonas; i++) {
        printf("| %-6s | %5.2f | %5.2f | %5.2f | %5.1f |\n",
            s->zonas[i].nombre, s->zonas[i].co2, s->zonas[i].so2,
            s->zonas[i].no2, s->zonas[i].pm25);
    }
    printf("+--------+-------+-------+-------+-------+\n");
}

// Lee un entero validado, permite cancelar si allowCancel=1
int leerEntero(const char *prompt, int min, int max, int allowCancel) {
    char buf[32];
    int val;
    while (1) {
        printf("%s", prompt);
        if (fgets(buf, sizeof(buf), stdin) == NULL) continue;
        if (allowCancel && (buf[0] == 'c' || buf[0] == 'C')) return -1;
        if (sscanf(buf, "%d", &val) == 1 && val >= min && val <= max)
            return val;
        printf("Entrada invalida. Intente de nuevo.\n");
    }
}

// Lee un float validado, permite cancelar si allowCancel=1
float leerFloat(const char *prompt, float min, float max, int allowCancel) {
    char buf[32];
    float val;
    while (1) {
        printf("%s", prompt);
        if (fgets(buf, sizeof(buf), stdin) == NULL) continue;
        if (allowCancel && (buf[0] == 'c' || buf[0] == 'C')) return -1.0f;
        if (sscanf(buf, "%f", &val) == 1 && val >= min && val <= max)
            return val;
        printf("Entrada invalida. Intente de nuevo.\n");
    }
}

// Confirmacion simple (si/no)
int confirmar(const char *mensaje) {
    char buf[8];
    printf("%s (s/n): ", mensaje);
    if (fgets(buf, sizeof(buf), stdin) == NULL) return 0;
    return (buf[0] == 's' || buf[0] == 'S');
}

// Ayuda contextual para menus
void ayudaMenu(const char *menuNombre) {
    printf("Ayuda para menu %s:\n", menuNombre);
    printf("Ingrese el numero de la opcion deseada. Presione 'c' para cancelar o volver.\n");
    // Extra: ayuda extendida
    printf("Opciones principales:\n");
    printf("1: Inicializa el sistema con datos de ejemplo.\n");
    printf("2: Carga datos historicos desde archivo.\n");
    printf("3: Muestra tabla y promedios de contaminantes.\n");
    printf("4: Predice contaminacion para 24h futuras.\n");
    printf("5: Muestra alertas si hay contaminantes fuera de norma.\n");
    printf("6: Muestra recomendaciones de mitigacion.\n");
    printf("7: Permite ingresar datos manualmente.\n");
    printf("8: Exporta alertas y recomendaciones a archivo.\n");
    printf("10: Muestra historial de zonas.\n");
    printf("11: Muestra detalle de una zona.\n");
}

// Muestra el historial de datos de todas las zonas (si existe el archivo)
void mostrarHistorialZonas() {
    Sistema s;
    FILE *f = fopen("datos_hist.dat", "rb");
    if (!f) {
        printf("No hay historial disponible.\n");
        return;
    }
    fread(&s, sizeof(Sistema), 1, f);
    fclose(f);
    printf("Historial de zonas:\n");
    mostrarTablaZonas(&s);
}

// Muestra detalles de una zona específica
void mostrarDetalleZona(Sistema *s) {
    if (s->numZonas == 0) {
        printf("No hay zonas cargadas.\n");
        return;
    }
    printf("Seleccione zona para ver detalles (0-%d, c para cancelar):\n", s->numZonas-1);
    for (int i = 0; i < s->numZonas; i++) {
        printf("%d. %s\n", i, s->zonas[i].nombre);
    }
    int idx = leerEntero("Opcion: ", 0, s->numZonas-1, 1);
    if (idx < 0) return;
    Zona *z = &s->zonas[idx];
    printf("+--------------------------------------+\n");
    printf("| Detalle de zona: %-18s |\n", z->nombre);
    printf("+--------------------------------------+\n");
    printf("| CO2:        %6.3f ppm                |\n", z->co2);
    printf("| SO2:        %6.3f ppm                |\n", z->so2);
    printf("| NO2:        %6.3f ppm                |\n", z->no2);
    printf("| PM2.5:      %6.1f ug/m3              |\n", z->pm25);
    printf("| Temp:       %6.1f C                  |\n", z->temperatura);
    printf("| Viento:     %6.1f m/s                |\n", z->viento);
    printf("| Humedad:    %6.1f %%                  |\n", z->humedad);
    printf("+--------------------------------------+\n");
}

// Extra: Permite buscar zona por nombre y mostrar sus datos
void buscarZonaPorNombre(Sistema *s) {
    if (s->numZonas == 0) {
        printf("No hay zonas cargadas.\n");
        return;
    }
    char nombre[32];
    printf("Ingrese nombre de la zona a buscar (c para cancelar): ");
    if (!fgets(nombre, sizeof(nombre), stdin)) return;
    if (nombre[0] == 'c' || nombre[0] == 'C') return;
    nombre[strcspn(nombre, "\n")] = 0;
    for (int i = 0; i < s->numZonas; i++) {
        if (strcasecmp(nombre, s->zonas[i].nombre) == 0) {
            printf("Zona encontrada:\n");
            printf("CO2: %.3f, SO2: %.3f, NO2: %.3f, PM2.5: %.1f, Temp: %.1f, Viento: %.1f, Humedad: %.1f\n",
                s->zonas[i].co2, s->zonas[i].so2, s->zonas[i].no2, s->zonas[i].pm25,
                s->zonas[i].temperatura, s->zonas[i].viento, s->zonas[i].humedad);
            return;
        }
    }
    printf("Zona no encontrada.\n");
}

// Maneja la opcion seleccionada del menu principal
void manejarOpcion(int opcion, Sistema *s) {
    float promedios[4], prediccion[MAX_ZONAS];
    char alertas[MAX_ALERTAS][64];
    int nAlertas;
    switch (opcion) {
        case 1:
            if (confirmar("¿Seguro que desea inicializar el sistema? Se perderan los datos actuales.")) {
                inicializarSistema(s);
            } else {
                printf("Operacion cancelada.\n");
            }
            break;
        case 2:
            if (cargarDatosHistoricos(s, "datos_hist.dat"))
                printf("Datos cargados correctamente.\n");
            else
                printf("No se pudo cargar datos.\n");
            break;
        case 3:
            mostrarTablaZonas(s);
            calcularPromedios(s, promedios);
            printf("Promedios: CO2=%.2f SO2=%.2f NO2=%.2f PM2.5=%.1f\n",
                promedios[0], promedios[1], promedios[2], promedios[3]);
            break;
        case 4:
            predecirContaminacion(s, prediccion);
            printf("Prediccion PM2.5 para 24h futuras:\n");
            for (int i = 0; i < s->numZonas; i++)
                printf("%s: %.1f ug/m3\n", s->zonas[i].nombre, prediccion[i]);
            break;
        case 5:
            predecirContaminacion(s, prediccion);
            emitirAlertas(s, prediccion, alertas, &nAlertas);
            if (nAlertas == 0)
                printf("No hay alertas.\n");
            else
                for (int i = 0; i < nAlertas; i++)
                    printf("%s\n", alertas[i]);
            break;
        case 6:
            predecirContaminacion(s, prediccion);
            emitirAlertas(s, prediccion, alertas, &nAlertas);
            generarRecomendaciones(alertas, nAlertas);
            break;
        case 7: {
            if (s->numZonas == 0) {
                printf("No hay zonas cargadas. Cargue datos primero.\n");
                break;
            }
            printf("Ingreso manual de datos (c para cancelar):\n");
            int idx = leerEntero("Seleccione zona (0-4): ", 0, 4, 1);
            if (idx < 0) break;
            s->zonas[idx].co2 = leerFloat("CO2: ", 0, 1, 1);
            s->zonas[idx].so2 = leerFloat("SO2: ", 0, 1, 1);
            s->zonas[idx].no2 = leerFloat("NO2: ", 0, 1, 1);
            s->zonas[idx].pm25 = leerFloat("PM2.5: ", 0, 500, 1);
            s->zonas[idx].temperatura = leerFloat("Temperatura: ", -50, 60, 1);
            s->zonas[idx].viento = leerFloat("Viento: ", 0, 100, 1);
            s->zonas[idx].humedad = leerFloat("Humedad: ", 0, 100, 1);
            guardarDatos(s, "datos_hist.dat");
            printf("Datos guardados correctamente.\n");
            break;
        }
        case 8: {
            if (!confirmar("¿Desea exportar los reportes actuales?")) {
                printf("Operacion cancelada.\n");
                break;
            }
            predecirContaminacion(s, prediccion);
            emitirAlertas(s, prediccion, alertas, &nAlertas);
            FILE *f = fopen("reportes.dat", "w");
            if (f) {
                for (int i = 0; i < nAlertas; i++)
                    fprintf(f, "%s\n", alertas[i]);
                fprintf(f, "Recomendaciones:\n");
                if (nAlertas == 0)
                    fprintf(f, "No hay alertas. Calidad del aire aceptable.\n");
                else
                    for (int i = 0; i < nAlertas; i++)
                        fprintf(f, "- %s: Limite actividades al aire libre.\n", alertas[i]);
                fclose(f);
                printf("Reportes exportados correctamente.\n");
            } else {
                printf("No se pudo exportar reportes.\n");
            }
            break;
        }
        case 9:
            printf("Saliendo del sistema.\n");
            break;
        case 10:
            mostrarHistorialZonas();
            break;
        case 11:
            mostrarDetalleZona(s);
            break;
        case 12:
            buscarZonaPorNombre(s);
            break;
        default:
            printf("Opcion no valida.\n");
    }
}

// Muestra el menu principal
void mostrarMenuPrincipal() {
    printf("\n+-----------------------------------------------+\n");
    printf("| Sistema Integral de Gestion y Prediccion Aire |\n");
    printf("+-----------------------------------------------+\n");
    printf("| 1. Inicializar sistema                       |\n");
    printf("| 2. Cargar datos historicos                   |\n");
    printf("| 3. Mostrar datos actuales y promedios        |\n");
    printf("| 4. Predecir contaminacion 24h                |\n");
    printf("| 5. Emitir alertas segun limites OMS          |\n");
    printf("| 6. Mostrar recomendaciones                   |\n");
    printf("| 7. Ingresar nuevos datos manualmente         |\n");
    printf("| 8. Exportar reportes                         |\n");
    printf("| 9. Salir                                     |\n");
    printf("| 10. Ver historial de zonas                   |\n");
    printf("| 11. Ver detalle de una zona                  |\n");
    printf("| 12. Buscar zona por nombre                   |\n");
    printf("+-----------------------------------------------+\n");
    printf("Presione 'h' para ayuda.\n");
}
