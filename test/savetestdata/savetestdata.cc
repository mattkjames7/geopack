#include <cmath>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include <geopack.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace {

json jnum(double v) {
    return std::isfinite(v) ? json(v) : json(nullptr);
}

void writeJson(const std::string &path, const json &j) {
    std::ofstream f(path);
    f << j.dump(2) << "\n";
}

void saveDipoleTilt(const std::string &outdir) {
    struct Case { int date; float ut; double vx; double vy; double vz; };
    const std::vector<Case> cases = {
        {20120101, 0.0f, -400.0, 0.0, 0.0},
        {20120101, 12.0f, -359.0, 11.0, -17.4},
        {20160320, 6.5f, -420.0, 5.0, -10.0},
    };

    json root;
    root["api"] = "GetDipoleTiltUT";
    root["cases"] = json::array();
    for (const auto &c : cases) {
        root["cases"].push_back({
            {"date", c.date},
            {"ut", c.ut},
            {"vx", c.vx},
            {"vy", c.vy},
            {"vz", c.vz},
            {"tilt", GetDipoleTiltUT(c.date, c.ut, c.vx, c.vy, c.vz)},
        });
    }
    writeJson(outdir + "/getdipoletiltut.json", root);
}

void saveConvCoords(const std::string &outdir) {
    const int n = 3;
    double xin[n] = {1.0, -2.0, 3.5};
    double yin[n] = {0.0, 1.0, -0.2};
    double zin[n] = {2.0, -1.0, 0.3};
    int date[n] = {20120101, 20120101, 20160320};
    float ut[n] = {12.0f, 12.0f, 6.5f};
    double vx[n] = {-359.0, -359.0, -420.0};
    double vy[n] = {11.0, 11.0, 5.0};
    double vz[n] = {-17.4, -17.4, -10.0};
    struct Pair { const char *in; const char *out; };
    const std::vector<Pair> pairs = {{"GSM", "GSE"}, {"GSE", "SM"}, {"GEO", "MAG"}};

    json root;
    root["api"] = "ConvCoords";
    root["cases"] = json::array();
    for (const auto &p : pairs) {
        double xout[n] = {0.0, 0.0, 0.0};
        double yout[n] = {0.0, 0.0, 0.0};
        double zout[n] = {0.0, 0.0, 0.0};
        ConvCoords(xin, yin, zin, n, vx, vy, vz, date, ut, xout, yout, zout, p.in, p.out);
        root["cases"].push_back({
            {"coord_in", p.in}, {"coord_out", p.out}, {"n", n},
            {"xin", {xin[0], xin[1], xin[2]}}, {"yin", {yin[0], yin[1], yin[2]}},
            {"zin", {zin[0], zin[1], zin[2]}}, {"date", {date[0], date[1], date[2]}},
            {"ut", {ut[0], ut[1], ut[2]}}, {"vx", {vx[0], vx[1], vx[2]}},
            {"vy", {vy[0], vy[1], vy[2]}}, {"vz", {vz[0], vz[1], vz[2]}},
            {"xout", {xout[0], xout[1], xout[2]}}, {"yout", {yout[0], yout[1], yout[2]}},
            {"zout", {zout[0], zout[1], zout[2]}},
        });
    }
    writeJson(outdir + "/convcoords.json", root);
}

void saveModelField(const std::string &outdir) {
    const int n = 2;
    const char *models[] = {"T89", "T96", "T01", "TS05"};
    int date[n] = {20120101, 20160320};
    float ut[n] = {12.0f, 6.5f};
    double xin[n] = {0.0, 2.0};
    double yin[n] = {10.0, -3.0};
    double zin[n] = {0.0, 1.2};
    int iopt[n] = {2, 2};
    double vx[n] = {-359.0, -420.0};
    double vy[n] = {11.0, 5.0};
    double vz[n] = {-17.4, -10.0};

    json root;
    root["api"] = "ModelField";
    root["cases"] = json::array();

    for (int m = 0; m < 4; m++) {
        double row0[10] = {2.5, -15.0, 1.0, -2.0, 0.3, -0.2, 0.1, 0.0, 0.0, 0.0};
        double row1[10] = {3.0, -20.0, 0.8, -1.2, 0.2, -0.1, 0.2, 0.1, 0.0, -0.1};
        double *parmod[n] = {row0, row1};
        double bx[n] = {0.0, 0.0}, by[n] = {0.0, 0.0}, bz[n] = {0.0, 0.0};
        ModelField(n, xin, yin, zin, date, ut, false, models[m], iopt, parmod, vx, vy, vz, "GSM", "GSM", false, bx, by, bz);
        root["cases"].push_back({
            {"model", models[m]}, {"same_time", false}, {"n", n},
            {"xin", {xin[0], xin[1]}}, {"yin", {yin[0], yin[1]}}, {"zin", {zin[0], zin[1]}},
            {"date", {date[0], date[1]}}, {"ut", {ut[0], ut[1]}},
            {"vx", {vx[0], vx[1]}}, {"vy", {vy[0], vy[1]}}, {"vz", {vz[0], vz[1]}},
            {"iopt", {iopt[0], iopt[1]}},
            {"parmod", {{row0[0], row0[1], row0[2], row0[3], row0[4], row0[5], row0[6], row0[7], row0[8], row0[9]},
                        {row1[0], row1[1], row1[2], row1[3], row1[4], row1[5], row1[6], row1[7], row1[8], row1[9]}}},
            {"bx", {bx[0], bx[1]}}, {"by", {by[0], by[1]}}, {"bz", {bz[0], bz[1]}},
        });
    }

    {
        const int n1 = 2;
        int date1[1] = {20120101};
        float ut1[1] = {12.0f};
        double xin1[n1] = {0.0, 1.5}, yin1[n1] = {10.0, -4.0}, zin1[n1] = {0.0, 2.0};
        int iopt1[1] = {2};
        double vx1[1] = {-359.0}, vy1[1] = {11.0}, vz1[1] = {-17.4};
        double row[10] = {2.5, -15.0, 1.0, -2.0, 0.3, -0.2, 0.1, 0.0, 0.0, 0.0};
        double *parmod1[1] = {row};
        double bx[n1] = {0.0, 0.0}, by[n1] = {0.0, 0.0}, bz[n1] = {0.0, 0.0};
        ModelField(n1, xin1, yin1, zin1, date1, ut1, true, "T96", iopt1, parmod1, vx1, vy1, vz1, "GSM", "GSM", false, bx, by, bz);
        root["cases"].push_back({
            {"model", "T96"}, {"same_time", true}, {"n", n1},
            {"xin", {xin1[0], xin1[1]}}, {"yin", {yin1[0], yin1[1]}}, {"zin", {zin1[0], zin1[1]}},
            {"date", {date1[0]}}, {"ut", {ut1[0]}},
            {"vx", {vx1[0]}}, {"vy", {vy1[0]}}, {"vz", {vz1[0]}},
            {"iopt", {iopt1[0]}},
            {"parmod", {{row[0], row[1], row[2], row[3], row[4], row[5], row[6], row[7], row[8], row[9]}}},
            {"bx", {bx[0], bx[1]}}, {"by", {by[0], by[1]}}, {"bz", {bz[0], bz[1]}},
        });
    }

    writeJson(outdir + "/modelfield.json", root);
}

json pushArray(double *arr, int n) {
    json a = json::array();
    for (int i = 0; i < n; i++) a.push_back(jnum(arr[i]));
    return a;
}

void saveTraceField(const std::string &outdir) {
    const char *models[] = {"T89", "T96", "T01", "TS05"};
    json root;
    root["api"] = "TraceField";
    root["cases"] = json::array();

    for (int m = 0; m < 4; m++) {
        int n = 1;
        int date[] = {20120101};
        float ut[] = {12.0f};
        double xin[] = {0.0}, yin[] = {8.0}, zin[] = {0.1};
        int iopt[] = {2};
        double vx[] = {-359.0}, vy[] = {11.0}, vz[] = {-17.4};
        double row[10] = {2.5, -15.0, 1.0, -2.0, 0.3, -0.2, 0.1, 0.05, 0.0, -0.03};
        double *parmod[] = {row};

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

        TraceField(n, xin, yin, zin, date, ut, models[m], iopt, parmod, vx, vy, vz,
                   100.0, maxLen, 1.0, false, 0, "GSM", &nstep, &xgsm, &ygsm, &zgsm,
                   &bxgsm, &bygsm, &bzgsm, &xgse, &ygse, &zgse, &bxgse, &bygse, &bzgse,
                   &xsm, &ysm, &zsm, &bxsm, &bysm, &bzsm, &s, &r, &rnorm, &fp, nalpha, alpha, halpha);

        root["cases"].push_back({
            {"model", models[m]}, {"nstep", nstep}, {"date", date[0]}, {"ut", ut[0]},
            {"x0", xin[0]}, {"y0", yin[0]}, {"z0", zin[0]}, {"iopt", iopt[0]},
            {"parmod", {row[0], row[1], row[2], row[3], row[4], row[5], row[6], row[7], row[8], row[9]}},
            {"xgsm", pushArray(xgsm, nstep)}, {"ygsm", pushArray(ygsm, nstep)}, {"zgsm", pushArray(zgsm, nstep)},
            {"bxgsm", pushArray(bxgsm, nstep)}, {"bygsm", pushArray(bygsm, nstep)}, {"bzgsm", pushArray(bzgsm, nstep)},
            {"xgse", pushArray(xgse, nstep)}, {"ygse", pushArray(ygse, nstep)}, {"zgse", pushArray(zgse, nstep)},
            {"xsm", pushArray(xsm, nstep)}, {"ysm", pushArray(ysm, nstep)}, {"zsm", pushArray(zsm, nstep)},
            {"s", pushArray(s, nstep)}, {"r", pushArray(r, nstep)}, {"rnorm", pushArray(rnorm, nstep)}, {"fp", pushArray(fp, nstep)},
        });

        free(xgsm); free(ygsm); free(zgsm); free(bxgsm); free(bygsm); free(bzgsm);
        free(xgse); free(ygse); free(zgse); free(bxgse); free(bygse); free(bzgse);
        free(xsm); free(ysm); free(zsm); free(bxsm); free(bysm); free(bzsm);
        free(s); free(r); free(rnorm); free(fp);
    }

    writeJson(outdir + "/tracefield.json", root);
}

}  // namespace

int main() {
    std::filesystem::create_directories("../data");
    const std::string outdir = "../data";
    saveDipoleTilt(outdir);
    saveConvCoords(outdir);
    saveModelField(outdir);
    saveTraceField(outdir);
    return 0;
}
