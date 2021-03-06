/* This file was generated by plugin 'Nordic Semiconductor nRF5x v.1.2.2' (BDS version 1.0.2095.0) */

#include "ble_gpio_test_service.h"
#include <string.h>
#include "nordic_common.h"
#include "ble_srv_common.h"
#include "app_util.h"
#include "app_util_bds.h"

#define OPCODE_LENGTH 1 /**< Length of opcode inside GPIO Test Service packet. */
#define HANDLE_LENGTH 2 /**< Length of handle inside GPIO Test Service packet. */

/* TODO Consider changing the max values if encoded data for characteristic/descriptor is fixed length */ 
#define MAX_LED_LEN (BLE_L2CAP_MTU_DEF - OPCODE_LENGTH - HANDLE_LENGTH) /**< Maximum size of a transmitted Led. */ 
#define MAX_BUTTON_LEN (BLE_L2CAP_MTU_DEF - OPCODE_LENGTH - HANDLE_LENGTH) /**< Maximum size of a transmitted Button. */ 

/**@brief Function for encoding Led.
 *
 * @param[in]   p_led              Led characteristic structure to be encoded.
 * @param[out]  p_encoded_buffer   Buffer where the encoded data will be written.
 *
 * @return      Size of encoded data.
 */
static uint8_t led_encode(ble_gpio_test_service_led_t * p_led, uint8_t * encoded_buffer)
{
    uint8_t len = 0; 
    encoded_buffer[len++] = p_led->toggle_led;
    return len;
}

/**@brief Function for decoding Led.
 *
 * @param[in]   data_len              Length of the field to be decoded.
 * @param[in]   p_data                Buffer where the encoded data is stored.
 * @param[out]  p_write_val           Decoded data.
 *
 * @return      Length of the decoded field.
 */
static uint8_t led_decode(uint8_t data_len, uint8_t * p_data, ble_gpio_test_service_led_t * p_write_val)
{
    uint8_t pos = 0;
    p_write_val->toggle_led = p_data[pos++]; 

    return pos;
} 
/**@brief Function for encoding Button.
 *
 * @param[in]   p_button              Button characteristic structure to be encoded.
 * @param[out]  p_encoded_buffer   Buffer where the encoded data will be written.
 *
 * @return      Size of encoded data.
 */
static uint8_t button_encode(ble_gpio_test_service_button_t * p_button, uint8_t * encoded_buffer)
{
    uint8_t len = 0; 
    encoded_buffer[len++] = p_button->button_state;
    return len;
}


/**@brief Function for handling the Connect event.
 *
 * @param[in]   p_gpio_test_service       GPIO Test Service Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_connect(ble_gpio_test_service_t * p_gpio_test_service, ble_evt_t * p_ble_evt)
{
    p_gpio_test_service->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
}

/**@brief Function for handling the Disconnect event.
 *
 * @param[in]   p_gpio_test_service       GPIO Test Service Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_disconnect(ble_gpio_test_service_t * p_gpio_test_service, ble_evt_t * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    p_gpio_test_service->conn_handle = BLE_CONN_HANDLE_INVALID;
}

/**@brief Function for handling the Write event.
 *
 * @param[in]   p_gpio_test_service       GPIO Test Service Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_write(ble_gpio_test_service_t * p_gpio_test_service, ble_gatts_evt_write_t * p_ble_evt)
{
    
    if(p_ble_evt->handle == p_gpio_test_service->led_handles.value_handle)
    {
        if(p_gpio_test_service->evt_handler != NULL)
        {
            ble_gpio_test_service_evt_t evt;
            evt.evt_type = BLE_GPIO_TEST_SERVICE_LED_EVT_WRITE;
            led_decode(p_ble_evt->len, p_ble_evt->data, &evt.params.led);
            p_gpio_test_service->evt_handler(p_gpio_test_service, &evt);
        }
    }
    if(p_ble_evt->handle == p_gpio_test_service->button_handles.cccd_handle)
    {
        if(p_gpio_test_service->evt_handler != NULL)
        {
            ble_gpio_test_service_evt_t evt;
            evt.evt_type = BLE_GPIO_TEST_SERVICE_BUTTON_EVT_CCCD_WRITE;
            bds_uint16_decode(p_ble_evt->len, p_ble_evt->data, &evt.params.cccd_value);
            p_gpio_test_service->evt_handler(p_gpio_test_service, &evt);
        }
    } 
}

/**@brief Authorize WRITE request event handler.
 *
 * @details Handles WRITE events from the BLE stack.
 *
 * @param[in]   p_sc_ctrlpt  SC Ctrlpt structure.
 * @param[in]   p_gatts_evt  GATTS Event received from the BLE stack.
 *
 */
static void on_rw_authorize_request(ble_gpio_test_service_t * p_gpio_test_service, ble_gatts_evt_t * p_gatts_evt)
{
    ble_gatts_evt_rw_authorize_request_t * p_auth_req = &p_gatts_evt->params.authorize_request;
    if (p_auth_req->type == BLE_GATTS_AUTHORIZE_TYPE_WRITE)
    {
        if (   (p_gatts_evt->params.authorize_request.request.write.op
                != BLE_GATTS_OP_PREP_WRITE_REQ)
            && (p_gatts_evt->params.authorize_request.request.write.op
                != BLE_GATTS_OP_EXEC_WRITE_REQ_NOW)
            && (p_gatts_evt->params.authorize_request.request.write.op
                != BLE_GATTS_OP_EXEC_WRITE_REQ_CANCEL)
           )
        {
        
            if (p_auth_req->request.write.handle == p_gpio_test_service->led_handles.value_handle)
            {
                on_write(p_gpio_test_service, &p_auth_req->request.write);
            }
        }
    }
}

/**@brief Function for handling BLE events.
 *
 * @param[in]   p_gpio_test_service       GPIO Test Service Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
void ble_gpio_test_service_on_ble_evt(ble_gpio_test_service_t * p_gpio_test_service, ble_evt_t * p_ble_evt)
{
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            on_connect(p_gpio_test_service, p_ble_evt);
            break;
        case BLE_GAP_EVT_DISCONNECTED:
            on_disconnect(p_gpio_test_service, p_ble_evt);
            break;
        case BLE_GATTS_EVT_WRITE:
            on_write(p_gpio_test_service, &p_ble_evt->evt.gatts_evt.params.write);
            break;
         case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
            on_rw_authorize_request(p_gpio_test_service, &p_ble_evt->evt.gatts_evt);
            break;
        default:
            //No implementation needed.
            break;
    }
}

/**@brief Function for initializing the GPIO Test Service. */
uint32_t ble_gpio_test_service_init(ble_gpio_test_service_t * p_gpio_test_service, const ble_gpio_test_service_init_t * p_gpio_test_service_init)
{
    uint32_t err_code;
    ble_uuid_t ble_uuid;

    // Initialize service structure
    p_gpio_test_service->evt_handler = p_gpio_test_service_init->evt_handler;
    p_gpio_test_service->conn_handle = BLE_CONN_HANDLE_INVALID;
    
    // Add a custom base UUID.
    ble_uuid128_t bds_base_uuid = {{0xFC, 0x19, 0xA4, 0x5D, 0xFE, 0x29, 0x54, 0x8E, 0x1D, 0x46, 0x2C, 0x7E, 0x00, 0x00, 0x1C, 0x49}};
    uint8_t       uuid_type;
    err_code = sd_ble_uuid_vs_add(&bds_base_uuid, &uuid_type);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    ble_uuid.type = uuid_type;
    ble_uuid.uuid = 0x3B9C;
        
    // Add service
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_gpio_test_service->service_handle);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    } 

    // Add Led characteristic
    ble_gpio_test_service_led_t led_initial_value = p_gpio_test_service_init->ble_gpio_test_service_led_initial_value; 

    uint8_t led_encoded_value[MAX_LED_LEN];
    ble_add_char_params_t add_led_params;
    memset(&add_led_params, 0, sizeof(add_led_params));
    
    add_led_params.uuid                = 0x6946;
    add_led_params.uuid_type           = ble_uuid.type; 
    add_led_params.max_len             = MAX_LED_LEN;
    add_led_params.init_len            = led_encode(&led_initial_value, led_encoded_value);
    add_led_params.p_init_value        = led_encoded_value; 
    add_led_params.char_props.write    = 1; 
    add_led_params.write_access        = SEC_OPEN; 
    // 1 for variable length and 0 for fixed length.
    add_led_params.is_var_len          = 1; 

    err_code = characteristic_add(p_gpio_test_service->service_handle, &add_led_params, &(p_gpio_test_service->led_handles));
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    } 

    // Add Button characteristic
    ble_gpio_test_service_button_t button_initial_value = p_gpio_test_service_init->ble_gpio_test_service_button_initial_value; 

    uint8_t button_encoded_value[MAX_BUTTON_LEN];
    ble_add_char_params_t add_button_params;
    memset(&add_button_params, 0, sizeof(add_button_params));
    
    add_button_params.uuid                = 0x02D8;
    add_button_params.uuid_type           = ble_uuid.type; 
    add_button_params.max_len             = MAX_BUTTON_LEN;
    add_button_params.init_len            = button_encode(&button_initial_value, button_encoded_value);
    add_button_params.p_init_value        = button_encoded_value; 
    add_button_params.char_props.notify   = 1; 
    add_button_params.char_props.read     = 1; 
    add_button_params.read_access         = SEC_OPEN; 
    add_button_params.cccd_write_access   = SEC_OPEN;
    // 1 for variable length and 0 for fixed length.
    add_button_params.is_var_len          = 1; 

    err_code = characteristic_add(p_gpio_test_service->service_handle, &add_button_params, &(p_gpio_test_service->button_handles));
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    } 

    return NRF_SUCCESS;
}

/**@brief Function for setting the Button. */
uint32_t ble_gpio_test_service_button_set(ble_gpio_test_service_t * p_gpio_test_service, ble_gpio_test_service_button_t * p_button)
{
    ble_gatts_value_t gatts_value;
    uint8_t encoded_value[MAX_BUTTON_LEN];

    // Initialize value struct.
    memset(&gatts_value, 0, sizeof(gatts_value));

    gatts_value.len     = button_encode(p_button, encoded_value);
    gatts_value.offset  = 0;
    gatts_value.p_value = encoded_value;

    return sd_ble_gatts_value_set(p_gpio_test_service->conn_handle, p_gpio_test_service->button_handles.value_handle, &gatts_value);
}

/**@brief Function for sending the Button. */
uint32_t ble_gpio_test_service_button_send(ble_gpio_test_service_t * p_gpio_test_service, ble_gpio_test_service_button_t * p_button)
{
    uint32_t err_code = NRF_SUCCESS;

    if (p_gpio_test_service->conn_handle != BLE_CONN_HANDLE_INVALID)
    {
        ble_gatts_hvx_params_t hvx_params;
        uint8_t encoded_value[MAX_BUTTON_LEN];
        uint16_t hvx_len;

        // Initialize value struct.
        memset(&hvx_params, 0, sizeof(hvx_params));

        hvx_len           = button_encode(p_button, encoded_value);
        hvx_params.handle = p_gpio_test_service->button_handles.value_handle;
        hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;
        hvx_params.p_len  = &hvx_len;
        hvx_params.offset = 0;
        hvx_params.p_data = encoded_value;

        err_code = sd_ble_gatts_hvx(p_gpio_test_service->conn_handle, &hvx_params);
    }
    else
    {
        err_code = NRF_ERROR_INVALID_STATE;
    }

    return err_code;
}

