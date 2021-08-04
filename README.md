[//]: # (Image References)

[results]: ./results.jpg "Results"

# CarND-Controls-PID
Self-Driving Car Engineer Nanodegree Program

---

## Overview
In this project I implemented a simple PID controller within the project framework. Additionally I implemented a Coordinate Ascent class which was used to optimize the PID parameters by runnning consecutive experiments where the simulated vehicle drove around the track, and I collected the sum() of the cross track error and compared the total error with different parameters to a "best error". This process was slow since it required the vehicle travel completely around the track to get the full variance of error around the different parts of the track geometry, however the final optimized results show a significant improvement compared to the originally selected PID coefficients.

## Implementation

The PID implementation is quite simple as just the sum() of the three error terms, the porportional term: ```Kp*p_error```, the integral term ```Ki*i_error```, and the derivative term ```Kd*d_error```. Where the p_error is simply the linear cross track error, the i_error is the sum of the linear errors (reset to zero to avoid windup), and the d_error is the the delta between the current and previous linear errors. Note that the d_error should in theory be divided by the delta time between samples, however, we can assume that the sample time is fixed, so this term can be simply incorporated into the coefficient ```Kd```.

## Results

The below image shows the cross track error (instaneous as well as average over time) between two sets of PID coefficients, the first being a hand tuned verion which produced the coefficients: (Kp = 0.15, KI = 0.001, Kd = 2.0 ) and the second being the coefficients that resulted from the parameter optimization function defined in the lession as "twiddle". The parameter optimization algorithm is defined in the ```CoordinateAscent``` class in CA.cpp. The parameter optimization algorithm was run for 232 iterations and produced the coefficients
Kp = 0.29876313 Ki = 0.00010000 Kd = 6.86093125.

![alt text][results]

## Additional Improvements

Even with the optimization process, the performance of the lane following is quite poor and would result in a bad driver experience. I attempted to add another error parameter as the double derivative of the cross track error (defined as the acceleration term) but this did not result in a measurable improvement to the total cross track error.

It may be possible that the seed parameters for the optimization created a local minimum with respect to performance, or that I need to modify the derivative term of the error function to take into consideration more than one previous sample.