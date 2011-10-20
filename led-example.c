/* 
 * Copyright (C) 2002,2003 Julien Danjou <julien@jdanjou.org>
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 * 
 * This is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/io.h>

#include <parportled.h>

/* Some code examples */

int led_on_right_to_left(int total_time)
{
	int i;
	for(i=1;i <= MAXLED;i++)
	{
		led_on(i);
		usleep(total_time/MAXLED);
		led_off(i);
	}
}

int led_on_left_to_right(int total_time)
{
	int i;
	for(i=MAXLED;i > 0;i--)
	{
		led_on(i);
		usleep(total_time/MAXLED);
		led_off(i);
	}
}

int led_on_center_to_border(int pause_time)
{
	int i;
	for(i=MAXLED/2;i>=0;i--){
		led_on(i);
		led_on(MAXLED-i+1);
    usleep(pause_time);
		if(i!=MAXLED/2) led_off(i+1);
		if(i!=MAXLED/2) led_off(MAXLED-i);
		usleep(pause_time);
	}
}


int main()
{
	/*	led_setperm();
	led_on_center_to_border(100000);

	led_on_right_to_left(50000);
	led_on_left_to_right(50000);

	led_blink_on(4, 100000);
	led_blink_on(5, 100000);
	sleep(2);
	led_blink_off(5);
	sleep(1);
	led_blink_off(4);
	sleep(5);
	led_on(1);
	*/

	led_setperm();
	while(1) {

	led_on_right_to_left(100000);
	led_on_left_to_right(100000);

	}

	exit();
}

