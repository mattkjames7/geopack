#include <cmath>
#include <fstream>
#include <string>
#include <vector>

#include <geopack.h>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace {

constexpr double kAbsTol = 1e-7;
constexpr double kRelTol = 1e-6;

json loadJson(const std::string &path) {
    std::ifstream f(path);
    return json::parse(f);
}

double jnum(const json &v) {
    return v.is_null() ? NAN : v.get<double>();
}

std::vector<double> jvecd(const json &v) {
    std::vector<double> out;
    out.reserve(v.size());
    for (const auto &x : v) out.push_back(jnum(x));
    return out;
}

std::vector<int> jveci(const json &v) {
    std::vector<int> out;
    out.reserve(v.size());
    for (const auto &x : v) out.push_back(x.get<int>());
    return out;
}

void expectNear(double actual, double expected) {
    if (!std::isfinite(expected)) {
        EXPECT_FALSE(std::isfinite(actual));
        return;
    }
    const double lim = kAbsTol + kRelTol * std::fabs(expected);
    EXPECT_LE(std::fabs(actual - expected), lim);
}

TEST(Golden, GetDipoleTiltUT) {
    const auto root = loadJson("../data/getdipoletiltut.json");
    for (const auto &c : root.at("cases")) {
        const int date = c.at("date").get<int>();
        const float ut = c.at("ut").get<float>();
        const double vx = c.at("vx").get<double>();
        const double vy = c.at("vy").get<double>();
        const double vz = c.at("vz").get<double>();
        const double expected = c.at("tilt").get<double>();
        expectNear(GetDipoleTiltUT(date, ut, vx, vy, vz), expected);
    }
}

TEST(Golden, ConvCoords) {
    const auto root = loadJson("../data/convcoords.json");
    for (const auto &c : root.at("cases")) {
        const int n = c.at("n").get<int>();
        auto xin = jvecd(c.at("xin")), yin = jvecd(c.at("yin")), zin = jvecd(c.at("zin"));
        auto date = jveci(c.at("date"));
        auto utd = jvecd(c.at("ut"));
        std::vector<float> ut(n);
        for (int i = 0; i < n; i++) ut[i] = static_cast<float>(utd[i]);
        auto vx = jvecd(c.at("vx")), vy = jvecd(c.at("vy")), vz = jvecd(c.at("vz"));
        auto ex = jvecd(c.at("xout")), ey = jvecd(c.at("yout")), ez = jvecd(c.at("zout"));
        std::vector<double> xout(n), yout(n), zout(n);
        ConvCoords(xin.data(), yin.data(), zin.data(), n, vx.data(), vy.data(), vz.data(),
                   date.data(), ut.data(), xout.data(), yout.data(), zout.data(),
                   c.at("coord_in").get<std::string>().c_str(), c.at("coord_out").get<std::string>().c_str());
        for (int i = 0; i < n; i++) {
            expectNear(xout[i], ex[i]);
            expectNear(yout[i], ey[i]);
            expectNear(zout[i], ez[i]);
        }
    }
}

TEST(Golden, ModelField) {
    const auto root = loadJson("../data/modelfield.json");
    for (const auto &c : root.at("cases")) {
        const int n = c.at("n").get<int>();
        const bool sameTime = c.at("same_time").get<bool>();
        auto xin = jvecd(c.at("xin")), yin = jvecd(c.at("yin")), zin = jvecd(c.at("zin"));
        auto date = jveci(c.at("date"));
        auto utd = jvecd(c.at("ut"));
        std::vector<float> ut(utd.size());
        for (size_t i = 0; i < utd.size(); i++) ut[i] = static_cast<float>(utd[i]);
        auto vx = jvecd(c.at("vx")), vy = jvecd(c.at("vy")), vz = jvecd(c.at("vz"));
        auto iopt = jveci(c.at("iopt"));

        const auto &pmArr = c.at("parmod");
        std::vector<std::vector<double>> pm(pmArr.size(), std::vector<double>(10, 0.0));
        std::vector<double *> pmPtrs(pmArr.size(), nullptr);
        for (size_t i = 0; i < pmArr.size(); i++) {
            for (size_t j = 0; j < pmArr[i].size(); j++) pm[i][j] = pmArr[i][j].get<double>();
            pmPtrs[i] = pm[i].data();
        }

        auto ex = jvecd(c.at("bx")), ey = jvecd(c.at("by")), ez = jvecd(c.at("bz"));
        std::vector<double> bx(n), by(n), bz(n);
        ModelField(n, xin.data(), yin.data(), zin.data(), date.data(), ut.data(), sameTime,
                   c.at("model").get<std::string>().c_str(), iopt.data(), pmPtrs.data(),
                   vx.data(), vy.data(), vz.data(), "GSM", "GSM", false, bx.data(), by.data(), bz.data());
        for (int i = 0; i < n; i++) {
            expectNear(bx[i], ex[i]);
            expectNear(by[i], ey[i]);
            expectNear(bz[i], ez[i]);
        }
    }
}

TEST(Golden, TraceField) {
    const auto root = loadJson("../data/tracefield.json");
    for (const auto &c : root.at("cases")) {
        int n = 1;
        int date[] = {c.at("date").get<int>()};
        float ut[] = {c.at("ut").get<float>()};
        double xin[] = {c.at("x0").get<double>()}, yin[] = {c.at("y0").get<double>()}, zin[] = {c.at("z0").get<double>()};
        int iopt[] = {c.at("iopt").get<int>()};
        double vx[] = {-359.0}, vy[] = {11.0}, vz[] = {-17.4};
        auto pm = jvecd(c.at("parmod"));
        double *parmod[] = {pm.data()};

        int maxLen = 256, nstep = 0;
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

        TraceField(n, xin, yin, zin, date, ut, c.at("model").get<std::string>().c_str(), iopt, parmod, vx, vy,
                   vz, 100.0, maxLen, 1.0, false, 0, "GSM", &nstep, &xgsm, &ygsm, &zgsm,
                   &bxgsm, &bygsm, &bzgsm, &xgse, &ygse, &zgse, &bxgse, &bygse, &bzgse, &xsm,
                   &ysm, &zsm, &bxsm, &bysm, &bzsm, &s, &r, &rnorm, &fp, nalpha, alpha, halpha);

        EXPECT_EQ(nstep, c.at("nstep").get<int>());
        auto exgsm = jvecd(c.at("xgsm")), eygsm = jvecd(c.at("ygsm")), ezgsm = jvecd(c.at("zgsm")), es = jvecd(c.at("s"));
        for (int i = 0; i < nstep; i++) {
            expectNear(xgsm[i], exgsm[i]);
            expectNear(ygsm[i], eygsm[i]);
            expectNear(zgsm[i], ezgsm[i]);
            expectNear(s[i], es[i]);
        }

        free(xgsm); free(ygsm); free(zgsm); free(bxgsm); free(bygsm); free(bzgsm);
        free(xgse); free(ygse); free(zgse); free(bxgse); free(bygse); free(bzgse);
        free(xsm); free(ysm); free(zsm); free(bxsm); free(bysm); free(bzsm);
        free(s); free(r); free(rnorm); free(fp);
    }
}

}  // namespace
