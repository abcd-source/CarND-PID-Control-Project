#include <math.h>
#include <uWS/uWS.h>
#include <iostream>
#include <string>
#include "json.hpp"
#include "PID.h"
#include "CA.h"
#include <iostream>
#include <fstream>
#include <time.h>

// for convenience
using nlohmann::json;
using std::string;

// For converting back and forth between radians and degrees.
constexpr double pi() { return M_PI; }
double deg2rad(double x) { return x * pi() / 180; }
double rad2deg(double x) { return x * 180 / pi(); }

// Checks if the SocketIO event has JSON data.
// If there is data the JSON object in string format will be returned,
// else the empty string "" will be returned.
string hasData(string s) {
  auto found_null = s.find("null");
  auto b1 = s.find_first_of("[");
  auto b2 = s.find_last_of("]");
  if (found_null != string::npos) {
    return "";
  }
  else if (b1 != string::npos && b2 != string::npos) {
    return s.substr(b1, b2 - b1 + 1);
  }
  return "";
}

int main() {
  uWS::Hub h;

  PID pid;
  CoordinateAscent ca;

  /**
   * TODO: Initialize the pid variable.
   */
  /* After 232 iterations of the optimization loop, the best
   * parameters were Kp = 0.29876313 Ki = 0.00010000 Kd = 6.86093125
  */
  static constexpr double KP = 0.29876313; // Hand Tuned: 0.15;
  static constexpr double KI = 0.00010000; // Hand Tuned: 0.0001;
  static constexpr double KD = 6.86093125; // Hand Tuned: 2.0;
  static constexpr bool RUN_COORDINATE_ASCENT = false;

  pid.Init(KP, KI, KD);
  ca.Init(KP, KI, KD);

  std::ofstream results ("results-optimized.csv");
  results << "dt" << "," << "cte" << "," << "cte_sum" << "," << "steer_value" << "," << "speed" << "\n";
  clock_t initial_time = 0;

  int subsampler = 0;
  static constexpr size_t SAMPLE = 10;

  h.onMessage([&](uWS::WebSocket<uWS::SERVER> ws, char *data, size_t length,
                     uWS::OpCode opCode) {
    // "42" at the start of the message means there's a websocket message event.
    // The 4 signifies a websocket message
    // The 2 signifies a websocket event
    if (length && length > 2 && data[0] == '4' && data[1] == '2') {
      auto s = hasData(string(data).substr(0, length));

      if (s != "") {
        auto j = json::parse(s);

        string event = j[0].get<string>();

        if (event == "telemetry") {
          // j[1] is the data JSON object
          double cte = std::stod(j[1]["cte"].get<string>());
          double speed = std::stod(j[1]["speed"].get<string>());
          double angle = std::stod(j[1]["steering_angle"].get<string>());
          double steer_value;
          /**
           * TODO: Calculate steering value here, remember the steering value is
           *   [-1, 1].
           * NOTE: Feel free to play around with the throttle and speed.
           *   Maybe use another PID controller to control the speed!
           */
          initial_time = (initial_time == 0) ? clock() : initial_time;

          pid.UpdateError(cte);
          steer_value = std::max(-1.0, std::min(1.0, pid.TotalError()));

          // Write the output data to a results.csv file
          // for post run analysis and plotting
          float dt = (initial_time != 0) ? float(clock() - initial_time) : 0;
          if (results.is_open()) {
              if ((subsampler++ % SAMPLE) == 0) {
                  results << std::fixed << float(dt/CLOCKS_PER_SEC) << ","
                          << cte << "," << pid.average_error() << "," << steer_value << "," << speed << "\n";
              }
          }
          else {
              std::cout << "Failed to open results file, exiting";
              exit(-1);
          }

          // Update the controller with new coefficients
          // based on the coordinate ascent algorithm.
          if (ca.Run(pid.cte_history, RUN_COORDINATE_ASCENT))
          {
              pid.UpdateCoeffs(ca.Coefficients());
              std::cout << "Updated coeffs "
                        << " Kp = " << pid.P()
                        << " Ki = " << pid.I()
                        << " Kd = " << pid.D() << std::endl;

              pid.cte_history.clear();
          }

          json msgJson;
          msgJson["steering_angle"] = steer_value;
          msgJson["throttle"] = 0.3;
          auto msg = "42[\"steer\"," + msgJson.dump() + "]";
          //std::cout << msg << std::endl;
          ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
        }  // end "telemetry" if
      } else {
        // Manual driving
        string msg = "42[\"manual\",{}]";
        ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
      }
    }  // end websocket message if
  }); // end h.onMessage

  h.onConnection([&h](uWS::WebSocket<uWS::SERVER> ws, uWS::HttpRequest req) {
    std::cout << "Connected!!!" << std::endl;
  });

  h.onDisconnection([&h](uWS::WebSocket<uWS::SERVER> ws, int code, 
                         char *message, size_t length) {
    ws.close();
    std::cout << "Disconnected" << std::endl;
  });

  int port = 4567;
  if (h.listen(port)) {
    std::cout << "Listening to port " << port << std::endl;
  } else {
    std::cerr << "Failed to listen to port" << std::endl;
    results.close();
    return -1;
  }
  
  h.run();
}