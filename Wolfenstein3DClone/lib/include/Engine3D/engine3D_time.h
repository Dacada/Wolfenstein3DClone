#ifndef ENGINE3D_TIME_H
#define ENGINE3D_TIME_H

// Conversion unit to convert the result of
// engine3D_timer_getTime into seconds
extern const double engine3D_timer_second;

// Initialize timer
void engine3D_timer_init(void);

// Get time since an unspecified moment.
// The units are such that if the returned value is divided by
// engine3D_timer_second, the resulting time is in seconds.
double engine3D_timer_getTime(void);

// Get value of delta
double engine3D_time_getDelta(void);

// Set value of delta
void engine3D_time_setDelta(const double d);

// Sleep the current thread for the given length of time
// Time is in the internal module's units
// engine3D_timer_second can be used to convert from seconds
void engine3D_time_sleep(const double t);

#endif /* ENGINE3D_TIME_H */
