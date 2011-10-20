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
#include <stdlib.h>
#include <unistd.h>
#include <sys/io.h>
#include <sys/types.h>
#include <signal.h>
#include <pthread.h>

#include "parportled.h"

static unsigned char lights;

int current_blinking_led[MAXLED];

int blink_thread = -1;


void led_print_debug(char s, char *f)
{	fprintf(stderr, "%s -> %d\n",f,s); }

int led_setperm()
{
	/* Return 1 if the process has not the root perm */
	if(geteuid() != 0) return 1;

	/* Return -1 if the process fails accessing device */
	return ioperm(BASEPORT,1,1);
}

void led_off_all()
{	outb(0x0, BASEPORT); }

void led_on_all()
{	outb(0xff, BASEPORT); }

int led_on(int led)
{
	if(led > MAXLED || led <= 0) return -1;

	lights = (unsigned char) inb(BASEPORT) | 1 << (led - 1);
	
#ifdef DEBUG
	led_print_debug(lights, "led_on lights");
#endif

	outb(lights, BASEPORT);

	return 0;
}

int led_off(int led)
{
	if(led > MAXLED || led <= 0) return -1;

	lights = (unsigned char) inb(BASEPORT) & (0xff ^ (1 << (led - 1)));

#ifdef DEBUG
	led_print_debug(lights, "led_off lights");
#endif

	outb(lights, BASEPORT);

	return 0;
}

int led_set_on(int *state)
{
	int x;
	unsigned char hop[MAXLED];

	lights = 0;

	if(sizeof(state) > sizeof(int) * MAXLED) return -1;

	for(x=0; x < MAXLED; x++)
	{
		if(state[x] > 0)
			hop[x] = 1 << x;
		else
			hop[x] = 0;
		
		lights |= hop[x];
	}

#ifdef DEBUG
	led_print_debug(lights, "led_set lights");
#endif

	lights = lights | inb(BASEPORT);

	outb(lights, BASEPORT); 

	return 0;
}


int led_set_off(int *state)
{
	int x;
	unsigned char hop[MAXLED];

	lights = 0;

	if(sizeof(state) > sizeof(int) * MAXLED) return -1;

	for(x=0; x < MAXLED; x++)
	{
		if(state[x] > 0)
			hop[x] = 1 << x;
		else
			hop[x] = 0;

		lights |= hop[x];
	}

#ifdef DEBUG
	led_print_debug(lights, "led_set_rev lights");
#endif

	lights = inb(BASEPORT) ^ lights;

	outb(lights, BASEPORT); 

	return 0;
}


int __led_blink_thread(void * us_time)
{
	int i;
	int *_us_time;
	*_us_time = (int) us_time;

	while(1)
	{
		led_set_on(current_blinking_led);
		usleep(*_us_time);
		led_set_off(current_blinking_led);
		usleep(*_us_time);
		
		/* Loop for all leds: if we reach MAXLED+1, 
			 it means that there is no more blinking led, 
			 so pthread_exit() */

		for(i = 0; i < MAXLED+1; i++)
		{
			if(i < MAXLED)
				if(current_blinking_led[i] != 0)
					break;
				else
					continue;
			else 
				pthread_exit(NULL);
		}
	}
	return 0;
}

int led_blink_on(int led, int us_time)
{
	pthread_t blink;
	if(led > MAXLED || led <= 0) return -1;
	
	current_blinking_led[led-1] = us_time;

	if(blink_thread != 0)
		blink_thread = pthread_create(&blink, NULL, (void *) &__led_blink_thread, (void *) us_time);	

	return 0;
}

int led_blink_off(int led)
{
	int wait_loop;
	if(led > MAXLED || led <= 0) return -1;
	
	wait_loop = current_blinking_led[led-1];

	/* We should wait a loop time in order
		 to be SURE that the LED will be off */
	current_blinking_led[led-1] = 0;
	usleep(wait_loop * 2);
	led_off(led);

	return(0);
}

