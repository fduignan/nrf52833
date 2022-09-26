#ifndef __sound_h
#define __sound_h
#include <stdint.h>
#include <zephyr/zephyr.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/device.h>
#include "musical_notes.h"
#define STARTUP_JINGLE_LENGTH 13

const uint16_t DefaultStartupJingleTones[STARTUP_JINGLE_LENGTH]={3*1047,3*1319,3*1175,3*1397,3*1319,3*1568,3*1397,3*1047,3*1047,3*1568,3*1047,3*1568,3*1047};
const uint16_t DefaultStartupJingleTimes[STARTUP_JINGLE_LENGTH]={200,200,200,200,200,200,200,200,200,200,200,200,200};
class sound
{
public:
    sound(){};
    int begin();
    void playTone(uint16_t Frequency, uint16_t ms);
    static void stopTone();
    int  SoundPlaying();
    void playMelody(const uint16_t *Tones,const uint16_t *Times,int Len);
    void tone_callback();
	
private:
    volatile uint32_t tone_time;          
	static const uint32_t BASE_PWM_CLOCK=16000000;	
	struct k_timer sound_timer;
	friend void sound_expiry_handler(struct k_work *work);
	const uint16_t *melody_tones,*melody_times;
	uint16_t melody_length;

};
void sound_timer_handler(struct k_timer *timer_id);

#endif
