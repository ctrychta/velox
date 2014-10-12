#include "stats.h"
#include "test_helpers.h"

using namespace velox;

struct StatTestResults {
  StatTestResults(const std::vector<double> &v)
      : sum_(sum(v)), mean_(mean(v)), median_(median(v)), percentile30_(percentile(v, 30)),
        percentile60_(percentile(v, 60)), percentile90_(percentile(v, 90)),
        percentile99_(percentile(v, 99)), variance_(variance(v)), std_dev_(std_dev(v)),
        quartiles_(quartiles(v)) {
    auto sorted_v = v;
    std::sort(sorted_v.begin(), sorted_v.end());

    median_of_sorted_ = median_of_sorted(sorted_v);

    {
      auto copy = sorted_v;
      median_destructive_ = median_destructive(copy);
    }

    {
      auto copy = sorted_v;
      std::vector<double> buffer;
      median_abs_dev_ = median_abs_dev_of_sorted_destructive(copy, buffer);
    }
  }

  double sum_;
  double mean_;
  double median_;
  double median_of_sorted_;
  double median_destructive_;
  double percentile30_;
  double percentile60_;
  double percentile90_;
  double percentile99_;
  double variance_;
  double std_dev_;
  double median_abs_dev_;
  Quartiles<double> quartiles_;
};

TEST_CASE("Stat functions binomial 5") {
  std::initializer_list<double> init{86, 74, 79, 79, 81};
  std::vector<double> v{init};
  StatTestResults r(v);

  CHECK(r.sum_ == Approx(399));
  CHECK(r.mean_ == Approx(79.8));
  CHECK(r.median_ == Approx(79));
  CHECK(r.median_of_sorted_ == Approx(79));
  CHECK(r.median_destructive_ == Approx(79));
  CHECK(r.percentile30_ == Approx(79));
  CHECK(r.percentile60_ == Approx(79.8));
  CHECK(r.percentile90_ == Approx(84));
  CHECK(r.percentile99_ == Approx(85.8));
  CHECK(r.variance_ == Approx(18.7));
  CHECK(r.std_dev_ == Approx(4.32435));
  CHECK(r.median_abs_dev_ == Approx(2.9652));
  CHECK(r.quartiles_.q1() == Approx(79));
  CHECK(r.quartiles_.q2() == Approx(79));
  CHECK(r.quartiles_.q3() == Approx(81));
  CHECK(r.quartiles_.iqr() == Approx(2));
}
TEST_CASE("Stat functions binomial 25") {
  std::initializer_list<double> init{512,
                                     492,
                                     482,
                                     473,
                                     516,
                                     491,
                                     501,
                                     516,
                                     480,
                                     494,
                                     463,
                                     513,
                                     508,
                                     527,
                                     520,
                                     544,
                                     500,
                                     512,
                                     515,
                                     502,
                                     489,
                                     485,
                                     497,
                                     498,
                                     526};
  std::vector<double> v{init};
  StatTestResults r(v);

  CHECK(r.sum_ == Approx(12556));
  CHECK(r.mean_ == Approx(502.24));
  CHECK(r.median_ == Approx(501));
  CHECK(r.median_of_sorted_ == Approx(501));
  CHECK(r.median_destructive_ == Approx(501));
  CHECK(r.percentile30_ == Approx(492.4));
  CHECK(r.percentile60_ == Approx(509.6));
  CHECK(r.percentile90_ == Approx(523.6));
  CHECK(r.percentile99_ == Approx(539.92));
  CHECK(r.variance_ == Approx(343.3567));
  CHECK(r.std_dev_ == Approx(18.52989));
  CHECK(r.median_abs_dev_ == Approx(17.7912));
  CHECK(r.quartiles_.q1() == Approx(491));
  CHECK(r.quartiles_.q2() == Approx(501));
  CHECK(r.quartiles_.q3() == Approx(515));
  CHECK(r.quartiles_.iqr() == Approx(24));
}
TEST_CASE("Stat functions binomial 50") {
  std::initializer_list<double> init{6959,
                                     7039,
                                     6967,
                                     6996,
                                     7001,
                                     6994,
                                     7041,
                                     6997,
                                     6932,
                                     7032,
                                     6996,
                                     7009,
                                     7047,
                                     7081,
                                     6946,
                                     6977,
                                     6999,
                                     7027,
                                     7015,
                                     6925,
                                     7064,
                                     6995,
                                     7048,
                                     7060,
                                     7022,
                                     6974,
                                     7055,
                                     7002,
                                     6998,
                                     6991,
                                     7068,
                                     6953,
                                     6979,
                                     7035,
                                     6956,
                                     7142,
                                     7073,
                                     7045,
                                     7008,
                                     6954,
                                     6949,
                                     7019,
                                     6896,
                                     7009,
                                     7058,
                                     7009,
                                     7041,
                                     6973,
                                     7025,
                                     6972};
  std::vector<double> v{init};
  StatTestResults r(v);

  CHECK(r.sum_ == Approx(350353));
  CHECK(r.mean_ == Approx(7007.06));
  CHECK(r.median_ == Approx(7005));
  CHECK(r.median_of_sorted_ == Approx(7005));
  CHECK(r.median_destructive_ == Approx(7005));
  CHECK(r.percentile30_ == Approx(6987.4));
  CHECK(r.percentile60_ == Approx(7016.6));
  CHECK(r.percentile90_ == Approx(7060.4));
  CHECK(r.percentile99_ == Approx(7112.11));
  CHECK(r.variance_ == Approx(2115.037));
  CHECK(r.std_dev_ == Approx(45.98953));
  CHECK(r.median_abs_dev_ == Approx(48.1845));
  CHECK(r.quartiles_.q1() == Approx(6974.75));
  CHECK(r.quartiles_.q2() == Approx(7005));
  CHECK(r.quartiles_.q3() == Approx(7040.5));
  CHECK(r.quartiles_.iqr() == Approx(65.75));
}
TEST_CASE("Stat functions chi 5") {
  std::initializer_list<double> init{
      98.8619818916057, 123.034270956706, 100.82069227891, 91.3758878553293, 98.8000610862524};
  std::vector<double> v{init};
  StatTestResults r(v);

  CHECK(r.sum_ == Approx(512.8929));
  CHECK(r.mean_ == Approx(102.5786));
  CHECK(r.median_ == Approx(98.86198));
  CHECK(r.median_of_sorted_ == Approx(98.86198));
  CHECK(r.median_destructive_ == Approx(98.86198));
  CHECK(r.percentile30_ == Approx(98.81245));
  CHECK(r.percentile60_ == Approx(99.64547));
  CHECK(r.percentile90_ == Approx(114.1488));
  CHECK(r.percentile99_ == Approx(122.1457));
  CHECK(r.variance_ == Approx(143.779));
  CHECK(r.std_dev_ == Approx(11.99079));
  CHECK(r.median_abs_dev_ == Approx(2.903984));
  CHECK(r.quartiles_.q1() == Approx(98.80006));
  CHECK(r.quartiles_.q2() == Approx(98.86198));
  CHECK(r.quartiles_.q3() == Approx(100.8207));
  CHECK(r.quartiles_.iqr() == Approx(2.020631));
}
TEST_CASE("Stat functions chi 25") {
  std::initializer_list<double> init{1012.1311687505,
                                     1039.39088678792,
                                     1028.55865328976,
                                     1146.0835062116,
                                     997.129885803622,
                                     1015.32084246004,
                                     1137.48194701911,
                                     1043.46765104444,
                                     1004.73208539742,
                                     945.733778147831,
                                     971.815571673066,
                                     979.509792408638,
                                     1034.05529571818,
                                     911.870875717547,
                                     1018.67288846904,
                                     934.833977113064,
                                     959.648601738989,
                                     1023.88120050981,
                                     1017.78325549675,
                                     1043.25364935584,
                                     1017.80811397815,
                                     966.674944705043,
                                     993.994577006199,
                                     1025.19386088844,
                                     1039.1069651742};
  std::vector<double> v{init};
  StatTestResults r(v);

  CHECK(r.sum_ == Approx(25308.13));
  CHECK(r.mean_ == Approx(1012.325));
  CHECK(r.median_ == Approx(1017.783));
  CHECK(r.median_of_sorted_ == Approx(1017.783));
  CHECK(r.median_destructive_ == Approx(1017.783));
  CHECK(r.percentile30_ == Approx(994.6216));
  CHECK(r.percentile60_ == Approx(1020.756));
  CHECK(r.percentile90_ == Approx(1043.382));
  CHECK(r.percentile99_ == Approx(1144.019));
  CHECK(r.variance_ == Approx(2783.64));
  CHECK(r.std_dev_ == Approx(52.76021));
  CHECK(r.median_abs_dev_ == Approx(32.03547));
  CHECK(r.quartiles_.q1() == Approx(979.5098));
  CHECK(r.quartiles_.q2() == Approx(1017.783));
  CHECK(r.quartiles_.q3() == Approx(1034.055));
  CHECK(r.quartiles_.iqr() == Approx(54.5455));
}
TEST_CASE("Stat functions chi 50") {
  std::initializer_list<double> init{244.742665181292,
                                     260.637937941892,
                                     255.975630652586,
                                     255.481703090723,
                                     259.483683636706,
                                     243.391929822081,
                                     245.618851480243,
                                     228.740867931612,
                                     244.784739558957,
                                     294.594743169454,
                                     273.028467574704,
                                     268.115225221155,
                                     225.514826666697,
                                     285.675624305521,
                                     267.025845275087,
                                     265.075595048677,
                                     281.53043067992,
                                     288.71953225487,
                                     234.272598496239,
                                     296.222919539829,
                                     304.894160866272,
                                     259.506414987366,
                                     228.111367006509,
                                     225.233133612603,
                                     299.980201567854,
                                     225.79120592849,
                                     258.31313551062,
                                     223.508368728143,
                                     257.988858111342,
                                     273.001553559297,
                                     263.757842777612,
                                     264.067799013243,
                                     269.592848065127,
                                     244.281224376741,
                                     239.997692420743,
                                     222.323995031648,
                                     238.686390928615,
                                     282.511610983329,
                                     287.886364841339,
                                     230.525558615198,
                                     263.063888477299,
                                     227.452837551436,
                                     251.868662262483,
                                     229.46395541115,
                                     244.876821789991,
                                     285.165579773281,
                                     247.916642768955,
                                     247.042663595815,
                                     243.349405930054,
                                     273.827975748446};
  std::vector<double> v{init};
  StatTestResults r(v);

  CHECK(r.sum_ == Approx(12832.62));
  CHECK(r.mean_ == Approx(256.6524));
  CHECK(r.median_ == Approx(256.9822));
  CHECK(r.median_of_sorted_ == Approx(256.9822));
  CHECK(r.median_destructive_ == Approx(256.9822));
  CHECK(r.percentile30_ == Approx(244.0144));
  CHECK(r.percentile60_ == Approx(261.6083));
  CHECK(r.percentile90_ == Approx(287.9697));
  CHECK(r.percentile99_ == Approx(302.4863));
  CHECK(r.variance_ == Approx(511.1306));
  CHECK(r.std_dev_ == Approx(22.6082));
  CHECK(r.median_abs_dev_ == Approx(23.77018));
  CHECK(r.quartiles_.q1() == Approx(240.8356));
  CHECK(r.quartiles_.q2() == Approx(256.9822));
  CHECK(r.quartiles_.q3() == Approx(272.1494));
  CHECK(r.quartiles_.iqr() == Approx(31.31376));
}
TEST_CASE("Stat functions exponential 5") {
  std::initializer_list<double> init{0.0539191847156349,
                                     0.912105008917761,
                                     2.43184782365773,
                                     0.255417859647423,
                                     0.404768015723675};
  std::vector<double> v{init};
  StatTestResults r(v);

  CHECK(r.sum_ == Approx(4.058058));
  CHECK(r.mean_ == Approx(0.8116116));
  CHECK(r.median_ == Approx(0.404768));
  CHECK(r.median_of_sorted_ == Approx(0.404768));
  CHECK(r.median_destructive_ == Approx(0.404768));
  CHECK(r.percentile30_ == Approx(0.2852879));
  CHECK(r.percentile60_ == Approx(0.6077028));
  CHECK(r.percentile90_ == Approx(1.823951));
  CHECK(r.percentile99_ == Approx(2.371058));
  CHECK(r.variance_ == Approx(0.9210588));
  CHECK(r.std_dev_ == Approx(0.9597181));
  CHECK(r.median_abs_dev_ == Approx(0.5201685));
  CHECK(r.quartiles_.q1() == Approx(0.2554179));
  CHECK(r.quartiles_.q2() == Approx(0.404768));
  CHECK(r.quartiles_.q3() == Approx(0.912105));
  CHECK(r.quartiles_.iqr() == Approx(0.6566871));
}
TEST_CASE("Stat functions exponential 25") {
  std::initializer_list<double> init{0.053254018628732,
                                     0.0322400220436975,
                                     0.0180287017508615,
                                     0.0283212491543964,
                                     0.0567019662513942,
                                     0.0608384393600232,
                                     0.0430174807531244,
                                     0.0388306312242007,
                                     0.010773335481648,
                                     0.0205512379296124,
                                     0.0351441272047764,
                                     0.0118977791396901,
                                     0.0111340129282326,
                                     0.00167028970524689,
                                     0.0195984495105222,
                                     0.0203838338144124,
                                     0.00769382137388073,
                                     0.04518164479895,
                                     0.0665928359122431,
                                     0.0331669436767697,
                                     0.0117223523158496,
                                     0.0499767655560617,
                                     0.0705080034169269,
                                     0.0592306304236359,
                                     0.0260788558749482};
  std::vector<double> v{init};
  StatTestResults r(v);

  CHECK(r.sum_ == Approx(0.8325374));
  CHECK(r.mean_ == Approx(0.0333015));
  CHECK(r.median_ == Approx(0.03224002));
  CHECK(r.median_of_sorted_ == Approx(0.03224002));
  CHECK(r.median_destructive_ == Approx(0.03224002));
  CHECK(r.percentile30_ == Approx(0.01975553));
  CHECK(r.percentile60_ == Approx(0.03661873));
  CHECK(r.percentile90_ == Approx(0.06019532));
  CHECK(r.percentile99_ == Approx(0.06956836));
  CHECK(r.variance_ == Approx(0.0004093357));
  CHECK(r.std_dev_ == Approx(0.02023205));
  CHECK(r.median_abs_dev_ == Approx(0.0262965));
  CHECK(r.quartiles_.q1() == Approx(0.0180287));
  CHECK(r.quartiles_.q2() == Approx(0.03224002));
  CHECK(r.quartiles_.q3() == Approx(0.04997677));
  CHECK(r.quartiles_.iqr() == Approx(0.03194806));
}
TEST_CASE("Stat functions exponential 50") {
  std::initializer_list<double> init{0.0502090178764122,
                                     0.0367384520156269,
                                     0.0498756814078198,
                                     0.0107328163459897,
                                     0.00443620080128312,
                                     0.0386657296104317,
                                     0.0140675874351167,
                                     0.0565276712662453,
                                     0.0317720670024639,
                                     0.025006961262876,
                                     0.0014044329430908,
                                     0.0126398038677871,
                                     0.0456272490942832,
                                     0.0171855383544209,
                                     0.0323059025305391,
                                     0.00474217222058435,
                                     0.00637522248671457,
                                     0.00383542207065263,
                                     0.0173884900837185,
                                     0.013012830298394,
                                     0.048493820687098,
                                     0.00333154315550036,
                                     0.0206112099640576,
                                     0.00155323576182127,
                                     0.00662546487525105,
                                     0.0110410280711949,
                                     0.0106375683564693,
                                     0.0260436311728301,
                                     0.0211963710925522,
                                     0.0107479507382959,
                                     0.00233477246016264,
                                     0.0130428046453744,
                                     0.0142191978109977,
                                     0.00630036268383265,
                                     0.186573094826445,
                                     0.00120075540183194,
                                     0.00029676147736609,
                                     0.0581410907175312,
                                     0.0197498688838425,
                                     0.0078161546587944,
                                     0.0137457921355963,
                                     0.0124571002449853,
                                     0.0208827884206719,
                                     0.00163977955803892,
                                     0.0061718842779894,
                                     0.0867292795279875,
                                     0.00392397687770426,
                                     0.0206359926789048,
                                     0.00384071293286979,
                                     0.00418382271192968};
  std::vector<double> v{init};
  StatTestResults r(v);

  CHECK(r.sum_ == Approx(1.116717));
  CHECK(r.mean_ == Approx(0.02233434));
  CHECK(r.median_ == Approx(0.01302782));
  CHECK(r.median_of_sorted_ == Approx(0.01302782));
  CHECK(r.median_destructive_ == Approx(0.01302782));
  CHECK(r.percentile30_ == Approx(0.006352765));
  CHECK(r.percentile60_ == Approx(0.01726672));
  CHECK(r.percentile90_ == Approx(0.04990902));
  CHECK(r.percentile99_ == Approx(0.1376496));
  CHECK(r.variance_ == Approx(0.0009060357));
  CHECK(r.std_dev_ == Approx(0.03010043));
  CHECK(r.median_abs_dev_ == Approx(0.01292502));
  CHECK(r.quartiles_.q1() == Approx(0.0050996));
  CHECK(r.quartiles_.q2() == Approx(0.01302782));
  CHECK(r.quartiles_.q3() == Approx(0.02578446));
  CHECK(r.quartiles_.iqr() == Approx(0.02068486));
}
TEST_CASE("Stat functions normal 5") {
  std::initializer_list<double> init{-0.559125181768341,
                                     0.651534777988678,
                                     -0.639924790091285,
                                     -0.78943223108541,
                                     -0.650139760186002};
  std::vector<double> v{init};
  StatTestResults r(v);

  CHECK(r.sum_ == Approx(-1.987087));
  CHECK(r.mean_ == Approx(-0.3974174));
  CHECK(r.median_ == Approx(-0.6399248));
  CHECK(r.median_of_sorted_ == Approx(-0.6399248));
  CHECK(r.median_destructive_ == Approx(-0.6399248));
  CHECK(r.percentile30_ == Approx(-0.6480968));
  CHECK(r.percentile60_ == Approx(-0.6076049));
  CHECK(r.percentile90_ == Approx(0.1672708));
  CHECK(r.percentile99_ == Approx(0.6031084));
  CHECK(r.variance_ == Approx(0.350701));
  CHECK(r.std_dev_ == Approx(0.5922002));
  CHECK(r.median_abs_dev_ == Approx(0.1197935));
  CHECK(r.quartiles_.q1() == Approx(-0.6501398));
  CHECK(r.quartiles_.q2() == Approx(-0.6399248));
  CHECK(r.quartiles_.q3() == Approx(-0.5591252));
  CHECK(r.quartiles_.iqr() == Approx(0.09101458));
}
TEST_CASE("Stat functions normal 25") {
  std::initializer_list<double> init{990.720960957983,
                                     1010.30222979975,
                                     995.555277105668,
                                     992.088301034307,
                                     990.790599620314,
                                     1008.96196847324,
                                     1009.22777996769,
                                     1003.25385729438,
                                     990.807291964739,
                                     1027.81975939755,
                                     1003.85102769909,
                                     1009.48742800353,
                                     993.492074205499,
                                     1008.05613351092,
                                     1007.60983307623,
                                     1003.81213154894,
                                     1013.12640512888,
                                     1006.3502316437,
                                     994.732511472554,
                                     1003.32729727116,
                                     992.852672923776,
                                     1019.37438066321,
                                     1006.48444474692,
                                     1001.70859209366,
                                     989.68050469443};
  std::vector<double> v{init};
  StatTestResults r(v);

  CHECK(r.sum_ == Approx(25073.47));
  CHECK(r.mean_ == Approx(1002.939));
  CHECK(r.median_ == Approx(1003.812));
  CHECK(r.median_of_sorted_ == Approx(1003.812));
  CHECK(r.median_destructive_ == Approx(1003.812));
  CHECK(r.percentile30_ == Approx(994.8971));
  CHECK(r.percentile60_ == Approx(1006.404));
  CHECK(r.percentile90_ == Approx(1011.997));
  CHECK(r.percentile99_ == Approx(1025.793));
  CHECK(r.variance_ == Approx(95.49415));
  CHECK(r.std_dev_ == Approx(9.772111));
  CHECK(r.median_abs_dev_ == Approx(9.62222));
  CHECK(r.quartiles_.q1() == Approx(993.4921));
  CHECK(r.quartiles_.q2() == Approx(1003.812));
  CHECK(r.quartiles_.q3() == Approx(1008.962));
  CHECK(r.quartiles_.iqr() == Approx(15.46989));
}
TEST_CASE("Stat functions normal 50") {
  std::initializer_list<double> init{500.176088007035,
                                     500.228043812204,
                                     500.396494945291,
                                     499.492185134377,
                                     500.210878808366,
                                     499.944525464039,
                                     499.931137112798,
                                     500.6319678458,
                                     499.192291507491,
                                     499.8775092037,
                                     500.470451707533,
                                     499.220567323169,
                                     500.609383034243,
                                     500.460496706627,
                                     500.597365223696,
                                     500.752019306853,
                                     500.37497641798,
                                     500.480053668647,
                                     499.567842155837,
                                     499.085713643426,
                                     500.049949894031,
                                     499.755132535133,
                                     500.063949078524,
                                     499.887706929493,
                                     500.128690629207,
                                     499.734534753875,
                                     500.133457042526,
                                     499.84325529444,
                                     499.501749450401,
                                     499.772007177791,
                                     500.256541642208,
                                     499.057698864551,
                                     499.719653786406,
                                     500.37318525625,
                                     499.62915614049,
                                     499.991696091003,
                                     499.918716543758,
                                     500.012345629597,
                                     500.901043281073,
                                     499.161017757423,
                                     499.556504818652,
                                     500.678120730881,
                                     500.33983045193,
                                     499.855925484268,
                                     500.657627754702,
                                     499.619853144237,
                                     500.075609829268,
                                     500.332631945989,
                                     499.860148106686,
                                     499.886377961932};
  std::vector<double> v{init};
  StatTestResults r(v);

  CHECK(r.sum_ == Approx(25000.45));
  CHECK(r.mean_ == Approx(500.0091));
  CHECK(r.median_ == Approx(500.002));
  CHECK(r.median_of_sorted_ == Approx(500.002));
  CHECK(r.median_destructive_ == Approx(500.002));
  CHECK(r.percentile30_ == Approx(499.8219));
  CHECK(r.percentile60_ == Approx(500.1306));
  CHECK(r.percentile90_ == Approx(500.6116));
  CHECK(r.percentile99_ == Approx(500.828));
  CHECK(r.variance_ == Approx(0.208174));
  CHECK(r.std_dev_ == Approx(0.4562609));
  CHECK(r.median_abs_dev_ == Approx(0.4955002));
  CHECK(r.quartiles_.q1() == Approx(499.7397));
  CHECK(r.quartiles_.q2() == Approx(500.002));
  CHECK(r.quartiles_.q3() == Approx(500.3648));
  CHECK(r.quartiles_.iqr() == Approx(0.6251624));
}
TEST_CASE("Stat functions poisson 5") {
  std::initializer_list<double> init{12, 11, 8, 9, 8};
  std::vector<double> v{init};
  StatTestResults r(v);

  CHECK(r.sum_ == Approx(48));
  CHECK(r.mean_ == Approx(9.6));
  CHECK(r.median_ == Approx(9));
  CHECK(r.median_of_sorted_ == Approx(9));
  CHECK(r.median_destructive_ == Approx(9));
  CHECK(r.percentile30_ == Approx(8.2));
  CHECK(r.percentile60_ == Approx(9.8));
  CHECK(r.percentile90_ == Approx(11.6));
  CHECK(r.percentile99_ == Approx(11.96));
  CHECK(r.variance_ == Approx(3.3));
  CHECK(r.std_dev_ == Approx(1.81659));
  CHECK(r.median_abs_dev_ == Approx(1.4826));
  CHECK(r.quartiles_.q1() == Approx(8));
  CHECK(r.quartiles_.q2() == Approx(9));
  CHECK(r.quartiles_.q3() == Approx(11));
  CHECK(r.quartiles_.iqr() == Approx(3));
}
TEST_CASE("Stat functions poisson 25") {
  std::initializer_list<double> init{113,
                                     95,
                                     102,
                                     110,
                                     110,
                                     112,
                                     100,
                                     104,
                                     101,
                                     98,
                                     118,
                                     108,
                                     112,
                                     101,
                                     105,
                                     111,
                                     112,
                                     85,
                                     109,
                                     90,
                                     95,
                                     110,
                                     94,
                                     95,
                                     106};
  std::vector<double> v{init};
  StatTestResults r(v);

  CHECK(r.sum_ == Approx(2596));
  CHECK(r.mean_ == Approx(103.84));
  CHECK(r.median_ == Approx(105));
  CHECK(r.median_of_sorted_ == Approx(105));
  CHECK(r.median_destructive_ == Approx(105));
  CHECK(r.percentile30_ == Approx(100.2));
  CHECK(r.percentile60_ == Approx(108.4));
  CHECK(r.percentile90_ == Approx(112));
  CHECK(r.percentile99_ == Approx(116.8));
  CHECK(r.variance_ == Approx(68.55667));
  CHECK(r.std_dev_ == Approx(8.279895));
  CHECK(r.median_abs_dev_ == Approx(8.8956));
  CHECK(r.quartiles_.q1() == Approx(98));
  CHECK(r.quartiles_.q2() == Approx(105));
  CHECK(r.quartiles_.q3() == Approx(110));
  CHECK(r.quartiles_.iqr() == Approx(12));
}
TEST_CASE("Stat functions poisson 50") {
  std::initializer_list<double> init{104,
                                     97,
                                     99,
                                     86,
                                     87,
                                     110,
                                     102,
                                     96,
                                     107,
                                     119,
                                     108,
                                     82,
                                     100,
                                     83,
                                     102,
                                     96,
                                     103,
                                     101,
                                     90,
                                     98,
                                     112,
                                     100,
                                     90,
                                     104,
                                     101,
                                     109,
                                     91,
                                     101,
                                     106,
                                     97,
                                     93,
                                     99,
                                     88,
                                     107,
                                     96,
                                     127,
                                     89,
                                     81,
                                     110,
                                     100,
                                     93,
                                     115,
                                     99,
                                     95,
                                     99,
                                     84,
                                     121,
                                     102,
                                     82,
                                     98};
  std::vector<double> v{init};
  StatTestResults r(v);

  CHECK(r.sum_ == Approx(4959));
  CHECK(r.mean_ == Approx(99.18));
  CHECK(r.median_ == Approx(99));
  CHECK(r.median_of_sorted_ == Approx(99));
  CHECK(r.median_destructive_ == Approx(99));
  CHECK(r.percentile30_ == Approx(95.7));
  CHECK(r.percentile60_ == Approx(101));
  CHECK(r.percentile90_ == Approx(110.2));
  CHECK(r.percentile99_ == Approx(124.06));
  CHECK(r.variance_ == Approx(103.4159));
  CHECK(r.std_dev_ == Approx(10.16936));
  CHECK(r.median_abs_dev_ == Approx(8.8956));
  CHECK(r.quartiles_.q1() == Approx(93));
  CHECK(r.quartiles_.q2() == Approx(99));
  CHECK(r.quartiles_.q3() == Approx(104));
  CHECK(r.quartiles_.iqr() == Approx(11));
}
TEST_CASE("Stat functions uniform 5") {
  std::initializer_list<double> init{0.848685432551429,
                                     0.818901048926637,
                                     0.802276718313806,
                                     0.822603191551752,
                                     0.873941182531417};
  std::vector<double> v{init};
  StatTestResults r(v);

  CHECK(r.sum_ == Approx(4.166408));
  CHECK(r.mean_ == Approx(0.8332815));
  CHECK(r.median_ == Approx(0.8226032));
  CHECK(r.median_of_sorted_ == Approx(0.8226032));
  CHECK(r.median_destructive_ == Approx(0.8226032));
  CHECK(r.percentile30_ == Approx(0.8196415));
  CHECK(r.percentile60_ == Approx(0.8330361));
  CHECK(r.percentile90_ == Approx(0.8638389));
  CHECK(r.percentile99_ == Approx(0.872931));
  CHECK(r.variance_ == Approx(0.0007931528));
  CHECK(r.std_dev_ == Approx(0.02816297));
  CHECK(r.median_abs_dev_ == Approx(0.03013603));
  CHECK(r.quartiles_.q1() == Approx(0.818901));
  CHECK(r.quartiles_.q2() == Approx(0.8226032));
  CHECK(r.quartiles_.q3() == Approx(0.8486854));
  CHECK(r.quartiles_.iqr() == Approx(0.02978438));
}
TEST_CASE("Stat functions uniform 25") {
  std::initializer_list<double> init{5.87723230002914,
                                     5.6846585479402,
                                     5.89022450711345,
                                     5.83130883357022,
                                     5.83619991771877,
                                     5.72601183444494,
                                     5.68820755925961,
                                     5.8528499130276,
                                     5.69887897853041,
                                     5.77280530784046,
                                     5.69265066596214,
                                     5.7186302333395,
                                     5.80333107203245,
                                     5.71937502638903,
                                     5.94235895415768,
                                     5.76360722513171,
                                     5.97911122135585,
                                     5.74486273665912,
                                     5.79828442061553,
                                     5.6748301498103,
                                     5.83155388433719,
                                     5.72220995835029,
                                     5.88001430155942,
                                     5.74535845764447,
                                     5.89278154956643};
  std::vector<double> v{init};
  StatTestResults r(v);

  CHECK(r.sum_ == Approx(144.7673));
  CHECK(r.mean_ == Approx(5.790694));
  CHECK(r.median_ == Approx(5.772805));
  CHECK(r.median_of_sorted_ == Approx(5.772805));
  CHECK(r.median_destructive_ == Approx(5.772805));
  CHECK(r.percentile30_ == Approx(5.72297));
  CHECK(r.percentile60_ == Approx(5.814522));
  CHECK(r.percentile90_ == Approx(5.891759));
  CHECK(r.percentile99_ == Approx(5.970291));
  CHECK(r.variance_ == Approx(0.007556079));
  CHECK(r.std_dev_ == Approx(0.08692571));
  CHECK(r.median_abs_dev_ == Approx(0.09398885));
  CHECK(r.quartiles_.q1() == Approx(5.719375));
  CHECK(r.quartiles_.q2() == Approx(5.772805));
  CHECK(r.quartiles_.q3() == Approx(5.85285));
  CHECK(r.quartiles_.iqr() == Approx(0.1334749));
}
TEST_CASE("Stat functions uniform 50") {
  std::initializer_list<double> init{0.00135945691782981,
                                     0.00221295096093323,
                                     0.00202564557293896,
                                     0.00121713364976691,
                                     0.00201010353010846,
                                     0.0013792662877962,
                                     0.000939159886662383,
                                     0.00236324996380834,
                                     0.000591637047498953,
                                     0.00129971312938724,
                                     0.00123513854666147,
                                     0.000253688028701581,
                                     0.00106666135023115,
                                     0.00192154758600285,
                                     0.000752839108677581,
                                     0.000767685990883037,
                                     0.00121620384492679,
                                     0.000297368703579996,
                                     0.00194635223497869,
                                     0.000972309603220783,
                                     0.00142038010848453,
                                     0.00120435642625205,
                                     0.000593827826962806,
                                     0.00101745254335459,
                                     0.00201298600221286,
                                     0.00124506608426804,
                                     0.000606940995308105,
                                     0.000319580983619671,
                                     0.00127760691389441,
                                     0.00124824991129106,
                                     0.00226709706114838,
                                     0.00217071269629057,
                                     0.000654444400598295,
                                     0.0013884466692619,
                                     0.000295986357077491,
                                     0.00100736540267477,
                                     0.00159235588235082,
                                     0.000911758342788089,
                                     0.00183249850905267,
                                     0.00165701069303788,
                                     0.000812399842205923,
                                     0.00189844515204895,
                                     0.00218796041656984,
                                     0.00102219639651943,
                                     0.00218211205177242,
                                     0.00168653694480425,
                                     0.00102559755309718,
                                     0.000901266633726191,
                                     0.00151460901992861,
                                     0.000968090076572262};
  std::vector<double> v{init};
  StatTestResults r(v);

  CHECK(r.sum_ == Approx(0.06475145));
  CHECK(r.mean_ == Approx(0.001295029));
  CHECK(r.median_ == Approx(0.001240102));
  CHECK(r.median_of_sorted_ == Approx(0.001240102));
  CHECK(r.median_destructive_ == Approx(0.001240102));
  CHECK(r.percentile30_ == Approx(0.0009710437));
  CHECK(r.percentile60_ == Approx(0.001367381));
  CHECK(r.percentile90_ == Approx(0.002171853));
  CHECK(r.percentile99_ == Approx(0.002316135));
  CHECK(r.variance_ == Approx(3.367113e-07));
  CHECK(r.std_dev_ == Approx(0.0005802683));
  CHECK(r.median_abs_dev_ == Approx(0.00062611));
  CHECK(r.quartiles_.q1() == Approx(0.0009186087));
  CHECK(r.quartiles_.q2() == Approx(0.001240102));
  CHECK(r.quartiles_.q3() == Approx(0.001796008));
  CHECK(r.quartiles_.iqr() == Approx(0.0008773994));
}
