/**
 * IMS project - Epidemiological model in macro scale
 *
 * @file discreteSIR.cpp
 *
 * @author Natália Holkova (xholko02)
 * @author Martina Chripkova (xchrip01)
 */

#include "sir.h"

/**
 * Create object with default
 */
SIR::SIR() {
    beta = 0.05634007047; // calculated beforehand
    gamma = 0.05524861878; // calculated beforehand
}

/**
 * Set initial values N, S0, I0, R0, exposure_factor for simulation
 * @param data Object from where values are taken
 */
void SIR::set_initial_data(Data *data, int max_t, std::string filename) {
    this->N = data->get_total_population();
    this->I0 = data->get_infected() / this->N;
    this->R0 = data->get_recovered() / this->N;
    this->S0 = 1 - this->I0 - this->R0;
    this->exposure_factor = data->calculate_exposure_factor();
    this->max_t = max_t;

    if (filename.length() > 0) {
        output_file.open(filename);
        output_file << "# exposure_factor " << this->exposure_factor << std::endl;
    }
}

/**
 * Prints initial values S0, I0, R0 and parameters beta, gamma, exposure_factor
 */
void SIR::print_initial_data() {
    std::cout << "SIR model initial data:" << std::endl;
    std::cout << "S0 = " << S0 << std::endl;
    std::cout << "I0 = " << I0 << std::endl;
    std::cout << "R0 = " << R0 << std::endl;
    std::cout << "beta = " << beta << std::endl;
    std::cout << "gamma = " << gamma << std::endl;
    std::cout << "exposure_factor = " << exposure_factor << std::endl;
}

/**
 * Outputs current value of daily infected
 */
void SIR::output_data() {
    printf("%d %d\n", t, (int)round(-dS * N));
    if (output_file.is_open())
        output_file << t << " " << (int)round(-dS * N) << std::endl;
}

/**
 * Run the simulation
 */
void SIR::run_simulation() {
    // set up initial conditions
    double prevS = S0;
    double prevI = I0;
    double prevR = R0;

    printf("t new_infected\n");
    for (t = 1; t <= max_t; t++) {
        dS = -beta * exposure_factor * prevS * prevI;
        dI = beta * exposure_factor * prevS * prevI - gamma * prevI;
        dR = gamma * prevI;

        S = prevS + dS;
        I = prevI + dI;
        R = prevR + dR;
        output_data();

        prevS = S;
        prevI = I;
        prevR = R;
    }

    if (output_file.is_open()) {
        output_file.close();
    }
}
