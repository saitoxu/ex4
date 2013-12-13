#include <vector>

using namespace std;

/* 教師データのクラス */
class training_data {
private:
  int dimension;    /* 教師データの次元 */
  vector<double> x; /* 教師データのベクトル */
  int y;            /* 属するクラス(1 or -1) */

  void set_dimension(int d);

public:
  void set_x(vector<double> training_vector);
  void set_y(int answer);
  int get_dimension();
  vector<double> get_x();
  double get_x(int i);
  int get_y();
};
