#include <cmath>
#include <fstream>
#include <map>
#include <string>
#include <vector>

#include <geopack.h>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace {

constexpr double kAbsTol = 1e-7;
constexpr double kRelTol = 1e-6;
using Scalar3Fn = void (*)(double, double, double, double, double, double, int, int, int, int, int, int, double *, double *, double *);
using UT3Fn = void (*)(double *, double *, double *, int, double *, double *, double *, int *, float *, double *, double *, double *);
using Scalar2Fn = void (*)(double, double, double, double, double, int, int, int, int, int, int, double *, double *);
using UT2Fn = void (*)(double *, double *, int, double *, double *, double *, int *, float *, double *, double *);

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

Scalar3Fn scalarConvFn(const std::string &name) {
    static const std::map<std::string, Scalar3Fn> kFn = {
        {"GSEtoGSM", GSEtoGSM}, {"GSMtoGSE", GSMtoGSE},
        {"GSMtoSM", GSMtoSM}, {"SMtoGSM", SMtoGSM},
        {"GSEtoSM", GSEtoSM}, {"SMtoGSE", SMtoGSE},
        {"GSEtoMAG", GSEtoMAG}, {"MAGtoGSE", MAGtoGSE},
        {"GEOtoMAG", GEOtoMAG}, {"MAGtoGEO", MAGtoGEO},
        {"GEItoGEO", GEItoGEO}, {"GEOtoGEI", GEOtoGEI},
        {"GSMtoGEO", GSMtoGEO}, {"GEOtoGSM", GEOtoGSM},
        {"GSEtoGEO", GSEtoGEO}, {"GEOtoGSE", GEOtoGSE},
        {"SMtoGEO", SMtoGEO}, {"GEOtoSM", GEOtoSM},
        {"GSEtoGEI", GSEtoGEI}, {"GEItoGSE", GEItoGSE},
        {"GSMtoGEI", GSMtoGEI}, {"GEItoGSM", GEItoGSM},
        {"SMtoGEI", SMtoGEI}, {"GEItoSM", GEItoSM},
        {"MAGtoGEI", MAGtoGEI}, {"GEItoMAG", GEItoMAG},
        {"MAGtoGSM", MAGtoGSM}, {"GSMtoMAG", GSMtoMAG},
        {"MAGtoSM", MAGtoSM}, {"SMtoMAG", SMtoMAG},
    };
    const auto it = kFn.find(name);
    return (it == kFn.end()) ? nullptr : it->second;
}

UT3Fn utConvFn(const std::string &name) {
    static const std::map<std::string, UT3Fn> kFn = {
        {"GSEtoGSM", GSEtoGSMUT}, {"GSMtoGSE", GSMtoGSEUT},
        {"GSMtoSM", GSMtoSMUT}, {"SMtoGSM", SMtoGSMUT},
        {"GSEtoSM", GSEtoSMUT}, {"SMtoGSE", SMtoGSEUT},
        {"GSEtoMAG", GSEtoMAGUT}, {"MAGtoGSE", MAGtoGSEUT},
        {"GEOtoMAG", GEOtoMAGUT}, {"MAGtoGEO", MAGtoGEOUT},
        {"GEItoGEO", GEItoGEOUT}, {"GEOtoGEI", GEOtoGEIUT},
        {"GSMtoGEO", GSMtoGEOUT}, {"GEOtoGSM", GEOtoGSMUT},
        {"GSEtoGEO", GSEtoGEOUT}, {"GEOtoGSE", GEOtoGSEUT},
        {"SMtoGEO", SMtoGEOUT}, {"GEOtoSM", GEOtoSMUT},
        {"GSEtoGEI", GSEtoGEIUT}, {"GEItoGSE", GEItoGSEUT},
        {"GSMtoGEI", GSMtoGEIUT}, {"GEItoGSM", GEItoGSMUT},
        {"SMtoGEI", SMtoGEIUT}, {"GEItoSM", GEItoSMUT},
        {"MAGtoGEI", MAGtoGEIUT}, {"GEItoMAG", GEItoMAGUT},
        {"MAGtoGSM", MAGtoGSMUT}, {"GSMtoMAG", GSMtoMAGUT},
        {"MAGtoSM", MAGtoSMUT}, {"SMtoMAG", SMtoMAGUT},
    };
    const auto it = kFn.find(name);
    return (it == kFn.end()) ? nullptr : it->second;
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
        auto exgsm = jvecd(c.at("xgsm")), eygsm = jvecd(c.at("ygsm")), ezgsm = jvecd(c.at("zgsm"));
        auto es = jvecd(c.at("s")), efp = jvecd(c.at("fp"));
        for (int i = 0; i < nstep; i++) {
            expectNear(xgsm[i], exgsm[i]);
            expectNear(ygsm[i], eygsm[i]);
            expectNear(zgsm[i], ezgsm[i]);
            expectNear(s[i], es[i]);
        }
        ASSERT_GE(efp.size(), 15u);
        for (int i = 0; i < 15; i++) {
            expectNear(fp[i], efp[i]);
        }

        free(xgsm); free(ygsm); free(zgsm); free(bxgsm); free(bygsm); free(bzgsm);
        free(xgse); free(ygse); free(zgse); free(bxgse); free(bygse); free(bzgse);
        free(xsm); free(ysm); free(zsm); free(bxsm); free(bysm); free(bzsm);
        free(s); free(r); free(rnorm); free(fp);
    }
}

TEST(Golden, OtherCWrappers) {
    const auto root = loadJson("../data/c_wrappers.json");
    for (const auto &c : root.at("conv_scalar")) {
        const auto fn = scalarConvFn(c.at("name").get<std::string>());
        ASSERT_NE(fn, nullptr);
        double xo = 0.0, yo = 0.0, zo = 0.0;
        fn(c.at("xin").get<double>(), c.at("yin").get<double>(), c.at("zin").get<double>(),
           -359.0, 11.0, -17.4, 1, 2012, 1, 12, 0, 0, &xo, &yo, &zo);
        expectNear(xo, jnum(c.at("xout"))); expectNear(yo, jnum(c.at("yout"))); expectNear(zo, jnum(c.at("zout")));
    }
    for (const auto &c : root.at("conv_ut")) {
        const auto fn = utConvFn(c.at("name").get<std::string>());
        ASSERT_NE(fn, nullptr);
        const int n = c.at("n").get<int>();
        auto xin = jvecd(c.at("xin")), yin = jvecd(c.at("yin")), zin = jvecd(c.at("zin"));
        auto vx = jvecd(c.at("vx")), vy = jvecd(c.at("vy")), vz = jvecd(c.at("vz"));
        auto date = jveci(c.at("date"));
        auto ut_d = jvecd(c.at("ut"));
        std::vector<float> ut(n);
        for (int i = 0; i < n; i++) ut[i] = static_cast<float>(ut_d[i]);
        std::vector<double> xo(n), yo(n), zo(n);
        fn(xin.data(), yin.data(), zin.data(), n, vx.data(), vy.data(), vz.data(), date.data(), ut.data(), xo.data(), yo.data(), zo.data());
        auto ex = jvecd(c.at("xout")), ey = jvecd(c.at("yout")), ez = jvecd(c.at("zout"));
        for (int i = 0; i < n; i++) { expectNear(xo[i], ex[i]); expectNear(yo[i], ey[i]); expectNear(zo[i], ez[i]); }
    }

    for (const auto &c : root.at("ll_scalar")) {
        const std::string name = c.at("name").get<std::string>();
        Scalar2Fn fn = (name == "GEOtoMAG_LL") ? GEOtoMAG_LL : MAGtoGEO_LL;
        double ao = 0.0, bo = 0.0;
        fn(c.at("a").get<double>(), c.at("b").get<double>(), -359.0, 11.0, -17.4, 1, 2012, 1, 12, 0, 0, &ao, &bo);
        expectNear(ao, jnum(c.at("aout"))); expectNear(bo, jnum(c.at("bout")));
    }
    for (const auto &c : root.at("ll_ut")) {
        const std::string name = c.at("name").get<std::string>();
        UT2Fn fn = (name == "GEOtoMAG_LL") ? GEOtoMAGUT_LL : MAGtoGEOUT_LL;
        const int n = c.at("n").get<int>();
        auto a = jvecd(c.at("a")), b = jvecd(c.at("b"));
        auto vx = jvecd(c.at("vx")), vy = jvecd(c.at("vy")), vz = jvecd(c.at("vz"));
        auto date = jveci(c.at("date"));
        auto ut_d = jvecd(c.at("ut"));
        std::vector<float> ut(n);
        for (int i = 0; i < n; i++) ut[i] = static_cast<float>(ut_d[i]);
        std::vector<double> ao(n), bo(n);
        fn(a.data(), b.data(), n, vx.data(), vy.data(), vz.data(), date.data(), ut.data(), ao.data(), bo.data());
        auto ex = jvecd(c.at("aout")), ey = jvecd(c.at("bout"));
        for (int i = 0; i < n; i++) { expectNear(ao[i], ex[i]); expectNear(bo[i], ey[i]); }
    }

    {
        const auto &c0 = root.at("mlt_scalar")[0];
        double out = 0.0;
        MLONtoMLT(c0.at("in").get<double>(), -359.0, 11.0, -17.4, 1, 2012, 1, 12, 0, 0, &out);
        expectNear(out, jnum(c0.at("out")));
        const auto &c1 = root.at("mlt_scalar")[1];
        double out2 = 0.0;
        MLTtoMLON(c1.at("in").get<double>(), -359.0, 11.0, -17.4, 1, 2012, 1, 12, 0, 0, &out2);
        expectNear(out2, jnum(c1.at("out")));
    }
    for (const auto &c : root.at("mlt_ut")) {
        const int n = c.at("n").get<int>();
        auto in = jvecd(c.at("in"));
        auto vx = jvecd(c.at("vx")), vy = jvecd(c.at("vy")), vz = jvecd(c.at("vz"));
        auto date = jveci(c.at("date"));
        auto ut_d = jvecd(c.at("ut"));
        std::vector<float> ut(n);
        for (int i = 0; i < n; i++) ut[i] = static_cast<float>(ut_d[i]);
        std::vector<double> out(n);
        if (c.at("name").get<std::string>() == "MLONtoMLTUT") {
            MLONtoMLTUT(in.data(), n, vx.data(), vy.data(), vz.data(), date.data(), ut.data(), out.data());
        } else {
            MLTtoMLONUT(in.data(), n, vx.data(), vy.data(), vz.data(), date.data(), ut.data(), out.data());
        }
        auto ex = jvecd(c.at("out"));
        for (int i = 0; i < n; i++) expectNear(out[i], ex[i]);
    }

    for (const auto &c : root.at("withinmp")) {
        const bool out = WithinMP(c.at("x").get<double>(), c.at("y").get<double>(), c.at("z").get<double>(),
                                  c.at("bz").get<double>(), c.at("pdyn").get<double>());
        EXPECT_EQ(out, c.at("out").get<bool>());
    }
}

}  // namespace
