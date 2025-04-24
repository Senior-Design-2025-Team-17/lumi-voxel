/**
  ******************************************************************************
  * @file    App/gatt_db.c
  * @author  SRA Application Team
  * @brief   Functions to build GATT DB and handle GATT events.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#include "gatt_db.h"
#include "bluenrg1_aci.h"
#include "bluenrg1_hci_le.h"

#define COPY_UUID_128(uuid_struct, uuid_15, uuid_14, uuid_13, uuid_12, uuid_11, uuid_10, uuid_9, uuid_8, uuid_7, uuid_6, uuid_5, uuid_4, uuid_3, uuid_2, uuid_1, uuid_0) \
  do {\
  	uuid_struct.uuid128[0] = uuid_0; uuid_struct.uuid128[1] = uuid_1; uuid_struct.uuid128[2] = uuid_2; uuid_struct.uuid128[3] = uuid_3; \
	uuid_struct.uuid128[4] = uuid_4; uuid_struct.uuid128[5] = uuid_5; uuid_struct.uuid128[6] = uuid_6; uuid_struct.uuid128[7] = uuid_7; \
	uuid_struct.uuid128[8] = uuid_8; uuid_struct.uuid128[9] = uuid_9; uuid_struct.uuid128[10] = uuid_10; uuid_struct.uuid128[11] = uuid_11; \
	uuid_struct.uuid128[12] = uuid_12; uuid_struct.uuid128[13] = uuid_13; uuid_struct.uuid128[14] = uuid_14; uuid_struct.uuid128[15] = uuid_15; \
	}while(0)

uint16_t sampleServHandle, sampleTXCharHandle, sampleRXCharHandle;
uint16_t TrianglemeshServHandle, TrianglemeshTxCharHandle, TriangleMeshRxVertsCharHandle,TriangleMeshRxTrisCharHandle,TriangleMeshRxReadyCharHandle;
uint16_t TransformServHandle, TransformTxCharHandle, TransformRxArrCharHandle,TransformRxReadyCharHandle;

/* UUIDs */
Service_UUID_t service_uuid;
Char_UUID_t char_uuid;

tBleStatus Add_Triangle_Mesh_Service(void)
{
	uint8_t ret;
	/**
	* Number of attribute records that can be added to this service
	* For this service it is given by:
	* 1 (fixed value) + 3 (for characteristic with CHAR_PROP_NOTIFY) + 2 (for characteristic with CHAR_PROP_WRITE)
	*/
	uint8_t max_attribute_records = 1+3+2+2+2;

	/*
	UUIDs:
	0A16D1E0-1B1B-11F0-9E92-0800200C9A66 - Main characteristic
	0A16D1E1-1B1B-11F0-9E92-0800200C9A66 - Tx characteristic. Not used
	0A16D1E2-1B1B-11F0-9E92-0800200C9A66 - Triangle Mesh verticies characteristic
	0A16D1E3-1B1B-11F0-9E92-0800200C9A66 - Triangle Mesh triangles characteristic
	0A16D1E4-1B1B-11F0-9E92-0800200C9A66 - Traingle Mesh read ready
	*/

	const uint8_t uuid[16] = {0x66,0x9a,0x0c,0x20,0x00,0x08,0x92,0x9e,0xf0,0x11,0x1b,0x1b,0xe0,0xd1,0x16,0x0a};
	const uint8_t charUuidTX[16] = {0x66,0x9a,0x0c,0x20,0x00,0x08,0x92,0x9e,0xf0,0x11,0x1b,0x1b,0xe1,0xd1,0x16,0x0a};
	const uint8_t charUuidRxVerts[16] = {0x66,0x9a,0x0c,0x20,0x00,0x08,0x92,0x9e,0xf0,0x11,0x1b,0x1b,0xe2,0xd1,0x16,0x0a};
	const uint8_t charUuidRxTris[16] = {0x66,0x9a,0x0c,0x20,0x00,0x08,0x92,0x9e,0xf0,0x11,0x1b,0x1b,0xe3,0xd1,0x16,0x0a};
	const uint8_t charUuidRxReady[16] = {0x66,0x9a,0x0c,0x20,0x00,0x08,0x92,0x9e,0xf0,0x11,0x1b,0x1b,0xe4,0xd1,0x16,0x0a};

	BLUENRG_memcpy(&service_uuid.Service_UUID_128, uuid, 16);

	ret = aci_gatt_add_service(UUID_TYPE_128, &service_uuid, PRIMARY_SERVICE, max_attribute_records, &TrianglemeshServHandle);
	if (ret != BLE_STATUS_SUCCESS) goto fail;

	BLUENRG_memcpy(&char_uuid.Char_UUID_128, charUuidTX, 16);
	ret =  aci_gatt_add_char(TrianglemeshServHandle, UUID_TYPE_128, &char_uuid, CHAR_VALUE_LENGTH, CHAR_PROP_NOTIFY, ATTR_PERMISSION_NONE, 0,
				16, 1, &TrianglemeshTxCharHandle);
	if (ret != BLE_STATUS_SUCCESS) goto fail;

	BLUENRG_memcpy(&char_uuid.Char_UUID_128, charUuidRxVerts, 16);
	ret =  aci_gatt_add_char(TrianglemeshServHandle, UUID_TYPE_128, &char_uuid, CHAR_VALUE_LENGTH, CHAR_PROP_WRITE|CHAR_PROP_WRITE_WITHOUT_RESP, ATTR_PERMISSION_NONE, 0,
				16, 1, &TriangleMeshRxVertsCharHandle);
	if (ret != BLE_STATUS_SUCCESS) goto fail;

	BLUENRG_memcpy(&char_uuid.Char_UUID_128, charUuidRxTris, 16);
	ret =  aci_gatt_add_char(TrianglemeshServHandle, UUID_TYPE_128, &char_uuid, CHAR_VALUE_LENGTH, CHAR_PROP_WRITE|CHAR_PROP_WRITE_WITHOUT_RESP, ATTR_PERMISSION_NONE, 0,
				16, 1, &TriangleMeshRxTrisCharHandle);
	if (ret != BLE_STATUS_SUCCESS) goto fail;

	BLUENRG_memcpy(&char_uuid.Char_UUID_128, charUuidRxReady, 16);
	ret =  aci_gatt_add_char(TrianglemeshServHandle, UUID_TYPE_128, &char_uuid, CHAR_VALUE_LENGTH, CHAR_PROP_WRITE|CHAR_PROP_WRITE_WITHOUT_RESP, ATTR_PERMISSION_NONE, GATT_NOTIFY_ATTRIBUTE_WRITE,
				16, 1, &TriangleMeshRxReadyCharHandle);
	if (ret != BLE_STATUS_SUCCESS) goto fail;

	PRINT_DBG("Triangle Mesh service added.\r\n");
	return BLE_STATUS_SUCCESS;

	fail:
	  PRINT_DBG("Error while adding Triangle Mesh service.\r\n");
	  return BLE_STATUS_ERROR ;
}
tBleStatus Add_Transform_Service(void)
{
	uint8_t ret;
	/**
	* Number of attribute records that can be added to this service
	* For this service it is given by:
	* 1 (fixed value) + 3 (for characteristic with CHAR_PROP_NOTIFY) + 2 (for characteristic with CHAR_PROP_WRITE)
	*/
	uint8_t max_attribute_records = 1+3+2+2;

	/*
	UUIDs:
	18472D90-1C98-11F0-B5EB-0800200C9A66 - Transform service
	18472D91-1C98-11F0-B5EB-0800200C9A66 - Tx Characteristic. Not used
	18472D92-1C98-11F0-B5EB-0800200C9A66 - Float array for transform
	18472D93-1C98-11F0-B5EB-0800200C9A66 - Transform ready
	*/

	const uint8_t uuid[16] = {0x66,0x9a,0x0c,0x20,0x00,0x08,0xeb,0xb5,0xf0,0x11,0x98,0x1c,0x90,0x2d,0x47,0x18};
	const uint8_t charUuidTX[16] = {0x66,0x9a,0x0c,0x20,0x00,0x08,0xeb,0xb5,0xf0,0x11,0x98,0x1c,0x91,0x2d,0x47,0x18};
	const uint8_t charUuidRXArr[16] = {0x66,0x9a,0x0c,0x20,0x00,0x08,0xeb,0xb5,0xf0,0x11,0x98,0x1c,0x92,0x2d,0x47,0x18};
	const uint8_t charUuidRXReady[16] = {0x66,0x9a,0x0c,0x20,0x00,0x08,0xeb,0xb5,0xf0,0x11,0x98,0x1c,0x93,0x2d,0x47,0x18};

	BLUENRG_memcpy(&service_uuid.Service_UUID_128, uuid, 16);

	ret = aci_gatt_add_service(UUID_TYPE_128, &service_uuid, PRIMARY_SERVICE, max_attribute_records, &TransformServHandle);
	if (ret != BLE_STATUS_SUCCESS) goto fail;

	BLUENRG_memcpy(&char_uuid.Char_UUID_128, charUuidTX, 16);
	ret =  aci_gatt_add_char(TransformServHandle, UUID_TYPE_128, &char_uuid, CHAR_VALUE_LENGTH, CHAR_PROP_NOTIFY, ATTR_PERMISSION_NONE, 0,
				16, 1, &TransformTxCharHandle);
	if (ret != BLE_STATUS_SUCCESS) goto fail;

	BLUENRG_memcpy(&char_uuid.Char_UUID_128, charUuidRXArr, 16);
	ret =  aci_gatt_add_char(TransformServHandle, UUID_TYPE_128, &char_uuid, CHAR_VALUE_LENGTH, CHAR_PROP_WRITE|CHAR_PROP_WRITE_WITHOUT_RESP, ATTR_PERMISSION_NONE, 0,
				16, 1, &TransformRxArrCharHandle);
	if (ret != BLE_STATUS_SUCCESS) goto fail;

	BLUENRG_memcpy(&char_uuid.Char_UUID_128, charUuidRXReady, 16);
	ret =  aci_gatt_add_char(TransformServHandle, UUID_TYPE_128, &char_uuid, CHAR_VALUE_LENGTH, CHAR_PROP_WRITE|CHAR_PROP_WRITE_WITHOUT_RESP, ATTR_PERMISSION_NONE, GATT_NOTIFY_ATTRIBUTE_WRITE,
				16, 1, &TransformRxReadyCharHandle);
	if (ret != BLE_STATUS_SUCCESS) goto fail;

	PRINT_DBG("Transform Service added.\r\n");
	return BLE_STATUS_SUCCESS;

	fail:
	  PRINT_DBG("Error while adding Transform service.\r\n");
	  return BLE_STATUS_ERROR ;

}

/*******************************************************************************
* Function Name  : Add_Sample_Service
* Description    : Add the 'Accelerometer' service.
* Input          : None
* Return         : Status.
*******************************************************************************/
uint8_t Add_Sample_Service(void)
{
  uint8_t ret;
  /**
   * Number of attribute records that can be added to this service
   * For this service it is given by:
   * 1 (fixed value) + 3 (for characteristic with CHAR_PROP_NOTIFY) + 2 (for characteristic with CHAR_PROP_WRITE)
   */
  uint8_t max_attribute_records = 1+3+2;

  /*
  UUIDs:
  D973F2E0-B19E-11E2-9E96-0800200C9A66
  D973F2E1-B19E-11E2-9E96-0800200C9A66
  D973F2E2-B19E-11E2-9E96-0800200C9A66
  */

  const uint8_t uuid[16] = {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0xe0,0xf2,0x73,0xd9};
  const uint8_t charUuidTX[16] = {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0xe1,0xf2,0x73,0xd9};
  const uint8_t charUuidRX[16] = {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0xe2,0xf2,0x73,0xd9};

  BLUENRG_memcpy(&service_uuid.Service_UUID_128, uuid, 16);

  ret = aci_gatt_add_service(UUID_TYPE_128, &service_uuid, PRIMARY_SERVICE, max_attribute_records, &sampleServHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;

  BLUENRG_memcpy(&char_uuid.Char_UUID_128, charUuidTX, 16);
  ret =  aci_gatt_add_char(sampleServHandle, UUID_TYPE_128, &char_uuid, CHAR_VALUE_LENGTH, CHAR_PROP_NOTIFY, ATTR_PERMISSION_NONE, 0,
                16, 1, &sampleTXCharHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;

  BLUENRG_memcpy(&char_uuid.Char_UUID_128, charUuidRX, 16);
  ret =  aci_gatt_add_char(sampleServHandle, UUID_TYPE_128, &char_uuid, CHAR_VALUE_LENGTH, CHAR_PROP_WRITE|CHAR_PROP_WRITE_WITHOUT_RESP, ATTR_PERMISSION_NONE, GATT_NOTIFY_ATTRIBUTE_WRITE,
                16, 1, &sampleRXCharHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;

  PRINT_DBG("Sample Service added.\r\nTX Char Handle %04X, RX Char Handle %04X\r\n", sampleTXCharHandle, sampleRXCharHandle);
  return BLE_STATUS_SUCCESS;

fail:
  PRINT_DBG("Error while adding Sample service.\r\n");
  return BLE_STATUS_ERROR ;
}

void APP_UserEvtRx(void *pData)
{
  uint32_t i;

  hci_spi_pckt *hci_pckt = (hci_spi_pckt *)pData;

  if(hci_pckt->type == HCI_EVENT_PKT)
  {
    hci_event_pckt *event_pckt = (hci_event_pckt*)hci_pckt->data;

    if(event_pckt->evt == EVT_LE_META_EVENT)
    {
      evt_le_meta_event *evt = (void *)event_pckt->data;

      for (i = 0; i < (sizeof(hci_le_meta_events_table)/sizeof(hci_le_meta_events_table_type)); i++)
      {
        if (evt->subevent == hci_le_meta_events_table[i].evt_code)
        {
          hci_le_meta_events_table[i].process((void *)evt->data);
        }
      }
    }
    else if(event_pckt->evt == EVT_VENDOR)
    {
      evt_blue_aci *blue_evt = (void*)event_pckt->data;

      for (i = 0; i < (sizeof(hci_vendor_specific_events_table)/sizeof(hci_vendor_specific_events_table_type)); i++)
      {
        if (blue_evt->ecode == hci_vendor_specific_events_table[i].evt_code)
        {
          hci_vendor_specific_events_table[i].process((void *)blue_evt->data);
        }
      }
    }
    else
    {
      for (i = 0; i < (sizeof(hci_events_table)/sizeof(hci_events_table_type)); i++)
      {
        if (event_pckt->evt == hci_events_table[i].evt_code)
        {
          hci_events_table[i].process((void *)event_pckt->data);
        }
      }
    }
  }
}
