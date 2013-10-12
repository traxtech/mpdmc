#ifndef MOSQUITTO_H
#define MOSQUITTO_H

#include "dataset.h"

int mosquitto_init();
void mosquitto_update(dataset_t *ds);
void mosquitto_close();

#endif // MOSQUITTO_H
