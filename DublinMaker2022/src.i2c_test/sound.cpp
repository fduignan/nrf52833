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
	k_timer_init(&sound_timer, this->ms_callback, NULL);
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
    int Index;
    for (Index = 0; Index < Len; Index++)
    {
        while(SoundPlaying()); // wait for previous note to complete        
        playTone(Tones[Index],Times[Index]);               
    }
}
void sound::ms_callback(struct k_timer *timer_id)
{
    // This callback should run every millisecond.  It is attached to the Timer object in console.cpp:begin
    if (pSound->tone_time)
    {
        pSound->tone_time--;
        if (pSound->tone_time == 0)
        {
            pSound->stopTone();
        }
    }
}
