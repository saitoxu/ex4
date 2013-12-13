#include <iostream>
#include <vector>
#include "training_data.hpp"

using namespace std;

void training_data::set_dimension(int d) {
  dimension = d;
}

void training_data::set_x(vector<double> training_vector) {
  int i;
  for (i = 0; i < training_vector.size(); i++) {
    x.push_back(training_vector[i]);
  }
  set_dimension(i);
}

void training_data::set_y(int answer) {
  y = answer;
}

int training_data::get_dimension() {
  return dimension;
}

vector<double> training_data::get_x() {
  return x;
}

double training_data::get_x(int i) {
  return x[i];
}

int training_data::get_y() {
  return y;
}
