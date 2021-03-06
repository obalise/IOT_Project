/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Wakaama LwM2M Client CLI support
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 * @}
 */

#include "thread.h"
#include "timex.h"
#include "ztimer.h"
#include "kernel_defines.h"

#include "lwm2m_client.h"
#include "lwm2m_client_objects.h"
#include "lwm2m_platform.h"

/* Include lpsxxx headers */


/* Include temperature object headers */


/* Declare lpsxxx_t sensor variable (globally) */


/* Define number of LwM2M objects */
#define OBJ_COUNT (3)


/* Declare LwM2M temperature object instance */


/* Define temperature sensor read interval (seconds) */


/* Define temperature sensor reader thread priority.
 * main = 7 < LwM2M client = 6 < radio at86rf2xx = 2 
 */


/* Declare thread stack size. */



uint8_t connected = 0;
lwm2m_object_t *obj_list[OBJ_COUNT];
static lwm2m_client_data_t client_data;


/* 
 * Thread that reads the temperature.
 * Update LwM2M temperature object instance and notify observers.
 */



void lwm2m_cli_init(void)
{
    /* this call is needed before creating any objects */
    lwm2m_client_init(&client_data);

    /* add objects that will be registered */
    obj_list[0] = lwm2m_client_get_security_object(&client_data);
    obj_list[1] = lwm2m_client_get_server_object(&client_data);
    obj_list[2] = lwm2m_client_get_device_object(&client_data);

    /* add temperature object */


    if (!obj_list[0] || !obj_list[1] || !obj_list[2]) {
        puts("Could not create mandatory objects");
    }
}

int lwm2m_cli_cmd(int argc, char **argv)
{
    if (argc == 1) {
        goto help_error;
    }

    if (!strcmp(argv[1], "start")) {
        /* run the LwM2M client */
        if (!connected && lwm2m_client_run(&client_data, obj_list, OBJ_COUNT)) {
            connected = 1;

            /* Initialize and enable the lps331ap device */


            /* Get temperature object instance */


            /* Create temperature sensor reader thread */


        }
        return 0;
    }

    if (IS_ACTIVE(DEVELHELP) && !strcmp(argv[1],"mem")) {
        lwm2m_tlsf_status();
        return 0;
    }

help_error:
    if (IS_ACTIVE(DEVELHELP)) {
        printf("usage: %s <start|mem>\n", argv[0]);
    }
    else {
        printf("usage: %s <start>\n", argv[0]);
    }

    return 1;
}
