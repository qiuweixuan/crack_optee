#ifndef GENERATE_KEY_H
#define GENERATE_KEY_H

#include "tee_key.h"

namespace crack
{
    namespace generate_key
    {
        void tee_otp_get_hw_unique_key(struct crack::tee_key::tee_hw_unique_key *hwkey);

        void tee_fs_init_key_manager();
    } // namespace generate_key
} // namespace crack

#endif /* GENERATE_KEY_H */
