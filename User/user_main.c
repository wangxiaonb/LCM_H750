/* Includes ------------------------------------------------------------------*/
#include "tx_api.h"

/* Private includes ----------------------------------------------------------*/
// extern void cpu_idle_rate_init();
extern void usb_bulk_init();
extern void usb_tx_init();
// extern void User_Shell_Init();

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

VOID tx_application_define(VOID *first_unused_memory)
{
    // cpu_idle_rate_init();
//     usb_bulk_init();
    usb_tx_init();
    // User_Shell_Init();
}