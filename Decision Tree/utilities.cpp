#include "creditrating.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

// convert string to rating_t
rating_t str2rating(const std::string &rating) {
    if (rating == "AAA")
        return RATING_AAA;
    if (rating == "AA")
        return RATING_AA;
    if (rating == "A")
        return RATING_A;
    if (rating == "BBB")
        return RATING_BBB;
    if (rating == "BB")
        return RATING_BB;
    if (rating == "B")
        return RATING_B;
    if (rating == "CCC")
        return RATING_CCC;
    // for any other strings, return rating RATING_UNKNOWN
    return RATING_UNKNOWN;
}

// convert rating_t to string
std::string rating2str(rating_t rating) {
    switch (rating) {
        case RATING_AAA:
            return "AAA";
        case RATING_AA:
            return "AA";
        case RATING_A:
            return "A";
        case RATING_BBB:
            return "BBB";
        case RATING_BB:
            return "BB";
        case RATING_B:
            return "B";
        case RATING_CCC:
            return "CCC";
        default:
            // for any other ratings, return string "unknown"
            return "unknown";
    }
}

// convert attr_t to string
std::string attr2str(attr_t attr) {
    switch (attr) {
        case ATTR_WC_TA:
            return "WC_TA";
        case ATTR_RE_TA:
            return "RE_TA";
        case ATTR_EBIT_TA:
            return "EBIT_TA";
        case ATTR_MVE_BVTD:
            return "MVE_BVTD";
        case ATTR_S_TA:
            return "S_TA";
        default:
            // for any other attributes, return string "unknown"
            return "unknown";
    }
}

// create training data set from file with the given filename
// after return successfully (true), the train_x and train_y will be filled with
// data read from file
bool create_training_data_set(
    const char *filename,
    std::vector<std::vector<double> *> &train_x,
    std::vector<rating_t> &train_y)
{
    std::ifstream ifs(filename);
    if (!ifs) {
        // cannot read file
        return false;
    }
    std::string line;
    std::getline(ifs, line); // skip the header row
    while (std::getline(ifs, line)) { // read row by row
        std::istringstream iss(line);
        double wc_ta, re_ta, ebit_ta, mve_bvtd, s_ta;
        std::string rating;
        // try reading attribute values and rating
        if (iss >> wc_ta >> re_ta >> ebit_ta >> mve_bvtd >> s_ta >> rating) {
            // append attributes to train_x
            train_x.push_back(new std::vector<double>{
                wc_ta, re_ta, ebit_ta, mve_bvtd, s_ta
            });
            // append rating to train_y
            train_y.push_back(str2rating(rating));
        } else {
            break;
        }
    }
    return true;
}

// create testing data set from file with the given filename
// after return successfully (true), the test_x will be filled with
// data read from file
bool create_testing_data_set(
    const char *filename,
    std::vector<std::vector<double> *> &test_x)
{
    std::ifstream ifs(filename);
    if (!ifs) {
        // cannot read file
        return false;
    }
    std::string line;
    std::getline(ifs, line); // skip the header row
    while (std::getline(ifs, line)) { // read row by row
        std::istringstream iss(line);
        double wc_ta, re_ta, ebit_ta, mve_bvtd, s_ta;
        // try reading attribute values and rating
        if (iss >> wc_ta >> re_ta >> ebit_ta >> mve_bvtd >> s_ta) {
            // append attributes to test_x
            test_x.push_back(
                new std::vector<double>{wc_ta, re_ta, ebit_ta, mve_bvtd, s_ta});
        } else {
            break;
        }
    }
    return true;
}
