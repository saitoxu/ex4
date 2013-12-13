#include <vector>
#include <string>
#include "training_data.hpp"

using namespace std;

const int NOKERNEL   = 0; /* カーネルなし */
const int POLYNOMIAL = 1; /* 多項式カーネル */
const int GAUSS      = 2; /* ガウスカーネル */
const int SIGMOID    = 3; /* シグモイドカーネル */

/* SVMクラス */
class svm {
private:
  int kernel;                         /* カーネル */
  string file_name;                   /* 教師データのファイル名 */
  double polynomial_d;                /* 多項式カーネルのパラメータ */
  double gauss_sigma;                 /* ガウスカーネルのパラメータ */
  double sigmoid_a;                   /* シグモイドカーネルのパラメータA */
  double sigmoid_b;                   /* シグモイドカーネルのパラメータB */
  training_data support_vector;       /* サポートベクター */
  vector<double> alpha;               /* Lagrange乗数 */
  vector<training_data> list_of_data; /* 教師データの集合 */
  vector<double> weight;              /* 重みベクトル */
  double theta;                       /* 閾値 */

  double get_kernel(vector<double> vector1, vector<double> vector2, int kernel);
  void input_data();
  double inner_product(vector<double> vector1, vector<double> vector2);
  double get_norm(vector<double> vector1, vector<double> vector2);
  vector<double> calculate_lagrange();
  training_data get_support_vector();
  vector<double> calculate_weight();
  double calculate_theta();

public:
  svm(int k, string f_name, double p_d, double g_s, double s_a, double s_b);
  vector<double> get_weight();
  double get_theta();
};
