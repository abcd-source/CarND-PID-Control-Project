#include "CA.h"

void CoordinateAscent::Step()
{
    if (active_experiment == Experiment::INCREASE) {
        p[parameter_index] += dp[parameter_index];
        std::cout << "Incrementing index " << parameter_index
                  << " by " << dp[parameter_index] << " new value = "
                  << p[parameter_index] << std::endl;
    }
    else if (active_experiment == Experiment::DECREASE) {
        p[parameter_index] -= 2.0 * dp[parameter_index];
        std::cout << "Decrementing index " << parameter_index
                  << " by " << dp[parameter_index] << " new value = "
                  << p[parameter_index] << std::endl;
    }
}

void CoordinateAscent::UpdateIndex() {
    // Increment the parameter index (with roll over)
    parameter_index = (parameter_index + 1) % p.size();
    std::cout << "New experimentation index " << parameter_index << std::endl;

    // Reset the experiment by setting flags to false
    // and default INCREASE direction (must always run first)
    active_experiment = Experiment::INCREASE;
    tested_increase = false;
    tested_decrease = false;

    Step();
}

bool CoordinateAscent::Run(std::vector<double>error, bool optimize)
{
    // Early out if optimization should not run
    if (!optimize) return false;

    if ((++execution_index % SAMPLE) != 0) {
        return false;
    }

    std::cout << "\n\nNew experiment iteration #" << ++experiment_number << std::endl;

    // Find the total cross track error across all of the samples
    double new_error = std::accumulate(error.begin(), error.end(), 0.0);

    // Print out the results of the experiment
    std::cout.precision(8);
    std::cout << std::fixed << "New error = " << new_error << " with "
              << " Kp = " << p[0]
              << " Ki = " << p[1]
              << " Kd = " << p[2] << std::endl;

    if (best_error == std::numeric_limits<double>::max())
    {
        // For the first execution, we need to set a baseline
        // error value of what to expect over SAMPLE data points
        best_error = new_error;
        std::cout << "Setting BASELINE best error = " << best_error << std::endl;

        // After setting the baseline, start by incrementing index 0
        active_experiment = Experiment::INCREASE;
        Step();

        return true;
    }

    // For all other executions, after gathering SAMPLE data points,
    // evaluate if the new error is less than the previous "best error"
    if (new_error < best_error ) {
        std::cout << "SUCCESS: NEW BEST ERROR  = " << new_error
                  << " previous error = " << best_error << std::endl;

        best_error = new_error;

        if (active_experiment == Experiment::INCREASE)
        {
            std::cout << "Increasing coefficient " << parameter_index
                      << " to a value of " << p[parameter_index]
                      << " improved the total error" << std::endl;
        }
        else
        {
            std::cout << "Decreasing coefficient " << parameter_index
                      << " to a value of " << p[parameter_index]
                      << " improved the total error" << std::endl;
        }

        UpdateIndex();
    }
    else if (new_error >= best_error) {
        // The used parameters did not result in an improvement
        // to the score, so change the experiment
        std::cout << "FAILURE: New error is greater than previous" << std::endl;

        if (tested_increase && tested_decrease) {
            std::cout << "Tested increasing and decreasing index = " << parameter_index
                      << " and neither resulted in a decrease in total error." << std::endl;

            // Neither increasing nor decreasing helped,
            // so try a smaller step size in the next round
            dp[parameter_index] *= 0.9;
            std::cout << "Changed delta p of index " << parameter_index
                      << " to " << dp[parameter_index] << std::endl;

            UpdateIndex();
        }
        else if (active_experiment == Experiment::INCREASE) {
            std::cout << "Increasing parameter " << parameter_index << " to "
                      << p[parameter_index] << " failed to improve error" << std::endl;

            tested_increase = true;
            active_experiment = Experiment::DECREASE;
            Step();
        }
        else if (active_experiment == Experiment::DECREASE)
        {
            std::cout << "Decreasing parameter " << parameter_index << " to "
                      << p[parameter_index] << " failed to improve error" << std::endl;

            tested_decrease = true;
            active_experiment = Experiment::INCREASE;
            Step();
        }
    }

    return true;
}