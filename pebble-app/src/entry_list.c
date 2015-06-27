#include "entry_list.h"
#define MAX_STACK_DEPTH 4
#define NUM_MENU_SECTIONS 1
#define NUM_MENU_ICONS 1

static GBitmap *s_menu_icons[NUM_MENU_ICONS];

static int16_t entry_list_get_cell_height_callback(EntryList *self, struct MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  return 28;
}

static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return NUM_MENU_SECTIONS;
}

static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  // Determine which section we're working with
  switch (section_index) {
    case 0:
      // Draw title text in the section header
      menu_cell_basic_header_draw(ctx, cell_layer, "Datalist");
      break;
  }
}

static void entry_list_draw_row_callback(GContext* ctx, Layer *cell_layer, MenuIndex *cell_index, void *data) {
  EntryList *self = ((EntryList *) data);
  Entry entry = self->entries[cell_index->row];
  if (entry.data_name){
	menu_cell_basic_draw(ctx, cell_layer, entry.data_name, NULL, NULL);
  }
}

static uint16_t entry_list_get_num_rows_callback(struct MenuLayer *menu_layer, uint16_t section_index, void *data) {
  EntryList *self = ((EntryList *) data);
  return self->entries_count;
}

static void entry_list_free_entries(EntryList *self) {
  free(self->entries);
  self->entries = NULL;
}

EntryList *entry_list_stack[MAX_STACK_DEPTH];
int8_t current_entry_list_index = -1;

void entry_list_window_load(Window *window) {
	s_menu_icons[0] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MENU_ICON);
	
  EntryList *self = entry_list_stack[current_entry_list_index];
  Layer *window_layer = window_get_root_layer(self->window);
  GRect window_frame = layer_get_frame(window_layer);
  self->menu_layer = menu_layer_create(window_frame);
  menu_layer_set_callbacks(self->menu_layer, self, (MenuLayerCallbacks) {
    .draw_row = (MenuLayerDrawRowCallback) entry_list_draw_row_callback,
    .get_num_rows = (MenuLayerGetNumberOfRowsInSectionsCallback) entry_list_get_num_rows_callback,
    .select_click = (MenuLayerSelectCallback) self->onclick,
	.draw_header = menu_draw_header_callback,
	.get_num_sections = menu_get_num_sections_callback,
  });
  menu_layer_set_click_config_onto_window(self->menu_layer, self->window);
  layer_add_child(window_layer, menu_layer_get_layer(self->menu_layer));
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Entry window loaded");
}

void entry_list_window_unload(Window *window) {
  EntryList *self = entry_list_stack[current_entry_list_index];
  if (self->entries) entry_list_free_entries(self);
  self->entries_count = 0;
  menu_layer_destroy(self->menu_layer);
  self->menu_layer = NULL;
  current_entry_list_index--;
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Entry window unloaded");
}

EntryList *entry_list_init(MenuLayerSelectCallback onclick) {
  EntryList *self = malloc(sizeof(EntryList));
  self->window = window_create();
  self->onclick = onclick;
  window_set_window_handlers(self->window, (WindowHandlers) {
    .load = entry_list_window_load,
    .unload = entry_list_window_unload,
  });
  return self;
}

void entry_list_show(EntryList *self, int8_t context, int8_t parent_context, int32_t parent_id) {
  current_entry_list_index++;
  entry_list_stack[current_entry_list_index] = self;
  const bool animated = true;
  window_stack_push(self->window, animated);
  request_data(context, parent_context, parent_id);
  self->parent_context = parent_context;
  self->parent_id = parent_id;
}

void entry_list_deinit(EntryList *self) {
  window_destroy(self->window);
  entry_list_free_entries(self);
  free(self);
}

bool entry_list_is_active(EntryList *self) {
  return window_stack_get_top_window() == self->window;
}

void entry_list_content_start(EntryList *self, uint16_t count) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Entry count received: %d", count);
  if (self->entries) entry_list_free_entries(self);
  self->entries = malloc(count * sizeof(Entry));
  self->entries_count_max = count;
  self->entries_count = 0;
}

void entry_list_content_add(EntryList *self, uint16_t index, uint32_t id, char* name, char* service, char* uri) {
  if (self->entries) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Entry #%d added: %s [%lu]", index, name, (unsigned long) id);
    self->entries[index].id = id;
    copy_string(self->entries[index].data_name, name, ENTRY_NAME_LEN);
	copy_string(self->entries[index].data_id, service, ENTRY_NAME_LEN);
	copy_string(self->entries[index].data_link, uri, ENTRY_LINK_LEN);
    self->entries_count++;
    if (self->menu_layer) menu_layer_reload_data(self->menu_layer);
  }
}

void dataitems_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  EntryList *self = ((EntryList *) data);
  Entry entry = self->entries[cell_index->row];
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Clicked dataitem #%d -- %s", cell_index->row, entry.data_name);
}
