#include <Arduino.h>
#include "pins.h"
#include "comms.h"
#include "SHETSource.h"

#define STUDIO1_GND 25
#define STUDIO1_SIGNAL 23

#define UNLOCK_TIME 2000

Comms comms = Comms(57600);

SHETSource::Client client = SHETSource::Client(&comms, "fuseduino");

SHETSource::LocalEvent *mic1_up;
SHETSource::LocalEvent *mic1_down;
SHETSource::LocalEvent *mic2_up;
SHETSource::LocalEvent *mic2_down;
SHETSource::LocalEvent *silence_up;
SHETSource::LocalEvent *silence_down;
SHETSource::LocalEvent *songs1_up;
SHETSource::LocalEvent *songs1_down;
SHETSource::LocalEvent *songs2_up;
SHETSource::LocalEvent *songs2_down;
SHETSource::LocalEvent *onAir1_up;
SHETSource::LocalEvent *onAir1_down;
SHETSource::LocalEvent *onAir2_up;
SHETSource::LocalEvent *onAir2_down;
SHETSource::LocalEvent *master1_up;
SHETSource::LocalEvent *master1_down;
SHETSource::LocalEvent *master2_up;
SHETSource::LocalEvent *master2_down;

struct lock {
	uint8_t state;
	unsigned long time;
	uint8_t pin_signal;
	uint8_t pin_gnd;
};

struct gpi {
	uint8_t pin_signal;
	uint8_t pin_gnd;
};

struct lock lock1 = {LOW, 0, 23, 25};
struct lock lock2 = {LOW, 0, 27, 29};
struct gpi silence = {24, 22};
struct gpi songs1 = {28, 26};
struct gpi songs2 = {32, 30};
struct gpi micLive1 = {36, 34};
struct gpi micLive2 = {40, 38};
struct gpi master = {44, 42};
struct gpi slaveOnAir = {48, 46};

bool mic1_wasup;
bool mic2_wasup;
bool silence_wasup;
bool onAir1_wasup;
bool onAir2_wasup;
bool songs1_wasup;
bool songs2_wasup;
bool master_wasup;

void unlock_studio(struct lock *l)
{
	digitalWrite(l->pin_signal, HIGH);
	l->state = HIGH;
	l->time = millis();
}

void setup_lock(struct lock *l)
{
	pinMode(l->pin_signal, OUTPUT);
	pinMode(l->pin_gnd, OUTPUT);
	digitalWrite(l->pin_gnd, LOW);
	l->state = LOW;
	l->time = 0;
}

void update_lock(struct lock *l)
{
	if (l->state == HIGH && l->time + UNLOCK_TIME < millis()) {
		digitalWrite(l->pin_signal, LOW);
		l->state = LOW;
	}
}

void unlock_studio1(void)
{
	unlock_studio(&lock1);
}

void unlock_studio2(void)
{
	unlock_studio(&lock2);
}

void setup_gpi(struct gpi *i)
{
	pinMode(i->pin_signal, INPUT);
	digitalWrite(i->pin_signal, HIGH);
	pinMode(i->pin_gnd, OUTPUT);
	digitalWrite(i->pin_gnd, LOW);
}

int get_micLive_studio1(void)
{
	digitalRead(micLive1.pin_signal);
}

int get_silence(void)
{
	digitalRead(silence.pin_signal);
}

int get_songs_studio1(void)
{
	digitalRead(songs1.pin_signal);
}

int get_songs_studio2(void)
{
	digitalRead(songs2.pin_signal);
}

int get_micLive_studio2(void)
{
	digitalRead(micLive2.pin_signal);
}

int get_isMaster_studio1(void)
{
	digitalRead(master.pin_signal);
}

int get_isMaster_studio2(void)
{
	if (digitalRead(master.pin_signal) == LOW) {
		return HIGH;
	} else {
		return LOW;
	}
}

int get_onAir_studio1(void)
{
	if ((digitalRead(master.pin_signal) == LOW) && (digitalRead(slaveOnAir.pin_signal) == LOW)) {
		return LOW;
	} else {
		return HIGH;
	}
}

int get_onAir_studio2(void)
{
	if ((digitalRead(master.pin_signal) == HIGH) && (digitalRead(slaveOnAir.pin_signal) == LOW)) {
		return LOW;
	} else {
		return HIGH;
	}
}


void setup()
{
	Serial.begin(57600);
	client.Init();
	setup_lock(&lock1);
	setup_lock(&lock2);
	setup_gpi(&micLive1);
	setup_gpi(&micLive2);
	setup_gpi(&silence);
	setup_gpi(&songs1);
	setup_gpi(&songs2);
	setup_gpi(&master);
	setup_gpi(&slaveOnAir);
	client.AddAction("studio1/unlock", unlock_studio1);
	client.AddAction("studio2/unlock", unlock_studio2);
	client.AddAction("studio1/micLive", get_micLive_studio1);
	mic1_up = client.AddEvent("studio1/micUp");
	mic1_down = client.AddEvent("studio1/micDown");
	client.AddAction("studio2/micLive", get_micLive_studio2);
	mic2_up = client.AddEvent("studio2/micUp");
	mic2_down = client.AddEvent("studio2/micDown");
	client.AddAction("silence", get_silence);
	silence_up = client.AddEvent("silenceUp");
	silence_down = client.AddEvent("silenceDown");
	client.AddAction("studio1/songs", get_songs_studio1);
	songs1_up = client.AddEvent("studio1/songsUp");
	songs1_down = client.AddEvent("studio1/songsDown");
	client.AddAction("studio2/songs", get_songs_studio2);
	songs2_up = client.AddEvent("studio2/songsUp");
	songs2_down = client.AddEvent("studio2/songsDown");
	client.AddAction("studio1/isMaster", get_isMaster_studio1);
	master1_up = client.AddEvent("studio1/isMasterUp");
	master1_down = client.AddEvent("studio1/isMasterDown");
	client.AddAction("studio2/isMaster", get_isMaster_studio2);
	master2_up = client.AddEvent("studio2/isMasterUp");
	master2_down = client.AddEvent("studio2/isMasterDown");
	client.AddAction("studio1/onAir", get_onAir_studio1);
	onAir1_up = client.AddEvent("studio1/onAirUp");
	onAir1_down = client.AddEvent("studio1/onAirDown");
	client.AddAction("studio2/onAir", get_onAir_studio2);
	onAir2_up = client.AddEvent("studio2/onAirUp");
	onAir2_down = client.AddEvent("studio2/onAirDown");
}

void loop()
{
	client.DoSHET();

	update_lock(&lock1);
	update_lock(&lock2);

	uint8_t micLive1 = digitalRead(36);
	if (micLive1 == LOW) {
		if (mic1_wasup) {
			mic1_wasup = false;
			(*mic1_down)(1);
		}
	} else {
		if (!mic1_wasup) {
			mic1_wasup = true;
			(*mic1_up)(1);
		}
	}
	uint8_t micLive2 = digitalRead(40);
	if (micLive2 == LOW) {
		if (mic2_wasup) {
			mic2_wasup = false;
			(*mic2_down)(1);
		}
	} else {
		if (!mic2_wasup) {
			mic2_wasup = true;
			(*mic2_up)(1);
		}
	}
	uint8_t silence = digitalRead(24);
	if (silence == LOW) {
		if (silence_wasup) {
			silence_wasup = false;
			(*silence_down)(1);
		}
	} else {
		if (!silence_wasup) {
			silence_wasup = true;
			(*silence_up)(1);
		}
	}
	uint8_t songs1 = digitalRead(28);
	if (songs1 == LOW) {
		if (songs1_wasup) {
			songs1_wasup = false;
			(*songs1_down)(1);
		}
	} else {
		if (!songs1_wasup) {
			songs1_wasup = true;
			(*songs1_up)(1);
		}
	}
	uint8_t songs2 = digitalRead(32);
	if (songs2 == LOW) {
		if (songs2_wasup) {
			songs2_wasup = false;
			(*songs2_down)(1);
		}
	} else {
		if (!songs2_wasup) {
			songs2_wasup = true;
			(*songs2_up)(1);
		}
	}
	bool onAir1 = ((digitalRead(44)==LOW)&&(digitalRead(48)==LOW));
	if (!onAir1) {
		if (onAir1_wasup) {
			onAir1_wasup = false;
			(*onAir1_down)(1);
		}
	} else {
		if (!onAir1_wasup) {
			onAir1_wasup = true;
			(*onAir1_up)(1);
		}
	}
	bool onAir2 = ((digitalRead(44)==HIGH)&&(digitalRead(48)==LOW));
	if (!onAir2) {
		if (onAir2_wasup) {
			onAir2_wasup = false;
			(*onAir2_down)(1);
		}
	} else {
		if (!onAir2_wasup) {
			onAir2_wasup = true;
			(*onAir2_down)(1);
		}
	}
	uint8_t master = digitalRead(46);
	if (master == LOW) {
		if (master_wasup) {
			master_wasup = false;
			(*master1_down)(1);
			(*master2_up)(1);
		}
	} else {
		if (!master_wasup) {
			master_wasup = true;
			(*master1_up)(1);
			(*master2_down)(1);
		}
	}

}
