#ifndef SHARED_UTILS_H
#define SHARED_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include <string.h>
#include <assert.h>
#include <semaphore.h>
#include <commons/string.h>
#include <pthread.h>
#include <time.h>

char *nombresCodigoOperaciones[] = {"MENSAJE", "PAQUETE", "NEW"};
char *nombresInstrucciones[] = {"F_READ", "F_WRITE", "SET", "MOV_IN", "MOV_OUT", "F_TRUNCATE", "F_SEEK", "CREATE_SEGMENT",
                                      "IO", "WAIT", "SIGNAL", "F_OPEN", "F_CLOSE", "DELETE_SEGMENT", "YIELD", "EXIT"};

typedef enum
{
	MENSAJE,
	PAQUETE,
	NEW
}op_code;

typedef enum {
	TERMINAR_CONSOLA,
} t_tipoMensaje;

typedef enum
{
    // 4 bytes
    AX,
    BX,
    CX,
    DX,
    // 8 bytes
    EAX,
    EBX,
    ECX,
    EDX,
    // 16 bytes
    RAX,
    RBX,
    RCX,
    RDX
} t_registro;

typedef enum
{
    // 3 parámetros
    F_READ,
    F_WRITE,
    // 2 parámetros
    SET,
    MOV_IN,
    MOV_OUT,
    F_TRUNCATE,
    F_SEEK,
    CREATE_SEGMENT,
    // 1 parámetros
    IO,
    WAIT,
    SIGNAL,
    F_OPEN,
    F_CLOSE,
    DELETE_SEGMENT,
    // 0 parámetros
    YIELD,
    EXIT,
} t_tipoInstruccion;

typedef struct
{
    t_tipoInstruccion tipo;
    t_registro registros[2];       // Puede que con 1 solo es suficiente
    uint32_t paramIntA, paramIntB; // En caso de que se deban guardar dos int
    // uint32_t longitudRecurso;   //Longitud de la palabra - Es necesario?
    char *recurso;                 // Disco, etc
    char *cadenaRegistro;          // Texto guardado en el registro
    char *nombreArchivo;
} __attribute__((packed)) t_instruccion; //esto tiene que ir?

typedef struct
{
	t_list *listaInstrucciones;
	uint32_t cantidadInstrucciones;
} __attribute__((packed)) t_instrucciones;

typedef struct
{
	unsigned char ax[4];
	unsigned char bx[4];
	unsigned char cx[4];
	unsigned char dx[4];
}t_registroC;

typedef struct
{
	unsigned char eax[8];
	unsigned char ebx[8];
	unsigned char ecx[8];
	unsigned char edx[8];
}t_registroE;

typedef struct
{
	unsigned char rax[16];
	unsigned char rbx[16];
	unsigned char rcx[16];
	unsigned char rdx[16];
}t_registroR;

typedef struct
{
	t_registro *registro;
	t_registroE *registroE;
	t_registroR *registroR;
}t_registrosCPU;


typedef struct
{
    uint32_t prueba;
}entrada_tablaDeSegmentos;

typedef struct
{
	int socket;
    uint32_t pid;
	uint32_t program_counter;
	t_instrucciones *instrucciones;
	t_list *tablaDeSegmentos;
	t_registrosCPU *registrosCPU;
}__attribute__((packed)) t_contextoEjecucion;



typedef struct
{
	uint32_t size; // Tamaño del payload
	void *stream;  // Payload
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer *buffer;
} t_paquete;

extern t_log* logger;

void* recibir_buffer(int*, int);

//int iniciar_servidor(void);
int iniciar_servidor(char *IP, char *PUERTO, t_log* logger);
int esperar_cliente(int, t_log*);
t_list* recibir_paquete(int);
void recibir_mensaje(int);
int recibir_operacion(int);
int crear_conexion(char *ip, char *puerto, t_log* logger);
void enviar_mensaje(char *mensaje, int socket_cliente);
t_paquete *crear_paquete(void);
t_paquete *crear_super_paquete(void);
void agregar_a_paquete(t_paquete *paquete, void *valor, int tamanio);
void enviar_paquete(t_paquete *paquete, int socket_cliente);
void liberar_conexion(int socket_cliente);
void eliminar_paquete(t_paquete *paquete);
//t_buffer *cargar_buffer_a_t_pcb(t_pcb t_pcb);
void cargar_buffer_a_paquete(t_buffer *buffer, int conexion);
//t_pcb *deserializar_pcb(t_buffer *buffer);
//void deserializar_paquete(int conexion);
//void serializarPCB(int socket, t_pcb *pcb, t_tipoMensaje tipoMensaje);
//void crearPaquete(t_buffer *buffer, t_tipoMensaje op, int unSocket);
//t_paqueteActual *recibirPaquete(int socket);
//t_pcb *deserializoPCB(t_buffer *buffer);
//int calcularSizeInfo(t_informacion* );
t_instrucciones recibir_informacion(int cliente_fd);
char* mi_funcion_compartida();

#endif