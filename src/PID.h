#ifndef PID_H
#define PID_H

#include <numeric>
#include <array>
#include <vector>
#include <iostream>
#include <cmath>

class PID {
 public:
  /**
   * Constructor
   */
  PID();

  /**
   * Destructor.
   */
  virtual ~PID();

  /**
   * Initialize PID.
   * @param (Kp_, Ki_, Kd_) The initial PID coefficients
   */
  void Init(double Kp_, double Ki_, double Kd_);

  /**
   * Update the PID error variables given cross track error.
   * @param cte The current cross track error
   */
  void UpdateError(double cte);

  void UpdateCoeffs(const std::array<double, 3> p) {
      Kp = p[0];
      Ki = p[1];
      Kd = p[2];
  }

  double P() { return Kp; }
  double I() { return Ki; }
  double D() { return Kd; }
  double average_error() { return error() / cte_history.size(); }
  double error() { return std::accumulate(cte_history.begin(),
                                   cte_history.end(), 0.0); }


  /**
   * Calculate the total PID error.
   * @output The total PID error
   */
  double TotalError();

  std::vector<double> cte_history;

 private:
  /**
   * PID Errors
   */
  double p_error;
  double i_error;
  double d_error;

  double previous_cte;
  double sum_cte;

  /**
   * PID Coefficients
   */ 
  double Kp;
  double Ki;
  double Kd;
};

#endif  // PID_H