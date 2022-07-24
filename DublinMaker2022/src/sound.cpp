#include "sound.h"
#include <stdint.h>

#define PWM_PIN 0
static sound *pSound;
static const struct device *pwm;

int sound::begin()
{
	pSound = this;
	pwm = device_get_binding("PWM_0");
	if (pwm == NULL)
	{
		printk("Error acquiring PWM\r\n");
		return -1;
		
	}
	stopTone();
	k_timer_init(&sound_timer, sound_timer_handler, NULL);
	k_timer_start(&sound_timer, K_MSEC(1), K_MSEC(1));
	return 0;
}
void sound::playTone(uint16_t Frequency, uint16_t ms)
{
	int ret;
	if (Frequency == 0)
		return;
	uint32_t Period = (uint32_t)BASE_PWM_CLOCK / (uint32_t)Frequency; 	
	ret = pwm_pin_set_cycles(pwm, PWM_PIN, Period,Period/2,0);
    pSound->tone_time = ms;    
}
void sound::stopTone()
{
	pwm_pin_set_cycles(pwm, PWM_PIN, BASE_PWM_CLOCK / 1000,0,0);	
}

int  sound::SoundPlaying()
{
	
    if (tone_time > 0)
        return 1;
    else        
        return 0;
}
void sound::playMelody(const uint16_t *Tones,const uint16_t *Times,int Len)
{
	// NOTE: The Tones and Times arrays must continue to exist in their original
	// context for the duration of the melody.  They are not copied to a local buffer
	this->melody_tones = Tones;
	this->melody_times = Times;
	this->melody_length=Len;
    tone_callback();
}
void sound::tone_callback()
{
	pSound->stopTone();
	// Should check to see if there are more notes to play
	if (melody_length)
	{
		playTone(*melody_tones, *melody_times);
		melody_tones++;
		melody_times++;
		melody_length--;
	}
}

void sound_expiry_handler(struct k_work *work)
{
	// If this note is done then notify the rest of the sound object
	if (pSound->tone_time)
    {
        pSound->tone_time--;
        if (pSound->tone_time == 0)
        {
            pSound->tone_callback();
        }
    }
	
}
K_WORK_DEFINE(my_sound_timer,sound_expiry_handler);

void sound_timer_handler(struct k_timer *timer_id)
{
	// This is in interrupt context so have to schedule work to be done in process context
	k_work_submit(&my_sound_timer);
}
