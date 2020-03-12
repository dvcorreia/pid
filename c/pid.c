typedef struct _controller{

    int Kp;
    int Ki;
    int Kd;

    int uBound;
    int lBound;

    int accum; 
    int prev;

    int dt;

    int * target; // Setpoint
    int * input;  // Sensor
    int * output; // Control Signal
    
} * controller;

// Creates and initializes new controler
controller pid_new(controller * pid, int Kp, int Ki, int Kd); 

// Update controler parameters
void pid_tune(controller * pid, int Kp, int Ki, int Kd);

// Update Bounds
void pid_bounds(controller * pid, int lBound, int uBound);

// Update Sampling Freq
void pid_dt(controller * pid, int dt);

// Set Target
void pid_target(controller * pid, int * tgt);

// Set Sensor
void pid_input(controller * pid, int * input);

// Computes next control output
void pid_compute(controller * pid);
