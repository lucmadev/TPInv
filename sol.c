
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROVINCIAS 20
#define MAX_PRODUCTOS 20
#define MAX_VENTAS 200
#define PROVINCIA_FILE "provincias.txt"
#define PRODUCTO_FILE "productos.txt"
#define VENTA_FILE "ventas.txt"

typedef struct {
    char codigo[3];
    char nombre[16];
} Provincia;

typedef struct {
    char codigo[3];
    char descripcion[21];
} Producto;

typedef struct {
    int factura;
    char codigoProducto[3];
    char codigoProvincia[3];
    float precioUnitario;
    float cantidad;
} Venta;

Provincia provincias[MAX_PROVINCIAS];
Producto productos[MAX_PRODUCTOS];
Venta ventas[MAX_VENTAS];
int contadorProvincias = 0;
int contadorProductos = 0;
int contadorVentas = 0;

void vaciarArchivos() {
    // Vaciar archivos usando "w" elimina el contenido anterior
    FILE *fp;

    // Vaciar provincias
    fp = fopen(PROVINCIA_FILE, "w");
    fclose(fp); // Si el archivo se abre correctamente, se cierra inmediatamente

    // Vaciar productos
    fp = fopen(PRODUCTO_FILE, "w");
    fclose(fp);

    // Vaciar ventas
    fp = fopen(VENTA_FILE, "w");
    fclose(fp);

    printf("Archivos vaciados correctamente.\n");
    getchar(); // Espera a que el usuario presione una tecla
}

void cargarProvincias() {
    FILE *fp = fopen(PROVINCIA_FILE, "a");
    if (!fp) {
        printf("Error al abrir el archivo de provincias.\n");
        return;
    }

    while (contadorProvincias < MAX_PROVINCIAS) {
        Provincia nuevaProvincia;
        printf("Ingrese codigo de provincia (0 para salir): ");
        scanf("%s", nuevaProvincia.codigo);
        if (strcmp(nuevaProvincia.codigo, "0") == 0) break;

        // Limpiar el buffer de entrada antes de usar fgets
        getchar(); // Para consumir el salto de línea que queda en el buffer

        printf("Ingrese nombre de provincia: ");
        fgets(nuevaProvincia.nombre, sizeof(nuevaProvincia.nombre), stdin);
        nuevaProvincia.nombre[strcspn(nuevaProvincia.nombre, "\n")] = '\0'; // Eliminar salto de línea

        fprintf(fp, "%s;%s\n", nuevaProvincia.codigo, nuevaProvincia.nombre);
        provincias[contadorProvincias++] = nuevaProvincia;
    }
    fclose(fp);
}

void cargarProductos() {
    FILE *fp = fopen(PRODUCTO_FILE, "a");
    if (!fp) {
        printf("Error al abrir el archivo de productos.\n");
        return;
    }

    while (contadorProductos < MAX_PRODUCTOS) {
        Producto nuevoProducto;
        printf("Ingrese codigo de producto (0 para salir): ");
        scanf("%s", nuevoProducto.codigo);
        if (strcmp(nuevoProducto.codigo, "0") == 0) break;

        // Limpiar el buffer de entrada antes de usar fgets
        getchar(); // Para consumir el salto de línea que queda en el buffer

        printf("Ingrese descripcion de producto: ");
        fgets(nuevoProducto.descripcion, sizeof(nuevoProducto.descripcion), stdin);
        nuevoProducto.descripcion[strcspn(nuevoProducto.descripcion, "\n")] = '\0'; // Eliminar salto de línea

        fprintf(fp, "%s;%s\n", nuevoProducto.codigo, nuevoProducto.descripcion);
        productos[contadorProductos++] = nuevoProducto;
    }
    fclose(fp);
}

int productoExiste(const char *codigo) {
    FILE *fp = fopen(PRODUCTO_FILE, "r");
    if (!fp) return 0;

    char linea[100];
    while (fgets(linea, sizeof(linea), fp)) {
        char codigoProducto[3];
        sscanf(linea, "%[^;];", codigoProducto);
        if (strcmp(codigoProducto, codigo) == 0) {
            fclose(fp);
            return 1; // Producto existe
        }
    }
    fclose(fp);
    return 0; // Producto no existe
}

int provinciaExiste(const char *codigo) {
    FILE *fp = fopen(PROVINCIA_FILE, "r");
    if (!fp) return 0;

    char linea[100];
    while (fgets(linea, sizeof(linea), fp)) {
        char codigoProvincia[3];
        sscanf(linea, "%[^;];", codigoProvincia);
        if (strcmp(codigoProvincia, codigo) == 0) {
            fclose(fp);
            return 1; // Provincia existe
        }
    }
    fclose(fp);
    return 0; // Provincia no existe
}

void cargarVentas() {
    FILE *fp = fopen(VENTA_FILE, "a");
    if (!fp) {
        printf("Error al abrir el archivo de ventas.\n");
        return;
    }

    while (contadorVentas < MAX_VENTAS) {
        Venta nuevaVenta;
        printf("Ingrese numero de factura (0 para salir): ");
        scanf("%d", &nuevaVenta.factura);
        if (nuevaVenta.factura == 0) break;

        // Validar código de producto
        printf("Ingrese codigo de producto: ");
        scanf("%s", nuevaVenta.codigoProducto);
        if (!productoExiste(nuevaVenta.codigoProducto)) {
            printf("El codigo de producto '%s' no existe.\n", nuevaVenta.codigoProducto);
            continue; // Regresar al inicio del ciclo para permitir una nueva entrada
        }

        // Validar código de provincia
        printf("Ingrese codigo de provincia: ");
        scanf("%s", nuevaVenta.codigoProvincia);
        if (!provinciaExiste(nuevaVenta.codigoProvincia)) {
            printf("El codigo de provincia '%s' no existe.\n", nuevaVenta.codigoProvincia);
            continue; // Regresar al inicio del ciclo para permitir una nueva entrada
        }

        printf("Ingrese precio unitario: ");
        scanf("%f", &nuevaVenta.precioUnitario);
        printf("Ingrese cantidad: ");
        scanf("%f", &nuevaVenta.cantidad);

        fprintf(fp, "%d;%s;%s;%.2f;%.2f\n", nuevaVenta.factura, nuevaVenta.codigoProducto,
                nuevaVenta.codigoProvincia, nuevaVenta.precioUnitario, nuevaVenta.cantidad);
        ventas[contadorVentas++] = nuevaVenta;
    }
    fclose(fp);
}

void mostrarListado() {
    FILE *ventaFile = fopen(VENTA_FILE, "r");
    if (!ventaFile) {
        printf("Error al abrir el archivo de ventas.\n");
        return;
    }

    // Estructuras para almacenar datos agrupados
    struct ProductoInfo {
        char codigo[3];
        char descripcion[21];
        float total;
        struct {
            char codigo[3];
            char nombre[16];
            float subtotal;
            struct {
                int factura;
                float monto;
            } facturas[MAX_VENTAS];
            int numFacturas;
        } provincias[MAX_PROVINCIAS];
        int numProvincias;
    } productos[MAX_PRODUCTOS];
    int numProductos = 0;

    // Inicializar estructura
    memset(productos, 0, sizeof(productos));

    // Leer y agrupar todas las ventas
    char linea[100];
    while (fgets(linea, sizeof(linea), ventaFile)) {
        Venta venta;
        sscanf(linea, "%d;%[^;];%[^;];%f;%f",
            &venta.factura,
            venta.codigoProducto,
            venta.codigoProvincia,
            &venta.precioUnitario,
            &venta.cantidad
        );

        // Buscar o crear producto
        int prodIndex = -1;
        for (int i = 0; i < numProductos; i++) {
            if (strcmp(productos[i].codigo, venta.codigoProducto) == 0) {
                prodIndex = i;
                break;
            }
        }

        if (prodIndex == -1) {
            prodIndex = numProductos++;
            strcpy(productos[prodIndex].codigo, venta.codigoProducto);

            // Obtener descripción del producto
            FILE *fpProducto = fopen(PRODUCTO_FILE, "r");
            if (fpProducto) {
                char lineaProducto[100];
                while (fgets(lineaProducto, sizeof(lineaProducto), fpProducto)) {
                    char codigo[3];
                    char descripcion[21];
                    sscanf(lineaProducto, "%[^;];%[^\n]", codigo, descripcion);
                    if (strcmp(codigo, venta.codigoProducto) == 0) {
                        strcpy(productos[prodIndex].descripcion, descripcion);
                        break;
                    }
                }
                fclose(fpProducto);
            }
        }

        // Buscar o crear provincia dentro del producto
        int provIndex = -1;
        for (int i = 0; i < productos[prodIndex].numProvincias; i++) {
            if (strcmp(productos[prodIndex].provincias[i].codigo, venta.codigoProvincia) == 0) {
                provIndex = i;
                break;
            }
        }

        if (provIndex == -1) {
            provIndex = productos[prodIndex].numProvincias++;
            strcpy(productos[prodIndex].provincias[provIndex].codigo, venta.codigoProvincia);

            // Obtener nombre de la provincia
            FILE *fpProvincia = fopen(PROVINCIA_FILE, "r");
            if (fpProvincia) {
                char lineaProvincia[100];
                while (fgets(lineaProvincia, sizeof(lineaProvincia), fpProvincia)) {
                    char codigo[3];
                    char nombre[16];
                    sscanf(lineaProvincia, "%[^;];%[^\n]", codigo, nombre);
                    if (strcmp(codigo, venta.codigoProvincia) == 0) {
                        strcpy(productos[prodIndex].provincias[provIndex].nombre, nombre);
                        break;
                    }
                }
                fclose(fpProvincia);
            }
        }

        // Agregar factura
        int factIndex = productos[prodIndex].provincias[provIndex].numFacturas++;
        productos[prodIndex].provincias[provIndex].facturas[factIndex].factura = venta.factura;
        productos[prodIndex].provincias[provIndex].facturas[factIndex].monto =
            venta.precioUnitario * venta.cantidad;

        // Actualizar subtotales y totales
        productos[prodIndex].provincias[provIndex].subtotal +=
            productos[prodIndex].provincias[provIndex].facturas[factIndex].monto;
        productos[prodIndex].total +=
            productos[prodIndex].provincias[provIndex].facturas[factIndex].monto;
    }
    fclose(ventaFile);

    // Mostrar el reporte con el formato especificado
    float totalGeneral = 0;
    for (int i = 0; i < numProductos; i++) {
        printf("Producto   %s - %s\n", productos[i].codigo, productos[i].descripcion);

        for (int j = 0; j < productos[i].numProvincias; j++) {
            printf("         Provincia   %s - %s\n",
                productos[i].provincias[j].codigo,
                productos[i].provincias[j].nombre);

            for (int k = 0; k < productos[i].provincias[j].numFacturas; k++) {
                printf("                             Factura %d        $ %.2f\n",
                    productos[i].provincias[j].facturas[k].factura,
                    productos[i].provincias[j].facturas[k].monto);
            }

            printf("         Subtotal %s                          $ %.2f\n",
                productos[i].provincias[j].codigo,
                productos[i].provincias[j].subtotal);
        }

        printf("Total %s                                  $ %.2f\n",
            productos[i].descripcion,
            productos[i].total);

        totalGeneral += productos[i].total;
    }

    printf("Total General: $ %.2f\n", totalGeneral);

    printf("\nPresione Enter para continuar...");
    getchar(); // Para capturar el salto de línea anterior
    getchar(); // Espera a que el usuario presione Enter
}

void limpiarConsola() {
    // Limpiar la consola de acuerdo al sistema operativo
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void mostrarMenu() {
    printf("1- Vaciar Archivos\n");
    printf("2- Cargar Provincias\n");
    printf("3- Cargar Productos\n");
    printf("4- Cargar Ventas\n");
    printf("5- Listado\n");
    printf("0- Salir\n");
}

int main() {
    int opcion;
    while (1) {
        limpiarConsola(); // Limpiar la consola al mostrar el menú
        mostrarMenu();
        printf("Seleccione una opcion: ");
        scanf("%d", &opcion);

        switch(opcion) {
            case 1:
                vaciarArchivos();
                break;
            case 2:
                cargarProvincias();
                break;
            case 3:
                cargarProductos();
                break;
            case 4:
                cargarVentas();
                break;
            case 5:
                mostrarListado();
                break;
            case 0:
                exit(0);
            default:
                printf("Opción inválida. Intente de nuevo.\n");
                getchar(); // Para consumir el salto de línea
                printf("Presione Enter para continuar...\n");
                getchar(); // Espera a que el usuario presione Enter
        }
    }
    return 0;
}
