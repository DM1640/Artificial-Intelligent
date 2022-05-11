#include "creditrating.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <cmath>
#include <cassert>

// calculate information content: I(R)
double calc_information(
    const std::vector<std::vector<double> *> &xs,
    const std::vector<rating_t> &ys)
{
    const size_t n = xs.size();

    size_t ns[NUM_RATINGS] = {0}; // counts of different ratings

    for (size_t i = 0; i < n; i++) {
        rating_t rating = ys[i];
        ns[rating]++;
    }

    // accumulate weighted information contents over all labels (ratings)
    double info = 0.0;
    for (size_t rating = 0; rating < NUM_RATINGS; rating++) {
        if (ns[rating] != 0) {
            double r = ns[rating] * 1.0 / n; // ns[rating] / n : P(c_i)
            info += -r * log2(r);            // -r * log2(r) : -P(c_i) * log2(P(c_i))
        }
    }

    return info;
}

// calculate information gain: I(R) - accumulate(P(A=v_i) * I(E_A=v_i))
double calc_information_gain(
    const std::vector<std::vector<double> *> &xs,
    const std::vector<rating_t> &ys,
    attr_t attr, double splitval, double info)
{
    const size_t n = xs.size();

    std::vector<std::vector<double> *> xs1;
    std::vector<rating_t> ys1;

    std::vector<std::vector<double> *> xs2;
    std::vector<rating_t> ys2;

    // split training data set by splitval
    for (size_t i = 0; i < n; i++) {
        if ((*xs[i])[attr] >= splitval) {
            // training data with attribute value >= splitval
            xs1.push_back(xs[i]);
            ys1.push_back(ys[i]);
        } else {
            // training data with attribute value < splitval
            xs2.push_back(xs[i]);
            ys2.push_back(ys[i]);
        }
    }

    // calculate information content: I(value >= splitval) for
    // training data with attribute value >= splitval
    double info1 = calc_information(xs1, ys1);
    // calculate information content: I(value < splitval) for
    // training data with attribute value < splitval
    double info2 = calc_information(xs2, ys2);

    // I(R) - (P(value >= splitval) * I(value >= splitval) +
    //         P(value < splitval) * I(value < splitval))
    return info - (xs1.size() * info1 + xs2.size() * info2) / n;
}

// choose the best attribute to split as well as the corresponding splitval
// return the best attribute to split and fill bestsplitval with
// the corresponding splitval
attr_t choose_split(
    const std::vector<std::vector<double> *> &xs,
    const std::vector<rating_t> &ys,
    double &bestsplitval)
{
    attr_t bestattr = ATTR_UNKNOWN;
    double bestgain = 0.0;
    const size_t n = xs.size();

    // calculate I(R)
    double info = calc_information(xs, ys);

    // iterate all attributes
    for (size_t attr = 0; attr < NUM_ATTRS; attr++) {

        // find unique attribute values
        std::set<double> s;
        for (size_t i = 0; i < n; i++) {
            s.insert((*xs[i])[attr]);
        }

        // sort unique attribute values
        std::vector<double> column(s.begin(), s.end());
        sort(column.begin(), column.end());

        // iterate all possible splitvals
        for (size_t i = 0; i < n - 1; i++) {
            double splitval = 0.5 * (column[i] + column[i + 1]);
            // calculate information gain: I(R) - accumulate(P(A=vi) * I(E_A=vi))
            double gain = calc_information_gain(
                xs, ys, static_cast<attr_t>(attr), splitval, info);
            if (gain > bestgain) {
                // update bestgain
                bestgain = gain;
                // update bestattr and bestsplitval
                bestattr = static_cast<attr_t>(attr);
                bestsplitval = splitval;
            } else if (gain == bestgain) {
                // update bestattr and bestsplitval
                bestattr = static_cast<attr_t>(attr);
                bestsplitval = splitval;
            }
        }
    }

    return bestattr;
}

// get the unique mode
rating_t get_mode(const std::vector<rating_t> &ys) {
    size_t n1s[NUM_RATINGS] = {0};  // counts of different ratings

    for (size_t i = 0; i < ys.size(); i++) {
        rating_t rating = ys[i];
        n1s[rating]++;
    }

    size_t n = 0;
    rating_t mode = RATING_UNKNOWN;  // pre-set mode to unknown

    // iterate all ratings
    for (size_t rating = 0; rating < NUM_RATINGS; rating++) {
        if (n1s[rating] > n) {
            // find the most frequent value (so far)
            n = n1s[rating];
            mode = static_cast<rating_t>(rating);
        } else if (n1s[rating] == n) {
            // find the most frequest value appearing more than once
            // therefore it's not unique
            // fall back to unknown
            mode = RATING_UNKNOWN;
        }
    }

    return mode;
}

// determine if all y are the same
bool same_ys(const std::vector<rating_t> &ys) {
    const size_t n = ys.size();

    if (n > 1) {
        for (size_t i = 0; i < n - 1; i++) {
            if (ys[i] != ys[i + 1]) {
                return false;
            }
        }
    }

    return true;
}

// determine if all x are the same
bool same_xs(const std::vector<std::vector<double> *> &xs) {
    const size_t n = xs.size();

    if (n > 1) {
        for (size_t i = 0; i < n - 1; i++) {
            if (*xs[i] != *xs[i + 1]) {
                return false;
            }
        }
    }

    return true;
}

// apply DTL algorithm to create the decision tree
// and return the tree root
node_t *dtl(
    const std::vector<std::vector<double> *> &xs,
    const std::vector<rating_t> &ys,
    size_t minleaf,
    size_t &id)
{
    const size_t n = xs.size();

    node_t *node = new node_t;

    node->id = id++;

    if (n <= minleaf) { // create new leaf node
        node->attr = ATTR_UNKNOWN;
        node->splitval = 0.0;
        node->left = nullptr;
        node->right = nullptr;
        node->isleaf = true;
        node->label = get_mode(ys);
        return node;
    }
    
    if (same_ys(ys)) { // create new leaf node
        node->attr = ATTR_UNKNOWN;
        node->splitval = 0.0;
        node->left = nullptr;
        node->right = nullptr;
        node->isleaf = true;
        node->label = ys[0];
        return node;
    }

    if (same_xs(xs)) { // create new leaf node
        node->attr = ATTR_UNKNOWN;
        node->splitval = 0.0;
        node->left = nullptr;
        node->right = nullptr;
        node->isleaf = true;
        node->label = get_mode(ys);
        return node;
    }

    // find the best attribute and the best splitval
    node->attr = choose_split(xs, ys, node->splitval);

    std::vector<std::vector<double> *> xs_left;
    std::vector<rating_t> ys_left;
    std::vector<std::vector<double> *> xs_right;
    std::vector<rating_t> ys_right;

    // split data into two categories
    for (size_t i = 0; i < n; i++) {
        double val = (*xs[i])[node->attr];
        if (val <= node->splitval) {
            xs_left.push_back(xs[i]);
            ys_left.push_back(ys[i]);
        } else {
            xs_right.push_back(xs[i]);
            ys_right.push_back(ys[i]);
        }
    }

    // recursively run DTL algo on left child
    node->left = dtl(xs_left, ys_left, minleaf, id);
    // recursively run DTL algo on right child
    node->right = dtl(xs_right, ys_right, minleaf, id);
    // not a leaf node thus no label (unknown label)
    node->isleaf = false;
    node->label = RATING_UNKNOWN;

    return node;
}

// traverse decision tree to print debug information
void traverse_tree(node_t *node) {
    if (node == nullptr) {
        return;
    }
    if (!node->isleaf) {
        std::cerr << node->id << ". If " << attr2str(node->attr) << " <= "
             << node->splitval << ", goto " << node->left->id
             << ", else goto " << node->right->id << "." << std::endl;
        traverse_tree(node->left);
        traverse_tree(node->right);
    } else {
        std::cerr << node->id << ". Return rating " << rating2str(node->label)
             << std::endl;
    }
}

// traverse decision tree to free all nodes
void free_tree(node_t *node) {
    if (node == nullptr) {
        return;
    }
    if (!node->isleaf) {
        free_tree(node->left);
        free_tree(node->right);
    }
    delete node;
}

// predict the labels (ratings) of the given testing data set
// after return test_y will be filled with predictions
void predict(
    const node_t *root,
    const std::vector<std::vector<double> *> &test_x,
    std::vector<rating_t> &test_y)
{
    const size_t n = test_x.size();

    for (size_t i = 0; i < n; i++) {
        const std::vector<double> *x = test_x[i];
        const node_t *node = root;
        while (!node->isleaf) { // continue only when current node is not a leaf
            if ((*x)[node->attr] <= node->splitval) {
                // go search left if attribute value <= splitval
                node = node->left;
            } else {
                // go search right if attribute value > splitval
                node = node->right;
            }
        }
        // obtain the label (rating)
        test_y.push_back(node->label);
    }
}

// print predictions to stdout
void print_predictions(const std::vector<rating_t> &test_y) {
    const size_t n = test_y.size();

    for (size_t i = 0; i < n; i++) {
        rating_t rating = test_y[i];
        std::cout << rating2str(rating) << std::endl;
    }
}

// main driver function
int main(int argc, char **argv) {
    // firstly check if got enough arguments
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " [train] [test] [minleaf]" << std::endl;
        return -1;
    }

    std::vector<std::vector<double> *> train_x;
    std::vector<rating_t> train_y;

    // try reading training data set
    if (!create_training_data_set(argv[1], train_x, train_y)) {
        std::cerr << "Failed to read training data set" << std::endl;
        return -1;
    }

    std::vector<std::vector<double> *> test_x;

    // try reading testing data set
    if (!create_testing_data_set(argv[2], test_x)) {
        std::cerr << "Failed to read testing data set" << std::endl;
        return -1;
    }

    // check if minleaf is valid
    // minleaf is supposed to be an integer which >= 1
    size_t minleaf = std::stoul(argv[3]);
    if (minleaf < 1) {
        std::cerr << "Invalid minleaf" << std::endl;
        return -1;
    }

    size_t id = 1;  // id starts with 1

    // apply DTL algo on training data set
    // return the decision tree root
    node_t *root = dtl(train_x, train_y, minleaf, id);

    std::vector<rating_t> test_y;

    // make predictions on testing data set
    predict(root, test_x, test_y);

    // uncomment this to get debug output of decision tree traversal
    // traverse_tree(root);

    // free allocated memory
    free_tree(root);

    // finally print predictions to stdout
    print_predictions(test_y);

    return 0;
}
