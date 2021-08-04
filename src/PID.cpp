#include "PID.h"

/**
 * TODO: Complete the PID class. You may add any additional desired functions.
 */

PID::PID() {}

PID::~PID() {}

void PID::Init(double Kp_, double Ki_, double Kd_) {
  /**
   * TODO: Initialize PID coefficients (and errors, if needed)
   */
   Kp = Kp_;
   Ki = Ki_;
   Kd = Kd_;

   p_error = 0;
   i_error = 0;
   d_error = 0;

   previous_cte = 0;
}

void PID::UpdateError(double cte) {
  /**
   * TODO: Update PID errors based on cte.
   */
  p_error = -Kp * (cte);
  i_error = -Ki * (sum_cte);
  d_error = -Kd * (cte - previous_cte);

  sum_cte += cte;
  previous_cte = cte;

  cte_history.push_back(fabs(cte));
  if (fabs(sum_cte) > 100)
  {
      // reset to prevent controller windup
      sum_cte = 0;
  }
}

double PID::TotalError() {
  /**
   * TODO: Calculate and return the total error
   */
  return (p_error + i_error + d_error);  // TODO: Add your total error calc here!
}