#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "MQTTClient.h"
#include <pthread.h>

#define ADDRESS     "tcp://localhost:1883"  
#define CLIENTID    "PC_Client"
#define TOPIC       "test/#"           
#define QOS         1
#define TIMEOUT     10000L

volatile MQTTClient_deliveryToken deliveredtoken;
char last_crossline_topic[100] = "";

void delivered(void* context, MQTTClient_deliveryToken dt) {
    printf("MESSAGE TRANSFER COMPLETE (token %d)\n", dt);
    deliveredtoken = dt;
}
typedef struct {
    MQTTClient client;
    const char* topic;
    const char* message;
} mqtt_send_args;



void* send_response_thread(void* arg) {
    mqtt_send_args* args = (mqtt_send_args*)arg;

    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;

    pubmsg.payload = (char*)args->message;
    pubmsg.payloadlen = (int)strlen(args->message);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;

    int rc = MQTTClient_publishMessage(args->client, args->topic, &pubmsg, &token);
    if (rc == MQTTCLIENT_SUCCESS) {
        printf(" SENT '%s' to %s\n", args->message, args->topic);
        MQTTClient_waitForCompletion(args->client, token, TIMEOUT);
    }
    else {
        printf(" SEND FAIL: %d\n", rc);
    }

    free(arg);  
    return NULL;
}
int msgarrvd(void* context, char* topicName, int topicLen, MQTTClient_message* message) {
    char* payload = (char*)message->payload;
    printf(" RECEIVE -> TOPIC: %s\n", topicName);
    printf(" message: %.*s\n", message->payloadlen, payload);

        pthread_t tid;

        mqtt_send_args* args = malloc(sizeof(mqtt_send_args));
        args->client = (MQTTClient)context;
        args->topic = "test/arduino_main";
        args->message = strndup(payload, message->payloadlen);
        if (pthread_create(&tid, NULL, send_response_thread, args) != 0) {
            printf("FAILED TO CREATE THREAD\n");
            free(args);
        }
        else {
            pthread_detach(tid);  
        }

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

void connlost(void* context, char* cause) {
    printf(" disconnected! REASON: %s\n", cause);
}

int main(int argc, char* argv[]) {
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;

    MQTTClient_create(&client, ADDRESS, CLIENTID,
        MQTTCLIENT_PERSISTENCE_NONE, NULL);


    MQTTClient_setCallbacks(client, client, connlost, msgarrvd, delivered);


    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    int rc;
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        printf(" CONNECT FAIL, reason: %d\n", rc);
        return -1;
    }

    printf(" MQTT BROKER COMPLETE. TOPIC SUBSCRIBE START!\n");
    MQTTClient_subscribe(client, TOPIC, QOS);

    while (1) {
        sleep(1);
    }

    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    return 0;
}
