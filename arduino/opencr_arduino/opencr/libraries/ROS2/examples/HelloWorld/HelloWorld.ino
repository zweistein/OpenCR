#include "micrortps.h"

#include "HelloWorld.h"
#include "xml.h"

typedef enum {
  CREATE_CLIENT = 0,
  CREATE_PARTICIPANT,
  CREATE_TOPIC,
  CREATE_PUBLISHER,
  CREATE_SUBSCRIBER,
  CREATE_WRITER,
  CREATE_READER,
  WRITE_TO_AGENT,
  READ_FROM_AGENT
} process_t;


#define BUFFER_SIZE  1024
#define DEBUG_SERIAL Serial2  
#define RTPS_SERIAL  Serial   //OpenCR USB
#define CREATE_TIMEOUT 500

ClientState* state;
XRCEInfo client_info;
XRCEInfo participant_info;
XRCEInfo publisher_info;
XRCEInfo data_writer_info;
XRCEInfo subscriber_info;
XRCEInfo data_reader_info;


void on_status_received(XRCEInfo info, uint8_t operation, uint8_t status, void* args);
void on_hello_topic(XRCEInfo info, const void* vtopic, void* args);


void setup() 
{
  DEBUG_SERIAL.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  state = new_serial_client_state(BUFFER_SIZE, "opencr_usb");
  
  while (!RTPS_SERIAL);
}

process_t process = CREATE_CLIENT;
uint8_t ret_status = STATUS_ERR_DDS_ERROR;
uint32_t pre_time = 0, pre_time_led = 0;
bool topic_recv_done = true;
bool led_state = false;


void loop() 
{
  switch (process)
  {   
    case CREATE_CLIENT:
      {
        if (ret_status == STATUS_OK)
        {
          ret_status = STATUS_ERR_DDS_ERROR;
          process = CREATE_PARTICIPANT;
        }
        else if (millis() - pre_time >= CREATE_TIMEOUT)
        {
          pre_time = millis();
          create_client(state, on_status_received, NULL);
        }
      }
      break;

    case CREATE_PARTICIPANT:
      {
        if (ret_status == STATUS_OK)
        {
          ret_status = STATUS_ERR_DDS_ERROR;
          process = CREATE_TOPIC;
        }
        else if (millis() - pre_time >= CREATE_TIMEOUT)
        {
          pre_time = millis();
          participant_info = create_participant(state);
        }
      }
      break;

    case CREATE_TOPIC:
      {
        if (ret_status == STATUS_OK)
        {
          ret_status = STATUS_ERR_DDS_ERROR;
          process = CREATE_PUBLISHER;
        }
        else if (millis() - pre_time >= CREATE_TIMEOUT)
        {
          pre_time = millis();
          create_topic(state, participant_info.object_id, topic_profile);
        }
      }
      break;

    case CREATE_PUBLISHER:
      {
        if (ret_status == STATUS_OK)
        {
          ret_status = STATUS_ERR_DDS_ERROR;
          process = CREATE_SUBSCRIBER;
        }
        else if (millis() - pre_time >= CREATE_TIMEOUT)
        {
          pre_time = millis();
          publisher_info = create_publisher(state, participant_info.object_id);
        }
      }
      break;

    case CREATE_SUBSCRIBER:
      {
        if (ret_status == STATUS_OK)
        {
          ret_status = STATUS_ERR_DDS_ERROR;
          process = CREATE_WRITER;
        }
        else if (millis() - pre_time >= CREATE_TIMEOUT)
        {
          pre_time = millis();
          subscriber_info = create_subscriber(state, participant_info.object_id);
        }
      }
      break;

    case CREATE_WRITER:
      {
        if (ret_status == STATUS_OK)
        {
          ret_status = STATUS_ERR_DDS_ERROR;
          process = CREATE_READER;
        }
        else if (millis() - pre_time >= CREATE_TIMEOUT)
        {
          pre_time = millis();
          data_writer_info = create_data_writer(state, participant_info.object_id, publisher_info.object_id, data_writer_profile);
        }
      }
      break;

    case CREATE_READER:
      {
        if (ret_status == STATUS_OK)
        {
          ret_status = STATUS_ERR_DDS_ERROR;
          process = WRITE_TO_AGENT;
        }
        else if (millis() - pre_time >= CREATE_TIMEOUT)
        {
          pre_time = millis();
          data_reader_info = create_data_reader(state, participant_info.object_id, subscriber_info.object_id, data_reader_profile);
        }
      }
      break;

    case WRITE_TO_AGENT:
      {
        HelloWorld hello_topic = {millis(), "HelloWorld"};
        write_data(state, data_writer_info.object_id, serialize_HelloWorld_topic, &hello_topic);
        process = READ_FROM_AGENT;
      }
      break;
    case READ_FROM_AGENT:
      {
        if (topic_recv_done == true)
        {
          read_data(state, data_reader_info.object_id, deserialize_HelloWorld_topic, on_hello_topic, NULL);
          topic_recv_done = false;
        }
        
        if (millis() - pre_time >= 100)
        {
          pre_time = millis();
          process = WRITE_TO_AGENT;
        } 
      }
      break;
  }

  if (millis() - pre_time_led >= 500)
  {
    pre_time_led = millis();
    led_state = !led_state;
    digitalWrite(LED_BUILTIN, led_state);
  }

  send_to_agent(state);

  receive_from_agent(state);
}


void on_status_received(XRCEInfo info, uint8_t operation, uint8_t status, void* args)
{
  ret_status = status;

  DEBUG_SERIAL.print("\t [Recv Callback] ");
  DEBUG_SERIAL.print(info.object_id, HEX); DEBUG_SERIAL.print(" ");
  DEBUG_SERIAL.print(info.request_id, HEX); DEBUG_SERIAL.print(" ");
  DEBUG_SERIAL.print(operation); DEBUG_SERIAL.print(" ");
  DEBUG_SERIAL.println(status);
}

void on_hello_topic(XRCEInfo info, const void* vtopic, void* args)
{
  HelloWorld* topic = (HelloWorld*) vtopic;

  DEBUG_SERIAL.print("- Message ");
  DEBUG_SERIAL.print(topic->m_message);
  DEBUG_SERIAL.print(" ");
  DEBUG_SERIAL.print(topic->m_index);
  DEBUG_SERIAL.println(" RECEIVED");

  free(topic->m_message);
  free(topic);

  topic_recv_done = true;
}

