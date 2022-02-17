/**
 * IMS project - Epidemiological model in macro scale
 *
 * @file main.cpp
 *
 * @author Natália Holkova (xholko02)
 * @author Martina Chripkova (xchrip01)
 */

#include "main.h"

int main(int argc, char *argv[]) {
    int opt, max_t = DEF_MAX_TIME;
	max_t = 30;
    std::string measures = "";
    std::string filename = "";

    while ((opt = getopt(argc, argv, "m:f:t:")) != -1) {
        switch (opt) {
            case 'm':
                measures = optarg;
                break;
            case 'f':
                filename = optarg;
                break;
            case 't':
                max_t = std::stoi(optarg);
                break;
        }
    }

    Data data;
    data.set_active_measures(measures);
    data.calculate_exposure_factor();
    SIR sir;
    sir.set_initial_data(&data, max_t, filename);
    sir.print_initial_data();
    sir.run_simulation();
}
