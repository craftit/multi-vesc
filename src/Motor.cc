//
// Created by charles on 17/06/24.
//

#include <utility>

#include "multivesc/Motor.hh"

namespace multivesc {

    Motor::Motor(std::shared_ptr<ComsInterface> coms, uint8_t id)
    : mComs(std::move(coms)),
      mId(id)
    {
    };

} // multivesc