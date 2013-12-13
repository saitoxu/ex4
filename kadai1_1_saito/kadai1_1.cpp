#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <math.h>
#include "svm.hpp"

using namespace std;

int get_option(int argc, char *argv[], string option_list);
void print_help();
void output_png(int kernel, vector<double> weight, double theta);

const double POLYNOMIAL_D =  2; /* 多項式カーネルのパラメータ */
const double GAUSS_SIGMA  = 10; /* ガウスカーネルのパラメータ */
const double SIGMOID_A    =  1; /* シグモイドカーネルのパラメータA */
const double SIGMOID_B    =  3; /* シグモイドカーネルのパラメータB */

const string FILE_NAME = "sample_linear.dat"; /* 教師データのファイル名 */
const string OPTION_LIST = "hk:";             /* getoptに渡すオプション */

int main(int argc, char *argv[]) {
  int kernel;            /* コマンドライン引数から */
  vector<double> weight; /* 重みベクトル */
  double theta;          /* 閾値 */

  /* コマンドライン引数から使用するカーネルを判断 */
  kernel = get_option(argc, argv, OPTION_LIST);

  svm svm(kernel, FILE_NAME, POLYNOMIAL_D, GAUSS_SIGMA, SIGMOID_A, SIGMOID_B);

  /* 重みと閾値を取得 */
  weight = svm.get_weight();
  theta = svm.get_theta();

  /* 重みベクトルと閾値の値を表示 */
  for (int i = 0; i < weight.size(); i++) {
    cout << "weight[" << i << "] = " << weight[i] << endl;
  }
  cout << "theta = " << theta << endl;

  /*
   * もしカーネル無しかつ2次元データだったら
   * gnuplotで画像表示するというおまけ
   * result.pngという画像が生成される
   * 動かなかったらコメントアウトしてください
   */
  output_png(kernel, weight, theta);

  // // 新規データを入力してどっちのクラスに属するかを識別
  // // このへんコード適当なので要リファクタリング
  // int temp = 0;
  // double f = 0.0;
  // while (1) {
  //   // 標準入力から新規ベクトル作成
  //   vector<double> temp_v;
  //   for (int i = 0; i < weight.size(); i++) {
  //     cin >> temp;
  //     cout << "vector = " << temp << endl;
  //     temp_v.push_back((double)temp);
  //   }

  //   // 識別器作成
  //   f = inner_product(weight, temp_v) - theta;
  //   cout << "f = " << f << endl;
  //   temp_v.clear();
  // }

  return(0);
}

/*******************************************************************************
 * get_option -- コマンドライン引数を処理する                                  *
 *                                                                             *
 * params -- int argc コマンドライン引数の数                                   *
 *           char *argv[] コマンドライン引数の文字列の配列                     *
 *           string option_list オプションのリスト                             *
 *                                                                             *
 * return -- int カーネルの種類                                                *
 *******************************************************************************/
int get_option(int argc, char *argv[], string option_list) {
  int option, result = 0;

  /* 引数がない場合：カーネルなし */
  while ((option = getopt(argc, argv, option_list.c_str())) != -1) {
    switch (option) {

    case 'h':
      /* ヘルプを出す */
      print_help();
      exit(0);

    case 'k':
      /* 外部変数optargに格納された値をresultに格納 */
      result = atoi(optarg);
      /* 引数に変な文字列を入れると0になってNOKERNEL扱いされる */
      if (result != NOKERNEL &&
          result != POLYNOMIAL &&
          result != GAUSS &&
          result != SIGMOID) {
        print_help();
        exit(0);
      }
      break;

      /* getoptの引数で指定されなかったオプションを受け取ると?を返す */
    case '?':
      print_help();
      exit(0);
    }
  }
  return result;
}

/*******************************************************************************
 * print_help -- ヘルプを出力する                                              *
 *******************************************************************************/
void print_help() {
  fprintf(stdout, "Usage: ./kadai1_1 [options]\n");
  fprintf(stdout, "\n");
  fprintf(stdout, "Options:\n");
  fprintf(stdout, "  -h     Display available options\n");
  fprintf(stdout, "  -k     Assign kernel\n");
  fprintf(stdout, "           0 -- no kernel\n");
  fprintf(stdout, "           1 -- polynomial\n");
  fprintf(stdout, "           2 -- gauss\n");
  fprintf(stdout, "           3 -- sigmoid\n");
  return;
}

/*******************************************************************************
 * output_png -- 識別結果の画像を出力                                          *
 *                                                                             *
 * params -- int kernel カーネルの番号                                         *
 *           vector<double> weight 重みベクトル                                *
 *           double theta 閾値                                                 *
 *******************************************************************************/
void output_png(int kernel, vector<double> weight, double theta) {
  if (kernel == NOKERNEL && weight.size() == 2) {
    FILE *gnuplot;
    if ((gnuplot = popen("gnuplot -persist","w")) == NULL) {
      cout << "Error: Can't open gnuplot" << endl;
      exit(1);
    }
    fprintf(gnuplot, "plot '%s' using 1:($3==1?$2:1/0) lc rgb 'blue' pt 6;",
            FILE_NAME.c_str());
    fprintf(gnuplot, "replot '%s' using 1:($3==-1?$2:1/0) lc rgb 'red' pt 2;",
            FILE_NAME.c_str());
    fprintf(gnuplot, "replot -1 * (%f / %f) * x + (%f / %f) lc rgb \"black\";",
            weight[0], weight[1], theta, weight[1]);
    fprintf(gnuplot, "set terminal png;");
    fprintf(gnuplot, "set out 'result.png';");
    fprintf(gnuplot, "replot\n");
    pclose(gnuplot);
  }
  return;
}
