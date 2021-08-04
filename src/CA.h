#ifndef PID_CA_H
#include <numeric>
#include <iostream>
#include <cmath>
#include <array>
#include <vector>

class CoordinateAscent {
public:
    CoordinateAscent() {};
    virtual ~CoordinateAscent() {};

    void Init(const double Kp, const double Ki, const double Kd) {
        p[0] = Kp;
        p[1] = Ki;
        p[2] = Kd;

        // Adjust the parameters by 10% to start
        dp[0] = p[0] * 0.1;
        dp[1] = p[1] * 0.1;
        dp[2] = p[2] * 0.1;
    }

    bool Run(std::vector<double>error, bool optimize);
    void Step();
    void UpdateIndex();

    std::array<double, 3> Coefficients() { return p; };

private:
    std::array<double, 3> p;
    std::array<double, 3> dp;

    enum Experiment {
        INCREASE,
        DECREASE
    };

    static constexpr size_t SAMPLE = 15000;
    size_t parameter_index = 0;
    size_t execution_index = 0;
    size_t experiment_number = 0;

    Experiment active_experiment = Experiment::INCREASE;
    bool tested_increase = false;
    bool tested_decrease = false;

    double best_error = std::numeric_limits<double>::max();
};

#define PID_CA_H

#endif //PID_CA_H
