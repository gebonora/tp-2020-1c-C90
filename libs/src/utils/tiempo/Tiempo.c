//
// Created by Alan Zhao on 29/04/2020.
//

#include "utils/tiempo/Tiempo.h"

time_t ahoraEnTimeT(){
    return time(NULL);
}

struct timeval ahoraEnTimeval() {
    struct timeval tiempoFinal;
    gettimeofday(&tiempoFinal, NULL);
    return tiempoFinal;
}

double diferenciaTimevalsEnMillis(struct timeval tiempoFinal, struct timeval tiempoInicio) {
    return milisegundos(tiempoFinal) - milisegundos(tiempoInicio);
}

double milisegundosHastaAhora(struct timeval tiempoInicio) {
    return milisegundos(ahoraEnTimeval()) - milisegundos(tiempoInicio);
}

double milisegundos(struct timeval tiempo) {
    return (tiempo.tv_sec * 1000) + (tiempo.tv_usec / 1000.0);
}

char* current_date_time_as_string() {
	// variables to store date and time components
	int hours, minutes, seconds, day, month, year;

	// time_t is arithmetic time type
	time_t now;

	// Obtain current time
	// time() returns the current time of the system as a time_t value
	time(&now);

	// localtime converts a time_t value to calendar time and
	// returns a pointer to a tm structure with its members
	// filled with the corresponding values
	struct tm *local = localtime(&now);

	hours = local->tm_hour;	  	// get hours since midnight (0-23)
	minutes = local->tm_min;	 	// get minutes passed after the hour (0-59)
	seconds = local->tm_sec;	 	// get seconds passed after minute (0-59)

	day = local->tm_mday;			// get day of month (1 to 31)
	month = local->tm_mon + 1;   	// get month of year (0 to 11)
	year = local->tm_year + 1900;	// get year since 1900

	// return current date time with format dd/MM/YYYY HH:mm:ss
	return string_from_format("%02d/%02d/%d %02d:%02d:%02d", day, month, year, hours, minutes, seconds);
}
