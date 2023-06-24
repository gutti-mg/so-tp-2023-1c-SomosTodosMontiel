#include <memoria.h>
#include <funciones.h>


void iniciar_servidor_con_karnel()
{
    int server_fd =iniciar_servidor(IPPORT_EFSSERVER,configuracionMemoria -> puerto_escucha);
    log_info(logger, "Servidor listo para recibir al kernel");

    socketKernel= esperar_cliente(server_fd);
    char *mensaje = recibirMensaje(socketKernel);

	log_info(logger, "Mensaje de confirmacion del Kernel: %s\n", mensaje);
   
    t_tipoInstruccion instruccion;

    while (1)
    {
    log_info(logger, "Estoy esperando paquete, soy memoria\n");
	
        uint8_t header = stream_recv_header(socketKernel);

        switch (header)
        {
        case CREATE_SEGMENT:
            log_info(logger, "Mi cod de op es: %d", header);
            t_segmento *segmentoACrear=malloc(sizeof(t_segmento));

            //-aca recibo el tamanio del segmento, la id, y la base 
            segmentoACrear =recibirIDTam(segmentoACrear->id_segmento,segmentoACrear->tamanio);

            //-creo el segemnto y lo meto en la lista  
            uint32_t baseMandar=comprobar_Creacion_de_Seg(lista_de_segmentos,lista_de_huecos,   segmentoACrear->id_segmento,segmentoACrear->tamanio);

                if(baseMandar!=-1 && baseMandar!=-2){
                      t_segmento* segmento = segmentoCrear(segmentoACrear->id_segmento,baseMandar,segmentoACrear->tamanio);
                 // Actualizar tabla de segmentos
                list_add(lista_de_segmentos,segmento);
                }

            //-devuelvo la base actualizada, 
            mandarLaBase(baseMandar);

            break;

        case DELETE_SEGMENT:

            log_info(logger, "Mi cod de op es: %d", header);

            //recibo el segmento a eliminar 
            uint32_t id;
            uint32_t id_recivida; recibirID(id);
            t_segmento *segmentoEncontrado=buscarSegmentoPorId(lista_de_segmentos,id_recivida);
            list_remove_element(lista_de_segmentos,segmentoEncontrado);

            //devuelvo la lista nueva con el segemto elimado     
            mandarLista();//Falta terminar esto 
            
            break;
     

        case 20:

            log_info(logger, "Mi cod de op es: %d", header);

            //Recibo el header.#pragma endregionCompacto y devuelvo la lista actualizada    
            
            
            break;
        default:
            log_info(logger, "Instruccion no valida ERROR: %d", header);

            break;
        }
        free(header);
    }
    
}
t_segmento* buscarSegmentoPorId(t_list* segmentos, uint32_t id) {
    for (int i = 0; i < list_size(segmentos); i++) {
        t_segmento* segmento = (t_segmento*)list_get(segmentos, i);
        
        if (segmento->id_segmento == id) {
            return segmento;
        }
    }
    
    return NULL;  // Si no se encuentra el segmento, se devuelve NULL
}



t_segmento *recibirIDTam(uint32_t id, uint32_t tam){

    t_buffer* buffer = buffer_create();

     stream_recv_buffer(socketKernel,buffer);

    t_segmento *seg =malloc(sizeof(t_segmento));

    //ID del segmento a crear
    seg->id_segmento=buffer_unpack(buffer,&id,sizeof(uint32_t));
    
    //Tamaño del segmento a crear
	seg->tamanio=buffer_unpack(buffer, &tam, sizeof(uint32_t));

    buffer_destroy(buffer);

    return seg;

}

void mandarLaBase(uint32_t baseMandar){

    t_buffer* buffer = buffer_create();

    t_Kernel_Memoria instruccion;

    if(baseMandar !=-1 && baseMandar!=-2){
        instruccion = BASE;
        buffer_pack(buffer,&instruccion,sizeof(t_Kernel_Memoria));
        buffer_pack(buffer,&baseMandar,sizeof(uint32_t));
    }
    if(baseMandar ==-1 ){
        instruccion = SIN_MEMORIA;
        buffer_pack(buffer,&instruccion,sizeof(t_Kernel_Memoria));
        //stream_send_buffer(socketKernel, &instruccion);
    }
    if(baseMandar ==-2 ){
        instruccion = NECESITO_COMPACTAR;
        buffer_pack(buffer,&instruccion,sizeof(t_Kernel_Memoria));
        //stream_send_buffer(socketKernel, &instruccion);
    }

    stream_send_buffer(socketKernel, &instruccion, buffer);
    buffer_destroy(buffer);
}


uint32_t recibirID(uint32_t id){

    t_buffer *buffer =buffer_create();

    uint32_t id_recibida;

    stream_recv_buffer(socketKernel,buffer);

    id_recibida=buffer_unpack(buffer,&id,sizeof(uint32_t));
    buffer_destroy(buffer);

    return id_recibida;


}