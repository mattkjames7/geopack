#include <cmath>
#include <string>
#include <vector>

#include <geopack.h>
#include <gtest/gtest.h>

#include "minijson.h"

namespace {

constexpr double kAbsTol = 1e-7;
constexpr double kRelTol = 1e-6;

void expectNear(double actual, double expected) {
    if (!std::isfinite(expected)) {
        EXPECT_FALSE(std::isfinite(actual));
        return;
    }
    const double lim = kAbsTol + kRelTol * std::fabs(expected);
    EXPECT_LE(std::fabs(actual - expected), lim);
}

double num(const minijson::Object &o, const std::string &k) { return o.at(k).asNumber(); }
std::string str(const minijson::Object &o, const std::string &k) { return o.at(k).asString(); }

std::vector<double> vecnum(const minijson::Object &o, const std::string &k) {
    std::vector<double> out;
    for (const auto &v : o.at(k).asArray()) out.push_back(v.isNull() ? NAN : v.asNumber());
    return out;
}

std::vector<int> vecint(const minijson::Object &o, const std::string &k) {
    std::vector<int> out;
    for (const auto &v : o.at(k).asArray()) out.push_back(static_cast<int>(v.asNumber()));
    return out;
}

TEST(Golden, GetDipoleTiltUT) {
    const auto root = minijson::parseFile("../data/getdipoletiltut.json").asObject();
    for (const auto &c : root.at("cases").asArray()) {
        const auto &o = c.asObject();
        const int date = static_cast<int>(num(o, "date"));
        const float ut = static_cast<float>(num(o, "ut"));
        const double vx = num(o, "vx");
        const double vy = num(o, "vy");
        const double vz = num(o, "vz");
        const double expected = num(o, "tilt");
        const double actual = GetDipoleTiltUT(date, ut, vx, vy, vz);
        expectNear(actual, expected);
    }
}

TEST(Golden, ConvCoords) {
    const auto root = minijson::parseFile("../data/convcoords.json").asObject();
    for (const auto &c : root.at("cases").asArray()) {
        const auto &o = c.asObject();
        const int n = static_cast<int>(num(o, "n"));
        auto xin = vecnum(o, "xin");
        auto yin = vecnum(o, "yin");
        auto zin = vecnum(o, "zin");
        auto datei = vecint(o, "date");
        auto utf = vecnum(o, "ut");
        auto vx = vecnum(o, "vx");
        auto vy = vecnum(o, "vy");
        auto vz = vecnum(o, "vz");
        auto ex = vecnum(o, "xout");
        auto ey = vecnum(o, "yout");
        auto ez = vecnum(o, "zout");
        std::vector<float> ut(n);
        for (int i = 0; i < n; i++) ut[i] = static_cast<float>(utf[i]);
        std::vector<double> xout(n), yout(n), zout(n);
        ConvCoords(xin.data(), yin.data(), zin.data(), n, vx.data(), vy.data(), vz.data(),
                   datei.data(), ut.data(), xout.data(), yout.data(), zout.data(),
                   str(o, "coord_in").c_str(), str(o, "coord_out").c_str());
        for (int i = 0; i < n; i++) {
            expectNear(xout[i], ex[i]);
            expectNear(yout[i], ey[i]);
            expectNear(zout[i], ez[i]);
        }
    }
}

TEST(Golden, ModelField) {
    const auto root = minijson::parseFile("../data/modelfield.json").asObject();
    for (const auto &c : root.at("cases").asArray()) {
        const auto &o = c.asObject();
        const int n = static_cast<int>(num(o, "n"));
        const bool sameTime = o.at("same_time").asNumber() != 0.0;
        auto xin = vecnum(o, "xin");
        auto yin = vecnum(o, "yin");
        auto zin = vecnum(o, "zin");
        auto date = vecint(o, "date");
        auto utd = vecnum(o, "ut");
        auto vx = vecnum(o, "vx");
        auto vy = vecnum(o, "vy");
        auto vz = vecnum(o, "vz");
        auto iopt = vecint(o, "iopt");
        std::vector<float> ut(utd.size());
        for (size_t i = 0; i < utd.size(); i++) ut[i] = static_cast<float>(utd[i]);
        const auto &pmArr = o.at("parmod").asArray();
        std::vector<std::vector<double>> pm(pmArr.size(), std::vector<double>(10, 0.0));
        std::vector<double *> pmPtrs(pmArr.size(), nullptr);
        for (size_t i = 0; i < pmArr.size(); i++) {
            for (size_t j = 0; j < pmArr[i].asArray().size(); j++) {
                pm[i][j] = pmArr[i].asArray()[j].asNumber();
            }
            pmPtrs[i] = pm[i].data();
        }
        auto ex = vecnum(o, "bx");
        auto ey = vecnum(o, "by");
        auto ez = vecnum(o, "bz");
        std::vector<double> bx(n), by(n), bz(n);
        ModelField(n, xin.data(), yin.data(), zin.data(), date.data(), ut.data(), sameTime,
                   str(o, "model").c_str(), iopt.data(), pmPtrs.data(), vx.data(), vy.data(),
                   vz.data(), "GSM", "GSM", false, bx.data(), by.data(), bz.data());
        for (int i = 0; i < n; i++) {
            expectNear(bx[i], ex[i]);
            expectNear(by[i], ey[i]);
            expectNear(bz[i], ez[i]);
        }
    }
}

TEST(Golden, TraceField) {
    const auto root = minijson::parseFile("../data/tracefield.json").asObject();
    for (const auto &c : root.at("cases").asArray()) {
        const auto &o = c.asObject();
        int n = 1;
        int date[] = {static_cast<int>(num(o, "date"))};
        float ut[] = {static_cast<float>(num(o, "ut"))};
        double xin[] = {num(o, "x0")};
        double yin[] = {num(o, "y0")};
        double zin[] = {num(o, "z0")};
        int iopt[] = {static_cast<int>(num(o, "iopt"))};
        double vx[] = {-359.0};
        double vy[] = {11.0};
        double vz[] = {-17.4};
        auto pm = vecnum(o, "parmod");
        double *parmod[] = {pm.data()};
        int maxLen = 256;
        int nstep = 0;
        double *xgsm = static_cast<double *>(malloc(maxLen * sizeof(double)));
        double *ygsm = static_cast<double *>(malloc(maxLen * sizeof(double)));
        double *zgsm = static_cast<double *>(malloc(maxLen * sizeof(double)));
        double *bxgsm = static_cast<double *>(malloc(maxLen * sizeof(double)));
        double *bygsm = static_cast<double *>(malloc(maxLen * sizeof(double)));
        double *bzgsm = static_cast<double *>(malloc(maxLen * sizeof(double)));
        double *xgse = static_cast<double *>(malloc(maxLen * sizeof(double)));
        double *ygse = static_cast<double *>(malloc(maxLen * sizeof(double)));
        double *zgse = static_cast<double *>(malloc(maxLen * sizeof(double)));
        double *bxgse = static_cast<double *>(malloc(maxLen * sizeof(double)));
        double *bygse = static_cast<double *>(malloc(maxLen * sizeof(double)));
        double *bzgse = static_cast<double *>(malloc(maxLen * sizeof(double)));
        double *xsm = static_cast<double *>(malloc(maxLen * sizeof(double)));
        double *ysm = static_cast<double *>(malloc(maxLen * sizeof(double)));
        double *zsm = static_cast<double *>(malloc(maxLen * sizeof(double)));
        double *bxsm = static_cast<double *>(malloc(maxLen * sizeof(double)));
        double *bysm = static_cast<double *>(malloc(maxLen * sizeof(double)));
        double *bzsm = static_cast<double *>(malloc(maxLen * sizeof(double)));
        double *s = static_cast<double *>(malloc(maxLen * sizeof(double)));
        double *r = static_cast<double *>(malloc(maxLen * sizeof(double)));
        double *rnorm = static_cast<double *>(malloc(maxLen * sizeof(double)));
        double *fp = static_cast<double *>(malloc(maxLen * sizeof(double)));
        int nalpha = 2;
        double alpha[] = {0.0, 90.0};
        double halpha[512] = {0.0};

        TraceField(n, xin, yin, zin, date, ut, str(o, "model").c_str(), iopt, parmod, vx, vy,
                   vz, 100.0, maxLen, 1.0, false, 0, "GSM", &nstep, &xgsm, &ygsm, &zgsm,
                   &bxgsm, &bygsm, &bzgsm, &xgse, &ygse, &zgse, &bxgse, &bygse, &bzgse, &xsm,
                   &ysm, &zsm, &bxsm, &bysm, &bzsm, &s, &r, &rnorm, &fp, nalpha, alpha, halpha);

        EXPECT_EQ(nstep, static_cast<int>(num(o, "nstep")));
        auto exgsm = vecnum(o, "xgsm");
        auto eygsm = vecnum(o, "ygsm");
        auto ezgsm = vecnum(o, "zgsm");
        auto es = vecnum(o, "s");
        for (int i = 0; i < nstep; i++) {
            expectNear(xgsm[i], exgsm[i]);
            expectNear(ygsm[i], eygsm[i]);
            expectNear(zgsm[i], ezgsm[i]);
            expectNear(s[i], es[i]);
        }

        free(xgsm); free(ygsm); free(zgsm);
        free(bxgsm); free(bygsm); free(bzgsm);
        free(xgse); free(ygse); free(zgse);
        free(bxgse); free(bygse); free(bzgse);
        free(xsm); free(ysm); free(zsm);
        free(bxsm); free(bysm); free(bzsm);
        free(s); free(r); free(rnorm); free(fp);
    }
}

}  // namespace
