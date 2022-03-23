#include <string.h>

#include "board.h"
#include "timex.h"
#include "ztimer.h"

/* Add sx127x radio driver necessary includes here */
#include "sx127x.h"
#include "sx127x_netdev.h"
#include "sx127x_params.h"

/* Nécessaire pour faire un thread d'écoute */
#include "thread.h"
#define RECV_MSG_QUEUE                   (4U)
static msg_t _recv_queue[RECV_MSG_QUEUE];
static char _recv_stack[THREAD_STACKSIZE_DEFAULT];

/* Add lps331ap related include here */
#include "lpsxxx.h"
#include "lpsxxx_params.h"

/* Add loramac necessary includes here */
#include "net/loramac.h"     /* core loramac definitions */
#include "semtech_loramac.h" /* package API */

/* Declare the sx127x radio driver descriptor globally here */
static sx127x_t sx127x;      /* The sx127x radio driver descriptor */

/* Declare the loramac descriptor globally here */
static semtech_loramac_t loramac;  /* The loramac stack descriptor */

/* Device and application parameters required for OTAA activation here */
static const uint8_t appeui[LORAMAC_APPEUI_LEN] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static const uint8_t deveui[LORAMAC_DEVEUI_LEN] = { 0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x04, 0xD3, 0x00 };
static const uint8_t appkey[LORAMAC_APPKEY_LEN] = { 0x4E, 0x61, 0x72, 0x3F, 0x3B, 0x57, 0xBB, 0x14, 0xD8, 0xFF, 0xB5, 0x80, 0x0F, 0x9B, 0xEB, 0x3F };

/* Declare the lps331ap device variable here */
static lpsxxx_t lpsxxx;

static void *_recv(void *arg)
{
   msg_init_queue(_recv_queue, RECV_MSG_QUEUE);
   (void)arg;
   while (1) {
       /* blocks until a message is received */
       semtech_loramac_recv(&loramac);
       loramac.rx_data.payload[loramac.rx_data.payload_len] = 0;
       printf("Data received: %s, port: %d\n",
              (char *)loramac.rx_data.payload, loramac.rx_data.port);
   }
   return NULL;
}

int main(void)
{
    
    /* Initialize the lps331ap sensor here */
    lpsxxx_init(&lpsxxx, &lpsxxx_params[0]);
    
    /* initialize the radio driver */
    sx127x_setup(&sx127x, &sx127x_params[0], 0);
    loramac.netdev = &sx127x.netdev;
    loramac.netdev->driver = &sx127x_driver;

    /* initialize loramac stack */
    semtech_loramac_init(&loramac);

    /* configure the device parameters */
    semtech_loramac_set_deveui(&loramac, deveui);
    semtech_loramac_set_appeui(&loramac, appeui);
    semtech_loramac_set_appkey(&loramac, appkey);

    /* change datarate to DR5 (SF7/BW125kHz) */
    semtech_loramac_set_dr(&loramac, 5);
    
    /* start the OTAA join procedure */
    if (semtech_loramac_join(&loramac, LORAMAC_JOIN_OTAA) != SEMTECH_LORAMAC_JOIN_SUCCEEDED) {
        puts("Join procedure failed ! Nous ne somme pas content !");
        return 1;
    }
    puts("Join procedure succeeded ! Nous sommes content !");
    
    thread_create(_recv_stack, sizeof(_recv_stack),
           THREAD_PRIORITY_MAIN - 1, 0, _recv, NULL, "recv thread");

    while (1) {
        
        uint16_t pres = 0;
        lpsxxx_read_pres(&lpsxxx, &pres);
        pres = pres;
        char message[64];
        snprintf(message, sizeof(message), "Pression: %ummHg", pres);

        /* send the message here */
        if (semtech_loramac_send(&loramac,
                                 (uint8_t *)message, strlen(message)) != SEMTECH_LORAMAC_TX_DONE) {
            printf("Cannot send message '%s'\n", message);
        }
        else {
            printf("Message '%s' sent\n", message);
        }

        /* wait 20 seconds between each message */
        ztimer_sleep(ZTIMER_MSEC, 20 * MS_PER_SEC);
    }

    return 0; /* should never be reached */
}
