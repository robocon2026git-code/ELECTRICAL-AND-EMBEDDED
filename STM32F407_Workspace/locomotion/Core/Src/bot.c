#include "bot.h"

BOT_Status_t war_status;

int system_status() {
	war_status.staff_p1 = 0;
	war_status.staff_p2 = 0;
	war_status.staff_p3 = 0;
	war_status.kfs_p1 = 0;
	war_status.kfs_p2 = 0;
	war_status.kfs_p3 = 0;
	war_status.kfs_ToF = 43;
	war_status.bot_speed = 0;
}
