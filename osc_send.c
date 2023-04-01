#include <stdio.h>
#include <stdlib.h>
#include <lo/lo.h>
#include <string.h>

#define PORT 1234
#define MAX_VALUES 256

typedef enum { TYPE_INT, TYPE_FLOAT, TYPE_STRING } ValueType;

typedef union {
    int i;
    float f;
    char *s;
} ValueData;

typedef struct {
    ValueType type;
    ValueData data;
} Value;

int main() {
    lo_address t;
    char input[256];

    t = lo_address_new(NULL, "1234");

    while (1) {
        printf("Enter message: ");
        fgets(input, 256, stdin);

        char *str = strtok(input, " ");
        lo_message msg = lo_message_new();

        int count = 0;
        char *address = NULL;
        Value values[MAX_VALUES];
        int value_count = 0;

        while (str != NULL) {
            if (count == 0) {
                address = str;
            } else {
                int val_int;
                float val_float;
                if (sscanf(str, "%f", &val_float) == 1) {
                    if (strchr(str, '.')) {
                        lo_message_add_float(msg, val_float);
                        values[value_count].type = TYPE_FLOAT;
                        values[value_count++].data.f = val_float;
                    } else {
                        val_int = (int)val_float;
                        lo_message_add_int32(msg, val_int);
                        values[value_count].type = TYPE_INT;
                        values[value_count++].data.i = val_int;
                    }
                } else {
                    lo_message_add_string(msg, str);
                    values[value_count].type = TYPE_STRING;
                    values[value_count++].data.s = str;
                }
            }
            count++;
            str = strtok(NULL, " ");
        }

        if (address != NULL) {
            printf("Sending message: %s with values:", address);
            for (int i = 0; i < value_count; i++) {
                switch (values[i].type) {
                    case TYPE_INT:
                        printf(" %d", values[i].data.i);
                        break;
                    case TYPE_FLOAT:
                        printf(" %f", values[i].data.f);
                        break;
                    case TYPE_STRING:
                        printf(" %s", values[i].data.s);
                        break;
                }
            }
            printf("\n");

            if (lo_send_message(t, address, msg) == -1) {
                printf("OSC error %d: %s\n", lo_address_errno(t), lo_address_errstr(t));
            }
        }

        lo_message_free(msg);
    }

    lo_address_free(t);
    return 0;
}
