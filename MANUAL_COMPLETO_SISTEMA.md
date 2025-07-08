# MANUAL COMPLETO - SISTEMA DE GESTIÓN Y PREDICCIÓN DE CALIDAD DEL AIRE

## DESCRIPCIÓN GENERA### 1. **INICIALIZAR SISTEMA** (Opción 1)
- **Propósito**: Crea datos de muestra NORMALES/SALUDABLES para demostración inicial
- **Valores**: Todos los contaminantes en niveles saludables (PM2.5 ≤ 10 μg/m³)
- **Efecto**: Sobrescribe datos existentes con valores seguros
- **Uso**: Ideal para primeras pruebas del sistema - muestra estado "verde" en todas las ciudades
- **Datos generados**: 5 ciudades ecuatorianas (Quito, Cuenca, Guayaquil, Loja, Ambato) con aire saludable
- **Siguiente paso**: Usar "Configuración → Generar datos de muestreo" para ver alertas variadasEl Sistema de Gestión y Predicción de Calidad del Aire es una aplicación integral desarrollada en C que permite monitorear, analizar y predecir la calidad del aire en múltiples zonas geográficas. El sistema maneja datos de contaminantes atmosféricos (CO2, SO2, NO2, PM2.5) junto con condiciones ambientales (temperatura, viento, humedad).

## CARACTERÍSTICAS PRINCIPALES

### ✅ FUNCIONES BÁSICAS
- **Gestión de 5 zonas predefinidas**: Quito, Cuenca, Guayaquil, Loja, Ambato
- **Monitoreo de 4 contaminantes principales**: CO2, SO2, NO2, PM2.5
- **Condiciones ambientales**: Temperatura, velocidad del viento, humedad
- **Sistema de alertas basado en estándares OMS**
- **Predicciones de calidad del aire para 24 horas**
- **Exportación de reportes y alertas**

### ✅ CARACTERÍSTICAS AVANZADAS
- **Gestión automática de fechas**
- **Sistema de checkpoints y recuperación de datos**
- **Importación/exportación masiva de datos**
- **Interfaz colorizada con códigos ANSI**
- **Validación robusta de entrada de datos**
- **Almacenamiento persistente en archivos binarios**

---

## ESTRUCTURA DEL PROGRAMA

### ARCHIVOS PRINCIPALES
- **`main.c`**: Programa principal y menú principal
- **`funciones.c`**: Funciones básicas del sistema
- **`funciones2.c`**: Funciones avanzadas y menús especializados
- **`funciones.h`**: Declaraciones de estructuras y funciones

### ESTRUCTURAS DE DATOS
```c
struct DatosAmbientales {
    float co2, so2, no2, pm25;     // Contaminantes
    char fecha[11];                 // Formato YYYY-MM-DD
};

struct Semana {
    struct DatosAmbientales dias[7];
    int numDias;
};

struct Zona {
    char nombre[32];
    struct Semana semanas[52];
    int numSemanas;
    // Datos actuales para compatibilidad
    float co2, so2, no2, pm25, temperatura, viento, humedad;
};

struct Sistema {
    struct Zona zonas[5];
    int numZonas;
};
```

---

## GUÍA DE USO

### 🚀 INICIO DEL PROGRAMA

Al ejecutar el programa:
1. **Primera vez**: Solicita configurar fecha inicial del sistema
2. **Ejecuciones posteriores**: Carga automáticamente la configuración guardada
3. Presenta el menú principal con 12 opciones principales

### 📋 MENÚ PRINCIPAL

```
+-----------------------------------------------+
| Sistema Integral de Gestion y Prediccion Aire |
+-----------------------------------------------+
| 1. Inicializar sistema                        |
| 2. Cargar datos historicos                    |
| 3. Mostrar datos actuales y promedios         |
| 4. Predicir contaminacion 24h                 |
| 5. Emitir alertas segun limites OMS           |
| 6. Mostrar recomendaciones                    |
| 7. Ingresar nuevos datos manualmente          |
| 8. Exportar reportes                          |
| 9. Salir                                      |
| 10. Ver historial de zonas                    |
| 11. Ver detalle de una zona                   |
| 12. Buscar zona por nombre                    |
+-----------------------------------------------+
```

---

## FUNCIONES PRINCIPALES

### 1. **INICIALIZAR SISTEMA** (Opción 1)
- **Propósito**: Crea datos de muestra para todas las zonas
- **Efecto**: Sobrescribe datos existentes
- **Uso**: Ideal para primeras pruebas del sistema
- **Datos generados**: 5 ciudades (Quito, Cuenca, Guayaquil, Loja, Ambato) con valores realistas de contaminantes

### 2. **CARGAR DATOS HISTÓRICOS** (Opción 2)
- **Propósito**: Recupera datos previamente guardados desde `datos_hist.dat`
- **Comportamiento**: Si no existe el archivo, muestra mensaje de error
- **Uso**: Para continuar trabajando con datos existentes

### 3. **MOSTRAR DATOS Y PROMEDIOS** (Opción 3)
- **Visualiza**: Tabla formateada con todas las zonas y sus contaminantes actuales
- **Calcula**: Promedios globales de CO2, SO2, NO2, PM2.5
- **Formato**: Tabla ASCII con bordes y alineación profesional

### 4. **PREDICCIÓN 24H** (Opción 4)
- **Algoritmo**: Modelo híbrido (70% valor actual + 30% promedio global)
- **Resultado**: Predicciones de PM2.5 para cada zona
- **Almacenamiento**: Guarda predicciones en `datos_pred.dat` y `predicciones.txt`

### 5. **ALERTAS OMS** (Opción 5)
- **Estándares**: Basado en límites de la Organización Mundial de la Salud
- **Niveles PM2.5**: 
  - 🟢 VERDE: ≤12 μg/m³ (Saludable)
  - 🟡 AMARILLA: 12.1-35 μg/m³ (Moderado)
  - 🟠 NARANJA: 35.1-55 μg/m³ (No saludable para sensibles)
  - 🔴 ROJA: >55 μg/m³ (Peligroso)
- **Alertas CO2**: >0.05 ppm

### 6. **RECOMENDACIONES** (Opción 6)
- **Automáticas**: Basadas en alertas activas
- **Personalizadas**: Según el nivel de contaminación detectado
- **Ejemplos**: "Limite actividades al aire libre", "Use mascarilla"

### 7. **INGRESO MANUAL** (Opción 7)
- **Validación**: Rangos específicos para cada contaminante
- **Interfaz**: Sistema mejorado con opción de cancelar ('c')
- **Rangos válidos**:
  - CO2: 0.000-1.000 ppm
  - SO2: 0.0-100.0 μg/m³
  - NO2: 0.0-300.0 μg/m³
  - PM2.5: 0.0-500.0 μg/m³
  - Temperatura: -50 a 60°C
  - Viento: 0-100 m/s
  - Humedad: 0-100%

### 8. **EXPORTAR REPORTES** (Opción 8)
- **Archivo generado**: `reportes.dat`
- **Contenido**: Alertas actuales y recomendaciones específicas
- **Formato**: Texto plano para fácil lectura

### 9. **SALIR** (Opción 9)
- **Acción**: Termina el programa
- **Persistencia**: Los datos se mantienen en archivos para próximas ejecuciones

### 10. **HISTORIAL DE ZONAS** (Opción 10)
- **Fuente**: Carga datos desde `datos_hist.dat`
- **Visualización**: Tabla resumen de todas las zonas
- **Utilidad**: Revisar datos históricos sin modificar estado actual

### 11. **DETALLE DE ZONA** (Opción 11)
- **Interactivo**: Permite seleccionar zona específica (0-4)
- **Información completa**: Todos los contaminantes y condiciones ambientales
- **Formato**: Tabla detallada con bordes decorativos

### 12. **BUSCAR ZONA** (Opción 12)
- **Búsqueda**: Por nombre (insensible a mayúsculas/minúsculas)
- **Algoritmo**: Comparación caracter por caracter personalizada
- **Resultado**: Muestra datos completos de la zona encontrada

---

## MENÚS AVANZADOS

### 🔧 MENÚ DE CONFIGURACIÓN
Accesible desde funciones especiales del sistema:

#### **Opciones disponibles:**
1. **Cambiar nombre de zona**: Personalizar nombres de las 5 ciudades ecuatorianas
2. **Ajustar máximo de semanas**: Configurar límite (1-104 semanas)
3. **Ajustar máximo de días**: Configurar días por semana (1-7)
4. **Generar datos de muestreo**: Crear datos de prueba VARIADOS específicos por ciudad
   - **Propósito**: Demostrar TODOS los tipos de alertas y niveles de contaminación
   - **Distribución de alertas por ciudad**:
     - 🟢 **Quito**: Nivel VERDE (saludable, PM2.5 ≈ 10 μg/m³)
     - 🟡 **Cuenca**: Nivel AMARILLA (moderado, PM2.5 ≈ 25 μg/m³)
     - 🟠 **Guayaquil**: Nivel NARANJA (no saludable para sensibles, PM2.5 ≈ 45 μg/m³)
     - 🔴 **Loja**: Nivel ROJA (peligroso, PM2.5 ≈ 70 μg/m³)
     - 🟡⚠️ **Ambato**: Nivel AMARILLA + Alerta CO2 alto
   - **Uso**: Después de inicializar con datos normales, usar esta función para ver el sistema completo en acción
5. **Configurar fechas de inicio**: Establecer fecha base del sistema
6. **Crear plantilla para datos**: Generar archivo de ejemplo para importación
7. **Importar datos**: Cargar datos desde archivo externo
8. **Editar fecha actual**: Modificar fecha del sistema manualmente

### 📊 MENÚ DE REPORTES
Sistema avanzado de informes:

#### **Funciones disponibles:**
1. **Reporte semanal por zona**: Análisis detallado día por día
2. **Tabla resumen**: Vista general de todas las zonas
3. **Alertas y recomendaciones**: Sistema visual con colores
4. **Exportar tabla de zonas**: Guardar en `reporte_zonas.txt`
5. **Exportar alertas**: Guardar en `alertas_recomendaciones.txt`

### 💾 MENÚ DE CHECKPOINTS
Sistema de respaldo y recuperación:

#### **Características:**
1. **Guardar semana actual**: Checkpoint permanente
2. **Retroceder a semana anterior**: Navegación temporal
3. **Recuperar datos temporales**: Restaurar backups automáticos
4. **Gestión automática**: Limpieza de archivos temporales

---

## GESTIÓN DE ARCHIVOS

### 📁 ARCHIVOS DE DATOS
- **`datos_hist.dat`**: Almacenamiento principal (binario)
- **`datos_pred.dat`**: Predicciones binarias
- **`predicciones.txt`**: Predicciones en texto legible
- **`config_fechas.dat`**: Configuración de fechas del sistema

### 📁 ARCHIVOS DE REPORTES
- **`reporte_zonas.txt`**: Tabla resumen de zonas
- **`alertas_recomendaciones.txt`**: Alertas y consejos
- **`alertas_temp.txt`**: Archivo temporal de alertas

### 📁 ARCHIVOS DE CHECKPOINTS
- **`[Zona]_semana[N].dat`**: Checkpoints permanentes por zona
- **`temp_[Zona]_semana[N].dat`**: Backups temporales

### 📁 PLANTILLAS DE IMPORTACIÓN
- **`plantilla_datos.txt`**: Archivo de ejemplo para carga masiva
- **Formato personalizable**: Según nombre especificado por usuario

---

## SISTEMA DE IMPORTACIÓN/EXPORTACIÓN

### 📥 IMPORTACIÓN DE DATOS

#### **Formatos soportados:**
1. **Formato simple CSV**:
   ```
   Zona,Fecha,CO2,SO2,NO2,PM25
   Quito,2024-07-01,400.0,5.0,25.0,15.0
   Cuenca,2024-07-02,380.0,4.0,22.0,12.0
   ```

2. **Formato de tabla ASCII** (decorativo):
   ```
   +--------+------------+--------+--------+--------+--------+
   | Zona   | Fecha      | CO2    | SO2    | NO2    | PM2.5  |
   +--------+------------+--------+--------+--------+--------+
   | Quito  | 2024-07-01 | 400.0  | 5.0    | 25.0   | 15.0   |
   | Cuenca | 2024-07-02 | 380.0  | 4.0    | 22.0   | 12.0   |
   ```

#### **Proceso de importación:**
1. El sistema lee el archivo línea por línea
2. Ignora comentarios (líneas que inician con #) y encabezados
3. Valida rangos de datos automáticamente
4. Distribuye datos en semanas disponibles
5. Muestra progreso y resumen final

### 📤 EXPORTACIÓN DE PLANTILLAS

#### **Características:**
- **Instrucciones incluidas**: Guía paso a paso en el archivo
- **Valores de ejemplo**: Datos realistas para referencia
- **Formato flexible**: Acepta múltiples estilos de entrada
- **Validación automática**: El sistema verifica al importar

---

## ALGORITMOS Y CÁLCULOS

### 🔮 PREDICCIÓN DE CONTAMINACIÓN
```
Predicción = (Valor_Actual × 0.7) + (Promedio_Global × 0.3)
```
- **Ventaja**: Equilibra tendencia local con contexto global
- **Aplicación**: Específica para PM2.5 (principal indicador)

### 📊 CÁLCULO DE PROMEDIOS
- **Por zona**: Suma de días/número de días en semana actual
- **Global**: Suma de todas las zonas/número de zonas
- **Temporal**: Datos de la semana más reciente con datos

### 🚨 SISTEMA DE ALERTAS
- **Prioridad**: PM2.5 como indicador principal
- **Secundario**: CO2 para alertas adicionales
- **Umbrales**: Basados en estándares OMS actualizados

---

## VALIDACIONES Y CONTROLES

### ✅ VALIDACIÓN DE ENTRADA
- **Rangos numéricos**: Específicos por tipo de contaminante
- **Formato de fecha**: YYYY-MM-DD obligatorio
- **Caracteres especiales**: Evitados para compatibilidad
- **Opción de cancelar**: Disponible en todos los menús con 'c'

### 🛡️ MANEJO DE ERRORES
- **Archivos inexistentes**: Mensajes informativos claros
- **Datos fuera de rango**: Rechazo automático con explicación
- **Memoria insuficiente**: Límites predefinidos (5 zonas, 52 semanas)
- **Entrada inválida**: Bucles de reintento con instrucciones

### 🔄 CONSISTENCIA DE DATOS
- **Sincronización**: Entre datos semanales y valores actuales
- **Integridad**: Verificación de rangos antes de almacenar
- **Persistencia**: Guardado automático en operaciones críticas

---

## CÓDIGOS DE COLORES Y INTERFAZ

### 🎨 SISTEMA DE COLORES ANSI
- **🟢 VERDE**: Estados saludables, operaciones exitosas
- **🟡 AMARILLO**: Advertencias, información importante
- **🟠 NARANJA**: Alertas moderadas
- **🔴 ROJO**: Peligro, errores críticos
- **🔵 AZUL**: Headers, información general
- **🟣 MAGENTA**: Alertas especiales
- **🔷 CIAN**: Menús, decoraciones
- **⚫ NEGRITA**: Énfasis, títulos

### 📱 DISEÑO DE INTERFAZ
- **Tablas ASCII**: Bordes decorativos con +, -, |
- **Menús contextuales**: Numeración clara y consistente
- **Mensajes informativos**: Claros y en español
- **Progreso visual**: Indicadores de estado en operaciones largas

---

## ESPECIFICACIONES TÉCNICAS

### ⚙️ LÍMITES DEL SISTEMA
- **Zonas máximas**: 5 ciudades ecuatorianas (configuración fija)
- **Semanas por zona**: 52 (configurable 1-104)
- **Días por semana**: 7 (configurable 1-7)
- **Alertas simultáneas**: 10 máximo
- **Tamaño de nombres**: 31 caracteres + terminador

### 💾 ALMACENAMIENTO
- **Binario**: Estructuras completas para velocidad
- **Texto**: Reportes legibles para usuarios
- **Temporal**: Archivos de respaldo automático
- **Configuración**: Persistencia de preferencias

### 🔧 COMPILACIÓN
```bash
gcc -Wall -Wextra -o programa main.c funciones.c funciones2.c
```
- **Estándar**: C99 compatible
- **Warnings**: Código limpio sin advertencias
- **Portabilidad**: Windows/Linux (ANSI codes)

---

## FLUJO DE TRABAJO RECOMENDADO

### 👨‍💼 USUARIO BÁSICO (DEMOSTRACIÓN COMPLETA)
1. **Primera vez**: Ejecutar → Configurar fecha → Opción 1 (Inicializar con datos saludables)
2. **Ver estado normal**: Opción 3 (Tabla), Opción 5 (Alertas) - Todo en verde ✅
3. **Ver alertas variadas**: Configuración → Generar datos de muestreo → Ver diferencias por ciudad
4. **Explorar alertas**: Opción 5 (Ver todas las alertas), Opción 6 (Recomendaciones)
5. **Uso regular**: Opción 7 (Ingreso manual) para datos reales propios

### 👨‍🔬 USUARIO AVANZADO
1. **Configuración inicial**: Menú configuración → Personalizar zonas y fechas
2. **Carga masiva**: Crear plantilla → Editar datos → Importar
3. **Análisis**: Menú reportes → Exportar para análisis externo
4. **Respaldo**: Menú checkpoints → Guardar estados importantes

### 👨‍💻 ADMINISTRADOR DEL SISTEMA
1. **Mantenimiento**: Revisar archivos .dat periódicamente
2. **Configuración**: Ajustar límites según necesidades
3. **Respaldos**: Copiar archivos críticos regularmente
4. **Monitoreo**: Verificar logs de importación/exportación

---

## RESOLUCIÓN DE PROBLEMAS COMUNES

### ❌ PROBLEMAS FRECUENTES

#### **"No se pudo cargar datos"**
- **Causa**: Archivo `datos_hist.dat` no existe
- **Solución**: Usar Opción 1 para inicializar sistema

#### **"Entrada inválida"**
- **Causa**: Formato numérico incorrecto o fuera de rango
- **Solución**: Usar punto (.) como separador decimal, verificar rangos

#### **"Zona no encontrada"**
- **Causa**: Búsqueda por nombre sin coincidencia exacta
- **Solución**: Verificar ortografía, usar mayúsculas/minúsculas correctas

#### **Colores no se muestran**
- **Causa**: Terminal no compatible con ANSI
- **Solución**: Usar terminal moderno (Windows Terminal, PowerShell 7+)

### 🔧 MANTENIMIENTO

#### **Limpieza de archivos temporales**
```bash
del temp_*.dat
del alertas_temp.txt
```

#### **Reset completo del sistema**
```bash
del datos_hist.dat
del config_fechas.dat
del *.txt
```

#### **Verificación de integridad**
- Revisar que archivos .dat no estén corruptos
- Verificar permisos de escritura en directorio
- Comprobar espacio en disco disponible

---

## CASOS DE USO EJEMPLO

### 🏭 MONITOREO INDUSTRIAL
**Escenario**: Empresa con 5 plantas industriales en Ecuador
**Configuración**: Usar las ciudades predefinidas (Quito, Cuenca, Guayaquil, Loja, Ambato) o renombrar por ubicación específica de plantas
**Uso**: Ingreso diario manual + alertas automáticas
**Beneficio**: Cumplimiento normativo ecuatoriano y prevención

### 🏙️ GESTIÓN MUNICIPAL ECUATORIANA
**Escenario**: Ayuntamiento monitoreando calidad del aire en ciudades ecuatorianas
**Configuración**: Usar ciudades predefinidas (Quito, Cuenca, Guayaquil, Loja, Ambato) o renombrar por distritos específicos, importación masiva de sensores
**Uso**: Reportes semanales + exportación para ciudadanos
**Beneficio**: Transparencia gubernamental y planificación urbana sostenible

### 🔬 INVESTIGACIÓN ACADÉMICA
**Escenario**: Universidad ecuatoriana estudiando patrones de contaminación en las principales ciudades del país
**Configuración**: Datos históricos extensos de Quito, Cuenca, Guayaquil, Loja y Ambato via importación
**Uso**: Análisis de tendencias urbanas + predicciones climáticas a largo plazo
**Beneficio**: Papers científicos sobre calidad del aire en Ecuador y modelos ambientales mejorados

### 👥 USO PERSONAL EN ECUADOR
**Escenario**: Ciudadano ecuatoriano interesado en calidad del aire de su ciudad
**Configuración**: Sistema básico con datos de estaciones públicas de MAAE (Ministerio del Ambiente) de Ecuador
**Uso**: Consultas ocasionales de Quito, Cuenca, Guayaquil + alertas para actividades al aire libre
**Beneficio**: Decisiones informadas sobre salud personal y familiar en el contexto ecuatoriano

---

## DESARROLLO Y EXTENSIONES

### 🚀 POSIBLES MEJORAS FUTURAS
- **Base de datos**: Migración de archivos planos a SQL
- **API web**: Interfaz REST para aplicaciones móviles
- **Gráficos**: Visualización de tendencias temporales
- **Machine Learning**: Predicciones más sofisticadas
- **GPS**: Ubicación automática de zonas
- **Notificaciones**: Alertas push en tiempo real

### 🔧 PERSONALIZACIÓN ACTUAL
- **Nombres de zonas**: Completamente configurables
- **Límites temporales**: Ajustables según necesidades
- **Umbrales de alerta**: Modificables en código fuente
- **Formatos de import**: Extensibles para nuevos formatos
- **Colores de interfaz**: Personalizables via #define

---

## CONCLUSIÓN

El Sistema de Gestión y Predicción de Calidad del Aire representa una solución completa y robusta para el monitoreo ambiental. Su diseño modular, interfaz intuitiva y capacidades de procesamiento masivo lo hacen adecuado tanto para uso personal como profesional.

La combinación de funcionalidades básicas accesibles y características avanzadas potentes permite que usuarios de diferentes niveles técnicos puedan aprovechar el sistema según sus necesidades específicas.

---

**Versión del Manual**: 1.0  
**Fecha**: Julio 2025  
**Compatibilidad**: Windows 10+, Linux, macOS  
**Autor**: Sistema de Gestión Ambiental  
**Soporte**: Consultar código fuente para detalles técnicos adicionales
