#pragma once

#include <vector>
#include <string>

#define NUM_ATTRS 5    // 5 attributes in total
#define NUM_RATINGS 7  // 7 ratings in total

enum attr {
    ATTR_UNKNOWN = -1, // indicate the attribute is unknown
    ATTR_WC_TA = 0,    // 0
    ATTR_RE_TA,        // 1
    ATTR_EBIT_TA,      // 2
    ATTR_MVE_BVTD,     // 3
    ATTR_S_TA          // 4
};
typedef enum attr attr_t;

enum rating {
    RATING_UNKNOWN = -1, // indicate the rating is unknown
    RATING_AAA = 0,      // 0
    RATING_AA,           // 1
    RATING_A,            // 2
    RATING_BBB,          // 3
    RATING_BB,           // 4
    RATING_B,            // 5
    RATING_CCC           // 6
};
typedef enum rating rating_t;

struct node {
    size_t id;             // id used to identify uniquely the node
    attr_t attr;           // attribute attached to node
    double splitval;       // attribute splitval attached to node
                           // nodes with attribute value <= splitval go to left child node
                           // nodes with attribute value > splitval got to right child node
    struct node *left;     // left child node
    struct node *right;    // right child node
    rating_t label;        // label (rating) attached to node
    bool isleaf;           // is the node a leaf or not
};
typedef node node_t;

rating_t str2rating(const std::string &rating);
std::string rating2str(rating_t rating);
std::string attr2str(attr_t attr);
bool create_training_data_set(
    const char *filename, std::vector<std::vector<double> *> &train_x,
    std::vector<rating_t> &train_y);
bool create_testing_data_set(
    const char *filename, std::vector<std::vector<double> *> &test_x);
