#include "kernel.h"

void implementar_hrrn(){
	t_pcb *pcb = algoritmo_hrrn(LISTA_READY); //Ordena la lista y obtiene el elemento que corresponde
	//log_info(logger, "Agregando UN pcb a lista exec");
	log_error(logger, "cantidad de pcbs en ready luego de hrrn: %d", list_size(LISTA_READY));
	log_debug(logger, "pasando a exec el socket %d", pcb->contexto->socket);
	pasar_a_exec(pcb);
	// Cambio de estado
	log_info(logger, "Cambio de Estado: PID %d - Estado Anterior: READY , Estado Actual: EXEC", pcb->contexto->pid);
	sem_post(&pasar_pcb_a_CPU);
}

int obtener_espera(t_pcb* pcb){
	struct timespec end;
    clock_gettime(CLOCK_REALTIME, &end);
	//                   momento actual               -                  momento de entrada a ready
	return (end.tv_sec * 1000 + end.tv_nsec / 1000000)-(pcb->llegadaReady.tv_sec * 1000 + pcb->llegadaReady.tv_nsec / 1000000);
}

double obtener_estimacion(t_pcb* pcb){
	return (pcb->estimacion_anterior) * (configuracionKernel->HRRN_ALFA) + (pcb->real_anterior) * (1-configuracionKernel->HRRN_ALFA);
}

double calcular_HRRN(t_pcb* pcb){
	// espera + estimacionCPU
	//------------------------
	//    estimacionCPU
	return (obtener_espera(pcb)+obtener_estimacion(pcb))/(obtener_estimacion(pcb));
}

t_pcb* mayorHRRN(t_pcb* unaPCB, t_pcb* otraPCB){
	double unHRRN   = calcular_HRRN(unaPCB);
    double otroHRRN = calcular_HRRN(otraPCB);
    return unHRRN <= otroHRRN
               ? otraPCB //Devuelvo la PCB con el HRRN mayor si se cumple la condicion 
               : unaPCB;
}

t_pcb *algoritmo_hrrn(t_list *LISTA_READY){
	t_pcb *pcb;
	log_error(logger, "cantidad de pcbs en ready hrrn: %d", list_size(LISTA_READY));
	if (list_size(LISTA_READY) == 1) //Si solo hay uno, lo saco por fifo (el 1ro de la lista)
        pcb = (t_pcb *)list_remove(LISTA_READY, 0);
    else if (list_size(LISTA_READY) > 1){ //Si hay mas tengo que obtener el que tenga el mayor HRRN.
		pcb = list_get_maximum(LISTA_READY, (void*)mayorHRRN);
		if(!list_remove_element(LISTA_READY, pcb))
			log_error(logger, "no se pudo eliminar la pcb por hrrn");
	}
	return pcb;
}