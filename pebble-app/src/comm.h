#ifndef _COMM
#define _COMM

#include <pebble.h>

#define MSG_TYPE 0
enum mtype {
  REQ,
  RSP_START,
  RSP_DATA,
  RSP_END, // ununsed
  PLAY
};

#define MSG_CTX 1
#define MSG_PARENT_CTX 2
enum mcontext {
	DATALIST
};

#define COUNT 100
#define INDEX 101
#define ID 102
#define DATA_NAME 103
#define DATA_ID 104
#define DATA_LINK 105

void request_data(int8_t context, int8_t parent_context, int32_t id);

#endif
