//
// Created by charles on 17/06/24.
//

#ifndef MULTI_VESC_MANAGER_HH
#define MULTI_VESC_MANAGER_HH

#include <multivesc/ComsInterface.hh>

namespace multivesc {


    //! Abstract class for managing multiple VESCs

    class Manager
    {


    protected:
        std::shared_ptr<ComsInterface> mPrimaryComs;
    };

} // multivesc

#endif //MULTI_VESC_MANAGER_HH
