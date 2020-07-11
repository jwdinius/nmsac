//! c/c++ headers
//! dependency headers
#include <armadillo>
//! project headers

/**
 * @brief Construct roll-pitch-yaw rotation matrix from angles
 *
 * @param [in] yaw rotation angle about z-axis
 * @param [in] pitch rotation angle about y-axis
 * @param [in] roll rotation angle about x-axis
 * @param [out] R rpy rotation matrix
 * @return
 */
void make_euler(double const & yaw, double const & pitch, double const & roll, arma::mat33 & R) {
    //! yaw
    auto const & psi = yaw;
    double const s_psi = std::sin(psi);
    double const c_psi = std::cos(psi);
    arma::mat33 const Ry = {{c_psi, -s_psi, 0}, {s_psi, c_psi, 0}, {0, 0, 1}};
    //! pitch
    auto const & theta = pitch;
    double const s_theta = std::sin(theta);
    double const c_theta = std::cos(theta);
    arma::mat33 const Rp = {{c_theta, 0, s_theta}, {0, 1, 0}, {-s_theta, 0, c_theta}};
    //! roll
    auto const & phi = roll;
    double const s_phi = std::sin(phi);
    double const c_phi = std::cos(phi);
    arma::mat33 const Rr = {{1, 0, 0}, {0, c_phi, -s_phi}, {0, s_phi, c_phi}};

    //! rpy sequence
    R = Ry * Rp * Rr;
}

/**
 * @brief Compute Euler angles from rotation matrix
 *
 * @param [in] R rpy rotation matrix
 * @param [out] angles column vector with (roll, pitch, yaw) angles of rpy rotation sequence
 * @param [in] flip_pitch (optional, default=false) use alternate convention for pitch angle
 * @return
 *
 * @note this routine assumes input matrix is a valid rotation matrix
 */
void find_euler_angles(arma::mat33 const & R, arma::vec3 & angles, bool flip_pitch = false) {
    auto const & r_31 = R(2, 0);
    if (std::abs(r_31 - 1.0) <= std::numeric_limits<double>::epsilon() ||
            std::abs(r_31 + 1.0) <= std::numeric_limits<double>::epsilon()) {
        //! gimbal lock condition
        angles(2) = static_cast<double>(0);
        if (r_31 < 0) {
            angles(1) = 0.5 * M_PI;
            angles(0) = angles(2) + std::atan2(R(0, 1), R(0, 2));
        } else {
            angles(1) = -0.5 * M_PI;
            angles(0) = -angles(2) + std::atan2(-R(0, 1), -R(0, 2));
        }
    } else {
        if (flip_pitch) {
            angles(1) = M_PI + std::asin(r_31);
        } else {
            angles(1) = -std::asin(r_31);
        }
        angles(0) = std::atan2(R(2, 1) / std::cos(angles(0)), R(2, 2) / std::cos(angles(0)));
        angles(2) = std::atan2(R(1, 0) / std::cos(angles(0)), R(0, 0) / std::cos(angles(0)));
    }
}
