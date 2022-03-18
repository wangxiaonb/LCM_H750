/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usbd_cdc_if.c
  * @version        : v1.0_Cube
  * @brief          : Usb device for Virtual Com Port.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "usbd_cdc_if.h"

/* USER CODE BEGIN INCLUDE */
#include "tx_api.h"
/* USER CODE END INCLUDE */

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
  uint8_t *pBuf;
  uint32_t len;
} USB_DATA;

/* Private define ------------------------------------------------------------*/
#define TASK_STACK_SIZE 1024
#define DEFAULT_THREAD_PRIO 20
#define DEFAULT_PREEMPTION_THRESHOLD DEFAULT_THREAD_PRIO
/* Private macro -------------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
static uint8_t tx_stack_memory_fs[TASK_STACK_SIZE];
static uint8_t tx_stack_memory_hs[TASK_STACK_SIZE];
static TX_THREAD tx_task_fs;
static TX_THREAD tx_task_hs;
static TX_SEMAPHORE tx_sem_usb_hs;
static TX_QUEUE queue_usb_fs;
static TX_QUEUE queue_usb_hs;
static uint32_t queue_buffer[sizeof(USB_DATA) * 4];
static uint32_t queue_buffer[sizeof(USB_DATA) * 4];

static TX_SEMAPHORE sem_usb_fs;

uint8_t UserRxBufferFS[APP_RX_DATA_SIZE];
// uint8_t UserTxBufferFS[APP_TX_DATA_SIZE];
uint8_t UserRxBufferHS[APP_RX_DATA_SIZE];
// uint8_t UserTxBufferHS[APP_TX_DATA_SIZE];

/* External variables ---------------------------------------------------------*/
extern USBD_HandleTypeDef hUsbDeviceFS;
extern USBD_HandleTypeDef hUsbDeviceHS;

/* Private functions ---------------------------------------------------------*/
static int8_t CDC_Init_FS(void);
static int8_t CDC_DeInit_FS(void);
static int8_t CDC_Control_FS(uint8_t cmd, uint8_t *pbuf, uint16_t length);
static int8_t CDC_Receive_FS(uint8_t *pbuf, uint32_t *Len);
static int8_t CDC_TransmitCplt_FS(uint8_t *pbuf, uint32_t *Len, uint8_t epnum);

static int8_t Bulk_Init_HS(void);
static int8_t Bulk_DeInit_HS(void);
static int8_t Bulk_Control_HS(uint8_t cmd, uint8_t *pbuf, uint16_t length);
static int8_t Bulk_Receive_HS(uint8_t *pbuf, uint32_t *Len);
static int8_t Bulk_TransmitCplt_HS(uint8_t *pbuf, uint32_t *Len, uint8_t epnum);

/* USER CODE BEGIN PRIVATE_FUNCTIONS_DECLARATION */
__weak void usb_fs_recv(uint8_t *pBuf, uint32_t len)
{
  CDC_Transmit_FS(pBuf, len);

  // tx_semaphore_get(&sem_usb_fs, TX_WAIT_FOREVER);
  // (void)USBD_LL_Transmit(&hUsbDeviceFS, CDC_IN_EP, pBuf, len);
}

static void usb_fs_task(ULONG arg)
{
  USB_DATA data;
  while (1)
  {
    tx_queue_receive(&queue_usb_fs, (VOID *)&data, TX_WAIT_FOREVER);
    usb_fs_recv(data.pBuf, data.len);
  }
}

__weak void usb_hs_recv(uint8_t *pBuf, uint32_t len)
{
  Bulk_Transmit_HS(pBuf, len);
}

static void usb_hs_task(ULONG arg)
{
  USB_DATA data;
  while (1)
  {
    tx_queue_receive(&queue_usb_hs, (VOID *)&data, TX_WAIT_FOREVER);
    usb_hs_recv(data.pBuf, data.len);
  }
}

void usb_tx_init()
{
  tx_semaphore_create(&sem_usb_fs, "sem_usb_fs", 1);

  tx_queue_create(&queue_usb_fs, "queue_usb_fs", 4, (VOID *)queue_buffer, sizeof(queue_buffer));
  if (TX_SUCCESS != tx_thread_create(&tx_task_fs, "tx_task_fs",
                                     usb_fs_task,
                                     0,
                                     tx_stack_memory_fs,
                                     TASK_STACK_SIZE,
                                     DEFAULT_THREAD_PRIO,
                                     DEFAULT_PREEMPTION_THRESHOLD,
                                     TX_NO_TIME_SLICE,
                                     TX_AUTO_START))
  {
    Error_Handler();
  }

  tx_semaphore_create(&tx_sem_usb_hs, "tx_sem_usb_hs", 1);
  tx_queue_create(&queue_usb_hs, "queue_usb_hs", 4, (VOID *)queue_buffer, sizeof(queue_buffer));
  if (TX_SUCCESS != tx_thread_create(&tx_task_hs, "tx_task_hs",
                                     usb_hs_task,
                                     0,
                                     tx_stack_memory_hs,
                                     TASK_STACK_SIZE,
                                     DEFAULT_THREAD_PRIO,
                                     DEFAULT_PREEMPTION_THRESHOLD,
                                     TX_NO_TIME_SLICE,
                                     TX_AUTO_START))
  {
    Error_Handler();
  }
}
/* USER CODE END PRIVATE_FUNCTIONS_DECLARATION */

USBD_CDC_ItfTypeDef USBD_Interface_fops_FS =
    {
        CDC_Init_FS,
        CDC_DeInit_FS,
        CDC_Control_FS,
        CDC_Receive_FS,
        CDC_TransmitCplt_FS};

USBD_CDC_ItfTypeDef USBD_Interface_fops_HS =
    {
        Bulk_Init_HS,
        Bulk_DeInit_HS,
        Bulk_Control_HS,
        Bulk_Receive_HS,
        Bulk_TransmitCplt_HS};

/* Private functions ---------------------------------------------------------*/
static int8_t CDC_Init_FS(void)
{
  /* USER CODE BEGIN 3 */
  /* Set Application Buffers */
  // USBD_CDC_SetTxBuffer(&hUsbDeviceFS, UserTxBufferFS, 0);
  USBD_CDC_SetRxBuffer(&hUsbDeviceFS, UserRxBufferFS);
  return (USBD_OK);
  /* USER CODE END 3 */
}

static int8_t CDC_DeInit_FS(void)
{
  /* USER CODE BEGIN 4 */
  return (USBD_OK);
  /* USER CODE END 4 */
}

static int8_t CDC_Control_FS(uint8_t cmd, uint8_t *pbuf, uint16_t length)
{
  /* USER CODE BEGIN 5 */
  switch (cmd)
  {
  case CDC_SEND_ENCAPSULATED_COMMAND:

    break;

  case CDC_GET_ENCAPSULATED_RESPONSE:

    break;

  case CDC_SET_COMM_FEATURE:

    break;

  case CDC_GET_COMM_FEATURE:

    break;

  case CDC_CLEAR_COMM_FEATURE:

    break;

    /*******************************************************************************/
    /* Line Coding Structure                                                       */
    /*-----------------------------------------------------------------------------*/
    /* Offset | Field       | Size | Value  | Description                          */
    /* 0      | dwDTERate   |   4  | Number |Data terminal rate, in bits per second*/
    /* 4      | bCharFormat |   1  | Number | Stop bits                            */
    /*                                        0 - 1 Stop bit                       */
    /*                                        1 - 1.5 Stop bits                    */
    /*                                        2 - 2 Stop bits                      */
    /* 5      | bParityType |  1   | Number | Parity                               */
    /*                                        0 - None                             */
    /*                                        1 - Odd                              */
    /*                                        2 - Even                             */
    /*                                        3 - Mark                             */
    /*                                        4 - Space                            */
    /* 6      | bDataBits  |   1   | Number Data bits (5, 6, 7, 8 or 16).          */
    /*******************************************************************************/
  case CDC_SET_LINE_CODING:

    break;

  case CDC_GET_LINE_CODING:

    break;

  case CDC_SET_CONTROL_LINE_STATE:

    break;

  case CDC_SEND_BREAK:

    break;

  default:
    break;
  }

  return (USBD_OK);
  /* USER CODE END 5 */
}

static int8_t CDC_Receive_FS(uint8_t *Buf, uint32_t *Len)
{
  /* USER CODE BEGIN 6 */
  USBD_CDC_SetRxBuffer(&hUsbDeviceFS, &Buf[0]);
  USBD_CDC_ReceivePacket(&hUsbDeviceFS);

  USB_DATA data = {Buf, *Len};
  tx_queue_send(&queue_usb_fs, &data, TX_NO_WAIT);

  // CDC_Transmit_FS(Buf, *Len);
  // (void)USBD_LL_Transmit(&hUsbDeviceFS, CDC_IN_EP, Buf, *Len);

  return (USBD_OK);
  /* USER CODE END 6 */
}

uint8_t CDC_Transmit_FS(uint8_t *Buf, uint32_t Len)
{
  uint8_t result = USBD_OK;
  /* USER CODE BEGIN 7 */
  tx_semaphore_get(&sem_usb_fs, TX_WAIT_FOREVER);
  USBD_CDC_SetTxBuffer(&hUsbDeviceFS, Buf, Len);
  result = USBD_CDC_TransmitPacket(&hUsbDeviceFS);
  /* USER CODE END 7 */
  return result;
}

static int8_t CDC_TransmitCplt_FS(uint8_t *Buf, uint32_t *Len, uint8_t epnum)
{
  uint8_t result = USBD_OK;
  /* USER CODE BEGIN 13 */
  UNUSED(Buf);
  UNUSED(Len);
  UNUSED(epnum);

  tx_semaphore_put(&sem_usb_fs);
  /* USER CODE END 13 */
  return result;
}

/*******************************************************************/
static int8_t Bulk_Init_HS(void)
{
  /* USER CODE BEGIN 8 */
  return (USBD_OK);
  /* USER CODE END 8 */
}

static int8_t Bulk_DeInit_HS(void)
{
  /* USER CODE BEGIN 9 */
  return (USBD_OK);
  /* USER CODE END 9 */
}

static int8_t Bulk_Control_HS(uint8_t cmd, uint8_t *pbuf, uint16_t length)
{
  /* USER CODE BEGIN 10 */
  return (USBD_OK);
  /* USER CODE END 10 */
}

static int8_t Bulk_Receive_HS(uint8_t *Buf, uint32_t *Len)
{
  /* USER CODE BEGIN 11 */
  (void)USBD_LL_PrepareReceive(&hUsbDeviceHS, CDC_OUT_EP, UserRxBufferHS, CDC_DATA_HS_OUT_PACKET_SIZE);
  USB_DATA data = {Buf, *Len};
  tx_queue_send(&queue_usb_hs, &data, TX_NO_WAIT);

  return (USBD_OK);
  /* USER CODE END 11 */
}

uint8_t Bulk_Transmit_HS(uint8_t *Buf, uint32_t Len)
{
  uint8_t result = USBD_BUSY;
  /* USER CODE BEGIN 12 */
  tx_semaphore_get(&tx_sem_usb_hs, TX_WAIT_FOREVER);
  (void)USBD_LL_Transmit(&hUsbDeviceHS, CDC_IN_EP, Buf, Len);
  result = USBD_OK;
  /* USER CODE END 12 */
  return result;
}

static int8_t Bulk_TransmitCplt_HS(uint8_t *Buf, uint32_t *Len, uint8_t epnum)
{
  uint8_t result = USBD_OK;
  /* USER CODE BEGIN 14 */
  UNUSED(Buf);
  UNUSED(Len);
  UNUSED(epnum);

  tx_semaphore_put(&tx_sem_usb_hs);
  /* USER CODE END 14 */
  return result;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
