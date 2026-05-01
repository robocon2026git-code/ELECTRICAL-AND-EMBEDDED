#include "bot.h"
#include "user.h" // Needed to read rx_pkt

BOT_Status_t war_status;

// ==========================================
// STATE VARIABLES
// ==========================================
uint8_t is_staff_mode = 1;        // Starts in Staff mode (1 = Staff, 0 = KFS)
uint8_t last_options_state = 0;   // For Edge Detection
uint32_t last_blue_blink = 0;     // For non-blocking blinks

int system_status() {
	war_status.staff_p1 = 0;
	war_status.staff_p2 = 0;
	war_status.staff_p3 = 0;
	war_status.kfs_p1 = 0;
	war_status.kfs_p2 = 0;
	war_status.kfs_p3 = 0;
	war_status.kfs_ToF = 43;
	war_status.bot_speed = 0;

	return 0;
}

int odu() {
    recieve_uart(&huart2);
    lo_4_wheel_handler(&htim3);

    // 1. BULLETPROOF EDGE DETECTION
    // We read bit 10 directly from the raw UART packet. This prevents any struct mapping bugs!
    uint8_t current_options_state = (rx_pkt.btn_flag & (1 << 10)) ? 1 : 0;

    if (current_options_state == 1 && last_options_state == 0) {
        is_staff_mode = !is_staff_mode; // Flip the mode

        // Print loudly to the terminal so we know it worked
        printf("\r\n==================================\r\n");
        printf("   MODE SWITCHED TO: %s   \r\n", is_staff_mode ? "STAFF" : "KFS");
        printf("==================================\r\n\n");

        if (is_staff_mode == 1) {
            HAL_GPIO_WritePin(LED_RED_PORT, LED_RED_PIN, GPIO_PIN_SET);   // Red ON
        } else {
            HAL_GPIO_WritePin(LED_RED_PORT, LED_RED_PIN, GPIO_PIN_RESET); // Red OFF
            HAL_GPIO_WritePin(LED_BLUE_PORT, LED_BLUE_PIN, GPIO_PIN_RESET); // Blue OFF
        }
    }
    last_options_state = current_options_state;

    // 2. Execute Modes
    if (is_staff_mode == 1) {
        // --- STAFF MODE ---
        staff_arm_control();

        // Blink Blue LED every 500ms safely without blocking
        if (HAL_GetTick() - last_blue_blink >= 500) {
            HAL_GPIO_TogglePin(LED_BLUE_PORT, LED_BLUE_PIN);
            last_blue_blink = HAL_GetTick();
        }
    } else {
        // --- KFS MODE ---
        kfs_arm_handler();
    }

    return 0;
}

void bot_setup() {
    printf("STM32 Ready. Booting into STAFF MODE.\r\n");
    kfs_arm_setup();
    staff_arm_setup();

    // Initialize Default State: Staff Mode (Red LED ON)
    HAL_GPIO_WritePin(LED_RED_PORT, LED_RED_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED_BLUE_PORT, LED_BLUE_PIN, GPIO_PIN_RESET);

    HAL_Delay(1000);
}
