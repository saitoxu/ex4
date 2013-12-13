#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <math.h>
#include "svm.hpp"
#include "QuadProg++.hh"

using namespace std;

/* コンストラクタ */
svm::svm(int k, string f_name, double p_d, double g_s, double s_a, double s_b) {
  kernel       = k;
  file_name    = f_name;
  polynomial_d = p_d;
  gauss_sigma  = g_s;
  sigmoid_a    = s_a;
  sigmoid_b    = s_b;

  /* ファイルから教師データを読み込んでlist_of_dataに格納 */
  input_data();
  /* list_of_dataをquadprogに投げて，Lagrange乗数aを求める */
  alpha = calculate_lagrange();
  /* サポートベクターを求める */
  support_vector = get_support_vector();
  /* 重みベクトルを求める */
  weight = calculate_weight();
  /* 閾値を求める */
  theta = calculate_theta();
}

/* 重みベクトルのゲッター */
vector<double> svm::get_weight() {
  return weight;
}

/* 閾値のゲッター */
double svm::get_theta() {
  return theta;
}

/*******************************************************************************
 * input_data -- 教師データを読み込む                                          *
 *******************************************************************************/
void svm::input_data() {
  ifstream ifs(file_name.c_str());
  string str;

  if (!ifs) {
    cout << "Error: Input data file not found" << endl;
    return;
  }

  /* ファイルから1行読み込む */
  while (getline(ifs, str)) {
    string token;
    double d;
    vector<double> temp_x;
    int temp_y;
    istringstream stream(str);
    training_data data;

    /* 1行からカンマまで読み込む（大いに違う可能性あり） */
    while (getline(stream, token, ' ')) {
      stringstream ss;
      ss << token;
      ss >> d;
      temp_x.push_back(d);
    }
    /* temp_xの末尾にクラスの値が入ってるので取り出す */
    temp_y = (int)temp_x[temp_x.size() - 1];
    /* クラスの値を消去 */
    temp_x.pop_back();
    /* 教師データのベクトルとクラスの値をセットする */
    data.set_x(temp_x);
    data.set_y(temp_y);
    /* 教師データの集合に追加 */
    list_of_data.push_back(data);
  }

  return;
}

/*******************************************************************************
 * calculate_lagrange -- Lagrange乗数を計算する                                *
 *                                                                             *
 * return -- vector<double> Lagrange乗数のベクトル                             *
 *******************************************************************************/
vector<double> svm::calculate_lagrange() {
  /* この辺の変数はquadprogを参照 */
  double G[MATRIX_DIM][MATRIX_DIM], g0[MATRIX_DIM];
  double CE[MATRIX_DIM][MATRIX_DIM], ce0[MATRIX_DIM];
  double CI[MATRIX_DIM][MATRIX_DIM], ci0[MATRIX_DIM];
  double x[MATRIX_DIM];
  int n = list_of_data.size(), m = n, p = 1;
  double sum = 0.0;
  vector<double> alpha; /* Lagrange乗数 */

  /* 行列Gの計算 */
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      /* Gを計算（inner_productをカーネルに替える） */
      G[i][j] = (double)list_of_data[i].get_y() * (double)list_of_data[j].get_y() *
        get_kernel(list_of_data[i].get_x(), list_of_data[j].get_x(), kernel);

      /* 対角成分に微小値を加える */
      if (i == j) {
        G[i][j] += 1.0e-9;
      }
    }
  }

  /* ベクトルg0の計算 */
  for (int i = 0; i < n; i++) {
    g0[i] = -1.0;
  }

  /* 行列CEの計算 */
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < p; j++) {
      CE[i][j] = list_of_data[i].get_y();
    }
  }

  /* ベクトルce0の計算 */
  for (int j = 0; j < p; j++) {
    ce0[j] = 0.0;
  }

  /* 行列CIの計算 */
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < m; j++) {
      if (i == j) {
        CI[i][j] = 1.0;
      } else {
        CI[i][j] = 0.0;
      }
    }
  }

  /* ベクトルci0の計算 */
  for (int j = 0; j < m; j++) {
    ci0[j] = 0.0;
  }

  /* quadprogに投げて2次計画問題を解く */
  solve_quadprog(G, g0, n, CE, ce0, p, CI, ci0, m, x);

  /* alphaに入れる */
  for (int i = 0; i < n; i++) {
    alpha.push_back(x[i]);
  }
  return alpha;
}

/*******************************************************************************
 * inner_product -- カーネルの計算                                             *
 *                                                                             *
 * params -- vector<double> ベクトル1                                          *
 *           vector<double> ベクトル2                                          *
 *           int カーネル                                                      *
 *                                                                             *
 * return -- カーネルの値                                                      *
 *******************************************************************************/
double svm::get_kernel(vector<double> vector1, vector<double> vector2, int kernel) {
  double rtn = 0.0; /* カーネルの値 */
  switch (kernel) {
  case NOKERNEL:
    rtn = inner_product(vector1, vector2);
    break;

  case POLYNOMIAL:
    rtn = 1.0 + inner_product(vector1, vector2);
    rtn = pow(rtn, polynomial_d);
    break;

  case GAUSS:
    rtn = pow(get_norm(vector1, vector2), 2) * -1.0;
    rtn = rtn / (2 * gauss_sigma * gauss_sigma);
    rtn = exp(rtn);
    break;

  case SIGMOID:
    rtn = sigmoid_a * inner_product(vector1, vector2) + sigmoid_b;
    rtn = tanh(rtn);
    break;
  }
  return rtn;
}

/*******************************************************************************
 * inner_product -- 内積を計算する                                             *
 *                                                                             *
 * params -- vector<double> ベクトル1                                          *
 *           vector<double> ベクトル2                                          *
 *                                                                             *
 * return -- 内積                                                              *
 *******************************************************************************/
double svm::inner_product(vector<double> vector1, vector<double> vector2) {
  double rtn = 0.0; /* 内積 */

  /* 次元が違ったらアウト */
  if (vector1.size() != vector2.size()) {
    cout << "Error: Dimension is different between vector1 and vector2" << endl;
    exit(1);
  }

  /* 内積計算 */
  for (int i = 0; i < vector1.size(); i++) {
    rtn += vector1[i] * vector2[i];
  }
  return rtn;
}

/*******************************************************************************
 * get_norm -- ノルムを計算する                                                *
 *                                                                             *
 * params -- vector<double> ベクトル1                                          *
 *           vector<double> ベクトル2                                          *
 *                                                                             *
 * return -- ノルム                                                            *
 *******************************************************************************/
double svm::get_norm(vector<double> vector1, vector<double> vector2) {
  double rtn = 0.0; /* ノルム */

  /* 次元が違ったらアウト */
  if (vector1.size() != vector2.size()) {
    cout << "Error: Dimension is different between vector1 and vector2" << endl;
    exit(1);
  }

  for (int i = 0; i < vector1.size(); i++) {
    rtn += pow(vector1[i] - vector2[i], 2);
  }

  return sqrt(rtn);
}

/*******************************************************************************
 * get_support_vector -- サポートベクターを返す（ラグランジュ乗数が最大値）    *
 *                                                                             *
 * return -- サポートベクター                                                  *
 *******************************************************************************/
training_data svm::get_support_vector() {
  int max = 0; /* 最大のラグランジュ乗数を持つvectorの番号 */
  for (int i = 0; i < alpha.size(); i++) {
    /* もし現在の最大値より大きければ更新する */
    if (alpha[max] < alpha[i]) {
      max = i;
    }
  }
  return list_of_data[max];
}

/*******************************************************************************
 * calculate_weight -- 重みベクトルを計算する                                  *
 *                                                                             *
 * return -- 重みベクトル                                                      *
 *******************************************************************************/
vector<double> svm::calculate_weight() {
  vector<double> weight;
  for (int i = 0; i < list_of_data[0].get_dimension(); i++) {
    double temp_vector = 0.0;
    for (int j = 0; j < alpha.size(); j++) {
      temp_vector += alpha[j] * list_of_data[j].get_y() * list_of_data[j].get_x(i);
    }
    weight.push_back(temp_vector);
  }
  return weight;
}

/*******************************************************************************
 * calculate_theta -- 閾値を計算する                                           *
 *                                                                             *
 * return -- 閾値                                                              *
 *******************************************************************************/
double svm::calculate_theta() {
  return inner_product(weight, support_vector.get_x()) - support_vector.get_y();
}
