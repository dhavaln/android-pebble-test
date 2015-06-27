#ifndef _ENTRY
#define _ENTRY
#define ENTRY_NAME_LEN 64
#define ENTRY_LINK_LEN 100

typedef struct {
  uint32_t id;
  char data_name[ENTRY_NAME_LEN];
  char data_id[ENTRY_NAME_LEN];
  char data_link[ENTRY_LINK_LEN];
} Entry;

#endif
