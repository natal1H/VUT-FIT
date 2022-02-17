/**
 * IMS project - Epidemiological model in macro scale
 *
 * @file discreteSIR.h
 *
 * @author Natália Holkova (xholko02)
 * @author Martina Chripkova (xchrip01)
 */

#ifndef  _DISCRETE_SIR_H
    #define _DISCRETE_SIR_H

    #include <iostream>
    #include <cstdio>
    #include <cmath>
    #include <fstream>

    #include "data.h"

    #define DEF_MAX_TIME 30

    class SIR {
    private:
        double beta; // growth rate
        double gamma; // recovery and death rate
        double exposure_factor; // Factor describing active countermeasures
        double N; // total population size
        double S, S0;
        double I, I0;
        double R, R0;
        double dS, dI, dR;
        int t; // time
        int max_t; // max time - in days
        std::ofstream output_file; // output file for later plotting
    public:
        SIR();
        void set_initial_data(Data *data, int max_t, std::string filename);
        void print_initial_data();
        void output_data();
        void run_simulation();
    };

#endif