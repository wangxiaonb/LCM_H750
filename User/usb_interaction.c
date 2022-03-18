/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tx_api.h"

/* Private define ------------------------------------------------------------*/
#define CONTROL_LENGTH 4 * 2

/* Private typedef -----------------------------------------------------------*/

/* Extern variables and Function definitions---------------------------------------------------------*/
extern void ux_write(UCHAR *buffer, ULONG requested_length);
extern UINT ux_read(UCHAR *buffer, ULONG requested_length);

extern void Bulk_Transmit_HS(uint8_t *pBuf, uint32_t len);

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
// static __attribute__((section(".JpegPoolSection"))) __ALIGNED(4) uint32_t jpeg_buffer[JPEG_BUFFER_SIZE / 4];

// static uint8_t tx_stack_memory[TASK_STACK_SIZE];

// static TX_THREAD tx_task;

/* Private function prototypes -----------------------------------------------*/

/* Function definitions -----------------------------------------------*/

void usb_hs_recv(uint8_t *pBuf, uint32_t len)
{
    // Bulk_Transmit_HS((UCHAR *)0x24000000, 0x100);
    Bulk_Transmit_HS((UCHAR *)pBuf, len);
    
    // uint16_t *pData16 = (uint16_t *)pBuf;
    // switch (pData16[0])
    // {
    // case 0: //read sensor reg
    //     pData16[2] = sensor_read(pData16[1]);
    //     Bulk_Transmit_HS((UCHAR *)pData16, CONTROL_LENGTH);
    //     break;
    // case 1: //write sensor reg
    //     sensor_write(pData16[1], pData16[2]);
    //     Bulk_Transmit_HS((UCHAR *)pData16, CONTROL_LENGTH);
    //     break;
    // case 2: //start continues capture
    //     Bulk_Transmit_HS((UCHAR *)pData16, CONTROL_LENGTH);
    //     dcmi_start();
    //     break;
    // case 3: //start snapshot capture
    //     Bulk_Transmit_HS((UCHAR *)pData16, CONTROL_LENGTH);
    //     HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, GPIO_PIN_SET);
    //     HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, GPIO_PIN_RESET);
    //     break;
    // case 4: //loop back
    //     Bulk_Transmit_HS((UCHAR *)pData16, CONTROL_LENGTH);
    //     break;
    // case 10:
    //     *(float *)&pData16[2] = get_cpu_idle_rate();
    //     Bulk_Transmit_HS((UCHAR *)pData16, CONTROL_LENGTH);
    //     break;
    // case 0xaa:
    //     Bulk_Transmit_HS((UCHAR *)0x24000000, 640 * 480 / 2 * 3);
    //     Bulk_Transmit_HS((UCHAR *)0x30000000, 640 * 480 / 2);
    //     break;
    // case 0xbb:
    //     Bulk_Transmit_HS((UCHAR *)0x24000000, (640 * 480));
    //     break;
    // case 0xFF:
    //     Bulk_Transmit_HS((UCHAR *)pData16, CONTROL_LENGTH);
    //     NVIC_SystemReset();
    //     Error_Handler();
    //     break;
    // }
}

void usb_hs_ctrl(uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, uint8_t *pBuf, uint32_t len)
{
    pBuf[0] = bRequest;
    pBuf[1] = wIndex;
    pBuf[2] = 0;
    pBuf[3] = 0;
    switch (bRequest)
    {
    case 0: //read sensor reg
        break;
    case 1: //write sensor reg
        break;
    case 2: //start continues capture
        break;
    case 3: //start snapshot capture
        // HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, GPIO_PIN_SET);
        // for (int i = 0; i < 100; i++)
        //     asm("nop");
        // HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, GPIO_PIN_RESET);
        break;
    case 4: //loop back
        break;
    case 10:
        break;
    case 0xaa:
        // Bulk_Transmit_HS((UCHAR *)0x24000000, 640 * 480 / 2 * 3);
        // Bulk_Transmit_HS((UCHAR *)0x30000000, 640 * 480 / 2);
        break;
    case 0xFF:
        NVIC_SystemReset();
        Error_Handler();
        break;
    }
}