#include "comunicacion.h"

void hilo_cpu(){
    
    while(1){
        log_info(logger, "Esperando mensaje de CPU...");
        uint8_t header = stream_recv_header(conexion_con_cpu);
        log_error(logger, "CPU envio un header incorrecto: %d", header);
        sleep(1);
        if(header==MOV_IN)
        {
            log_info(logger, "Mi cod de op es: %d", header);
            uint32_t cantBytes;
            uint32_t dir_fisica;
            uint32_t pid;
            char *dato;
            pedidoLectura_CPU(&cantBytes, &dir_fisica , &pid);
            sleep((configuracionMemoria->retardo_memoria/1000));
            dato =malloc(cantBytes);
            leer_CPU(dato, dir_fisica,cantBytes,pid);
            enviarDato_CPU(cantBytes, dato);
            free(dato);
        }

        else if (header==MOV_OUT)
        {
            log_info(logger, "Mi cod de op es: %d", header);
                uint32_t cantBytes;
                uint32_t dir_fisica;
                uint32_t pid;
                char *dato;
            pedidoEscritura_CPU(&cantBytes,&dir_fisica,dato,pid);
            sleep((configuracionMemoria->retardo_memoria/1000));
            escribir_CPU(dato,dir_fisica,cantBytes ,pid);
            ok_CPU();
        }
        
        else
            log_error(logger, "CPU envio un header incorrecto: %d", header);
    }	
}


void leer_CPU(char *dato, uint32_t dirF,uint32_t cantBytes, uint32_t pid){
	

    memcpy(dato, espacioUsuario+dirF, cantBytes);

    log_debug(logger, "PID:%d - Acción: LEER - Dirección física: %d - Tamaño: %ld - Origen: CPU",pid,dirF,cantBytes);

}

void escribir_CPU(char* dato, uint32_t dirF,uint32_t cantBytes,uint32_t pid ){

    memcpy(espacioUsuario+dirF,dato,cantBytes);

    log_debug(logger, "PID:%d - Acción: ESCRITURA - Dirección física: %d - Tamaño: %d - Origen: CPU",pid,dirF,cantBytes);
}

void pedidoLectura_CPU(uint32_t *cantBytes, uint32_t *dir_fisica ,uint32_t *pid)
{
    
    t_buffer* buffer = buffer_create();

    stream_recv_buffer(conexion_con_cpu,buffer);

    // DIRECCION FISICA DONDE ESCRIBIR
    buffer_unpack(buffer, dir_fisica, sizeof(uint32_t));
    // CANTIDAD DE BYTES
    buffer_unpack(buffer, cantBytes, sizeof(uint32_t));
    //PID
    buffer_unpack(buffer,pid,sizeof(uint32_t));
    
    buffer_destroy(buffer);
}

void pedidoEscritura_CPU(uint32_t* cantBytes ,uint32_t *dir_fisica,uint32_t* dato,uint32_t *pid){

    t_buffer* buffer = buffer_create();

    stream_recv_buffer(conexion_con_cpu,buffer);
    // CANTIDAD DE BYTES
    buffer_unpack(buffer, cantBytes, sizeof(uint32_t));    
    // DATO
    buffer_unpack(buffer, dato, *cantBytes);
    // DIRECCION FISICA DONDE ESCRIBIR
    buffer_unpack(buffer, dir_fisica, sizeof(uint32_t));
    //PID
    buffer_unpack(buffer,pid,sizeof(uint32_t));


    buffer_destroy(buffer);
    
}

void enviarDato_CPU( uint32_t cantBytes, char* dato){

    t_buffer* buffer =buffer_create();
   // buffer_pack(buffer,OK,sizeof(t_CPU_memoria));
    buffer_pack(buffer,cantBytes,sizeof(uint32_t)); //Creo que no iria
    buffer_pack(buffer, dato, cantBytes);
    stream_send_buffer(conexion_con_cpu, OK, buffer);
    buffer_destroy(buffer);
}

void ok_CPU(){
    t_buffer* buffer = buffer_create();
    stream_send_empty_buffer(conexion_con_cpu, OK);
    buffer_destroy(buffer);
}