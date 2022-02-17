/**
 * IMS project - Epidemiological model in macro scale
 *
 * @file data.h
 *
 * @author Natália Holkova (xholko02)
 * @author Martina Chripkova (xchrip01)
 */

#ifndef  _DATA_H
    #define _DATA_H

    #include <map>
    #include <string>
    #include <vector>
    #include <sstream>

    class Data {
        private:
            double size; // Total size of population
            int infected, recovered; // Initial susceptibles, infecteds and recovereds (recovered + dead)
            std::map<std::string, double> measuresMap;
            std::vector<std::string> activeMeasures;
        public:
            Data();
            void set_active_measures(std::string active);
            double calculate_exposure_factor();
            int get_infected();
            int get_recovered();
            double get_total_population();
    };

    std::vector<std::string> split(const std::string &s, char delim);
#endif
