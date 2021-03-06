#include "framework/request.h"
#include "framework/response.h"
#include "framework/configuration.h"
#include "framework/tcpclient.h"
#include "framework/udpclient.h"
#include "framework/serviceagent.h"
#include "framework/packet.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

void testPacket() {
    Packet packet;
    packet.flags.action = 1;
    packet.flags.config = 1;
    packet.flags.flow = 0;
    packet.flags.init = 0;
    packet.flags.respond = 0;
    packet.flags.target = 0;
    packet.flags.type = 0;
    packet.flags.window = 0;

    packet.id = 1;
    packet.data_window_start = 1000;
    packet.data_window_stop = 1001;
    packet.request_address = "127.0.0.1";
    packet.response_address = "127.0.0.1";
    packet.payload = "Bravo Marko, care! :D";

    int response = packet_verify(&packet);
    sleep(1);

    if (response > 0) {
        printf("Packet tested successfully (%lu)\n", sizeof(Packet));
    } else {
        printf("Supplied packet isn't valid DNA format\n");
    }
}

void testConfigs() {
    Request request;
    Response response;
    ConfigurationConsumer config_consumer;
    ConfigurationProducer config_producer;
    ConfigurationServiceAgent config_serviceAgent;
    ConfigurationQualityOfService config_qualityOfService;

    request_init(&request);
    response_init(&response);

    char *consumerHost = "127.0.0.1";
    char *producerHost = "127.0.0.1";
    int consumerPort = 9878;
    int producerPort = 9879;
    char *qosHost = "127.0.0.1";
    int qosPort = 9877;
//    char *producerEndpoint = "/histogram";

    config_producer.host = producerHost;
    config_producer.port = producerPort;
    config_consumer.host = consumerHost;
    config_consumer.port = consumerPort;

    config_serviceAgent.consumerHost = consumerHost;
    config_serviceAgent.consumerPort = consumerPort;
    config_serviceAgent.producerHost = producerHost;
    config_serviceAgent.producerPort = producerPort;

    config_qualityOfService.host = qosHost;
    config_qualityOfService.port = qosPort;

    printf("Quality of service host: %s\n", config_qualityOfService.host);
    printf("Consumer host: %s\n", config_consumer.host);
    printf("Producer host: %s\n", config_producer.host);
    printf("Quality of service port: %d\n", config_qualityOfService.port);
    printf("Consumer port: %d\n", config_consumer.port);
    printf("Producer port: %d\n", config_producer.port);
}

void testClients() {
    // test configuration
    int target_message_count = 500;
    int target_thread_count = 1;
    int thread_count = 6;
    Task task[thread_count];
    char *testHost[] = {"127.0.0.1", "127.0.0.1", "127.0.0.1", "127.0.0.1", "127.0.0.1", "127.0.0.1"};
    // first three are TCP clients
    // second three are UDP clients
    char *testPort[] = {"9879", "9889", "9899", "9979", "9989", "9999"};
    char *message = "Bravo, Marko, care! :D";

    // creating and initializing tasks
    for (int i = 0; i < 6; i++) {
        task[i].host = testHost[i];
        task[i].port = testPort[i];
        task[i].message = message;
        task[i].repeat_count = target_message_count;
        task[i].thread_count = target_thread_count;
    }

    // dispatching threads for individual tcp/udp clients
    for (int i = 0; i < 1; i++) {

        pthread_t thread[thread_count];
        pthread_attr_t attributes[thread_count];

        for (int i = 0; i < thread_count; i++) {
            pthread_attr_init(&(attributes[i]));
            if (i < 3) {
                pthread_create(&(thread[i]), &(attributes[i]), (void *) tcp_client, &task[i]);
            } else {
                pthread_create(&(thread[i]), &(attributes[i]), (void *) udp_client, &task[i]);
            }
        }

        for (int i = 0; i < thread_count; i++) {
            pthread_join(thread[i], NULL);
        }

    }
}

void testDevice() {
    char* path = "storage/example.conf";
    char* mode = "r";
    Device device;
    int result;

    result = ServiceAgentDeviceConfiguration(path, mode, &device);
    if (1 == result) {
        printf("=== Correctly parsed configuration ===\n");
    } else {
        printf("=== Error parsing configuration ===\n");
    }

    if (result) {
        printf("Host: %s, port: %d\n\n", device.host, device.port);
    } else {
        printf("Error reading device configuration\n");
    }
}

int main(int argc, char **argv) {

    if (argc > 1 && !strcmp(argv[1], "y")) {
        printf("Testing creation and initialization of configuration data structure");
        testConfigs();
    }
    if (argc > 2 && !strcmp(argv[2], "y")) {
        printf("Testing TCP and UDP client creation and threaded execution");
        testClients();
    }
    if (argc > 3 && !strcmp(argv[3], "y")) {
        printf("Testing device configuration (Service Agent target node)\n");
        testDevice();
    }
    if (argc > 4 && !strcmp(argv[4], "y")) {
        printf("Testing verification of DNA Packet\n");
        testPacket();
    }

    if (argc <= 1) {
        printf("No tests selected, bye :)\n");
    }

    return 0;
}
