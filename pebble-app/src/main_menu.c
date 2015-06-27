#include "main_menu.h"

#define DATA_ITEM 0
#define NUM_MENU_ICONS 1

static GBitmap *s_menu_icons[NUM_MENU_ICONS];
static Window *main_menu_window;
static MenuLayer *main_menu_layer;

static int16_t main_menu_get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  return 44;
}

static void main_menu_draw_row_callback(GContext* ctx, Layer *cell_layer, MenuIndex *cell_index, void *data) {
  char* text = "XXXXXXXXXXX";
  GBitmap* icon = NULL;

  switch(cell_index->row) {
	case 0: text = "10 Rows"; icon = s_menu_icons[0]; break;
	case 1: text = "30 Rows"; icon = s_menu_icons[0]; break;
    case 2: text = "50 Rows"; icon = s_menu_icons[0]; break;    
  }
  menu_cell_basic_draw(ctx, cell_layer, text, NULL, icon);
}

static uint16_t main_menu_get_num_rows_callback(struct MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return 3;
}

static void main_menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
	uint16_t count = 0;
	switch(cell_index->row) {
		case 0: count = 10; break;
		case 1: count = 30; break;
		case 2: count = 50; break;
	}
  entry_list_show(dataitems, DATALIST, count, -1);
}

void main_menu_window_load(Window *window) {
  s_menu_icons[0] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MENU_ICON);
	
  Layer *window_layer = window_get_root_layer(window);
  GRect window_frame = layer_get_frame(window_layer);
  main_menu_layer = menu_layer_create(window_frame);
  
  menu_layer_set_callbacks(main_menu_layer, NULL, (MenuLayerCallbacks) {
    .get_cell_height = (MenuLayerGetCellHeightCallback) main_menu_get_cell_height_callback,
    .draw_row = (MenuLayerDrawRowCallback) main_menu_draw_row_callback,
    .get_num_rows = (MenuLayerGetNumberOfRowsInSectionsCallback) main_menu_get_num_rows_callback,
    .select_click = (MenuLayerSelectCallback) main_menu_select_callback,
  });
  menu_layer_set_click_config_onto_window(main_menu_layer, window);
  layer_add_child(window_layer, menu_layer_get_layer(main_menu_layer));
}

void main_menu_window_unload(Window *window) {
  menu_layer_destroy(main_menu_layer);
}

void main_menu_init() {
  main_menu_window = window_create();
  window_set_window_handlers(main_menu_window, (WindowHandlers) {
    .load = main_menu_window_load,
    .unload = main_menu_window_unload,
  });
}

void main_menu_show() {
  const bool animated = true;
  window_stack_push(main_menu_window, animated);
}

void main_menu_deinit() {
  window_destroy(main_menu_window);
}