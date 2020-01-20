/*
 * Author: George Karabassis
 * Co-Author: 
 * Organisation: HYPED
 * Date: 05/04/2019
 * Description: Navigation algorithm.
 *
 *    Copyright 2019 HYPED
 *    Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
 *    except in compliance with the License. You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software distributed under
 *    the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 *    either express or implied. See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include <iostream>
#include <vector>
#include "navigation/kalman_filter.hpp"
#include "navigation/navigation.hpp"
#include "IMUs_faker.hpp"

namespace hyped {
namespace navigation {

  Navigation::Navigation(Logger& log, int n, int m, int k, KalmanFilter& KF_)
    : log_(log)
    , sys_(System::getSystem())
    , data_(Data::getInstance())
    , KF_(KF_)
    , status_(ModuleStatus::kStart)
    , dt_(0)
    , m_(m)
    , n_(n)
    , k_(k)
  {}

  VectorXf Navigation::IMUQuerying()
  {
    // TODO(george): Make it better.

    // store IMU data
    DataPoint<ImuDataArray> sensorReadings = data_.getSensorsImuData();
    // Wheel encoders data is empty for now. Change the function as soon as there is one available in sensors
    DataPoint<ImuDataArray> wheelEncoders_data = data_.getSensorsImuData();

    Navigation::timestampUpdate();

    /* Outlier guys correct me:
    *
    * Outlier detection will return the updated version of the sensorReadings captured from above.
    * It will remove possible failed IMUs or values outside the range.
    */

    int failedIMUs = Navigation::OutlierDetection(sensorReadings, wheelEncoders_data);

    if (1 > failedIMUs) {  // more than one IMUs failed
      status_ = ModuleStatus::kCriticalFailure;
      log_.ERR("NAV", "More than 1 IMU had failed, entering kCriticalFailure");
    }
    else {
      // demo
      for (int j = 0; j < m_; ++j) {
        for (int i = 0; i < data::Sensors::kNumImus; ++i) {
          // if faulty imu value, retrun 0 acceleration
          // else
          z_(j*data::Sensors::kNumImus + i) = sensorReadings.value[i].acc[j];
          // acc_raw[i] = a[axis_];  // accNorm(a) * (1 - 2 * (a[axis_] < 0));
        }
      }
      // z_ = sensorReadings;  // Store IMU data to a measurement vector
    }
    return z_;
  }

  int Navigation::OutlierDetection(DataPoint<ImuDataArray> IMUdata, DataPoint<ImuDataArray> wheelEncoders_data)
  {
    // TODO(Outlier_lads): Complete.

    // For now, there are 0 failed IMUS for demo purposes
    return 0;
  };

  void Navigation::KFCalc(int i)
  {
    // The way KF works is subject to change to use the stacking KF technique.
    // Will work through it soon
    
    VectorXf s(n_);
    s = KF_.getState();

    // for demo purposes only
    demoIMUData IMUdemo = demoIMUData("fake_state_acc.txt", m_);
    vector< vector<float> > data = IMUdemo.getData();

    for (int d = 0; d < m_; d++) {
        z_(d) = data[i][d+1];
    }
    
    // Filtering - predict next state.
    if (i == 0) {
        KF_.filter(0.05, s, z_);
    } else {
        dt_ = (float)0.001*(data[i][0] - data[i-1][0]);
        KF_.filter(dt_, s, z_);
    }
  };

  void Navigation::StackingKFCalc()
  {
    // TODO(george): Complete.
  };

  void Navigation::QueryKeyences()
  {
    // TODO(george): Complete.
  };

  bool Navigation::SensorsResultsSimilarity()
  {
    // TODO(george): Complete.
    return true;
  };

  void Navigation::timestampUpdate()
  {
    DataPoint<ImuDataArray> sensorReadings = data_.getSensorsImuData();

    uint32_t t = sensorReadings.timestamp;
    dt_ = t - prev_timestamp_;
    prev_timestamp_ = t;
  }

  bool Navigation::calibrateGravity()
  {
    // TODO(george): Complete.
    return true;
  }

  // VectorXf Navigation::getState()
  // {
  //   return KF_.getState();
  // }

  void Navigation::run(int i)
  {
    Navigation::IMUQuerying();
    // TODO(george): Complete.
    
    // Navigation::IMUQuerying();

    // Navigation::Outlier_detection();
  };

}}  // namespace hyped navigation
