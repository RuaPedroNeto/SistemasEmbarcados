#ifndef PID_CONSTANTS_H
#define PID_CONSTANTS_H

//! ------------------------- PID GAINS --------------------------
#define PROPORTIONAL_GAIN 0
#define INTEGRAL_GAIN     0.15721
#define DERIVATIVE_GAIN   0
// ---------------------------------------------------------------

#define SAMPLING_FREQUENCY          1000
#define DERIVATIVE_LOWPASS_CONSTANT 10

// ---------------------------------------------------------------
// Safety boundaries
#define MAX_INTEGRATOR_LIMIT 42
#define MIN_INTEGRATOR_LIMIT -42
#define MAX_OUTPUT           42
#define MIN_OUTPUT           42
// ---------------------------------------------------------------

#endif  // PID_CONSTANTS_H
