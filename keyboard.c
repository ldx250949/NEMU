#include "hal.h"

#define NR_KEYS 18
enum {KEY_STATE_EMPTY, KEY_STATE_WAIT_RELEASE, KEY_STATE_RELEASE, KEY_STATE_PRESS};

static const int keycode_hash[] = {
	K_UP, K_DOWN, K_LEFT, K_RIGHT, K_ESCAPE,
	K_RETURN, K_SPACE, K_PAGEUP, K_PAGEDOWN, K_r,
	K_a, K_d, K_e, K_w, K_q,
	K_s, K_f, K_p
};

static int key_state[NR_KEYS];
void
keyboard_event(void) {
	uint32_t code = in_byte(0x60);
	uint32_t keycode = code & 0x7f;
	int i;
	for(i = 0; i < NR_KEYS; i ++) {
		if(keycode_hash[i] == keycode) {
			if(code & 0x80) {
				if(key_state[i]==KEY_STATE_PRESS||key_state[i]==KEY_STATE_WAIT_RELEASE) key_state[i] = KEY_STATE_RELEASE;
				else key_state[i]=KEY_STATE_EMPTY;
			}
			else if(key_state[i] == KEY_STATE_EMPTY||key_state[i]==KEY_STATE_RELEASE) {
				key_state[i] = KEY_STATE_PRESS;
			}
			else key_state[i]=KEY_STATE_WAIT_RELEASE;
		}
	}
}

int
get_keycode(int index) {
	assert(index >= 0 && index < NR_KEYS);
	return keycode_hash[index];
}

int
query_key(int index) {
	assert(index >= 0 && index < NR_KEYS);
	return key_state[index];
}

void
release_key(int index) {
	assert(index >= 0 && index < NR_KEYS);
	key_state[index] = KEY_STATE_WAIT_RELEASE;
}

void
clear_key(int index) {
	assert(index >= 0 && index < NR_KEYS);
	key_state[index] = KEY_STATE_EMPTY;
}

bool 
process_keys(void (*key_press_callback)(int), void (*key_release_callback)(int)) {
	cli();
	/* TODO: Traverse the key states. Find a key just pressed or released.
	 * If a pressed key is found, call `key_press_callback' with the keycode.
	 * If a released key is found, call `key_release_callback' with the keycode.
	 * If any such key is found, the function return true.
	 * If no such key is found, the function return false.
	 * Remember to enable interrupts before returning from the function.
	 */
	int i;
	bool flag=0;
	for(i = 0; i < NR_KEYS; i ++) {
		//if(old_key_state[i]!=key_state[i]) Log("different!");
		if(key_state[i]==KEY_STATE_PRESS){
			Log("Pressed button %d",i);
			key_press_callback(i);
			key_state[i]=KEY_STATE_WAIT_RELEASE;
			flag=1;
		}
		if(key_state[i]==KEY_STATE_RELEASE){
			key_release_callback(i);
			key_state[i]=KEY_STATE_EMPTY;
			flag=1;
		}
	}	
	sti();
	return flag;
}
